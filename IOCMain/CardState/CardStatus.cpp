// CardStatus.cpp: implementation of the CCardStatus class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardStatus.cpp 1.7 2009/05/01 09:27:25PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.7 $
 */

#include "CardStatus.h"
#include "AllProcess.h"
#include "EventId.h"
#include "Fpga.h"
#include "CardProcess.h"
#include "IstateProcess.h"
#include "IstateDti.h"
#include "machine.h"
#include "BT3Support.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCardStatus *CCardStatus::s_pCardStatus = NULL;

CCardStatus::CCardStatus()
{
	m_state = CARD_WARMUP;
	m_pFpga = NULL;
	s_pCardStatus = this;
	m_stateCnt = 0L;
	m_twinState = CARD_WARMUP;
	m_twinStateFresh = 10000;
	m_todSource = -1;
	m_isXsyncOk = 1;
}

CCardStatus::~CCardStatus()
{

}

unsigned int CCardStatus::TickCardStatus(int isWarm /* = 1 */)
{
	int active;
	int ret = 0;

	if (++m_twinStateFresh >= 20000) {
		m_twinStateFresh = 10000;
	}

	active = m_pFpga->IsFpgaActive();

	m_stateCnt++;
	switch (m_state) {
	case CARD_WARMUP:
		if (active == 1) {
			SetStatusState(CARD_COLD_ACTIVE);
			ret = 1;
		} else if (active == 0) {
			SetStatusState(CARD_COLD_STANDBY);
			ret = 1;
		}
		break;
	case CARD_COLD_ACTIVE:
		if (isWarm == 1) {
			if (active == 1) {
				SetStatusState(CARD_HOT_ACTIVE);
				ret = 1;
				break;
			} else if (active == 0) {
				SetStatusState(CARD_HOT_STANDBY);
				ret = 1;
				break;
			}
		}

		if (active == 0) {
			SetStatusState(CARD_COLD_STANDBY);
			ret = 1;
		}
		break;
	case CARD_COLD_STANDBY:
		if (isWarm == 1) {
			if (active == 1) {
				SetStatusState(CARD_HOT_ACTIVE);
				ret = 1;
				break;
			} else if (active == 0) {
				SetStatusState(CARD_HOT_STANDBY);
				ret = 1;
				break;
			}
		}
		if (active == 1) {
			SetStatusState(CARD_COLD_ACTIVE);
			ret = 1;
			break;
		}
		break;
	case CARD_HOT_ACTIVE:
		ret = TickHotActive(active);
		break;
	case CARD_HOT_STANDBY:
		if (active == 1) {
			SetStatusState(CARD_HOT_ACTIVE);
			ret = 1;
			break;
		}
		TickHotStandby();
		break;
	case CARD_FAIL:
		return 1;
	case CARD_OFFLINE:
		return 1;
	}

	return ret;

}

void CCardStatus::TickHotStandby(void)
{
	int xsync;

	if (m_stateCnt > 3) { 
		xsync = CFpga::s_pFpga->IsXsyncOk();
		if (m_isXsyncOk > 0) {
			if (xsync <= 0) {
				REPORT_EVENT(COMM_RTE_EVENT_XSYNC_LOSS, 1);
				m_isXsyncOk = 0;
			}
		} else {
			if (xsync > 0) {
				REPORT_EVENT(COMM_RTE_EVENT_XSYNC_LOSS, 0);
				m_isXsyncOk = 1;
			}
		}
	}
}

void CCardStatus::CardStatusFail()
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	if (m_state != CARD_FAIL) {
		SetStatusState(CARD_FAIL);
	}
	set_exr(tmp_exr);
}

void CCardStatus::CardStatusOffline()
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(tmp_exr);
	if (m_state != CARD_OFFLINE) {
		SetStatusState(CARD_OFFLINE);
	}
	set_exr(tmp_exr);
}

void CCardStatus::SetStatusState(int state)
{
	int old_state;
	int isActive = 0;
	uint8 tmp_exr;

	if (m_state == state) return;
	old_state = m_state;
	ExitStatusState(m_state, state);
	m_state = state;
	switch (state) {
	case CARD_COLD_ACTIVE: 
		if (old_state != CARD_HOT_ACTIVE) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_ACTIVE, 1);
		}
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_ACTIVE);
		isActive = 1;
		break;
	case CARD_HOT_ACTIVE:
		if (old_state != CARD_COLD_ACTIVE) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_ACTIVE, 1);
		}
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_ACTIVE);
		isActive = 1;
		
		if (m_todSource == CIstateDti::TOD_SOURCE_GPS) {
			if (old_state == CARD_HOT_STANDBY) {
//				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);
			}
		}
		break;
	case CARD_COLD_STANDBY:
		if (old_state != CARD_HOT_STANDBY) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_STANDBY, 1);
		}
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_STANDBY);
		break;
	case CARD_HOT_STANDBY:
		if (old_state != CARD_COLD_STANDBY) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_STANDBY, 1);
		} 
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_STANDBY);
		break;
	case CARD_FAIL:
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_FAILED);
		break;
	case CARD_OFFLINE:
		SET_MY_ISTATE(COMM_RTE_KEY_CARD_STATUS, 0, 0, CIstateDti::ISTATE_CARD_OFFLINE);
		break;
	}
	CCardProcess::s_pCardProcess->StateChange(2);
	m_stateCnt = 0L;
	if (isActive) {
		tmp_exr = get_exr();
		set_exr(7);
		if ((m_twinState == CARD_HOT_ACTIVE) || (m_twinState == CARD_COLD_ACTIVE)) {
			m_twinState = CARD_UNKNOWN;
		}
		set_exr(tmp_exr);
	}
}

