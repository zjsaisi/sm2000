// MinuteProcess.cpp: implementation of the CMinuteProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: MinuteProcess.cpp 1.7 2010/10/14 00:17:54PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.7 $
 */

#include "MinuteProcess.h"
#include "string.h"
#include "BT3Api.h"
#include "CardStatus.h"
#include "NewCode.h"
#include "AllProcess.h"        
#include "TwinProxy.h"
#include "PTPLink.h"
#include "CardLoop.h"
#include "IstateDti.h"
#include "IstateProcess.h"
#include "CardAlign.h"
#include "RTC.h"
#include "Fpga.h"
#include "SecondProcess.h"

#ifndef TRACE
#define TRACE TRACE_MINUTE
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMinuteProcess *CMinuteProcess::s_pMinuteProcess = NULL;

CMinuteProcess::CMinuteProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_pIocLink = NULL;
	s_pMinuteProcess = this;
	m_autoReboot = 0;
	m_currentMinute = 0L;
	m_giveupMinute = 0L;
}

CMinuteProcess::~CMinuteProcess()
{

}

void CMinuteProcess::Entry()
{
	s_pMinuteProcess->EventEntry(TICK_1SEC * 63);
}

void CMinuteProcess::Init()
{
	m_pIocLink = CAllProcess::g_pAllProcess->GetIocLink();
}

void CMinuteProcess::EventDispatch(uint16 event)
{
	CBaseProcess::EventDispatch(event);
	if (event & EVT_ONE_MINUTE) {
		OneMinuteTask();
	}

	if (event & EVT_HALF_MINUTE) {
		CheckGpsPhase();
		CheckNewCode();
	}
}

void CMinuteProcess::TickMinuteProcess()
{
	SignalEvent(EVT_ONE_MINUTE);
}

void CMinuteProcess::CheckGpsPhase(void)
{
	static int32 gpsPhase = 0;
	int32 phase;
	if (CSecondProcess::s_pSecondProcess->GetTodSource() == CIstateDti::TOD_SOURCE_GPS) {
		phase = CFpga::s_pFpga->GetGPSPhase();
		if (phase != gpsPhase) {
			TRACE("Gps-Local phase diff %ld ==> %ld\r\n", gpsPhase, phase);
			gpsPhase = phase;
		}
	}  
	// GPZ June 2010 allow reading of phase data in user mode (ever though we won't use it)
	else if (CSecondProcess::s_pSecondProcess->GetTodSource() == CIstateDti::TOD_SOURCE_USER) {
		phase = CFpga::s_pFpga->GetGPSPhase();
		if (phase != gpsPhase) {
			TRACE("Gps-Local phase diff %ld ==> %ld\r\n", gpsPhase, phase);
			gpsPhase = phase;
		}
	}
	// GPZ June 2010 allow reading of phase data in hybrid mode 
	else if (CSecondProcess::s_pSecondProcess->GetTodSource() == CIstateDti::TOD_SOURCE_HYBRID) {
		phase = CFpga::s_pFpga->GetGPSPhase();
		if (phase != gpsPhase) {
			TRACE("Gps-Local phase diff %ld ==> %ld\r\n", gpsPhase, phase);
			gpsPhase = phase;
		}
	}
	
}

void CMinuteProcess::OneMinuteTask()
{
	CBT3Api::BT3TickOneMinute();
	m_currentMinute++;

#if !TO_BE_COMPLETE
	CBT3Api::BT3TickOneMinute();
	m_currentMinute++;

	{
		int year, month, day, hr, minute, second;
		uint16 high, low;
		int  dither, cnt;
		g_pRTC->GetDateTime(&year, &month, &day, &hr, &minute, &second);
		TRACE("GPS phase(1M): %ld @%04d-%02d-%02d %02d-%02d-%02d\r\n"
			, CCardAlign::s_pCardAlign->GetSignedGpsPhase()
			, year, month, day, hr, minute, second);
		CFpga::s_pFpga->GetCurrentEfcControl(&high, &low, &dither, &cnt);
		TRACE("EFC Ctrl: high=%04X low=%04X dither=%d Dcnt=%d\r\n",
			high, low, dither, cnt);

	}
#endif
}

void CMinuteProcess::TickHalfMinute()
{
	SignalEvent(EVT_HALF_MINUTE);
}

void CMinuteProcess::CheckNewCode()
{
	int ret;
	int stat;

	CNewCode::s_pNewCode->CheckCode();

	ret = CNewCode::s_pNewCode->HasValidNewCode();
	if (ret != 1) {
		// no new code loaded
		if (m_autoReboot) {
			m_autoReboot--;
		} else {
			SET_MY_ISTATE(COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE, 0, 0, 1);		
		}
		return;
	}

	if (m_autoReboot == 0) {
		// not told to reboot yet.
		return;
	}

	stat = CCardStatus::GetCardStatus();
	switch (stat) {
	case CCardStatus::CARD_HOT_ACTIVE:
		RebootHotActive();
		break;
	case CCardStatus::CARD_COLD_ACTIVE:
		RebootColdActive();
		break;
	case CCardStatus::CARD_WARMUP:
	case CCardStatus::CARD_HOT_STANDBY:
	case CCardStatus::CARD_FAIL:
	case CCardStatus::CARD_OFFLINE:
	case CCardStatus::CARD_COLD_STANDBY:
	default:
		// reboot now
		CAllProcess::g_pAllProcess->RebootSystem(0, NULL, COMM_RTE_EVENT_AUTO_RESET);
		break;
	}
}

