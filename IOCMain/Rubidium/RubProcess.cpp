// RubProcess.cpp: implementation of the CRubProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * 
 * Author: Zheng Miao
 * All rights reserved.
 *
 * $Header: RubProcess.cpp 1.2 2009/05/05 13:33:00PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */
#include "iodefine.h"     
#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "CxFuncs.h"
#include "RubProcess.h"
#include "stdio.h"
#include "sci.h"
#include "AllProcess.h"
#include "machine.h"
#include "PTPLink.h"
#include "MemBlock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRubProcess *CRubProcess::s_pRubProcess = NULL;

CRubProcess::CRubProcess(CPTPLink *pIMCLink, CSci *pSci, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry /*= Entry*/)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_nIMCCmdCnt = 0L;
	m_nDbgCmdCnt = 0L;
	m_nInternalCmdCnt = 0L;

	m_pIMCLink = pIMCLink;
	m_baudrateSettled = 0;
	if (s_pRubProcess == NULL) {
		s_pRubProcess = this;
	}
	m_pSciPort = pSci;

	// Init cmd buffer
	m_cmdHead = m_cmdTail = 0;
	m_lastCmd = -1; // No last cmd

	// Init response
	m_responseIndex = 0;

	SetDebugGroup(27);

	m_imcCmdBox.SetMailEvent(m_taskSlot, EVT_IMC_CMD);
	m_pIMCLink->SetupClient(LINK_RUB_IMC_IOC, this);

}

CRubProcess::~CRubProcess()
{

}

void CRubProcess::Init(void)
{
}

void CRubProcess::Entry()
{
	s_pRubProcess->entry();
}

// Called from SCI ISR driver.
int CRubProcess::Notify(uint32 from, uint32 what)
{
	if ((uint32)m_pSciPort == from) {
		K_Intrp_Event_Signal(0, m_taskSlot, EVT_SERIAL_INPUT);
		return 0;
	} else if (((uint32)m_pIMCLink) == from) {
		int ret;
		ret = m_imcCmdBox.SendMail((char *)what);
		if (ret <= 0) return 0;
		return 1;
	}
}

void CRubProcess::entry()
{
	// m_pSciPort->Send("Hello from Rub process\r\n");
	m_pSciPort->SetBaudRate(DEFAULT_BAUD1);
	m_pSciPort->SetSciNotification(this, (uint32)m_pSciPort);

	// Detect baudrate
#if 1
	for (int i = 0; i < 5; i++) {
		AppendRubCmdFromSelf("\r");
		AppendRubCmdFromSelf("i");
	}
#endif

	// Setup init process to rubidium
	AppendRubCmdFromSelf("qef\r"); // only enable ACOMS output
	AppendRubCmdFromSelf("o6\r");

	Pause(TICK_1SEC * 2);
	EventEntry(TICK_1SEC >> 1);
}

void CRubProcess::EventDispatch(uint16 event)
{
	int ret;
	if (event & EVT_SERIAL_INPUT) {
		ret = DealResponse();
		if (ret > 0) {
			// Skip output request. Since it's dealing with response
			event &= ~(uint16)EVT_SERIAL_OUTPUT_REQUEST;
		}
	}
	if (event & EVT_SERIAL_OUTPUT_REQUEST) {
		DealOutputRequest();
	}
	
	if (event & EVT_IMC_CMD) {
		DealIMCRequest();
	}
}

// Send to Rubidium
int CRubProcess::DealOutputRequest()
{
	if (m_cmdHead == m_cmdTail) return 0; // no request. False alarm
	if (m_lastCmd >= 0) {
		// Command pending
		return 0;
	}
	m_pSciPort->Send(m_rubCmdSource[m_cmdHead].cmdBuff, m_rubCmdSource[m_cmdHead].len/*m_rubCmdList[m_cmdHead]*/);
	m_lastCmd = m_cmdHead++;
	m_cmdHead %= NUM_RUB_CMD;
	// Pause(TICK_1SEC); // test code
}

