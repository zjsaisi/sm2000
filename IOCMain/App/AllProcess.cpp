#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "iodefine.h"
#include "Cxfuncs.h"
#include "AllProcess.h"
#include "asm.h"
#include "machine.h"
#include "stdarg.h"
#include "stdio.h"
#include "sci.h"
#include "RTC.h"
#include "comm.h"
#include "flash.h"
#include "version.h"
#include "IdleProcess.h"
#include "loader.h"
#include "Amd29DL161DB.h"
#include "DebugProcess.h"
#include "WatchDog.h"
#include "NVRAM.h"
#include "HouseProcess.h"
#include "Factory.h"
#include "UserProcess.h"
#include "PTPIsr.h"
#include "PTPLink.h"
#include "PTPProcess.h"
#include "IstateProcess.h"
#include "PTPDnldProcess.h"    
#include "EventStore.h"
#include "EventProcess.h"
#include "EventKb.h"
#include "Fpga.h"
#include "DebugMask.h"
#include "Warmup.h"
#include "CardProcess.h"
#include "TwinProxy.h"
#include "RmtComm.h"
#include "RmtDbgProcess.h"
#include "OutputProcess.h"
#include "SecondProcess.h"
#include "MinuteProcess.h"
#include "DtiOutput.h"
#include "CardStatus.h"
#include "IocKeyList.h"
#include "IstateDti.h"
#include "Input.h"
#include "InputTod.h"
#include "InputSelect.h"
#include "DtiOutputTrace.h"
#include "DtiDelayStable.h"
#include "BT3Api.h"
#include "BestCal.h"
#include "NewCode.h"
#include "TeleOutput.h"
#include "TenMHz.h"
#include "OneHz.h"             
#include "RubProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma section Mark
static uint16 EndOfRamVar;
static uint16 EndOfDSection = 1;
#pragma section

CAllProcess *CAllProcess::g_pAllProcess = NULL;
char CAllProcess::g_versionString[MAX_SERIAL_LENGTH + 1];

