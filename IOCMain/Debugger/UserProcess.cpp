// UserProcess.cpp: implementation of the CUserProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: UserProcess.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: UserProcess.cpp 1.1 2007/12/06 11:41:18PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#include "DebugDL.h"
#include "UserProcess.h"
#include "comm.h"
#include "flash.h"
#include "string.h"
#include "g_cmx.h"
#include "Loader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static CUserProcess *s_pUserProcess = NULL;

CUserProcess::CUserProcess(Ccomm *pUserComm, Ccomm *pDebugComm, 
			CLoader *pLoader, CFlash *pFlash, 
			const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
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
	// m_pUserComm->SetBaudRate(57600);
	m_pDebug->Start(m_pLoader, m_pUserComm, m_pDebugComm, m_pFlash);
}

void CUserProcess::Init()
{

}
