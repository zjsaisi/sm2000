// PTPLink.cpp: implementation of the CPTPLink class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPLink.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: PTPLink.cpp 1.4 2009/05/05 14:21:47PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "iodefine.h"     
#include "CxFuncs.h"
#include "RTC.h"
#include "machine.h"
#include "stdio.h"
#include "sci.h"
#include "PTPIsr.h"
#include "PTPLink.h"
#include "MemHeap.h"
#include "MemBlock.h"
#include "BaseProcess.h"
#include "AllProcess.h"
#include "DebugMask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPTPLink::CPTPLink(const char *pName) // : m_sendBufferSema(1)
{
	int i;

	m_tick = 0L;
	m_SSR = 0;
	m_ASR = 0;
	memset(m_cmdHeap, 0, sizeof(m_cmdHeap));
	for (i = 0; i < NUM_DATA_BLOCK; i++) {
		m_dataList[i] = NULL;
	}
	for (i = 0; i < NUM_SEND_BLOCK; i++) {
		m_sendList[i] = NULL;
	}
	m_resendTimer = 0L;

	m_linkUpCnt = 0;
	// Event report setup
	m_discEvent = 0;
	m_discEventAsserted = 0;

	m_pDriver = NULL;
	m_dataTail = 0; m_dataHead = 0; 
	m_cmdTail = 0; m_cmdHead = 0;
	m_taskSlot = 0;
	m_state = DISC_STATE;
	strncpy(m_name, pName, 8);
	m_name[8] = 0;
	m_pRecvMemHeap = new CMemHeap();
	m_pRecvMemHeap->SetupList(40, 74);
	m_pRecvMemHeap->SetupList(40, 264);
	m_pRecvMemHeap->SetupList(6, FRAME_LENGTH + 6);

	m_pSendMemHeap = new CMemHeap();
	m_pSendMemHeap->SetupList(80, 72);
	m_pSendMemHeap->SetupList(40, 264);
	m_pSendMemHeap->SetupList(6, FRAME_LENGTH + sizeof(PTPSendBuffer));
	m_pending = 0;
	m_sendHead = 0; m_sendTail = 0;

	// Client
	m_clientCnt = 0;
	memset(m_clientList, 0, sizeof(m_clientList));

	m_recvRepeatCnt = 0L; // for debug
	m_nakSentCnt = 0L; // for debug
	m_nakRecvCnt = 0L; // for debug
	m_discCnt = 0L; // debug
	m_timeoutResendCnt = 0L; // debug
	m_cmdErrorCnt = 0L; // debug

#if DEBUG_LAST_MESSAGE
	m_lastSize = 0;
	memset(m_lastMessage, 0, sizeof(m_lastMessage));
	m_lastPort = 0;
	m_lastSSR = 0;
	m_lastASR = 0;
#endif
}

CPTPLink::~CPTPLink()
{

}

// Called by ISR to retrieve the data out of ISR
// Execution of this function is part of ISR
void CPTPLink::ReportFrame(int tail, int len)
{
	char *ptr;
	if (len == 4) {
		// Incoming command
		int next;
		next = (m_cmdTail >= (NUM_CMD_BLOCK - 1)) ? (0) : (m_cmdTail + 1);
		if (next != m_cmdHead) {
			ptr = m_cmdHeap[m_cmdTail];
			m_pDriver->RtrvFrame(tail, ptr, 4);
			m_cmdTail = next;
		} // else {
		  // Too many incoming command, ignore this one	
		//}

		// notify the cmd pointed by ptr
		K_Intrp_Event_Signal(0, m_taskSlot, CMD_INDICATION);
	} else if (len > 4) {

#if 0
		if (m_dataCnt < 2) {
			// Incoming data
			ptr = m_dataHeap[m_dataTail];
			m_pDriver->RtrvFrame(tail, ptr, len);
			m_dataLength[m_dataTail] = len;
			m_dataTail++;
			m_dataTail %= NUM_DATA_BLOCK;
			m_dataCnt++;
		}
#else 
		CMemBlock *pBlock;
#if INSERT_ERROR
		static int error_cnt = 0;
		error_cnt++;
		if (error_cnt > 15) {
			error_cnt = 0;
			return;
		}
#endif
		pBlock = m_pRecvMemHeap->Allocate(len);
		if (pBlock) {
			ptr = (char *)pBlock->GetBuffer();
			m_pDriver->RtrvFrame(tail, ptr, len);
			pBlock->SetCurSize(len);
			m_dataList[m_dataTail++] = pBlock;
			m_dataTail %= NUM_DATA_BLOCK;
		}
#endif
		// else {
			// Buffer full, should never happen
			// ignore the incoming data once
		//}
		// Notify the data referenced by ptr
		K_Intrp_Event_Signal(0, m_taskSlot, DATA_INDICATION);
	} // else {
		// Weird garbage. Ignore
	//}
}

