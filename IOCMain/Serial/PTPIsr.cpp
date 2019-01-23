// PTPIsr.cpp: implementation of the CPTPIsr class.
// This ISR support Point-To-Point protocol on SCI3 & SCI4
// It should only run after OS(CMX) is running
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPIsr.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: PTPIsr.cpp 1.3 2009/05/01 09:54:57PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#include "CodeOpt.h"
#include "ConstDef.h"
#include "DataType.h"
#include "string.h"
#include "PTPIsr.h"
#include "machine.h"
#include "cxfuncs.h"
#include "PTPLink.h"
#include "DtcSci.h"
#include "DtcSciSend.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPTPIsr *g_pSci3;
CPTPIsr *g_pSci4;

CPTPIsr::CPTPIsr(const char *pName, SCI_REG reg, long rate, unsigned int DTCVector, unsigned int sendVector) : m_frameEnds(20)
{
	rx_buffer[0] = 0;
	tx_buffer[0] = 0;

#if DEBUG_PHYSICAL
	m_debugIndex = 0;
	memset(m_debugBuffer, 0, sizeof(m_debugBuffer));
#endif
	strncpy(m_name, pName, 20);
	m_name[20] = 0;

	m_sci = reg; 
	m_baudrate = rate;
	m_pLink = NULL;

	m_pDTC = NULL;
	if (DTCVector) {
		int bValid = CDtcSci::CheckVector(DTCVector);
		if (bValid == 1) {
			m_pDTC = new CDtcSci(DTCVector);
		}
	}

	m_pSendDtc = NULL;
	if (sendVector) {
		int bValid = CDtcSci::CheckVector(sendVector);
		if (bValid == 1) {
			m_pSendDtc = new CDtcSciSend(reg, sendVector);
		}
	}
	init();
}

CPTPIsr::~CPTPIsr()
{

}

void CPTPIsr::StartSend()
{
	uint8 tmp_exr; 
	
	if (m_pSendDtc != NULL) {
		m_pSendDtc->flush();
		return;
	}
	tmp_exr	= get_exr();
	set_exr(7);
	if (!(m_sci->SCR.BIT.TIE)) {
		isr_txd();
		m_sci->SCR.BIT.TIE = 1;
	}
	set_exr(tmp_exr);
}

const char flag_escape[2] = { CPTPIsr::ESCAPE, CPTPIsr::FLAG2};
const char escape_escape[2] = { CPTPIsr::ESCAPE, CPTPIsr::ESCAPE2 };

void CPTPIsr::AppendByteDtc(char ch, int8 datatype)
{
	if (datatype == 0) {
		// Check for special char
		switch (ch) {
		case FLAG:
			m_pSendDtc->Send(flag_escape, 2);
			return;
		case ESCAPE:
			m_pSendDtc->Send(escape_escape, 2);
			return;
		}
	}

	m_pSendDtc->Send(&ch, 1);
		

}

int CPTPIsr::AppendByte(char ch)
{
	int next;

	next = (tx_tail >= (TX_SIZE -1)) ? (0) : (tx_tail + 1);
	while (next == tx_head) {
		// Tx buffer full
		StartSend();
		// Pause for a tick
		K_Task_Wait(1);
	}
	tx_buffer[tx_tail] = ch;
	return next;
}

int CPTPIsr::SendFrameDtc(const char *str, int len, const char *ctrl)
{
	int i;
	char ch;
	uint16 crc16;
	const char *ptr = str;

	// Add the byte into buffer
	AppendByteDtc(FLAG, 1);

	if (ctrl != NULL) {
		// Insert two control byte
		for (i = 0; i < 2; i++) {
			ch = *ctrl++;
			AppendByteDtc(ch);
			//tx_tail = next;
			m_sendCRC.UpdateCrc(ch);
		}
#if INSERT_ERROR
		{
			// Fake 10% error
			static int frame_error = 1;
			if (frame_error++ <= 2) {
				// Fake an error
				m_sendCRC.UpdateCrc(0);
			}
			frame_error %= 10;
		}
#endif 
	}

	for (i = 0; i < len; i++) {
		// Add the byte into buffer
		ch = *ptr++;
		AppendByteDtc(ch);
		// accumulate CRC
		m_sendCRC.UpdateCrc(ch);
	}
	// Append the crc
	crc16 = m_sendCRC.GetCrc16();
	ptr = (char *)&crc16;
	for (i = 0; i < 2; i++) {
		AppendByteDtc(*ptr++);
	}
	AppendByteDtc(FLAG, 1);
	// Make sure the sending is started
	StartSend();
	return len;

}

