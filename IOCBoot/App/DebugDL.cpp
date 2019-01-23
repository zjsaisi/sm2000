// DebugDL.cpp: implementation of the CDebugDL class.
//
//////////////////////////////////////////////////////////////////////

#include "DebugDL.h"
#include "string.h"
#include "BTmonDL.h"
#include "YModemRecv.h"
#include "Storage.h"
#include "sci.h"
#include "stdio.h"
#include "Loader.h"
#include "AllProcess.h" 
#include "cxconfig.h"
#include "BaseProcess.h"
#include "RTC.h"
#include "PTPDnldProcess.h"
#include "Amd29DL161DB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const CmdStruct CDebugDL::m_cmdList[] = {
	{"BTMON ", CMD_BTMON, "BTMON\t\t\t: Start BTMON download"},
	{"YMODEM ", CMD_YMODEM, "YMODEM Y <baudrate>\t: Start YMODEM download"},
	{"Y ", CMD_YMODEM, NULL},
	{"BAUD ", CMD_BAUDRATE, "BAUD\t\t\t: Change baudrate"}, 
	{"LOAD ", CMD_LOAD_DLD, "LOAD\t<adr>\t\t: Load code from <adr>"},
	{"VER ", CMD_VERSION, "VER REV\t\t: Print revision name"},
	{"REV ", CMD_VERSION, NULL},
	{"REVNAME ", CMD_VERSION, NULL},
	{"DATE ", CMD_SET_DATE, "DATE <year> <month> <day>: Set Date"},
	{"TIME ", CMD_SET_TIME, "TIME <hour> <minute> <second>: Set Time"},
	{"PROC ", CMD_PROCESS, "PROC\t\t\t: List process status"},
	{"SYS ", CMD_SYSTEM, "SYS <option>\t\t: System operation"},
	{"DIR ", CMD_DIR, "DIR\t\t\t: List DLD file"},
	{"FLASH ", CMD_FLASH, "FLASH\t\t\t: Flash debug"},

	{NULL, CMD_END, NULL}
};

CDebugDL::CDebugDL(const char *name, CLoader *pLoader, CBaseProcess *pProcess)
{
	char buff[80];
	m_pProcess = pProcess;
	m_pLoader = pLoader;
	m_pBTMON = new CBtmonDL();
	m_pYModem = new CYModemRecv();
	m_pStore = new CCodeStorage(pLoader);

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
	char buff[200];
	switch (cmd) {
	case CMD_SET_DATE:
		CmdSetDate(param);
		break;     
	case CMD_SET_TIME:
		CmdSetTime(param);
		break;
	case CMD_PROCESS: {
		CBaseProcess *pUser = g_pAll->GetDebugProcess();
		CBaseProcess *ptr;
		uint16 stkAvail, stkTotal;
		uint16 *stkAddress;
		
		sprintf(buff, "\r\nDebugger @%08lX\r\n", this);
		m_pUserComm->Send(buff);
		m_pUserComm->Send("\tName\tSlot\tAvail\tUsed\tTotal\tAddress\r\n");
		for (int i = 0; i < C_MAX_TASKS; i++) {
			ptr = pUser->GetProcess(i);
			if (ptr == NULL) break;
			ptr->GetStackStatus(&stkAvail, &stkTotal, &stkAddress);
			sprintf(buff, "%02d\t%s\t%d\t%d\t%d\t%d\t0x%08lX\r\n", 
				i, ptr->GetTaskName(), ptr->GetSlot(),
				stkAvail * 2,  (stkTotal - stkAvail) * 2,
				stkTotal * 2,
				stkAddress);
			m_pUserComm->Send(buff);
		}
					  }
		break;
	case CMD_VERSION: {
		g_pAll->ReportVersion(m_pUserComm);
					  }
		
		break;
	case CMD_LOAD_DLD:{
		unsigned long address = 0x810000;
		int ret;
		if (*param == 1) {
			address = param[1];
		}
		sprintf(buff, "Loading from 0x%lx\r\n", address);
		m_pUserComm->ForceSend(buff);
		ret = m_pLoader->CheckDLDBlock(address);
		if (ret != 1) {
			sprintf(buff, "Integrity check = %d\r\n", ret);
			m_pUserComm->ForceSend(buff);
			break;
		}
		ret = m_pLoader->Start(address, 1, 0);
		// Should never be here.
		m_pUserComm->ForceSend("FAILURE");
					  }
		break;
	case CMD_BAUDRATE: {
			long baudrate = m_startingBaudrate;
			if (*param == 1) {
				baudrate = param[1];
			}                            
			sprintf(buff, "Baudrate will be/is %ld\r\n", baudrate);
			m_pUserComm->ForceSend(buff);                  
			m_pUserComm->SetBaudRate(baudrate);           
			m_startingBaudrate = baudrate;
			if (*param == 0) {
				SendStr("Syntax for BAUD:\r\n\t<baud>: Change baudrate to <baud> (Should be 9600, 19200, 38400, 57600)\r\n");
			}
		}
		break;
	case CMD_BTMON:
		// BTMON requires 9600
		m_pUserComm->ForceSend("BTMON(firmware) started\r\n");
		m_pUserComm->SetBaudRate(9600);
		m_pBTMON->Start(m_pUserComm, m_pDebugComm, m_pFlash);
		m_pUserComm->SetBaudRate(m_startingBaudrate);
		m_pUserComm->ForceSend("BTMON(firmware ended\r\n");
		break;
	case CMD_YMODEM: {
		long baudrate = m_startingBaudrate; // Default
		int ret;
		const char *ptr;

		if (*param == 1) {
			baudrate = param[1];
		}
		sprintf(buff, "Set baudrate to %ld\r\n", baudrate);
		m_pUserComm->ForceSend(buff); 
		m_pUserComm->SetBaudRate(baudrate);
		// m_pStore->SetInit(m_pFlash);
		m_pStore->Rewind();
		ret = m_pYModem->Start(m_pUserComm, m_pStore);
		if (ret == 1) {
			ret = m_pYModem->Start(m_pUserComm, NULL);
			ret = (ret != 2) ? (-1) : (1);
		} else {
			ret = -1;
		}
		m_pUserComm->WaitTxDone(1000);
		m_pUserComm->SetBaudRate(m_startingBaudrate);
		ptr = (ret == 1) ? "\r\nYmodem file receiving OK" : "\r\nYModem file receiving failed/abort";
		if (ret == 1) {
			// Further check if it'd legitimate .DLD file.
			ret = m_pLoader->CheckDLDBlock(0x810000);
			if (ret != 1) {
				ptr = "Invalid .DLD file received\r\n";
			}
		}
		m_pUserComm->Send(ptr);
		break;
					 }
	case CMD_SYSTEM:
		CmdSystem(param);
		return;
	case CMD_DIR:
		CmdDir(param);
		break;
	case CMD_FLASH:
		CmdFlash(param);
		break;
	default:   
		SendStr("To be implemented\r\n");
		return;
	}
}

