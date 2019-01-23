// CardProcess.cpp: implementation of the CCardProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardProcess.cpp 1.17 2010/12/22 16:47:21PST Jining Yang (jyang) Exp  $
 * $Revision: 1.17 $
 */

#include "CardProcess.h"
#include "HouseProcess.h"
#include "stdio.h"
#include "Warmup.h"
#include "EventId.h"
#include "CardLoop.h"
#include "CardStatus.h"
#include "CardAlign.h"
#include "Fpga.h"
#include "OutputProcess.h"
#include "Input.h"
#include "InputSelect.h"
#include "AllProcess.h" 
#include "TwinProxy.h"
#include "machine.h"
#include "BT3Support.h"
#include "PTPLink.h"
#include "MemBlock.h"
#include "InputTod.h"

#define TRACE TRACE_CARD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCardProcess *CCardProcess::s_pCardProcess = NULL;

CCardProcess::CCardProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
, m_rgpQueue(sizeof(SImcRgpReport), 20)
{
	m_pFpga = NULL;
	s_pCardProcess = this;
	m_pCardStatus = new CCardStatus();
	m_pCardLoop = new CCardLoop();
	m_pCardAlign = new CCardAlign();
	m_stateMachineDelay = 0;
	memset(&m_twinCardState, 0, sizeof(m_twinCardState));
	m_sendCardStateCnt = 0;
	m_pImcLink = NULL;
	m_pIocLink = NULL;
	m_pWarmup = NULL;
}

CCardProcess::~CCardProcess()
{

}

void CCardProcess::Init()
{
	m_pWarmup = CWarmup::s_pWarmup;
	m_pFpga = CFpga::s_pFpga;
	m_pCardStatus->Init();
	m_pCardLoop->Init();
	m_pCardAlign->Init();
	AllowMessage();
	m_pImcLink = CAllProcess::g_pAllProcess->GetImcLink();
	m_pIocLink = CAllProcess::g_pAllProcess->GetIocLink();
	m_pImcLink->SetupClient(PTP_IOC_PORT_RGP, this);
}

void CCardProcess::EventDispatch(uint16 event)
{
	if (event & EVT_ONE_SECOND) {
		CardDoOneSecond();
	}

	if (event & EVT_RGP_REPORT) {
		DispatchRgpReport();
	}

	if (event & EVT_SETUP_INPUT) {
		CInput::AllInputSetup();
	}

	CBaseProcess::EventDispatch(event);
}

void CCardProcess::EventTimeOut()
{

}

void CCardProcess::ENTRY()
{
	// Wait for FPGA to get some data
	s_pCardProcess->Pause(TICK_1SEC << 1);

	// Just to make it ready.

	s_pCardProcess->m_pWarmup->TickWarmup();
	
	REPORT_EVENT(COMM_RTE_EVENT_WARMUP, 1);

	s_pCardProcess->Pause(TICK_1SEC);
	s_pCardProcess->PostSetupInput();
	
	s_pCardProcess->StateChange(1);
	s_pCardProcess->StateChange(2);

	CInputSelect::s_pInputSelect->InitInputSelect();

	s_pCardProcess->EventEntry(TICK_1SEC * 2);
}

void CCardProcess::MessageHandler(uint16 message_type, uint32 param)
{
	switch (message_type) {
	case MESSAGE_CARD_FAIL:
		TRACE("Card Failed\r\n");
		CWarmup::s_pWarmup->CardWarmupFail();
		m_pCardStatus->CardStatusFail();
		m_pCardLoop->CardLoopFail();
		m_pCardAlign->CardAlignFail();
		m_pFpga->SetReady(0);
		break;
	case MESSAGE_CARD_OFFLINE:
		TRACE("Card Offline\r\n");
		CWarmup::s_pWarmup->CardWarmupOffline();
		m_pCardStatus->CardStatusOffline();
		m_pCardLoop->CardLoopOffline();
		m_pCardAlign->CardAlignOffline();
		m_pFpga->SetReady(0);
		break;
	}
}

void CCardProcess::ReportCardOneSecondIsr()
{
	SignalEventIsr(EVT_ONE_SECOND);
}