// ptr point to data from CTRL1, CTRL2. 
// FLAGs CRC16 shouldn't be included.
// Only one process can call SendFrame().
int CPTPIsr::SendFrame(const char *str, int len, const char *ctrl)
{
	int next, i;
	char ch;
	uint16 crc16;
	const char *ptr = str;

	if (len <= 0) return 0;

	m_sendCRC.Reset();

	if (m_pSendDtc) {
		return SendFrameDtc(str, len, ctrl);
	}

	// Add the byte into buffer
	next = AppendByte(FLAG);
	// Mark the beginning of the frame
	m_frameEnds.Append(tx_tail);
	tx_tail = next;

	if (ctrl != NULL) {
		// Insert two control byte
		for (i = 0; i < 2; i++) {
			ch = *ctrl++;
			next = AppendByte(ch);
			tx_tail = next;
			m_sendCRC.UpdateCrc(ch);
		}
#if INSERT_ERROR
		{
			// Fake 10% error
			static int frame_error = 1;
			if (frame_error++ <= 2) {
				// Fake an error
				m_sendCRC.UpdateCrc(0);
			}
			frame_error %= 10;
		}
#endif 
	}

	for (i = 0; i < len; i++) {
		// Add the byte into buffer
		ch = *ptr++;
		next = AppendByte(ch);
		// Validate the byte
		tx_tail = next;
		// accumulate CRC
		m_sendCRC.UpdateCrc(ch);
	}
	// Append the crc
	crc16 = m_sendCRC.GetCrc16();
	ptr = (char *)&crc16;
	for (i = 0; i < 2; i++) {
		next = AppendByte(*ptr++);
		tx_tail = next;
	}
	next = AppendByte(FLAG);
	// Mark the end of the frame
	m_frameEnds.Append(tx_tail);
	// Validate the byte
	tx_tail = next;
	// Make sure the sending is started
	StartSend();
	return len;
}

int CPTPIsr::RtrvFrame(int tail, char *buffer, int max)
{
	int len = 0;
	int head = rx_head;
	int next;
	while (head != tail) {
		if (buffer != NULL)
			*buffer++ = rx_buffer[head];
		head = (head >= (RX_SIZE -1)) ? (0) : (head + 1);
		len++;
		if (len >= max) break;
	}
	return len;
}

void CPTPIsr::isr_rxd()
{
	char ch;

	if (!(m_sci->SSR.BIT.RDRF)) return;
	ch = m_sci->RDR;
	m_sci->SSR.BIT.RDRF = 0;
	RecvCh(ch);
}

// Check incoming bytes via DTC
void CPTPIsr::CheckInput()
{
	if (m_pDTC) {
		char ch;
		int result;
		do {
			result = m_pDTC->GetDtcCh(&ch);
			if (result == 1) RecvCh(ch);
			else break;
		} while (1);
	}
}

void CPTPIsr::RecvCh(char ch)
{
	uint16 len;
	int next;

#if DEBUG_PHYSICAL
	m_debugBuffer[m_debugIndex++] = ch;
	m_debugIndex %= DEBUG_SIZE;
#endif

	if (ch == FLAG) {
		if (rx_head != rx_tail) {
			// Find out the length of current frame
			if (rx_head < rx_tail) {
				len = rx_tail - rx_head;
			} else {
				len = RX_SIZE + rx_tail - rx_head;
			}
			// Post the data
			if (m_recvCRC.GetCrc16() == 0) {
				// See if any data bytes gobbled
				uint8 data_ok = 1;
				// Check for data length
				if (len > 4) {
					uint16 data_length;
					data_length = ((uint16)rx_buffer[(rx_head + 2) % RX_SIZE] << 8) + 
						rx_buffer[(rx_head + 3) % RX_SIZE];
					if (data_length != len - 6) {
						data_ok = 0;
						if (m_pLink) {
							m_pLink->ReportError(rx_tail, len);
						}
#if DEBUG_PHYSICAL
						m_invalidLengthCnt++;
#endif
					}
				}
				
				if (m_pLink && data_ok) {
					m_pLink->ReportFrame(rx_tail, len);
				} 
			} else {
				// Recving error
#if DEBUG_PHYSICAL
				m_crcErrorCnt++;
#endif
				if (m_pLink) {
					m_pLink->ReportError(rx_tail, len);
				}
			}
			// rx_head == rx_tail
			// Make sure the data is gone if the link layer is not present 
			// or any error happens
			rx_head = rx_tail;
		}
		m_recvCRC.Reset();
		return;
	}
	if (m_recvEscape) {
		// Can only expect FLAG2 or ESCAPE2
		switch (ch) {
		case FLAG2:
			ch = FLAG;
			break;
		case ESCAPE2:
			ch = ESCAPE;
			break;
		default:
			// Let the CRC16 catch the error
			ch = 0; // ESCAPE;
			break;
		}
		m_recvEscape = 0;
	} else if (ch == ESCAPE) {
		m_recvEscape = 1;
		return;
	}
	// Get incoming data in ch
	
	// Update CRC16
	m_recvCRC.UpdateCrc(ch);

	// Store it into buffer
	rx_buffer[rx_tail] = ch;
	next = (rx_tail >= (RX_SIZE -1)) ? (0) : (rx_tail + 1);
	if (next != rx_head) {
		rx_tail = next;
		m_rx_total++;
	} else {
		m_rx_overrun++;
	}
}