CAllProcess::CAllProcess()
{	
	CFlash *pFlash;
	CLoader *pLoader;

	m_bPassEvent = 3;

	m_pRubProcess = NULL;

	m_shutdown = 0;
	sprintf(g_versionString, "%d.%02d.%02d",
			(uint8)(VERSION_NUMBER >> 16), (uint8)(VERSION_NUMBER >> 8),
			(uint8)(VERSION_NUMBER));
	m_bIstateInited = 0;
	m_failedOffline = 0;
	EndOfRamVar = 0xa55a;
	m_bBooting = 1;
	g_pAllProcess = this;
	m_pDebugProcess = NULL;
	m_pUserProcess = NULL;
	m_pNVRAM = NULL;
	m_pLinkImcIoc = NULL;
	m_pLinkIocIoc = NULL;

	m_pDebugMask = new CDebugMask();
	// Create all object in the system
	sci_init();
	g_pRTC = new CRTC();
	CFpga::CreateFpga();
	
	{
		// There is dependence chain. I don't see any big reason to untangle it.
		// RTC, Flash ==> loader NVRAM ==> Factory, Istate, BT3 Setting
		CCheapTimer *pTimer = new CCheapTimer();
		CPVOperation *pPV = new CPVOperation();
		pFlash = new CAmd29DL161DB(0x800000, pTimer, pPV);
		pLoader = new CLoader(pFlash);
		// Leave 0x800000 and 0x808000 available for something else.
#if RUN_ON_TIMEPROVIDER
		m_pNVRAM = new CNVRAM(g_pRTC, pFlash, 0x804000, 0x806000); 
#else
		m_pNVRAM = new CNVRAM(g_pRTC, pFlash, 0x802000, 0x80a000); // Each block could be up to 8K*3 if needed
#endif
		new CFactory(m_pNVRAM);
	}

	SetupPtp();

	new CIstateProcess(m_pLinkImcIoc, m_pLinkIocIoc, m_pNVRAM, 
		"ISTATE", ISTATE_PROC_PRIORITY, 6000);


	new CPTPDnldProcess(CFactory::s_pFactory, CIstateProcess::s_pIstateProcess,
		pLoader, pFlash, m_pLinkIocIoc, m_pLinkImcIoc,
		"PTPDNLD", PTP_DNLD_PROC_PRIORITY, 6000);

	m_pRubProcess = new CRubProcess(m_pLinkImcIoc, g_pSci1,
		"RUBIDIUM", RUBIDIUM_PROC_PRIORITY, 6000);

	new CIdleProcess("IDLE", g_pSci0, FIRST_PROC_PRIORITY, 3000);

#if ENABLE_SCI1
	m_pUserProcess = new CUserProcess(g_pSci1, g_pSci0, pLoader, pFlash,
		"USER", USER_PROC_PRIORITY, 5000);
#endif


	{
		CEventStore *pEventStore;
		CIstate *pIstate;

		pEventStore = new CEventStore(pFlash);
		pIstate = CIstateProcess::s_pIstateProcess->GetIOCIstate();
		new CEventProcess(m_pLinkImcIoc, pEventStore, pIstate,
			"EVENT", EVENT_PROC_PRIORITY, 8000);
	}

	new CHouseProcess("HOUSE", HOUSE_PROC_PRIORITY, 5004);
	new CWarmup();
	new CCardProcess("CARD", CARD_PROC_PRIORITY, 6000);

	// Add remote debug tunnel
	{
		CCheapTimer *pTimer;              
		CPVOperation *pPV;

	    pPV = new CPVOperation();                        
		pTimer = new CCheapTimer();
		m_pRmtComm = new CRmtComm(pTimer, pPV);
		new CRmtDbgProcess(pFlash, pLoader, m_pRmtComm, "RmtDbg", REMOTE_DEBUG_PROC_PRIORITY, 5008);
	}

	new COutputProcess("OUTPROC", OUTPUT_PROC_PRIORITY, 5004);
	new CSecondProcess("SECPROC", SECOND_PROC_PRIORITY, 5000);
	new CMinuteProcess("MINPROC", MINUTE_PROC_PRIORITY, 5008);
	CDtiOutput::Create();
	CTeleOutput::CreateTeleOutput();
	CTenMHzOutput::CreateTenMHzOutput();
	COneHzOutput::CreateOneHzOutput();
	CInput::CreateInput();
	new CInputSelect();
	CDtiOutputTrace::Create();
	CDtiDelayStable::Create();
	new CBT3Api(m_pNVRAM);
	CNewCode::Create((char *)MAIN_CODE_START, pLoader);

	// Add new object at the end and go to Init() and then go to Start().

	// This is supposed to be the last one. So that we will know the heap usage.
#if ENABLE_SCI0
	m_pDebugProcess = new CDebugProcess(g_pSci0, pLoader, pFlash, 
		"DEBUG", DEBUG_PROC_PRIORITY, 5000);
#endif
}

CAllProcess::~CAllProcess()
{

}

void CAllProcess::Pause(int ticks) 
{ 
	if (g_cmx_started && ticks) K_Task_Wait(ticks); 
}

void CAllProcess::MaskedPrint(uint32 mask, const char *format, ...)
{
	va_list list;

	if (CAllProcess::g_pAllProcess->CheckMask(mask) <= 0) return;
	va_start(list, format);
	CAllProcess::g_pAllProcess->vPrint(format, list);
	va_end(list);
	
}

void CAllProcess::vMaskedPrint(uint32 mask, const char *format, va_list list)
{
	if (CAllProcess::g_pAllProcess->CheckMask(mask) <= 0) return;
	CAllProcess::g_pAllProcess->vPrint(format, list);
}

void CAllProcess::Print(const char *format, ... )
{
	va_list list;
	va_start(list, format);
	CAllProcess::g_pAllProcess->vPrint(format, list);
	va_end(list);

}