void CMinuteProcess::SetAutoReboot(int reboot /* = 1 */)
{
	m_autoReboot = (reboot) ? (5) : (0);
	if (m_autoReboot) {
		SET_MY_ISTATE(COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE, 0, 0, 0);
		TickHalfMinute();
	}
}

void CMinuteProcess::RebootColdActive()
{
	int fresh;
	int link;
	int twinState, stateFresh;

	fresh = CTwinProxy::s_pTwinProxy->GetFresh();
	link = m_pIocLink->GetLinkStatus();
	if ((fresh > 300) && (link < -100)) {
		// The other guy is not talking.
		CAllProcess::g_pAllProcess->RebootSystem(0, NULL, COMM_RTE_EVENT_AUTO_RESET);
		return;
	}

	twinState = CCardStatus::s_pCardStatus->GetTwinStatus(&stateFresh);

	TRACE("Twin Fresh: %d, ComLink=%d Giveup=%ld CurMin=%ld TwinStatusFresh=%d\r\n"
		, fresh, link, m_giveupMinute, m_currentMinute, stateFresh);

	if ((fresh >= 200) || (link < 100)) {
		m_giveupMinute = 0L;
		return;
	}

	if (stateFresh > 60) return;

	if (m_giveupMinute == 0L) {
		m_giveupMinute = m_currentMinute;
		TRACE("Ready to switch @%ld minute\r\n", m_giveupMinute);
		return;
	}

	// The other guy is present. 
	if (twinState == CCardStatus::CARD_COLD_STANDBY) {
		if ((m_currentMinute - m_giveupMinute) > 3) {
			SET_MY_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_FORCE_STANDBY);
			m_giveupMinute = 0L;
			return;
		}
		return;
	} else {
		m_giveupMinute = 0L;
	}

	
}

void CMinuteProcess::RebootHotActive()
{
	int fresh;
	int link;
	int myLoop;
	int twinLoop, loopFresh;

	fresh = CTwinProxy::s_pTwinProxy->GetFresh();
	link = m_pIocLink->GetLinkStatus();
	if ((fresh > 300) && (link < -100)) {
		// The other guy is not talking.
		CAllProcess::g_pAllProcess->RebootSystem(0, NULL, COMM_RTE_EVENT_AUTO_RESET);
		return;
	}

	myLoop = CCardLoop::s_pCardLoop->GetCardLoop();
	twinLoop = CCardLoop::s_pCardLoop->GetTwinLoop(&loopFresh);

	TRACE("Twin Fresh: %d, ComLink=%d Giveup=%ld CurMin=%ld TwinLoopFresh=%d\r\n"
		, fresh, link, m_giveupMinute, m_currentMinute, loopFresh);

	if ((fresh >= 200) || (link < 100)) {
		m_giveupMinute = 0L;
		return;
	}
		
	if (loopFresh > 60) return;

	if (m_giveupMinute == 0L) {
		m_giveupMinute = m_currentMinute;
		TRACE("Ready to switch @%ld minute\r\n", m_giveupMinute);
		return;
	}

	if (myLoop == CCardLoop::CARD_FREQ_FREERUN) { // Freerun case
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			break;
		default:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
			m_giveupMinute = 0L;
			break;
		}
	}

	if (myLoop == CCardLoop::CARD_FREQ_FAST_TRACK) {
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
			break;
		default:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			m_giveupMinute = 0L;
			break;
		}
	}

	if (myLoop == CCardLoop::CARD_FREQ_NORMAL_TRACK) {
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
			break;
		default:
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			m_giveupMinute = 0L;
			break;
		}

	}

	if (myLoop == CCardLoop::CARD_FREQ_BRIDGE) {
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			break;
		default:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
			m_giveupMinute = 0L;
			break;   
		}
	}

	if (myLoop == CCardLoop::CARD_FREQ_HOLDOVER) {
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			break;
		default:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
			m_giveupMinute = 0L;
			break;     
		}
	}

	if (myLoop == CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER) {
		switch (twinLoop) {
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		case CCardLoop::CARD_FREQ_FAST_TRACK:
		case CCardLoop::CARD_FREQ_BRIDGE:
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		case CCardLoop::CARD_FREQ_FREERUN:
			break;
		default:
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
			m_giveupMinute = 0L;
			break;     
		}
	}

	if ((m_giveupMinute != 0L) && (m_currentMinute - m_giveupMinute > 3)) {
		SET_MY_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_FORCE_STANDBY);
	}
}
