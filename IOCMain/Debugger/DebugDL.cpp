// DebugDL.cpp: implementation of the CDebugDL class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DebugDL.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DebugDL.cpp 1.13 2009/05/05 14:41:06PDT Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.13 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "iodefine.h"
#include "CxFuncs.h"
#include "DebugDL.h"
#include "iodefine.h"
#include "string.h"
#include "YModemRecv.h"
#include "Storage.h"
#include "sci.h"
#include "stdio.h"
#include "Loader.h"
#include "AllProcess.h" 
#include "cxconfig.h"
#include "BaseProcess.h"
#include "IdleProcess.h"
#include "RTC.h"
#include "PTPIsr.h"
#include "PTPLink.h"
#include "machine.h"
#include "Factory.h"
#include "NVRAM.h"
#include "IstateProcess.h"
#include "IstateDti.h"
#include "PTPDnldProcess.h"
#include "Queue.h"
#include "EventProcess.h"
#include "NVRAM.h"
#include "AdcConverted.h"
#include "HouseProcess.h"
#include "DebugMask.h"
#include "Fpga.h"
#include "TwinProxy.h"
#include "DtiOutput.h"
#include "IOCKeyList.h"
#include "CardStatus.h"
#include "Amd29DL161DB.h"
#include "SecondProcess.h"
#include "DtiOutputPm.h"
#include "Input.h"
#include "BT3Debug.h"
#include "Warmup.h"
#include "WatchDog.h" 
#include "RubProcess.h"
#include "ZModemRecv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const CDebugDL::CmdStruct CDebugDL::m_cmdList[] = {
	{1, "YMODEM ", CMD_YMODEM, "YMODEM Y <baudrate>\t: Start YMODEM download"},
	{1, "Y ", CMD_YMODEM, NULL},
	{1, "RZ ", CMD_ZMODEM, "Start ZMODEM"},
	{2, "BAUD ", CMD_BAUDRATE, "BAUD <baudrate>\t\t: Change baudrate"}, 
	{2, "VER ", CMD_VERSION, "VER REV\t\t\t: Print revision name"},
	{2, "REV ", CMD_VERSION, NULL},
	{2, "REVNAME ", CMD_VERSION, NULL},
	{2, "DATE ", CMD_SET_DATE, "DATE <year> <month> <day>: Set Date"},
	{2, "TIME ", CMD_SET_TIME, "TIME <hour> <minute> <second>: Set Time"},
	{3, "PROC ", CMD_PROCESS, "PROC <option>\t\t: List process status"},
	{1, "ADC ", CMD_ADC, "ADC <option>\t\t: List ADC values"},
	{3, "SYSTEM ", CMD_SYS, "SYSTEM SYS\t\t: List system information"},
	{3, "SYS ", CMD_SYS, NULL},
	{3, "S ", CMD_SYS, NULL},
	{3, "COND ", CMD_CONDITION, "COND <option>\t\t: List condition"},
	{3, "LOG ", CMD_EVENT_LOG, "LOG <option> <cnt>\t: List event log"},
	{2, "DEBUGMASK ", CMD_DEBUG_MASK, "DEBUGMASK/DM <option>\t: Setup debug option"},
	{2, "DM ", CMD_DEBUG_MASK, NULL},
	{3, "EVT ", CMD_EVENT, "EVT <option> ... \t: Generate event"},
	{3, "ISTATE ", CMD_ISTATE, "ISTATE <options>\t: List Istate values"},
	{3, "NVRAM ", CMD_NVRAM, "NVRAM <options>\t\t: NVRAM debug"},
	{2, "WHOAMI ", CMD_WHOAMI, "WHOAMI \t\t\t: Which port I am on"},
	{2, "DIR ", CMD_DIR, "DIR\t\t\t: List main code in falsh" },
	{2, "DNLD ", CMD_DOWNLOAD, "DNLD <option>\t\t: Print Download info" },
	{2, "FPGA ", CMD_FPGA, "FPGA <option>\t\t: Hack FPGA info" },
	{4, "CARD ", CMD_CARD, "CARD <option>\t\t: Card Status"},
	{4, "OUT ", CMD_OUTPUT, "OUT <option>\t\t: DTI output ctrl"},
	{4, "FLASH ", CMD_FLASH, "FLASH <option>\t\t: Test Flash"},
	{4, "SEC ", CMD_SECOND, "SEC <option>\t\t: Debug info for Second Process"},
	{4, "OUTPM ", CMD_OUTPUT_PM, "OUTPM <options>\t\t: Debug output PM"},
	{2, "INPUT ", CMD_INPUT,	"INPUT <chan> <options>\t\t: Input status"},
	
	{3, "FACT ", CMD_MANF, "FACT <options>\t\t: Read/Setup factory info"},
	{3, "TEST ", CMD_TEST, "TEST\tT\t\t: Test code"},
	{3, "T ", CMD_TEST, NULL},

	{2, "OVENELAPSE ", CMD_OVENELAPSE, "OVENELAPSE\t\t: Force RTE out of warmup" },
	{3, "CNTRD ", CMD_CONTINUE_READ, "CNTRD <options>\t\t: Read memory continuously" },
	{3, "RMTDBG ", CMD_REMOTE_DEBUG, "RMTDBG <option>\t\t: Remote debug"},

	{1, "RD_SNO ",CMD_READ_SERIAL, "RD_SNO\t: List serial numbers including other factory settings"},
	{1, "WR_SNO ",CMD_WRITE_SERIAL, "WR_SNO\t: Modify serial numbers & other factory settings"},
	{3, "DEBUG ", CMD_DEBUG, "DEBUG <id> <option>\t: Set debug option for <id>"},
	{2, "SW ", CMD_SWITCH_ACTIVE, "SW \t\t\t: Switch card status"},
	{2, "ICW ", CMD_ICW,	"ICW <option>\t\t: ICW debug"},
	{2, "RWAVE ", CMD_READ_WAVE, "RWAVE <option>\t\t: Read wave form data"},
	{3, "PM ", CMD_PM, "PM <options>\t\t: PM debug"},
	{3, "DRAM ", CMD_DUAL_RAM, "DRAM <options>\t\t: Access Dual RAM in FPGA"},
	{3, "BT ", CMD_BT3, "BT\t\t\t: Bestime debug command"},
	{4, "WARM ", CMD_WARMUP, "WARM\t\t<option>: Warmup status"},
	{4, "HOUSE ", CMD_HOUSE, "Show House process status"},
	{4, "RUB", CMD_RUBIDIUM, "Rubidium OSC debug"},
	{0, NULL, CMD_END, NULL}
};

CDebugDL::CDebugDL(const char *name, CLoader *pLoader, CBaseProcess *pProcess)
{
	char buff[80];
	m_pProcess = pProcess;
	m_bAutoRateAllowed = 0; // Not Allow automatic baudrate
	m_pYModem = new CYModemRecv();
	m_pZModem = new CZModemRecv();
	m_pStore = new CCodeStorage("IOCCODE-DBG", pLoader);
	m_pLoader = pLoader;
	strncpy(m_name, name, 32);
	m_name[32] = 0;

	sprintf(buff, "\r\n%s >", name);
	SetPrompt(buff);

}

CDebugDL::~CDebugDL()
{

}

int CDebugDL::ExtendedGetCmd(const char *pCmd, int *pLen) 
{                   
	const CmdStruct *ptr = m_cmdList; 
	int cmdLen;
	int eq;
	while (ptr->cmdId != CMD_END) {
		 cmdLen = strlen(ptr->pCmdName);
		 eq = strncmp(pCmd, ptr->pCmdName, cmdLen);
		 if (eq == 0) {
			 *pLen = cmdLen;
			 return ptr->cmdId;
		 }
		 ptr++;
	}
	return(CMD_END); 
}

