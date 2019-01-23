// BaseProcess.cpp: implementation of the CBaseProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: BaseProcess.cpp 1.2 2009/05/05 15:20:36PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

//#include "zmdefine.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "CxFuncs.h"
#endif
#include "string.h"                    
#include "BaseProcess.h" 
#include "g_cmx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseProcess * CBaseProcess::m_TaskList[C_MAX_TASKS + 1] = { NULL };
int CBaseProcess::m_TaskCounter = 0; 

CBaseProcess::CBaseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
{
	word16 *ptr;
	byte ret;

	m_priority = priority;
	stack_size >>= 1; 
	m_nStackWord = stack_size;
	ptr = new word16[stack_size];
	m_pStack = ptr;
	memset(ptr, 0xa5, stack_size << 1);
	ptr += stack_size;     
	ret = K_Task_Create_Stack(priority, &m_taskSlot, entry, ptr);
	m_bValid = (ret == K_OK) ? (1) : (0);
	if (!m_bValid) {
		delete []m_pStack;
		m_pStack = NULL;
		m_taskSlot = 255;
	}
	if (name) {
		strncpy(m_name, name, 8);
		m_name[8] = 0;
	} else {
		strcpy(m_name, "NULL");
	}

	m_TaskList[m_TaskCounter] = this;
	m_TaskCounter++;
	m_TaskList[m_TaskCounter] = NULL;
}

CBaseProcess::~CBaseProcess()
{
	// No use actually.
	if (m_bValid && m_pStack) {
   		delete []m_pStack;
	}
}
                            
CBaseProcess * CBaseProcess::GetProcess(int index)
{
	if (index >= m_TaskCounter) return NULL;
	return m_TaskList[index];
}

void CBaseProcess::GetStackStatus(uint16 *available, uint16 *total, uint16 **pStack)
{
	uint16 *ptr = m_pStack;
	uint16 left;
	if (!m_bValid) {
		*available = 0;
		*total = 0;
		*pStack = NULL;
		return;
	}
	for (left = 0; left < m_nStackWord; left++) {
		if (*ptr != 0xa5a5) break;
		ptr++;
	}
	*total = m_nStackWord;
	*available = left;
	*pStack = m_pStack;
}

// return 1 for success
// -1 for failure
// -2 invalid process
int CBaseProcess::StartProcess()
{
	uint8 ret;
	if (!m_bValid) return -2;
	ret = K_Task_Start(m_taskSlot);
	if (ret != K_OK) return -1;
	return 1;
}

int CBaseProcess::SignalEvent(uint16 event)
{
	uint8 ret;

	ret = K_Event_Signal(0, m_taskSlot, event);
	if (ret == K_OK) return 1;
	return -1;
}

void CBaseProcess::EventEntry(uint16 timeout)
{
	uint16 ret;
	do {
		ret = K_Event_Wait(0xffff, timeout, 2);
		if (ret == 0) {
			EventTimeOut();
		} else {
			EventDispatch(ret);
		}
	} while (1);
}

void CBaseProcess::EventDispatch(uint16 event)
{

}

void CBaseProcess::EventTimeOut(void)
{

}

int CBaseProcess::Notify(uint32 from, uint32 what)
{
	return 0;
}

EXTERN uint8 CreateTask(const char *name, uint8 priority, uint8 *slot, 
	FUNC_PTR start, uint16 stack_size)
{
	CBaseProcess *process;
	int ret;

	process = new CBaseProcess(name, priority, stack_size, start);
	*slot = process->m_taskSlot;
	if (process->m_bValid) {
		ret = process->StartProcess();
		if (ret == 1) return K_OK;
	}
	return K_ERROR;
}

uint8 CBaseProcess::SetPriority(uint8 new_priority)
{
	uint8 old;
	old = m_priority;
	if (!g_cmx_started) return old;
	K_Task_Priority(m_taskSlot, new_priority);
	// Pause 1 tick to make sure it could be stopped. IMPORTANT.
	// Otherwise it could never shift from high to low
	Pause(1);
	m_priority = new_priority;
	return old;
}

int CBaseProcess::Pause(uint16 ticks)
{
	if (!g_cmx_started) return -2;
	if (K_Task_Wait(ticks) == K_OK) return 1;
	return -1;

}
