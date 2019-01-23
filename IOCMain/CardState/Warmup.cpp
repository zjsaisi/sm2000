// Warmup.cpp: implementation of the CWarmup class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: Warmup.cpp 1.19 2010/10/14 00:19:41PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.19 $
 */

#include "Warmup.h"
#include "stdio.h"
#include "HouseProcess.h"
#include "EventId.h"
#include "AllProcess.h"
#include "TwinProxy.h"
#include "IstateDti.h"
#include "Fpga.h"
#include "CardStatus.h"
#include "SecondProcess.h"
#include "IstateProcess.h"
#include "InputSelect.h"
#include "Input.h"
#include "BT3Support.h"
#include "BT3Api.h"
#include "CardAlign.h"
#include "AdcConverted.h"
#include "Inputgps.h"

#define TRACE TRACE_HOUSE
// 15 minutes according to George Z 4/27/2006
#define STANDBY_ACQUIRE_TIME 900

#define INITIAL_DELAY 30

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWarmup *CWarmup::s_pWarmup = NULL;
uint32 CWarmup::m_timeSpentInWarmup[2] = {0, 0};

CWarmup::CWarmup()
{
	m_pAdc = NULL;
	m_todSource = 0;

	s_pWarmup = this;
	m_leastDelay = INITIAL_DELAY;
	m_isWarm = WARM_UNKNOWN;
	m_istateInited = 0;
	m_subtendLocked = 0;
	m_twinHotCnt = 0L;
	m_toResetBt3 = -1;
	m_warmupStateCnt = 0L;
	m_twinHotStandbyCnt = 0;
	m_rubLockCnt = 0;
	m_rubTimeout = RUB_LOCK_MAXIMUM; // Allow 1 hour for rubidium to lock
}

CWarmup::~CWarmup()
{

}

// (10.24M*256/35) * (10*10^-6) = 748.9828
#define GPS_PHASE_WARMUP_LIMIT 749L

