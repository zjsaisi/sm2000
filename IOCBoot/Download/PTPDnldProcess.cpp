// PTPDnldProcess.cpp: implementation of the CPTPDnldProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldProcess.cpp 1.1 2007/12/06 11:39:07PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#include "PTPDnldProcess.h"
#include "MemBlock.h"
#include "PTPLink.h"
#include "Storage.h"
#include "Flash.h"
#include "Loader.h"
#include "AllProcess.h"
#include "PTPDnldSend.h"
#include "MailBox.h"
//#include "BlockStorage.h"
#include "comm.h"
//#include "IstateProcess.h"
//#include "IstateParser.h"
#include "stdio.h"
#include "errorcode.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPTPDnldProcess *s_pPTPDnldProcess;

CPTPDnldProcess::CPTPDnldProcess(/* CIstateProcess *pIstateProcess, */
	CLoader *pLoader, CFlash *pFlash, CPTPLink *pIOCLink, 
	CPTPLink *pIMCLink,
	// uint8 inPort, uint8 outPort,
	const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry /*= Entry*/)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_timeoutCnt = 0;
	s_pPTPDnldProcess = this;
	m_pIOCLink = pIOCLink;
	m_pIMCLink = pIMCLink;
	// m_pIstateProcess = pIstateProcess;

	// Init Storage
	m_pStorage = new CCodeStorage(pLoader);
	m_pStorage->Rewind();
	
	m_iocDnldBox.SetMailEvent(m_taskSlot, EVT_IOC_CODE_DNLD);
	m_imcDnldBox.SetMailEvent(m_taskSlot, EVT_IMC_CODE_DNLD);

	// For Receiving code from IOC 
	m_pIOCLink->SetupClient(PTP_PORT_IOC_RECEIVER, this);
	m_pIOCReceiver = new CPTPDnldRecv(m_pStorage, MAX_CODE_SIZE, CMD_CODE_DOWNLOAD,  
		pIOCLink, PTP_PORT_IOC_SENDER);

	// For Sending code to IOC
	m_pIOCLink->SetupClient(PTP_PORT_IOC_SENDER, this);
	m_pIOCSender = new CPTPDnldSend(m_pStorage, CMD_CODE_DOWNLOAD,  
		pIOCLink, PTP_PORT_IOC_SENDER);

	// For Receiving code from IMC
	m_pIMCLink->SetupClient(PTP_PORT_IMC_RECEIVER, this);
	m_pIMCReceiver = new CPTPDnldRecv(m_pStorage, MAX_CODE_SIZE, CMD_CODE_DOWNLOAD,
		m_pIMCLink, PTP_PORT_IMC_CODE_SENDER);

	// To receive firmware version reading from IMC
	m_pIMCLink->SetupClient(PTP_PORT_IOC_IOCISTATE_ACCESS, this);

#if 0
	// For Receiving IMC Istate from IMC
	m_pIMCIstateStorage = new CBlockStorage("IMC Istate", MAX_IMC_ISTATE_SIZE);
	m_pIMCIstateReceiver = new CPTPDnldRecv(m_pIMCIstateStorage, MAX_IMC_ISTATE_SIZE,
		CMD_IMC_ISTATE_DOWNLOAD, m_pIMCLink, PTP_PORT_IMC_IMCISTATE_SENDER);

	// For Receiving IOC Istate from IMC
	m_pIOCIstateStorage = new CBlockStorage("IOC Istate", MAX_IOC_ISTATE_SIZE);
	m_pIOCIstateReceiver = new CPTPDnldRecv(m_pIOCIstateStorage, MAX_IOC_ISTATE_SIZE,
		CMD_IOC_ISTATE_DOWNLOAD, m_pIMCLink, PTP_PORT_IMC_IOCISTATE_SENDER);

	m_pIstateParser = new CIstateParser(m_pIOCIstateStorage, m_pIstateProcess);
#endif	
}

CPTPDnldProcess::~CPTPDnldProcess()
{

}

void CPTPDnldProcess::Entry(void)
{
	s_pPTPDnldProcess->entry();
}

void CPTPDnldProcess::entry()
{
	g_pAll->WaitForChoice();
	EventEntry(TICK_1SEC / 2);
}

void CPTPDnldProcess::EventTimeOut()
{
	m_timeoutCnt++;
	if (m_timeoutCnt >= 4) {
		m_timeoutCnt = 0;
		m_pIOCReceiver->TickTime(2);
		m_pIOCSender->TickTime(2);
		m_pIMCReceiver->TickTime(2);
	}

	if (m_timeoutCnt & 2) {
		*((uint8 volatile *)0x600000) = 0xff;
	} else {
		*((uint8 volatile *)0x600000) = 0;
	}
}

void CPTPDnldProcess::EventDispatch(uint16 event)
{
	CMemBlock *pBlock;
	SCmdGeneral *pCmd;
	int size;

	// Deal code copy with twin IOC
	while (event & EVT_IOC_CODE_DNLD) {
		pBlock = (CMemBlock *)m_iocDnldBox.ReceiveMail();
		if (pBlock != NULL) {
			size = pBlock->GetCurSize();
			if (size > 500) {
				// Must be data packet
				m_pIOCReceiver->ReceivePacket(pBlock);
				pBlock->Release();  
				return;
			} else {
				m_pIOCSender->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			}
		} else {
			break;
		}
	}

	// Deal code copy with IMC
	while (event & EVT_IMC_CODE_DNLD) {
		pBlock = (CMemBlock *)m_imcDnldBox.ReceiveMail();
		if (pBlock != NULL) {
			// Only receive code from IMC.
			pCmd = (SCmdGeneral *)pBlock->GetBuffer();
			switch (pCmd->cmd) {
			case CMD_CODE_DOWNLOAD:
				m_pIMCReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_IOC_ISTATE_READING:
				FirmwareVersionQuery(pBlock);
				pBlock->Release();
				return;
#if 0				
			case CMD_IMC_ISTATE_DOWNLOAD:
				// Receive IMC Istate from IMC
				m_pIMCIstateReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_IOC_ISTATE_DOWNLOAD:
				// Receive IOC Istate from IMC
				m_pIOCIstateReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;    
#endif		
			case CMD_GET_FACTORY:
				VersionStringQuery(pBlock);
				pBlock->Release();
				return;
	
			default:
				pBlock->Release();
				return;
			}
		} else {
			break;
		}
	}

	if (event & EVT_START_SEND_CODE_TO_TWIN_IOC) {
		// Start download code to twin IOC
		m_pIOCSender->StartSendCode();
	}
}

