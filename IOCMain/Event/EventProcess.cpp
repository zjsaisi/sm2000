// EventProcess.cpp: implementation of the CEventProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventProcess.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventProcess.cpp 1.5 2008/03/06 16:55:11PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.5 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "EventId.h"
#include "EventProcess.h"
#include "Queue.h"
#include "EventStore.h"
#include "AllProcess.h"
#include "Istate.h"
#include "EventKb.h"
#include "EventCondition.h"
#include "comm.h"
#include "stdio.h"
#include "event.h"
#include "rtc.h"
#include "PTPLink.h"
#include "PTPLink.h"
#include "MemBlock.h"
#include "errorcode.h"
#include "HouseProcess.h"
#include "Factory.h"
#include "CardStatus.h"

// Depending on application. Choose the level of event
#include "EventBasic.h"
#include "EventDti.h"

#define TRACE PrintDebugOutput 
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEventProcess *CEventProcess::s_pEventProcess = NULL;

CEventProcess::CEventProcess(CPTPLink *pIMCLink, 
	CEventStore *pEventStore, CIstate *pIstate,
	const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry):
CBaseProcess(name, priority, stack_size, entry)
{
	m_refreshEventId = 0;
	m_holdEventCnt = 0; 
	HoldEvent();

	m_pQueue = new CQueue(CEventStore::EVENT_ITEM_SIZE, 200);
	m_pQueueCondCmd = new CQueue(sizeof(SRtrvCmd), 15);

	m_bEnableLog = 1;
	m_pIMCLink = pIMCLink;
	m_debugOption = 1L;
	if (s_pEventProcess == NULL) {
		s_pEventProcess = this;
	}
	m_pEventStore = pEventStore;
	
	m_pKB = new CEventKb(pIstate, this);
	new CEventBasic(m_pKB); 
#if (!defined(BASIC_EVENT_ONLY)) || (BASIC_EVENT_ONLY == 0)
	new CEventDti(m_pKB);
#endif
	m_pCond = new CEventCondition(m_pKB);

}

CEventProcess::~CEventProcess()
{

}

void CEventProcess::Entry()
{
	s_pEventProcess->entry();
}

void CEventProcess::entry()
{
	m_pIMCLink->SetupClient(PTP_IOC_PORT_COND, this);
	EventEntry(TICK_1SEC * 5);
}

void CEventProcess::EventTimeOut()
{
	if (m_holdEventCnt > 0) {
		if (m_holdEventCnt > 5) {
			m_holdEventCnt -= 5;
		} else {
			ReleaseEvent();
		}
	}
}

void CEventProcess::EventDispatch(uint16 event)
{
	int ret;
	uint8 bLog;
	uint8 bReport;

	if ((event & EVT_NEW_EVENT_ITEM) && (m_holdEventCnt <= 0)) {
		SEventItem item;
		SEventReport report;

		//TRACE("Receive signal\r\n");

		do {
			ret = m_pQueue->GetTop((uint8 *)&item);
			bLog = item.active & 0x80;
			bReport = item.active & 0x40;
			item.active &= 0x3f;
			if (ret >= 1) {
				if (m_holdEventCnt < 0) {
					// Refresh alarm level
					if (item.active == 1) {
						uint8 alarm;
						m_pCond->OpenCondition();
						m_pCond->UpdateCondition(item.event_id);
						m_pCond->CloseCondition();
						m_pKB->GetAlarmLevel(item.event_id, &alarm);
						item.alarm_level = alarm;
					}

				}

				if ((bLog) && (m_bEnableLog)) {
					m_pEventStore->AppendItem((uint8 *)&item);
				}

				// Send it out to PPC
				report.cmd = 0x100;
				report.event_id = item.event_id;
				report.active = item.active;
				report.alarm_level = item.alarm_level;
				report.aid1 = item.aid1;
				report.aid2 = item.aid2;
				report.time_stamp = item.time_stamp;
				if (bReport) {
					m_pIMCLink->PostFrame((char *)&report, sizeof(SEventReport), PTP_IMC_PORT_AO);
				}

				if (m_debugOption) {
					if (m_debugOption & 1) {
						// Print all new ones
						PrintEvent(&item); 
					} else if ((m_debugOption & 2) && (item.active == 1)) {
						// Print only the active one
						PrintEvent(&item);
					} else if ((m_debugOption & 4) && (item.active == 0)) {
						// Print inactive one
						PrintEvent(&item);
					}
				}

			} else {
#if 0			// It's ok. This expected message might be consumed along with previous message.
				PrintDebugError("Rtrieve error[%d]\r\n", ret);
#endif
			}
		} while (ret == 1);
		m_holdEventCnt = 0; // Initially stuck event get processed
	}

	if (event & EVT_ONE_MINUTE) {
		// Do alarm upgrade stuff
		m_pCond->TickPerMinute();
	}

	// Get rtrv condition command
	if (event & EVT_RTRV_COND_CMD) {
		RespondRtrvCond();
	}

	if (event & EVT_RESEND_TO_IMC) {
		ResendEventToImc();
	}
}

