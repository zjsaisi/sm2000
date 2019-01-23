// EventDesc.h: interface for the CEventKb class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventDesc.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventKb.h 1.3 2008/03/06 16:55:12PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#if !defined(AFX_EVENTDESC_H__3F3F8D53_A35F_4EF8_80D2_BE090F1E8B1A__INCLUDED_)
#define AFX_EVENTDESC_H__3F3F8D53_A35F_4EF8_80D2_BE090F1E8B1A__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "IocEvtList.h"

class CIstate;
class CEvent;
class CEventProcess;

typedef struct {
	int16 eventId;
	uint16 flag;
	uint8 aidOneStart;
	uint8 aidOneEnd;
	uint8 aidTwoStart;
	uint8 aidTwoEnd;
	char const * pAcronym;
	char const * pDebug;
	uint8	initState;
	int8	default_alarm_level;
	int8	default_log;
	int8	default_service_affect;
} SEventKB;

class CEventKb  
{
public:
	int GetDefaultServiceAffect(int16 eventId);
	int GetDefaultLog(int16 eventId);
	int GetDefaultAlarmLevel(int16 eventId);
	virtual void Init(void);
	int GetUpgradeMinute(int16 eventId, uint8 alarm, int32 *pMinute);
	CEvent * GetEventStatus(int16 eventId);
	void GetEventIdRange(int16 *pStart, int16 *pEnd);
	int IsPartOfCondition(int16 eventId);
	int NeedLogged(int16 eventId, uint8 *pLogged);
	const char * GetDebugString(int16 eventId, char const **pAcronym);
	int GetAlarmLevel(int16 eventId, uint8 *pAlarm);
	CEventKb(CIstate *pIstate, CEventProcess *pProcess);
	virtual ~CEventKb();
	enum {EVT_FLAG_REPORT_COND = 1, 
		EVT_FLAG_SLAVE_REPORT = 2, EVT_FLAG_REPORT_STATISTIC = 4,
		EVT_FLAG_TRANSIENT = 8,
		EVT_FLAG_RESEND_PPC = 16 // resend to PPC when trigger.
	};
	int AddEvent(SEventKB const *pEvt);
private:
	enum { NUM_EVENT_KB =  COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE + 10};
	// event knowledge list, One event could have multiple entry
	static const SEventKB *m_pEventKBList[NUM_EVENT_KB];
	int16 m_kbCnt;

	// Range of event id
	int16 m_eventStart;
	int16 m_eventEnd;
	// Point to Istate
	CIstate *m_pIstate;
	// Point to event process. CEvent need to use process to report upgraded event
	CEventProcess *m_pEventProcess;

	// Event status list
	void CreateEventStatus(void);
	CEvent *m_pEventStatusList[COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE + 1];
};

#endif // !defined(AFX_EVENTDESC_H__3F3F8D53_A35F_4EF8_80D2_BE090F1E8B1A__INCLUDED_)
