// BaseProcess.h: interface for the CBaseProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_)
#define AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_

//#include "zmdefine.h"
#include "DataType.h"
#include "ConstDef.h"
#include "cxconfig.h"

EXTERN uint8 CreateTask(const char *name, uint8 priority, uint8 *slot, 
	FUNC_PTR start, uint16 stack_size);
	
#ifdef __cplusplus
class CBaseProcess  
{
	friend uint8 CreateTask(const char *name, uint8 priority, 
		uint8 *slot, FUNC_PTR start, uint16 stack_size);

public:
	int Pause(uint16 ticks);
	uint8 SetPriority(uint8 new_priority);
	virtual CBaseProcess * GetProcess(int index);
	virtual uint8 GetSlot(void) const { return m_taskSlot; };
	virtual const char *GetTaskName(void) const { return m_name; };
	virtual int Notify(uint32 from, uint32 what);
	virtual int SignalEvent(uint16 event);
	virtual int StartProcess(void);
	virtual void GetStackStatus(uint16 *available, uint16 *total, uint16 **pStack);
	CBaseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry);
	virtual ~CBaseProcess();
protected:
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
	virtual void EventEntry(uint16 timeout = TICK_1SEC);
	uint8 m_taskSlot;     
	uint16 *m_pStack;
	uint16 m_nStackWord; 
	uint8 m_priority;
	int8 m_bValid;
	char m_name[9];
private:
	static CBaseProcess *m_TaskList[C_MAX_TASKS + 1];
	static int m_TaskCounter;
};
#endif // __cplusplus

#endif // !defined(AFX_BASEPROCESS_H__F618377A_DD7A_4FBE_9F5D_E7A87C566E7F__INCLUDED_)
