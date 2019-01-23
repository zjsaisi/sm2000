/*                   
 * Filename: sci.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: sci.cpp 1.4 2009/05/01 10:02:50PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.4 $
 */             


#define IN_FILE_SCI_CPP 1

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "machine.h"
#include "sci.h"
#include "string.h"
#include "timer.h"
#include "BaseProcess.h"
#include "DtcSci.h"
#include "DtcSciSend.h"
#include "stdio.h"

#pragma section SCI      
// Don't want to create D* section out of this. Otherwise, it requires moving D* to B* when running from
// ROM.
CSci *g_pSci0;
CSci *g_pSci1;           
#pragma section

	
	
CSci::CSci(const char *pName, SCI_REG reg, CCheapTimer *pTimer, CPVOperation *pPV, long rate,
		   unsigned int DTCVector, unsigned int sendVector)
{     
	rx_buffer[0] = 0;

	strncpy(m_name, pName, 19);
	m_name[19] = 0;

	m_stopErrorCnt = 0;
	m_sci = reg; 
    m_pTimer = pTimer;
    
	m_sci->SCR.BIT.TE = 0;
	m_sci->SCR.BIT.RE = 0;          
	m_baudrate = rate;
	m_intrLevel = 7;
	m_pTask = NULL;
	m_from = 0;
	m_pPV = pPV; //new CPVOperation();
	m_bOpened = 0;

	m_pDTC = NULL;
	if (DTCVector) {
		// Check if boot code support it
		int bValid;
		bValid = CDtcSci::CheckVector(DTCVector);
		if (bValid == 1) {
			m_pDTC = new CDtcSci(DTCVector);
		}
	}

	m_pSendDtc = NULL;
	if (sendVector) {
		int bValid;
		bValid = CDtcSci::CheckVector(sendVector);
		if (bValid == 1) {
			m_pSendDtc = new CDtcSciSend(m_sci, sendVector);
		}
	}

	if (m_pSendDtc == NULL) {
		tx_buffer = new char[TX_SIZE];
	} else {
		tx_buffer = NULL;
	}
	init();	
	 
}

// Never used
CSci::~CSci()
{
	delete m_pPV;
}

