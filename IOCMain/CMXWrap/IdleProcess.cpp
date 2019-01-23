// IdleProcess.cpp: implementation of the CIdleProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IdleProcess.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * $Header: IdleProcess.cpp 1.4 2009/04/30 16:31:15PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 * Don't do any optimization on this file.
 */

#include "machine.h"
#include "CodeOpt.h"  
#include "string.h"
#include "IdleProcess.h"
#include "timer.h"
#include "comm.h"    
#include "stdio.h" 
#include "iodefine.h"
#include "AllProcess.h"
#include "g_cmx.h"
#include "RTC.h"
#include "WatchDog.h"
#include "NewCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIdleProcess * CIdleProcess::s_pIdleProcess = NULL;
uint16 const CIdleProcess::MAX_CNT = 2000;

CIdleProcess::CIdleProcess(const char *name, Ccomm *pComm, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	volatile int i, j;

	memset(m_performance, 0, sizeof(m_performance));
	memset(m_tickDiff, 0, sizeof(m_tickDiff));
	m_preTimer = 0;
	m_i = m_j = 0;
	m_mark = 0;
	s_pIdleProcess = this;
	
	m_watchDogTied = MAX_BUSY_PERIOD; 

	s_pIdleProcess = this;
	m_cnt = 0;
	// m_bStartOver = 0;
	m_bCounting = 0;
	m_index = 0;
	m_pComm = pComm;

	// Set how quick the processor is
	m_msPer1000 = 1400;

	m_preTick = 0;
	memset(m_timerDiff, 0, sizeof(m_timerDiff));
}

CIdleProcess::~CIdleProcess()
{

}

void CIdleProcess::Entry()
{
	s_pIdleProcess->entry();
}

void CIdleProcess::entry(void)
{       
#if MEASURE_CPU_SPEED
	// s_pIdleProcess->Pause(TICK_1SEC * 1);
	TMR3.TCR.BIT.CMIEA = 0; // disable RTOS tick
  	INTC.IPRL.BIT.HIGH = 7;  // Set TMR2,TMR3 Priority Level. Temporarily make it highest
	TMR3.TCR.BIT.CMIEA = 1; // enable RTOS tick
	SetPriority(CAllProcess::FIRST_PROC_PRIORITY);
	Pause(TICK_1SEC/4);
	MeasureProcessSpeed();
	SetPriority(CAllProcess::IDLE_PROC_PRIORITY);
	// Make it in SetPriority
	//s_pIdleProcess->Pause(TICK_1SEC);
	
	// Should be done in CAllProcess::Start() if not measuring speed
  	INTC.IER.BIT.IRQ4E   = 1;  /* ZM: enable(1) 25msec interrupt */

#endif

	
	TMR3.TCR.BIT.CMIEA = 0; // disable RTOS tick
  	INTC.IPRL.BIT.HIGH = INT_LEVEL_OS_TIMER;  // Set TMR2,TMR3 Priority Level
	TMR3.TCR.BIT.CMIEA = 1; // enable RTOS tick

	// Start watch dog
#if ENABLE_WATCHDOG
	CWatchDog::ClearWatchDog();
#endif

	// Get to know current code size
	CNewCode::s_pNewCode->DetectCodeSize();

	// Go to Idle loop
	SetPriority(CAllProcess::IDLE_PROC_PRIORITY);
	m_bCounting = 1;
	for (;;) {
		m_cnt = 0;
		IdleEntry(1);
	}
	// Dummy code
	SetPriority(CAllProcess::IDLE_PROC_PRIORITY);
}

void CIdleProcess::IdleEntry(uint8 limited)
{
	//volatile int i;
	//volatile int j; // Key: has to be defined as class member in case this file is optimized.
	// uint8 * const pOutput = (uint8 *)0x40002f;
	uint8 tmp_exr;

	do {
		while (1) {
			m_watchDogTied = MAX_BUSY_PERIOD;
			for (m_i = 0; m_i < 31; m_i++) {
				for (m_j = 0; m_j < 8; m_j++) {
					//nop();
					CWatchDog::ClearWatchDog();
				}
#if 0
				*pOutput = m_i & 1;
#endif
			}
			tmp_exr = get_exr();
			set_exr(7);
			m_cnt++;
			if (m_cnt >= MAX_CNT) {
				set_exr(tmp_exr);
				break;
			}
			set_exr(tmp_exr);
		}
	} while (!limited); 	 
}

