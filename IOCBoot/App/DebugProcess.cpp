// DebugProcess.cpp: implementation of the DebugProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "comm.h"
#include "DebugProcess.h"
#include "string.h"
#include "g_cmx.h"
#include "DebugDL.h"
#include "AllProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CDebugProcess *s_pDebugProcess = NULL;

CDebugProcess::CDebugProcess(Ccomm *pPort, CLoader *pLoader, CFlash *pFlash, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_pPort = pPort;    
	m_pLoader = pLoader;
	m_pFlash = pFlash;
	m_pDebugger = new CDebugDL(name, pLoader, this);             
	if (s_pDebugProcess == NULL) {
		s_pDebugProcess = this;
	}
	m_bRunning = 0;
}

CDebugProcess::~CDebugProcess()
{

}

void CDebugProcess::Entry()
{
	s_pDebugProcess->entry();
}

void CDebugProcess::entry()
{
	g_pAll->MakeBootChoice(m_pPort, 1);
	m_bRunning = 1;
	m_pDebugger->Start(m_pLoader, m_pPort, (Ccomm *)NULL, m_pFlash);
}