void CDebugDL::ExtendedExecCmd(int cmd, unsigned long *param) 
{ 
	//char buff[100];
	switch (cmd) {
	case CMD_BT3:
		CmdBT3(param);
		break;
	case CMD_SYS:
		CmdSys(param);
		break;
	case CMD_READ_SERIAL:
		CmdReadSerial(param);
		break;
	case CMD_WRITE_SERIAL:
		CmdWriteSerial(param);
		break;
	case CMD_ADC:
		CmdAdc(param);
		break;
	case CMD_SET_DATE:
		CmdSetDate(param);
		break;
	case CMD_SET_TIME:
		CmdSetTime(param);
		break;
	case CMD_PROCESS: 
		CmdProc(param);
		break;
	case CMD_VERSION: 
		ReportBootVersion();
		CAllProcess::g_pAllProcess->ReportVersion(m_pUserComm);
		break;
	case CMD_BAUDRATE: 
		CmdBaud(param);
		break;       
	case CMD_YMODEM: 
		CmdYModemRecv(param);
		break;
	case CMD_ZMODEM:
		CmdZModemRecv(param);
		break;
	case CMD_TEST:
		CmdTest(param);
		break;
	case CMD_CONDITION:
		CmdCondition(param);
		break;
	case CMD_EVENT:
		CmdEvent(param);
		break;
	case CMD_DEBUG:
		CmdDebug(param);
		break;
	case CMD_EVENT_LOG:
		CmdEventLog(param);
		break;
	case CMD_ISTATE:
		CmdIstate(param);
		break;
	case CMD_NVRAM:
		CmdNVRAM(param);
		break;
	case CMD_WHOAMI:
		CmdWHOAMI(param);
		break;
	case CMD_SWITCH_ACTIVE:
		CmdSwitchActive(param);
		break;
	case CMD_INPUT:
		CmdInput(param);
		break;
	case CMD_ICW:
		CmdICW(param);
		break;
	case CMD_DEBUG_MASK:
		CmdDebugMask(param);
		break;
	case CMD_READ_WAVE:
		CmdReadWave(param);
		break;
	case CMD_DOWNLOAD:
		CmdDownload(param);
		break;
	case CMD_OVENELAPSE:
		CmdOvenElapse(param);
		break;
	case CMD_DIR:
		CmdDir(param);
		break;
	case CMD_CONTINUE_READ:
		CmdContinueRead(param);
		break;
	case CMD_MANF:
		CmdManf(param);
		break;
	case CMD_REMOTE_DEBUG:
		CmdRemoteDebug(param);
		break;
	case CMD_PM:
		CmdPm(param);
		break;
	case CMD_DUAL_RAM:
#if COMPLETE	
		CDualRam::DebugAccess(param, m_port);    
#endif
		break;
	case CMD_FPGA:
		CmdFpga(param);
		break;
	case CMD_CARD:
		CTwinProxy::s_pTwinProxy->TwinProxyHack(param, m_port);
		break;
	case CMD_OUTPUT:
		CDtiOutput::DtiOutputHack(param, m_port);
		break;
	case CMD_FLASH:
		CmdFlash(param);
		break;
	case CMD_SECOND:
		CSecondProcess::s_pSecondProcess->HackSecondProcess(param, m_port);
		break;
	case CMD_OUTPUT_PM:
		CDtiOutputPm::s_pDtiOutputPm->DtiOutputPmHack(param, m_port);
		break;
	case CMD_WARMUP:
		CmdWarmup(param);
		break;
	case CMD_HOUSE:
		CHouseProcess::s_pHouseProcess->HackHouseProcess(param, m_port);
		break;
	case CMD_RUBIDIUM: 
		CmdRub(param);
		break;
	default:   
		SendStr("To be implemented\r\n");
		return;
	}
}

void CDebugDL::ExtendedPrintHelp(unsigned long *param) 
{
	const CmdStruct *ptr = m_cmdList;
	if (*param == 1) {
		while (ptr->cmdId != CMD_END) {
			if ((ptr->pComment) && ( param[1] & (1 << (ptr->option - 1)) )) {
				SendStr(ptr->pComment);
				SendStr("\r\n");
			}
			ptr++;
		}
	}
}

void CDebugDL::Start(CLoader *pLoader, Ccomm *pUserComm, Ccomm *pDebugComm, CFlash *pFlash)
{
	m_pDebugComm = pDebugComm;
	m_pUserComm = pUserComm;
	m_pFlash = pFlash;
	m_startingBaudrate = pUserComm->GetBaudRate();
	CDebug::Start(m_pUserComm, m_pFlash, 0);
}

void CDebugDL::CmdSetDate(unsigned long *param)
{
	char buff[150];
	int year, month, day, hour, minute, second;
	unsigned long gpsSeconds, computerSeconds;
	uint32 mjd;

	g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
	gpsSeconds = g_pRTC->GetGPSSeconds();
	mjd = g_pRTC->GetMjd();
	computerSeconds = gpsSeconds + 315964800L;
	sprintf(buff, "Current Time: %04d-%02d-%02d %02d-%02d-%02d"
		" GPS=%04X%04X"
		" PC=%04X%04X"
		" MJD=%05ld\r\n"
		,year, month, day, hour, minute, second,
		(uint16)(gpsSeconds >> 16), (uint16)(gpsSeconds), 
		(uint16)(computerSeconds >> 16), (uint16)(computerSeconds)
		, mjd);
	SendStr(buff);
	sprintf(buff, "Timing from %s, %s\r\n", 
		((TIMING_CPU) ? "CPU": "CSM"),
		((COPY_TIME_FROM_ACTIVE) ? ("Depend on Active") : ("Independent"))
		);
	SendStr(buff);
	if (*param != 3) {
		return;
	}
	year = param[1]; month = param[2]; day = param[3];
	g_pRTC->SetDateTime(year, month, day, hour, minute, second);

	sprintf(buff, "New Time: %04d-%02d-%02d %02d-%02d-%02d\r\n",
		year, month, day, hour, minute, second);
	SendStr(buff);

}

void CDebugDL::CmdSetTime(unsigned long *param)
{
	char buff[100];
	int year, month, day, hour, minute, second;
	unsigned long gpsSeconds, computerSeconds;
	uint32 mjd;

	g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
	mjd = g_pRTC->GetMjd();

	gpsSeconds = g_pRTC->GetGPSSeconds();
	computerSeconds = gpsSeconds + 315964800L;
	sprintf(buff, "Current Time: %04d-%02d-%02d %02d-%02d-%02d"
		" GPS=%04X%04X "
		"PC=%04X%04X"
		" MJD=%05ld\r\n",
		year, month, day, hour, minute, second,
		(uint16)(gpsSeconds >> 16), (uint16)gpsSeconds, 
		(uint16)(computerSeconds >> 16), (uint16)computerSeconds 
		, mjd
		);
	SendStr(buff);
	sprintf(buff, "Timing from %s, %s\r\n", 
		((TIMING_CPU) ? "CPU": "CSM"),
		((COPY_TIME_FROM_ACTIVE) ? ("Depend on Active") : ("Independent"))
		);
	SendStr(buff);
	if (*param != 3) {
		return;
	}

	hour = param[1]; minute = param[2]; second = param[3];
	g_pRTC->SetDateTime(year, month, day, hour, minute, second);

	sprintf(buff, "New Time: %04d-%02d-%02d %02d-%02d-%02d\r\n",
		year, month, day, hour, minute, second);
	SendStr(buff);

}

void CDebugDL::CmdAdc(unsigned long *param)
{    
	CAdcConverted *pAdc;

	pAdc = CHouseProcess::s_pHouseProcess->GetAdc();
	pAdc->PrintAdc(m_pUserComm, param);

#if 0
	CHlthProcess *pHlth;
	CAdcConverted *pAdc;

	pAdc = CAllProcess::g_pAllProcess->GetAdc();
	
	if (*param != 1) {
		SendStr("ADC <option>:\r\n\t1: List raw data\r\n\t2: List converted data\r\n"
			"\t3: List Filtered data\r\n"
			"\t4: Similar to 3 including Min/MAX\r\n"
			"\t5: Reset MIN/MAX\r\n"
			);
		return;
	}
	if (param[1] == 1) {
		pHlth = CAllProcess::g_pAllProcess->GetHlthProcess();
		pHlth->PrintAdc(m_pUserComm);
		return;
	}
	if (param[1] == 2) {
		pAdc->PrintAdc(m_pUserComm);
		return;
	}
	if (param[1] == 3) {
		pAdc->PrintAdc(m_pUserComm, 1);
		return;		
	}
	if (param[1] == 4) {
		pAdc->PrintAdc(m_pUserComm, 2);
		return;		
	}
	if (param[1] == 5) {
		pAdc->ResetMinMax();
		SendStr("Adc Min/Max Reset\r\n");
		return;
	}
	SendStr("Invalid Option\r\n");
#endif
}

void CDebugDL::CmdTest(unsigned long *param)
{
	SendStr("Invalid Test Option\r\n");
}

void CDebugDL::CmdZModemRecv(unsigned long *param)
{
	int ret;

	ret = m_pStore->OpenStorage((uint32)this);
	if (ret != 1) {
		m_pUserComm->ForceSend("Open Code storage failed\r\n");
		return;
	}
	m_pStore->Rewind();


	m_pUserComm->Open();
	ret = m_pZModem->Start(m_pUserComm, m_pStore);
	m_pUserComm->ResetStopError();

	m_pUserComm->Close();
	m_pStore->CloseStorage((uint32)this);
	m_pStore->EndOfDownload();

	if (ret <= 0) {
		m_pUserComm->Send("ZModem failed\r\n");	
	}

}