// Report event from other modules
// initAlarm is only used for alarm upgrading. It's called within event module.
// For external call, initAlarm should be left out. 
int CEventProcess::ReportEvent(int16 id, uint8 active, uint8 aid1, uint8 aid2, uint8 initAlarm)
{
	uint32 timeStamp;
	int toStore;
	uint8 log = 0;
	uint8 alarm = initAlarm; 
	uint8 bReportIMC = 0;

	if (id >= COMM_RTE_EVENT_MAX) return -1;

	// After Check if it's new status
	timeStamp = CAllProcess::g_pAllProcess->GetComputerSeconds(); //g_pAll->GetGPSSeconds();

	// Make sure it's 1 or 0. Leave flexiblity to other parts.
	if (active) active = 1;

	// Report event to status
	m_pCond->OpenCondition();
	toStore = m_pCond->ReportCondition(id, active, aid1, aid2, timeStamp, &alarm, &bReportIMC);
	
	//TRACE("Report Evt%d act=%d toStore=%d\r\n", id, active, toStore);

	// Store it into log if necessary.
	if (toStore == 1) {
		int tmp = m_pKB->NeedLogged(id, &log);
		if ((tmp == 1) && (log == 1)) {
			//TRACE("Store & report event\r\n");
			// Log this event to flash and report to PPC
			StoreEvent(id, active, alarm, aid1, aid2, timeStamp, 1, bReportIMC);
		} else if (tmp == 1) {
			// Don't log it but report it to PPC
			//TRACE("Store event only\r\n");
			StoreEvent(id, active, alarm, aid1, aid2, timeStamp, 0, bReportIMC);
		}
	}

	// Setup Alarm Led
	SetupAlarmLed();

	m_pCond->CloseCondition();
	return toStore;
}

void CEventProcess::StoreEvent(int16 id, uint8 active, uint8 alarm, uint8 aid1, uint8 aid2,
							   uint32 timeStamp, uint8 bLog, uint8 bReport /* = 1 */)
{
	SEventItem item;
	
	item.event_id = id;
	// Put log flag into active. When retrieve from queue, get it out.
	item.active = active | ((bLog) ? (0x80) : (0)) | ((bReport) ? (0x40) : (0)) ;
	item.alarm_level = alarm;
	item.aid1 = aid1;
	item.aid2 = aid2;
	item.time_stamp = timeStamp; 
	int ret = m_pQueue->Append((unsigned char *)&item);
	SignalEvent(EVT_NEW_EVENT_ITEM);
}

