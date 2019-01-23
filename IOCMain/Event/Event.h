// Event.h: interface for the CEvent class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * RCS: $Header: Event.h 1.2 2008/01/29 11:29:24PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_EVENT_H__600D56EB_0D8D_4CEC_B72F_E00BE62548F4__INCLUDED_)
#define AFX_EVENT_H__600D56EB_0D8D_4CEC_B72F_E00BE62548F4__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CEventKb;
class Ccomm;
class CEventStat;
class CEventProcess;
class CPTPLink;

class CEvent  
{
public:
	void ResendToImc(CPTPLink *pLink);
	void UpdateEventAlarmLevel_SA(void);
	void RegenerateEvent(int bClearIt = 0 );
	static void CountAllAlarms(int *pMinor, int *pMajor, int *pCritical);
	//void CountAlarms(int *pMinor, int *pMajor, int *pCritical);
	// Basic datat structure used to dump condition to PPC
	typedef struct {
		int16 event_id;
		uint8 active;
		uint8 alarm_level;
		uint8 aid1;
		uint8 aid2;
		uint32 time_stamp;
		uint32 extra1;
		uint32 extra2;
	} SConditionBlock;

	int DumpCondition(uint16 flag, int maxNumber, SConditionBlock *pBlock, 
		uint8 *pAid1Start, uint8 *pAid2Start, int bAcitveOnly = 1);
	CEvent *GetNext(void) { return m_pNext; };
	void TickPerMinute(void);
	static char const * AlmString(uint8 alm);
	int PrintCondition(Ccomm *pComm, unsigned long option);
	void SetInitState(uint8 active, uint8 alarm);
	void Append(CEvent *pNext);
	virtual int ReportState(int16 eventId, uint8 active, uint8 aid1, uint8 aid2,
		uint32 timeStamp, uint8 *pAlarm, uint8 *bReportIMC);
	CEvent(CEventProcess *pProcess, CEventKb *pKB, int16 eventId, uint8 aidOneStart, uint8 aidOneEnd,
		uint8 aidTwoStart, uint8 aidTwoEnd, int8 bDoStatistic, uint16 flag);
	virtual ~CEvent();
	int GetFlag(void) { return m_flag; };
	enum { EVENT_TRUE = 1, EVENT_FALSE = 0, EVENT_START = 0x80 };
private:
	static CEventKb *m_pKB;
	static CEventProcess *m_pEventProcess;
	int GetAidIndex(uint8 aid1, uint8 aid2);
	int16 m_eventId;
	uint8 m_aidOneStart; // Inclusive
	uint8 m_aidOneEnd; // exclusive
	uint8 m_aidTwoStart; // Inclusive
	uint8 m_aidTwoEnd; // exclusive

	CEvent *m_pNext; // Point to same event with different aid range.

	uint16 m_size;
	uint8 *m_pEventActiveList;
	uint8 *m_pEventAlarmLevel;
	int32 *m_pEventUpgradeTime;
	uint32 *m_pTimeStamp;
	CEventStat **m_pEventStat;

	uint16 m_flag;

	// Alarm counting 
	int m_minorAlarms;
	int m_majorAlarms;
	int m_criticalAlarms;

	// Alarm counting for all
	static int m_allMinorAlarms;
	static int m_allMajorAlarms;
	static int m_allCriticalAlarms;
};

#endif // !defined(AFX_EVENT_H__600D56EB_0D8D_4CEC_B72F_E00BE62548F4__INCLUDED_)
