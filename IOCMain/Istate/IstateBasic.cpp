// IstateBasic.cpp: implementation of the CIstateBasic class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateBasic.cpp 1.2 2008/01/29 11:16:27PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "IstateBasic.h"
#include "string.h"
#include "EventId.h"
#include "IstateKbIoc.h"
#include "AllProcess.h"
#include "Istate.h"
#include "IstateProcess.h"
#include "NVRAM.h"
#include "PTPDnldProcess.h"
#include "EventProcess.h"
#include "EventKb.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACE  TRACE_ISTATE

CIstateBasic *CIstateBasic::s_pIstateBasic = NULL;

CIstateBasic::CIstateBasic(CIstateKb *pKb)
{
	s_pIstateBasic = this;
	m_pEventKb = NULL;
	AddIstateList(pKb, m_istateList);
}

CIstateBasic::~CIstateBasic()
{

}

void CIstateBasic::Init()
{
	m_pEventKb = CEventProcess::s_pEventProcess->GetEventKB();
}

SIstateItem const CIstateBasic::m_istateList[NUM_ISTATE + 1] = {
	{ COMM_RTE_KEY_EVENT_LOG, (void *)&dt_eventLog },
	{ COMM_RTE_KEY_ALARM_LEVEL, (void *)&de_eventAlarmLevel },
	{ COMM_RTE_KEY_SERVICE_AFFECT, (void *)&dt_eventServiceAffect } ,
	{ COMM_RTE_KEY_MINOR_UPGRADE_TIME, (void *)&de_minorUpgradeTime },
	{ COMM_RTE_KEY_MAJOR_UPGRADE_TIME, (void *)&de_majorUpgradeTime },
	{ COMM_RTE_KEY_MISC_ACTION, (void *)&de_miscAction },
	{ COMM_RTE_KEY_FIRMWARE_VERSION, (void *)&de_firmwareVersion },
	{ COMM_RTE_KEY_ISTATE_VERSION, (void *)&de_istateVersion },

	// Add new one here. Ready to change IOC_ISTATE_VERSION
	{ 0, NULL }
};


SToggleIstate const CIstateBasic::dt_eventServiceAffect = { // COMM_RTE_KEY_SERVICE_AFFECT
	"Event Service Affect",
	(IstateFlag) (FLAG_IS_TOGGLE_ISTATE | FLAG_READ_ONLY_ISTATE), // IstateFlag flag;
	"EVTSA", // char const * const name; This is special, it's not keyword
	COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.vt_eventServiceAffect, // void *pVariable;	
	ActionServiceAffect, // ActionEventServiceAffect, //FUNC_PTR pAction;
	InitServiceAffect, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam; // 1 for service affecting, 0: not
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	BIT8 // IstateSize size_type; // length of unit 8-bit 32-bit

};

void CIstateBasic::InitServiceAffect(void *pEntry)
{
	CEventKb *pEventKb;
	int16 evt;
	int sa;
	int size;
	SToggleIstate *pItem = (SToggleIstate *)pEntry;

	size = pItem->max_first_index;
	memset(pItem->pVariable, 0, size);

	pEventKb = CEventProcess::s_pEventProcess->GetEventKB();
	for (evt = COMM_RTE_EVENT_NONE + 1; (evt < COMM_RTE_EVENT_MAX) && (evt < size); evt++) {
		sa = pEventKb->GetDefaultServiceAffect(evt);
		((uint8 *)pItem->pVariable)[evt] = (uint8)sa;
	}

}

void CIstateBasic::ActionServiceAffect(int16 index1, int16 index2, uint32 value)
{
	ActionAlarmLevel(index1, index2, value);
}