void CAllProcess::vPrint(const char *format, va_list list)
{
	char buff[1525];

	vsprintf(buff, format, list);
	if (g_pSci0) {
		g_pSci0->Send(buff);
	}
	if (m_pRmtComm) {
		m_pRmtComm->PostAO(buff, strlen(buff));
	}
}

void CAllProcess::DumpHex(const char *pHeader, unsigned char const *ptr, int len)
{
	char buff[250];
	char b[10];

	//if (!(m_debugOption & 0x10000)) {
	//	return;
	//}
	if (CheckMask(0x10000) <= 0) return;

	if (len > 65) len = 65;
	//buff[0] = 0;
	strcpy(buff, pHeader);
	for (int i = 0; i < len; i++) {
		sprintf(b, "%02X ", *ptr++);
		strcat(buff, b);
	}
	strcat(buff, "\r\n");
	if (g_pSci0) {
		g_pSci0->Send(buff);
	}
	if (m_pRmtComm) {
		m_pRmtComm->PostAO(buff, strlen(buff));
	}
}

int CAllProcess::Init()
{
	// Take care of the spider connection between objects.
	int isRubidium = 0;
	{ // Set slot id to FPGA
		int8 slot;
		slot = GetSlotId();
		CFpga::s_pFpga->SetSlotId(slot);
	}


	// Pack up factory, istate, BT3 blocks into NVRAM
	CFactory::s_pFactory->AddToNVRAM();   // This add factory setting into NVRAM
	CIstateProcess::s_pIstateProcess->Init(); // This will add istate to NVRAM
	CBestCal::s_pBestCal->RegisterNvram();

	m_pNVRAM->InitSetup();
	
	CFactory::s_pFactory->InitFactory(); // Read from NVRAM

	if (CFpga::s_pFpga->IsRubidium() == 1) {
		m_oscQL = 3;
		isRubidium = 1;
	} else {
		m_oscQL = CFactory::s_pFactory->GetOscQualityLevel();
	}

	InitBaudrate(); // Setup baud rate according to factory setting

	m_pNVRAM->Init();

	m_pDebugMask->Init();
	CCardProcess::s_pCardProcess->Init();
	CWarmup::s_pWarmup->Init();
	CEventProcess::s_pEventProcess->Init();
	CPTPDnldProcess::s_pPTPDnldProcess->Init();
	m_pImcIocProcess->Init();
	m_pIocIocProcess->Init();
	COutputProcess::s_pOutputProcess->Init();
	CSecondProcess::s_pSecondProcess->Init();
	CMinuteProcess::s_pMinuteProcess->Init();
	CIdleProcess::s_pIdleProcess->Init();
	m_pDebugProcess->Init();
	m_pUserProcess->Init();
	CDtiOutput::Init();
	CInput::Init();
	CDtiDelayStable::Init();
	CNewCode::s_pNewCode->Init();

	if (CHouseProcess::s_pHouseProcess != NULL) 
		CHouseProcess::s_pHouseProcess->Init();

	if (m_pRubProcess != NULL) {
		m_pRubProcess->Init();
	}

	CBT3Api::s_pBT3Api->Init(isRubidium);

	return 1;
}