void CIdleProcess::OneSecondCut()
{   
	uint32 tick;
	uint16 timer;

	if (m_bCounting) {
		timer = TMRA.TCNT;
		m_index %= RECORD_SIZE;
		m_timerDiff[m_index] = (timer - m_preTimer) * 8192L / (SYST_CLK / 1000);
		m_preTimer = timer;
		m_performance[m_index] = m_cnt;
		m_cnt = 0;
		tick = GetRawTick();
		m_tickDiff[m_index] = tick - m_preTick;
		m_preTick = tick;
		m_index++;
	}
	if (m_watchDogTied) {
		CWatchDog::ClearWatchDog();
		m_watchDogTied--;
	}
}

void CIdleProcess::SetMark()
{
	m_mark = m_index;
	return;
}


long CIdleProcess::GetIdleMS(int index, uint16 *pRaw /*= NULL */, 
							 uint16 *pTimer /* = NULL */, uint16 *pLoop /*= NULL*/)
{
	uint16 idle;
	long ret;
	index = (m_mark + RECORD_SIZE - 1 - index) % RECORD_SIZE;
	idle = m_performance[index];
	ret = m_msPer1000 * (long)idle / (long)MAX_CNT;
#if 0
	ret = (long)idle;
#endif
	if (pRaw) *pRaw = m_tickDiff[index];
	if (pLoop) *pLoop = idle;
	if (pTimer) *pTimer = m_timerDiff[index];
	return ret;
}

void CIdleProcess::MeasureProcessSpeed()
{   
#if MEASURE_CPU_SPEED
	char buff[150];
	uint8 tmp_exr;
	unsigned long start_point, end_point;
	//uint8 volatile *pFPGAClock = (uint8 *)0x400013;
	uint16 time0, time1;
	int32 diff;

	g_cmx_started = 0;
	TMR2.TCNT = 0; TMR3.TCNT = 0;
	tmp_exr = get_exr();
	set_exr(7);
	start_point =  GetRawTick();
	//TMRA.TCNT = 0;
	m_cnt = 0;
	time0 = TMRA.TCNT; //*pFPGAClock;
	IdleEntry(1);
	time1 = TMRA.TCNT; //*pFPGAClock;
	end_point =  GetRawTick();
	g_cmx_started = 1;
	m_msPer1000 = (end_point - start_point) * 1000 / TICK_1SEC;
	//if (m_msPer1000 < 1111) { // Should be 1880 per MAX_CNT (used to 1000. now it's 4000
	//	m_msPer1000 = 1111;
	//}
	set_exr(tmp_exr);
	
	if (time1 > time0) {
		diff = time1 - time0;
	} else {
		diff = (65536L - time0 + time1);
	}

	diff = diff * 8192 / (SYST_CLK / 1000);

	// Use FPGA time
	m_msPer1000 = diff;

	if (m_pComm) {
		sprintf(buff, "%ld ms per %d Time0=%d Time1=%d TimeDiff=%ldms\r\n", 
			m_msPer1000, MAX_CNT, time0, time1, diff);
		m_pComm->ForceSend(buff);		
	}	
#endif
}

void CIdleProcess::Init()
{

}

void CIdleProcess::PrintIdle(Ccomm *pComm)
{
	char buff[200];  
	int i;
	uint16 loop;
	uint16 timer;

	// Idle time
	SetMark();
	sprintf(buff, "%ldms per %d\r\n", m_msPer1000, MAX_CNT);
	pComm->Send(buff);
	for (i = 0; i < RECORD_SIZE - 20; i++) {
		uint16 raw;
		long idle = GetIdleMS(i, &raw, &timer, &loop);
	
		if ( (i % 4) == 0) {
			pComm->Send("\r\n");
		}
		sprintf((char *)buff, "%04ld(%02d -L%04d T=%d) ", idle, raw, loop, timer);			
		pComm->Send((char *)buff);
	}
}
