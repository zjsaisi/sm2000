// OutputProcess.h: interface for the COutputProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: OutputProcess.h 1.2 2008/01/10 13:49:07PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#if !defined(AFX_OUTPUTPROCESS_H__09B0BBEE_EF26_41B2_9013_83660E4C25EE__INCLUDED_)
#define AFX_OUTPUTPROCESS_H__09B0BBEE_EF26_41B2_9013_83660E4C25EE__INCLUDED_

#include "BaseProcess.h"

class COutputProcess : public CBaseProcess  
{
public:
	void SetupStandbyOutput(void);
	void ToSetupOutput(void);
	void ToServiceOutputIsr(void);
	void TickOutputProcess(void);
	void Init(void);
	COutputProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~COutputProcess();
	static COutputProcess *s_pOutputProcess;
	enum { MESSAGE_LOOP_CHANGE = 1, MESSAGE_STATUS_CHANGE = 2 }; // enumeration
protected:
	virtual void MessageHandler(uint16 message_type, uint32 param);
	virtual void EventDispatch(uint16 event);

private:
	void entry(void);
	void OneSecondTask(void);
	enum { EVT_ONE_SECOND_OUTPUT = 1, EVT_SERVICE_OUTPUT = 2,
		EVT_SETUP_OUTPUT = 4, EVT_SETUP_STANDBY = 8
	}; // bit message
	static void Entry(void);
};

#ifndef TRACE_OUTPUT
#define TRACE_OUTPUT COutputProcess::s_pOutputProcess->PrintDebugOutput
#endif

#endif // !defined(AFX_OUTPUTPROCESS_H__09B0BBEE_EF26_41B2_9013_83660E4C25EE__INCLUDED_)