int CAllProcess::StartAll()
{
	int ret;

	// Gentlemen, start your engine!
    m_bBooting = 0;


	m_pImcIocProcess->StartProcess();
	m_pIocIocProcess->StartProcess();
	CIstateProcess::s_pIstateProcess->StartProcess();
	CPTPDnldProcess::s_pPTPDnldProcess->StartProcess();
	if (CHouseProcess::s_pHouseProcess != NULL) {
		CHouseProcess::s_pHouseProcess->StartProcess();
	}
	CCardProcess::s_pCardProcess->StartProcess();
	m_pDebugProcess->StartProcess();
	m_pUserProcess->StartProcess();
	
	ret = CEventProcess::s_pEventProcess->StartProcess();
	
	if (CRmtDbgProcess::s_pRmtDbgProcess != NULL) {
		CRmtDbgProcess::s_pRmtDbgProcess->StartProcess();
	}

	if (COutputProcess::s_pOutputProcess != NULL) {
		COutputProcess::s_pOutputProcess->StartProcess();
	}
	
	if (CSecondProcess::s_pSecondProcess != NULL) {
		CSecondProcess::s_pSecondProcess->StartProcess();
	}

	if (CMinuteProcess::s_pMinuteProcess != NULL) {
		CMinuteProcess::s_pMinuteProcess->StartProcess();
	}

	if (m_pRubProcess != NULL) {
		m_pRubProcess->StartProcess();
	}

	CIdleProcess::s_pIdleProcess->StartProcess();
	return 1;
}

// return 0 for out of mask
// return 1 for inside mask
int CAllProcess::CheckMask(uint32 mask)
{
	if (mask == 0) return 0;
	return m_pDebugMask->CheckMask(mask);
}

void CAllProcess::TaskPerSecond() // Called House process
{
	m_pNVRAM->NVRAMTickPerSecond();
	CTwinProxy::s_pTwinProxy->TickOneSecondTwinProxy();
	if (m_shutdown) {
		m_shutdown--;
		if (m_shutdown == 0) {
			RebootSystem();
		}
	}
}

// To announce second edge
// Entry point for ISR from Hitachi timer
void CAllProcess::OsSecondAnnounce()
{
	if (m_bBooting) return;
	if (m_failedOffline) {
		if (m_bPassEvent) {
			m_bPassEvent--;
		}
	}
#if TIMING_CPU
	g_pRTC->TickOfOneSecond();
	CCardProcess::s_pCardProcess->ReportCardOneSecondIsr();
#endif
}

// To announce second edge
// Entry point for ISR from CSM FPGA
void CAllProcess::CsmSecondAnnounce()
{
	if (m_bBooting) return;
	CSecondProcess::s_pSecondProcess->TickOneSecondIsr();
#if TIMING_CSM
	g_pRTC->TickOfOneSecond();
	CCardProcess::s_pCardProcess->ReportCardOneSecondIsr();
#endif
}

int CAllProcess::ReportEvent(int16 id, uint8 active, uint8 aid1, uint8 aid2)
{
	if (m_bBooting || (m_bPassEvent <= 0) ) return 0;

	// Added event reporting function here
	return CEventProcess::s_pEventProcess->ReportEvent(id, active, aid1, aid2);
}

uint32 CAllProcess::GetEndOfRam(uint16 *pValue)
{
	if (pValue) *pValue = EndOfRamVar;
	return (uint32)&EndOfRamVar;
}

