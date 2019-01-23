// DebugProcess.h: interface for the DebugProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_)
#define AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_

#include "BaseProcess.h"
#include "flash.h"
#include "comm.h"
#include "DebugDL.h"
#include "loader.h"

class CDebugProcess : public CBaseProcess  
{
public:
	CDebugProcess(Ccomm *pPort, CLoader *pLoader, CFlash *pFlash, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CDebugProcess();
	// 0: Before choice is made.
	// 1: Choice made. Stay in boot code.
	int IsRunning(void) { return m_bRunning; };
private:
	static void Entry(void);
private:
	void entry(void);
	Ccomm *m_pPort;
	CLoader *m_pLoader;
	CFlash *m_pFlash;
	CDebugDL *m_pDebugger;
	int m_bRunning;
};

#endif // !defined(AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_)
