// IstateExec.cpp: implementation of the CIstateExec class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateExec.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateExec.cpp 1.3 2008/01/30 09:22:48PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "IstateExec.h"
#include "IstateProcess.h"
#include "Istate.h"
#include "PTPLink.h"
#include "IstateBlockTrans.h"
#include "IstateItemTrans.h"
#include "IstateCheckSum.h"
#include "IstateKb.h"
#include "stdio.h"
#include "AllProcess.h"
#include "CardStatus.h"
           
#define TRACE	TRACE_ISTATE  

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateExec::CIstateExec( //CFPGA *pFPGA, CCardState *pCardState,
	CIstate *pIstate, 
	// CPTPLink *pIMCLink, uint8 imcPeer, // Keep only RTE Istate in RTE
	CPTPLink *pIOCLink,
	uint8 iocPeer)
{
	m_iocCompatible = -1;
	m_cnt = 0;
	// m_pFPGA = pFPGA;
	m_pLink = pIOCLink;
	m_linkedTime = 0L;
	m_copyEngine = 10;

	m_pIstate = pIstate;
	// m_pCardState = pCardState;
	m_pKB = m_pIstate->GetKB();
	//m_pIstateLocation = m_pKB->GetIstateLocation(&m_istateSize, &m_istateVersion); 
	
	// Tansfer mechnism with RTE card
	m_pCheckSum_IOC = new CIstateCheckSum(/* m_pCardState ,*/ m_pIstate, 
		pIOCLink, iocPeer);
	m_pBlockTR_IOC = new CIstateBlockTrans(m_pIstate,
		pIOCLink, iocPeer);
	m_pItemTR_IOC = new CIstateItemTrans(m_pCheckSum_IOC, m_pIstate, 
		pIOCLink, iocPeer);

	/// Transfer machnism with PPC card
#if 0
	// PPC Istate handling changed. NO PPC Istate but a memory block.
	m_pBlockTR_IMC = NULL;
	m_pItemTR_IMC = NULL;
	m_pCheckSum_IMC = NULL;
#endif

#if 0 // Keep only RTE-Istate in RTE
	// Enable simlilar scheme with PPC
	m_pBlockTR_IMC = new CIstateBlockTrans(m_pIstate,
		pIMCLink, imcPeer);
	m_pItemTR_IMC = new CIstateItemTrans(m_pIstate,
		pIMCLink, imcPeer);
	m_pCheckSum_IMC = new CIstateCheckSum(m_pCardState, m_pIstate,
		pIMCLink, imcPeer);
#endif
}

CIstateExec::~CIstateExec()
{

}

// return > 0: Expected message
// -1: Unexpected message
// This code deal with both PPC Istate and RTE Istate.
// Between RTE card, both Istates behave the same. That is
// active RTE card dominates.
int CIstateExec::DispatchMessageFromIOC(const uint8 *ptr, uint16 len)
{
	int result = -1;
	uint16 *pCmd;
	uint16 curCmd;
	int myCardState, twinCardState;
	int mySlotId, twinSlotId = 0;

	pCmd = (uint16 *)ptr;
	curCmd = *pCmd;

	if ((curCmd > CIstateCheckSum::CMD_START) && 
		(curCmd < CIstateCheckSum::CMD_END)) {
		result = m_pCheckSum_IOC->DispatchMessage(ptr, len);

		if (curCmd != CIstateCheckSum::CMD_CHECKSUM) return result;

		CheckIocCompatibility();
		
		InitTwinIstate();

	} else if ((curCmd > CIstateBlockTrans::CMD_START) &&
		(curCmd < CIstateBlockTrans::CMD_END)) {
		// Command for block transceiver
		result = m_pBlockTR_IOC->DispatchMessage(ptr, len);
		if (result == 0x101) {
			// Receiveing done
			m_pCheckSum_IOC->SetInited();
			m_pKB->ActionAll();
		}
	} else if ((*pCmd > CIstateItemTrans::CMD_START) && 
		(*pCmd < CIstateItemTrans::CMD_END)) {
		result = m_pItemTR_IOC->DispatchMessage(ptr, len);
		if (result == 0x101) {
			// Receiving done
			m_pCheckSum_IOC->SetInited();
			m_pKB->ActionAll();
		}

		if (m_copyEngine < 5) { // See if copy engine could start
			InitTwinIstate();
		} 

	}
	
	return result;
}

int CIstateExec::DispatchMessageFromIMC(const uint8 *ptr, uint16 len)
{
	// Behaviour for RTE/PPC Istate are different when dealing with PPC card.
	// RTE card dominate RTE Istate. For any automatic transfer, RTE card 
	// will be the source. PPC is the destination.
	// PPC card dominate PPC Istate. For any automatic transfer, PPC card
	// will be the source. RTE will be the destination.

	return -1;
}

// Call when process startup. To read init value from NVRAM.
//void CIstateExec::Init()
//{
//	m_pIstate->Init();
//}