// Called by ISR to indicate receiving error
// Execution of this function is part of ISR
void CPTPLink::ReportError(int tail, int len)
{
	if (len > 4) {
		// Receive data error, report to link task
		K_Intrp_Event_Signal(0, m_taskSlot, DATA_ERROR_INDICATION);
	} else {
	  // Cmd error or something else. Ignore
		m_cmdErrorCnt++;
	}
}

void CPTPLink::SetDriver(CPTPIsr *pDriver)
{
	m_pDriver = pDriver;
}

void CPTPLink::SetTaskSlot(uint8 slot)
{
	m_taskSlot = slot;
}

void CPTPLink::EventDispatch(uint16 events)
{
	if (events & CMD_INDICATION) {
		IndicateCmd();
	}
	if (events & DATA_INDICATION) {
		IndicateData();
	}
	if (events & DATA_ERROR_INDICATION) {
		IndicateDataError();
	}
	if (events & DATA_REQUEST) {
		DataRequest();
	}
	if (events & RESET_INDICATION) {
		EnterDisc();
	}
}

void CPTPLink::IndicateData()
{   
	uint8 cmd;
	uint8 SSin, ASin;
	uint8 *ptr;
	uint8 tmp;
	int len;
	CMemBlock *pBlock = NULL;
	uint8 bDataPosted = 0;

	while (1) {
		// Retrieve one item /////////////////////////////////////////////////
		if ((!bDataPosted) && (pBlock != NULL)) {
			pBlock->Release();
		}
		bDataPosted = 0;
		if (m_dataHead == m_dataTail) break;
		pBlock = m_dataList[m_dataHead++];
		m_dataHead %= NUM_DATA_BLOCK;
		ptr = pBlock->GetBuffer();
		len = pBlock->GetCurSize();
		
#if 0
		len = 0;
		ptr = NULL;
		tmp_exr = get_exr();
		set_exr(7);
		if (m_dataHead == m_dataTail) {
			if (m_dataCnt == 0) {
				//ptr = NULL;
			} else if (m_dataCnt == NUM_DATA_BLOCK) {
				ptr = (uint8 *)(m_dataHeap[m_dataHead]);
				len = m_dataLength[m_dataHead];
				m_dataHead++;
				m_dataHead %= NUM_DATA_BLOCK;
				m_dataCnt--;
			} else {
				// Should never be here. Something is messed up.
				// throw away all data
				m_dataCnt = 0;
				m_dataHead = m_dataTail;
				//ptr = NULL;
			}
		} else if (m_dataCnt > 0) {
			ptr = (uint8 *)(m_dataHeap[m_dataHead]);
			len = m_dataLength[m_dataHead];
			m_dataHead++;
			m_dataHead %= NUM_DATA_BLOCK;
			m_dataCnt--;
		} else {
			// Should never be here
			m_dataCnt = 0;
			m_dataHead = m_dataTail;
			//ptr = NULL;
		}
		set_exr(tmp_exr);
		if (ptr == NULL) break;
#endif

		// At this point Item retrieved and pointed by ptr //////////////////////////////////
		
		if ((*ptr & 0xc0) != 0x80) { 
			// Invalid data. 
			continue;
		}

		SSin = (*ptr >> 3) & 7;
		ASin = *ptr & 7;
		cmd = ptr[1];

		// In DISC state.///////////////////////////////////////////////////
		if (m_state == DISC_STATE) {
			// In disconnected state, discard the data.
			uint8 tmp_exr = get_exr();
			set_exr(7);
			m_dataHead = m_dataTail;
			//m_dataCnt = 0;
			set_exr(tmp_exr);
			m_tick = GetRawTick();
			continue;
		}

		// Connected state////////////////////////////////////////////////////
		if (m_state == CONN_STATE) {
			// Check SSin //////////////////////////////////
			if (NextSequence(SSin) == m_ASR) {
				// Repetition of previous frame, send ACK
				SendCmd(CMD_ACK, m_SSR, m_ASR); ///SendCmd(CMD_ACK, m_SSR, SSin); // 3/9/2006.
				m_tick = GetRawTick();
				m_recvRepeatCnt++;
			} else if (SSin == m_ASR) {
				// New Data arrived.
				// Post the data according to ctrl2
				
				// Debug
				if (ptr[1] == 0) {
#if 0
					char buff[80];
					//sprintf(buff, "SSin=%d ASin=%d Ctrl2=0x%02x Len=%d\r\n",
					//	(*ptr >> 3) & 7, *ptr & 7, ptr[1], len);
					//DebugOutput(buff);
#else
					sDebugOutput("SSin=%d ASin=%d Ctrl2=0x%02x Len=%d\r\n",
						(*ptr >> 3) & 7, *ptr & 7, ptr[1], len);
#endif              
					*(ptr + len - 2) = 0;
					DebugOutput((char *)(ptr + 4), 1);
					pBlock->Release();
					bDataPosted = 1;
				} else {
					// For now release the memory here
					//pBlock->Release();
					CBaseProcess *pClient = FindClient(ptr[1]);
					if (pClient == NULL) {
						// No client found
						pBlock->Release();
					} else {
						// Before give out the memblock, adjust the data block
						// to delete link info
						pBlock->SetOffset(4);
						pBlock->SetCurSize(len - 6);
						if (pClient->Notify((uint32)this, (uint32)pBlock) <= 0) {
							pBlock->Release();
						}
					}

					bDataPosted = 1;
				}
				
				m_tick = GetRawTick();
				m_ASR = NextSequence(m_ASR);
				// This ACK can be saved, if we know are going to send data
				SendCmd(CMD_ACK, m_SSR, m_ASR);
			} else {
				// SSin is out of sequence;
#if DEBUG_PTP
				{
					char buff[100];
					sprintf(buff, "SSin[%d] is out of sequence\r\n", SSin);
					DebugOutput(buff);
				}
#endif
				EnterDisc();
				continue;
			}
			
			// Check ASin //////////////////////////////////
			if (ASin == m_SSR) {
				// That's expected. Not confirming any thing new
			} else if (ASin == NextSequence(m_SSR)) {
				if (m_pending) {
					// The frame was confirmed.
#if DEBUG_PTP					
					{
						char buffer[70];
						sprintf(buffer, "Frame[%d] confirmed\r\n", m_SSR);
						DebugOutput(buffer);
					}
#endif
					m_SSR = NextSequence(m_SSR);
					// Release send buffer
					m_pending = 0;
					m_sendList[m_sendHead]->Release();
					m_sendHead = (m_sendHead + 1) % NUM_SEND_BLOCK;
					K_Event_Signal(0, m_taskSlot, DATA_REQUEST);
					//m_sendBufferSema.Post();
				} else {
					// ASin moved ahead all by itself. Gross error
#if DEBUG_PTP					
					char buff[80];
					sprintf(buff, "Unexpected ASin[%d] in data\r\n", ASin);
					DebugOutput(buff);					
#endif
				}
			} else if (NextSequence(ASin) == m_SSR) {
				// Ack Sending sequence in data packet could be one behind another ACK.
				// because incoming data is dealt with in ISR. ACK and Data could be presented
				// at the same time.
			} else {
				// ASin unexpected in data frame
#if DEBUG_PTP
				char buff[80];
				sprintf(buff, "Upexpected ASin[%d] m_SSR=[%d] in data frame\r\n", 
					ASin, m_SSR);
				DebugOutput(buff);
#endif				              
				EnterDisc();
				continue;
			}
			continue;
		}
	}
}

