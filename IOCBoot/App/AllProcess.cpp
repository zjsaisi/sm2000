// AllProcess.cpp: implementation of the CAllProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * 
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: AllProcess.cpp 1.5 2009/05/05 15:20:35PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 *
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "AllProcess.h"
#include "DebugProcess.h"
// #include "amd29.h" 
#include "Amd29DL161DB.h"
#include "sci.h"
#include "UserProcess.h"
#include "RTC.h"
#include "version.h"
#include "stdio.h"
#include "loader.h"
#include "machine.h"
#include "vect.h"
#include "PTPIsr.h"
#include "PTPLink.h"
#include "PTPProcess.h"
#include "PTPDnldProcess.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "CxFuncs.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAllProcess *g_pAll;
int CAllProcess::m_skipHeader;

CAllProcess::CAllProcess()
{
	m_blinkIndex = 0;
	m_skipHeader = 0;
	// Facility
	sci_init();
	CCheapTimer *pTimer1 = new CCheapTimer();          
	CPVOperation *pPV = new CPVOperation();                
	
	// m_pFlash = new CAmd29lv800bb(0x800000, pTimer1, pPV); 
	m_pFlash = new CAmd29DL161DB(0x800000, pTimer1, pPV);
	
	g_pRTC = new CRTC();   
	m_pLoader = new CLoader(m_pFlash);
	
	m_pDebug = NULL;
	m_pUser = NULL;
    m_pIMCIOC = NULL;
	m_pIOCIOC = NULL;

	if (g_pSci1) {
		g_pSci1->SetBaudRate(DEFAULT_BAUD1);
	} 
	if (g_pSci0) {
		g_pSci0->SetBaudRate(DEFAULT_BAUD0);
	}              
	//ReportVersion();
#if 0    
	if (g_pSci0) {
		g_pSci0->ForceSend("Debug port Enabled\r\n");
	}
	if (g_pSci1) { 
		g_pSci1->ForceSend("User port Enable\r\n");	 
	}
#endif		
	
	// Process
#if ENABLE_SCI0     
	m_pDebug = new CDebugProcess(g_pSci0, m_pLoader, m_pFlash, "BTDBG", DEBUG_PRIORITY, 3000);
#endif

#if ENABLE_SCI1
	m_pUser = new CUserProcess(g_pSci1, g_pSci0, m_pLoader, m_pFlash, "BTUSER", USER_PRIORITY, 3002);
#endif          

	// Start Point-to-Point ISR
	init_ptp();
#if ENABLE_SCI4
	{
		CPTPLink *pLink = new CPTPLink("IOC_IOC");
		g_pSci4->SetLink(pLink);
		pLink->SetDriver(g_pSci4);
		CPTPProcess *pTask = new CPTPProcess(pLink, "IOC_IOC", IOC_IOC_PRIORITY, 2006, CPTPProcess::Entry2);
		pLink->SetTaskSlot(pTask->GetSlot());
		m_pIOCIOC = pTask;
		m_pLinkIOCIOC = pLink;
	}
#endif
#if ENABLE_SCI3
	{
		CPTPLink *pLink = new CPTPLink("IMC_IOC");
		g_pSci3->SetLink(pLink);
		pLink->SetDriver(g_pSci3);
		CPTPProcess *pTask = new CPTPProcess(pLink, "IMC_IOC", IMC_IOC_PRIORITY, 2004, CPTPProcess::Entry1);
		pLink->SetTaskSlot(pTask->GetSlot());
		m_pIMCIOC = pTask;
		m_pLinkIMCIOC = pLink;
	}
#endif

	m_pPTPDnldProcess = new CPTPDnldProcess(m_pLoader, m_pFlash, m_pLinkIOCIOC,
		m_pLinkIMCIOC, "DNLD", PTP_DNLD_PRIORITY, 3016);

	// Compensate for time lost in RAM check 
#if RUN_IN_ROM	
	for (int i = 0; i < 4; i++) {
		g_pRTC->Tick();
	}	
#endif
#if RUN_IN_RAM
	g_pRTC->Tick();
#endif
	
	m_mainAvailable = 0;

	// Init baudrate according to setup in Flash
	{
		int32 *pHead1, *pHead2;
		int32 *pRate = NULL;
		int32 debugRate = DEFAULT_BAUD0, userRate = DEFAULT_BAUD1;
        int32 invalid = 0L;
        
		pHead1 = (int32 *)0x802000;
		pHead2 = (int32 *)0x80a000;
		if (*((int32 *)0x80200a) != 0x5652414d) {
			pHead1 = &invalid;
		} 
		if (*((int32 *)0x80a00a) != 0x5652414d) {
			pHead2 = &invalid;
		}
		if (*pHead1 > *pHead2 ) {
			pRate = pHead1 + (48 >> 2);
		} else if (*pHead1 < *pHead2) {
			pRate = pHead2 + (48 >> 2);
		} 

		if (pRate != NULL) {
			debugRate = *pRate;
			userRate = *(pRate + 1);
		}
		switch (debugRate) {
		case 9600L: case 19200L: case 38400L: case 57600L:
			break;
		default:
			debugRate = DEFAULT_BAUD0;
			break;
		}
		switch (userRate) {
		case 9600L: case 19200L: case 38400L: case 57600L:
			break;
		default:
			userRate = DEFAULT_BAUD1;
			break;
		}

		if (g_pSci0) {
			g_pSci0->SetBaudRate(debugRate);
		}

		if (g_pSci1) {
			g_pSci1->SetBaudRate(userRate);
		}
	}
}