// 2: Already warmup
// 1: Finished warmup in this tick
// 0: Still working
int CWarmup::TickWarmup()
{
	if (m_isWarm == WARM_COMPLETE) return 2;

	m_warmupStateCnt++;
	m_timeSpentInWarmup[INDEX_SUBSTATE]++;
	m_timeSpentInWarmup[INDEX_TOTAL_WARMUP]++;

	switch (m_isWarm) {
	case WARM_INIT:
		GotTod(0);
		CFpga::s_pFpga->GotImcJam(1);
		if (m_leastDelay) {
			if (m_leastDelay > 6) {
				if (m_istateInited) m_leastDelay = 7;
			} else if (m_leastDelay == 6) {
				if (m_istateInited == 0) {
					TRACE("Istate done by warmup object\r\n");
					CAllProcess::g_pAllProcess->IstateInited();
				}
			} else if (m_leastDelay == 1) {
				//REPORT_EVENT(COMM_RTE_EVENT_WARMUP, 1);
				InitTimer();
			}
			m_leastDelay--;
			CFpga::s_pFpga->UnlockPll();
			if (m_leastDelay < INITIAL_DELAY) {
				CFpga::s_pFpga->SetReady(1); // always ready approach suggested by Bob H on 1/24/2006.
			}
			return 0;
		}

		if (m_toResetBt3 != CBT3InBound::RESET_NORMAL_COLD) {
			int32 left;
			left = m_fixTimer.TimeLeft();
			if (left < 300000L) {
				m_toResetBt3 = CBT3InBound::RESET_NORMAL_COLD;
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_NORMAL_COLD);
				return 0; 
			}
		}

		if (CFpga::s_pFpga->IsRubidium() == 1) {
			// Look at lock bit
			if (CFpga::s_pFpga->IsRubLocked()) {
				if (m_rubLockCnt++ < 120) {
					return 0;
				}
			} else {
				TickRubTimeout();
				m_rubLockCnt = 0;
				return 0;
			}
		} else if (m_fixTimer.TimeOut() <= 0) {
			if (CTwinProxy::s_pTwinProxy->GetShelfLife() > 1800) {
				if (m_fixTimer.TimeLeft() > 10000) {
					m_fixTimer.Start(10000);
				}
			}
			return 0;
		}

		CFpga::s_pFpga->UnlockPll();
		CFpga::s_pFpga->SetTodSource(m_todSource);
		m_subtendLocked = 0;
		//GPZ June 2010 added hybrid mode
		if ((m_todSource == CIstateDti::TOD_SOURCE_GPS)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID))
		 {
			SetWarmupState(WARM_GPS);
			return 0;
		} else {
			SetWarmupState(WARM_OSC);
			if (m_toResetBt3 != CBT3InBound::RESET_WARM_START) {
				m_toResetBt3 = CBT3InBound::RESET_WARM_START;
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);
			}
		}
		return 0;
	case WARM_GPS:
		//GPZ June 2010 added hybrid mode
		if ((m_todSource != CIstateDti::TOD_SOURCE_GPS)&&(m_todSource != CIstateDti::TOD_SOURCE_HYBRID)) {
			TRACE("Warmup: TODSource changed to %d\r\n", m_todSource);
			SetWarmupState(WARM_INIT); // gosh, changed your mind?
			return 0;
		}
		if (IS_STANDBY) {
			int twin;
			twin = CCardStatus::s_pCardStatus->GetTwinStatus();
			if (m_warmupStateCnt >= m_twinHotCnt) {
				if (m_twinHotCnt > STANDBY_ACQUIRE_TIME) {
					SetWarmupState(WARM_COMPLETE);
					return 1;
				}
				m_twinHotCnt++;
			} else {
				m_twinHotCnt = m_warmupStateCnt - 1;
			}
			if (twin != CCardStatus::CARD_HOT_ACTIVE) {
				m_twinHotCnt = 0L;
			} else if (m_toResetBt3 != CBT3InBound::RESET_WARM_START) {
				m_toResetBt3 = CBT3InBound::RESET_WARM_START;
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);
			}

			return 0;
		}
		if (CBT3OutBound::s_pBT3OutBound->GetMode() != CBT3OutBound::BT3_MODE_NORMAL) {
			return 0;
		}
		// reach gear 3 and wait for jam
		// then wait for gear 4 to claim out of warmup.
		if (CBT3OutBound::s_pBT3OutBound->GetGear() < 3) {
			return 0;
		}
		if (CInputGps::s_pInputGps->GetReportedStatus() !=  CInput::INPUT_STAT_OK) {//GPZ June 2010 use RGP quality
 //		if (CBT3OutBound::s_pBT3OutBound->GetGPSReg() != 0) {
			return 0;
		}

		SetWarmupState(WARM_OSC);
		return 0;
	case WARM_OSC:
		// Wait for TOD  
		if (GotTod(1) == 0) {
			if ((m_todSource == CIstateDti::TOD_SOURCE_GPS)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID)) {
				if (IS_ACTIVE) {
					// Make the mode/gear/register is still ok
					if (CBT3OutBound::s_pBT3OutBound->GetMode() != CBT3OutBound::BT3_MODE_NORMAL) {
						SetWarmupState(WARM_INIT);
						return 0;
					}
					if (CBT3OutBound::s_pBT3OutBound->GetGear() < 3) {
						SetWarmupState(WARM_INIT);
						return 0;
					}
					if (CBT3OutBound::s_pBT3OutBound->GetGPSReg() != 0) {
						SetWarmupState(WARM_INIT);
						return 0;
					}
				} else {
					TRACE("Warmup: Going inactive in WARM_OSC\r\n");
					SetWarmupState(WARM_INIT);
					return 0;
				}
			}
			return 0;
		}
		
		// Add further condition here
		// For now, just ok it.
		CSecondProcess::s_pSecondProcess->UpdateRtc();
		if (IS_ACTIVE) {
			CSecondProcess::s_pSecondProcess->ApplyTodSource(m_todSource);
		} 
        //GPZ June 2010 added hybrid mode
		if (((m_todSource == CIstateDti::TOD_SOURCE_GPS)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID))
			 && (IS_ACTIVE)) {
			SetWarmupState(WARM_POST_GPS);
			return 0;
		}

		SetWarmupState(WARM_COMPLETE);
		return 1;
	case WARM_POST_GPS: {
		int bt3Mode;

		if (!IS_ACTIVE) {
			TRACE("Warmup: Going inactive\r\n");
			SetWarmupState(WARM_INIT);
			return 0;
		}
		//GPZ JUNE 2010 added hybrid mode 
		if ((m_todSource != CIstateDti::TOD_SOURCE_GPS)&&(m_todSource != CIstateDti::TOD_SOURCE_HYBRID))
		 {
			TRACE("Warmup: TodSource changed to %d from GPS\r\n", m_todSource);
			SetWarmupState(WARM_INIT);
			return 0;
		}
		bt3Mode = CBT3OutBound::s_pBT3OutBound->GetMode();
		if (bt3Mode != CBT3OutBound::BT3_MODE_NORMAL) {
			TRACE("Warmup: BT3 mode changed to %d\r\n", bt3Mode);
			SetWarmupState(WARM_INIT);
			return 0;
		}

		// Wait for gear 4
		if (CBT3OutBound::s_pBT3OutBound->GetGear() < 4) {
			return 0;
		}
		// GPZ June 2010 protect exit from warmup if there is no GPS
		if (CInputGps::s_pInputGps->GetReportedStatus() !=  CInput::INPUT_STAT_OK) {
//		if (CBT3OutBound::s_pBT3OutBound->GetGPSReg() != 0) {
			return 0;
		}

		if ( CCardAlign::s_pCardAlign->GetGpsPhase() > GPS_PHASE_WARMUP_LIMIT) {
			// very bad. Restart
			SetWarmupState(WARM_INIT);
			return 0;
		}
		SetWarmupState(WARM_COMPLETE);
		return 1;
						}
	case WARM_OFFLINE:
	case WARM_FAIL:
		return 0;
	default:
		SetWarmupState(WARM_INIT);
		m_timeSpentInWarmup[INDEX_TOTAL_WARMUP] = 0L;
		return 0;
	}
	
	return 0;
}

