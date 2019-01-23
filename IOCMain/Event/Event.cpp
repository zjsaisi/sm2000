// Event.cpp: implementation of the CEvent class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Event.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Event.cpp 1.4 2009/04/30 17:19:11PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.4 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "Event.h"
#include "string.h"
#include "EventId.h"
#include "EventKb.h"
#include "comm.h"
#include "stdio.h"
#include "EventStat.h"
#include "AllProcess.h"
#include "EventProcess.h"
#include "rtc.h"
#include "PTPLink.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEventKb *CEvent::m_pKB = NULL;
CEventProcess *CEvent::m_pEventProcess = NULL;

int CEvent::m_allMinorAlarms = 0;
int CEvent::m_allMajorAlarms = 0;
int CEvent::m_allCriticalAlarms = 0;

// One object holds the information about current status of one event.
// The Aid1 & Aid2 forms 2-dimension array. 
// For non-continual AID, use m_pNext to chain them together.
CEvent::CEvent(CEventProcess *pProcess, CEventKb *pKB, int16 eventId, uint8 aidOneStart, uint8 aidOneEnd,
		uint8 aidTwoStart, uint8 aidTwoEnd, int8 bDoStatistic,
		uint16 flag)
{
	int i;

	m_minorAlarms = 0;
	m_majorAlarms = 0;
	m_criticalAlarms = 0;

	if (m_pKB == NULL) {
		m_pKB = pKB;
	}
	if (m_pEventProcess == NULL) {
		m_pEventProcess = pProcess;
	}

	uint8 len1 = 1, len2 = 1;
	uint16 size;

	m_flag = flag | 0x8000;
	m_eventId = eventId;
	m_aidOneStart = aidOneStart;
	m_aidOneEnd = aidOneEnd;
	m_aidTwoStart = aidTwoStart;
	m_aidTwoEnd = aidTwoEnd;
	m_pNext = NULL;

	if (m_aidOneEnd > m_aidOneStart) len1 = m_aidOneEnd - m_aidOneStart;
	if (m_aidTwoEnd > m_aidTwoStart) len2 = m_aidTwoEnd - m_aidTwoStart;
	m_size = size = (uint16)len1 * len2;
	m_pEventActiveList = new uint8[size];
	m_pEventAlarmLevel = new uint8[size];
	m_pEventUpgradeTime = new int32[size];
	m_pTimeStamp = new uint32[size];

	for (i = 0; i < size; i++) {
		m_pTimeStamp[i] = 0L;
		m_pEventActiveList[i] = 0;
		m_pEventUpgradeTime[i] = 0x7fffffff;
		m_pEventAlarmLevel[i] = COMM_ALARM_LEVEL_NULL; // No alarm, not srvice affecting
	}
	
	if (bDoStatistic) {
		m_pEventStat = new CEventStat *[size];
		for (i = 0; i < size; i++) {
			m_pEventStat[i] = new CEventStat();
		}
	} else {
		m_pEventStat = NULL;
	}
}

void CEvent::SetInitState(uint8 active, uint8 alarm)
{
	for (int i = 0; i < m_size; i++) {
		m_pEventActiveList[i] = active;
		m_pEventAlarmLevel[i] = alarm;
		if (m_pEventStat) {
			m_pEventStat[i]->InitState(active);
		}
	}
}

CEvent::~CEvent()
{

}