void CSci::init(void)
{                    
	volatile unsigned char tmp;
	int i;
	
	tx_head = 0; tx_tail = 0;
	rx_head = 0; rx_tail = 0;
	m_rx_error = 0; m_rx_overrun = 0;
	m_tx_total = 0; m_rx_total = 0;

	// Hardware setup
	m_sci->SCR.BYTE = 0x0; // clear all 
	m_sci->SMR.BYTE = 0;  // clear all first
	CSci::SetBaudRate(m_baudrate); // m_sci->BRR = 51; /* Baudrate 9600 under 16M */
	
	m_sci->SMR.BIT.CA = 0; // Async mode
	m_sci->SMR.BIT.CHR = 0; // 8-bit
	
	m_sci->SMR.BIT.STOP = 1; // 2-bit stop for sending
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

long CSci::SetBaudRate(long rate)
{  
	long old;
	long N;
	
	old = m_baudrate;
	m_baudrate = rate;
	N = ((long) SYST_CLK) / (16 * rate) - 2; // CKS1 = 0, CKS0 = 0 in SMR
	N++;
	N >>= 1;
	m_sci->BRR = (unsigned char)N;
	
	m_realRate = (long)((double)SYST_CLK / (double)((N + 1) * 32) );
	return old;
}

long CSci::GetBaudRate(void)
{
	return m_baudrate;
}

void CSci::isr_rxd_error(void)
{            
	if (m_sci->SSR.BIT.FER) {   
		m_sci->SSR.BIT.FER = 0;
		m_stopErrorCnt++;
		m_rx_error++;
	} else if (m_sci->SSR.BIT.ORER) {  
		m_sci->SSR.BIT.ORER = 0;
		m_rx_error++;
	} else if (m_sci->SSR.BIT.PER) {  
		m_sci->SSR.BIT.PER = 0;
		m_rx_error++;
	}
}

void CSci::isr_rxd(void)
{ 
	int next;

#if 0
	if (m_pDTC) {
		if (m_sci->SSR.BIT.RDRF) {
			m_sci->SSR.BIT.RDRF = 0;
		}
		return;
	}
#endif

	if (m_sci->SSR.BIT.RDRF) {
		next = (rx_tail >= (RX_SIZE - 1)) ? (0) : (rx_tail + 1);
		rx_buffer[rx_tail] = m_sci->RDR;
		m_sci->SSR.BIT.RDRF = 0;  
		
		if (next != rx_head) {
			rx_tail = next;
			m_rx_total++;
		} else {
			m_rx_overrun++;
		}                  
		if (m_pTask) {
			m_pTask->Notify(m_from, 0);
		}
	}          
	
}

void CSci::isr_txd(void)
{    
	if (m_pSendDtc) {
		m_pSendDtc->SendIsr();
		return;
	}

    if (tx_head != tx_tail) {
    	m_sci->TDR = tx_buffer[tx_head];
    	m_sci->SSR.BIT.TDRE = 0;
    	tx_head = (tx_head >= (TX_SIZE -1)) ? (0) : (tx_head + 1);
    	m_tx_total++;
    } else {
    	/* Disable the interruption */
    	m_sci->SCR.BIT.TIE = 0;  
    }                        
}

void CSci::isr_txd_idle(void)
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

int CSci::Receive(char *pBuffer, int max_len, long within_ms)
{
	return Recv(pBuffer, max_len, within_ms, 0);
}

void CSci::CheckInput()
{
	register int index;
	int result;
	int oldTail = rx_tail;

	index = rx_tail;
	// int cnt = 0;
	if (m_pDTC != NULL) {
		do {
			result = m_pDTC->GetDtcCh(rx_buffer+index);
			if (result == 1) {
				if (++index >= RX_SIZE) {
					index = 0;
				}
				continue;
			} else {
				break;
			}
		} while (1);
		rx_tail = index;

		if ((oldTail != index) && m_pTask) {
			m_pTask->Notify(m_from, 0);
		}
	}
}

void memcpy_zm(char *pTarget, char *pSrc, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		*pTarget++ = *pSrc++;
	}
}

int CSci::Recv(char *pBuffer, int max_len, long within_ms, int no_interrupt)
{                  
	register int tail;
	int block;
    int next, len = 0;
    if (max_len <= 0) return 0;
    if (m_pTimer) {                             
    	m_pTimer->Start(within_ms);
    }

    while (max_len > 0) {
		if ((no_interrupt) && (m_pDTC == NULL)) {
			isr_rxd();
			isr_rxd_error();
		}
    	if (rx_head != rx_tail) {
#if 0
    		*pBuffer = rx_buffer[rx_head];
    		next = (rx_head >= (RX_SIZE -1)) ? (0) : (rx_head+1);
    		len++;
    		rx_head = next;
    		pBuffer++;   
    		max_len--;
#else
			tail = rx_tail;
			if (rx_head < tail) {
				block = tail - rx_head;
				if (block > max_len) {
					block = max_len;
				}
				memcpy_zm(pBuffer, rx_buffer + rx_head, block);
				pBuffer += block;
				max_len -= block;
				next = rx_head + block;
				if (next >= RX_SIZE) next -= RX_SIZE;
				rx_head = next;
				len += block;
			} else if (rx_head > tail) {
				block = RX_SIZE - rx_head;
				if (block > max_len) {
					block = max_len;
				}
				memcpy_zm(pBuffer, rx_buffer + rx_head, block);
				pBuffer += block;
				max_len -= block;
				next = rx_head + block;
				if (next >= RX_SIZE) next -= RX_SIZE;
				rx_head = next;
				len += block;
			}
#endif
        } else if (m_pTimer) {
        	if (m_pTimer->TimeOut()) return len;
			m_pPV->Pause(1);
        }
    }
    return len; 
}
                               