void CPTPLink::IndicateDataError()
{
	// If not connected, don't deal with data errro
	if (m_state != CONN_STATE) return;
	// Connect state
	SendCmd(CMD_NAK, m_SSR, m_ASR);
#if DEBUG_PTP
	DebugOutput("Invalid data frame received\r\n");
#endif
	m_nakSentCnt++;
}

void CPTPLink::IndicateCmd()
{
	uint8 *ptr;
	uint8 cmd;
	uint8 SSin, ASin;
	while (m_cmdHead != m_cmdTail) {
		// Retrieve command //////////////////////////////////////////////
		ptr = (uint8 *)m_cmdHeap[m_cmdHead];
		cmd = ptr[1];
		SSin = (*ptr >> 3) & 7;
		ASin = *ptr & 7;
		m_tick = GetRawTick();
		m_cmdHead = (m_cmdHead >= (NUM_CMD_BLOCK -1)) ? (0) : (m_cmdHead + 1);
		if ((*ptr & 0xc0) != 0xc0) {
			// Invalid command, ignore it
			continue;
		}
		
		// In disc state //////////////////////////////////////////////////
		if (m_state == DISC_STATE) {
			if ((cmd == CMD_ACK) && (SSin == 1) && (ASin == 1)) {
				SendCmd(CMD_ACK, 1, 1);
				EnterConn();
			} else if ((cmd == CMD_RESET_LINK) && (SSin == 1) && (ASin == 1)) {
				SendCmd(CMD_ACK, 1, 1);
				continue;
			}
			continue;
		} 

		// In connected state /////////////////////////////////////////////
		if (m_state == CONN_STATE) {
			if (cmd == CMD_ACK) {
				// Ignore SSin. SSin should == m_ASR
				
				// Check for ASin
				if ((ASin == 1) && (SSin == 1)) {
					// Special ACK for DISC -> CONN
					// Ignore here
					continue;
				} else if (ASin == m_SSR) {
					// Repetition of ACK, ignore it
				} else if (ASin == NextSequence(m_SSR)) {
					// There should be data frame to be confirmed on this side
					if (m_pending) {
						m_SSR = NextSequence(m_SSR);
						// Release send buffer
						m_pending = 0;
						//m_sendBufferSema.Post();
						m_sendList[m_sendHead]->Release();
						m_sendHead = (m_sendHead + 1) % NUM_SEND_BLOCK;
						K_Event_Signal(0, m_taskSlot, DATA_REQUEST);
					} else {
						// No data pending here.
						// ASin moved ahead all by itself. Gross error
#if DEBUG_PTP					
						char buff[80];
						sprintf(buff, "Unexpected ASin[%d] in ACK\r\n", ASin);
						DebugOutput(buff);					
#endif
					}
				} else if (NextSequence(ASin) == m_SSR) {
					// This case is forgiven. See comment in function IndicateData().
				} else {
					// ASin is not expected.
					// ASin unpexected in data frame
#if DEBUG_PTP
					char buff[80];
					sprintf(buff, "Upexpected ASin[%d] SSR=%d in cmd frame\r\n", ASin, m_SSR);
					DebugOutput(buff);
#endif					              
					EnterDisc();
					continue;
				}

			} else if (cmd == CMD_NAK) {
				if ((m_pending > 0) && (m_pending < 5)) { // 3/6/2009 zmiao: Changed from 10 to 5. The next 5 repetition will depend on time out. 
					// Resend the data frame
					PTPSendBuffer *pBuffer =(PTPSendBuffer *)(m_sendList[m_sendHead]->GetBuffer());
					m_pending++;
					//SendData(m_sendBuffer, m_len, m_ctrl2);
					SendData((char *)pBuffer->buffer, pBuffer->len, pBuffer->ctrl2);
				} else if (m_pending >= 10) {
					// timeout
					// View this as major error
#if DEBUG_PTP
					DebugOutput("Maximum NAK received\r\n");
#endif
#if DEBUG_LAST_MESSAGE
					PTPSendBuffer *pBuffer =(PTPSendBuffer *)(m_sendList[m_sendHead]->GetBuffer());
					RecordLast((char *)pBuffer->buffer, pBuffer->len, pBuffer->ctrl2);
#endif
					EnterDisc();
				} // else {
					// Unexpected NAK, may be extra NAK after timeout
				//}
				m_nakRecvCnt++;
			} else if (cmd == CMD_PING) {
				// Ignore for now. timer was cleared
				SendCmd(CMD_ACK, m_SSR, m_ASR);
				continue;
			} else if (cmd == CMD_RESET_LINK) {
#if DEBUG_PTP
				{
					DebugOutput("RESET recved\r\n");
				}
#endif
				// When receive RESET, don't send reset back.
				// Instead, to send a ACK(1, 1) back.
				EnterDisc(0);
				continue;
			}
			continue;
		}
	}
}