CAllProcess::~CAllProcess()
{

}

void CAllProcess::StartAll(unsigned long nextBlock)
{
	char buff[100];
	int i, ret;
	const char *str;
	// Cannot enable interruption if OS is started.
	set_exr(7);
#if 0	
	TMR0.TCR.BIT.CMIEA = 1; // Enable RTOS tick.
  	INTC.IPRI.BIT.HIGH = 6;  // Set TMR0 - RTOS tick  
#else 
  	INTC.IPRL.BIT.HIGH = 6;  // Set TMR2,TMR3 Priority Level
	TMR3.TCR.BIT.CMIEA = 1; // enable RTOS tick
#endif
  	
#if _DEBUG
	ReportVersion(g_pSci1); ReportVersion(g_pSci0);
#endif  	     

#if NDEBUG  	
  	// Try to load the next block
  	if (nextBlock) {
		const char *ptr;
		sprintf(buff, "Continue to load from 0x%lX\r\n", nextBlock); 
		SendStr(buff); ForceDebugOutput(buff);
		// Clear part of RAM. FPGA module could mess up with first 512K
		{
			uint16 *wPtr;
			wPtr = (uint16 *)0x210000;
			while (wPtr < (uint16 *)0x280000) {
				*wPtr = 0;
				wPtr++;
			}
		}

		m_pLoader->Start(nextBlock, 1, 0);
		// Shouldn't be here
		ptr = "Loading failed\r\nRebooting...";
		SendStr(ptr); ForceDebugOutput(ptr);
		PowerON_Reset();
  	}                 
  	ReportVersion(g_pSci1); ReportVersion(g_pSci0);
	SendStr("User Port connected\r\nL: Load main code\tD: Debugger\r\n"); 
	ForceDebugOutput("Debug Port connected\r\nL: Load main code\tD: Debugger\r\n"); 	
  	CheckBoot();
  	// Wait for choice       
  	i = m_pLoader->CheckDLDBlock(0x810000);
	if (i != 1) {
		str = "No valid main code\r\n";
		SendStr(str); ForceDebugOutput(str);
	} else {
		m_mainAvailable = 1; 
#if 0		
		uint16 stay = 0;           
		int time;
		char ch;
		// Give it 5 seconds before starting main code
		for (i = 4; i >= 0; i--) {
			sprintf(buff, "\r%d \r", i);
			SendStr(buff); ForceDebugOutput(buff);
			if ((g_pSci1)||(g_pSci0)) {
				time = 1000;
                if (g_pSci1) {
					ret = g_pSci1->Recv(&ch, 1, 900, 1);
					time -= 1000;
				} else {
					ret = -1;
				}
				if ((ret != 1) && (g_pSci0)) {
					ret = g_pSci0->Recv(&ch, 1, time, 1);
				}   
				
				switch (ch) {
			 	case 'd': case '\r': case '\n': case ' ':
					ch = 'D';
					break;
				case 'l': 
					ch = 'L';
					break;	
				}
				                        
				if ( ( (ch == 'L') && (stay == 1) && (ret != 1) ) 
					|| ((ret != 1) && (i == 0))
					)
				{   
					str = "Loading main code\r\n";
					SendStr(str); ForceDebugOutput(str);
					m_pLoader->Start(0x810000, 1, 0);
					// Should be here
					break;
				}
				if ((ch == 'D') && (stay == 1) && (ret != 1)) {
					// Previous second got one char. And so far there is only one char.
					// Get into debugger.
					break;
				}
				
				if (ret == 1) {
					stay++;
					i++;
				} else {
					stay = 0;
					ch = 0;
				}
			}
		}
#endif
	}
  	set_exr(7);
#endif   // NDEBUG

	// Start debugger
	if (m_pDebug)
		m_pDebug->StartProcess();
	if (m_pUser) 
		m_pUser->StartProcess();
	if (m_pIMCIOC) 
		m_pIMCIOC->StartProcess();
	if (m_pIOCIOC) 
		m_pIOCIOC->StartProcess();
	if (m_pPTPDnldProcess) 
		m_pPTPDnldProcess->StartProcess();
}

