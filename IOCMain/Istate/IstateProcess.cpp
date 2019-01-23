// IstateProcess.cpp: implementation of the CIstateProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateProcess.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateProcess.cpp 1.4 2009/05/05 13:36:58PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.4 $
 */

#include "CodeOpt.h"
#include "Constdef.h"
#include "DataType.h"
#include "iodefine.h"     
#include "version.h"
#include "string.h"
#include "IstateProcess.h"
#include "Resource.h"
#include "MemBlock.h"
#include "cxfuncs.h"
#include "PTPLink.h"
#include "sci.h"
#include "stdio.h"
#include "AllProcess.h"
#include "Istate.h"
#include "IstateKb.h"
#include "IstateExec.h"
#include "NVRAM.h"
#include "IstateAccess.h"
#include "errorcode.h"
#include "comm.h"
#include "Factory.h"  
#include "IstateCheckSum.h"
#include "IOCEvtList.h"
#include "IstateBasic.h"
#include "IstateKbIoc.h"
#include "TwinProxy.h"
#include "IOCKeyList.h"  
#if DTI_ISTATE
#include "IstateDti.h"
#endif
#include "CardStatus.h"
#include "InputTod.h"

#define TRACE TRACE_ISTATE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIstateProcess *CIstateProcess::s_pIstateProcess = NULL;

CIstateProcess::CIstateProcess( // COutput **pOutputs, CFPGA *pFPGA, CCardState *pCardState,
	CPTPLink *pIMCLink, CPTPLink *pIOCLink,
	CNVRAM *pNVRAM,
	const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
:CBaseProcess(name, priority, stack_size, entry),
m_queue(sizeof(SWriteIstateItem), 160)
{
#if 0
	int i;

	for (i = 0; i < COutput::OUTPUT_BANK_NUM; i++) {
		m_pOutputs[i] = new COutput(pFPGA, i);
	}
#endif
	m_houseKeepingCnt = 0L;
	m_busyCnt = 0;
	m_iocLinkStatus = 0;
	//m_compatible = 1;
	//m_compatibleCheckTimer = 0;
	// m_pFPGA = pFPGA;

	// m_pOutputs = pOutputs;

	m_pIMCLink = pIMCLink;
	m_pIOCLink = pIOCLink;
	m_pNVRAM = pNVRAM;
	
	new CTwinProxy(pIOCLink, LINK_IOCISTATE_IOC_IOC);

	if (s_pIstateProcess == NULL) {
		s_pIstateProcess = this;
	}

	// Get mail box
	//m_iocBox = GetMailBoxAssignment();
	//m_imcBox = GetMailBoxAssignment();
	m_iocBox.SetMailEvent(m_taskSlot, EVENT_IOC_LINK);
	m_imcBox.SetMailEvent(m_taskSlot, EVENT_IMC_LINK);

	// To be a client of the RTE/RTE link
	if (m_pIOCLink != NULL) {
		m_pIOCLink->SetupClient(LINK_IOCISTATE_IOC_IOC, this);
#if 0 // Take out PPC Istate from RTE
		m_pIOCLink->SetupClient(LINK_IMCISTATE_IOC_IOC, this);
#endif
	}

#if 0 // Take out PPC Istate from RTE
	if (m_pIMCLink != NULL) {
		m_pIMCLink->SetupClient(LINK_IOCISTATE_IMC_IOC, this);
		m_pIMCLink->SetupClient(LINK_IMCISTATE_IMC_IOC, this);
	}
#endif

	// Setup to receive Istate access from PPC
	if (m_pIMCLink != NULL) {
		m_pIMCLink->SetupClient(LINK_IOCISTATE_ACCESS, this);
	}

	// Init RTE Istate //////////////////////////////////////////////////
		// Create knowledge base of RTE istate
	CIstateKbIoc *pIOCKB;
	m_pIOCKB = pIOCKB = new CIstateKbIoc(IOC_ISTATE_VERSION, COMM_RTE_KEY_START, COMM_RTE_KEY_END); //new CIstateKBIOC(m_pOutputs, pFPGA, this);
	new CIstateBasic(pIOCKB);
#if DTI_ISTATE
	new CIstateDti(pIOCKB);
#endif

		// Create RTE istate
	m_pIOCIstate = new CIstate(this, pIOCKB, m_pNVRAM, "RTE-ISTATE");
		// Create RTE Istate exec
	m_pIOCIstateExec = new CIstateExec( // pFPGA, pCardState, 
		m_pIOCIstate, // m_pIMCLink, LINK_IOCISTATE_IMC_IOC, // Keep only RTE-Istate in RTE
		m_pIOCLink, LINK_IOCISTATE_IOC_IOC);
	m_pIocCheckSum = m_pIOCIstateExec->GetCheckSum();

    //////////////////////////////////////////////////////////////////////

	// Init PPC Istate ///////////////////////////////////////////////////
#if 0 // Take out PPC Istate from RTE
	CIstateKb *pIMCKB = new CIstateKBIMC();
		// Create PPC Istate 
	m_pIMCIstate = new CIstate(this, pIMCKB, m_pNVRAM, "PPC-ISTATE");
		// Create PPC Istate Exec
	m_pIMCIstateExec = new CIstateExec(pCardState, 
		m_pIMCIstate, m_pIMCLink, LINK_IMCISTATE_IMC_IOC,
		m_pIOCLink, LINK_IMCISTATE_IOC_IOC);
#endif
	//////////////////////////////////////////////////////////////////////

	m_pImcAccess = new CIstateAccess(this, /*pCardState,*/ m_pIOCIstate, m_pIMCLink);

	ClearTwinVersions();
}

CIstateProcess::~CIstateProcess()
{

}

int CIstateProcess::StartProcess()
{	
	m_pIOCIstate->InitIstate();
#if 0 // Take out PPC Istate from RTE
	m_pIMCIstate->InitIstate();
#endif
	return CBaseProcess::StartProcess();
}

void CIstateProcess::Entry()
{
	s_pIstateProcess->entry();
}

void CIstateProcess::entry()
{
	// Setup mail box to have event association
	//K_Mbox_Event_Set(m_iocBox, m_taskSlot, EVENT_IOC_LINK);
	//K_Mbox_Event_Set(m_imcBox, m_taskSlot, EVENT_IMC_LINK);
	
	Pause(TICK_1SEC * 5); // Wait for card state machine to be up

	WaitIocLink();
	m_pIOCIstateExec->TickPerSecond(); // Send out check sum immediately
	SET_MY_ISTATE(COMM_RTE_KEY_THIS_CLOCK_TYPE, 0, 0, CFactory::s_pFactory->GetClockType());
	SET_TWIN_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_TWIN_BOOTUP);
	EventEntry();
}