void CPTPLink::EventTimeOut()
{
	uint32 time;
	time = GetRawTick();
	if (m_state == DISC_STATE) {
		if (m_linkUpCnt-- > 0) {
			m_linkUpCnt = 0; // From connected to disconnected
		} else if (m_linkUpCnt < -10000) {
			m_linkUpCnt = -10000;
		}
		
		if ((time - m_tick) > 15 * TICK_1SEC) {
			// In DISC state time out
			SendCmd(CMD_RESET_LINK, 1, 1);
			m_tick = time;
		}
		return;
	}
	if (m_state == CONN_STATE) {
		if (m_linkUpCnt++ < 0) {
			m_linkUpCnt = 0; // From disc to conn
		} else if (m_linkUpCnt > 10000) {
			m_linkUpCnt = 10000;
		}

		if ((time - m_tick) > 7 * TICK_1SEC) {
#if DEBUG_PTP
			{
				DebugOutput("Timeout in Connected\r\n");
			}
#endif
			EnterDisc();
			m_tick = time;
			return;
		}
		if ((time - m_tick) > 3 * TICK_1SEC) {
			SendCmd(CMD_PING, m_SSR, m_ASR);
		}
		if (m_pending) {
			PTPSendBuffer *pBuffer = (PTPSendBuffer *)(m_sendList[m_sendHead]->GetBuffer());
			uint32 timeout = (pBuffer->len > 256) ? (2*TICK_1SEC) : (TICK_1SEC);
			if ((time - m_resendTimer) > timeout) {
				if (m_pending++ < 10) {
					//SendData(m_sendBuffer, m_len, m_ctrl2);
					SendData((char *)(pBuffer->buffer), pBuffer->len, pBuffer->ctrl2);
					m_timeoutResendCnt++;
				} else {
#if DEBUG_PTP
					DebugOutput("Maximum resending exceeded @500\r\n");
#endif
#if DEBUG_LAST_MESSAGE
					RecordLast((char *)pBuffer->buffer, pBuffer->len, pBuffer->ctrl2);
#endif
					EnterDisc();
				}
			}
		}
		return;
	}
}