// 1 OK
// -1, -2, -3 timeout
int CSci::WaitTxDone(long within_ms)
{            
	CCheapTimer timer;

	timer.Start(within_ms);

	while (m_pSendDtc) {
		if (m_pSendDtc->IsIdle() > 0) return 1;
		if (timer.TimeOut()) return -1;
		m_pPV->Pause(1);
	}

	while (tx_head != tx_tail) {
		if (timer.TimeOut()) return -1;
		m_pPV->Pause(1);
	}                  
	while (!(m_sci->SSR.BIT.TEND)) {
		if (timer.TimeOut()) return -3;
		m_pPV->Pause(1);
	}
	return 1;
}                         

// return bytes sent
int CSci::Send(const char *pBuffer, int len, int toOpen)
{ 
	int i, next, sent = 0;    

	if (len == -1) {
		len = strlen(pBuffer);
	}
	if (len <= 0) return 0;

	if (toOpen) {
		// If already opened by other task, skip the output.
		if (m_bOpened) return 0;
		m_pPV->P();
	}

	if (m_pSendDtc) {
		sent = m_pSendDtc->Send(pBuffer, len);
		m_pSendDtc->flush();
		//m_sci->SCR.BIT.TIE = 1;
		if (toOpen) m_pPV->V();
		return sent;
	}

	for (i = 0; i < len; i++) {
	    next = (tx_tail >= (TX_SIZE - 1)) ? (0):(tx_tail + 1);
	    while (next == tx_head) {
	    	/* Full now*/
			// Make sure the interrupt is on. Otherwise it will 
			// wait forever. Interruption should be enabled.
			if (!(m_sci->SCR.BIT.TIE)) {
				uint8 tmp_exr = get_exr();
				set_exr(7);
				isr_txd();
				set_exr(tmp_exr);
				m_sci->SCR.BIT.TIE = 1;
			}
	    	m_pPV->Pause(1); 
	    };
		tx_buffer[tx_tail] = *pBuffer;
		tx_tail = next;
		pBuffer++;
		sent++;
	}            
	/* Enable sending intr here */
	uint8 oldMask;
	oldMask = get_exr();
	set_exr(7); // DISABLE interrupt;
	if (!(m_sci->SCR.BIT.TIE)) { 
		isr_txd();
		// If intr is disabled when this is called, shouldn't enable Tx interrupt.
		if ((oldMask & 7) < m_intrLevel)            
			m_sci->SCR.BIT.TIE = 1;
	} 

	set_exr(oldMask); // ENABLE_INTR;

	if (toOpen) m_pPV->V();
	return sent;

}

// 1 for success
// -1 for timeout
int CSci::ForceSend(const char *pBuffer, int len)
{
	int i, ret;   
//	unsigned long max_cnt;
	if (len == -1) {
		len = strlen(pBuffer);
	}
	
	WaitTxDone(2000L);
	for (i = 0; i < len; i++) {
		Send(pBuffer, 1);
		ret = WaitTxDone(100L);
		if (ret != 1) return ret;
#if 0
		max_cnt = 30000;
    	while ( ((m_sci->SSR.BIT.TDRE) == 0) || (m_sci->SSR.BIT.TEND == 0)) {
    		if ((max_cnt--) == 0) return -1;
    	};
#endif		
		pBuffer++;
	}
    return 1;
}

void CSci::SetSciNotification(CBaseProcess *pTask, uint32 from)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	m_pTask = pTask;
	m_from = from;
	set_exr(tmp_exr);
}

void CSci::Open(void)
{
	m_pPV->P();
	m_bOpened = 1;
}

void CSci::Close(void)
{
	m_bOpened = 0;
	m_pPV->V();
}

void sci0_rxd_error(void)
{                              
	if (g_pSci0) {
		g_pSci0->isr_rxd_error();
	}
}

void sci0_rxd(void)
{
	if (g_pSci0) {
		g_pSci0->isr_rxd();
	}   
	
}

void sci0_txd(void)
{                         
	if (g_pSci0) {
		g_pSci0->isr_txd();
	}
}

void sci0_txd_idle(void)
{                         
	if (g_pSci0) {
		g_pSci0->isr_txd_idle();
	}
}


void sci1_rxd_error(void)
{                              
	if (g_pSci1) {
		g_pSci1->isr_rxd_error();
	}
}