void CDebugDL::CmdYModemRecv(unsigned long *param)
{          
	char buff[100];
	long baudrate = m_startingBaudrate; // Default
	int ret;
	const char *ptr;


	if (*param == 1) {
		baudrate = param[1];
	}
	sprintf(buff, "Set baudrate to %ld\r\nKey in Ctrl-X(0x18) to abort it\r\n", baudrate);
	m_pUserComm->ForceSend(buff); 
	ret = m_pStore->OpenStorage((uint32)this);
	if (ret != 1) {
		m_pUserComm->ForceSend("Open Code storage failed\r\n");
		return;
	}
	m_pUserComm->SetBaudRate(baudrate);
	m_pStore->Rewind();

	m_pUserComm->Open();
	ret = m_pYModem->Start(m_pUserComm, m_pStore);
	if (ret == 1) {
		ret = m_pYModem->Start(m_pUserComm, NULL);
		ret = (ret != 2) ? (-1) : (1);
	} else {
		ret = -1;
	}
	m_pUserComm->Close();
	
	m_pStore->CloseStorage((uint32)this);

	m_pUserComm->WaitTxDone(1000);
	m_pUserComm->SetBaudRate(m_startingBaudrate);
	ptr = (ret == 1) ? "\r\nYmodem file receiving OK" : "\r\nYModem file receiving failed/abort";
	if (ret == 1) {
		// Further check if it'd legitimate .DLD file.
		ret = m_pLoader->CheckDLDBlock(MAIN_CODE_START);
		if (ret != 1) {
			ptr = "Invalid .DLD file received\r\n";
		} else {
			m_pStore->EndOfDownload();
		}
	}
	m_pUserComm->Send(ptr);
}


void CDebugDL::CmdProc(unsigned long *param)
{       
	char buff[100];
	CIdleProcess *pIdle = CIdleProcess::s_pIdleProcess; //CAllProcess::g_pAllProcess->GetIdleProcess();
	CBaseProcess *ptr;
	uint16 stkAvail, stkTotal;
	uint16 *stkAddress;
	int i;

	if (*param == 0) {
		// List help info
		char const *ptr = "1: List idle time per second\r\n\
2: List resource usage\r\n3: List processes\r\n4: Similar to 3. Also clear process counter";
		SendStr(ptr);
		return;
	}
	if ((*param == 1) && (param[1] == 1)) {
		CIdleProcess::s_pIdleProcess->PrintIdle(m_pUserComm);
		return;
	}

	if ((*param == 1) && (param[1] == 2)) {
		// List resource usage
		uint8 used;
		uint8 total;
		used = GetResourceUsed();
		total = C_MAX_RESOURCES;
		sprintf(buff, "\r\nResource used %d, total=%d, %s\r\n",
			used, total, (used <= total) ? ("OK") : ("Exceeded!!!"));
		SendStr(buff);
		// check semaphore
		used = GetSemaUsed();
		total = C_MAX_SEMAPHORES;
		sprintf(buff, "Semaphore used %d, total=%d, %s\r\n",
			used, total, (used <= total) ? ("OK") : ("Exceeded!!!"));
		SendStr(buff);
		// check Mail box
		used = GetMailBoxUsed();
		total = C_MAX_MAILBOXES;
		sprintf(buff, "Mailbox used %d, total=%d, %s\r\n", used, total, (used <= total) ? ("OK"):("Exceeded!!!"));
		SendStr(buff);
		
		CQueue::PrintStatus(m_pUserComm);
		return;
	}

	if ((*param == 1) && ((param[1] == 3) || (param[1] == 4))) {
		// List processes
		CBaseProcess::PrintStatus(param, m_pUserComm);
		return;
	}
	SendStr("Invalid Option\r\n");
}
              
extern CSci *g_pSci0;
extern CSci *g_pSci1;

void CDebugDL::CmdBaud(unsigned long *param)
{                 
	char buff[100];
	long baudrate = m_startingBaudrate;
	if (*param == 1) {
		baudrate = param[1];
		switch (baudrate) {
			case 9600:
			case 19200:
			case 38400:
			case 57600:
				m_bAutoRateAllowed = 0;
				break;
		}
	}    
	switch (baudrate) {
	case 0:
		m_bAutoRateAllowed = 0;
		baudrate = m_startingBaudrate;
		break;
	case 1:
		m_bAutoRateAllowed = 1;
		baudrate = m_startingBaudrate;
		break;
	}
	sprintf(buff, "Baudrate will be/is %ld\r\n", baudrate);	
	m_pUserComm->ForceSend(buff);
	m_pUserComm->ForceSend((m_bAutoRateAllowed) ? 
		("Auto baudrate enabled\r\n"):("Auto baudrate disabled\r\n"));
	m_pUserComm->SetBaudRate(baudrate);           
	m_startingBaudrate = baudrate;
	
	CAllProcess::g_pAllProcess->SaveBaudrate();

	if (*param == 0) {
		SendStr("Syntax for BAUD:\r\n\t<baud>: Set baudrate to <baud> (9600, 19200, 38400 or 57600) &\r\n\t\tdisable automatic baudrate detection\r\n\
\t0: Disable automatic baudrate detection\r\n\
\t1: Enable automatic baudrate detection\r\n\
");
		if (g_pSci0) {
			sprintf(buff, "SCI0 baudrate:%ld\r\n", g_pSci0->GetBaudRate());
			SendStr(buff);
		}
		if (g_pSci1) {
			sprintf(buff, "SCI1 baudrate:%ld\r\n", g_pSci1->GetBaudRate());
			SendStr(buff);
		}
	}
}

void CDebugDL::CmdReadSerial(unsigned long *param)
{   
#if COMPLETE
	CFactory *pFactory;
	char buff[50];
	pFactory = CAllProcess::g_pAllProcess->GetFactorySetting();
	
	SendStr("OSC serial No:\t");
	SendStr(pFactory->GetOscSerial());
	
	SendStr("\r\nCard serial No:\t");
	SendStr(pFactory->GetCardSerial());
	
	SendStr("\r\nCLEI code:\t");
	SendStr(pFactory->GetCleiCode());

	SendStr("\r\nOSC type:\t");
	sprintf(buff, "%ld", pFactory->GetOscType());
	SendStr(buff);

	SendStr("\r\nHardware Rev:\t");
	SendStr(pFactory->GetHardwareRev());

	SendStr("\r\nECI code:\t");
	SendStr(pFactory->GetECI());

	// Insert new report here

	SendStr("\r\n");
#endif	
}

