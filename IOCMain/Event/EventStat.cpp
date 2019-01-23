// EventStat.cpp: implementation of the CEventStat class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventStat.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventStat.cpp 1.1 2007/12/06 11:41:24PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "EventStat.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// This object holds statistic info of one event with specific aid. AID is not contained in
// this object. It's supposed to be used with CEvent. Corresponding AID should be obvious.
CEventStat::CEventStat()
{
	memset(m_assertSeconds, 0, sizeof(m_assertSeconds));
	memset(m_assertTimes, 0, sizeof(m_assertTimes));
	memset(m_startWith, 0, sizeof(m_startWith));
	m_lastTime = 0L;
	m_lastState = 0;

}

CEventStat::~CEventStat()
{

}

void CEventStat::InitState(uint8 state)
{
	// In statistic, don't use the third state.
	state = (state == CEvent::EVENT_TRUE) ? (CEvent::EVENT_TRUE) : (CEvent::EVENT_FALSE);
	m_startWith[0] = (uint8)state;
	m_lastState = state;
}

// Time point to the hour to retrieve
void CEventStat::GetStat(uint32 time, uint8 *pStart, uint16 *pTimes, uint16 *pSeconds, uint16 *pCurrentState)
{
	int index = (int)((time / HOUR) % STATISTIC_LENGTH);
	*pStart = m_startWith[index];
	*pTimes = m_assertTimes[index];
	*pSeconds = m_assertSeconds[index];
	if (pCurrentState != NULL) {
		*pCurrentState = m_lastState;
	}
}

void CEventStat::Update(uint32 new_time, uint8 cur_state)
{
	int old_index;
	uint32 extra_seconds;
	uint32 diff;
	int cnt;
	uint32 time; int index;
	uint32 old_time;

	old_time = m_lastTime;

	if (old_time > new_time) return;

	old_index = (int)((old_time / HOUR) % STATISTIC_LENGTH);
	extra_seconds = HOUR - (old_time % HOUR);
	diff = new_time - old_time;
	if (extra_seconds <= diff) {
		cnt = 1;
	} else {
		cnt = 0; // cnt used as control as well. It will decide if there is new partial hour
		extra_seconds = diff;
	}

	// partial hour at the beginning
	if (m_lastState == CEvent::EVENT_TRUE) {
		m_assertSeconds[old_index] += extra_seconds;
	} else if (cur_state == CEvent::EVENT_TRUE) {
		// From FALSE to TRUE
		m_assertTimes[old_index]++;
	}

	// extra whole hours
	time = old_time + extra_seconds;
	while ((time + HOUR) <= new_time) {
		index = (int)((time / HOUR) % STATISTIC_LENGTH);
		if (m_lastState == CEvent::EVENT_TRUE) {
			m_assertSeconds[index] = HOUR;
			m_startWith[index] = CEvent::EVENT_TRUE;
		} else {
			m_assertSeconds[index] = 0;
			m_startWith[index] = CEvent::EVENT_FALSE;
		}
		m_assertTimes[index] = 0;
		cnt++;
		time += HOUR;
		if (cnt > STATISTIC_LENGTH + 2) {
			// Filled all the loop, skip the rest of hours
			cnt = 1;
			time = (new_time / HOUR - 2) * HOUR; 
		}
	}
	// partial hour at the end
	index = (int)((new_time / HOUR) % STATISTIC_LENGTH);
	if (cnt) {
		// New partial hour
		extra_seconds = new_time % HOUR;
		m_startWith[index] = (extra_seconds == 0) ? (cur_state) : (m_lastState);
		m_assertTimes[index] = 0;
		m_assertSeconds[index] = 0;
		if (m_lastState == CEvent::EVENT_TRUE) {
			m_assertSeconds[index] = extra_seconds;
		} else if (cur_state == CEvent::EVENT_TRUE) {
			m_assertTimes[index] = 1;
		}
	}

	m_lastState = cur_state;
	m_lastTime = new_time;	
}
