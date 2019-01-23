// MinuteProcess.h: interface for the CMinuteProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: MinuteProcess.h 1.2 2008/05/28 18:11:12PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#if !defined(AFX_MINUTEPROCESS_H__B9A7ADC5_0B73_4F4A_A77C_3322F73888FF__INCLUDED_)
#define AFX_MINUTEPROCESS_H__B9A7ADC5_0B73_4F4A_A77C_3322F73888FF__INCLUDED_

#include "BaseProcess.h"

class CPTPLink;

class CMinuteProcess : public CBaseProcess  
{
public:
	void SetAutoReboot(int reboot = 1);
	void TickHalfMinute(void);
	void TickMinuteProcess(void);
	void Init(void);
	CMinuteProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CMinuteProcess();
	static CMinuteProcess *s_pMinuteProcess;
private:
	void CheckGpsPhase(void);
	void RebootHotActive(void);
	void RebootColdActive(void);
	void CheckNewCode(void);
	void OneMinuteTask(void);
	enum { EVT_ONE_MINUTE = 1, EVT_HALF_MINUTE = 2 };
	static void Entry(void);
	int m_autoReboot;
	CPTPLink *m_pIocLink;
	uint32 m_currentMinute;
	uint32 m_giveupMinute; // Time ready to give up active status
protected:
	virtual void EventDispatch(uint16 event);
};

#ifndef TRACE_MINUTE
#define TRACE_MINUTE CMinuteProcess::s_pMinuteProcess->PrintDebugOutput
#endif

#endif // !defined(AFX_MINUTEPROCESS_H__B9A7ADC5_0B73_4F4A_A77C_3322F73888FF__INCLUDED_)
