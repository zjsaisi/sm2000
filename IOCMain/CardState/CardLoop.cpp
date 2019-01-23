// CardLoop.cpp: implementation of the CCardLoop class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardLoop.cpp 1.17 2010/10/14 00:12:15PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.17 $
 */

#include "CardLoop.h"
#include "AllProcess.h"
#include "CardProcess.h"
#include "IstateProcess.h"
#include "IstateDti.h"
#include "SecondProcess.h"
#include "InputSelect.h"
#include "CardStatus.h"
#include "Fpga.h"
#include "Input.h"
#include "BT3Support.h"
#include "CardAlign.h"
#include "BT3Api.h"
#include "stdio.h"

#ifndef TRACE
#define TRACE TRACE_CARD
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCardLoop *CCardLoop::s_pCardLoop = NULL;

CCardLoop::CCardLoop()
{
	s_pCardLoop = this;
	m_loopState = CARD_FREQ_WARMUP;
	m_maxHoldTime = 60;
	m_loopCnt = 0L;
	m_twinLoop = CARD_FREQ_WARMUP;
	m_twinLoopFresh = 10000;
	m_todSource = -1;
	m_cardStatus = -1;
	m_gpsPhaseErrorOverLimit = -2;
	m_gpsPhaseFine = -2;
	m_bt3Mode = 0;
	m_bt3Gear = 0;
	m_maxBridgeTime = 60;
	m_bridgeTime = 60;
}

CCardLoop::~CCardLoop()
{

}

int CCardLoop::TickCardLoop()
{
	int old;
	uint32 life;

	if (++m_twinLoopFresh >= 20000) {
		m_twinLoopFresh = 10000;
	}

	GetGpsModeGear();

	m_cardStatus = CCardStatus::GetCardStatus();
	life = CCardStatus::s_pCardStatus->GetCardStateLife();
	if (m_cardStatus == CCardStatus::CARD_HOT_STANDBY) {
		if (m_twinLoopFresh < 20) {
			if (m_loopState != m_twinLoop) {
				switch (m_twinLoop) {
				case CARD_FREQ_FREERUN:
				case CARD_FREQ_FAST_TRACK:
				case CARD_FREQ_NORMAL_TRACK:
				case CARD_FREQ_BRIDGE:
				case CARD_FREQ_HOLDOVER:
				case CARD_FREQ_EXTENDED_HOLDOVER:
					TransitLoop(m_twinLoop);
					return 1;
				case CARD_FREQ_WARMUP:
				case CARD_FREQ_FAIL:
				case CARD_FREQ_OFFLINE:
				default:
					if (m_loopState != CARD_FREQ_HOLDOVER) {
						TransitLoop(CARD_FREQ_HOLDOVER);
					}
					break;
				}
			} else {
				return 0;
			}
			if (life < 10) {
				// zmiao 3/5/2009: If Active card doesn't change state, it won't trigger phase control change. 
				// Here it is to re-assure it happens. 
				MonitorGpsPhase(1);
				MonitorFineGpsPhase();
			}
		} else if (m_loopState != CARD_FREQ_HOLDOVER) {
			TransitLoop(CARD_FREQ_HOLDOVER); 
			return 1;
		}
		return 0;
	}

	if (CFpga::s_pFpga->IsFpgaActive() != 1) return 0; // Hold on

	old = m_loopState;
	switch (m_loopState) {
	case CARD_FREQ_WARMUP:
		TickWarmup();
		break;
	case CARD_FREQ_FREERUN:
		TickFreerun();
		break;
	case CARD_FREQ_FAST_TRACK:
		TickFastLock();
		break;
	case CARD_FREQ_NORMAL_TRACK:
		TickNormalLock();
		break;
	case CARD_FREQ_BRIDGE:
		TickBridge();
		break;
	case CARD_FREQ_HOLDOVER:
		TickHoldover();
		break;
	case CARD_FREQ_EXTENDED_HOLDOVER:
		TickExtendedHoldover();
		break;
	case CARD_FREQ_FAIL:
		TickFail();
		break;
	case CARD_FREQ_OFFLINE:
		TickOffline();
		break;
	}
	m_loopCnt++;
	if (old != m_loopState) return 1;
	return 0;
}