// Route the message to the right istate exec
void CIstateProcess::EventDispatch(uint16 event)
{
	CMemBlock *pBlock;
    uint16 len  = 0;
	const uint8 *ptr = NULL;
	int ret;
    uint8 ctrl2 = 0XFF;
	int16 curCmd;

	while (EVENT_IOC_LINK & event) {
		// pBlock = (CMemBlock *)K_Mesg_Get(m_iocBox);
		pBlock = (CMemBlock *)(m_iocBox.ReceiveMail());
		if (pBlock != NULL) {
			ctrl2 = pBlock->GetCtrl2();
			len = pBlock->GetCurSize();
			ptr = pBlock->GetBuffer();
			curCmd = *((int16 *)ptr);
		} else {
			break; // No more mail
		}
		if (ctrl2 == LINK_IOCISTATE_IOC_IOC) {
			ret = m_pIOCIstateExec->DispatchMessageFromIOC(ptr, len);
			if (ret < 0) {
				// it could be process level message.
				switch (curCmd) {
				case CMD_TWIN_WRITE_NO_RESPONSE:
					ret = ReceiveTwinMessage(ptr, len);
					break;
				case CMD_TWIN_SEND_VERSIONS:
					ret = ReceiveIocVersions(ptr, len);
					break;
				}
			}
			if (ret < 0) {
				ret = CTwinProxy::s_pTwinProxy->ReceiveProxy((const char *)ptr, len);
				if ((ret == 10 ) && (IS_ACTIVE)) {
					CTwinProxy::s_pTwinProxy->SendShelfLife();
				}
			}
			if (ret < 0) {
				// Don't know this command. Dump it out on debug port
				CAllProcess::g_pAllProcess->DumpHex("RTE Istate recv from RTE: ", ptr, len);
			}

			
		} else if (ctrl2 != 0xff) {
			TRACE("Ctrl2=%02X\r\n", ctrl2);
			CAllProcess::g_pAllProcess->DumpHex("Istate recv from RTE: ", ptr, len);			
		}
		if (pBlock != NULL) {
			pBlock->Release();
		}
	}

	if (EVENT_ISTATE_WRITE_REQUEST & event) { // if busy on PPC/RTE link, hold on writing.
		ExecuteWritingRequest();
	}

	while (EVENT_IMC_LINK & event) {
		// pBlock = (CMemBlock *)K_Mesg_Get(m_imcBox);
		pBlock = (CMemBlock *)m_imcBox.ReceiveMail();
		if (pBlock != NULL) {
			ctrl2 = pBlock->GetCtrl2();
			len = pBlock->GetCurSize();
			ptr = pBlock->GetBuffer();
		} else {
			break; // No more mail
		}
		
		if (ctrl2 == LINK_IOCISTATE_ACCESS) {
			m_pImcAccess->ExecuteImcAccess((char const *)ptr, len);
#if 0 // Take out PPC Istate from RTE
		} else if (ctrl2 == LINK_IOCISTATE_IMC_IOC) {
			ret = m_pIOCIstateExec->DispatchMessageFromIMC(ptr, len);
//#if DEBUG_ISTATE
			if (ret < 0) {
				// Don't know this command. Dump it out on debug port
				CAllProcess::g_pAllProcess->DumpHex("RTE Istate recv from PPC: ", ptr, len);
			}
//#endif		
		} else if (ctrl2 == LINK_IMCISTATE_IMC_IOC) {
			ret = m_pIMCIstateExec->DispatchMessageFromIMC(ptr, len);
//#if DEBUG_ISTATE
			if (ret < 0) {
				// Don't know this command. Dump it out on debug port
				CAllProcess::g_pAllProcess->DumpHex("PPC Istate recv from PPC: ", ptr, len);
			}
//#endif					
#endif // #if 0

		} else if (ctrl2 != 0xff) {

			TRACE("Ctrl2=%02X\r\n", ctrl2);
			CAllProcess::g_pAllProcess->DumpHex("Istate recv from PPC: ", ptr, len);							
		}

		if (pBlock != NULL) {
			pBlock->Release();
		}
	}

	if (EVENT_HOUSE_KEEPING & event) {
		if (m_busyCnt > 2) {
			m_houseKeepingCnt++;
			HouseKeeping();
		}
	}
}