// return 1: State changed by this call.
// 0: state stays the same
// -1: event id mismatch
// -2: aid1, aid2 out of range.
int CEvent::ReportState(int16 eventId, uint8 active, uint8 aid1, uint8 aid2,
						uint32 timeStamp, uint8 *pAlarm, uint8 *bReportIMC)
{
	int index = 0;
	int ret = 0;
	uint8 saBit = 0;
	uint8 alarm = COMM_ALARM_LEVEL_NULL;

	if (eventId != m_eventId) return -1;

	if (m_flag & CEventKb::EVT_FLAG_TRANSIENT) {
		if (!active) {
			return 0;
		}   
		
#if 0  // Special case. Take it off 8/16/2005.		
		// For transient event, ignore AID2 to accommodate SSM change event.
		if (eventId == COMM_RTE_EVENT_INPUT_SSM_CHANGE) {
			aid2 = 0;
		}
#endif		
	}

	index = GetAidIndex(aid1, aid2);
	if (index < 0) {
		// It's not in this, it could be in others
		if (m_pNext != NULL) {
			return m_pNext->ReportState(eventId, active, aid1, aid2, timeStamp, pAlarm, bReportIMC);
		}
		return -2;
	}

	if ((m_pEventActiveList[index] == active) &&
		(*pAlarm == COMM_ALARM_LEVEL_NULL) && 
		(!(m_flag & CEventKb::EVT_FLAG_TRANSIENT)) 
		) {
		// state the same & not upgraded alarm
		return 0;
	}


	if (!(m_flag & CEventKb::EVT_FLAG_TRANSIENT)) {
		m_pEventActiveList[index] = active;
	}

	// Set alarm level of this event
	alarm = COMM_ALARM_LEVEL_NULL;
	if (active) {
		if (m_flag & CEventKb::EVT_FLAG_TRANSIENT) {
			ret = m_pKB->GetAlarmLevel(eventId, &alarm);
			// For transient event, cannot be alarm or SA
			alarm &= 0xf;
			if (alarm != COMM_ALARM_LEVEL_NULL) {
				alarm = COMM_ALARM_LEVEL_JUST_REPORT;
			}			
		} else {
			ret = m_pKB->GetAlarmLevel(eventId, &alarm);
		}
		saBit = alarm & 0x10;
	} else {
	}
	
	// Get the highest alarm as current one if it's not an update
	if (!(*pAlarm & 0x10)) {
		if ((alarm & 0xf) > (*pAlarm & 0xf)) {
			*pAlarm = alarm;
		} else {
			alarm = *pAlarm;
		}
	}

	// Put back Service affect bit
	*pAlarm |= saBit;
	alarm |= saBit;

	// New alarm status
	switch (alarm & 0xf) {
	case COMM_ALARM_LEVEL_MINOR:

		m_minorAlarms++;
		m_allMinorAlarms++;
		*bReportIMC = 1; // To alarm, report it to PPC.
		break;
	case COMM_ALARM_LEVEL_MAJOR:
		m_majorAlarms++;
		m_allMajorAlarms++;
		*bReportIMC = 1; // To alarm, report it to PPC.
		break;
	case COMM_ALARM_LEVEL_CRITICAL:
		m_criticalAlarms++;
		m_allCriticalAlarms++;
		*bReportIMC = 1; // To alarm, report it to PPC.
		break;
	case COMM_ALARM_LEVEL_JUST_REPORT:
		*bReportIMC = 1; // To NA, report it to PPC.
		break;
	case COMM_ALARM_LEVEL_NULL:
		// Don't report.
		break;
	}

	switch (m_pEventAlarmLevel[index] & 0xf) {
	case COMM_ALARM_LEVEL_MINOR:
		m_minorAlarms--;
		m_allMinorAlarms--;
		*bReportIMC = 1; // From Alarm to clear, report it to PPC
		break;
	case COMM_ALARM_LEVEL_MAJOR:
		m_majorAlarms--;
		m_allMajorAlarms--;
		*bReportIMC = 1; // From Alarm to clear, report it to PPC
		break;
	case COMM_ALARM_LEVEL_CRITICAL:
		m_criticalAlarms--;
		m_allCriticalAlarms--;
		*bReportIMC = 1; // From Alarm to clear, report it to PPC
		break;
	case COMM_ALARM_LEVEL_JUST_REPORT:
		*bReportIMC = 1; // From NA to clear, report it to PPC.
		break;
	}

	m_pEventAlarmLevel[index] = alarm;
	m_pTimeStamp[index] = timeStamp;

	// setup upgrading timer 
	if (active) {
		// So far, allow minor -> major -> critical
		m_pKB->GetUpgradeMinute(m_eventId, alarm, m_pEventUpgradeTime + index);
	} else {
		m_pEventUpgradeTime[index] = 0x7fffffff;
	}

	// Update statistic if necessary
	if (m_pEventStat) {
		uint32 now;
		now = CAllProcess::g_pAllProcess->GetAbsoluteSecond();
		m_pEventStat[index]->Update(now, active);
	}
	return 1;
}

