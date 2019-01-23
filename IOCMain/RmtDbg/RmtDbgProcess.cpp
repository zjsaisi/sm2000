// RmtDbgProcess.cpp: implementation of the CRmtDbgProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "CodeOpt.h"
#include "DataType.h"
#include "stdio.h"
#include "RmtDbgProcess.h"
#include "RmtComm.h"
#include "DebugDL.h"
#include "loader.h"
#include "flash.h"
#include "PTPLink.h"
#include "AllProcess.h"
#include "MemBlock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRmtDbgProcess * CRmtDbgProcess::s_pRmtDbgProcess = NULL;

CRmtDbgProcess::CRmtDbgProcess(CFlash *pFlash, CLoader *pLoader, CRmtComm *pRmtComm, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry /* = Entry*/)
: CBaseProcess(name, priority, stack_size, entry)
{
	if (s_pRmtDbgProcess == NULL) {
		s_pRmtDbgProcess = this;
	}
	m_pRmtComm = pRmtComm;
	m_pDebug = new CDebugDL(name, pLoader, this);
	m_pLoader = pLoader;
	m_pFlash = pFlash;
}

CRmtDbgProcess::~CRmtDbgProcess()
{

}

void CRmtDbgProcess::Entry()
{
	s_pRmtDbgProcess->entry();
}

void CRmtDbgProcess::entry()
{
	Pause(TICK_1SEC * 5);

	// register with PTP
	m_pRmtComm->Init();
	m_pLink = CAllProcess::g_pAllProcess->GetImcLink();
	m_pLink->SetupClient(CRmtComm::PTP_IOC_PORT_REMOTE_DEBUG, this);

	m_pDebug->SetEcho(0);

	// Open up remote comm

	m_pDebug->Start(m_pLoader, m_pRmtComm, NULL, m_pFlash);
}


// Called from PTP
int CRmtDbgProcess::Notify(uint32 from, uint32 what)
{
	CPTPLink *pFrom = (CPTPLink *)from;
	CMemBlock *pBlock = (CMemBlock *)what;
	int ret;
	
	if (pFrom != m_pLink) {
		pBlock->Release();
		return 1;
	}
	ret = m_pRmtComm->FeedInput(pBlock);
	if (ret <= 0) {
		pBlock->Release();
	}
	return 1; // 12/29/2004: Added.
}