void CCardLoop::CardLoopFail()
{

	 if (m_loopState == CARD_FREQ_FAIL) return;

	 TransitLoop(CARD_FREQ_FAIL);	 
}

void CCardLoop::CardLoopOffline()
{

	if (m_loopState == CARD_FREQ_OFFLINE) return;
	if (m_loopState == CARD_FREQ_FAIL) return;

	TransitLoop(CARD_FREQ_OFFLINE);
}

void CCardLoop::Init()
{

}

void CCardLoop::SetLoopState(int state, int from)
{
	if (state == from) return;

	switch (state) {
	case CARD_FREQ_WARMUP:
		break;
	case CARD_FREQ_FREERUN:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_FREERUN);
		REPORT_EVENT(COMM_RTE_EVENT_FREERUN, 1);
		//CSecondProcess::s_pSecondProcess->SetTodValid(1);
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		break;
	case CARD_FREQ_FAST_TRACK:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_FAST_TRACK);
		REPORT_EVENT(COMM_RTE_EVENT_FAST_TRACK, 1);
		break;
	case CARD_FREQ_NORMAL_TRACK:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_NORMAL_TRACK);
		REPORT_EVENT(COMM_RTE_EVENT_NORM_TRACK, 1);
		break;
	case CARD_FREQ_BRIDGE:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_BRIDGE);
		REPORT_EVENT(COMM_RTE_EVENT_BRIDGE, 1);
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		break;
	case CARD_FREQ_HOLDOVER:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_HOLDOVER);
		REPORT_EVENT(COMM_RTE_EVENT_HOLDOVER, 1);
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		break;
	case CARD_FREQ_EXTENDED_HOLDOVER:
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_EXTENDED_HOLDOVER);
		REPORT_EVENT(COMM_RTE_EVENT_EXTENDED_HOLDOVER, 1);
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		break;
	case CARD_FREQ_FAIL:
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_FAIL);
		// REPORT_EVENT(COMM_RTE_EVENT_CARD_FAIL, 1);
		break;
	case CARD_FREQ_OFFLINE:
		MonitorGpsPhase(1);
		MonitorFineGpsPhase();
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_OFFLINE);
		REPORT_EVENT(COMM_RTE_EVENT_CARD_OFFLINE, 1);
		break;
	default:
		return;
	}

	if (m_loopState != state) {
		m_loopCnt = 0L;
		m_loopState = state;
	}
	
	CCardProcess::s_pCardProcess->StateChange(1);

}

void CCardLoop::ExitLoopState(int state, int to)
{
	if (state == to) return;

	switch (state) {
	case CARD_FREQ_WARMUP:
		REPORT_EVENT(COMM_RTE_EVENT_WARMUP, 0);
		CSecondProcess::s_pSecondProcess->SetTodValid(1);
		SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_CALCULATE_LEAP_TIME);
		break;
	case CARD_FREQ_FREERUN:
		REPORT_EVENT(COMM_RTE_EVENT_FREERUN, 0);
		break;
	case CARD_FREQ_FAST_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_FAST_TRACK, 0);
		break;
	case CARD_FREQ_NORMAL_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_NORM_TRACK, 0);
		break;
	case CARD_FREQ_BRIDGE:
		REPORT_EVENT(COMM_RTE_EVENT_BRIDGE, 0);
		break;
	case CARD_FREQ_HOLDOVER:
		REPORT_EVENT(COMM_RTE_EVENT_HOLDOVER, 0);
		break;
	case CARD_FREQ_EXTENDED_HOLDOVER:
		REPORT_EVENT(COMM_RTE_EVENT_EXTENDED_HOLDOVER, 0);
		break;
	case CARD_FREQ_FAIL:
		break;
	case CARD_FREQ_OFFLINE:
		break;
	}
}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
void CCardLoop::TickWarmup()
{
	int loop;
	int lock;

	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((lock & MASK) == FLAG) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	case CIstateDti::TOD_SOURCE_GPS:
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return; 
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ June 2010 added hybrid case
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	case CIstateDti::TOD_SOURCE_USER:
		if (m_loopCnt < 20) return;
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		TransitLoop(CARD_FREQ_FREERUN);
		return;
	}

	// For now, always goes to freerun. Revisit.
	TransitLoop(CARD_FREQ_FREERUN);
}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
void CCardLoop::TickFreerun()
{
	int lock;

	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((lock & MASK) == FLAG) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID:  //GPZ June 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		if ((m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) && (m_loopCnt > 20)) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	}

}

