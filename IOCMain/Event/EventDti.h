// EventDti.h: interface for the CEventDti class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: EventDti.h 1.1 2007/12/06 11:41:23PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_EVENTDTI_H__9AAB58C0_8BCE_47C1_925F_FB69826D5E16__INCLUDED_)
#define AFX_EVENTDTI_H__9AAB58C0_8BCE_47C1_925F_FB69826D5E16__INCLUDED_


#include "EventApp.h"

#if (!defined(BASIC_EVENT_ONLY)) || (BASIC_EVENT_ONLY == 0)

class CEventDti : public CEventApp  
{
public:
	CEventDti(CEventKb *pKb);
	virtual ~CEventDti();
private:
	static SEventKB const m_eventList[];
};
#endif

#endif // !defined(AFX_EVENTDTI_H__9AAB58C0_8BCE_47C1_925F_FB69826D5E16__INCLUDED_)