void CDebugDL::CmdManf(unsigned long *param)
{
	int option = 10000;
	int item = 10000;
	int syntaxError = 0;
	int quoted = 0;
	char *ptr = NULL;
	char buff[100];
	CFactory *pFactory;
	// CIstateProcess *pIstateProcess;
	pFactory = CFactory::s_pFactory;
	// pIstateProcess = CAllProcess::g_pAllProcess->GetIstateProcess();

	if (*param >= 1) {
		option = param[1];
		if ((option == 0) && (*param != 1L)) {
			syntaxError = 1;
		}
		if ((option == 1) && (*param != 2L)) {
			syntaxError = 1;
		}
		if ((option == 2) && (*param != 3L)) {
			syntaxError = 1;
		}
		if ((option == 10) && (*param == 1L)) {
			option = 0;
			quoted = 1;
		}
		if ((option == 11) && (*param == 2L)) {
			option = 1;
			quoted = 1;
		}
	}
	if (*param >= 2) {
		item = param[2];
	}
	if (*param == 3) {
		ptr = GetParamStr(param[3]);
	}

	if (option >= 3) {
		syntaxError = 1;
	}

	if (option <= 1) { // read manufacture info
		int cnt = 0;
		if ((item == 1) || (option == 0)) {
			// read item 1: OSC serial number
			SendStr("1. OSC serial No:\t");
			if (quoted) SendStr("\"");
			SendStr(pFactory->GetOscSerial());
			if (quoted) SendStr("\"");
			SendStr("\r\n");
			cnt++;
		}
		if ((item == 2) || (option == 0)) {
			// read item 2: Card serial number
			SendStr("2. Card serial No:\t");
			if (quoted) SendStr("\"");
			SendStr(pFactory->GetCardSerial());
			if (quoted) SendStr("\"");
			SendStr("\r\n");
			cnt++;
		}
		if ((item == 3) || (option == 0)) {
			// read item 3: CLEI code
			SendStr("3. CLEI code:\t\t");
			if (quoted) SendStr("\"");
			SendStr(pFactory->GetCleiCode());
			if (quoted) SendStr("\"");
			SendStr("\r\n");
			cnt++;
		}
#if 0
		if ((item == 4) || (option == 0)) {
			// read item 4: OSC type
			SendStr("4. OSC type:\t\t");
			if (quoted) SendStr("\"");
			sprintf(buff, "%ld", pFactory->GetOscType());
			SendStr(buff);
			if (quoted) {
				SendStr("\"\r\n");
			} else {
				SendStr("\r\n");
			}
			if (option == 0) {
				sprintf(buff, "\tHint:\t%d: 023-630001-02 single oven OSC\r\n"
					"\t\t%d: MTI260 double oven OSC\r\n"
					"\t\t%d: X72 rubidium OSC\r\n",
					OSC_PART_NUMBER_023_63001_02,
					OSC_PART_MTI260, 
					OSC_PART_RUBIDIUM_X72);
				SendStr(buff);
			}
			cnt++;
		}
#endif
		if ((item == 5) || (option == 0)) {
			// read item 5: hardware rev
			SendStr("5. Hardware Rev:\t");
			if (quoted) SendStr("\"");
			SendStr(pFactory->GetHardwareRev());
			if (quoted) SendStr("\"");
			if (option == 0) {
				SendStr("\r\n\tHint: single alphabet such as A B etc.\r\n");
			}
			cnt++;
		}
#if 0
		if ((item == 6) || (option == 0)) {
			// read item 6: ECI code
			SendStr("6. ECI code:\t\t");
			if (quoted) SendStr("\"");
			SendStr(pFactory->GetECI());
			if (quoted) SendStr("\"");
			SendStr("\r\n");
			cnt++;
		} 
#endif

#if 0
		if ((item == 8) || (option == 0)) {
			if (quoted) {
				// IOCHW_IOCFW_IOCHW. old way
				sprintf(buff, "8. IOCFW_IOCHW(local):\t\t\t\"%ld\"\r\n", pFactory->GetForLocalMask());
			} else {
				sprintf(buff, "8. IOCFW_IOCHW(local):\t\t\t%ld\r\n", pFactory->GetForLocalMask());
			}
			SendStr(buff);
			cnt++;
		}
#endif

#if 0 
		if ((item == 9) || (option == 0)) {
			if (quoted) {
				// IOCHW_IMCHW_IOCHW. old way
				sprintf(buff, "9. IMCHW_IOCHW(for PPC):\t\t\"%ld\"\r\n", pFactory->GetForImcMask());
			} else {
				sprintf(buff, "9. IMCHW_IOCHW(for PPC):\t\t%ld\r\n", pFactory->GetForImcMask());
			}
			SendStr(buff);
			cnt++;
		}
#endif

#if 0
		if ((item == 10) || (option == 0)) {
			if (quoted) {
				// IOCHW_PIOCHW_IOCHW. old way
				sprintf(buff, "10.PIOCHW_IOCHW(for Twin):\t\t\"%ld\"\r\n", pFactory->GetForTwinMask());
			} else {
				sprintf(buff, "10.PIOCHW_IOCHW(for Twin):\t\t%ld\r\n", pFactory->GetForTwinMask());
			}
			SendStr(buff);
			cnt++;
		}
#endif
		if (cnt <= 0) {
			syntaxError = 1;
		}
	} else if (option == 2) {

		// All these need number
		switch (item) {
#if 0
		case 8: // local mask
			pFactory->SetForLocalMask(param[3]);
			item = 0;
			break;
#endif
#if 0
		case 9:
			
			pFactory->SetForImcMask(param[3]);
			item = 0;
			break;
#endif
#if 0 
		case 10:
			pFactory->SetForTwinMask(param[3]);
			item = 0;
			break;
#endif
		case 4: // OSC type
#if 0
			switch (param[3]) {
			case OSC_PART_NUMBER_023_63001_02:
				pIstateProcess->WriteOneIOCIstateItem(COMM_RTE_KEY_OSC_TYPE, 0, 0, param[3]);
				break;
			case OSC_PART_MTI260:
				pIstateProcess->WriteOneIOCIstateItem(COMM_RTE_KEY_OSC_TYPE, 0, 0, param[3]);
				break;
			case OSC_PART_RUBIDIUM_X72:
				pIstateProcess->WriteOneIOCIstateItem(COMM_RTE_KEY_OSC_TYPE, 0, 0, param[3]);
				break;
			default:
				break;
			}
			item = 0;  
#endif			
			break;
		case 7:
#if 0		
			if (param[3] == 0) {
				pFactory->SetJIOSwitch(0);
			} else if (param[3] == 1) {
				pFactory->SetJIOSwitch(1);
			} else {
				syntaxError = 1;
			}
			item = 0;
#endif			
			break;
		}
		if ((ptr == NULL) && (item != 0)) {
			item = 10000;
		}

		// All these need string
		switch (item) {
		case 1: // OSC serial number
			pFactory->SetOscSerial(ptr);
			break;
		case 2: // Card Serial number
			pFactory->SetCardSerial(ptr);
			break;
		case 3: // CLEI code
			pFactory->SetCleiCode(ptr);
			break;
		case 5: // hardware rev
			//if ((ptr[1] != 0) || (*ptr < 'A') || (*ptr > 'B')) {
			//	SendStr("Invalid hardware revision\r\n");
			//	syntaxError = 1;
			// } else {
				pFactory->SetHardwareRev(ptr);
			//}
			break;
#if 0
		case 6: // ECI code
			pFactory->SetECI(ptr);
			break;
#endif
		case 0: 
			break;
		default:
			syntaxError = 1;
			break;
		}
	} 

	if (syntaxError) {
		SendStr("Input parameter error\r\n");
	}
	if ( (*param == 0) || (syntaxError) ) {
		const char *ptr = "Syntax: <option> [<item>] [content]\r\n" 
			"\toption\t0: List all manufacture configure with help info\r\n"
			"\t\t1: Read specific item without help info\r\n"
			"\t\t2: Write specific item\r\n"
			"\t\t10: List configure as option 0 and with \" around the parameter\r\n"
			"\t\t11: Similar to option 1 but with \" around the parameter\r\n"
			"\titem\t1: OSC serial number\r\n"
			"\t\t2: Card Serial number\r\n"
			"\t\t3: CLEI code\r\n"
			//"\t\t4: OSC type\r\n"
			"\t\t5: Hardware revision\r\n"
			//"\t\t6: ECI code\r\n"
			//"\t\t7: JIO enabled\r\n"
			;
		SendStr(ptr);	
		ptr = //"\t\t8: IOCFW_IOCHW\r\n"
			//"\t\t9: IMCHW_IOCHW\r\n"
			//"\t\t10: PIOCHW_IOCHW\r\n"
			"\tcontent: If content is string type, please start & end string with \"."
			;
		SendStr(ptr);
	}  

}

void CDebugDL::CmdWriteSerial(unsigned long *param)
{     
#if COMPLETE
	int ret;
	char buff[100];
	CFactory *pFactory;
	pFactory = CAllProcess::g_pAllProcess->GetFactorySetting();
	// List current value
	CmdReadSerial(param);

	// Set OSC serial no
	SendStr("Enter new OSC serial No: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		pFactory->SetOscSerial(buff);		
	}
	
	// Card serial no
	SendStr("Enter new card serial No: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		pFactory->SetCardSerial(buff);
	}
	
	// Clei code
	SendStr("Enter CLEI code: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		pFactory->SetCleiCode(buff);
	}

	// Set osc type
	SendStr("Enter OSC type: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		int32 type;
		ret = sscanf(buff, "%ld", &type);
		if (ret == 1) {
			// Check legal types
			pFactory->SetOscType(type);
		}
	}

	// Set Hardware rev
	SendStr("Enter Hardware rev: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		pFactory->SetHardwareRev(buff);
	}

	// Set ECI
	SendStr("Enter ECI code: ");
	ret = ReadStr(buff, MAX_SERIAL_LENGTH, 1);
	if (ret > 0) {
		pFactory->SetECI(buff);
	} 
#endif	
}