void CCardLoop::TickFastLock()
{
	int lock;
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if (lock == CInput::LOCK_LEVEL_TOTAL) {
			TransitLoop(CARD_FREQ_NORMAL_TRACK);
			return;
		}
		if (lock & CInput::LOCK_LEVEL_LOCK_FLAG) return;
		//TransitLoop(CARD_FREQ_EXTENDED_HOLDOVER);
		TransitLoop(CARD_FREQ_BRIDGE);
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ June 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		if ((m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) && (m_bt3Gear > 4) && (m_loopCnt > 20))
		{
			if ((m_todSource == CIstateDti::TOD_SOURCE_GPS)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID))
			{ 
//				TRACE("GPZ June 2010 Exit Fast Mode Freq Cond Met \r\n");

				if(CInputSelect::s_pInputSelect->GetActiveInput()==CHAN_GPS) //GPZ June 2010 only when GPS is input
                {
	   				TRACE("GPZ June 2010 Exit Fast Mode Phase Test \r\n");

					if (m_gpsPhaseErrorOverLimit == 1) {
					TransitLoop(CARD_FREQ_NORMAL_TRACK); // For GPS only, has to wait for normal phase control
					return;
					}
				}
				else 
				{  
					TRACE("GPZ June 2010 Exit Fast Mode \r\n");
					TransitLoop(CARD_FREQ_NORMAL_TRACK);
					return;
				} 
			}
			else 
			{  
					TRACE("GPZ June 2010 Exit Fast Mode \r\n");
					TransitLoop(CARD_FREQ_NORMAL_TRACK);
					return;
			} 
		} //end if MODE, Gear, m_loop case
		else if (m_bt3Mode != CBT3OutBound::BT3_MODE_NORMAL) 
		{
			m_bridgeTime = 900L;
			TransitLoop(CARD_FREQ_BRIDGE);
			return;
		}

		if ((m_bt3Gear <= 3) || (m_todSource == CIstateDti::TOD_SOURCE_USER)
			|| (IS_STANDBY)) { // Gear 3 is good for frequency. 
			MonitorGpsPhase(1);
		} else { // Gear 4, phase can be held.
			MonitorGpsPhase(0);
		}
		MonitorFineGpsPhase();

		return;
	default:
		TransitLoop(CARD_FREQ_FREERUN);
		return;
	}

}