void CIstateProcess::EventTimeOut()
{

	m_busyCnt = 0;

	m_pIOCIstateExec->TickPerSecond();

#if 0 // Take out PPC Istate from RTE
	m_pIMCIstateExec->TickPerSecond();
#endif

#if 0
	// Test code
	{
		static int cnt = 0;
		if (cnt++ >= 3) {
			cnt = 0;
			m_pIOCLink->PostFrame("9876543210\r\n", 12, LINK_IOCISTATE_IOC_IOC);
		}
	}
#endif
	{
		int num;
		num = m_queue.IsFilled();
		if (num) {
			SignalEvent(EVENT_ISTATE_WRITE_REQUEST);
		}
	}

	HouseKeeping();
}

void CIstateProcess::HouseKeeping()
{
	int iocLink;

	iocLink = m_pIOCLink->GetLinkStatus();

	if (iocLink < 10) { // Used to be -10. To handle with a twin with only boot code, the detection of reboot is not easy.
		if (m_iocLinkStatus < -20000) {
			m_iocLinkStatus = -10000;
		} else if (m_iocLinkStatus >= 0) {
			m_iocLinkStatus = -1;
		} else {
			m_iocLinkStatus--;
		}
		IocLinkDown(m_iocLinkStatus);
	} else if (iocLink >= 15) {
		if (m_iocLinkStatus > 20000) {
			m_iocLinkStatus = 10000;
		} else if (m_iocLinkStatus <= 0) {
			m_iocLinkStatus = 1;
		} else {
			m_iocLinkStatus++;
		}
		IocLinkUp(m_iocLinkStatus);
	} else {
		m_iocLinkStatus = 0;
	}
}