void CEventProcess::ConditionHack(unsigned long *param, Ccomm *pComm)
{
	int16 event, start, end;
	CEvent *pEvt, *pNext;
	char buff[180];

	if (*param == 0) {
		const char *ptr = "0: List all event status\r\n1: List active events\r\n\
2 <id>: List the condition of event <id>\r\n\
3: Test rtrv-cond command, rtrv all\r\n\
4: Test rtrv-cond command, rtrv conditionally(active:all standby: partial)\r\n\
";
		pComm->Send(ptr);
		return;
	}

	m_pKB->GetEventIdRange(&start, &end);

	if ((*param == 1) && ((param[1] == 1) || (param[1] == 0))) {
		for (event = start+1; event < end; event++) {
			pEvt = m_pKB->GetEventStatus(event);
			if (pEvt == NULL) {
				sprintf(buff, "Evt[%d] not supported\r\n", event);
				pComm->Send(buff);
				continue;
			}
			do {
				pEvt->PrintCondition(pComm, param[1]);
				pEvt = pEvt->GetNext();
			} while (pEvt != NULL);
		}
		return;
	}

	// List specific event condition
	if ((*param == 2) && (param[1] == 2)) {
		event = param[2];
		pEvt = m_pKB->GetEventStatus(event);
		if (pEvt == NULL) {
			sprintf(buff, "Evt[%d] not supported\r\n", event);
			pComm->Send(buff);
			return;
		}
		do {
			pEvt->PrintCondition(pComm, 0L);
			pEvt = pEvt->GetNext();
		} while (pEvt != NULL);
		return;
	}

	// test rtrv-condition
	if ((*param == 1) && ((param[1] == 3) || (param[1] ==4) )) {
		uint32 seq;
		int cnt, i;
		int option = (param[1] == 3) ? (0) : (1);
		CEvent::SConditionBlock cond[MAX_CONDITION_NUM + 1];
		int printCnt = 0;

		seq = 0L;
		do {
			memset(cond, 0, sizeof(cond));
			cnt = RtrvCondition(option, &seq, cond);
			// Print buff
			for (i = 0; i <= cnt; i++) { // Have extra one to find any step-over
				sprintf(buff, "[%03d] id=%04X active=%d alm=%02X aid1=%d aid2=%d Time=%08lX ex1=%lX ex2=%lX\r\n",
					printCnt,			
					cond[i].event_id, cond[i].active, cond[i].alarm_level, cond[i].aid1,
					cond[i].aid2, cond[i].time_stamp, cond[i].extra1, cond[i].extra2);
				pComm->Send(buff);
				printCnt++;
			}
			sprintf(buff, "Sequence=%08lX\r\n", seq);
			pComm->Send(buff);

			if (cnt < MAX_CONDITION_NUM) {
				break;
			}

		} while (1);
		return;
	}

	pComm->Send("Invalid Option\r\n");
}


int16 CEventProcess::RtrvSingleCondition(int16 event_id, uint8 aid1, uint8 aid2, CEvent::SConditionBlock *pBlock)
{
	int16 ret;
	CEvent *pEvt;
	uint8 lAid1, lAid2;

	pEvt = m_pKB->GetEventStatus(event_id);
	if (pEvt == NULL) {
		return ERROR_RTRV_EVENT_INVALID_EVENT_ID; // Invalid Event id
	}
	
	do {
		lAid1 = aid1; lAid2 = aid2;
		ret = pEvt->DumpCondition(0xffff, 1, pBlock, &lAid1, &lAid2, 0); // Rrtrv event status acitve or not
		if ((ret == 1) && (lAid1 == (aid1 + 1)) && (lAid2 == aid2)) {
			return 1;
		}
		pEvt = pEvt->GetNext();
	} while (pEvt != NULL);

	memset(pBlock, 0, sizeof(CEvent::SConditionBlock));
	return ERROR_RTRV_EVENT_INVALID_AID;
}

