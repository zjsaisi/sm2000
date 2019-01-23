// EventApp.cpp: implementation of the CEventApp class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: EventApp.cpp 1.3 2008/01/30 11:13:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
 */

#include "EventApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventApp::CEventApp()
{

}

CEventApp::~CEventApp()
{

}

int CEventApp::AddEventList(const SEventKB *pList, CEventKb *pKb)
{
	SEventKB const *pEvt;
	int ret;

	pEvt = pList;
	while (pEvt->eventId != COMM_RTE_EVENT_NONE) {
		if (pEvt->eventId < COMM_RTE_EVENT_MAX) {
			ret = pKb->AddEvent(pEvt);
			if (ret <= 0) return ret;
		}
		pEvt++;
	}
	return 1;
}