// Return >=0 as index
// -1: out of range
int CEvent::GetAidIndex(uint8 aid1, uint8 aid2)
{
	int index;
	if ((aid1 < m_aidOneStart) || (aid1 >= m_aidOneEnd)) {
		return -1;
	}
	if ((aid2 < m_aidTwoStart) || (aid2 >= m_aidTwoEnd)) {
		return -1;
	}
	index = (int)(aid2 - m_aidTwoStart) * (m_aidOneEnd - m_aidOneStart) +
		(int)aid1 - m_aidOneStart;
	return index;
}

void CEvent::Append(CEvent *pNext)
{
	CEvent *ptr = this;
	do {
		if (ptr->m_pNext == NULL) {
			ptr->m_pNext = pNext;
			return;
		} else {
			ptr = ptr->m_pNext;
		}
	} while (1);
}

// return lines printed
// option 1: Only the active ones are printed
int CEvent::PrintCondition(Ccomm *pComm, unsigned long option)
{
	char buff[300];
	char const *pDesc;
	char const *pAcronym;
	int lines = 0;
	CRTC rtc;
	int year, month, day, hour, minute, second;

	pDesc = m_pKB->GetDebugString(m_eventId, &pAcronym);
	if (pDesc == NULL) {
		// Internal error;
		sprintf(buff, "Unknown event [%d]\r\n", m_eventId);
		pComm->Send(buff);
		return 1;
	}
	uint8 aid1, aid2;
	uint8 active;
	int index;
	for (aid2 = m_aidTwoStart; aid2 < m_aidTwoEnd; aid2++) {
		for (aid1 = m_aidOneStart; aid1 < m_aidOneEnd; aid1++) {
			index = GetAidIndex(aid1, aid2);
			active = m_pEventActiveList[index];

			// Option 1: Only active events
			if ((option == 1) && (!active)) continue;

			rtc.SetComputerSeconds(m_pTimeStamp[index]);
			rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);

			sprintf(buff, "%s[id=%02d]\t[%d,%d]\tAct=%d " 
				"Alm=%s,"
				"%s(Cd=%ld)"
				"\t%04d-%02d-%02d %02d-%02d-%02d"
				" \"%s%s\"\r\n", 
				pAcronym, m_eventId, aid2, aid1, active,
				(active == 1) ? (AlmString(m_pEventAlarmLevel[index])) : ("CL"),
				(m_pEventAlarmLevel[index] & 0x10) ? ("SA") : ("NSA"), m_pEventUpgradeTime[index],
				year, month, day, hour, minute, second,
				(active ==1) ? ("") : ("ENDED "), pDesc);
			lines++;
			pComm->Send(buff);
		}
	}
	if (m_pNext) {
		m_pNext->PrintCondition(pComm, option);
	}
	return lines;
}

