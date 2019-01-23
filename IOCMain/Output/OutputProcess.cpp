// OutputProcess.cpp: implementation of the COutputProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: OutputProcess.cpp 1.4 2008/01/21 13:36:58PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */

#include "OutputProcess.h"
#include "string.h"
#include "DtiOutput.h"
#include "TeleOutput.h"
#include "OneHz.h"
#include "TenMHz.h"

#define TRACE TRACE_OUTPUT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutputProcess * COutputProcess::s_pOutputProcess = NULL;

COutputProcess::COutputProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	s_pOutputProcess = this;
}

COutputProcess::~COutputProcess()
{

}

void COutputProcess::Entry()
{
	s_pOutputProcess->entry();
}

void COutputProcess::entry()
{
	AllowMessage();
	Pause(TICK_1SEC * 3); // Let the output led on for 3 seconds.
	CDtiOutput::PowerOn();
	// Pause(TICK_1SEC * 15); // Let FPGA get valid data and Istate settles
	EventEntry(TICK_1SEC * 3);
}

void COutputProcess::Init()
{

}

void COutputProcess::TickOutputProcess()
{
	SignalEvent(EVT_ONE_SECOND_OUTPUT);
}

void COutputProcess::EventDispatch(uint16 event)
{
	CBaseProcess::EventDispatch(event);
	if (event & EVT_ONE_SECOND_OUTPUT) {
		OneSecondTask();
	}
	if (event & EVT_SERVICE_OUTPUT) {
#if !TO_BE_COMPLETE
		CDtiOutput::ServiceOutput();
#endif
	}
	if (event & EVT_SETUP_OUTPUT) {
#if !TO_BE_COMPLETE
		CDtiOutput::SetupOutput();
#endif
		CTeleOutput::SetupOutput();
		CTenMHzOutput::s_pTenMHzOutput->SetupOutput();
		COneHzOutput::s_pOneHzOutput->SetupOutput();
	}
	if (event & EVT_SETUP_STANDBY) {
#if !TO_BE_COMPLETE
		CDtiOutput::SetupStandby();
#endif
	}
}

void COutputProcess::OneSecondTask()
{

}

void COutputProcess::MessageHandler(uint16 message_type, uint32 param)
{
	switch (message_type) {
	case MESSAGE_LOOP_CHANGE:
#if !TO_BE_COMPLETE
		CDtiOutput::DoLoopChange();
#endif
		CTeleOutput::DoLoopChange();
		break;
	case MESSAGE_STATUS_CHANGE:
#if !TO_BE_COMPLETE
		CDtiOutput::DoStatusChange();
#endif
		break;

	}
}

void COutputProcess::ToServiceOutputIsr()
{
	SignalEventIsr(EVT_SERVICE_OUTPUT);
}

void COutputProcess::ToSetupOutput()
{
	SignalEvent(EVT_SETUP_OUTPUT);
}

void COutputProcess::SetupStandbyOutput()
{
	SignalEvent(EVT_SETUP_STANDBY);
}
