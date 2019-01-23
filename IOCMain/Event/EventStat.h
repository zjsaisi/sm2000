// EventStat.h: interface for the CEventStat class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventStat.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventStat.h 1.1 2007/12/06 11:41:24PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_EVENTSTAT_H__17E4B832_4539_419B_8377_BA69F7F729CA__INCLUDED_)
#define AFX_EVENTSTAT_H__17E4B832_4539_419B_8377_BA69F7F729CA__INCLUDED_


class CEventStat  
{
public:
	CEventStat();
	virtual ~CEventStat();
	void InitState(uint8 state);
	enum { STATISTIC_LENGTH = 26 /* 25 hour + */, HOUR = 3600 /*300*/ };
	void Update(uint32 new_time, uint8 cur_state);
	void GetStat(uint32 time, uint8 *pStart, uint16 *pTimes, uint16 *pSeconds
		, uint16 *pCurrentState = NULL);
private:
	uint16 m_assertSeconds[STATISTIC_LENGTH]; // Duration in the hour that the state stays asserting.
	uint16 m_assertTimes[STATISTIC_LENGTH]; // Times in the hour that the state gets from de-assert to assert
	uint8 m_startWith[STATISTIC_LENGTH]; // Starting state of the hour
	uint32 m_lastTime; // Latest update time
	uint8 m_lastState; // Latest state known


};

#endif // !defined(AFX_EVENTSTAT_H__17E4B832_4539_419B_8377_BA69F7F729CA__INCLUDED_)