// sequence = [event_id: 16bit] [NextLevel: 4-bit] [aid2: 6-bit] [aid1: 6-bit]
int CEventProcess::RtrvCondition(int option, uint32 *sequence, CEvent::SConditionBlock *pBlock)
{
	int lines = 0;
	uint32 seq = *sequence;
	int16 event_id;
	uint8 aid2, aid1;
	int16 start, end;
	CEvent *pEvt;
	int ret, i;
	int nextLevel;                  
	uint16 flag = CEventKb::EVT_FLAG_REPORT_COND; // Rertrive all active event

	if (option == 1) {
		// If standby card, don't report all
#if 1 
		if (!IS_ACTIVE) {
			flag = CEventKb::EVT_FLAG_SLAVE_REPORT;
		}
#else
		int state = m_pCardState->GetCardState();
		if (state != CCardState::CARD_STATE_ACTIVE) {
			// I am not active card.
			flag = CEventKb::EVT_FLAG_SLAVE_REPORT;
		}
#endif		
	} 

	m_pKB->GetEventIdRange(&start, &end);

	m_pCond->OpenCondition();

	// Get current starting point
	if (seq == 0L) {
		// New request
		event_id = start + 1;
		aid2 = 0; aid1 = 0; nextLevel = 0;
	} else {	
		// Get starting point
		event_id = (seq >> 16) & 0x7fff;
		nextLevel = ((uint8)(seq >> 12)) & 0xf;
		aid2 = ((uint8)(seq >> 6)) & 0x3f;
		aid1 = ((uint8)seq) & 0x3f;
	}

	do {
		pEvt = m_pKB->GetEventStatus(event_id);
		for (i = 0; i < nextLevel; i++) {
			if (pEvt != NULL) {
				pEvt = pEvt->GetNext();
			}
		}

		if (pEvt != NULL) {
			do {
				ret = pEvt->DumpCondition(flag, MAX_CONDITION_NUM - lines, pBlock, &aid1, &aid2);
				lines += ret;
				pBlock += ret;
				if (lines < MAX_CONDITION_NUM) {
					pEvt = pEvt->GetNext();
					nextLevel++;
					aid2 = 0; aid1 = 0;
				} else {
					break;
				}
			} while (pEvt != NULL);

			if (lines < MAX_CONDITION_NUM) {
				event_id++; // Get to next one
				aid1 = 0; aid2 = 0; nextLevel = 0;
			}
		} else {
			// Not supported any more, get to next one
			if (event_id < end) event_id++;
			aid1 = 0; aid2 = 0; nextLevel = 0;
		}
	} while ((event_id < end) && (lines < MAX_CONDITION_NUM));

	*sequence = (((uint32)event_id) << 16) + (((uint32)nextLevel) << 12) + 
		(((uint32)aid2) << 6) + ((uint32)aid1);

	m_pCond->CloseCondition();

	return lines;
}

// Called by PrintCondition to list current status.
// Also called by DebugDL to list event log
void CEventProcess::PrintEvent(SEventItem *pItem, Ccomm *pComm)
{
	CRTC rtc;
	char buff[120];
	char const * pAcronym;
	char const * pDesc;
	int year, month, day, hour, minute, second;

	pDesc = m_pKB->GetDebugString(pItem->event_id, &pAcronym);
	if (pDesc == NULL) {

		if (pComm == NULL) {
			PrintDebugOutput("Unknown Evt[%d] Aid1=%d Aid2=%d\r\n", pItem->event_id,
				pItem->aid1, pItem->aid2);
		} else {
			sprintf(buff, "Unknown Evt[%d] Aid1=%d Aid2=%d\r\n", pItem->event_id,
				pItem->aid1, pItem->aid2);
			pComm->Send(buff);
		}
		return;
	}
	rtc.SetComputerSeconds(pItem->time_stamp);
	rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);

#if 0
	sprintf(buff, "%s[0x%X] Act=%d Alm=%s,%s Aid1=%d Aid2=%d\t%04d-%02d-%02d %02d-%02d-%02d \"%s\"\r\n",
		pAcronym, pItem->event_id, pItem->active, 
		(pItem->active == 1) ? (CEvent::AlmString(pItem->alarm_level)) : ("CL"),
		(pItem->alarm_level & 0x10) ? ("SA") : ("NSA"),
		pItem->aid1, pItem->aid2,		
		year, month, day, hour, minute, second,
		pDesc);
	if (pComm == NULL) {
		DebugOutput(buff);
	} else {
		pComm->Send(buff);
	}