void CCardLoop::TickNormalLock()
{
	int lock;
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if (lock & CInput::LOCK_LEVEL_LOCK_FLAG) return;
		//TransitLoop(CARD_FREQ_EXTENDED_HOLDOVER);
		TransitLoop(CARD_FREQ_BRIDGE);
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ June 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		if ((m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) && (m_bt3Gear <= 4)) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		} else if (m_bt3Mode != CBT3OutBound::BT3_MODE_NORMAL) {
			m_bridgeTime = m_maxBridgeTime;
			TransitLoop(CARD_FREQ_BRIDGE);
			return;
		}
		if(CInputSelect::s_pInputSelect->GetActiveInput()==CHAN_GPS) //GPZ June 2010 only when GPS is input
		{
			if (((m_todSource == CIstateDti::TOD_SOURCE_GPS)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID)) && (IS_ACTIVE))
			{
				MonitorGpsPhase(0);
				if (m_gpsPhaseErrorOverLimit != 1)
				{
					TransitLoop(CARD_FREQ_FAST_TRACK);
					return;
				}
			} 
			else
			{
				MonitorGpsPhase(1);
			}
			MonitorFineGpsPhase();
		}
		return;
	default:
		TransitLoop(CARD_FREQ_FREERUN);
		return;
	}
}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
void CCardLoop::TickBridge()
{
	int lock;
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((lock & MASK) == FLAG) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		if (m_loopCnt >= 3) {
			TransitLoop(CARD_FREQ_EXTENDED_HOLDOVER);
		}
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ added hybrid case JUNE 2010
	case CIstateDti::TOD_SOURCE_GPS:
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		if (m_loopCnt >= m_bridgeTime) {
			TransitLoop(CARD_FREQ_HOLDOVER);
			return;
		}
		return;
	default:
		TransitLoop(CARD_FREQ_FREERUN);
		return;
	}

}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
void CCardLoop::TickHoldover()
{
	int lock;
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((lock & MASK) == FLAG) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		TransitLoop(CARD_FREQ_EXTENDED_HOLDOVER);
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ June 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		break;
	default:
		TransitLoop(CARD_FREQ_FREERUN);
		return;

	}

	if (m_loopCnt >= m_maxHoldTime) {
		TransitLoop(CARD_FREQ_EXTENDED_HOLDOVER);
		return;
	}
}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
void CCardLoop::TickExtendedHoldover()
{
	int lock;
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_DTI:
		lock = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((lock & MASK) == FLAG) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_HYBRID:   //Added hybrid case GPZ JUNE 2010
	case CIstateDti::TOD_SOURCE_GPS:
		if (m_bt3Mode == CBT3OutBound::BT3_MODE_NORMAL) {
			TransitLoop(CARD_FREQ_FAST_TRACK);
			return;
		}
		return;
	default:
		TransitLoop(CARD_FREQ_FREERUN);
		return;
	}
}

void CCardLoop::TickFail()
{
	// always fail
}

void CCardLoop::TickOffline()
{
	// always offline
}

int CCardLoop::GetCardLoop() const
{
	return m_loopState;
}

void CCardLoop::CfMaxHoldTime(int32 time)
{
	if (m_maxHoldTime != time) {
		m_maxHoldTime = time;
	}
}

void CCardLoop::SetTwinLoop(int loop)
{
	m_twinLoop = loop;
	m_twinLoopFresh = 0;
}

int CCardLoop::GetTwinLoop(int *pFresh /*= NULL*/) const
{
	if (pFresh) {
		*pFresh = m_twinLoopFresh;
	}
	return m_twinLoop;
}

const char * CCardLoop::GetName(int state)
{
	switch (state) {
	case CARD_FREQ_WARMUP:
		return "WARMUP";
	case CARD_FREQ_FREERUN:
		return "FREERUN";
	case CARD_FREQ_FAST_TRACK:
		return "FAST-TRACK";
	case CARD_FREQ_NORMAL_TRACK:
		return "NORMAL-TRACK";
	case CARD_FREQ_BRIDGE:
		return "BRIDGE";
	case CARD_FREQ_HOLDOVER:
		return "HOLDOVER";
	case CARD_FREQ_EXTENDED_HOLDOVER:
		return "EXT-HOLDOVER";
	case CARD_FREQ_FAIL:
		return "FAIL";
	case CARD_FREQ_OFFLINE:
		return "OFFLINE";
	default:
		return "UNKNOWN";
	}
}