void CWarmup::SetWarmupState(int state)
{
	int old = m_isWarm;
	
	if (old == state) return;
	
	if (old == WARM_INIT) {
		// Start OCXO current monitor
		// Low end of Timeprovider OCXO is 5mA
		// zmiao 4/18/2008: Bob told me the following values. 
		if (CFpga::s_pFpga->IsRubidium()) {
			m_rubTimeout = RUB_LOCK_MAXIMUM;
			m_pAdc->SetNormalRange(CAdc::ADC_CHAN_OCXO_CURRENT, 20.0, 2600.0);
		} else {
			m_pAdc->SetNormalRange(CAdc::ADC_CHAN_OCXO_CURRENT, 5.0, 550.0);
		}
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_1V8, 1.8 * 0.9, 1.8 * 1.1);
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_10M_EFC, 0.3, 2.0);
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_20M_EFC, 0.3, 2.0);
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_25M_EFC, 0.3, 2.0);
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_24M5_EFC, 0.3, 2.0);
		m_pAdc->SetNormalRange(CAdc::ADC_CHAN_24M7_EFC, 0.3, 2.0);
	}

	m_isWarm = state;
	switch (state) {
	case WARM_INIT:
		CBT3Api::s_pBT3Api->SetPhaseControl(0);	
		m_toResetBt3 = -1;
		break;
	case WARM_OSC:
		CFpga::s_pFpga->GotImcJam(1);
		TRACE("Ready to be jammed: WARMUP\r\n");
		SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 1);
		break;
	case WARM_POST_GPS:
		CBT3Api::s_pBT3Api->SetPhaseControl(1);
		break;
	case WARM_COMPLETE:
		SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
		TRACE("Warmup Completed\r\n");
		break;
	}
	switch (old) {
	case WARM_OSC:
		SET_MY_ISTATE(COMM_RTE_KEY_OSC_READY, 0, 0, 0);
		break;
	}

	m_warmupStateCnt = 0L;
	m_timeSpentInWarmup[INDEX_SUBSTATE] = 0L;
	m_twinHotCnt = 0L;
	m_twinHotStandbyCnt = 0;

	TRACE("Warmup state: %d(%s) --> %d(%s)\r\n"
		, old, GetWarmName(old), state, GetWarmName(state));

	SET_MY_ISTATE(COMM_RTE_KEY_WARMUP_STATE, 0, 0, state);
}

