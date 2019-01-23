// BaseProcess.h: interface for the CBaseProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_)
#define AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_

#include "DataType.h"
#include "codeopt.h"
#include "ConstDef.h"
#include "cxconfig.h"

class Ccomm;

EXTERN uint8 CreateTask(const char *name, uint8 priority, uint8 *slot, 
	FUNC_PTR start, uint16 stack_size);
	
#ifdef __cplusplus
#include "Queue.h"

typedef struct {
	uint16 message_type;
	uint32 param1_32;
} SMessage;

class CBaseProcess  
{
	friend uint8 CreateTask(const char *name, uint8 priority, 
		uint8 *slot, FUNC_PTR start, uint16 stack_size);

public:
	void PostMessage(uint16 type, uint32 param);
	static void BroadCast(uint16 type, uint32 param);
	virtual void PrintDebugError(const char *format, ...);
	static void PrintStatus(unsigned long *param, Ccomm *pComm);
	void SetDebugGroup(int group);
	virtual void PrintDebugOutput(const char *format, ...);
	int ClearTimeoutCnt(int bClear = 1);
	int ClearRunCnt(int bClear = 1);
	virtual int Pause(uint16 ticks);
	virtual uint8 GetPriority(void) const { return m_priority; };
	virtual uint8 SetPriority(uint8 new_priority);
	static CBaseProcess * GetProcess(int index);
	virtual uint8 GetSlot(void) const { return m_taskSlot; };
	virtual const char *GetTaskName(void) const { return m_name; };
	virtual int Notify(uint32 from, uint32 what);
	virtual int SignalEvent(uint16 event);
	virtual int SignalEventIsr(uint16 event);
	virtual int StartProcess(void);
	virtual void GetStackStatus(uint16 *available, uint16 *total, uint16 **pStack);
	CBaseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry);
	virtual ~CBaseProcess();
protected:
	virtual void MessageHandler(uint16 message_type, uint32 param);
	void AllowMessage(void);
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
	virtual void EventEntry(uint16 timeout = TICK_1SEC);
	uint8 m_taskSlot;     
	int8 m_bValid;
	uint16 *m_pStack;
	uint16 m_nStackWord; 
	char m_name[9];
	uint8 m_priority;
	int m_timeoutCnt;
	CQueue *m_pMessageQueue;
private:
	int m_runCnt;
	static CBaseProcess *m_TaskList[C_MAX_TASKS + 1];
	static int m_TaskCounter;
	int m_debugGroup;	
};
#endif // __cplusplus

#endif // !defined(AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_)