extern volatile unsigned long g_systemTicks;
void CAllProcess::RebootSystem(int quick, /* = 0*/Ccomm *pComm /* = NULL */, int evt/*=COMM_RTE_EVENT_RTE_RESET*/)
{                 
	int ret;
	uint8 tmp_exr;
	
	m_bBooting = 1;
	CFpga::s_pFpga->SetReady(0);
	
	tmp_exr = get_exr();
	set_exr(7);

	CWatchDog::StopWatchDog();

	set_exr(tmp_exr);

	ClearStandingEvent();
	// Report event
	CEventProcess::s_pEventProcess->ReportEvent(evt, 1);

	// Save NVRAM
	if (pComm) {
		pComm->ForceSend("Saving NVRAM..."); 
		//pComm->ForceSend("What is going on");
	}
	ret = m_pNVRAM->SaveNVRAMCopy();
	if (pComm) {
		pComm->ForceSend( (ret == 1) ? ("Done!\r\n") : ("Skipped!\r\n") );
	}

	// Delay 3s for event to get thru
	K_Task_Wait(TICK_1SEC * 3);

	set_exr(7);
	FUNC_PTR fPtr;
	unsigned long *lPtr = (unsigned long *)0;
	fPtr = (FUNC_PTR)((*lPtr) & 0xffffff);
	if (quick == 1) {
		ForceNextBlock(MAIN_CODE_START);
		Quit();
	} else if (quick == 2) {
		(*fPtr)();
	} else {
		// Tell boot code don't count this as a failure
		MarkNormalReboot();

		(*fPtr)();
	}

#if 0
	int ret;
	uint8 tmp_exr;
	int state;

	state = CCardState::s_pCardState->GetCardState();
	if (state == CCardState::CARD_STATE_ACTIVE) {
		for (int i = 0; i < 3; i++) {
			CCardState::s_pCardState->ForceStandby();
			Pause(TICK_1SEC);
		}
	}

	tmp_exr = get_exr();
	set_exr(7);

	CWatchDog::StopWatchDog();

	REPORT_EVENT(COMM_RTE_EVENT_RTE_RESET, 1);
	ClearStandingEvents(); // Clear all events according to Tom F's TL1 
	m_bRebooting = 1;

	set_exr(tmp_exr);

	if (pComm) {
		pComm->Send("Saving NVRAM...");
	}
	ret = m_pNVRAM->SaveNVRAMCopy();
	if (pComm) {
		pComm->ForceSend( (ret == 1) ? ("Done!\r\n") : ("Skipped!\r\n") );
	}


	// Delay 1s for event to get thru
	K_Task_Wait(TICK_1SEC * 3);

	// Disable FPGA watchdog
	m_pFPGA->EnableWatchdog(0);
	m_pFPGA->SetReady(0); // Not ready when rebooting.
	m_pFPGA->SetOutputSW(0, 0); // Turn off relay
	m_pFPGA->TurnAntennaPower(0); // Turn off GPS power.

	FUNC_PTR fPtr;

#if 0 // _DEBUG
	unsigned long *lPtr = (unsigned long *)0x200000;
#else
	// Release version
	unsigned long *lPtr = (unsigned long *)0;
#endif
	set_exr(7);
	fPtr = (FUNC_PTR)((*lPtr) & 0xffffff);
	(*fPtr)();


#endif

}

void CAllProcess::MarkNormalReboot(void)
{
	if (g_systemTicks < (3600L * 72 * TICK_1SEC)) {
		g_systemTicks = (3600L * 72 * TICK_1SEC);
	}
}

void CAllProcess::ReportVersion(Ccomm *pSci, int bForceIt)
{
	char buff[200];
	char const *ptr;       
#if 0
	sprintf(buff, "\r\nSM2000 STM MAIN[%s]:"
		" %d.%02d."
		"%03d\r\n"
		"Build: %s\r\n"
		"Build on: %s(%02d-%02d-%04d) "
		"%s\r\n"
		"Version String: %s\r\n"
#if _DEBUG
		,"DEBUG"
#else
		,"RELEASE"
#endif
		,(int)(VERSION_NUMBER >> 16), (int)((VERSION_NUMBER >> 8) & 0xff)
		, (int)(VERSION_NUMBER & 0xff)
		, BUILD_NAME
		, g_sBuildDate, BUILD_MONTH, BUILD_DAY, BUILD_YEAR
		, g_sBuildTime
		, VERSION_STRING
		);   
#endif   
	sprintf(buff, "\r\n%s MAIN:"
		" %d.%02d."
		"%02d%s\r\n"
		"Build on: %s "
		"%s\r\n"   
		, BUILD_NAME
		,(int)(VERSION_NUMBER >> 16), (int)((VERSION_NUMBER >> 8) & 0xff)
		, (int)(VERSION_NUMBER & 0xff)    
		#if _DEBUG
		,"D"
#else
		,"R"
#endif
		, g_sBuildDate
		, g_sBuildTime
		); 
	if (pSci) {
		if (bForceIt) pSci->ForceSend(buff);
		else pSci->Send(buff);
	}
	ptr = "Copyright of Zhejiang Saisi.\r\nAll Rights Reserved.\r\n";
	if (pSci) {
		if (bForceIt) pSci->ForceSend(ptr);
		else pSci->Send(ptr);
	}

}