// Dump condition to memory block to send out to PPC
// Flag == 0xffff or 0x8000 to retrieve all active event
// Flag non-zero to retrieve event has that flag.
int CEvent::DumpCondition(uint16 flag, int maxNumber, SConditionBlock *pBlock, uint8 *pAid1Start, uint8 *pAid2Start, int bAcitveOnly)
{
	int lines = 0; // Max up to 10 items
	int index;
	uint8 aid1 = *pAid1Start, aid2 = *pAid2Start;
    uint8 active;
    uint8 alarm;

	if ((aid1 == 0) && (aid2 == 0)) {
		aid1 = m_aidOneStart;
		aid2 = m_aidTwoStart;
	}

	if (!(flag & m_flag)) {
		return 0; // This is the event we are looking for. Skip it.
	}

	m_pKB->GetAlarmLevel(m_eventId, &alarm);

	for (; aid2 < m_aidTwoEnd; aid2++) {
		for (; aid1 < m_aidOneEnd; aid1++) {
			index = GetAidIndex(aid1, aid2);
			if (index < 0) return lines;
			active = m_pEventActiveList[index];
			if ((!active) && (bAcitveOnly)) continue; // Inactive, skip it.
			pBlock->event_id = m_eventId;
			pBlock->active = active;
#if 1		// Always use SA from current Istate
			pBlock->alarm_level = (m_pEventAlarmLevel[index] & 0xf) | ((active) ? (alarm & 0xf0) : (0));
#else		// Use SA from when it happened
			pBlock->alarm_level = m_pEventAlarmLevel[index];
#endif
			pBlock->aid1 = aid1;
			pBlock->aid2 = aid2;
			pBlock->time_stamp = m_pTimeStamp[index];
			pBlock->extra1 = 0L; // no use for now
			pBlock->extra2 = 0L; // no use for now
			lines++;
			pBlock++;
			if (lines >= maxNumber) {
				*pAid1Start = aid1 + 1;
				*pAid2Start = aid2;
				return lines;
			}
		}
		aid1 = m_aidOneStart;
	}

	// Don't have enough event to fill
	*pAid1Start = 0;
	*pAid2Start = 0;

	return lines;
}

char const * CEvent::AlmString(uint8 alm)
{
	switch (alm & 0xf) {
	case COMM_ALARM_LEVEL_NULL:
		return "NR";
	case COMM_ALARM_LEVEL_JUST_REPORT:
		return "NA";
	case COMM_ALARM_LEVEL_MINOR:
		return "MN";
	case COMM_ALARM_LEVEL_MAJOR:
		return "MJ";
	case COMM_ALARM_LEVEL_CRITICAL:
		return "CR";
	default:
		return "UNALM";
	}
}

void CEvent::TickPerMinute()
{
	int index1, index2;
	int i = -1;
	// See if any alarm need to be upgraded
	for (index2 = m_aidTwoStart; index2 < m_aidTwoEnd; index2++) {
		for (index1 = m_aidOneStart; index1 < m_aidOneEnd; index1++) {
			i++;
			if (m_pEventActiveList[i] & 1) {
				switch (m_pEventAlarmLevel[i] & 0xf) {
				case COMM_ALARM_LEVEL_MINOR:
					// Let it roll from 0 to -1. It's OK
					if ((--m_pEventUpgradeTime[i]) == 0L) {
						// Time to upgrade from minor to major
						m_pEventProcess->ReportEvent(m_eventId, 1, index1, index2, COMM_ALARM_LEVEL_MAJOR);
					}
					break;
				case COMM_ALARM_LEVEL_MAJOR:
					if ((--m_pEventUpgradeTime[i]) == 0L) {
						// Time to upgrade from major to critical
						m_pEventProcess->ReportEvent(m_eventId, 1, index1, index2, COMM_ALARM_LEVEL_CRITICAL);
					}
					break;
				default:
					//	COMM_ALARM_LEVEL_NULL,
					// COMM_ALARM_LEVEL_JUST_REPORT,
					// COMM_ALARM_LEVEL_CRITICAL,
					// For other alarms, don't do anything.
					break;
				}
			}
		}
	}
}

#if 0
void CEvent::CountAlarms(int *pMinor, int *pMajor, int *pCritical)
{
	int minor = 0, major = 0, critical = 0;
	if (m_pNext) {
		m_pNext->CountAlarms(&minor, &major, &critical);
	}
	*pMinor = minor + m_minorAlarms;
	*pMajor = major + m_majorAlarms;
	*pCritical = critical + m_criticalAlarms;

#if 0
	int minor = 0, major = 0, critical = 0;
	int next_minor, next_major, next_critical;
	int i;
	for (i = 0; i < m_size; i++) {
		switch (m_pEventAlarmLevel[i] & 0xf) {
		case COMM_ALARM_LEVEL_MINOR:
			minor++;
			break;
		case COMM_ALARM_LEVEL_MAJOR:
			major++;
			break;
		case COMM_ALARM_LEVEL_CRITICAL:
			critical++;
			break;
		}
	}
	if (m_pNext) {
		m_pNext->CountAlarms(&next_minor, &next_major, &next_critical);
	}
	*pMinor = minor + next_minor;
	*pMajor = major + next_major;
	*pCritical = critical + next_critical;
#endif
}
#endif