void CDebugDL::CmdSys(unsigned long *param)
{  
	const char *pHelp1, *pHelp2;
	CPTPLink *pLink;
	pHelp1 = "SYS <option>:\r\n"
		"\t2 1: List PPC-RTE link status(recv)\r\n"
		"\t2 2: List PPC-RTE link status(send)\r\n"
		"\t2 3: List PPC-RTE link register\r\n"
		"\t2 4: PPC-RTE Statistic\r\n"
		"\t2 5: PPC-RTE PTP Extra Help\r\n"
		"\t3 1: List RTE-RTE link status(recv)\r\n"
		"\t3 2: List RTE-RTE link status(send)\r\n"
		"\t3 3: List RTE-RTE link register\r\n"
		"\t3 4: RTE-RTE statistic\r\n"
		"\t3 5: RTE-RTE Extra help\r\n"
		"\t6: Copy code to twin\r\n"
		"\t20: Print SCI0 info\r\n"
		"\t21: Print SCI1 info\r\n"
		"\t22: Print SCI3 info\r\n"
		"\t23: Print SCI4 info\r\n";
	pHelp2 = 
		"\t100: Kill self with a dead loop in this process\r\n" 
		"\t101: Kill self now\r\n"
		;

	if (*param == 2) {
		switch (param[1]) {
		case 2:
			pLink = CAllProcess::g_pAllProcess->GetImcLink();
			pLink->PrintPtpStatus(m_pUserComm, (int)param[2]);
			return;
		case 3:
			pLink = CAllProcess::g_pAllProcess->GetIocLink();
			pLink->PrintPtpStatus(m_pUserComm, (int)param[2]);
			return;
		}
	}

	if (*param == 1) {
		switch (param[1]) {
#if 0
		case 2: // PPC link
			pLink = CAllProcess::g_pAllProcess->GetImcLink();
			pLink->PrintStatus(m_pUserComm);
			return;
		case 3: // RTE link
			pLink = CAllProcess::g_pAllProcess->GetIocLink();
			pLink->PrintStatus(m_pUserComm);
			return;
#endif
		case 6: // copy code to twin
			CmdSetTime(param);
			CPTPDnldProcess::s_pPTPDnldProcess->StartSendCodeToTwinIOC();
			return;
		case 20:
			if (g_pSci0) {
				g_pSci0->PrintStatus(m_pUserComm);
			} else {
				SendStr("SCI0 disabled\r\n");
			}
			return;
		case 21: 
			if (g_pSci1) {
				g_pSci1->PrintStatus(m_pUserComm);
			} else {
				SendStr("SCI1 disabled\r\n");
			}
			return;
		case 22:
			if (g_pSci3) {
				g_pSci3->PrintStatus(m_pUserComm);
			} else {
				SendStr("SCI3 disabled\r\n");
			}
			return;
		case 23:
			if (g_pSci4) {
				g_pSci4->PrintStatus(m_pUserComm);
			} else {
				SendStr("SCI4 disabled\r\n");
			}
			return;    
		case 100:
			SendStr("Commit suicide with a dead loop in this process\r\nBye!\r\n");
			while(1);
			return;
		case 101:
			SendStr("Commit suicide now\r\n");
			CAllProcess::g_pAllProcess->RebootSystem(2, m_port);
			return;
		}
	}

	if (*param != 0) {
		SendStr("Invalid option\r\n");
	}
	SendStr(pHelp1);
	SendStr(pHelp2);
	
#if COMPLETE
	char buff[200];
	CFPGA *pFPGA;

	pFPGA = CAllProcess::g_pAllProcess->GetFPGA();

	if (*param == 0) {
		char const *ptr;
		ptr = // "SYS 1: List NVRAM writing time\r\n // It's obsolete
"2: About PPC-RTE link\r\n3: About RTE-RTE link\r\n4: Get Card status\r\n\
5: List Led status\r\n\
6: Copy Main code to Twin RTE\r\n\
7: RTE position\r\n\
8 <chan>: Clear absolute phase <chan>=0 .. 4 for GPS, PRS, SPAN1, SPAN2, OSC. 10: For all \r\n\
9 : List FPGA status\r\n\
10 0: Force it standby  \t10 1: Force it Active\r\n";


		SendStr(ptr);
		ptr = "10 2: Set twin life to 1 hour\r\n"
"11 0: Turn Off GPS power\t11 1: Turn On GPS power\r\n11 2: Query state\r\n\
12 0: Turn Off Module power\t12 1: Turn On Module power\r\n12 2: Query state\r\n"
"13 1: Set CCK phase align\t 13 2: Set CCK reversed align(obsolete)\r\n"
"13 3: Clear CCK phase align\r\n"
"14 : 1PPS phase in ISR counter\r\n"
"15 : Print GPS current\r\n";
		SendStr(ptr);
		ptr = "16 : Align 25ms ISR\r\n";
		SendStr(ptr);
		return;
	}

	if ((*param == 1) && (param[1] == 16)) {
		CFPGA *pFPGA;

		pFPGA = CAllProcess::g_pAllProcess->GetFPGA();
		pFPGA->SyncUpIsr();
		return;
	}
	if ((*param == 1) && (param[1] == 15)) {
		CCardProcess *pCard;
		pCard = CAllProcess::g_pAllProcess->GetCardProcess();
		pCard->PrintStatus(m_pUserComm, 1005);
		return;
	}

	if ((*param == 1) && (param[1] == 14)) {
		int8 pos;
		pos = pFPGA->Get1PPSPos();
		sprintf(buff, "1PPS pos is @ %d\r\n", pos);
		SendStr(buff);
		return;
	}

	if ((*param == 1) && (param[1] == 9)) {
		pFPGA->PrintStatus(m_pUserComm);
		return;
	}

	if ((*param == 2) && (param[1] == 13)) {
		switch (param[2]) {
		case 1:
			pFPGA->SetCCKAlign(1);
			return;
		case 2:
			// pFPGA->SetCCKAlign(-1); // 8/16/2004: obsolete
			return;
		case 3:
			pFPGA->SetCCKAlign(0);
			return;
		}
	}

	if ((*param == 2) && (param[1] == 11)) {
		const char *ptr = "Invalid choice\r\n";

		if (param[2] == 1L) {
			pFPGA->TurnAntennaPower(1);
			ptr = "GPS power turned on\r\n";
		} else if (param[2] == 0L) {
			pFPGA->TurnAntennaPower(0);
			ptr = "GPS power turned off\r\n";
		} else if (param[2] == 2L) {
			ptr = NULL;
		}

		if (ptr == NULL) {
			// crude way of getting info.
			uint8 *pData = (uint8 *)0x400000;
			if (*pData & 0x20) {
				ptr = "GPS power is ON\r\n";
			} else {
				ptr = "GPS power is OFF\r\n";
			}
		}
		SendStr(ptr);
		return;
	}

	if ((*param == 2) && (param[1] == 12)) {
		const char *ptr = "Invalid choice\r\n";

		if (param[2] == 1L) {
			pFPGA->TurnModulePower(1);
			ptr = "Module power turned on\r\n";
		} else if (param[2] == 0L) {
			pFPGA->TurnModulePower(0);
			ptr = "Module power turned off\r\n";
		} else if (param[2] == 2L) {
			ptr = NULL;
		}

		if (ptr == NULL) {
			// crude way of getting info.
			uint8 *pData = (uint8 *)0x400000;
			if (*pData & 0x80) {
				ptr = "Module power is ON\r\n";
			} else {
				ptr = "Module power is OFF\r\n";
			}
		}
		SendStr(ptr);
		return;
	}

#if 0
	// Obsolete. Use NVRAM
	if ((*param == 1) && (param[1] == 1)) {
		// About NVRAM writing
		CRTC rtc;
		uint32 time;
		int year, month, day, hour, minute, second;
		uint32 cnt;
		CNVRAM *pNVRAM = CAllProcess::g_pAllProcess->GetNVRAM();

		time = pNVRAM->GetLastUpdateTimeStamp();
		cnt = pNVRAM->GetWriteCnt();
		if (time != 0) {
			rtc.SetGPSSeconds(time);
			rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);
			sprintf(buff, "NVRAM updating[%ld]: Last NVRAM written @ %04d-%02d-%02d %02d-%02d-%02d\r\n",
				cnt,
				year, month, day, hour, minute, second);
			SendStr(buff);
		} else {
			SendStr("NVRAM updating: No NVRAM updating so far\r\n");
		}
		return;
	}
#endif

	if ((*param == 2) && (param[1] == 8)) {
		// Clear absolute phase
		CSecondProcess *pProcess; 
		CInputPhase *pPhase;     
		pProcess = CAllProcess::g_pAllProcess->GetSecondProcess();
		pPhase = pProcess->GetInputPhase();
		pPhase->ClearAbsolutePhase(param[2], m_pUserComm);
		return;
	}
	if ((*param == 1) && (param[1] == 7)) {
		sprintf(buff, "Twin RTE %s\r\n", (pFPGA->ReadTwinExist()) ? ("Exist") : ("GONE"));
		SendStr(buff);
		return;
	}

	if ((*param == 1) && (param[1] == 6)) {
		CPTPDnldProcess *pDnld = CAllProcess::g_pAllProcess->GetPTPDnldProcess();
		CmdSetTime(param);
		pDnld->StartSendCodeToTwinIOC();
		return;
	}

	if ((*param == 1) && (param[1] == 2)) {
		// About Link PPC -- RTE
		CPTPLink *pLink = CAllProcess::g_pAllProcess->GetIMCLink();
		pLink->PrintStatus(m_pUserComm);
		return;
	}

	if ((*param == 1) && (param[1] == 3)) {
		// About Link RTE -- RTE
		CPTPLink *pLink = CAllProcess::g_pAllProcess->GetIOCLink();
		pLink->PrintStatus(m_pUserComm);
		sprintf(buff, "Twin RTE %s\r\n", ((pFPGA->ReadTwinExist()) ? ("Exist") : ("Gone")));
		SendStr(buff);
		return;
	}

	if ((*param == 1) && (param[1] == 4)) {
		CCardState *pState = CAllProcess::g_pAllProcess->GetCardState();
		pState->PrintStatus(m_pUserComm);
		return;
	}
	
	if ((*param == 1) && (param[1] == 5)) {
		CLed *pLed = CAllProcess::g_pAllProcess->GetLed();
		pLed->PrintStatus(m_pUserComm);
		return;
	}

	if ((*param == 2) && (param[1] == 10)) {
		CCardState *pState = CAllProcess::g_pAllProcess->GetCardState();
		switch (param[2]) {
		case 0:
			pState->ForceStandby();
			SendStr("Force it to standby\r\n");
			return;
		case 1:
			pState->ForceActive();
			SendStr("Force it to active. Cannot work between incompatible RTE\r\n");
			return;
		case 2:
			CAllProcess::g_pAllProcess->SetTwinLife(60*60);
			SendStr("Force twin life to 1 hour\r\n");
			return;
		default: 
			break;
		}		
	}

	SendStr("Invalid option for SYSTEM\r\n");
