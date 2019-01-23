// InputEvent.cpp: implementation of the CInputEvent class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputEvent.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputEvent.cpp 1.8 2008/09/30 15:57:30PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.8 $
 */             

#include "InputEvent.h"
#include "Input.h"
#include "AllProcess.h"
#include "DtiOutput.h"
#include "machine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputEvent::CInputEvent(int chan)
{
	m_chan = chan;
	m_qualified = -1;
	m_stat = -10000;
	m_serverTrack = 0;
	m_ipv4 = 0L;
	m_serverFlag = 0xff;
}

CInputEvent::~CInputEvent()
{

}

int CInputEvent::ReportInputStatus(int stat)
{
	if (m_stat == stat) return 0;

	// Clear old event/alarm
	switch (m_stat) {
	case CInput::INPUT_STAT_OK:
		break;
	case CInput::INPUT_STAT_LOS:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_DISCONNECTED, 0, m_chan);
		break;
	case CInput::INPUT_STAT_OOF:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_OUT_OF_FRAME, 0, m_chan);
		break;
	case CInput::INPUT_STAT_AIS:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_AIS, 0, m_chan);
		break;
	case CInput::INPUT_STAT_PHASE_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_PHASE_ERROR, 0, m_chan);
		break;
	case CInput::INPUT_STAT_FREQUENCY_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_FREQUENCY_ERROR, 0, m_chan);
		break;
	case CInput::INPUT_STAT_IPLL_UNSETTLED:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_IPLL_UNSETTLED, 0, m_chan);
		break;
	case CInput::INPUT_STAT_BT3_UNSETTLED:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_BT3_UNSETTLED, 0, m_chan);
		break;
	case CInput::INPUT_STAT_POOR_SSM:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_POOR_SSM, 0, m_chan);
		break;
	case CInput::INPUT_STAT_CABLE_ADVANCE_INVALID:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_CABLE_ADVANCE_INVALID, 0, m_chan);
		break;
	case CInput::INPUT_STAT_INVALID_PATH_TRACEABILITY:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_PATH_INVALID, 0, m_chan);
		break;
	case CInput::INPUT_STAT_INVALID_TOD_MESSAGE:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_TOD_INVALID, 0, m_chan);
		break;
	case CInput::INPUT_STAT_SERVER_WARMUP:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_WARMUP, 0, m_chan);
		break;
	case CInput::INPUT_STAT_NO_RGP:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_GPS_NO_RGP, 0, m_chan);
		break;
	case CInput::INPUT_STAT_TA_TUNING_OUT_OF_RANGE:
		REPORT_EVENT(COMM_RTE_EVENT_TA_TUNE_OUT_OF_RANGE, 0, 0);
		break;
	case CInput::INPUT_STAT_TPIU_TUNING_OUT_OF_RANGE:
		REPORT_EVENT(COMM_RTE_EVENT_TPIU_TUNE_OUT_OF_RANGE, 0, 0);
		break;
	case CInput::INPUT_STAT_GPS_PATH_CRC_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_GPS_CRC_ERROR, 0, 0);
		break;
	case CInput::INPUT_STAT_GPS_ACQUIRING:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_GPS_ACQUIRING, 0, m_chan);
		break;
	default:
		break;
	}

	// Set the new event/alarm
	switch (stat) {
	case CInput::INPUT_STAT_OK:
		break;
	case CInput::INPUT_STAT_LOS:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_DISCONNECTED, 1, m_chan);
		break;
	case CInput::INPUT_STAT_OOF:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_OUT_OF_FRAME, 1, m_chan);
		break;
	case CInput::INPUT_STAT_AIS:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_AIS, 1, m_chan);
		break;
	case CInput::INPUT_STAT_PHASE_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_PHASE_ERROR, 1, m_chan);
		break;
	case CInput::INPUT_STAT_FREQUENCY_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_FREQUENCY_ERROR, 1, m_chan);
		break;
	case CInput::INPUT_STAT_IPLL_UNSETTLED:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_IPLL_UNSETTLED, 1, m_chan);
		break;
	case CInput::INPUT_STAT_BT3_UNSETTLED:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_BT3_UNSETTLED, 1, m_chan);
		break;
	case CInput::INPUT_STAT_POOR_SSM:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_POOR_SSM, 1, m_chan);
		break;
	case CInput::INPUT_STAT_CABLE_ADVANCE_INVALID:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_CABLE_ADVANCE_INVALID, 1, m_chan);
		break;
	case CInput::INPUT_STAT_SERVER_WARMUP:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_WARMUP, 1, m_chan);
		break;
	case CInput::INPUT_STAT_INVALID_PATH_TRACEABILITY:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_PATH_INVALID, 1, m_chan);
		break;
	case CInput::INPUT_STAT_INVALID_TOD_MESSAGE:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_TOD_INVALID, 1, m_chan);
		break;
	case CInput::INPUT_STAT_NO_RGP:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_GPS_NO_RGP, 1, m_chan);
		break;
	case CInput::INPUT_STAT_GPS_ACQUIRING:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_GPS_ACQUIRING, 1, m_chan);
		break;
	case CInput::INPUT_STAT_TA_TUNING_OUT_OF_RANGE:
		REPORT_EVENT(COMM_RTE_EVENT_TA_TUNE_OUT_OF_RANGE, 1, 0);
		break;
	case CInput::INPUT_STAT_TPIU_TUNING_OUT_OF_RANGE:
		REPORT_EVENT(COMM_RTE_EVENT_TPIU_TUNE_OUT_OF_RANGE, 1, 0);
		break;
	case CInput::INPUT_STAT_GPS_PATH_CRC_ERROR:
		REPORT_EVENT(COMM_RTE_EVENT_GPS_CRC_ERROR, 1, 0);
		break;
	default:
		break;
	}

	m_stat = stat;
	return 1;
}