void CEvent::CountAllAlarms(int *pMinor, int *pMajor, int *pCritical)
{
	*pMinor = m_allMinorAlarms;
	*pMajor = m_allMajorAlarms;
	*pCritical = m_allCriticalAlarms;
}

void CEvent::RegenerateEvent(int bClearIt /* = 0 */)
{
	int aid1, aid2;
	int index;
	uint8 alarmlevel = COMM_ALARM_LEVEL_NULL;


	if (m_pNext) {
		m_pNext->RegenerateEvent(bClearIt);
	}
	
	m_pKB->GetAlarmLevel(m_eventId, &alarmlevel);

	for (aid1 = m_aidOneStart; aid1 < m_aidOneEnd; aid1++) {
		for (aid2 = m_aidTwoStart; aid2 < m_aidTwoEnd; aid2++) {
			index = GetAidIndex(aid1, aid2);
			if (index < 0) continue;

			if (m_pEventActiveList[index] == 1) {
				if ((alarmlevel != m_pEventAlarmLevel[index]) || (bClearIt)) {
					CEventProcess::s_pEventProcess->ReportEvent(m_eventId, 0, aid1, aid2);
					if (!bClearIt) 
						CEventProcess::s_pEventProcess->ReportEvent(m_eventId, 1, aid1, aid2);
				}
			}
		}
	}
}

// Update alarm level and SA bit from Istate
void CEvent::UpdateEventAlarmLevel_SA()
{
	int aid1, aid2;
	int index;

	uint8 alarmLevel = COMM_ALARM_LEVEL_NULL;

	if (m_pNext) {
		m_pNext->UpdateEventAlarmLevel_SA();
	}

	m_pKB->GetAlarmLevel(m_eventId, &alarmLevel);
	for (aid1 = m_aidOneStart; aid1 < m_aidOneEnd; aid1++) {
		for (aid2 = m_aidTwoStart; aid2 < m_aidTwoEnd; aid2++) {
			uint8 alarm;
			uint8 report = 0;

			index = GetAidIndex(aid1, aid2);
			if (index < 0) continue;
			
			if (m_pEventActiveList[index] != 1) continue;
			if (m_pEventAlarmLevel[index] == alarmLevel) continue; // same alarm level. No need to update
			alarm = alarmLevel | 0x10; // Make sure it won't be rejected as repetition
			ReportState(m_eventId, 1, aid1, aid2, m_pTimeStamp[index], &alarm, &report);
		}
	}
}

void CEvent::ResendToImc(CPTPLink *pLink)
{
	CEventProcess::SEventReport report;
	int aid1, aid2;
	int index;
	if (!(m_flag & CEventKb::EVT_FLAG_RESEND_PPC)) {
		return;
	}
	if (m_pNext != NULL) {
		m_pNext->ResendToImc(pLink);
	}

	for (aid1 = m_aidOneStart; aid1 < m_aidOneEnd; aid1++) {
		for (aid2 = m_aidTwoStart; aid2 < m_aidTwoEnd; aid2++) {
			index = GetAidIndex(aid1, aid2);
			if (index < 0) continue;
			report.cmd = 0x100;
			report.event_id = m_eventId;
			report.active = m_pEventActiveList[index];
			report.aid1 = aid1;
			report.aid2 = aid2;
			report.time_stamp = m_pTimeStamp[index];
			pLink->PostFrame((char *)&report, sizeof(CEventProcess::SEventReport), CEventProcess::PTP_IMC_PORT_AO);

		}
	}
}