SToggleIstate const CIstateBasic::dt_eventLog = { // COMM_RTE_KEY_EVENT_LOG
	"Event log",
	(IstateFlag)(FLAG_IS_TOGGLE_ISTATE | FLAG_READ_ONLY_ISTATE), // IstateFlag flag;
	"EVTLOG", // char const * const name; This is special, it's not keyword
	COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.vt_eventLog, // void *pVariable;	
	CIstateKb::ActionNull, //ActionEventLog, //FUNC_PTR pAction;
	InitEventLog, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam; // 1 for log, 0: not log
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	BIT8 // IstateSize size_type; // length of unit 8-bit 32-bit

};

void CIstateBasic::InitEventLog(void *pEntry)
{
	CEventKb *pEventKb;
	int16 evt;
	int log;
	int size;
	SToggleIstate *pItem = (SToggleIstate *)pEntry;

	size = pItem->max_first_index;
	memset(pItem->pVariable, 0, size);

	pEventKb = CEventProcess::s_pEventProcess->GetEventKB();
	for (evt = COMM_RTE_EVENT_NONE + 1; (evt < COMM_RTE_EVENT_MAX) && (evt < size); evt++) {
		log = pEventKb->GetDefaultLog(evt);
		((uint8 *)pItem->pVariable)[evt] = (uint8)log;
	}

}

SEntryIstate const CIstateBasic::de_eventAlarmLevel = { // COMM_RTE_KEY_ALARM_LEVEL
	"Event Alarm level",
	FLAG_READ_ONLY_ISTATE, // IstateFlag flag;
	"ALMLVL", // char const * const name;
	COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_eventAlarmLevel, // void *pVariable;	
	ActionAlarmLevel, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	InitAlarmLevel, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	COMM_ALARM_LEVEL_NULL, // int32 minEntryValue;
	COMM_ALARM_LEVEL_MAX - 1// int32 maxEntryValue;
};

void CIstateBasic::InitAlarmLevel(void *pEntry)
{
	CEventKb *pEventKb;
	int16 evt;
	int alarm;
	int size;
	SEntryIstate *pItem = (SEntryIstate *)pEntry;

	size = pItem->max_first_index;
	memset(pItem->pVariable, 0, size * sizeof(uint32));

	pEventKb = CEventProcess::s_pEventProcess->GetEventKB();
	for (evt = COMM_RTE_EVENT_NONE + 1; (evt < COMM_RTE_EVENT_MAX) && (evt < size); evt++) {
		alarm = pEventKb->GetDefaultAlarmLevel(evt);
		((uint32 *)pItem->pVariable)[evt] = alarm;
	}
}

void CIstateBasic::ActionAlarmLevel(int16 index1, int16 index2, uint32 value)
{
	int i;
	int start, end;
	CEvent *pEvt;
    CEventKb *pKb;
    
	if ((index1 < 0) || (index2 < 0)) {
		start =  COMM_RTE_EVENT_NONE + 1;
		end = COMM_RTE_EVENT_MAX;
	} else {
		start = index1;
		end = start + 1;
	}
    
	pKb = s_pIstateBasic->m_pEventKb; 
	for (i = start; i < end; i++) {
		pEvt = pKb->GetEventStatus(i);
		if (pEvt != NULL) {
			pEvt->RegenerateEvent();
		}
	}
}

SEntryIstate const CIstateBasic::de_minorUpgradeTime = { // COMM_RTE_KEY_MINOR_UPGRADE_TIME
	"Minor alarm Upgrade time",
	FLAG_NULL, // IstateFlag flag; Entry
	"MNUPT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&(CIstateKbIoc::m_istateInRam.ve_minorUpgradeTime), // void *pVariable;
	CIstateKb::ActionNull, // FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, // FUNC_INIT pInit;
	0x7fffffff, //24*60, //int32 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;
	
	1, // int32 minEntryValue; inclusive
	0x7fffffff // int32 maxEntryValue; inclusive

};

