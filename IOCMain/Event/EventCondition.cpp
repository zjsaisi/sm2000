// EventCondition.cpp: implementation of the CEventCondition class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventCondition.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventCondition.cpp 1.1 2007/12/06 11:41:22PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "EventId.h"
#include "EventCondition.h"
#include "EventKb.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventCondition::CEventCondition(CEventKb *pKB)
{
	int num;
	int16 start, end;
	m_pKB = pKB;
	m_pKB->GetEventIdRange(&start, &end);
	num = end - start - 1;

}

CEventCondition::~CEventCondition()
{

}

// This is called by event process to update event condition.
// 1: State changed by this call
// 0: stay the same.
// -1: error
int CEventCondition::ReportCondition(int16 eventId, uint8 active, 
			uint8 aid1, uint8 aid2, uint32 timeStamp, uint8 *pAlarm, uint8 *bReportIMC)
{
	int changed;
	CEvent *pEvt;
	pEvt = m_pKB->GetEventStatus(eventId);
	if (pEvt == NULL) return -1;
	changed = pEvt->ReportState(eventId, active, aid1, aid2, timeStamp, pAlarm, bReportIMC);
	return changed;
}

void CEventCondition::UpdateCondition(int16 eventId)
{
	CEvent *pEvt;

	pEvt = m_pKB->GetEventStatus(eventId);
	if (pEvt == NULL) return;
	pEvt->UpdateEventAlarmLevel_SA();
}

void CEventCondition::OpenCondition()
{
	m_pv.P();
}

void CEventCondition::CloseCondition()
{
	m_pv.V();
}

// Do alarm upgrade
void CEventCondition::TickPerMinute()
{
	int16 evt;
	int16 start, end;
	CEvent *pEvt;

	m_pKB->GetEventIdRange(&start, &end);
	for (evt = start + 1; evt < end; evt++) {
		pEvt = m_pKB->GetEventStatus(evt);
		if (pEvt) {
			pEvt->TickPerMinute();
		}
	}
}