// receive from rubdium
int CRubProcess::DealResponse()
{
	int ret;
	int unexpected = 0;
	char *pReason;
	if (m_lastCmd < 0) {
		unexpected = 1;
		pReason = "Unexpected response from Rub OSC:\r\n{";
		m_responseIndex = 0; 
	} else if (m_responseIndex >= RUB_RESPONSE_SIZE) {
		pReason = "Receive buffer overflow:\r\n{";
		unexpected = 1;
	}

	if (unexpected) {
		//No command pending. Weird or delay response
		// Get all response 
		ret = m_pSciPort->Receive(m_responseBuffer, RUB_RESPONSE_SIZE, 0L);
		if (ret < m_responseIndex) {
			ret = m_responseIndex;
		}
		m_responseBuffer[ret] = 0;

		// Report to debug port
		PrintDebugOutput(pReason);
		PrintDebugOutput(m_responseBuffer);
		PrintDebugOutput("}\r\n");
		m_responseIndex = 0;
		return 0;
	}
	
	ret = m_pSciPort->Receive(m_responseBuffer + m_responseIndex,
		RUB_RESPONSE_SIZE - m_responseIndex, 0L);
	m_responseIndex += ret;
	return ret;
}

void CRubProcess::EventTimeOut()
{
	// See if response complete 
	if (m_lastCmd >= 0) {
		// complete
		m_responseBuffer[m_responseIndex] = 0;
		switch (m_rubCmdSource[m_lastCmd].source) {
		case CMD_SOURCE_DEBUG_PORT:
			ReportToDebugPort();
			break;
		case CMD_SOURCE_IMC:
			ReportToIMC();
			break;
		case CMD_SOURCE_SELF:
			ReportToSelf();
			break;
		}
		// delete command
		m_lastCmd = -1;
		// delete response buffer
		m_responseIndex = 0;
	} else {
		DealOutputRequest();
	}
}

// Report the response back to debug port
void CRubProcess::ReportToDebugPort()
{
	m_responseBuffer[m_responseIndex] = 0;
	CAllProcess::g_pAllProcess->Print(m_responseBuffer);
#if 1 // debug info
	{
		char buff[100];
		sprintf(buff, "\r\nbuff: 0x@%08lX size =%d\r\n", m_responseBuffer, m_responseIndex);
		CAllProcess::g_pAllProcess->Print(buff);
	}
#endif
}

// report the resposne to IMC
void CRubProcess::ReportToIMC()
{
	SRubResp resp;
	int len = m_responseIndex + 1;
	m_responseBuffer[m_responseIndex] = 0;
	resp.cmd = PTP_CMD_RUB_X72;
	resp.handle = m_rubCmdSource[m_lastCmd].handle;
	resp.error_code = 1;
	resp.len = len;
	if (len > RUB_RESPONSE_SIZE) return; // error, skip it. 
	memcpy(resp.buffer, m_responseBuffer, len);
	m_pIMCLink->PostFrame((char *)&resp, len + 10/*sizeof(resp)*/, LINK_RUB_IOC_IMC);
}

// Get response and consume myself
void CRubProcess::ReportToSelf()
{
	long rate;
	// m_lastCmd
	// Look at original command
	if ((m_baudrateSettled == 0) && (strcmp(m_rubCmdSource[m_lastCmd].cmdBuff/*m_rubCmdList[m_lastCmd]*/, "i") == 0)) {
		int baudrateOK = 1;
		// This is the command to sync up baudrate
		if (m_responseIndex < 300) {
			baudrateOK = 0;
		} else if (strstr(m_responseBuffer, "Symmetricom") == NULL) {
			baudrateOK = 0;
		}
		if (baudrateOK) {
			m_baudrateSettled = 1;
			rate = m_pSciPort->GetBaudRate();
			PrintDebugOutput("Rubidium Baudrate found: %ld\r\n", rate);
		} else {
			// Change baudrate
			rate = m_pSciPort->GetBaudRate();
			rate = (rate == 9600L) ? (57600L) : (9600L);
			m_pSciPort->SetBaudRate(rate);
			PrintDebugOutput("Rubidium Baudrate changed to %ld\r\n", rate); 
			Pause(TICK_1SEC * 3);
		}
#if 1
		PrintDebugOutput("Response to self: len:%d Settled:%d Cmd Left:%d\r\n",
			m_responseIndex, m_baudrateSettled, (m_cmdTail - m_cmdHead + NUM_RUB_CMD) % NUM_RUB_CMD);
		PrintDebugOutput(m_responseBuffer);
#endif
	}

}