void CPTPIsr::isr_txd()
{
	char ch;
	char next_ch;
	if (m_pSendDtc != NULL) {
		m_pSendDtc->SendIsr();
		return;
	}

    if (tx_head != tx_tail) {
		ch = tx_buffer[tx_head];
		if (ch == FLAG) {
			if (m_frameEnds.Top() != tx_head) {
				tx_buffer[tx_head] = FLAG2;
				ch = ESCAPE;
			} else {
				m_frameEnds.DeleteTop();
			}
		} else if (ch == ESCAPE) {
			tx_buffer[tx_head] = ESCAPE2;
		}
		if (ch == ESCAPE) {
			// Escape sequence, don't move pointer
			m_sci->TDR= ch;
			m_sci->SSR.BIT.TDRE = 0;
			m_tx_total++;
		} else {
			// Normal case, move pointer one further
			m_sci->TDR = tx_buffer[tx_head];
    		m_sci->SSR.BIT.TDRE = 0;
    		tx_head = (tx_head >= (TX_SIZE -1)) ? (0) : (tx_head + 1);
    		m_tx_total++;
		}
    } else {
    	/* Disable the interruption */
    	m_sci->SCR.BIT.TIE = 0;  
    }                        

}

void CPTPIsr::isr_rxd_error()
{
	if (m_sci->SSR.BIT.FER) {   
		m_sci->SSR.BIT.FER = 0;
		m_rx_error++;
	} else if (m_sci->SSR.BIT.ORER) {  
		m_sci->SSR.BIT.ORER = 0;
		m_rx_error++;
	} else if (m_sci->SSR.BIT.PER) {  
		m_sci->SSR.BIT.PER = 0;
		m_rx_error++;
	}

}

void CPTPIsr::isr_txd_idle()
{
	if (tx_head == tx_tail) { 
	    m_sci->SSR.BIT.TDRE = 0;
		m_sci->SCR.BIT.TIE = 0;
		m_sci->SCR.BIT.TEIE = 0;
		return;
	}
	if (m_pSendDtc == NULL) {
		isr_txd();
	}
}

long CPTPIsr::GetBaudRate()
{
	return m_baudrate;
}

long CPTPIsr::SetBaudRate(long rate)
{
	long old;
	long N;
	
	old = m_baudrate;
	m_baudrate = rate;
	N = ((long) SYST_CLK) / (16 * rate) - 2;
	N++;
	N >>= 1;
	m_sci->BRR = (unsigned char)N;
	
	m_realRate = (long)((double)SYST_CLK / (double)((N + 1) * 32) );	
	return old;

}

