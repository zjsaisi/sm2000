// PTPProcess.cpp: implementation of the CPTPProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPProcess.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: PTPProcess.cpp 1.1 2007/12/06 11:41:39PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "PTPProcess.h"
#include "PTPLink.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CPTPProcess *s_pPTP1 = NULL;
static CPTPProcess *s_pPTP2 = NULL;

// There will two instance of this process
CPTPProcess::CPTPProcess(CPTPLink *pLink, const char *pName, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(pName, priority, stack_size, entry)
{
	if (entry == Entry1) {
		s_pPTP1 = this;
	}
	if (entry == Entry2) {
		s_pPTP2 = this;
	}
	m_pLink = pLink;
}

CPTPProcess::~CPTPProcess()
{

}

void CPTPProcess::Entry1()
{
	if (s_pPTP1) {
		s_pPTP1->entry();
	}
}

void CPTPProcess::Entry2()
{
	if (s_pPTP2) {
		s_pPTP2->entry();
	}
}

void CPTPProcess::entry()
{
	m_pLink->Restart();
	EventEntry(TICK_1SEC / 2);
}


void CPTPProcess::EventTimeOut()
{
	m_pLink->EventTimeOut();
}

void CPTPProcess::EventDispatch(uint16 event)
{
	m_pLink->EventDispatch(event);
}

void CPTPProcess::Init()
{

}
