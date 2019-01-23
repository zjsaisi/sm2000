// EventDesc.cpp: implementation of the CEventKb class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventDesc.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventKb.cpp 1.2 2008/01/28 17:18:13PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "EventId.h"
#include "EventKb.h"
#include "string.h"
#include "Istate.h"
#include "Event.h"
#include "iocEvtList.h"
#include "iocKeyList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const SEventKB *CEventKb::m_pEventKBList[NUM_EVENT_KB];

void CEventKb::CreateEventStatus()
{
	SEventKB const *pItem;
	int index;
	CEvent *pEvt;
	int8 bDoStatistic;
    int cnt = 0;
    
	pItem = m_pEventKBList[cnt++];
	while ((pItem != NULL) && (pItem->eventId != m_eventStart)) {
		
		index = pItem->eventId - m_eventStart - 1;
		
		bDoStatistic = (pItem->flag & EVT_FLAG_REPORT_STATISTIC) ? (1) : (0);
		
		// Create event
		pEvt = new CEvent(m_pEventProcess, this, pItem->eventId, 
			pItem->aidOneStart, pItem->aidOneEnd, pItem->aidTwoStart,
			pItem->aidTwoEnd, bDoStatistic, pItem->flag);
		pEvt->SetInitState(pItem->initState, COMM_ALARM_LEVEL_NULL);

		// Add new event to the list
		if (m_pEventStatusList[index] == NULL) {
			m_pEventStatusList[index] = pEvt;
		} else {
			m_pEventStatusList[index]->Append(pEvt);
		}
		// Go to next item
		pItem = m_pEventKBList[cnt++];;
	}
}

CEvent * CEventKb::GetEventStatus(int16 eventId)
{
	if ((eventId <= m_eventStart) || (eventId >= m_eventEnd)) {
		return NULL;
	}
	return m_pEventStatusList[eventId - m_eventStart - 1];
}

CEventKb::CEventKb(CIstate *pIstate, CEventProcess *pProcess)
{
	m_kbCnt = 0;
	m_pEventProcess = pProcess;
	m_eventStart = COMM_RTE_EVENT_NONE;
	m_eventEnd = COMM_RTE_EVENT_MAX;
	m_pIstate = pIstate;
	memset(m_pEventStatusList, 0, sizeof(m_pEventStatusList));
	memset(m_pEventKBList, 0, sizeof(m_pEventKBList));
}

CEventKb::~CEventKb()
{

}

// 1: for success. Alarm level return via pAlarm
// The low 4-bit will be alarm level. The bit5 is 1 for Service affecting
//	or 0 for non-service affecting 
// -1: eventId out of range
int CEventKb::GetAlarmLevel(int16 eventId, uint8 *pAlarm)
{
	int index, ret;
	uint32 alarm = COMM_ALARM_LEVEL_NULL; // To be defensive
	uint32 sa = 0; // To be defensive

	index = eventId - m_eventStart /*- 1*/;
	ret = m_pIstate->ReadOneIstateItem(COMM_RTE_KEY_ALARM_LEVEL, index, 0, &alarm);
	if ((alarm < COMM_ALARM_LEVEL_NULL) || (alarm >= COMM_ALARM_LEVEL_MAX)) {
		// Invalid alarm value. 
		alarm = COMM_ALARM_LEVEL_NULL;
	}
	// Once Service Affect setting have the same result. If the first is successful,
	// I don't see why the second one should fail.
	m_pIstate->ReadOneIstateItem(COMM_RTE_KEY_SERVICE_AFFECT, index, 0, &sa);
	
	if (sa) sa = 1L;

	if (ret == 1) {
		*pAlarm = alarm | (sa << 4);
	} else {
		*pAlarm = COMM_ALARM_LEVEL_NULL; // No alarm, not service affecting
	}

	return 1;
}

// Return a string for debug purpose about the event
// Return NULL if cannot find it.
const char * CEventKb::GetDebugString(int16 eventId, char const **pAcronym)
{
	SEventKB const *pItem;
	int cnt = 0;
	pItem = m_pEventKBList[cnt++];
	while ((pItem != NULL) && (pItem->eventId != m_eventStart)) {		
		if (pItem->eventId == eventId) {
			*pAcronym = pItem->pAcronym;
			return pItem->pDebug;
		}
		pItem = m_pEventKBList[cnt++];
	}
	return NULL;
}