void CCardProcess::CardDoOneSecond()
{
	int changed = 0;
	int warm;

	// TRACE("Do One Second task in card process\r\n");

	CHouseProcess::TickHouseProcessPerSecond();
	warm = m_pWarmup->TickWarmup();
	if (warm == 1) {
		// just out of warmup
		m_pFpga->UnlockPll();
		m_stateMachineDelay = 2;
		return;
	} 
	
	if (warm == 0) {
		changed |= m_pCardStatus->TickCardStatus(0);
		if (changed) {
			SendCardState();
		}

		changed |= CInput::AllInputTick();
		if (changed) {
			CInputSelect::s_pInputSelect->MakeSelect();
		}

	} else if ((warm > 0) && (m_stateMachineDelay <= 0)){
		changed |= m_pCardStatus->TickCardStatus(1);
		changed |= m_pCardLoop->TickCardLoop();
		changed |= m_pCardAlign->TickCardAlign();
		if (changed) {
			SendCardState();
		}

		changed |= CInput::AllInputTick();
		if (changed) {
			CInputSelect::s_pInputSelect->MakeSelect();
		}
	} else {
		m_stateMachineDelay--;

	}

	if ((++m_sendCardStateCnt >= 8) || (changed)) {
		SendCardState();
	}

#if !TO_BE_COMPLETE

	int warm;
	int ret;
	warm = m_pWarmup->TickWarmup();
	CHouseProcess::TickHouseProcessPerSecond();

	if (warm == 1) {
		// just out of warmup
		// m_pFpga->SetMpll(CFpga::MPLL_HOLDOVER); // It should depend on TOD source. Move it to CWarmup
		m_pFpga->UnlockPll();
		// m_pFpga->SetReady(1); // always ready approach suggested by Bob H on 1/24/2006
		m_stateMachineDelay = 0;
		// Enable FPGA to use UNLOCK info to trigger a switch
		m_pFpga->SetUnlockHardControl(1);
		return;
	}
	if ((warm > 0) && (m_stateMachineDelay-- <= 0)) {
		m_stateMachineDelay = 0;
		ret = 0;
		ret |= m_pCardStatus->TickCardStatus(0);
		ret |= m_pCardLoop->TickCardLoop();
		ret |= m_pCardAlign->TickCardAlign();
		if (ret) {
			SendCardState();
		}

		ret |= CInput::AllInputTick();
		if (ret) {
			CInputSelect::s_pInputSelect->MakeSelect();
			//TRACE("Input Select Called\r\n");			
		} 
		
	} else { // It's in warmup
		ret = 0;
		ret |= m_pCardStatus->TickCardStatus(0);
		ret |= m_pCardAlign->TickCardAlign();
		if (ret) {
			SendCardState();
		}

		ret |= CInput::AllInputTick();
		if (ret) {
			CInputSelect::s_pInputSelect->Select();
			//TRACE("Input Select Called\r\n");			
		} 
	}


	if (++m_sendCardStateCnt >= 8) {
		SendCardState();
	}
#endif
}

void CCardProcess::StateChange(int which)
{
	// 1 : Loop changed.
	// 2 : Active/Standby change
	switch (which) {
	case 1:
		COutputProcess::s_pOutputProcess->PostMessage(COutputProcess::MESSAGE_LOOP_CHANGE, 0);
		PostSetupInput();
		break;
	case 2:
		COutputProcess::s_pOutputProcess->PostMessage(COutputProcess::MESSAGE_STATUS_CHANGE, 0);
		PostSetupInput();
		CInputTod::ForceChange();
		CInput::TouchInput();
		break;

	}
}

int CCardProcess::ReceiveTwinCardState(const char *ptr, int size)
{
	SCardState *pState;
	uint8 tmp_exr;

	pState = (SCardState *)ptr;
	if (pState->cmd != CTwinProxy::CMD_CARD_STATE) return -1;
	if (pState->version != 1) return -2;
	
	tmp_exr = get_exr();
	set_exr(7);
	if (size > sizeof(SCardState)) {
		size =  sizeof(SCardState);
	} else if (size < sizeof(SCardState)) {
		memset(&m_twinCardState, 0, sizeof(m_twinCardState));
	}
	memcpy(&m_twinCardState, ptr, size);

	m_pCardStatus->SetTwinStatus(m_twinCardState.cardState);
	m_pCardLoop->SetTwinLoop(m_twinCardState.cardLoop);
	m_pCardAlign->SetTwinAlign(m_twinCardState.cardAlign);
	set_exr(tmp_exr);

	return 0;
}

void CCardProcess::SendCardState()
{
	SCardState state;

	state.cmd = CTwinProxy::CMD_CARD_STATE;
	state.version = 1;
	state.cardState = CCardStatus::GetCardStatus();
	state.cardLoop = m_pCardLoop->GetCardLoop();
	state.cardAlign = m_pCardAlign->GetCardAlign();
	CTwinProxy::s_pTwinProxy->PostBuffer((char *)&state, sizeof(state));
	m_sendCardStateCnt = 0;
}

int CCardProcess::Notify(uint32 from, uint32 what)
{
	CMemBlock *pBlock;
	int ret;
	uint16 *pPacket;
	int len;

	pBlock = (CMemBlock *)what;

	if (from == (uint32)m_pImcLink) {
		pPacket = (uint16 *)pBlock->GetBuffer();
		len = pBlock->GetCurSize();
		switch (*pPacket) {
		case CMD_PTP_RGP: 
			if (len == sizeof(SImcRgpReport)) {
				m_rgpQueue.Append((uint8 *)pPacket);
				pBlock->Release();
				SignalEvent(EVT_RGP_REPORT);
				return 1;
			}
			break;
		default:
			FORCE_TRACE("CCardProcess Unknown Packet Cmd=0x%X\r\n"
				, *pPacket);
			return 0;
		}
		FORCE_TRACE("Unexpected message from PPC Cmd=0x%X\r\n", *pPacket);
		return 0;
	}

#if 0
	if (from == (uint32)m_pIocLink) {
		pPacket = (uint16 *)pBlock->GetBuffer();
		FORCE_TRACE("Unexpected message from RTE Cmd=0x%X\r\n", *pPacket);
		return 0;
	}
#endif

	FORCE_TRACE("Major Error in CCardProcess 225\r\n");
	return 0; // shouldn't be here
}

void CCardProcess::DispatchRgpReport()
{
	SReportGPS gps;
	SImcRgpReport report;  
	int ret;
	
	do {
		ret = m_rgpQueue.GetTop((uint8 *)&report);
		if (ret >= 1) {
			gps.offset = report.offset;
			gps.offsetTime = report.offsetTime;
			gps.status = report.status;
			gps.handle = report.handle;
			CBT3InBound::s_pBT3InBound->SetGPSReport(&gps);
		}
	} while (ret == 1);
}

void CCardProcess::PostSetupInput()
{
	SignalEvent(EVT_SETUP_INPUT);
}
