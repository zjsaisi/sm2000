// BaseProcess.cpp: implementation of the CBaseProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: BaseProcess.cpp 1.2 2009/04/30 16:26:32PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include "CxFuncs.h"
#include "string.h"                    
#include "BaseProcess.h"
#include "g_cmx.h"
#include "stdarg.h"  
#include "AllProcess.h"
#include "comm.h"
#include "stdio.h"

#define EVENT_COMMON_QUEUE 0x8000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseProcess * CBaseProcess::m_TaskList[C_MAX_TASKS + 1] = { NULL };
int CBaseProcess::m_TaskCounter = 0; 

CBaseProcess::CBaseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
{
	word16 *ptr;
	byte ret;

	m_debugGroup = 16; // Default debug group
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

	m_priority = priority;

	m_runCnt = 0;

	m_pMessageQueue = NULL;
	// PrintDebugOutput("Task %s has slot %d\r\n", m_name, m_taskSlot);
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

int CBaseProcess::SignalEventIsr(uint16 event)
{
	uint8 ret;

	ret = K_Intrp_Event_Signal(0, m_taskSlot, event);
	if (ret == K_OK) return 1;
	return -1;
}

void CBaseProcess::EventEntry(uint16 timeout)
{
	uint16 ret;

	do {
		ret = K_Event_Wait(0xffff, timeout, 2);
		if (ret == 0) {
			m_timeoutCnt++;
			EventTimeOut();
		} else {
			m_runCnt++;
			EventDispatch(ret);
		}
	} while (1);
}

int CBaseProcess::Notify(uint32 from, uint32 what)
{
	return 0;
}

void CBaseProcess::EventDispatch(uint16 event)
{
	SMessage msg;
	int ret;

	if ((event & EVENT_COMMON_QUEUE) && (m_pMessageQueue)) {
		do {
			ret = m_pMessageQueue->GetTop((uint8 *)&msg);
			if (ret > 0) {
				MessageHandler(msg.message_type, msg.param1_32);
			}
		} while (ret == 1);
	}
}

void CBaseProcess::EventTimeOut(void)
{
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

int CBaseProcess::ClearRunCnt(int bClear)
{
	int ret = m_runCnt;
	if (bClear) m_runCnt = 0;
	return ret;
}

int CBaseProcess::ClearTimeoutCnt(int bClear)
{
	int ret = m_timeoutCnt;
	if (bClear) m_timeoutCnt = 0;
	return ret;
}

void CBaseProcess::PrintDebugOutput(const char *format, ...)
{
	va_list list;

	va_start(list, format);
	CAllProcess::vMaskedPrint((uint32)1 << m_debugGroup, format, list);
	va_end(list);

}

void CBaseProcess::PrintDebugError(const char *format, ...)
{
	va_list list;

	va_start(list, format);
	CAllProcess::g_pAllProcess->vPrint(format, list);
	va_end(list);

}

void CBaseProcess::SetDebugGroup(int group)
{
	m_debugGroup = group;
}

extern char * const g_pHeapStart;
extern char * const g_pHeapEnd;

void CBaseProcess::PrintStatus(unsigned long *param, Ccomm *pComm)
{
	CBaseProcess *ptr;
	uint8 total;
	char buff[200]; 
	int i;
   	uint16 stkAvail, stkTotal;
	uint16 *stkAddress;

	sprintf(buff, "Debugger @%08lX\r\nHEAP @0x%08lX -- 0x%08lX\r\n", 
		param, g_pHeapStart, g_pHeapEnd);
	pComm->Send(buff);
	pComm->Send("    Name\t@\tSlot PRI Avail\tUsed\tTotal\tstkAddr  EvtCnt TOUTCnt \r\n");
	for (i = 0; i < C_MAX_TASKS; i++) {
		int runCnt, timeoutCnt;	
		ptr = GetProcess(i);
		if (ptr == NULL) break;
		runCnt = ptr->ClearRunCnt((param[1] == 4) ? (1) : (0));
		timeoutCnt = ptr->ClearTimeoutCnt( (param[1] == 4) ? (1) : (0) );
		ptr->GetStackStatus(&stkAvail, &stkTotal, &stkAddress);
		sprintf(buff, "%02d  %s\t%06lX\t%02d   %02X  %d\t%d\t%d\t0x%06lX %04X   %04X\r\n", 
			i, ptr->GetTaskName(), ptr, 
			ptr->GetSlot(),
			ptr->GetPriority(),
			stkAvail * 2,  (stkTotal - stkAvail) * 2,
			stkTotal * 2,
			stkAddress,
			runCnt,
			timeoutCnt
			);
		pComm->Send(buff);
	}
	total = C_MAX_TASKS;
	sprintf(buff, "Total tasks allowed: %d\r\n", total);
	pComm->Send(buff);
		
	{
		uint16 EndOfRam;
		uint32 address;
		address = CAllProcess::g_pAllProcess->GetEndOfRam(&EndOfRam);
		sprintf(buff, "EndOfRAM 0x@%08lX Mark=0x%X\r\n", address, EndOfRam);
		pComm->Send(buff);
	}
		
	uint32 duration;
	int hr, min, sec;

	duration = CAllProcess::g_pAllProcess->GetAbsoluteSecond();
	hr = duration / 3600;
	min = ( duration % 3600 ) / 60;
	sec = duration % 60;
	sprintf(buff, "Running for %d:%02d:%02d\r\n", hr, min, sec);
	pComm->Send(buff);

	if (param[1] == 4) {
		pComm->Send("Process Cnt cleared.\r\n");
	}

}

void CBaseProcess::AllowMessage()
{
	if (m_pMessageQueue == NULL) {
		m_pMessageQueue = new CQueue(sizeof(SMessage), 20);
	}
}

void CBaseProcess::BroadCast(uint16 type, uint32 param)
{
	int i;
	CBaseProcess *pProc;
	SMessage msg;

	msg.message_type = type;
	msg.param1_32 = param;

	for (i = 0; i < m_TaskCounter; i++) {
		pProc = m_TaskList[i];
		if (pProc->m_pMessageQueue != NULL) {
			pProc->m_pMessageQueue->Append((uint8 *)&msg);
			pProc->SignalEvent(EVENT_COMMON_QUEUE);
		}
	}
}

void CBaseProcess::PostMessage(uint16 type, uint32 param)
{
	SMessage msg;

	if (m_pMessageQueue) {
		msg.message_type = type;
		msg.param1_32 = param;
		m_pMessageQueue->Append((uint8 *)&msg);
		SignalEvent(EVENT_COMMON_QUEUE);
	}
}

// This should be over-written by sub-class.
void CBaseProcess::MessageHandler(uint16 message_type, uint32 param)
{

}