void CCardLoop::CfTodSource(int source)
{
	if (m_todSource != source) {    //GPZ June 2010 extend monitoring for user and hybrid case
		if ((m_todSource == CIstateDti::TOD_SOURCE_GPS) ||
			(m_todSource == CIstateDti::TOD_SOURCE_USER)||
			(m_todSource == CIstateDti::TOD_SOURCE_HYBRID)) {
			MonitorGpsPhase(1);
			MonitorFineGpsPhase();
		}
		m_todSource = source;
	}
}

void CCardLoop::TransitLoop(int loop)
{
	int old;

	if (loop == m_loopState) return;

	old = m_loopState;
	ExitLoopState(m_loopState, loop);
	SetLoopState(loop, old);

}

// TC1000 10.24M*256/35 * (10^-3). 1ms limit
// TP5000 20.48M * (10^-3) 1ms limit: 20480 and histerisis 1.05ms = 
// GPZ June 2010 adjust to 50 microseconds 1024, 1075 with hyst
#define GPS_ADJUST_HOLD_LIMIT 1075 /* 78643L */
#define GPS_ADJUST_LIMIT  1024L /* 74898L */

// TC1000 10.24M*256/35/* (2*10^-6) 1 -- 2us
// TP5000 20.48M * 2*10^-6 = 41
#define GPS_FAST_ADJUST_HOLD_LIMIT 41 /* 150L */
#define GPS_FAST_ADJUST_LIMIT 20 /* 75L */

void CCardLoop::MonitorGpsPhase(int clear)
{
	int32 gpsPhase;
	int ctrl;

	if ((clear) || (!IS_ACTIVE)) {
		if (m_gpsPhaseErrorOverLimit != -1) {
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 0);	
			m_gpsPhaseErrorOverLimit = -1;
			CBT3Api::s_pBT3Api->SetPhaseControl(0);
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
			return;
		}
		return;
	}

	gpsPhase = CCardAlign::s_pCardAlign->GetGpsPhase();
	if (gpsPhase < 0) {
		gpsPhase = -gpsPhase;
	}

	ctrl = m_gpsPhaseErrorOverLimit;
	if (gpsPhase < GPS_FAST_ADJUST_LIMIT) {
		ctrl = 1;
	} else if (gpsPhase < GPS_FAST_ADJUST_HOLD_LIMIT) {
		if (ctrl != 2) {
			ctrl = 1;
		}
	} else if (gpsPhase < GPS_ADJUST_LIMIT) {
		ctrl = 2;
	} else if (gpsPhase < GPS_ADJUST_HOLD_LIMIT) {
		if (ctrl != 100) {
			ctrl = 2;
		}
	} else {
		ctrl = 100;
	}

	if (m_gpsPhaseErrorOverLimit != ctrl) {
		m_gpsPhaseErrorOverLimit = ctrl;
		TRACE("GPS Phase Diff = %ld\r\n", gpsPhase);
		switch (ctrl) {
		case 1:
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 0);	
			m_gpsPhaseErrorOverLimit = 1;
			CBT3Api::s_pBT3Api->SetPhaseControl(1);
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
			break;
		case 2:
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 0);	
			m_gpsPhaseErrorOverLimit = 2;
			CBT3Api::s_pBT3Api->SetPhaseControl(2);
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
			break;
		case 100:
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 1);	
			m_gpsPhaseErrorOverLimit = 100;
			CBT3Api::s_pBT3Api->SetPhaseControl(0);
			TRACE("Ready to be jammed. Frequency control only\r\n");
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 1);
			break;
		default: // should never be here
			break;
		}
		if (ctrl > 1) {
			Set_GPS_Drift_Low();
		}
	}