void CIstateExec::TickPerSecond()
{
	int stat;
	stat = m_pLink->GetLinkStatus();

	m_pIstate->TickPerSecond();
	m_pCheckSum_IOC->TickPerSecondIstateCheckSum();
	m_pItemTR_IOC->TickPerSecond();
	m_pBlockTR_IOC->TickPerSecond();

#if 0 // Key only RTE-Istate in RTE
	if (m_pCheckSum_IMC)
		m_pCheckSum_IMC->TickPerSecond();
	if (m_pItemTR_IMC) 
		m_pItemTR_IMC->TickPerSecond();
	if (m_pBlockTR_IMC) 
		m_pBlockTR_IMC->TickPerSecond();
#endif

	if (stat < 10) {
		//if (m_linkedTime) m_linkedTime++;
		if (m_linkedTime > 10) {
			m_pCheckSum_IOC->LinkDown();
			m_pItemTR_IOC->LinkDown();
			m_pBlockTR_IOC->LinkDown();
			m_copyEngine = 10;
			m_linkedTime = 0L;
		}
		if (m_cnt > 20) {
			CheckIocCompatibility();
		}
	} else {
		m_linkedTime++;
	}

	if (m_cnt++ > 20000) {
		m_cnt = 10000;
	}
}

// 1: For Idle
// 0: Either doing item transfer or block transfer
int CIstateExec::IsAvailable()
{
	if (m_pBlockTR_IOC->IsAvailable() != 1) return 0;
	if (m_pItemTR_IOC->IsAvailable() != 1) return 0;
	if (m_copyEngine < 5) return 2; // Copy engine is wait for its turn.
#if 0 // Keep only RTE-Istate in RTE
	if (m_pBlockTR_IMC) { 
		if (m_pBlockTR_IMC->IsAvailable() != 1) return 0;
	}
	if (m_pItemTR_IMC) {
		if (m_pItemTR_IMC->IsAvailable() != 1) return 0;
	}
#endif
	
	// All are not busy.
	return 1;
}

// 1: OK
int CIstateExec::RelayItemToTwinIOC(int16 item, int16 index1, int16 index2)
{
	// Reset checksum counter
	m_pCheckSum_IOC->ResetMismatchCounter();
	return m_pItemTR_IOC->RelayOneItem(item, index1, index2);
}

void CIstateExec::ResendIstateToTwin()
{
	// Resending is started by clear twin's init flag
	m_pCheckSum_IOC->ClearTwinInitFlag(); 
}

void CIstateExec::PrintIstateExec(Ccomm *pComm)
{
	m_pCheckSum_IOC->PrintIstateCheckSum(pComm);
	m_pBlockTR_IOC->PrintIstateBlockTrans(pComm);
	m_pItemTR_IOC->PrintIstateItemTrans(pComm);
}

void CIstateExec::Init()
{
	m_pCheckSum_IOC->Init();
}


int CIstateExec::CheckIocCompatibility()
{
	int compatible;

	compatible = m_pCheckSum_IOC->IsTwinCompatible();

	if (m_iocCompatible == compatible) return 0;

	if (compatible == 0) {
		REPORT_EVENT(COMM_RTE_EVENT_RTE_INCOMPATIBLE, 1);
	} else {
		REPORT_EVENT(COMM_RTE_EVENT_RTE_INCOMPATIBLE, 0);
	}

	m_iocCompatible = compatible;
}

int CIstateExec::InitTwinIstate()
{
	int bMaster = 0;

	bMaster = m_pCheckSum_IOC->IsMaster();
		
	if (bMaster == 1) {
		// This card is active, look at twin.
		// If twin's Istate is not inited, copy it over
		uint32 times;
		int32 sendCnt;
		int32 recvCnt;
		int bSameVersion = m_pCheckSum_IOC->IsTwinSameVersion();
		int bMatch = m_pCheckSum_IOC->IsMatch(&times);
		int bInited = m_pCheckSum_IOC->IsTwinInited();
		sendCnt = m_pBlockTR_IOC->GetTransferCnt() + m_pItemTR_IOC->GetTransferCnt();
		recvCnt = m_pBlockTR_IOC->GetReceiveCnt() + m_pItemTR_IOC->GetReceiveCnt();
		if (IsAvailable() <= 0) {
			m_copyEngine = 0;
			return -1;
		}
		if ((bInited == 0) || ((sendCnt+recvCnt) <= 0L)) { // If twin not inited or I never sent/recv it over				
			// Send Istate over
			if (bSameVersion == 1) {
				//if (bMatch != 1) { // copy it once no matter what. 8/10/2005
				// I am send to the other guy. Better call myself initialized.
				m_pCheckSum_IOC->SetInited();
				m_pBlockTR_IOC->StartBlockTransfer();
				//}
			} else if (bSameVersion == 0) {
				// I am sending to the other guy. Better call myself initialized.
				m_pCheckSum_IOC->SetInited();
				m_pItemTR_IOC->StartItemTransfer();
			} // No more else should happen here
		} else if ((bSameVersion == 1) && (bMatch == 0)) {
			// Twin is the same version, but checksum
			// doesn't match
			TRACE("%s Mismatch %ld times between RTE/RTE\r\n", 
				m_pIstate->GetName(), times);
			if (times >= 5) { // should be 5
				// Send it over
				m_pBlockTR_IOC->StartBlockTransfer();
				m_pCheckSum_IOC->ResetMismatchCounter();
			}
		}
		m_copyEngine = 10;
	} else if (bMaster == 0) {
		// This card is InActive card
		uint32 times;
		int bMatch = m_pCheckSum_IOC->IsMatch(&times);
		if (bMatch == 1) {
			// Istate matches. 
			//m_pCheckSum_IOC->SetInited();
		}
	}

	return 1;
}

// unknow -1.  0 not  . 1 ok
int CIstateExec::IsTwinSameIstateVersion()
{
	return m_pCheckSum_IOC->IsTwinSameVersion();
}