int CInputEvent::ReportQualified(int qualified)
{
	if (m_qualified == qualified) return 0;

	m_qualified = qualified;
	switch (qualified) {
	case CInput::INPUT_QUALIFY_DISQ:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_DISQUALIFIED, 1, m_chan);
		break;
	case CInput::INPUT_QUALIFY_OK:
	case CInput::INPUT_QUALIFY_IGNORE:
	default:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_DISQUALIFIED, 0, m_chan);
		break;
	}
	return 1;
}

int CInputEvent::ReportServerTrack(int track, int32 *pIstateVariable) // For DTI input only
{
	int report = 0;

	if (m_serverTrack == track) return 0;
	
	switch (m_serverTrack) {
	case CDtiOutput::SERVER_WARMUP:
		// do it in input stat
		break;
	case CDtiOutput::SERVER_FREERUN:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_FREERUN, 0, m_chan);
		break;
	case CDtiOutput::SERVER_FAST_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_FAST_TRACK, 0, m_chan);
		break;
	case CDtiOutput::SERVER_NORMAL_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_NORMAL_TRACK, 0, m_chan);
		break;
	case CDtiOutput::SERVER_HOLDOVER:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_HOLDOVER, 0, m_chan);
		break;
	default:
		break;
	}

	switch (track) {
	case CDtiOutput::SERVER_WARMUP:
		// Do it in input stat
		report = 1;
		break;
	case CDtiOutput::SERVER_FREERUN:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_FREERUN, 1, m_chan);
		report = 2;
		break;
	case CDtiOutput::SERVER_FAST_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_FAST_TRACK, 1, m_chan);
		report = 3;
		break;
	case CDtiOutput::SERVER_NORMAL_TRACK:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_NORMAL_TRACK, 1, m_chan);
		report = 4;
		break;
	case CDtiOutput::SERVER_HOLDOVER:
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SERVER_HOLDOVER, 1, m_chan);
		report = 5;
		break;
	default:
		break;
	}

	m_serverTrack = track;

	uint8 tmp_exr = get_exr();
	set_exr(7);
	*pIstateVariable = (int32)report;
	set_exr(tmp_exr);
	return 1;
}

void CInputEvent::ReportServerIpv4(uint32 ip)
{
	if (m_ipv4 != ip) {
		m_ipv4 = ip;
		// Send to Istate
	}

}

void CInputEvent::ReportPerformanceStable(unsigned char flag)
{
	if (m_serverFlag != flag) {
		m_serverFlag = flag;
		if (flag & CDtiOutput::CLIENT_PERFORMANCE_STABLE) {
			REPORT_EVENT(COMM_RTE_EVENT_INPUT_PERFORMANCE_UNSTABLE, 0, m_chan);
		} else {
			REPORT_EVENT(COMM_RTE_EVENT_INPUT_PERFORMANCE_UNSTABLE, 1, m_chan);
		}
	}
}