void CPTPIsr::init()
{
	volatile unsigned char tmp;
	int i;
	
	tx_head = 0; tx_tail = 0;
	rx_head = 0; rx_tail = 0;
	m_rx_error = 0; m_rx_overrun = 0;
	m_tx_total = 0; m_rx_total = 0;
	m_recvEscape = 0;

	// Hardware setup
	m_sci->SCR.BYTE = 0x0; // clear all 
	m_sci->SMR.BYTE = 0;  // clear all first
	SetBaudRate(m_baudrate); // m_sci->BRR = 51; /* Baudrate 9600 under 16M */
	
	m_sci->SMR.BIT.CA = 0; // Async mode
	m_sci->SMR.BIT.CHR = 0; // 8-bit
	
	m_sci->SMR.BIT.STOP = 1; // 1-bit stop // 3/7/2006: Make it 2 bit
	m_sci->SMR.BIT.MP = 0; // No Multi-process bit
	
	m_sci->SCR.BIT.TIE = 0;
	m_sci->SCR.BIT.TEIE = 0;
	
	m_sci->SCR.BIT.TE = 1;
	m_sci->SCR.BIT.RE = 1;
	m_sci->SCR.BIT.RIE = 1;
	
	i = 5000;
	while (i--);
	
	tmp = m_sci->RDR;
	
	i = 5000;
	while (i--);

}

void sci3_rxd_error(void)
{
	if (g_pSci3) {
		g_pSci3->isr_rxd_error();
	}
}

void sci3_rxd(void)
{
	if (g_pSci3) {
		g_pSci3->isr_rxd();
	}
}

void sci3_txd(void)
{
	if (g_pSci3) {
		g_pSci3->isr_txd();
	}
}

void sci3_txd_idle(void)
{                         
	if (g_pSci3) {
		g_pSci3->isr_txd_idle();
	}
}

void sci4_rxd_error(void)
{
	if (g_pSci4) {
		g_pSci4->isr_rxd_error();
	}
}

void sci4_rxd(void)
{
	if (g_pSci4) {
		g_pSci4->isr_rxd();
	}
}

void sci4_txd(void)
{
	if (g_pSci4) {
		g_pSci4->isr_txd();
	}
}

void sci4_txd_idle(void)
{                         
	if (g_pSci4) {
		g_pSci4->isr_txd_idle();
	}
}


void init_ptp(void)
{

#if ENABLE_SCI3

#if USE_DTC3
	g_pSci3 = new CPTPIsr("SCI3-PPC", SCI3_REG, DEFAULT_BAUD3, 121, 122);
#else
	g_pSci3 = new CPTPIsr("SCI3-PPC", SCI3_REG, DEFAULT_BAUD3, 0);
#endif

#if FAKE_SCI3
	INTC.IPRK.BIT.HIGH = 2;   // On SCI1 temply
#else
	INTC.IPRO.BIT.HIGH = INT_LEVEL_SCI3;  // Set SCI3
#endif

#else
	g_pSci3 = NULL;
  	INTC.IPRO.BIT.HIGH = 0;  // Set SCI3
#endif


#if ENABLE_SCI4
#if USE_DTC4
	g_pSci4 = new CPTPIsr("SCI4-RTE", SCI4_REG, DEFAULT_BAUD4, 125, 126); // SCI4 RTE-RTE 125  TXI4 126
#else
	g_pSci4 = new CPTPIsr("SCI4-RTE", SCI4_REG, DEFAULT_BAUD4, 0, 0); // SCI4 RTE-RTE 125  TXI4 126
#endif
  	INTC.IPRO.BIT.LOW  = INT_LEVEL_SCI4;  // Set SCI4 
#else
	g_pSci4 = NULL;
  	INTC.IPRO.BIT.LOW  = 0;  // Set SCI4 
#endif
}



void CPTPIsr::SetLink(CPTPLink *pLink)
{
	m_pLink = pLink;
}


void CPTPIsr::PrintStatus(Ccomm *pComm)
{
	char buff[100];
	uint8 N;

	N = m_sci->BRR;

	sprintf(buff, "%s Baudrate=%ld Real:%ld BRR=%d\r\n", 
		m_name, m_baudrate, m_realRate, N);
	pComm->Send(buff);

#if DEBUG_PHYSICAL
	sprintf(buff, "Debug Buffer @0x%lX Size=%d Current offset=%d(0x%X)\r\n",
		m_debugBuffer, DEBUG_SIZE, m_debugIndex, m_debugIndex);
	pComm->Send(buff);
	sprintf(buff, "CRC Error Cnt: %ld  InvalidLengthCnt=%ld\r\n"
		, m_crcErrorCnt, m_invalidLengthCnt);
	pComm->Send(buff);
#endif
	if (m_pSendDtc) {
		pComm->Send("DTC sending Info:\r\n");
		m_pSendDtc->PrintStatus(pComm);
	}

	if (m_pDTC) {
		pComm->Send("DTC recv Info:\r\n");
		m_pDTC->DtcSciHack(pComm);
	}

}