void CAllProcess::ReportVersion(Ccomm *pSci)
{
	char buff[250];
	char const *ptr;
	
	if (!pSci) return;

	sprintf(buff, 
	"\r\n%s BOOT: %d.%02d.%02d\r\nBuild on: %s %s\r\n",
	BUILD_NAME,
	(int)(VERSION_NUMBER >> 16), (int)((VERSION_NUMBER >> 8) & 0xff),
	(int)(VERSION_NUMBER & 0xff),
	g_sBuildDate, 
	g_sBuildTime
	);

	pSci->ForceSend(buff);
	ptr = "Copyright of Zhejiang Saisi.\r\nAll Rights Reserved.\r\n";
	pSci->ForceSend(ptr);

#if RUN_IN_ROM
	pSci->ForceSend("Run from ROM\r\n");
#else
	pSci->ForceSend("Run from RAM\r\n");
#endif
}

void CAllProcess::SendStr(const char *str)
{
	if (g_pSci1) {
		g_pSci1->ForceSend(str);
	}
}

EXTERN int PassCheckSum(unsigned long *sumH, unsigned long *sumL);

int CAllProcess::CheckBoot()
{   
	char buff[100];
	int ret;            
	unsigned long chkH, chkL;
	const char *ptr;
	
	ret = PassCheckSum(&chkH, &chkL); 
	switch (ret) {
	case 1: ptr = "OK";
	break;
	case 2: ptr = "passed with MAGIC";
	break;
	default:
		ptr = "Failed";
	break;
	}
	sprintf(buff, "ROM checksum %s: [H:%08lX L: %08lX]\r\n", ptr, chkH, chkL);
	SendStr(buff);
	ForceDebugOutput(buff);
	return 1;
}

CBaseProcess * CAllProcess::GetUserProcess()
{
	return m_pUser;
}

CBaseProcess * CAllProcess::GetDebugProcess()
{
	return m_pDebug;
}

void CAllProcess::DebugOutput(const char *str, int len/* =-1*/, uint32 option /*= 0x10000*/)
{
	// Option to be implemented. 
	if (len <= 0) {
		len = strlen(str);
	}
	if (g_pSci0) {
		g_pSci0->Send(str, len);
	}
}

void CAllProcess::ForceDebugOutput(const char *str)
{
	if (g_pSci0) {
		g_pSci0->ForceSend(str);
	}

}
                  
void CAllProcess::SecondTick()
{   
	if (g_pRTC)
		g_pRTC->Tick();
	// Anything else should be stopped if g_cmx_started == 0	
}

// Block other process when making choice
void CAllProcess::WaitForChoice()
{
	if (m_pUser) {
		while (!m_pUser->IsRunning()) {
			// Wait here
			K_Task_Wait(2);
		}
	}

	if (m_pDebug) {
		while (!m_pDebug->IsRunning()) {
			// Wait here
			K_Task_Wait(2);
		}
	}
}

EXTERN void GoFailed(void);

extern unsigned char g_appFailCnt;
extern volatile unsigned long g_systemTicks;