#endif
}

void CDebugDL::CmdCondition(unsigned long *param)
{ 
	CEventProcess::s_pEventProcess->ConditionHack(param, m_pUserComm); 
}

void CDebugDL::CmdEvent(unsigned long *param)
{                          

	CEventProcess::s_pEventProcess->EventHack(param, m_pUserComm);

#if COMPLETE
	int ret;
	char buff[100];
	int pairs = 0;

	CEventProcess *pEvtProcess;
	if (*param == 0) {
		const char *ptr;
		ptr = "EVT 1 <id> <active> <aid1> <aid2>\r\n\tGenerate event\r\n\
EVT 2 <id> <aid1> <aid2>\r\n\tGenerate 10 pairs of ON/OFF event\r\n\
EVT 3 <id> <aid1> <aid2>\r\n\tGenerate 100 pairs of ON/OFF event\r\n\
EVT 4 <id> <aid1> <aid2>\r\n\tGenerate 1000 pairs of ON/OFF event\r\n\
EVT 5 <id> <active> <aid1> <aid2> <alarm>\r\n\tGenerate event with alarm level specified\r\n\
";
		SendStr(ptr);
		return;
	}
	pEvtProcess = CAllProcess::g_pAllProcess->GetEventProcess();

	if ((*param == 5) && (param[1] == 1)) {
		ret = pEvtProcess->ReportEvent(param[2], param[3], param[4], param[5]);
		sprintf(buff, "Ret=%d\r\n", ret);
		SendStr(buff);
		return;
	}
	

#endif
}

void CDebugDL::CmdDebug(unsigned long *param)
{
#if COMPLETE
	char buff[100];
	if (*param != 2) {
		char const *ptr = 
"1 <option>: New log debug option\r\n\tBit0=1: Report all new log\r\n\
\tBit1=1: Report new active log\r\n\tBit2=1: Report inactive new log\r\n";
		SendStr(ptr);
		return;
	}
	if (param[1] == 1) {
		CEventProcess *pEvtProcess;
		pEvtProcess = CAllProcess::g_pAllProcess->GetEventProcess();
		pEvtProcess->m_debugOption = param[2];
		sprintf(buff, "New log option set to 0x%lX\r\n", param[2]);
		SendStr(buff);
		return;
	}
	SendStr("Invalid option\r\n");
#endif
}

void CDebugDL::CmdEventLog(unsigned long *param)
{
	CEventProcess::s_pEventProcess->LogHack(param, m_pUserComm);

#if COMPLETE


	if (*param != 2) {
		char const *ptr = 
"1 <cnt>: Retrieve most recent <cnt> of event log, Pause for every 10\r\n\
2 <cnt>: Same as option 1 except no pause\r\n\
3 <logEnable>: Enable(1)/Disable(0) writing to event log\r\n\
";
		SendStr(ptr);
		sprintf(buff, "Event log is %s\r\n", 
			(bEnableLog) ? ("Enabled") : ("Disabled"));
		SendStr(buff);
		return;
	}



#endif	
}

void CDebugDL::PauseTick(int ticks)
{
	K_Task_Wait(ticks);
}

void CDebugDL::CmdIstate(unsigned long *param)
{
	CIstateProcess::s_pIstateProcess->IstateHack(param, m_pUserComm);
}

void CDebugDL::CmdNVRAM(unsigned long *param)
{     
	CNVRAM *pNVRAM;
	int ret;

	pNVRAM = CAllProcess::g_pAllProcess->GetNvram();
	if (*param == 0) {
		SendStr("Options:\r\n"
			"\t1: Write NVRAM now\r\n"
			"\t2: Print NVRAM status\r\n"
			);
		return;
	}
	if (*param == 1) {
		switch (param[1]) {
		case 1: 
			SendStr("Saving NVRAM ...");
			ret = pNVRAM->SaveNVRAMCopy();
			SendStr((ret == 1) ? ("Done!\r\n") : ("Skipped!\r\n"));
			return;
		case 2:
			pNVRAM->PrintStatus(m_pUserComm);
			return;
		}
	}
	SendStr("Invalid option\r\n");
	return;
#if COMPLETE
	int ret;
	if (*param == 0) {
		// Print help info
		const char *ptr = "Options:\r\n\t1: Write NVRAM now\r\n\
\t2: Print NVRAM status\r\n\
";
		SendStr(ptr);
		return;
	};
	
	CNVRAM *pNVRAM;
	pNVRAM = CAllProcess::g_pAllProcess->GetNVRAM();
	if ((*param == 1) && (param[1] == 1)) {
		// Save NVRAM now
		SendStr("Saving NVRAM ...");
		ret = pNVRAM->SaveNVRAMCopy();
		SendStr((ret == 1) ? ("Done!\r\n") : ("Skipped!\r\n"));
		return;
	}
	if ((*param == 1) && (param[1] == 2)) {
		// Print status of NVRAM
		pNVRAM->PrintStatus(m_pUserComm);
		return;
	}
	SendStr("Invalid option\r\n");
	return; 
#endif	
}

extern int s_istateInterfaceObjects;

void CDebugDL::CmdWHOAMI(unsigned long *param)
{
	char buff[100];
	const char *pStat;

	switch (IS_ACTIVE) {
	case COLD_ACTIVE:
		pStat = "COLD-ACTIVE";
		break;
	case HOT_ACTIVE:
		pStat = "ACTIVE";
		break;
	default:
		pStat = "NON-ACTIVE";
		break;
	}
	sprintf(buff, "RTE%d is %s\r\n"
		,CAllProcess::GetSlotId() + 1 
		,pStat
	);
	SendStr(buff);
	sprintf(buff, "Hardware Revision: %X\r\n"
		, CAllProcess::GetHardwareRevision() );
	SendStr(buff);
	
	sprintf(buff, "FPGA load is %s\r\n",
		(CAllProcess::GetFpgaDone()) ? ("DONE") : ("UNLOAD"));
	SendStr(buff);

	sprintf(buff, "Istate Interface created: %d\r\n", s_istateInterfaceObjects);
	SendStr(buff);

	sprintf(buff, "OSC type is: %s\r\n", 
		(CFpga::s_pFpga->IsRubidium() == 1) ? ("Rub") : ("QUARTZ"));
	SendStr(buff);

#if COMPLETE
	CFPGA *pFPGA;
	char buff[100];
	int id = 0; // For now just don't know which RTE.
	uint8 sw;

	pFPGA = CAllProcess::g_pAllProcess->GetFPGA();
	id = pFPGA->ReadSlotId();
	sprintf(buff, "RTE%d on %s port\r\nThe other RTE is %s\r\nExpansion Panel is %s\r\n", id, m_name,
		pFPGA->ReadTwinExist() ? ("PRESENT") : ("ABSENT"),
		pFPGA->ReadExpansionExist() ? ("PRESENT") : ("ABSENT") );
	SendStr(buff);
	sw = P7.PORT.BYTE;
	sprintf(buff, "SW2-1: %s SW2-2: %s SW2-3: %s\r\n", 
		(sw & 8) ? ("OFF") : ("ON "),
		(sw & 4) ? ("OFF") : ("ON "),
		(sw & 2) ? ("OFF") : ("ON "));
	SendStr(buff);
	sprintf(buff, "Twin in for %ld seconds\r\n", CAllProcess::g_pAllProcess->GetTwinLife());
	SendStr(buff);
	sprintf(buff, "Master: %d\r\n", CAllProcess::g_pAllProcess->IsMaster());
	SendStr(buff);
	{
		uint32 life;
		double day;
		life = g_pRTC->GetAbsoluteSecond();
		day = (double)life / 86400.0;
		sprintf(buff, "Running for %ld (%.4f Days) since last reboot\r\n", life, day);
		SendStr(buff);
	}
#endif	
}

void CDebugDL::CmdSwitchActive(unsigned long *param)
{
#if DTI_ISTATE 
	if (*param == 1) {
		switch (param[1]) {
		case 0:
			SET_MY_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_FORCE_STANDBY);
			return;
		case 1:
			SET_TWIN_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, CIstateDti::SINGLE_ACTION_FORCE_STANDBY);
			return;
		}
	}

	SendStr("Syntax <option>\r\n"
		"\t0: Set this RTE standby\r\n"
		"\t1: Set this RTE active\r\n"
		);    
#endif		
}

void CDebugDL::CmdInput(unsigned long *param)
{  
	CInput::AllInputHack(param, m_port);	
}

