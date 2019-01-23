// BasicEvent.cpp: implementation of the CEventBasic class.
//
//////////////////////////////////////////////////////////////////////
#include "CodeOpt.h"
#include "DataType.h"
#include "EventBasic.h"
#include "EventProcess.h"
#include "EventId.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const SEventKB CEventBasic::m_eventList[] = {
	// eventId , flag,	
	// aidOneStart(inclusive, allowed),	AidOneEnd(non-inclusive),	AidTwoStart,	AidTwoEnd,	
	// pAcronym, pDebug	initState(0: inactive, 1: active) 
	// alarm level,  log  ,  SA
{COMM_RTE_EVENT_RTE_RESET, CEventKb::EVT_FLAG_TRANSIENT, 
	0, 1, 0, 1, "RTERST", "RTE Reset", 0,
	COMM_ALARM_LEVEL_JUST_REPORT,		1,	1
},

{COMM_RTE_EVENT_PPC_COM_ERROR, CEventKb::EVT_FLAG_REPORT_COND | CEventKb::EVT_FLAG_SLAVE_REPORT
	| CEventKb::EVT_FLAG_RESEND_PPC,
	0, 1, 0, 1, "PPCNCOM", "Communication with PPC failed", 0,
	COMM_ALARM_LEVEL_MINOR,				1,	0
},

{COMM_RTE_EVENT_RTE_COM_ERROR, CEventKb::EVT_FLAG_REPORT_COND | CEventKb::EVT_FLAG_SLAVE_REPORT
	| CEventKb::EVT_FLAG_RESEND_PPC,
	0, 1, 0, 1, "RTENCOM", "Communication with twin RTE failed", 0,
	COMM_ALARM_LEVEL_MINOR,				1,	0
},

{COMM_RTE_EVENT_FIRMWARE_COPY, CEventKb::EVT_FLAG_REPORT_COND | CEventKb::EVT_FLAG_SLAVE_REPORT
	| CEventKb::EVT_FLAG_RESEND_PPC,
	0, 1, 0, 1, "FIRMCOPY", "Copying Firmware to Twin", 0,
	COMM_ALARM_LEVEL_JUST_REPORT,		1,	0
},
  
{COMM_RTE_EVENT_FIRMWARE_COPY_FAIL, CEventKb::EVT_FLAG_TRANSIENT, 
	0, 1, 0, 1, "COPYFAIL", "RTE Copy Firmware to Twin Failed", 0,
	COMM_ALARM_LEVEL_JUST_REPORT,		1,	0
},

	
{COMM_RTE_EVENT_NONE, 0, 0, 0, 0, 0, NULL, 0} // Last one 

};

CEventBasic::CEventBasic(CEventKb *pKb)
{
	CEventApp::AddEventList(m_eventList, pKb);
}

CEventBasic::~CEventBasic()
{

}