int CIstateProcess::Notify(uint32 from, uint32 what)
{
	// Notification could be from PPC link or RTE link
	CPTPLink *pLink = (CPTPLink *)from;
	CMemBlock *pBlock = (CMemBlock *)what;
	int ret;

	// From nowhere
	if (pLink == NULL) return 0;

	if (pLink == m_pIOCLink) {
		//ret = K_Mesg_Send(m_iocBox, pBlock);
		ret = m_iocBox.SendMail(pBlock);
		if (ret != 1/* K_OK */) {
			pBlock->Release();
		}
		return 1;
	} else if (pLink == m_pIMCLink) {
		//ret = K_Mesg_Send(m_imcBox, pBlock);
		ret = m_imcBox.SendMail(pBlock);
		if (ret != 1/*K_OK*/) {
			pBlock->Release();
		}
		return 1;
	}
	return 0;

}

CIstate * CIstateProcess::GetIOCIstate()
{
	return m_pIOCIstate;
}

#if 0
CIstate * CIstateProcess::GetIMCIstate(void)
{
	return m_pIMCIstate;
}
#endif

//1: For OK
// 2 OK_IOC_ISTATE_NO_CHANGE: Nothing changed.
// -20 ERROR_ISTATE_PROCESS_BUSY: queue full.
// Other negative possibly from CheckWritingIstateItem
int CIstateProcess::WriteOneIOCIstateItem(int16 item, int16 index1, int16 index2, uint32 value, int16 cmd /*=CMD_WRITE_ISTATE*/)
{
	int ret;
	SWriteIstateItem msg;
	msg.cmd = cmd;
	msg.item = item;
	msg.index1 = index1;
	msg.index2 = index2;
	msg.value = value;
	ret = m_pIOCIstate->CheckWritingIstateItem(item, index1, index2, value, cmd);
	if (ret != 1) return ret; // Cannot write to Istate
	ret = m_queue.Append((uint8 *)(&msg));
	if (ret < 0) {
		// queue full;
		TRACE("Istate Queue full\r\n");
		return ERROR_ISTATE_PROCESS_BUSY;
	}
	SignalEvent(EVENT_ISTATE_WRITE_REQUEST);
    return 1;
}

void CIstateProcess::ExecuteWritingRequest(void)
{
	SWriteIstateItem msg;
	int ret, changed; 

	while (1) {
		if (m_pIOCIstateExec->IsAvailable() != 1) {
			// Still busy, holds on
			return;	
		}
		ret = m_queue.GetTop((uint8 *)&msg);
		if (ret > 0) {
			// Write to local copy
			changed = m_pIOCIstate->WriteOneIstateItemByIstateProcess(msg.cmd,
				msg.item, msg.index1, msg.index2, msg.value);
			// Return 2 for stand alone Istate.
			// Relay to twin RTE
			if (changed & 1) {
				m_pIOCIstateExec->RelayItemToTwinIOC(msg.item,
					msg.index1, msg.index2);
			}

			if (changed & 4) {
				// reset the istate
				m_pIOCKB->ResetIstateValue(msg.item);
			}

			// Copy it to PPC if desirable.		
		} else {
			break;
		}
	}
}

void CIstateProcess::ReportProcessIdle()
{
	// When Block/Item transceiver gets idle, Check the queue again.
	SignalEvent(EVENT_ISTATE_WRITE_REQUEST);
}

void CIstateProcess::ResendIOCIstateToTwin()
{
	m_pIOCIstateExec->ResendIstateToTwin();
}

void CIstateProcess::PrintIstateStatus(Ccomm *pComm)
{
	{
		char buff[200];
		sprintf(buff, "Action Cnt: %d HouseKeeping: %ld\r\n"
			"RTE Link Status: %d\r\n" 
			, m_busyCnt, m_houseKeepingCnt
			, m_iocLinkStatus);
		pComm->Send(buff);
		sprintf(buff, "Twin Firmware version: 0x%lX\r\n"
			"Twin Hardware Rev: %s\r\n"
			"Twin Part Number: %s\r\n",
			m_twinFirmwareVersion,
			m_twinHardwareVersion,
			m_twinPartNumber);
		pComm->Send(buff);
	}
	pComm->Send("Istate Exec status:------------------------\r\n");
	m_pIOCIstateExec->PrintIstateExec(pComm);
	
}

