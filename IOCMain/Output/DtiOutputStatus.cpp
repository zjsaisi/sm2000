// DtiOutputStatus.cpp: implementation of the CDtiOutputStatus class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputStatus.cpp 1.2 2008/01/28 17:18:15PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include "DtiOutputStatus.h"
#include "AllProcess.h"
#include "DtiOutput.h"
#include "IstateProcess.h"
#include "IstateDti.h"
#include "CardStatus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDtiOutputStatus *CDtiOutputStatus::s_pDtiOutputStatus[NUM_OUTPUT_CHAN];

CDtiOutputStatus::CDtiOutputStatus(int chan)
{
	m_chan = chan;
	m_connected = 1;
	m_loopStatus = 0;
	m_cableAdvanceValid = -1;
	m_outputPerformanceStable = -1;
	m_testing = 0;
	m_problem = OUTPUT_NO_PROBLEM;
	if ((chan >= 0) && (chan < NUM_OUTPUT_CHAN)) {
		s_pDtiOutputStatus[chan] = this;
	}
	m_clockType = -1;
	m_dtiVersion = -2;
}

CDtiOutputStatus::~CDtiOutputStatus()
{

}

void CDtiOutputStatus::ReportConnect(int connected)
{
	if (m_connected != connected) {
		if (connected) {
			REPORT_EVENT(COMM_RTE_EVENT_OUTPUT_DISCONNECT, 0, m_chan);
			m_problem &= ~OUTPUT_PROBLEM_DISC;
		} else {
			REPORT_EVENT(COMM_RTE_EVENT_OUTPUT_DISCONNECT, 1, m_chan);
			m_problem |= OUTPUT_PROBLEM_DISC;
		}
		m_connected = connected;
	}
}

void CDtiOutputStatus::ReportLoopStatus(int loop)
{
	if (m_loopStatus != loop) {
		ExitLoop(m_loopStatus);
		EnterLoop(loop);
		m_loopStatus = loop;
	}
}

void CDtiOutputStatus::EnterLoop(int loop)
{
	int active;
	int problem = OUTPUT_NO_PROBLEM;

	active = IS_ACTIVE;
	switch (loop) {
	case CDtiOutput::CLIENT_WARMUP:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_WARMUP);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_WARMUP, 1, m_chan);
		problem |= OUTPUT_PROBLEM_LOOP;
		break;
	case CDtiOutput::CLIENT_FREERUN:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_FREERUN);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_FREERUN, 1, m_chan);
		problem |= OUTPUT_PROBLEM_LOOP;
		break;
	case CDtiOutput::CLIENT_FAST_TRACK:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_FAST_TRACK);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_FAST_TRACK, 1, m_chan);
		break;
	case CDtiOutput::CLIENT_NORMAL_TRACK:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_NORMAL_TRACK);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_NORMAL_TRACK, 1, m_chan);
		break;
	case CDtiOutput::CLIENT_HOLDOVER:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_HOLDOVER);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_HOLDOVER, 1, m_chan);
		problem |= OUTPUT_PROBLEM_LOOP;
		break;
	case CDtiOutput::CLIENT_BRIDGE:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_BRIDGE);
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_BRIDGE, 1, m_chan);
		problem |= OUTPUT_PROBLEM_LOOP;
		break;
	default:
		if (active) SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_STATUS, m_chan, 0, CIstateDti::CLIENT_STAT_NA);
		break;
	}

	if (problem == OUTPUT_NO_PROBLEM) {
		m_problem &= ~OUTPUT_PROBLEM_LOOP;	
	} else {
		m_problem |= problem;
	}
}

void CDtiOutputStatus::ExitLoop(int loop)
{
	switch (loop) {
	case CDtiOutput::CLIENT_WARMUP:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_WARMUP, 0, m_chan);
		break;
	case CDtiOutput::CLIENT_FREERUN:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_FREERUN, 0, m_chan);
		break;
	case CDtiOutput::CLIENT_FAST_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_FAST_TRACK, 0, m_chan);
		break;
	case CDtiOutput::CLIENT_NORMAL_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_NORMAL_TRACK, 0, m_chan);
		break;
	case CDtiOutput::CLIENT_HOLDOVER:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_HOLDOVER, 0, m_chan);
		break;
	case CDtiOutput::CLIENT_BRIDGE:
		REPORT_EVENT(COMM_RTE_EVENT_CLIENT_BRIDGE, 0, m_chan);
		break;
	default:
		break;
	}

}

void CDtiOutputStatus::RefreshLoopStatus()
{
	EnterLoop(m_loopStatus);
}

void CDtiOutputStatus::ReportCableDelay(int32 lsb)
{

}

void CDtiOutputStatus::ReportCableAdvanceValid(int valid)
{
	if (m_cableAdvanceValid != valid) {
		if (valid) {
			REPORT_EVENT(COMM_RTE_EVENT_CLIENT_CABLE_ADVANCE_INVALID, 0, m_chan);
			m_problem &= ~OUTPUT_PROBLEM_CABLE_ADVANCE;
		} else {
			REPORT_EVENT(COMM_RTE_EVENT_CLIENT_CABLE_ADVANCE_INVALID, 1, m_chan);
			m_problem |= OUTPUT_PROBLEM_CABLE_ADVANCE;
		}
		m_cableAdvanceValid = valid;
	}
}

void CDtiOutputStatus::ReportPerformanceStable(int stable)
{
	if (m_outputPerformanceStable != stable) {
		if (stable) {
			REPORT_EVENT(COMM_IOC_EVNET_CLIENT_PERFORMANCE_UNSTABLE, 0, m_chan);
			m_problem &= ~OUTPUT_PROBLEM_PERFORMANCE;
		} else {
			REPORT_EVENT(COMM_IOC_EVNET_CLIENT_PERFORMANCE_UNSTABLE, 1, m_chan);
			m_problem |= OUTPUT_PROBLEM_PERFORMANCE;
		}
		m_outputPerformanceStable = stable;
	}
}

void CDtiOutputStatus::ReportTesting(int test)
{
	if (m_testing == test) return;
	m_testing = test;
	if (test) {
		REPORT_EVENT(COMM_RTE_EVENT_OUTPUT_TESTING, 1, m_chan);
	} else {
		REPORT_EVENT(COMM_RTE_EVENT_OUTPUT_TESTING, 0, m_chan);
	}
}

int CDtiOutputStatus::GotProblem()
{
	int chan;
	CDtiOutputStatus *pStat;
	int problem = OUTPUT_NO_PROBLEM;

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pStat = s_pDtiOutputStatus[chan];
		if (pStat != NULL) {
			problem |= pStat->m_problem;
		}
	}
	return problem;
}

void CDtiOutputStatus::ReportClockType(int type)
{
	if (m_clockType != type) {
		SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_CLOCK_TYPE, m_chan, 0, type);
		m_clockType = type;
	}
}

void CDtiOutputStatus::ReportDtiVersion(int version)
{
	if (version < 0) version = -1; // Safe guard.
	if (m_dtiVersion != version) {
		SET_MY_ISTATE(COMM_RTE_KEY_CLIENT_DTI_VERSION, m_chan, 0, version);
		m_dtiVersion = version;
	}
}
