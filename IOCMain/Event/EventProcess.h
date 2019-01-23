// EventProcess.h: interface for the CEventProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventProcess.h 1.2 2007/12/07 13:59:46PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_EVENTPROCESS_H__A7031925_7CBA_46E1_864B_70AE38AD78FD__INCLUDED_)
#define AFX_EVENTPROCESS_H__A7031925_7CBA_46E1_864B_70AE38AD78FD__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "BaseProcess.h"
#include "Queue.h"
#include "EventStore.h"
#include "eventId.h"
#include "Event.h"

class CEventStore;
class CIstate;
class CEventKb;
class CEventCondition;
class Ccomm;
class CPTPLink;
class CCardState;
class CLed;

#ifndef TRACE_EVENT
#define TRACE_EVENT CEventProcess::s_pEventProcess->PrintDebugOutput
#endif

class CEventProcess : public CBaseProcess  
{
public:
	static CEventProcess *s_pEventProcess;
	int ReportEvent(int16 id, uint8 active, uint8 aid1= 0, uint8 aid2 = 0, uint8 initAlarm = COMM_ALARM_LEVEL_NULL);
	CEventProcess(CPTPLink *pIMCLink, CEventStore *pEventStore, CIstate *pIstate,
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CEventProcess();
	CEventKb *GetEventKB(void) { return m_pKB; };
	CEventStore *GetEventStore(void) { return m_pEventStore; };
	typedef struct {
		uint16 cmd; // ALways 0x100
		int16 event_id;
		uint8 active;
		uint8 alarm_level;
		uint8 aid1;
		uint8 aid2;
		uint32 time_stamp;	
	} SEventReport;
	enum { PTP_IMC_PORT_AO = 0x40, PTP_IOC_PORT_COND = 0x50,
		PTP_IMC_PORT_COND = 0x30 };
private:
	void ResendEventToImc(void);
	int16 volatile m_holdEventCnt; // Hold event initially to make sure Istate is settled so that Alarm level and SA is right.
	int16 m_bEnableLog;
	int16 RtrvSingleCondition(int16 event_id, uint8 aid1, uint8 aid2, CEvent::SConditionBlock *pBlock);
	enum { CMD_RTRV_LIST = 0x120, CMD_RTRV_SINGLE_EVENT = 0x121 };
	void SetupAlarmLed(void);
	void RespondRtrvCond(void);
	typedef struct {
		uint16 cmd;
		uint32 handle;
		uint16 option;
		uint32 sequence;
	} SRtrvCmd; // This structure should be bigger than SRtrvSingleCmd below.
	
	typedef struct {
		uint16 cmd;  // CMD_RTRV_SINGLE_EVENT
		uint32 handle;
		int16 event_id;
		uint8 aid1;
		uint8 aid2;
	} SRtrvSingleCmd; // This struct has to be smaller than SRtrvCmd. Otherwise the unit size of queue should be decided by this.

	enum { MAX_CONDITION_NUM = 10 };
	
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error_code;
		uint32 sequence;
		uint16 cnt;
		CEvent::SConditionBlock condition[MAX_CONDITION_NUM];
	} SRtrvResp;

	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error_code;
		CEvent::SConditionBlock condition;
	} SRtrvSingleResp;

	int RtrvCondition(int option, uint32 *sequence, CEvent::SConditionBlock *pBlock);

	void StoreEvent(int16 id, uint8 active, uint8 alarm, 
		uint8 aid1, uint8 aid2, uint32 timeStamp, uint8 bLog, uint8 bReport = 1);
	void entry(void);
	static void Entry(void);
	CEventStore *m_pEventStore;
	CQueue *m_pQueue;
	enum {EVT_NEW_EVENT_ITEM = 1, EVT_ONE_MINUTE = 2,
		EVT_RTRV_COND_CMD = 4, EVT_RESEND_TO_IMC = 8 };
	CEventKb *m_pKB;
	CEventCondition *m_pCond;
	CPTPLink *m_pIMCLink;
	//CCardState *m_pCardState;
	CQueue *m_pQueueCondCmd;
	int m_refreshEventId;

protected:
	virtual int Notify(uint32 from, uint32 what);
	virtual void EventDispatch(uint16 event);
	virtual void EventTimeOut(void);
public:
	void ResendEventToImcTrigger(void);
	void ReleaseEvent(void);
	void HoldEvent(void);
	void LogHack(unsigned long *param, Ccomm *pComm);
	void ConditionHack(unsigned long *param, Ccomm *pComm);
	void EventHack(unsigned long *param, Ccomm *pComm);
	void Init(void);
	//static void sDebugOutput(const char *format, ...);
	void SetEnableLog(int16 bEnableLog);
	//static void DebugOutput(const char *str);
	void TickPerMinute(void);
	void PrintEvent(SEventItem *pItem, Ccomm *pComm = NULL);
	// Bit0 = 1: Print every new log
	// Bit1 = 1: Print active new log
	// Bit2 = 1: Printf inactive new log
	uint32 m_debugOption;
};

#endif // !defined(AFX_EVENTPROCESS_H__A7031925_7CBA_46E1_864B_70AE38AD78FD__INCLUDED_)