void CWarmup::Init()
{
	m_pAdc = CHouseProcess::s_pHouseProcess->GetAdc();
	if (CAllProcess::GetSlotId()) {
		m_leastDelay += 5;
		TRACE("RTE in Slot B++++++++++++++++++++++++++++++++++++++++++\r\n"); 
	}
}

void CWarmup::InitTimer()
{
	// 8/23/2006. Make it 15 minutes
//	m_fixTimer.Start(900000L); // 10 minutes according to Tom F's doc.
	m_fixTimer.Start(600000L); // GPZ JUne 2010 Ten minutes should be fine
}

// return 1: warmup done.
// return 0: Not done.
int CWarmup::IsWarm()
{
	return (m_isWarm == WARM_COMPLETE) ? 1 : 0;
}

void CWarmup::IstateDone()
{
	m_istateInited = 1;
}

void CWarmup::CardWarmupFail()
{
	if (m_isWarm != WARM_FAIL) {
		REPORT_EVENT(COMM_RTE_EVENT_WARMUP, 0); // make sure the event is taken off
		SetWarmupState(WARM_FAIL);
	}
}

void CWarmup::CardWarmupOffline()
{
	if (m_isWarm != WARM_OFFLINE) {
		REPORT_EVENT(COMM_RTE_EVENT_WARMUP, 0); // make sure the event is taken off
		SetWarmupState(WARM_OFFLINE);
	}
}

#undef MASK
#undef FLAG
#define MASK (CInput::LOCK_LEVEL_LOCK_FLAG | CInput::LOCK_LEVEL_CRC_NOISY | CInput::LOCK_LEVEL_PHASE_ERROR)
#define FLAG (CInput::LOCK_LEVEL_LOCK_FLAG)
int CWarmup::GotTod(int clear)
{
	int ret;
	int twinState;
	int myState;

	myState = CCardStatus::GetCardStatus();
	twinState = CCardStatus::s_pCardStatus->GetTwinStatus();
	// With active RTE as twin, don't wait for TOD. 
	if (twinState == CCardStatus::CARD_HOT_ACTIVE) {
		if (m_twinHotCnt++ > STANDBY_ACQUIRE_TIME) {
			return 1;
		}
		if ((m_toResetBt3 != CBT3InBound::RESET_WARM_START) && (clear)) {
			m_toResetBt3 = CBT3InBound::RESET_WARM_START;
			CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);
		}
		return 0;
	} else {
		m_twinHotCnt = 0L;
	}

#if 0 // It's not of much use
	if (twinState == CCardStatus::CARD_HOT_STANDBY) {
		// I am warmup, the other is standby. Better let twin go active.
		m_twinHotStandbyCnt++;
		if ((CTwinProxy::s_pTwinProxy->GetFresh() < 10) && (m_twinHotStandbyCnt > 20)) {
			CFpga::s_pFpga->Transfer();
			return 0;
		} 
	} else {
		m_twinHotStandbyCnt = 0;
	}