void CDebugDL::ExtendedListHelp(void) 
{
	const CmdStruct *ptr = m_cmdList;
	while (ptr->cmdId != CMD_END) {
		if (ptr->pComment) {
			SendStr(ptr->pComment);
			SendStr("\r\n");
		}
		ptr++;
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
	char buff[100];
	int year, month, day, hour, minute, second;
	unsigned long gpsSeconds, computerSeconds;
	g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
	gpsSeconds = g_pRTC->GetGPSSeconds();
	computerSeconds = gpsSeconds + 315964800L;
	sprintf(buff, "Current Time: %04d-%02d-%02d %02d-%02d-%02d GPS=%04X%04X PC=%04X%04X\r\n",
		year, month, day, hour, minute, second,
		(uint16)(gpsSeconds >> 16), (uint16)(gpsSeconds),
		(uint16)(computerSeconds >> 16), (uint16)(computerSeconds));
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
	g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
	gpsSeconds = g_pRTC->GetGPSSeconds();
	computerSeconds = gpsSeconds + 315964800L;
	sprintf(buff, "Current Time: %04d-%02d-%02d %02d-%02d-%02d GPS=%04X%04X PC=%04X%04X\r\n",
		year, month, day, hour, minute, second,
		(uint16)(gpsSeconds >> 16), (uint16)gpsSeconds, 
		(uint16)(computerSeconds >> 16), (uint16)computerSeconds 
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

void CDebugDL::CmdSystem(unsigned long *param)
{
	char buff[80];
	if (*param == 0) {
		const char *ptr;
		ptr = "\t6: Copy Code to Twin IOC\r\n"
			"\t7: Skip header checking for downloading\r\n";
			;
        SendStr(ptr);
		sprintf(buff, "Hardware Rev: %d\r\n", CAllProcess::GetHardwareRevision());
		SendStr(buff);
		sprintf(buff, "Check Dld header: %d\r\n", CAllProcess::CheckDldHeader((char *)0x810000));
		SendStr(buff);
		return;
	}
	if ((*param == 1) && (param[1] == 6)) {
		CPTPDnldProcess *pDnld;
		pDnld = g_pAll->GetPTPDnldProcess();
		pDnld->StartSendCodeToTwinIOC();
		return;
	}
	if ((*param == 1) && (param[1] == 7)) {
		CAllProcess::m_skipHeader = 1;
	}
	
	SendStr("Invalid Option\r\n");
}

void CDebugDL::CmdDir(unsigned long *param)
{
	CLoader *pLoader;
	unsigned long start = 0x810000;
	uint32 total;
	int ret;
	char product[20];
	//char card[20];
	char version[20];
	char buff[150];
	char *ptr;
	int lines;

	pLoader = g_pAll->GetLoader();
	ret = pLoader->CheckDLDBlock(start, &total);
	if (ret != 1) {
		SendStr("No file loaded\r\n");
		return;
	}
	strncpy(product, (char *)start + 6, 20); product[19] = 0;
	strncpy(version, (char *)start + 6 + HEADER_PRODUCT_NAME_LENGTH, 20); version[19] = 0;
	ret = strcmp(product, "DTI090-93121-01");
	if (ret != 0) {
		sprintf(buff, "Unknown DLD file, Length: %ld\r\n", total);
		SendStr(buff);
		return;
	}
	sprintf(buff, "IOC DLD Version: %s\r\nLength: %ld\r\nHardware Supported:\r\n", version, total);
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
	return;

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
	//CNVRAM *pNVRAM = NULL;
	int oldPriority;
	int repetition = 1;
	int pause = 0;

	//pNVRAM = CAllProcess::g_pAllProcess->GetNVRAM();

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
			//if (pNVRAM) pNVRAM->SaveNVRAMCopy();

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

			//if (pNVRAM) pNVRAM->SaveNVRAMCopy();


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