uint8 CPTPLink::NextSequence(uint8 cur)
{
	return((cur >= 7) ? (4) : (cur+1));
}

void CPTPLink::EnterDisc(int bToReset)
{
	uint8 tmp_exr;

	m_tick = GetRawTick();

	tmp_exr = get_exr();
	set_exr(7);

	m_SSR = 1;
	m_ASR = 1;
	m_state = DISC_STATE;
	// Clear receivind buffer
	m_cmdHead = m_cmdTail = 0;
	m_dataHead = m_dataTail = 0; // m_dataCnt = 0;
	m_pending = 0; // No data is pending for ACK
 	
	set_exr(tmp_exr);
    
    m_pRecvMemHeap->Reset();
    m_pSendMemHeap->Reset();
	m_sendTail = m_sendHead = 0;

	// Send RESET
	if (bToReset) {
		SendCmd(CMD_RESET_LINK, m_SSR, m_ASR);
	} else {
		SendCmd(CMD_ACK, 1, 1);
	}

#if DEBUG_PTP
	DebugOutput("Disconnected STATE\r\n");
#endif

	if (m_discEvent) {
		if (!m_discEventAsserted) {
			CAllProcess::g_pAllProcess->ReportEvent(m_discEvent, 1);
			m_discEventAsserted = 1;
		}
	}

	m_discCnt++;
}

void CPTPLink::EnterConn()
{
	m_SSR = 2;
	m_ASR = 2;
	m_tick = GetRawTick();
	m_state = CONN_STATE;
	//m_sendBufferSema.Reset();

#if DEBUG_PTP
	DebugOutput("Connected STATE\r\n");
#endif

	if (m_discEvent) {
		if (m_discEventAsserted) {
			// Clear event
			CAllProcess::g_pAllProcess->ReportEvent(m_discEvent, 0);
			m_discEventAsserted = 0;
		}
	}
}

void CPTPLink::Restart()
{
	K_Event_Signal(0, m_taskSlot, RESET_INDICATION);
}

void CPTPLink::SendCmd(uint8 cmd, uint8 ss, uint8 as)
{
	char buff[2];

	buff[0] = 0xc0 + ((ss & 7) << 3) + (as & 7);
	buff[1] = cmd;
	m_pDriver->SendFrame(buff, 2);
}

