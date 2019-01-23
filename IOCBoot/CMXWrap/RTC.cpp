// RTC.cpp: implementation of the CRTC class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Real Time Clock management
 * $Header: RTC.cpp 1.3 2009/05/05 15:41:53PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
 *
 */
//#include "zmdefine.h"
#include "iodefine.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "cxfuncs.h"
#endif
#include "iodefine.h"     
#include "DataType.h"
#include "RTC.h"
#include "machine.h"
#include "g_cmx.h"
#include "Constdef.h"
#include "AllProcess.h"
#include "PTPIsr.h"
#include "sci.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#pragma section RTC           
static volatile unsigned long g_previousTime;
static volatile unsigned long _g_previousTime;
volatile unsigned long g_systemTicks;  // Don't move this to .h file. 
									// Otherwise the sequence will be changed.
#pragma section

CRTC *g_pRTC;

extern unsigned char g_appFailCnt;

CRTC::CRTC()
{
	if (g_previousTime == ~_g_previousTime) {
		m_seconds_1970 = g_previousTime;
	} else {
		m_seconds_1970 = 315964800L; // 1/6/1980
		g_systemTicks = 0L;
		g_appFailCnt = 0;
	}
	m_absoluteSecond = 0L;
}

CRTC::~CRTC()
{

}

void CRTC::Tick()
{
	unsigned char temp_exr = get_exr();
	set_exr(7);
	m_seconds_1970++;
	g_previousTime = m_seconds_1970;
	_g_previousTime = ~g_previousTime;
	m_absoluteSecond++;
	set_exr(temp_exr);
}

uint32 CRTC::GetAbsoluteSecond()
{
	uint32 ret;
	unsigned char temp_exr = get_exr();
	set_exr(7);
	ret = m_absoluteSecond;
	set_exr(temp_exr);
	return ret;
}

void CRTC::SetTime(uint32 seconds)
{
	m_seconds_1970 = seconds;
}

void CRTC::SetGPSSeconds(uint32 seconds)
{
	m_seconds_1970 = seconds + 315964800L;
}

unsigned long CRTC::GetGPSSeconds()
{
	uint32 sec;
	sec = m_seconds_1970;
	if (sec < 315964800L) {
		return 0L;
	}
	return (sec - 315964800L);
}


void CRTC::GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second)
{
	uint32 cur_time = m_seconds_1970;
	uint32 total_days;
	uint32 seconds_in_day;
	seconds_in_day = cur_time % 86400L;
	total_days = cur_time / 86400L;
	*second = (int)(seconds_in_day % 60L);
	seconds_in_day /= 60L;
	*minute = (int)(seconds_in_day % 60L);
	*hour = (int)(seconds_in_day / 60L);

	int cur_year = 1970;
	{
		int day_in_year;
		// Find the year
		do {
			day_in_year = DaysOfYear(cur_year);
			if (total_days >= day_in_year) {
				total_days -= day_in_year;
				cur_year++;
			} else {
				break;
			}
		} while (1);
		*year = cur_year;
	}

	{
		// Find month
		int cur_month = 1;
		int day_in_month;
		do {
			day_in_month = DaysOfMonth(cur_year, cur_month);
			if (total_days >= day_in_month) {
				total_days -= day_in_month;
				cur_month++;
			} else {
				break;
			}
		} while (1);
		*month = cur_month;
	}
	*day = total_days + 1;
}

int CRTC::DaysOfYear(int year)
{
	return (IsLeapYear(year) + 365);
}

int CRTC::IsLeapYear(int year)
{
	while ((year % 100) == 0) {
		year /= 100;
	}
	if (year % 4) {
		return 0;
	} else {
		return 1;
	}
}

int CRTC::DaysOfMonth(int year, int month)
{
	static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (month == 2) {
		return (28 + IsLeapYear(year));
	}
	return (days[month-1]);
}

int CRTC::SetDateTime(int year, int month, int day, int hour, int minute, int second)
{
	uint32 cur_time;
	uint32 total_days;
	
	// Year must be between 1970, 2037
	if ((year < 1970)||(year > 2037)) return -1;
	if ((month < 1) || (month > 12)) return -1;
	if (day < 1) return -1;
	if (day > DaysOfMonth(year, month)) return -1;
	if ((hour < 0) || (hour > 23)) return -1;
	if ((minute < 0) || (minute > 59)) return -1;
	if ((second < 0) || (second > 59)) return -1;

	cur_time = (unsigned long)hour * 3600 + (unsigned long)minute * 60 + second;
	total_days = (unsigned long)day - 1;
	// Add days in previous month
	for (int i = 1; i < month; i++) {
		total_days += (unsigned long)DaysOfMonth(year, i);
	}
	// Add days in previous year
	for (i = 1970; i < year; i++) {
		total_days += (unsigned long)DaysOfYear(i);
	}
	cur_time += total_days * 86400L;
	SetTime(cur_time);
	return 1;
}


extern "C" void RTC_TICK(void)
{
	g_pRTC->Tick();
}

extern "C" unsigned long GetGPSSeconds(void)
{
	return g_pRTC->GetGPSSeconds();
}


extern "C" void SetGPSSeconds(unsigned long seconds)
{
	g_pRTC->SetGPSSeconds(seconds);
}

// Called by ISR of RTOS tick
EXTERN void RTOS_TICK(void)
{
	static int16 cnt = 0; 

	if (TMR3.TCSR.BIT.CMFA) {
		TMR3.TCSR.BIT.CMFA = 0;
	}   
	g_systemTicks++;           
	cnt++;

	if (g_pSci4) {
		g_pSci4->CheckInput();
	}
	if (g_pSci3) {
		g_pSci3->CheckInput();
	}
	if (g_pSci0) {
		g_pSci0->CheckInput();
	}
	if (g_pSci1) {
		g_pSci1->CheckInput();
	}

	if ( cnt >= TICK_1SEC) {
		// One second
		cnt = 0;     
		g_pAll->SecondTick();
	}
	 
	if (!g_cmx_started) return;
	K_OS_Tick_Update();

#if 0
	//TMR0.TCSR.BIT.CMFA = 0;
	if (TMR3.TCSR.BIT.CMFA) {
		TMR3.TCSR.BIT.CMFA = 0;
	}   
	g_systemTicks++; 

	K_OS_Tick_Update();
#endif	
}

EXTERN uint32 GetRawTick(void)
{
	uint32 ret;
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	ret = g_systemTicks;
	set_exr(tmp_exr);
	return ret;

}