void CDebugDL::CmdICW(unsigned long *param)
{     
#if COMPLETE
	CFPGA *pFPGA;

	if (*param == 0) {
		const char *ptr;
		ptr = "ICW options:\r\n\t1 <icw>: Impose ICW to FPGA\r\n";
		SendStr(ptr);
		return;
	}

	pFPGA = CAllProcess::g_pAllProcess->GetFPGA();
	if ((*param == 2) && (param[1] = 1)) {
		pFPGA->ImposeICW(param[2]);
		pFPGA->EnableSoftHoldover(1);
		SendStr("Impose ICW to FPGA\r\n");
		return;
	}
	if (param[1] == 2) {
		pFPGA->ImposeICW(0x7fffffff);
		pFPGA->EnableSoftHoldover(0);
		SendStr("Clear imposed ICW\r\n");
		return;
	}

	SendStr("Invalid option\r\n");
#endif	
}

void CDebugDL::CmdDebugMask(unsigned long *param)
{
	CDebugMask *pMask;

	pMask = CAllProcess::g_pAllProcess->GetDebugMask();
	pMask->DebugHack(m_pUserComm, param);
}

void CDebugDL::CmdReadWave(unsigned long *param)
{     
#if COMPLETE
	int8 data[128];
	CFPGA *pFPGA;
	int len = 0;
	int i;
	char buff[80];

	pFPGA = CAllProcess::g_pAllProcess->GetFPGA();

	if (*param == 0) {
		const char *ptr;
		ptr = "Syntax: RWAVE <option>\r\n\
\t 0: Read first 32 bytes\r\n\
\t 1: Read second 32 bytes\r\n\
\t 2: Read third 32 bytes\r\n\
\t 3: Read fourth 32 bytes\r\n\
\t10: Read all wave form\r\n";
		SendStr(ptr);
		return;
	}
	if (*param == 1) {
		switch (param[1]) {
		case 0:
		case 1:
		case 2:
		case 3:
			pFPGA->ReadWaveForm(COutput::m_addressList[param[1]],
				data, 32);
			len = 32;
			sprintf(buff, "Waveform Output %d:\r\n", (int)param[1]);
			SendStr(buff);
			break;
		case 10:
			pFPGA->ReadWaveForm(COutput::m_addressList[param[1]],
				data, 128);
			len = 128;
			SendStr("All Waveform:\r\n");
			break;
		default:
			break;
		}
	}
	if (len > 0) {
		for (i = 0; i < len; i++) {
			sprintf(buff, "%3d", data[i]);
			SendStr(buff);
			if ((i % 8) == 7) {
				SendStr("\r\n");
			} else {
				SendStr(", ");
			}
		}
		return;
	}

	SendStr("Invalid option\r\n");
#endif	
}

void CDebugDL::CmdDownload(unsigned long *param)
{     
	CPTPDnldProcess::DnldHack(param, m_pUserComm);
}

void CDebugDL::CmdOvenElapse(unsigned long *param)
{     
	CTwinProxy::s_pTwinProxy->ForceShelfLife(2000);
	SendStr("Force out of warmup\r\n");
}

int CDebugDL::BaudrateGoUp()
{     
#if 1
	int ret = 1;
	if (m_startingBaudrate < 19200L) {
		m_startingBaudrate = 19200L;
	} else if (m_startingBaudrate < 38400L) {
		m_startingBaudrate = 38400L;
	} else if (m_startingBaudrate < 57600L) {
		m_startingBaudrate = 57600L;
	} else {
		ret = 0;
	}
	m_pUserComm->SetBaudRate(m_startingBaudrate); 

	if (ret) {
		CAllProcess::g_pAllProcess->SaveBaudrate();
	}

#if 0 // debug only
	{
		char buff[100];
		sprintf(buff, "Baud go up to %ld\r\n", m_startingBaudrate);
		CAllProcess::g_pAllProcess->DebugOutput(buff);
	}
#endif

	return ret;
#else
	return 0;
#endif
}

int CDebugDL::BaudrateGoDown()
{
#if 1
	int ret = 1;

	if (m_startingBaudrate > 38400L) {
		m_startingBaudrate = 38400L;
	} else if (m_startingBaudrate > 19200L) {
		m_startingBaudrate = 19200L;
	} else if (m_startingBaudrate > 9600L) {
		m_startingBaudrate = 9600L;
	} else {
		ret = 0;
	}
	m_pUserComm->SetBaudRate(m_startingBaudrate); 

	if (ret) {
		CAllProcess::g_pAllProcess->SaveBaudrate();
	}

#if 0 // debug only
	{
		char buff[100];
		sprintf(buff, "Baud go down to %ld\r\n", m_startingBaudrate);
		CAllProcess::g_pAllProcess->DebugOutput(buff);
	}
#endif

	return ret;
#else 
	return 0;
#endif	
}

void CDebugDL::AllowAutoRate(int16 bAllowed)
{
	m_bAutoRateAllowed = bAllowed;
}

void CDebugDL::ReportBootVersion()
{
	uint32 *pBootVersion;
	char buff[150];
	uint32 date;
	char *pBuildDate;
	char *pBuildTime;
	uint32 version;

	pBootVersion = (uint32 *)0x1ff00;
	if (*pBootVersion != 1) {
		SendStr("Boot version: UNKNOWN\r\n");
		return;
	}
	pBootVersion++;
	date = *(pBootVersion++);
	version = *(pBootVersion++);
	pBuildDate = (char *)(*(pBootVersion++));
	pBuildTime = (char *)(*(pBootVersion++));

	sprintf(buff, "Boot version: %d.%02d.%03d on %s(%02d-%02d-%02d) %s",
		(int)(version >> 16), (int)((version >> 8) & 0xff),
		(int)(version & 0xff), 
		pBuildDate,
		(int)((date % 10000) / 100), // month
		(int)(date / 10000),  // day
		(int)(date % 100),  // year
		pBuildTime);
	SendStr(buff);
}

void CDebugDL::CmdDir(unsigned long *param)
{     
	CLoader *pLoader;
	unsigned long start = MAIN_CODE_START;
	uint32 total;
	int ret;
	char product[20];
	char version[20];
	char buff[150];
	int lines;
	char *ptr;
	uint16 checkSum;

	pLoader = m_pLoader;
	ret = pLoader->CheckDLDBlock(start, &total);
	if (ret != 1) {
		SendStr("No file loaded\r\n");
		return;
	}
	checkSum = pLoader->GetCheckSum();

	strncpy(product, (char *)start + 6, 20); product[19] = 0;
	strncpy(version, (char *)start + 6 + HEADER_PRODUCT_NAME_LENGTH, 20); version[19] = 0;
	ret = strcmp(product, "RTE090-50321-01");
	if (ret != 0) {
		sprintf(buff, "Unknown DLD file, Length: %ld\r\n", total);
		SendStr(buff);
		return;
	}
	sprintf(buff, "RTE DLD Version: %s\r\nLength: %ld CheckSum=0x%X\r\nHardware Supported:\r\n"
		, version, total, checkSum);
	SendStr(buff);
	
	// SendStr((char *)start + 6 + HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH + HEADER_MAGIC_LENGTH);
	ptr = (char *)start + 6 + ((uint8 *)start)[1];
	lines = 0;
	do {
		if (ptr[6] != 0) {
			SendStr(ptr + 6);
			ptr += 6 + (uint8)ptr[1];
			lines++;
		}
	} while ((ptr[6] != 0) && (lines < 10));
	
	sprintf(buff, "\r\nCheck file header: %d\r\n", CAllProcess::CheckDldHeader((char *)start));
	SendStr(buff);
	return; 
}

void CDebugDL::LoadE1AIS(void) 
{
#if COMPLETE
	int i;
	CFPGA *pFPGA;
	COutput **pOutputs;
	
	COutputCal::m_E1CalTarget = 0;
	pOutputs = CAllProcess::g_pAllProcess->GetOutputs();
	pFPGA = CAllProcess::g_pAllProcess->GetFPGA();

	for (i = 0; i < COutput::OUTPUT_BANK_NUM; i++) {
		pFPGA->SetFpgaOutputType(i, CFPGA::OUTPUT_TYPE_SQUELCH);
	}

	for (i = 0; i < COutput::OUTPUT_BANK_NUM; i++) {
		pOutputs[i]->LoadWaveForm(COutput::OUTPUT_WAVE_E1);
	}

	for (i = 0; i < COutput::OUTPUT_BANK_NUM; i++) {
		pFPGA->SetFpgaOutputType(i, CFPGA::OUTPUT_TYPE_E1_AIS);
	} 
#endif	
}