// return 1: for success. *pLogged = 1 if event need to be logged
// if needn't to be logged, *pLogged = 0;
// return -1: If not found.
int CEventKb::NeedLogged(int16 eventId, uint8 *pLogged)
{
	uint32 log;
	int ret, index;

	index = eventId - m_eventStart/* - 1*/;
	ret = m_pIstate->ReadOneIstateItem(COMM_RTE_KEY_EVENT_LOG, index, 0, &log);

	if (ret == 1) {
		*pLogged = log;
	} else {
		*pLogged = 0;
	}

	return ret;
}

// return 1: If this event need to be reported as part of condition
// 0: if not part of condition(rtrv-cond)
int CEventKb::IsPartOfCondition(int16 eventId)
{
	SEventKB const *pItem;      
	int cnt = 0;
	pItem = m_pEventKBList[cnt++];

	while ((pItem != NULL) && (pItem->eventId != m_eventStart)) {
		if (pItem->eventId == eventId) {
			if (pItem->flag & EVT_FLAG_REPORT_COND) {
				return 1;
			} else {
				return 0;
			}
		}
		pItem = m_pEventKBList[cnt++];
	}
	// Cannot find it.
	return 0;
}

// Get the event id range supported by this.
void CEventKb::GetEventIdRange(int16 *pStart, int16 *pEnd)
{
	*pStart = m_eventStart;
	*pEnd = m_eventEnd;
}

// Get the minutes it takes for the alarm to go to next level.
// eventId is not used for now. 
// return 1: OK
int CEventKb::GetUpgradeMinute(int16 eventId, uint8 alarm, int32 *pMinute)
{
	*pMinute = 0x7fffffff; // Takes forever to upgrade to next level
	alarm &= 0xf;
	switch (alarm) {
	case COMM_ALARM_LEVEL_MINOR:
		m_pIstate->ReadOneIstateItem(COMM_RTE_KEY_MINOR_UPGRADE_TIME, 0, 0, 
			(uint32 *)pMinute);
		break;
	case COMM_ALARM_LEVEL_MAJOR:
		m_pIstate->ReadOneIstateItem(COMM_RTE_KEY_MAJOR_UPGRADE_TIME, 0, 0, 
			(uint32 *)pMinute);
		break;
	default:
		//COMM_ALARM_LEVEL_NULL,
		//COMM_ALARM_LEVEL_JUST_REPORT,	
		//COMM_ALARM_LEVEL_CRITICAL,
		return 1;
	}
	return 1;
}


// Return 1 on success
int CEventKb::AddEvent(const SEventKB *pEvt)
{
	if (m_kbCnt >= NUM_EVENT_KB) return -1;
	if (pEvt == NULL) return -2;
	m_pEventKBList[m_kbCnt++] = pEvt;
	return 1;
}

void CEventKb::Init()
{
	CreateEventStatus();
}

int CEventKb::GetDefaultAlarmLevel(int16 eventId)
{
	int i;
	SEventKB const *ptr;

	for (i = 0; i < m_kbCnt; i++) {
		ptr = m_pEventKBList[i];
		if (ptr == NULL) break;
		if (ptr->eventId != eventId) continue;
		return ptr->default_alarm_level;
	}
	return COMM_ALARM_LEVEL_NULL;
}

int CEventKb::GetDefaultLog(int16 eventId)
{
	int i;
	SEventKB const *ptr;

	for (i = 0; i < m_kbCnt; i++) {
		ptr = m_pEventKBList[i];
		if (ptr == NULL) break;
		if (ptr->eventId != eventId) continue;
		return ptr->default_log;
	}
	return 0;
}

int CEventKb::GetDefaultServiceAffect(int16 eventId)
{
	int i;
	SEventKB const *ptr;

	for (i = 0; i < m_kbCnt; i++) {
		ptr = m_pEventKBList[i];
		if (ptr == NULL) break;
		if (ptr->eventId != eventId) continue;
		return ptr->default_service_affect;
	}
	return 0;
}