uint32 CAllProcess::GetAbsoluteSecond()
{
	return g_pRTC->GetAbsoluteSecond();
}

uint32 CAllProcess::GetComputerSeconds()
{
	return g_pRTC->GetComputerSeconds();
}

void CAllProcess::SaveBaudrate()
{
	int32 debugRate = DEFAULT_BAUD0, userRate = DEFAULT_BAUD1;
	int16 debugAuto = 0, userAuto = 0;
	CDebugDL *pDL;

	if (g_pSci0) 
		debugRate = g_pSci0->GetBaudRate();
	if (g_pSci1)
		userRate = g_pSci1->GetBaudRate();

	if (m_pDebugProcess) {
		pDL = m_pDebugProcess->GetDebugger();
		debugAuto = pDL->GetAutoAllowed();
	}

	if (m_pUserProcess) {
		pDL = m_pUserProcess->GetDebugger();
		userAuto = pDL->GetAutoAllowed();
	}

	CFactory::s_pFactory->SetBaudrate(debugRate, userRate, debugAuto, userAuto);

}

void CAllProcess::InitBaudrate()
{
	int32 debugRate, userRate;
	int16 debugAuto, userAuto;
	CDebugDL *pDL;
		
	CFactory::s_pFactory->GetBaudrate(&debugRate, &userRate, &debugAuto, &userAuto);
	if (g_pSci0) 
		g_pSci0->SetBaudRate(debugRate);
	if (g_pSci1) 
		g_pSci1->SetBaudRate(userRate);    
	if (m_pDebugProcess) {
		pDL = m_pDebugProcess->GetDebugger();
		pDL->AllowAutoRate(debugAuto); 
	}   
	if (m_pUserProcess) {
		pDL = m_pUserProcess->GetDebugger();
		pDL->AllowAutoRate(userAuto);
	}

	// g_pSci1->Test();
}

void CAllProcess::SetupPtp()
{
	init_ptp();
#if ENABLE_SCI3
	{
		CPTPLink *pLink = new CPTPLink("IMC_IOC");
		g_pSci3->SetLink(pLink);
		pLink->SetDriver(g_pSci3);
		CPTPProcess *pTask = new CPTPProcess(pLink, "IMC_IOC", IMC_IOC_PROC_PRIORITY, 6004, CPTPProcess::Entry1);
		pLink->SetTaskSlot(pTask->GetSlot());
		m_pImcIocProcess = pTask;
		m_pLinkImcIoc = pLink;
	}
#endif
#if ENABLE_SCI4
	{
		CPTPLink *pLink = new CPTPLink("IOC_IOC");
		g_pSci4->SetLink(pLink);
		pLink->SetDriver(g_pSci4);
		CPTPProcess *pTask = new CPTPProcess(pLink, "IOC_IOC", IOC_IOC_PROC_PRIORITY, 6008, CPTPProcess::Entry2);
		pLink->SetTaskSlot(pTask->GetSlot());
		m_pIocIocProcess = pTask;
		m_pLinkIocIoc = pLink;
	}
#endif

}

// Return 1 if I am master.
// Otherwise return 0
int CAllProcess::AmIMaster()
{
	// Revisit the logic when card state machine is in.
	return CIstateProcess::s_pIstateProcess->IsMaster();
}

int8 CAllProcess::GetSlotId()
{
	if (P9.PORT.BIT.B0) return 1;
	return 0;
}

int8 CAllProcess::GetFpgaDone()
{
	if (P1.PORT.BIT.B3) return 1;
	return 0;
}