#endif

	//	Depending on operation, might get tod from different sources
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_RTC:
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_NTP:
		if (myState == CCardStatus::CARD_COLD_STANDBY) { // 5/4/2006: Wait for the other guy
			return 0;
		}
		// Set from PPC.
		ret = CFpga::s_pFpga->GotImcJam(clear);
		if (ret) {
			TRACE("PPC Jam received in warmup (todsource=%d)\r\n", m_todSource);
			return 1;
		}
		return 0;
	case CIstateDti::TOD_SOURCE_HYBRID:  //GPZ June 2010 Added Hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		if (myState == CCardStatus::CARD_COLD_STANDBY) {
			return 0;
		}
		ret = CFpga::s_pFpga->GotImcJam(clear);
		if (ret) {
			TRACE("PPC Jam received in warmup (todsource=%d)\r\n", m_todSource);
			return 1;
		}
		return 0;
	case CIstateDti::TOD_SOURCE_DTI:
		if (myState == CCardStatus::CARD_COLD_STANDBY) {
			m_subtendLocked = 0;
			return 0;
		}
		// Get it from input
		ret = CInputSelect::s_pInputSelect->IsSelectLocked();
		if ((ret & MASK) == FLAG) {
			m_subtendLocked++;
			TRACE("Locked cnt: %d\r\n", m_subtendLocked);
			if (m_subtendLocked >= 10) return 1;
			return 0;
		} else {
			m_subtendLocked = 0;
		}
		//TRACE("Lock flag=0x%x masked=0x%x\r\n", ret, ret & MASK);
		return 0;
	}
	return 0;
}

void CWarmup::CfWarmupTodSource(int source)
{
	if (m_todSource == source) return;
	m_todSource = source;
}

int CWarmup::IsOvenWarm()
{
	if (m_isWarm == WARM_INIT) {
		if (m_fixTimer.TimeOut() <= 0)
			return 0;
	}
	return 1;
}


const char * CWarmup::GetWarmName(int state)
{
	switch (state) {
	case WARM_INIT:
		return "WARM-INIT";
	case WARM_GPS:
		return "WARM-GPS";
	case WARM_OSC:
		return "WARM-OSC";
	case WARM_POST_GPS:
		return "WARM-POST-GPS";
	case WARM_COMPLETE:
		return "WARM-COMPLETE";
	case WARM_FAIL:
		return "WARM-FAIL";
	case WARM_OFFLINE:
		return "WARM-OFFLINE";
	default:
		return "WARM-UNKNOWN";
	}
}

void CWarmup::WarmupHack(unsigned long *param, Ccomm *pComm)
{
	char buff[200];
	int8 needHelp = 1;

	sprintf(buff, "State: %d(%s) todSource=%d \r\n", m_isWarm, GetWarmName(m_isWarm), m_todSource);
	pComm->Send(buff);
	if (CFpga::s_pFpga->IsRubidium() == 1) {
		sprintf(buff, "Rubdium wait: %d to 120\r\nTimeout=%d\r\n"
			, m_rubLockCnt, m_rubTimeout);
		pComm->Send(buff);
	}
	sprintf(buff, "Standby Cnt: %ld\r\n", m_twinHotCnt);
	pComm->Send(buff);

	if (param[0] == 1) {
		if (m_twinHotCnt < STANDBY_ACQUIRE_TIME) {
			m_warmupStateCnt = STANDBY_ACQUIRE_TIME + 1;
			m_twinHotCnt = STANDBY_ACQUIRE_TIME;
			pComm->Send("Speed up standby warmup\r\n");
		}
		needHelp = 0;
	}

	if (needHelp) {
		char *ptr = "Help Info:\r\n\twarm 1: Speed up warmup in standby\r\n";
		pComm->Send(ptr);
	}
}

void CWarmup::TickRubTimeout(void)
{
	if (m_rubTimeout-- <= 0) {
		m_rubTimeout = 0;
		REPORT_EVENT(COMM_RTE_EVENT_RUBIDIUM_UNLOCK, 1);
		CAllProcess::g_pAllProcess->CardFail(CAllProcess::CARD_FAIL_INDEX_RUBIDIUM_UNLOCK);
	}
}