void CDebugDL::CmdContinueRead(unsigned long *param)
{     
#if COMPLETE
	uint8 volatile *pAddress;
	int32 cnt;
	char buff[100];
	uint8 rdBuff[256];
	uint8 andBuff[256];
	uint8 orBuff[256];
	uint8 ch;
	int len;
	int i;

	if (*param == 5) {
		if ((param[1] == 1) || (param[1] == 2)) {
			memset(andBuff, 0xff, sizeof(andBuff));
			memset(orBuff, 0, sizeof(orBuff));
			if (param[5] > 256) {
				len = 256;
			} else {
				len = param[5];
			}
			
			for (cnt = 0; cnt < param[3]; cnt++) {
				pAddress = (uint8 *)(param[4]);
				if (param[2] !=0) 
					CAllProcess::g_pAllProcess->Pause(param[2]);
				for (i = 0; i < len; i++) {
					rdBuff[i] = ch = *pAddress;
					andBuff[i] &= ch;
					orBuff[i] |= ch;
					pAddress++;
				}
				if (param[1] == 1) {
					LWordToASCII(param[4], buff); buff[9] = 0;
					SendStr(buff + 2);
					for (i = 0; i < len; i++) {
						ByteToASCII(rdBuff[i], buff); buff[3] = 0;
						SendStr(buff);
					}
					SendStr("\r\n");
				}				

			}

			SendStr("And result: ");
			for (i = 0; i < len; i++) {
				ByteToASCII(andBuff[i], buff); buff[3] = 0;
				SendStr(buff);
			}

			SendStr("\r\nOr  result: ");
			for (i = 0; i < len; i++) {
				ByteToASCII(orBuff[i], buff); buff[3] = 0;
				SendStr(buff);
			}
			return;
		}
	}
	SendStr("Syntax: <option> <delay_tick> <address> <len>\r\n"
		"\t1 <delay_tick> <cycles> <address> <len>: \r\n\t\tRead with delay and print each read\r\n"
		"\t2 <delay_tick> <cycles> <address> <len>: \r\n\t\tRead with delay without printing each read\r\n"
		);  
#endif		
}

void CDebugDL::CmdRub(unsigned long *param)
{     
#if 1
	char *ptr;
	CRubProcess *pRub;
	CSci *pRubPort;
	char buff[100];

	pRub = CRubProcess::s_pRubProcess; //CAllProcess::g_pAllProcess->GetRubProcess();
	if (pRub == NULL) {
		SendStr("Not a rubidium RTE\r\n");
		return;
	}
	pRubPort = pRub->GetSci();
	if (*param == 2) {
		switch (param[1]) {
		case 1:
			ptr = GetParamStr(param[2]);
			if (ptr == NULL) break;
			pRub->AppendRubCmdFromDebugPort(ptr);
			return;
		case 3: // set baudrate
			{
				pRubPort->SetBaudRate(param[2]);
			} 
			return;
		default:
			break;
		}
	} else if (*param == 1) {
		switch (param[1]) {
		case 2:
			{
#if 0
				long rate;
				rate = pRubPort->GetBaudRate();
				sprintf(buff, "Current Rub Port baudrate: %ld\r\n", rate);
				SendStr(buff);
#endif
				pRub->PrintRubStatus(m_port);
			}
			return;
		default:
			break;
		}
	}
	SendStr("Syntax: <option> <str>\r\n"
		"\t1: Send <str> command to Rubidium\r\n"
		"\t2: Get current rubidium status\r\n"
		"\t3 <baud>: Set rubidium port baudrate\r\n");
#endif
}

void CDebugDL::CmdBT3(unsigned long *param)
{     
	// SendStr("BT3 debug command\r\n");
	CBT3Debug::s_pBT3Debug->PrintBT3Status(param, m_port);    
}

void CDebugDL::CmdRemoteDebug(unsigned long *param)
{
#if COMPLETE
	CRmtComm *pRmt;
	pRmt = CAllProcess::g_pAllProcess->GetRmtComm();
	if (pRmt == NULL) {
		SendStr("Remote debug not supported\r\n");
	}
	pRmt->PrintRmtDbgStatus(param, m_port);
#endif	
}

void CDebugDL::CmdPm(unsigned long *param)
{     
#if COMPLETE
	CInputPhase *pPhase;

	pPhase = CSecondProcess::s_pSecondProcess->GetInputPhase();
	if (pPhase != NULL) {
		pPhase->PrintInputPhaseStatus(param, m_port);
	} else {
		SendStr("Internal error\r\n");
	}
#endif	
}

void CDebugDL::CmdFpga(unsigned long *param)
{
	CFpga::s_pFpga->FpgaHack(param, m_pUserComm);
}

#define SECTOR_LIST CAmd29DL161DB::m_amd29DL161dbSectorList
void CDebugDL::CmdFlash(unsigned long *param)
{
	int ok = 0;
	CAmd29DL161DB *pFlash;
	int maxSector = CAmd29DL161DB::SECT_NUM;
	int i;
	long delay;
	char buff[200];
	unsigned long start = 0x800000;
	int ret;
	uint8 *pTmp;
	CNVRAM *pNVRAM = NULL;
	int oldPriority;
	int repetition = 1;
	int pause = 0;

	pNVRAM = CAllProcess::g_pAllProcess->GetNVRAM();

	pFlash = (CAmd29DL161DB *)m_pFlash;

	if ((*param == 1) && (pFlash != NULL)) {
		switch (param[1]) {
		case 1:
			for (i = 0; i < maxSector; i++) {
				delay = pFlash->GetEraseDelay(i);
				sprintf(buff, "Sector[%d start from 0x%lX] erase = %ld ms\r\n",
					i, SECTOR_LIST[i << 1 ] + start, delay);
				SendStr(buff);
				if (++pause == 20) {
					pause = 0; 
					WaitSpace();
				}
			}
			ok = 1;
			break;
		case 2:
			if (pNVRAM) pNVRAM->SaveNVRAMCopy();

			oldPriority = m_pProcess->SetPriority(CAllProcess::FIRST_PROC_PRIORITY);
			
			for (i = 0; i < maxSector; i++) {
				ret = pFlash->EraseSector(start + SECTOR_LIST[i << 1]);
				delay = pFlash->GetEraseDelay(i);
				sprintf(buff, "Sector[%d start from 0x%lX] erase = %ld ms %s\r\n",
					i, SECTOR_LIST[i << 1 ] + start, delay,
					(ret == 1) ? "OK" : "FAILED");
				SendStr(buff);
				if (++pause == 20) {
					pause = 0; 
					WaitSpace();
				}

				if (++pause == 20) {
					pause = 0; 
					WaitSpace();
				}
			}
			ok = 1;
			m_pProcess->SetPriority(oldPriority);
			break;
		case 4: 
			repetition = 10;
			pause = 100;
			// fall thru
		case 3: {
			unsigned long sectorHead;
			unsigned long sectorLen;
			unsigned long j;
			long writeDelay;
			CCheapTimer writeTimer;

			if (pNVRAM) pNVRAM->SaveNVRAMCopy();


			pTmp = new uint8[64*1024];
			if (pTmp == NULL) {
				SendStr("Not enough RAM!");
				return;
			}

			do {
				oldPriority = m_pProcess->SetPriority(CAllProcess::FIRST_PROC_PRIORITY);

				for (i = 0; i < maxSector; i++) {
#if ENABLE_WATCHDOG
					CWatchDog::ClearWatchDog();
#endif
					pFlash->Open();
					sectorHead = start + SECTOR_LIST[i << 1];
					sectorLen = SECTOR_LIST[(i << 1)+1] - SECTOR_LIST[i << 1] + 1;
					memcpy(pTmp, (char *)sectorHead, sectorLen);
					pFlash->Close();
#if ENABLE_WATCHDOG
					CWatchDog::ClearWatchDog();
#endif
					ret = pFlash->EraseSector(start + SECTOR_LIST[i << 1]);
				
					// Write 0.5K each time
					writeTimer.Start(30000L);
					for (j = 0; j < sectorLen; j += 512) {
						pFlash->WriteBytes(sectorHead + j, 512, pTmp + j);
#if ENABLE_WATCHDOG
						CWatchDog::ClearWatchDog();
#endif
						writeTimer.TimeOut();
					}
					writeDelay = 30000L - writeTimer.TimeLeft();

					delay = pFlash->GetEraseDelay(i);
					sprintf(buff, "Sector[%d start from 0x%lX] erase = %ld write = %ld ms %s\r\n",
						i, SECTOR_LIST[i << 1 ] + start, delay,
						writeDelay,
						(ret == 1) ? "OK" : "FAILED");
					SendStr(buff);

					m_pProcess->Pause(TICK_1SEC);

					if (++pause == 20) {
						pause = 0; 
						WaitSpace();
					}
				}
				m_pProcess->SetPriority(oldPriority);
			} while (--repetition > 0);

			ok = 1;
			delete [] pTmp;
				}
			break;
		}
	}
	if (!ok) {
		SendStr("Syntax: <option> Manufacture use only\r\n"
			"\t1: List current Flash erase delay.\r\n"
			"\t2: Erase all sectors and destroy all code and data.\r\n"
			"\t3: Save each sector and erase it and write back\r\n"
			"\t4: repeat option 3 ten times\r\n"
			);
	}

}

void CDebugDL::CmdWarmup(unsigned long *param)
{
	CWarmup::s_pWarmup->WarmupHack(param, m_port);
}