uint8 CAllProcess::GetHardwareRevision()
{
	uint8 ret;
	uint8 bp;

	ret = P9.PORT.BYTE;
	ret >>= 4;
	bp = P7.PORT.BYTE;
	bp <<= 3;
	return ret | (bp & 0xf0);
}


void CAllProcess::ClearStandingEvent()
{
	CEventKb *pKb;
	int16 id, start, end;
	CEvent *pEvent;

	pKb = CEventProcess::s_pEventProcess->GetEventKB();
	pKb->GetEventIdRange(&start, &end);
	for (id = start + 1; id < end; id++) {
		pEvent = pKb->GetEventStatus(id);
		if (pEvent != NULL) {
			pEvent->RegenerateEvent(1);
		}
	}
}

void CAllProcess::IstateInited()
{
	if (m_bIstateInited == 0) {
		m_bIstateInited = 1;
		MaskedPrint(0xffffffff, "Istate settled\r\n");
		CEventProcess::s_pEventProcess->ReleaseEvent(); // Let event process start
		m_pLinkImcIoc->SetEvent(COMM_RTE_EVENT_PPC_COM_ERROR);
		if (CFpga::s_pFpga->IsTwinIn() == 1) {
			m_pLinkIocIoc->SetEvent(COMM_RTE_EVENT_RTE_COM_ERROR);
		}
		CWarmup::s_pWarmup->IstateDone(); // Not an ideal way but should be good enough.
	}
}

void CAllProcess::CardFail(int index)
{
	if (m_failedOffline == 0) {
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_FAIL);
		CEventProcess::s_pEventProcess->ReportEvent(COMM_RTE_EVENT_CARD_FAIL, 1, (uint8)index);
		CBaseProcess::BroadCast(MESSAGE_CARD_FAIL, 0);
		CFpga::s_pFpga->SetReady(0);
		m_failedOffline = THIS_CARD_FAIL;
	}
}

void CAllProcess::CardOffline()
{
	if (m_failedOffline == 0) {
		m_failedOffline = THIS_CARD_OFFLINE;
		CFpga::s_pFpga->Transfer();
		SET_MY_ISTATE(COMM_RTE_KEY_CLOCK_LOOP_STATUS, 0, 0, CIstateDti::CLOCK_STAT_OFFLINE);
		
		// No more event/alarm
		ClearStandingEvent();
		CEventProcess::s_pEventProcess->ReportEvent(COMM_RTE_EVENT_CARD_OFFLINE, 1);

		CBaseProcess::BroadCast(MESSAGE_CARD_OFFLINE, 0);

	}
}

int CAllProcess::CheckDldHeader(const char *pHead)
{
	int failed = 0;
	const char *ptr;
	int lines;

	if (*pHead != 0x25) return -1;

	if (strcmp(pHead + 6, "RTE090-50321-01") != 0) {
		failed = -1;
	} else if (strcmp(pHead + 6 + HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH 
		, "M\rA\nG\r\nI\n\rC1234") != 0) { 
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
		if (strstr(pHead + 6 + HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH + HEADER_MAGIC_LENGTH
			, hardwareRev) == NULL) {
			failed = -1;
		}
#endif
	}
	return failed;

}


void CAllProcess::GraceShutDown(int grace /* = 10 */)
{
	if (m_shutdown == 0) {
		char buff[100];
		m_shutdown = grace;
		sprintf(buff, "Reboot in %d seconds\r\n", grace);
		Print(buff);
		CFpga::s_pFpga->SetReady(0);
		MarkNormalReboot();
	}
}

void CAllProcess::AllAction()
{
	CInputTod::ForceChange();
	CInput::TouchInput();
}

int CAllProcess::GetOscQL(void)
{
	return m_oscQL;
#if 0 
	int level;
	if (CFpga::s_pFpga->IsRubidium() == 1) {
		level = 3;
	} else {
		level = 4;
	}
	return level;
#endif
}

int CAllProcess::m_oscQL = 3;
void CAllProcess::SetOscQL(int level)
{
	m_oscQL = level;
}
