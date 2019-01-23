// UserProcess.cpp: implementation of the CUserProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "DebugDL.h"
#include "UserProcess.h"
#include "comm.h"
#include "flash.h"
#include "string.h"
#include "g_cmx.h"
#include "Loader.h"
#include "AllProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static CUserProcess *s_pUserProcess = NULL;

CUserProcess::CUserProcess(Ccomm *pUserComm, Ccomm *pDebugComm, CLoader *pLoader, CFlash *pFlash, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_pUserComm = pUserComm;
	m_pDebugComm = pDebugComm;
	m_pLoader = pLoader;
	m_pFlash = pFlash;
	m_pDebug = new CDebugDL(name, pLoader, this);
	if (s_pUserProcess == NULL) {
		s_pUserProcess = this;
	}
	m_bRunning = 0;
}

CUserProcess::~CUserProcess()
{

}

void CUserProcess::Entry()
{
	s_pUserProcess->entry();
}

void CUserProcess::entry()
{  
	g_pAll->MakeBootChoice(m_pUserComm);
	m_bRunning = 1;
	m_pDebug->Start(m_pLoader, m_pUserComm, m_pDebugComm, m_pFlash);
}