void CCardStatus::ExitStatusState(int state, int new_state)
{
	switch (state) {
	case CARD_COLD_ACTIVE:
		if (new_state != CARD_HOT_ACTIVE) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_ACTIVE, 0);
		}
		break;
	case CARD_HOT_ACTIVE: // Get out of active
		if (new_state != CARD_COLD_ACTIVE) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_ACTIVE, 0);
		}
		break;
	case CARD_COLD_STANDBY:
		if (new_state != CARD_HOT_STANDBY) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_STANDBY, 0);			
		}
		break;
	case CARD_HOT_STANDBY: // Get out of standby
		if (new_state != CARD_COLD_STANDBY) {
			REPORT_EVENT(COMM_RTE_EVENT_CARD_STANDBY, 0);
		}
		if (m_isXsyncOk <= 0) {
			m_isXsyncOk = 1;
			REPORT_EVENT(COMM_RTE_EVENT_XSYNC_LOSS, 0);
		}
		break;
	case CARD_FAIL:
		break;
	case CARD_OFFLINE:
		break;
	}
}

void CCardStatus::Init()
{
	m_pFpga = CFpga::s_pFpga;
}

int CCardStatus::IsCardActive()
{
	if (m_state == CARD_HOT_ACTIVE) return HOT_ACTIVE;
	if (m_state == CARD_COLD_ACTIVE) return COLD_ACTIVE;
	return 0;
}


int CCardStatus::IsCardStandby()
{
	if (m_state == CARD_HOT_STANDBY) return HOT_STANDBY;
	if (m_state == CARD_COLD_STANDBY) return COLD_STANDBY;
	return 0;
}

int CCardStatus::GetCardStatus() 
{
	return s_pCardStatus->m_state;
}

int CCardStatus::TickHotActive(int active)
{
	if (active == 1) {
		if (m_stateCnt == 3L) {
			// Refresh event/alarm
			// 1/26/2006: After talking with Jining, looks it's not necessary.
			// SET_MY_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_RESEND_ALARM_TO_PPC);
			
			// Sync RTC
			SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_SYNC_RTC_FROM_FPGA);
		}
		return 0;
	} else if (active == 0) {
		SetStatusState(CARD_HOT_STANDBY);
		return 1;
	}
	return 0;
}

void CCardStatus::SetTwinStatus(int stat)
{
	if (m_twinState != stat) {
		m_twinState = stat;
		m_stateCnt = 0L; // should not hurt. So it can mark if the system is stable.
	}
	m_twinStateFresh = 0;
}

int CCardStatus::GetTwinStatus(int *pFresh /* = NULL*/) const
{ 
	if (pFresh) {
		*pFresh = m_twinStateFresh;
	}
	return m_twinState; 
}

int CCardStatus::IsWarmup()
{
	switch (m_state) {
	case CARD_WARMUP:
	case CARD_COLD_ACTIVE:
	case CARD_COLD_STANDBY:
		return 1;
	}
	return 0;
}

const char * CCardStatus::GetName(int state)
{
	switch (state) {
	case CARD_HOT_ACTIVE:
		return "HOT-ACTIVE";
	case CARD_COLD_ACTIVE:
		return "COLD-ACTIVE";
	case CARD_HOT_STANDBY:
		return "HOT-STANDBY";
	case CARD_COLD_STANDBY:
		return "COLD-STANDBY";
	case CARD_FAIL:
		return "FAIL";
	case CARD_OFFLINE:
		return "OFFLINE";
	case CARD_WARMUP:
		return "WARMUP";
	default:
		return "UNKNOWN";
	}
}

void CCardStatus::CfTodSource(int tod)
{
	m_todSource = tod;
}

void CCardStatus::CardStatusHack(Ccomm *pComm)
{
	char buff[200];

	sprintf(buff, "StateCnt: %ld\r\n"
		, m_stateCnt);
	pComm->Send(buff);
}

uint32 CCardStatus::GetCardStatusCnt() const
{
	return m_stateCnt;
}