#else
	if (pComm == NULL) {
		PrintDebugOutput("%s[0x%X] Act=%d "
			"Alm=%s,%s"
			" Aid1=%d Aid2=%d"
			"\t%04d-%02d-%02d %02d-%02d-%02d"
			" \"%s%s\"\r\n",
			pAcronym, pItem->event_id, pItem->active, 
			(pItem->active == 1) ? (CEvent::AlmString(pItem->alarm_level)) : ("CL"), (pItem->alarm_level & 0x10) ? ("SA") : ("NSA"),
			pItem->aid1, pItem->aid2,		
			year, month, day, hour, minute, second,
			(pItem->active == 1) ? ("") : ("ENDED "), pDesc);
	} else {
		sprintf(buff, "%s[0x%X] Act=%d"
			" Alm=%s,%s"
			" Aid1=%d Aid2=%d"
			"\t%04d-%02d-%02d %02d-%02d-%02d"
			" \"%s%s\"\r\n",
			pAcronym, pItem->event_id, pItem->active, 
			(pItem->active == 1) ? (CEvent::AlmString(pItem->alarm_level)) : ("CL"), (pItem->alarm_level & 0x10) ? ("SA") : ("NSA"),
			pItem->aid1, pItem->aid2,		
			year, month, day, hour, minute, second,
			(pItem->active == 1) ? ("") : ("ENDED "), pDesc);
		pComm->Send(buff);
	}
#endif
}

void CEventProcess::TickPerMinute()
{
	SignalEvent(EVT_ONE_MINUTE);
}


int CEventProcess::Notify(uint32 from, uint32 what)
{
	CMemBlock *pBlock;

	if (from == (uint32)m_pIMCLink) {
		pBlock = (CMemBlock *)what;
		if (pBlock->GetCurSize() <= sizeof(SRtrvCmd)) { // It could be rtrv-list or rtrv single
			m_pQueueCondCmd->Append(pBlock->GetBuffer());
			pBlock->Release();
			SignalEvent(EVT_RTRV_COND_CMD);
			return 1;
		} 
		return 0;
	}
	return 0;
}

void CEventProcess::RespondRtrvCond()
{
	SRtrvCmd cmd;
	SRtrvResp resp;
	uint32 seq;
	int ret;

	do {
		ret = m_pQueueCondCmd->GetTop((uint8 *)&cmd);

		if (ret >= 1) {
			memset(&resp, 0, sizeof(resp)); // No necessary. for easy read only on imc
			resp.cmd = CMD_RTRV_LIST;
			resp.handle = cmd.handle;
			resp.error_code = 1; // No error
			resp.cnt = 0;

			if (cmd.cmd == CMD_RTRV_SINGLE_EVENT) {
				// Rtrv single event status
				SRtrvSingleCmd *pSingleCmd;
				SRtrvSingleResp singleResp;
				pSingleCmd = (SRtrvSingleCmd *)&cmd;
				memset(&singleResp, 0, sizeof(singleResp));
				singleResp.cmd = CMD_RTRV_SINGLE_EVENT;
				singleResp.handle = pSingleCmd->handle;
				singleResp.error_code = RtrvSingleCondition(pSingleCmd->event_id, pSingleCmd->aid1,
					pSingleCmd->aid2, &singleResp.condition);
				m_pIMCLink->PostFrame((char *)&singleResp, sizeof(singleResp), 
					PTP_IMC_PORT_COND);
				continue;
			}

			if (cmd.cmd != CMD_RTRV_LIST) {
				// Command not supported
				//char buff[100];
				resp.error_code = ERROR_UNKNOWN_COMMAND;
				m_pIMCLink->PostFrame((char *)&resp, sizeof(resp),
					PTP_IMC_PORT_COND);

				PrintDebugOutput("Unexpected Cmd[Event]: %X\r\n", cmd.cmd);
				continue;
			}
			seq = cmd.sequence;
			resp.cnt = RtrvCondition(cmd.option, &seq, resp.condition);
			resp.sequence = seq;
			m_pIMCLink->PostFrame((char *)&resp, sizeof(resp),
				PTP_IMC_PORT_COND);
		}
	} while (ret == 1);
}

//void CEventProcess::DebugOutput(const char *str)
//{
//	g_pAll->DebugOutput(str, -1, (uint32)1 << 21);
//}