void CPTPLink::DebugOutput(const char *str, int no_header)
{     
	if (!no_header) {
		CAllProcess::MaskedPrint((uint32)1 << CDebugMask::PTP_GROUP, m_name);
		CAllProcess::MaskedPrint((uint32)1 << CDebugMask::PTP_GROUP, ": ");
	}
	CAllProcess::MaskedPrint((uint32)1 << CDebugMask::PTP_GROUP, str);

}

void CPTPLink::sDebugOutput(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	CAllProcess::vMaskedPrint((uint32)1 << CDebugMask::PTP_GROUP, format, list);
	va_end(list);
}

void CPTPLink::SendData(const char *ptr, int len, uint8 ctrl2)
{
	uint8 ctrl[2];

	ctrl[0] = 0x80 + ((m_SSR & 7) << 3) + (m_ASR & 7);
	ctrl[1] = ctrl2;
	m_pDriver->SendFrame(ptr, len, (char *)ctrl);
	m_resendTimer = GetRawTick();
}

#if DEBUG_LAST_MESSAGE
void CPTPLink::RecordLast(const char *ptr, int len, uint8 port)
{
	int min;
	m_lastSSR = m_SSR & 7;
	m_lastASR = m_ASR & 7;
	m_lastPort = port;
	m_lastSize = len;
	min = (len <= LAST_SIZE) ? (len) : (LAST_SIZE);
	memcpy(m_lastMessage, ptr, min);
}
#endif

// 1 OK
// -1 link busy
// -2 Data too long
// -3 link is not connected
// -4 no more buffer
// -5 no more buffer pointer
int CPTPLink::PostFrame(const char *ptr, uint16 len, uint8 ctrl2, int min_left /* = 0 5/27/2004 */)
{
	if (len > FRAME_LENGTH) return -2;
	if (m_state != CONN_STATE) return -3;

#if 1
	CMemBlock *pBlock;
	PTPSendBuffer *pBuffer;
	int next;
	m_sendPV.P();
	pBlock = m_pSendMemHeap->Allocate(len + sizeof(PTPSendBuffer), min_left);
	if (pBlock == NULL) {
		m_sendPV.V();
		return -4;
	}
	m_sendList[m_sendTail] = pBlock;
	next = (m_sendTail + 1) % NUM_SEND_BLOCK;
	if (next == m_sendHead) {
		// Full
		pBlock->Release();
		m_sendPV.V();
		return -5;
	}
	// Load the data
	pBuffer = (PTPSendBuffer *)pBlock->GetBuffer();
	pBuffer->ctrl2 = ctrl2;
	pBuffer->len = len + 2;
	pBuffer->buffer[0] = (uint8)(len >> 8);
	pBuffer->buffer[1] = (uint8)len;
	memcpy(pBuffer->buffer + 2, ptr, len);
	m_sendTail = next;
	m_sendPV.V();
#else
	// Wait for send buffer available
	m_sendBufferSema.Wait();
	
	// if (m_pending) return -1;

	memcpy(m_sendBuffer + 2, ptr, len);
	m_sendBuffer[0] = len >> 8;
	m_sendBuffer[1] = (uint8)(len & 0xff);
	m_ctrl2 = ctrl2;
	m_len = len + 2;
#endif
	K_Event_Signal(0, m_taskSlot, DATA_REQUEST);
	return 1;
}

void CPTPLink::DataRequest()
{	
	if (m_pending) {
		// Current one still pending
		return;
	}
	//SendData(m_sendBuffer, m_len, m_ctrl2);
	if (m_sendHead == m_sendTail) return;
	PTPSendBuffer *pBuffer = (PTPSendBuffer *)(m_sendList[m_sendHead]->GetBuffer());
	SendData((char *)(pBuffer->buffer), pBuffer->len, pBuffer->ctrl2);
	m_pending = 1;
}