// return >= 0: index of item added
// return -1: full. Too many command
// return -2: Command too long
int CRubProcess::AppendCmd(char *pCmd, int len, int *pNext)
{
	uint8 tmp;          
	int volatile ret;
	int next;
	if (len >= RUB_CMD_SIZE) return -2;
	tmp = get_exr();
	set_exr(7);
	next = (m_cmdTail + 1) % NUM_RUB_CMD;
	if (next == m_cmdHead) {
		set_exr(tmp);
		return -1; // full
	}
	m_rubCmdSource[m_cmdTail].len = len;
	memcpy(m_rubCmdSource[m_cmdTail].cmdBuff, pCmd, len);
	ret = m_cmdTail;
	// m_cmdTail = next;
	*pNext = next;
	set_exr(tmp);
	//SignalEvent(EVT_SERIAL_OUTPUT_REQUEST);
	return ret;
}


void CRubProcess::DealIMCRequest()
{
	int ret;
	int cnt = 0;
	while (1) {
		// retrieve reqeust from IMC.
		CMemBlock *pBlock;
		SRubCmd *pIMCCmd;

		pBlock = (CMemBlock *)(m_imcCmdBox.ReceiveMail());
		if (pBlock == NULL) break; // no more request.
		pIMCCmd = (SRubCmd *)pBlock->GetBuffer();
		if (pIMCCmd->cmd == PTP_CMD_RUB_X72) {
			int len;
			int next;
			len = pIMCCmd->len; // pBlock->GetCurSize() - 6;
			ret = AppendCmd(pIMCCmd->buffer, len, &next);
			if (ret >= 0) {
				m_rubCmdSource[ret].source = CMD_SOURCE_IMC;
				m_rubCmdSource[ret].handle = pIMCCmd->handle;
				m_cmdTail = next;
				cnt++;
				m_nIMCCmdCnt++;
			}
		}
		pBlock->Release();
	}
	if (cnt > 0) {
		SignalEvent(EVT_SERIAL_OUTPUT_REQUEST);
	}
}

// Called by debug port to add a command to Rub OSC
// return >=0: OK
// return -1: full. Cannot add more command
// return -2: Command too long.
int CRubProcess::AppendRubCmdFromDebugPort(char *pCmd)
{
	int ret;
	int next;
	ret = AppendCmd(pCmd, strlen(pCmd), &next);
	if (ret < 0) return ret;
	m_nDbgCmdCnt++;
	m_rubCmdSource[ret].source = CMD_SOURCE_DEBUG_PORT;
	m_cmdTail = next;
	SignalEvent(EVT_SERIAL_OUTPUT_REQUEST);
#if 0
	uint8 tmp;
	int next;
	int ret;

	if (strlen(pCmd) >= RUB_CMD_SIZE) return -2;
	tmp = get_exr();
	set_exr(7);
	next = (m_cmdTail + 1) % NUM_RUB_CMD;
	if (next == m_cmdHead) {
		set_exr(tmp);
		return -1; // Full
	}
	m_rubCmdSource[m_cmdTail].source = CMD_SOURCE_DEBUG_PORT;
	strcpy(m_rubCmdSource[m_cmdTail].cmdBuff/*m_rubCmdList[m_cmdTail]*/, pCmd);   
	m_rubCmdSource[m_cmdTail].len = strlen(pCmd);
	ret = m_cmdTail;
	m_cmdTail = next;
	set_exr(tmp);
	SignalEvent(EVT_SERIAL_OUTPUT_REQUEST);
	return ret;
#endif
}

int CRubProcess::AppendRubCmdFromSelf(char *pCmd)
{
	int ret;
	int next;
	ret = AppendCmd(pCmd, strlen(pCmd), &next);
	if (ret < 0) return ret;
	m_nInternalCmdCnt++;
	m_rubCmdSource[ret].source = CMD_SOURCE_SELF;
	m_cmdTail = next;
	SignalEvent(EVT_SERIAL_OUTPUT_REQUEST);
#if 0
	int ret;

	ret = AppendRubCmdFromDebugPort(pCmd);
	if (ret < 0) return ret;
	m_rubCmdSource[ret].source = CMD_SOURCE_SELF;
	return ret;
#endif
}


void CRubProcess::PrintRubStatus(Ccomm *pComm)
{
	char buff[100];
	long rate;

	rate = m_pSciPort->GetBaudRate();
	sprintf(buff, "Rubidium baudrate: %ld, Settled: %d\r\nRequest pending: %d\r\n",
		rate, m_baudrateSettled, (m_cmdTail - m_cmdHead + NUM_RUB_CMD) % NUM_RUB_CMD);
	pComm->Send(buff);
	sprintf(buff, "Cmd Cnt: IMC=%ld\t Debug=%ld\t Internal=%ld\r\n",
		m_nIMCCmdCnt, m_nDbgCmdCnt, m_nInternalCmdCnt);
	pComm->Send(buff);

}