#if 0 
	if (m_gpsPhaseErrorOverLimit != 1) {
		if (gpsPhase < GPS_FAST_ADJUST_LIMIT)  {
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 0);	
			m_gpsPhaseErrorOverLimit = 1;
			CBT3Api::s_pBT3Api->SetPhaseControl(1);
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
			return;
		}
	} else if (gpsPhase < GPS_FAST_ADJUST_HOLD_LIMIT) {
		return;
	}

	if (m_gpsPhaseErrorOverLimit != 2) {
		if ((gpsPhase >= GPS_FAST_ADJUST_LIMIT) && (gpsPhase < GPS_ADJUST_LIMIT)) {
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 0);	
			m_gpsPhaseErrorOverLimit = 2;
			CBT3Api::s_pBT3Api->SetPhaseControl(2);
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
			Set_GPS_Drift_Low();
			return;			
		}
	} else if (gpsPhase < GPS_ADJUST_HOLD_LIMIT) {
		return;
	}

	if (m_gpsPhaseErrorOverLimit != 100) {
		if (gpsPhase >= GPS_ADJUST_LIMIT) {
			REPORT_EVENT(COMM_RTE_EVENT_CLOCK_FREQUENCY, 1);	
			m_gpsPhaseErrorOverLimit = 100;
			CBT3Api::s_pBT3Api->SetPhaseControl(0); 
			Set_GPS_Drift_Low();
			SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 1);
			return;
		}
	}
#endif

}

void CCardLoop::MonitorFineGpsPhase()
{
	int align;
	if ((m_gpsPhaseErrorOverLimit < 0) || (m_gpsPhaseErrorOverLimit >= 100)) {
		// Phase error too big.
		if (m_gpsPhaseFine != -1) {
			m_gpsPhaseFine = -1;
			REPORT_EVENT(COMM_RTE_EVENT_GPS_PHASE_PULLING, 0);
			return;
		}
		return;
	}
	align = CCardAlign::s_pCardAlign->GetCardAlign();
	if (align == CCardAlign::CARD_ALIGN_TIGHT) {
		if (m_gpsPhaseFine != 1) {
			m_gpsPhaseFine = 1;
			REPORT_EVENT(COMM_RTE_EVENT_GPS_PHASE_PULLING, 0);
		}
		return;
	} else if (align == CCardAlign::CARD_ALIGN_LOOSE) {
		if (m_gpsPhaseFine != 0) {
			m_gpsPhaseFine = 0;
			REPORT_EVENT(COMM_RTE_EVENT_GPS_PHASE_PULLING, 1);
			Set_GPS_Drift_Low();  
		}
		return;
	}

}

void CCardLoop::GetGpsModeGear()
{
	if ((m_todSource == CIstateDti::TOD_SOURCE_GPS) ||  //GPZ June 2010 extend for hybrid case
		(m_todSource == CIstateDti::TOD_SOURCE_USER)||
		(m_todSource == CIstateDti::TOD_SOURCE_HYBRID)) {
		m_bt3Mode = CBT3OutBound::s_pBT3OutBound->GetMode();
		m_bt3Gear = CBT3OutBound::s_pBT3OutBound->GetGear();
	} else {
		m_bt3Mode = CBT3OutBound::BT3_MODE_WARMUP;
		m_bt3Gear = 0;
	}
}

void CCardLoop::CardLoopHack(Ccomm *pComm)
{
	char buff[300];
	sprintf(buff, "BT3 Mode: %d GPS Gear: %d\r\n"
		, m_bt3Mode, m_bt3Gear);
	pComm->Send(buff);
	sprintf(buff, "GPS Phase Error Over Limit: %d\r\n"
		"GPS Phase Error Fine Lock: %d\r\n"
		"Bridgetime: %ld Max: %ld StateCnt: %ld\r\n"
		, m_gpsPhaseErrorOverLimit
		, m_gpsPhaseFine
		, m_bridgeTime, m_maxBridgeTime, m_loopCnt);
	pComm->Send(buff);
}

void CCardLoop::CfBridgeTime(uint32 time)
{
	m_maxBridgeTime = time;
}
