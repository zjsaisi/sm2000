// PTPDnldProcess.cpp: implementation of the CPTPDnldProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldProcess.cpp 1.2 2008/01/29 11:29:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
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
#include "BlockStorage.h"
#include "comm.h"
#include "IstateProcess.h"
#include "IstateParser.h"
#include "stdio.h"
#include "PTPFactory.h"
#include "PTPDnldReport.h"
#include "IOCIstateStorage.h"
#include "IOCEvtList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACE PrintDebugOutput

CPTPDnldProcess *CPTPDnldProcess::s_pPTPDnldProcess = NULL;

CPTPDnldProcess::CPTPDnldProcess(CFactory *pFactory,
	CIstateProcess *pIstateProcess, CLoader *pLoader, 
	CFlash *pFlash, CPTPLink *pIOCLink, 
	CPTPLink *pIMCLink,
	// uint8 inPort, uint8 outPort,
	const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry /*= Entry*/)
: CBaseProcess(name, priority, stack_size, entry)
{
	s_pPTPDnldProcess = this;
	m_pIOCLink = pIOCLink;
	m_pIMCLink = pIMCLink;
	m_pIstateProcess = pIstateProcess;

	// Init Storage
	m_pStorage = new CCodeStorage("IOCCODE-DNLD", pLoader);
	m_pStorage->Rewind();
	
	m_iocDnldBox.SetMailEvent(m_taskSlot, EVT_IOC_CODE_DNLD);
	m_imcDnldBox.SetMailEvent(m_taskSlot, EVT_IMC_CODE_DNLD);

	// For Receiving code from RTE 
	m_pIOCLink->SetupClient(PTP_PORT_IOC_RECEIVER, this);
	m_pIOCReceiver = new CPTPDnldRecv(m_pStorage, MAX_CODE_SIZE, CMD_CODE_DOWNLOAD,  
		pIOCLink, PTP_PORT_IOC_SENDER, 1 /* Check header*/);
	m_pIOCReceiver->SetDelayedWriting(1);

	// For Sending code to RTE
	m_pIOCLink->SetupClient(PTP_PORT_IOC_SENDER, this);
	m_pIOCSender = new CPTPDnldSend(m_pStorage, CMD_CODE_DOWNLOAD,  
		pIOCLink, PTP_PORT_IOC_SENDER);

	// For Receiving code from PPC
	m_pIMCLink->SetupClient(PTP_PORT_IMC_RECEIVER, this);
	m_pIMCReceiver = new CPTPDnldRecv(m_pStorage, MAX_CODE_SIZE, CMD_CODE_DOWNLOAD,
		m_pIMCLink, PTP_PORT_IMC_CODE_SENDER, 1 /* Check header */);
	m_pIMCReceiver->SetDelayedWriting(1);

	// For Receiving PPC Istate from PPC
	m_pIMCIstateStorage = new CBlockStorage("PPC Istate", MAX_IMC_ISTATE_SIZE);
	m_pIMCIstateReceiver = new CPTPDnldRecv(m_pIMCIstateStorage, MAX_IMC_ISTATE_SIZE,
		CMD_IMC_ISTATE_DOWNLOAD, m_pIMCLink, PTP_PORT_IMC_IMCISTATE_SENDER);
	// For reporting PPC Istate back to PPC
	m_pIMCIstateReporter = new CPTPDnldReport(m_pIMCIstateStorage,
		CMD_GET_IMCISTATE_MAIN, m_pIMCLink, PTP_PORT_IMC_IMCISTATE_REPORT_PEER,
		CPTPDnldRecv::BIG_DNLD_PACKET_SIZE);

	// For receiving PPC istate verion from PPC
	m_pIMCIstateVersionStorage = new CBlockStorage("PPC Istate Version", 
		MAX_IMC_ISTATE_VERSION_SIZE);
	m_pIMCIstateVersionReceiver = new CPTPDnldRecv(m_pIMCIstateVersionStorage,
		MAX_IMC_ISTATE_VERSION_SIZE, 
		CMD_IMC_ISTATE_VERSION_DOWNLOAD,
		m_pIMCLink, PTP_PORT_IMC_IMCISTATE_VERSION_SENDER);
	// For Reporting PPC Istate verion back to PPC
	m_pIMCIstateVerionReporter = new CPTPDnldReport(m_pIMCIstateVersionStorage, 
		CMD_GET_IMCISTATE_VERSION, m_pIMCLink, 
		PTP_PORT_IMC_IMCISTATE_VERSION_REPORT_PEER,
		MAX_IMC_ISTATE_VERSION_SIZE);

	// For Receiving RTE Istate from PPC
	m_pIOCIstateStorage = new CIOCIstateStorage("RTE Istate", MAX_IOC_ISTATE_SIZE);
	m_pIOCIstateReceiver = new CPTPDnldRecv(m_pIOCIstateStorage, MAX_IOC_ISTATE_SIZE,
		CMD_IOC_ISTATE_DOWNLOAD, m_pIMCLink, PTP_PORT_IMC_IOCISTATE_SENDER);
	m_pIOCIstateReporter = new CPTPDnldReport(m_pIOCIstateStorage,
		CMD_GET_IOC_ISTATE, m_pIMCLink,
		PTP_PORT_IMC_IOCISTATE_REPORT_PEER,
		MAX_IOC_ISTATE_SIZE);
	m_pIstateParser = new CIstateParser(m_pIOCIstateStorage, m_pIstateProcess);
	m_pIOCIstateStorage->SetParser(m_pIstateParser);

	// For factory setting
	m_pPTPFactory = new CPTPFactory(pFactory, m_pIMCLink, PTP_PORT_IMC_FACTORY_SENDER);
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
	EventEntry(TICK_1SEC * 2);
}