void CIstateProcess::Init()
{
	CIstateBasic::s_pIstateBasic->Init();
#if DTI_ISTATE
	CIstateDti::s_pIstateDti->Init();
#endif
	m_pIOCIstate->AddToNvram();
	m_pIOCIstateExec->Init();
	CTwinProxy::s_pTwinProxy->Init();
}

// return > 0: Expected message
// -1: Unexpected message
int CIstateProcess::ReceiveTwinMessage(const uint8 *ptr, uint16 len)
{
	SIOCIstateTwinWrite *pWrite;
	int16 curCmd;
	if (ptr == NULL) return -1;
	pWrite = (SIOCIstateTwinWrite *)ptr;
	curCmd = pWrite->cmd;
	if ((curCmd <= CMD_TWIN_START) || (curCmd >= CMD_TWIN_END)) return -1;
	switch (curCmd) {
	case CMD_TWIN_WRITE_NO_RESPONSE:
		WriteOneIOCIstateItem(pWrite->data.item, pWrite->data.index1, 
			pWrite->data.index2, pWrite->data.value);
		return 1;
	}
	return -1;
}

int CIstateProcess::WriteTwinOneIOCIstateItem(int16 item, int16 index1, int16 index2, uint32 value)
{
	SIOCIstateTwinWrite msg;

	msg.cmd = CMD_TWIN_WRITE_NO_RESPONSE;
	msg.data.cmd = CMD_WRITE_ISTATE; // no necessary. Just put in there in case I need it.
	msg.data.item = item;
	msg.data.index1 = index1;
	msg.data.index2 = index2;
	msg.data.value = value;
	return m_pIOCLink->PostFrame((char *)&msg, sizeof(msg), LINK_IOCISTATE_IOC_IOC);
}


int CIstateProcess::IsTwinSameIstateVersion()
{
	return m_pIOCIstateExec->IsTwinSameIstateVersion();
}

void CIstateProcess::ClearTwinVersions()
{
	m_twinFirmwareVersion = 0;
	memset(m_twinHardwareVersion, 0, sizeof(m_twinHardwareVersion));
	memset(m_twinPartNumber, 0, sizeof(m_twinPartNumber));
}

void CIstateProcess::IocLinkDown(int link)
{
	if (link == -1) {
		ClearTwinVersions();
		CTwinProxy::s_pTwinProxy->TwinOffline();
		TRACE("IstateProcess: RTE comm link is down!\r\n");
	} else if (link > -100) {
		CTwinProxy::s_pTwinProxy->TwinOffline();
		// TRACE("IstateProcess: Twin off line\r\n");
	}
}

void CIstateProcess::IocLinkUp(int link)
{
	if (link == 1) {
		TRACE("IstateProcess: RTE comm link up\r\n");
	}

	switch (link) {
	case 3:
		SendIocVersions();
		CInputTod::ForceChange();
		return;
	case 4:
		CTwinProxy::s_pTwinProxy->SendShelfLife();
		return;
	}
	
	link &= 63;
	switch (link) {
	case 0:
		SendIocVersions();
		return;
	case 1:
		CTwinProxy::s_pTwinProxy->SendShelfLife();
		return;
	}

}

int CIstateProcess::SendIocVersions()
{  
	SIOCVersions msg;
	
	memset(&msg, 0, sizeof(msg));
	msg.cmd = CMD_TWIN_SEND_VERSIONS;
	msg.firmware_version = VERSION_NUMBER;
	strncpy(msg.hardware_version, CFactory::s_pFactory->GetHardwareRev(), MAX_SERIAL_LENGTH);
	strncpy(msg.part_number, CFactory::s_pFactory->GetPartNumber(), MAX_SERIAL_LENGTH);

	return m_pIOCLink->PostFrame((char *)&msg, sizeof(msg), LINK_IOCISTATE_IOC_IOC);
}