void sci1_rxd(void)
{
	if (g_pSci1) {
		g_pSci1->isr_rxd();
	}   
	
}

void sci1_txd(void)
{                         
	if (g_pSci1) {
		g_pSci1->isr_txd();
	}
}

void sci1_txd_idle(void)
{                         
	if (g_pSci1) {
		g_pSci1->isr_txd_idle();
	}
}

void sci1_init(void)
{
#if ENABLE_SCI1                              
    CCheapTimer *pTimer;    
    CPVOperation *pPV;
                        
    pPV = new CPVOperation();
	pTimer = new CCheapTimer();
#if USE_DTC1
	g_pSci1 = new CSci("SCI1(USR)", SCI1_REG, pTimer, pPV, DEFAULT_BAUD1, 85, 86); // RXI1 85  TXI 86
#else
	g_pSci1 = new CSci("SCI1(USR)", SCI1_REG, pTimer, pPV, DEFAULT_BAUD1, 0, 0); // RXI1 85  TXI 86
#endif
	g_pSci1->m_intrLevel = 1;
	INTC.IPRK.BIT.HIGH = INT_LEVEL_SCI1;   
#else
	g_pSci1 = NULL;
#endif
}
                         
static void sci0_init(void)
{            
#if ENABLE_SCI0       
	CCheapTimer *pTimer;              
	char *ptr;
	int i;  
    CPVOperation *pPV;
                        
    pPV = new CPVOperation();                        
	pTimer = new CCheapTimer();
	P3.DDR = 0x89;
	P3.DR.BYTE |= 0x89;
	P3.ODR.BYTE = 0;

#if USE_DTC0
	g_pSci0 = new CSci("Sci0(Dbg)", SCI0_REG, pTimer, pPV, DEFAULT_BAUD0, 81, 82); // RXI0 81 TXI0 82
#else
	g_pSci0 = new CSci("Sci0(Dbg)", SCI0_REG, pTimer, pPV, DEFAULT_BAUD0, 0, 0); // RXI0 81 TXI0 82
#endif
	// g_pSci0->SetBaudRate(38400);
	// g_pSci0->SetBaudRate(57600);
	g_pSci0->m_intrLevel = 1; 
	INTC.IPRJ.BIT.LOW = INT_LEVEL_SCI0;
#ifdef _DEBUG	
	ptr = "\r\n(Debug)Debug port ready:\r\n";  
#else    
	ptr = "\r\nMain: Debug port ready\r\n";  
#endif	    
	// g_pSci0->send(ptr, strlen(ptr));
    // Assume Intr is not enabled yet
    //g_pSci0->ForceSend(ptr); 
#else                     
	g_pSci0 = NULL;
	INTC.IPRJ.BIT.LOW = 0;
#endif	
}
               
void sci_init(void)
{      	
	sci1_init();         
	sci0_init();
	if (g_pSci1) {
		g_pSci1->SetBaudRate(DEFAULT_BAUD1);
	} 
	if (g_pSci0) {
		g_pSci0->SetBaudRate(DEFAULT_BAUD0);
	}              
}             


int CSci::GetStopError()
{
	return m_stopErrorCnt;
}

void CSci::ResetStopError()
{
	m_stopErrorCnt = 0;
}

void CSci::flush()
{
	if (m_pSendDtc) {
		m_pSendDtc->flush();
	}
}

void CSci::Test()
{
	if (m_pSendDtc) {
		m_pSendDtc->Test();
	}
}

void CSci::PrintStatus(Ccomm *pComm)
{
	char buff[100];
	uint8 N;

	N = m_sci->BRR;

	sprintf(buff, "%s Baudrate=%ld Real:%ld BRR=%d\r\n", 
		m_name, m_baudrate, m_realRate, N);
	pComm->Send(buff);
	if (m_pSendDtc) {
		m_pSendDtc->PrintStatus(pComm);
	}
}

int CSci::GetRxSize(void) 
{
	register int head, tail;
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	head = rx_head;
	tail = rx_tail;

	set_exr(tmp_exr);

	if (head <= tail) {
		return tail - head;
	}
	return (RX_SIZE - head) + tail;
}