void CPTPDnldProcess::EventTimeOut()
{
	m_pIOCReceiver->TickTime(2);
	m_pIOCSender->TickTime(2);
	m_pIOCIstateReceiver->TickTime(2);
	m_pIMCReceiver->TickTime(2);
	m_pIMCIstateReceiver->TickTime(2);
	m_pIMCIstateVersionReceiver->TickTime(2);

}

void CPTPDnldProcess::EventDispatch(uint16 event)
{
	CMemBlock *pBlock;
	SCmdGeneral *pCmd;
	int size;

	// Deal code copy with twin RTE
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

	// Deal code copy with PPC
	while (event & EVT_IMC_CODE_DNLD) {
		pBlock = (CMemBlock *)m_imcDnldBox.ReceiveMail();
		if (pBlock != NULL) {
			// Only receive code from PPC.
			pCmd = (SCmdGeneral *)pBlock->GetBuffer();
			//TRACE("Recv Dnld Cmd 0x%X\r\n", pCmd->cmd);
			switch (pCmd->cmd) {
			case CMD_CODE_DOWNLOAD: // Write firmware from PPC to RTE
				m_pIMCReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;

			case CMD_IOC_ISTATE_DOWNLOAD: // Write RTE-Istate to RTE
				// Receive RTE Istate from PPC
				m_pIOCIstateReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_GET_IOC_ISTATE: // Read RTE-Istate from RTE to PPC
				m_pIOCIstateReporter->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_IMC_ISTATE_DOWNLOAD: // Write PPC-Istate to PPC
				// Receive PPC Istate from PPC
				m_pIMCIstateReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_GET_IMCISTATE_MAIN: // Read PPC-Istate back to PPC
				// Receive GETTING cmd from PPC to get PPC Istate main block
				m_pIMCIstateReporter->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_IMC_ISTATE_VERSION_DOWNLOAD: // Write PPC-Istate verion to RTE
				m_pIMCIstateVersionReceiver->ReceivePacket(pBlock);
				pBlock->Release();
				return;
			case CMD_GET_IMCISTATE_VERSION: // Read PPC-Istate version back to PPC
				m_pIMCIstateVerionReporter->ReceivePacket(pBlock);
				pBlock->Release();
				return;

			case CMD_GET_FACTORY:
				m_pPTPFactory->ReceiveReadPacket(pBlock);
				pBlock->Release();
				return;
			case CMD_SET_FACTORY:
				m_pPTPFactory->ReceiveWritePacket(pBlock);
				pBlock->Release();
				return;
			default:
				pBlock->Release();
				TRACE("Invalid Dnld Cmd 0x%X\r\n", pCmd->cmd);
				return;
			}
		} else {
			break;
		}
	}

	if (event & EVT_START_SEND_CODE_TO_TWIN_IOC) {
		// Start download code to twin RTE
		m_pIOCSender->StartSendCode();
		REPORT_EVENT(COMM_RTE_EVENT_FIRMWARE_COPY, 1);
	}
}

int CPTPDnldProcess::Notify(uint32 from, uint32 what)
{
	CMemBlock *pBlock;
	int ret;
	
	pBlock = (CMemBlock *)what;

	// Receive from RTE
	if (from == (uint32)m_pIOCLink) {
		ret = m_iocDnldBox.SendMail(pBlock);
		if (ret != 1) {
			pBlock->Release();
		}
		return 1;
	}

	// Receive from PPC
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

void CPTPDnldProcess::StartSendCodeToTwinIOC()
{
	SignalEvent(EVT_START_SEND_CODE_TO_TWIN_IOC);
}


void CPTPDnldProcess::DnldHack(unsigned long *param, Ccomm *pComm)
{
	char buff[100];
	int ret;
	if (*param == 0) {
		const char *ptr;
		ptr = "Option list:\r\n\t10: Print PPC Istate in storage\r\n\
\t11: Print PPC Istate in storage as ASCII\r\n\
\t12: Print PPC Istate version in storage\r\n\
\t13: Print PPC Istate version in storage as ASCII\r\n\
\t20: Print RTE Istate in storage\r\n\
\t21: Print RTE Istate in storage as ASCII\r\n\
\t30: Print RTE Istate to Storage\r\n\
\t31: Parse RTE Istate from Storage\r\n\
";
		pComm->Send(ptr);
		return;
	}
	switch (param[1]) {
	case 10:
	case 11:
		s_pPTPDnldProcess->m_pIMCIstateStorage->PrintContent(pComm, param[1] & 1);
		return;
	case 12:
	case 13:
		s_pPTPDnldProcess->m_pIMCIstateVersionStorage->PrintContent(pComm, 
			param[1] & 1);
		return;
	case 20: 
		s_pPTPDnldProcess->m_pIOCIstateStorage->PrintContent(pComm);
		return;
	case 21:
		s_pPTPDnldProcess->m_pIOCIstateStorage->PrintContent(pComm, 1);
		return;
	case 30:
		ret = s_pPTPDnldProcess->m_pIstateParser->PrintIstate(); // Convert binary into ASCII
		sprintf(buff, "%d Istate Item(s) converted\r\n", ret);
		pComm->Send(buff);
		return;
	case 31:
		ret = s_pPTPDnldProcess->m_pIstateParser->ReadIstate(); // Convert ASCII Istate to binary
		sprintf(buff, "%d Istate Item(s) Read\r\n", ret);
		pComm->Send(buff);
		return;
	default:
		break;
	}
	pComm->Send("Invalid option\r\n");
}


void CPTPDnldProcess::Init()
{

}