int CPTPDnldProcess::Notify(uint32 from, uint32 what)
{
	CMemBlock *pBlock;
	int ret;
	
	pBlock = (CMemBlock *)what;

	// Receive from IOC
	if (from == (uint32)m_pIOCLink) {
		ret = m_iocDnldBox.SendMail(pBlock);
		if (ret != 1) {
			pBlock->Release();
		}
		return 1;
	}

	// Receive from IMC
	if (from == (uint32)m_pIMCLink) {
		ret = m_imcDnldBox.SendMail(pBlock);
		if (ret != 1) {
			pBlock->Release();
		}
		return 1;
	}

	// shouldn't be here.
	return 0;
}

void CPTPDnldProcess::DebugOutput(const char *str)
{
	g_pAll->DebugOutput(str, -1, (uint32)1 << 23);
}



void CPTPDnldProcess::StartSendCodeToTwinIOC()
{
	SignalEvent(EVT_START_SEND_CODE_TO_TWIN_IOC);
}

#if 0
void CPTPDnldProcess::PrintStatus(Ccomm *pComm, unsigned long *param)
{
	char buff[100];
	int ret;
	if (*param == 0) {
		const char *ptr;
		ptr = "Option list:\r\n\t1: Print IMC Istate in storage\r\n\
\t2: Print IOC Istate in storage\r\n\
\t3: Print IOC Istate in storage as ASCII\r\n\
\t4: Print IOC Istate to Storage\r\n\
\t5: Parse IOC Istate from Storage\r\n\
";
		pComm->Send(ptr);
		return;
	}
	switch (param[1]) {
	case 1:
		s_pPTPDnldProcess->m_pIMCIstateStorage->PrintContent(pComm);
		return;
	case 2: 
		s_pPTPDnldProcess->m_pIOCIstateStorage->PrintContent(pComm);
		return;
	case 3:
		s_pPTPDnldProcess->m_pIOCIstateStorage->PrintContent(pComm, 1);
		return;
	case 4:
		ret = s_pPTPDnldProcess->m_pIstateParser->PrintIstate(); // Convert binary into ASCII
		sprintf(buff, "%d Istate Item(s) converted\r\n", ret);
		pComm->Send(buff);
		return;
	case 5:
		ret = s_pPTPDnldProcess->m_pIstateParser->ReadIstate(); // Convert ASCII Istate to binary
		sprintf(buff, "%d Istate Item(s) Read\r\n", ret);
		pComm->Send(buff);
		return;
	default:
		break;
	}
	pComm->Send("Invalid option\r\n");
}
#endif

void CPTPDnldProcess::FirmwareVersionQuery(CMemBlock *pBlock)
{
	SCmdGetIstate *pCmd;
	SRespGetIstate resp;

	pCmd = (SCmdGetIstate *)pBlock->GetBuffer();
	if (pCmd->cmd != CMD_IOC_ISTATE_READING) {
		return;
	}
	resp.cmd = CMD_IOC_ISTATE_READING;
	resp.error = 1;
	resp.handle = pCmd->handle;
	resp.index1 = 0;
	resp.index2 = 0;
	resp.istate_id = pCmd->istate_id;
	resp.value = 0xff000000 + VERSION_NUMBER;
	if (pCmd->istate_id != 1 /* Firmware version */) {
		resp.error = ERROR_UNKNOWN_IOC_ISTATE_ID;
	} else if (pCmd->index1 != 0) {
		resp.error = ERROR_INDEX1_OOR;
	} else if (pCmd->index2 != 0) {
		resp.error = ERROR_INDEX2_OOR;
	}
	m_pIMCLink->PostFrame((char *)&resp, sizeof(resp), PTP_PORT_IMC_IOCISTATE_ACCESS); 
}

#define ERROR_FACTORY_UNKNOWN_ID   -141
void CPTPDnldProcess::VersionStringQuery(CMemBlock *pBlock)
{
	SCmdReadFactory *pCmd;
	SRespReadFactory resp;
	int16 id;

	pCmd = (SCmdReadFactory *)pBlock->GetBuffer();
	if (pCmd->cmd != CMD_GET_FACTORY) {
		return;
	}

	id = pCmd->id;
	memset(&resp, 0, sizeof(resp));
	resp.cmd = CMD_GET_FACTORY;
	resp.error_code = 1;
	resp.handle = pCmd->handle;

	switch (id) {
	case FACTORY_ID_VERSION_STRING:
		sprintf(resp.value, "BOOT%d.%02d.%02d",
			(int)((VERSION_NUMBER >> 16) & 0xff), (int)((VERSION_NUMBER >> 8) & 0xff),
			(int)(VERSION_NUMBER & 0xff)
			);
		break;
	default:
		resp.error_code = ERROR_FACTORY_UNKNOWN_ID;
		break;
	}

	m_pIMCLink->PostFrame((char *)&resp, sizeof(resp), PTP_PORT_IMC_FACTORY_SENDER);
}