SEntryIstate const CIstateBasic::de_majorUpgradeTime = { // COMM_RTE_KEY_MAJOR_UPGRADE_TIME
	"Major alarm Upgrade",
	FLAG_NULL, // IstateFlag flag; Entry
	"MJUPT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&(CIstateKbIoc::m_istateInRam.ve_majorUpgradeTime), // void *pVariable;
	CIstateKb::ActionNull, // FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, // FUNC_INIT pInit;
	0x7fffffff, //24*60, //int32 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;
	
	1, // int32 minEntryValue; inclusive
	0x7fffffff // int32 maxEntryValue; inclusive

};

uint32 CIstateBasic::ve_miscAction = 0L;

SEntryIstate const CIstateBasic::de_miscAction = { // COMM_RTE_KEY_MISC_ACTION
	"Misc Action:\r\n" "Write\t1: To reset RTE Istate\r\n" 
	"\t2: Reboot the card\r\n" "\t3: Copy firmware to twin RTE\r\n"
	"\t4: Save setting to NVRAM immediately\r\n"
	,
	FLAG_STANDALONE_ISTATE, // IstateFlag flag;
	"MISC", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&ve_miscAction, // void *pVariable;	
	ActionMisc, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	NULL, //FUNC_INIT pInit;
	0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue;
	20// int32 maxEntryValue;
};
static uint32 miscDebugCnt = 0L;
void CIstateBasic::ActionMisc(int16 index1, int16 index2, uint32 value)
{
	int ret;
	if (index1 != 0) return;

	switch (value) {
	case 1: {
		CIstate *pIocIstate;
		CIstateKb *pIocKb;
		pIocIstate = CIstateProcess::s_pIstateProcess->GetIOCIstate();
		pIocKb = pIocIstate->GetKB();
		pIocKb->InitAllIstate();
		if (CAllProcess::g_pAllProcess->AmIMaster() == 1) {
			// Send it to twin
			CIstateProcess::s_pIstateProcess->ResendIOCIstateToTwin();
		}
		pIocKb->ActionAll();
		pIocIstate->SetModified();
			}
		break;
	case 2:
		//CAllProcess::g_pAllProcess->RebootSystem();
		CAllProcess::g_pAllProcess->GraceShutDown();
		break;
	case 3: // copy firmware
		CPTPDnldProcess::s_pPTPDnldProcess->StartSendCodeToTwinIOC();
		break;
	case 4: // Save NVRAM now 
		{
			CNVRAM *pNvram = CAllProcess::g_pAllProcess->GetNvram();
			ret = pNvram->SaveNVRAMCopy();
			TRACE("Saving NVRAM = %d\r\n", ret);
		}
		break;
	default:
		TRACE("Invalid MISC action: %ld (%ld)\r\n", value, miscDebugCnt);
		miscDebugCnt++;
		break;
	}

	ve_miscAction = 0L;
}

uint32 CIstateBasic::ve_firmwareVersion = VERSION_NUMBER;

SEntryIstate const CIstateBasic::de_firmwareVersion = { // COMM_RTE_KEY_FIRMWARE_VERSION
	"Help for FIRMVER:\r\n" "Read only firmware version\r\n"
	"Format is: 0x00XXYYZZ for version XX.YY.ZZ (shown as decimal on UI)\r\n"
		,
	FLAG_READ_ONLY_ISTATE, // IstateFlag flag;
	"FIRMVER", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&(ve_firmwareVersion), // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	VERSION_NUMBER, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0L, // int32 minEntryValue;
	0x7fffffffL // int32 maxEntryValue;

};

uint32 CIstateBasic::ve_istateVersion = (uint32)IOC_ISTATE_VERSION;

SEntryIstate const CIstateBasic::de_istateVersion = { // COMM_RTE_KEY_ISTATE_VERSION
	"Help for ISTVER:\r\n" "Read only istate version\r\n"
	"Format is: 0x0000XXXX for version 0xXXXX \r\n"
		,
	FLAG_READ_ONLY_ISTATE, // IstateFlag flag;
	"ISTVER", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&(ve_istateVersion), // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)IOC_ISTATE_VERSION, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0L, // int32 minEntryValue;
	0x7fffffffL // int32 maxEntryValue;

};