//void CEventProcess::sDebugOutput(const char *format, ...)
//{
//	va_list list;
//	va_start(list, format);
//	g_pAll->vDebugOutput(format, (uint32)1 << 21, list);
//	va_end(list);
//}

void CEventProcess::SetupAlarmLed()
{
	int minor, major, critical;
	int all;

	CEvent::CountAllAlarms(&minor, &major, &critical);
	all = minor + major + critical;
}


void CEventProcess::SetEnableLog(int16 bEnableLog)
{
	m_bEnableLog = bEnableLog;
}

void CEventProcess::Init()
{
	//TRACE("EVENT process Slot: %d\r\n", GetSlot());
	m_pKB->Init();
	SetEnableLog(CFactory::s_pFactory->GetEnableLog());
}

void CEventProcess::EventHack(unsigned long *param, Ccomm *pComm)
{
	int ret;
	int pairs;
	char buff[100];

	if (*param == 0) {
		const char *ptr;
		ptr = "EVT 1 <id> <active> <aid1> <aid2>\r\n\tGenerate event\r\n"
			"EVT 2 <id> <aid1> <aid2>\r\n\tGenerate 10 pairs of ON/OFF event\r\n"
			"EVT 3 <id> <aid1> <aid2>\r\n\tGenerate 100 pairs of ON/OFF event\r\n"
			"EVT 4 <id> <aid1> <aid2>\r\n\tGenerate 1000 pairs of ON/OFF event\r\n"
			"EVT 5 <id> <active> <aid1> <aid2> <alarm>\r\n\tGenerate event with alarm level specified\r\n"
			;
		pComm->Send(ptr);
		return;
	}

	if ((*param == 5) && (param[1] == 1)) {
		ret = REPORT_EVENT(param[2], param[3], param[4], param[5]);
		sprintf(buff, "Ret=%d\r\n", ret);
		pComm->Send(buff);
		return;
	}

	if ((*param == 4) && (param[1] == 2)) {
		pairs = 10;
	}
	if ((*param == 4) && (param[1] == 3)) {
		pairs = 100;
	}
	if ((*param == 4) && (param[1] == 4)) {
		pairs = 1000;
	}

	if (pairs != 0) {
		int cnt = 0;
		for (int i = 0; i < pairs; i++) {
			ret = REPORT_EVENT(param[2], 1, param[3], param[4]);
			sprintf(buff, "Event[%ld] On ret=%d\r\n", param[2], ret);
			pComm->Send(buff);
			ret = REPORT_EVENT(param[2], 0, param[3], param[4]);
			sprintf(buff, "Event[%ld] Off ret=%d\r\n", param[2], ret);
			pComm->Send(buff);
			if (cnt++ >= 10) {
				cnt = 0;
				Pause(TICK_1SEC);
			}
		}
		return;
	}

	if ((*param == 6) && (param[1] == 5)) {
		ret = ReportEvent(param[2], param[3], param[4], param[5], param[6]);
		sprintf(buff, "Ret=%d\r\n", ret);
		pComm->Send(buff);
		return;
	}

	pComm->Send("Invalid Option\r\n");

}