// Print the status of the link for debug purpose
void CPTPLink::PrintPtpStatus(Ccomm *pComm, int option)
{
	char buff[180];
	int i;
	int used, available;

	sprintf(buff, "\r\nLink %s: %s SSR=%d ASR=%d\r\n"
		"Link Status: %d\r\n"
		, m_name, (m_state == CONN_STATE) ? ("CONN") : ("DISC"), m_SSR, m_ASR
		, m_linkUpCnt);
	pComm->Send(buff);

	if (option == 1) {
		pComm->Send("Recv buffer allocation:\r\n");
		m_pRecvMemHeap->PrintStatus(pComm);
		m_pRecvMemHeap->GetUsage(&used, &available);
		sprintf(buff, "Total Block used=%d, avail=%d Repeat=%ld\r\n",
			used, available, m_recvRepeatCnt);
		pComm->Send(buff);
		return;
	}
	if (option == 2) {
		pComm->Send("Send Buffer allocation:\r\n");
		m_pSendMemHeap->PrintStatus(pComm);
		m_pSendMemHeap->GetUsage(&used, &available);
		sprintf(buff, "Total Block used=%d, avail=%d\r\n",
			used, available);
		pComm->Send(buff);
		return;
	}

	if (option == 3) {
		sprintf(buff, "Client list (max=%d):\r\n", NUM_CLIENT);
		pComm->Send(buff);	
		for (i = 0; i < m_clientCnt; i++) {
			CBaseProcess *pProcess;
			uint8 id;
			id = m_clientList[i].client_id;
			pProcess = m_clientList[i].pClient;
			sprintf(buff, "Listen on port 0x%02X by %s\r\n", 
				id, pProcess->GetTaskName());
			pComm->Send(buff);
		}
		return;
	}

	if (option == 4) {
		sprintf(buff, "Repetition Recved=%ld CmdErrorCnt=%ld\r\n"
			"NAK sent=%ld  NAK recved=%ld\r\n"
			"Disc Cnt=%ld  Timeout Resend=%ld\r\n"
			"Pending=%d\r\n"
			, m_recvRepeatCnt, m_cmdErrorCnt
			, m_nakSentCnt, m_nakRecvCnt
			, m_discCnt, m_timeoutResendCnt
			, m_pending
			);
		pComm->Send(buff);
	}

	if (option == 5) {
		const char *pHelp = "More Help:\r\n"
#if DEBUG_LAST_MESSAGE
			"option 6: Print Last message info\r\n"
#endif
			;
		pComm->Send(pHelp);
	}

#if DEBUG_LAST_MESSAGE
	if (option == 6) {
		sprintf(buff, "Last Message @0x%lX Size=%d Port=0x%X SSR=%d ASR=%d\r\n",
			m_lastMessage, m_lastSize, m_lastPort, m_lastSSR, m_lastASR);
		pComm->Send(buff);
	}
#endif
}

void CPTPLink::SetupClient(uint8 client_id, CBaseProcess *pClient)
{
	int pos = m_clientCnt, i;
	for (i = 0; i < m_clientCnt; i++) {
		if (m_clientList[i].client_id > client_id) {
			pos = i;
			break;
		}
	}
	for (i = NUM_CLIENT - 1; i > pos; i--) {
		m_clientList[i] = m_clientList[i-1];
	}
	m_clientList[pos].client_id = client_id;
	m_clientList[pos].pClient = pClient;
	m_clientCnt++;
}

CBaseProcess * CPTPLink::FindClient(uint8 client_id)
{
	int start = 0, end = m_clientCnt -1;
	int next;
	uint8 id;
	do {
		next = (start + end) >> 1;
		id = m_clientList[next].client_id;
		if ( id == client_id) {
			// Found
			return m_clientList[next].pClient;
		} else if ( id > client_id) {
			end = next - 1;
		} else if ( id < client_id) {
			start = next + 1;
		}
	} while (start <= end);
	return NULL;
}

void CPTPLink::SetEvent(int evt)
{
	if (m_discEvent && (evt == 0)) {
		// Clear events
		if (m_discEventAsserted) {
			// Clear it.
			CAllProcess::g_pAllProcess->ReportEvent(m_discEvent, 0);
			m_discEventAsserted = 0;
		}
	} else if ((!m_discEvent) && evt) {
		if (m_state == DISC_STATE) {
			CAllProcess::g_pAllProcess->ReportEvent(evt, 1);
			m_discEventAsserted = 1;
		}
	}

	m_discEvent = evt; 
}

// counting of up/down time in 0.5 second
int CPTPLink::GetLinkStatus()
{
	return m_linkUpCnt; // counting of half second
}

void CPTPLink::GetSendUsage(int *pUsed, int *pAvail)
{
	m_pSendMemHeap->GetUsage(pUsed, pAvail);
}


int CPTPLink::GetBusySendingBufferCnt(int port)
{
	return m_pSendMemHeap->GetUsagePerPort(port);
}