int CIstateProcess::ReceiveIocVersions(const uint8 *ptr, uint16 len)
{     
	SIOCVersions *pMsg;

	if (len != sizeof(SIOCVersions)) return -1;
	pMsg = (SIOCVersions *)ptr;
	if (pMsg->cmd != CMD_TWIN_SEND_VERSIONS) return -1;
	m_twinFirmwareVersion = pMsg->firmware_version;
	strncpy(m_twinHardwareVersion, pMsg->hardware_version, MAX_SERIAL_LENGTH);
	strncpy(m_twinPartNumber, pMsg->part_number, MAX_SERIAL_LENGTH);
	return 0;
}

void CIstateProcess::TickIstateProcess()
{
	m_busyCnt++;
	if (m_busyCnt > 2) {
		SignalEvent(EVENT_HOUSE_KEEPING);
	}
}

void CIstateProcess::IstateHack(unsigned long *param, Ccomm *pComm)
{
	const char * pHelp;
	char buff[200];
	uint16 size;
	uint16 version;
	uint8 *location;


	pHelp = "ISTATE option: \r\n"
			"\t1: List RTE Istate value\r\n"
			"\t3 <id>: Read single RTE Istate value\r\n"
			"\t5 <id> <index1> <index2> <value>: Write single RTE Istate value\r\n"
			"\t7 : Print Istate status\r\n"
			"\t8(0) <id> <index1> <index2> <value>: Write single RTE istate value to Twin RTE. 80: Send 100 times\r\n"
			"\t9 : Print Istate process debug\r\n"
			;
	if (*param == 0) {
		location = m_pIOCKB->GetIstateLocation(&size, &version);
		sprintf(buff, "RTE Istate version: %04X size=%d @%08lX\r\n",
			version, size, location);
		pComm->Send(buff);
		pComm->Send(pHelp);
		return;
	}
	if (*param == 1) {
		switch (param[1]) {
		case 1: // Readl Istate List
			m_pIOCIstate->PrintStatus(pComm);
			return;
		case 7:
			PrintIstateStatus(pComm);
			return;
		case 9: {
			int queueSize = m_queue.IsFilled();
			sprintf(buff, "QUEUE Size: %d\r\n", queueSize);
			pComm->Send(buff);
				}
			return;
		}
	}
	if (*param == 2) {
		switch (param[1]) {
		case 3: 
			m_pIOCIstate->PrintIstateItem(pComm, param[2], 1/* with help info */);
			return;
		}
	}
	if (*param == 5) {
		switch (param[1]) {
		case 5:  {
			int ret = WriteOneIOCIstateItem(param[2], param[3], param[4], param[5], CMD_WRITE_ISTATE_FROM_IMC);
			sprintf(buff, "Ret=%d\r\n", ret);
			pComm->Send(buff);
			return;
			}
		case 80:
		case 8: {
			int load1, load2;
			int cnt;
			int i;
			cnt = (param[1] != 8 ) ? (100) : (1);
			load1 = m_pIOCLink->GetBusySendingBufferCnt(LINK_IOCISTATE_IOC_IOC);
			for (i = 0; i < cnt; i++) {
				WriteTwinOneIOCIstateItem(param[2], param[3], param[4], param[5]);
			}
			load2 = m_pIOCLink->GetBusySendingBufferCnt(LINK_IOCISTATE_IOC_IOC);
			sprintf(buff, "Sending buffer %d --> %d\r\n", load1, load2);
			pComm->Send(buff);
			return;
			}
	
		}
	}
	pComm->Send("Invalid Option\r\n");
	pComm->Send(pHelp);
}

// 1: twin active
// 0: twin inactive
// -1: don't know
int CIstateProcess::IsTwinActive()
{
	return m_pIocCheckSum->IsTwinActive();
}

int CIstateProcess::IsMaster()
{
	return m_pIocCheckSum->IsMaster();
}

void CIstateProcess::WaitIocLink()
{
	int max = 5;
	do {
		int stat;
		Pause(TICK_1SEC); // Wait for comm to work
		stat = m_pIOCLink->GetLinkStatus();
		if (stat > 2) break;
	} while (max-- > 0);

}
