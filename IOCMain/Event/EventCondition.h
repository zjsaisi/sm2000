// EventCondition.h: interface for the CEventCondition class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventCondition.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventCondition.h 1.1 2007/12/06 11:41:23PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_EVENTCONDITION_H__911E1813_582C_4A0B_A0E1_FAE8C9CD9056__INCLUDED_)
#define AFX_EVENTCONDITION_H__911E1813_582C_4A0B_A0E1_FAE8C9CD9056__INCLUDED_

#include "pv.h"

class CEventKb;

class CEventCondition  
{
public:
	void UpdateCondition(int16 eventId);
	void TickPerMinute(void);
	void CloseCondition(void);
	void OpenCondition(void);
	int ReportCondition(int16 eventId, uint8 active, uint8 aid1, uint8 aid2, 
		uint32 timeStamp, uint8 *pAlarm, uint8 *bReportIMC);
	CEventCondition(CEventKb *pKB);
	virtual ~CEventCondition();
private:
	CEventKb *m_pKB;
	CPVOperation m_pv;
};

#endif // !defined(AFX_EVENTCONDITION_H__911E1813_582C_4A0B_A0E1_FAE8C9CD9056__INCLUDED_)