void CAllProcess::MakeBootChoice(Ccomm *pComm, int bBlinking/* = 0 */)
{ 
	static int8 volatile stopChoice = 0;
	char buff[150];
	uint16 cnt = 0;           
	char ch = 0;
	int i;
	int ret;
	const char *str;
	int blink = 0;

#if _DEBUG
	return;
#endif		
	if (pComm == NULL) return;
    if (!m_mainAvailable) return;

	if (g_appFailCnt) {
		sprintf(buff, "Application failed %d times\r\n", g_appFailCnt);
		pComm->Send(buff);
	}

	// Give it 5 seconds before starting main code
	for (i = 19; i >= 0; i--) {	
		
		if (bBlinking) {
			if ((i & 0x2) == blink) {
				BlinkChoice();
				blink ^= 2;
			}
		}

		sprintf(buff, "\r%d \r", i>>2);
		pComm->Send(buff);
		ret = pComm->Receive(&ch, 1, 250);
			
		switch (ch) {
	 	case 'd': case '\r': case '\n': case ' ':
			ch = 'D';
			break;
		case 't':
			ch = 'T';
			break;	
		case 'l': 
			ch = 'L';
			break;	
		}
		
		if (i <= 0) {
			if (!GetFpgaDone()) {
				i = 19; // Do it again without FPGA done
				pComm->ForceSend("FPGA is not loaded, Retry!\r\n");
			}
		}

		if ( ( (ch == 'L') && (cnt == 1) && (ret != 1) ) 
			|| ((ret != 1) && (i == 0) && (g_appFailCnt < MAX_APP_FAIL_ALLOWED))
			)
		{ 
			if ((ch == 'L') && (cnt == 1) && (ret != 1)) {
				g_appFailCnt = 0;
			}
			stopChoice = 1;
			g_systemTicks = 0L;
			str = "Loading main code\r\n";
			pComm->ForceSend(str);
			//SendStr(str); ForceDebugOutput(str);
			m_pLoader->Start(0x810000, 1, 0);
			// Shouldn't be here
			break;
		}
		
		if ((ch == 'D') && (cnt == 1) && (ret != 1)) {
			// Previous second got one char. And so far there is only one char.
			// Get into debugger.
			stopChoice = 1;
			break;
		}
		          
		if ((ch == 'T') && (cnt == 1) && (ret != 1)) {  
			set_exr(7); // Go to failed mode  
			stopChoice = 1;
			GoFailed();
			break;
		}
		
		if (stopChoice) break;

		if (ret == 1) {
			cnt++;
			i++;
		} else {
			cnt = 0;
			ch = 0;
		}			
	}

}

void CAllProcess::Reboot()
{
	FUNC_PTR fPtr;
	unsigned long *lPtr = (unsigned long *)0;

	// Delay 0.5s for event to get thru
	K_Task_Wait(TICK_1SEC >> 1);

	set_exr(7);
	g_appFailCnt = 0;
	fPtr = (FUNC_PTR)((*lPtr) & 0xffffff);
	(*fPtr)();

}

uint16 CAllProcess::m_pattern[BLINK_WAIT_NUM] = {
 ~0x80, ~0x400, ~0x2000
};

void CAllProcess::BlinkChoice()
{
#if 0 // For TP5000, no led
	*((uint16 volatile *)0x600000) = m_pattern[m_blinkIndex];

	m_blinkIndex++;
	if (m_blinkIndex >= BLINK_WAIT_NUM) {
		m_blinkIndex = 0;
	}
#endif
}

uint8 CAllProcess::GetHardwareRevision()
{	// This is verified to work. 
	uint8 ret;
	uint8 bp;
	ret = P9.PORT.BYTE;
	ret >>= 4;
	bp = P7.PORT.BYTE;
	bp <<= 3;
	return ret | (bp & 0xf0);
}

int8 CAllProcess::GetFpgaDone()
{
	if (P1.PORT.BIT.B3) return 1;
	return 0;
}

// return 0 for ok
// return -1 for invalid header
int CAllProcess::CheckDldHeader(const char *pHead)
{
	int failed = 0;
	const char *ptr;
	int lines;
	
	if (m_skipHeader == 1) return 0;

	if (*pHead != 0x25) return -1;

	if (strcmp(pHead + 6, "RTE090-50321-01") != 0) {
		failed = -1;
	} else if (strcmp(pHead + 6 + HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH, "M\rA\nG\r\nI\n\rC1234") != 0) {
		failed = -1;
	} else {
		char hardwareRev[10];
		sprintf(hardwareRev, " %d ", GetHardwareRevision());

		ptr = pHead + 6 + (uint8)pHead[1];
		lines = 0;
		failed = -1;
		do {
			if (strstr(ptr + 6, hardwareRev) != NULL) {
				failed = 0;
				break;
			}
			ptr += 6 + (uint8)ptr[1];
			lines++;
		} while ((ptr[6] != 0) && (lines < 10));
#if 0
		if (strstr(pHead + 6 + HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH + HEADER_MAGIC_LENGTH, 
			hardwareRev) == NULL) {
			failed = -1;
		}
#endif
	}
	return failed;
}