void CEventProcess::LogHack(unsigned long *param, Ccomm *pComm)
{
	int i;
	int cnt;
	int total, ret;
	char ch;
	unsigned long evtIndex = 0xffffffff;
	SEventItem recList[10];
	char buff[80];
	int bPause = 1;
	int bEnableLog;
	CFactory *pFactory;

	pFactory = CFactory::s_pFactory; 
	bEnableLog = pFactory->GetEnableLog();

	if (*param != 2) {
		char const *ptr = 
			"1 <cnt>: Retrieve most recent <cnt> of event log, Pause for every 10\r\n"
			"2 <cnt>: Same as option 1 except no pause\r\n"
			"3 <logEnable>: Enable(1)/Disable(0) writing to event log\r\n"
			"4 1: Print status\r\n"
			;
		pComm->Send(ptr);
		sprintf(buff, "Event log is %s\r\n", 
			(bEnableLog) ? ("Enabled") : ("Disabled"));
		pComm->Send(buff);
		return;
	}

	if (param[1] == 2) {
		bPause = 0;
		param[1] = 1;
	}

	if (param[1] == 1)  {
		CEventStore *pEvtStore;
		pEvtStore = GetEventStore();
		total = 0;
		do {
			cnt = pEvtStore->RtrvItems(evtIndex, &evtIndex, (uint8 *)recList, 10);
			for (i = 0; i < cnt; i++) {
				sprintf(buff, "%04d: ", total); 
				pComm->Send(buff);
				total++;
				PrintEvent(recList + i, pComm);
				if (total >= param[2]) {
					return;
				}
				if ( ((total % 10) == 0) && bPause) {
					do {
						pComm->flush();
						ret = pComm->Receive(&ch, 1, 1000);
						if (ret != 1) continue;
						if ((ch == 'Q') || (ch == 'q')) {
							return;
						}
						if ((ch == 'G') || (ch == 'g')) {
							bPause = 0;
						}
						if ((ch == '\r') || (ch == '\n')) {
							ret = 0;
							continue;
						}
					} while (ret != 1);
				}
			}
		} while ((cnt > 0) && (evtIndex != 0x00ffffff));
		return;
	}

	if (param[1] == 3L) {
		
		bEnableLog = (param[2]) ? (1) : (0);
		pFactory->SetEnableLog(bEnableLog);
		SetEnableLog(bEnableLog);
		sprintf(buff, "Event log is %s\r\n", (bEnableLog) ? ("Enabled") : ("Disabled"));
		pComm->Send(buff);
		return;
	}

	if ((param[1] == 4) && (param[2] == 1)) {
		sprintf(buff, "HoldCnt: %d\r\n"
			, m_holdEventCnt);
		pComm->Send(buff);
		return;
	}

	pComm->Send("Invalid Option\r\n");

}

void CEventProcess::HoldEvent()
{
	if (m_holdEventCnt <= 0) {
		m_holdEventCnt = 20;
	}
}

void CEventProcess::ReleaseEvent()
{
	if (m_holdEventCnt > 0) {
		m_holdEventCnt = -20;
		SignalEvent(EVT_NEW_EVENT_ITEM);
		return;
	}
	SignalEvent(EVT_NEW_EVENT_ITEM);
}

void CEventProcess::ResendEventToImc()
{
	int evtId;
	int from, to;
	CEvent *pEvtStat;
	int used, avail;

	// Check the link availability
	m_pIMCLink->GetSendUsage(&used, &avail);
	if (used >= 10) {
		// Hold on
		Pause(TICK_1SEC / 5);
		ResendEventToImcTrigger();
		return;
	}

	m_pKB->GetEventIdRange(&from, &to);

	if (m_refreshEventId == 0) {
		m_refreshEventId = from + 1;
		TRACE("Refresh Alarm to PPC\r\n");
	} else if (m_refreshEventId >= to) {
		m_refreshEventId = 0; // We are done here.
		TRACE("Refresh Alarm done to PPC\r\n");
		return;
	}

	do {
		pEvtStat = m_pKB->GetEventStatus(m_refreshEventId);
		if (pEvtStat != NULL) {
			m_pCond->OpenCondition();
			pEvtStat->ResendToImc(m_pIMCLink);	
			m_pCond->CloseCondition();
		} 
		m_refreshEventId++;
	} while ((pEvtStat == NULL) && (m_refreshEventId < to));
	ResendEventToImcTrigger();

#if 0
	m_pCond->OpenCondition();
	for (evtId = from + 1; evtId < to; evtId++) {
		pEvtStat = m_pKB->GetEventStatus(evtId);
		while (pEvtStat != NULL) {
			if (!(pEvtStat->GetFlag() & CEventKb::EVT_FLAG_RESEND_PPC)) {
				break;
			}

		}
	}
	m_pCond->CloseCondition();
#endif
}

void CEventProcess::ResendEventToImcTrigger()
{
	SignalEvent(EVT_RESEND_TO_IMC);
}
