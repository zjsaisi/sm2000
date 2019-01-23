// SecondProcess.cpp: implementation of the CSecondProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: SecondProcess.cpp 1.12 2010/10/14 00:18:38PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.12 $
 */

#include "SecondProcess.h"
#include "string.h"
#include "OutputProcess.h"
#include "MinuteProcess.h"
#include "rtc.h"
#include "machine.h"
#include "stdio.h"
#include "Fpga.h"
#include "comm.h"
#include "IstateProcess.h"
#include "AllProcess.h"
#include "IstateDti.h"
#include "machine.h"
#include "CardStatus.h"
#include "BT3Api.h"
#include "IdleProcess.h"
#include "CardStatus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACE TRACE_SECOND

#define TOD_STRING_OFFSET 6
#define TOD_LEAP_SECOND_OFFSET 5
#define TOD_GPSSEC_OFFSET 1
#define TOD_STATUS_OFFSET 0

CSecondProcess *CSecondProcess::s_pSecondProcess = NULL;

CSecondProcess::CSecondProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	m_timeStamp = 0L;
	m_totalLeap = 0;

	s_pSecondProcess = this;
	m_second = 0;
	m_timeZone = 0L;
	m_leapPending = 0;
	m_leapTime = 0L;
	m_timeStampDebug = 0;
	m_todSource = -1;
	memset(m_asciiTimeStamp, 0, sizeof(m_asciiTimeStamp));
	m_asciiTimeStamp[TOD_STATUS_OFFSET] = 1;
	strcpy(m_timeZoneStr, "+00.0");
	m_effectiveTodSource = -1;
	m_timeSettingMode = 0;
	m_todSentTime = -1;
	m_updateRtc = 10;
	m_todJamArmCnt = 0;
}

CSecondProcess::~CSecondProcess()
{

}

void CSecondProcess::Entry()
{
	s_pSecondProcess->EventEntry(TICK_1SEC * 3);	
}

void CSecondProcess::Init()
{

}

void CSecondProcess::EventDispatch(uint16 event)
{
	CBaseProcess::EventDispatch(event);
	if (event & EVT_ONE_SECOND) {
		CIdleProcess::s_pIdleProcess->OneSecondCut();
		TickTasks();
	}
}

void CSecondProcess::TickOneSecondIsr()
{
	SignalEventIsr(EVT_ONE_SECOND);
}

void CSecondProcess::TickTasks()
{

	COutputProcess::s_pOutputProcess->TickOutputProcess();

	if (++m_second >= 60) {
		m_second = 0;
		CMinuteProcess::s_pMinuteProcess->TickMinuteProcess();
	} else if (m_second == 20) {
		CMinuteProcess::s_pMinuteProcess->TickHalfMinute();
	}

	CBT3Api::BT3TickOneSecond();

	CheckRTC();

	// TRACE("SecondTask %d\r\n", m_second);
#if !TO_BE_COMPLETE

	MonitorTod();

	BuildTimeStamp();
	
	// Send TOD message
	CFpga::s_pFpga->SendTod(m_asciiTimeStamp);
	m_todSentTime = CFpga::s_pFpga->GetTick();

	COutputProcess::s_pOutputProcess->TickOutputProcess();
	if (++m_second >= 60) {
		m_second = 0;
		CMinuteProcess::s_pMinuteProcess->TickMinuteProcess();
	} else if (m_second == 20) {
		CMinuteProcess::s_pMinuteProcess->TickHalfMinute();
	}

	CBT3Api::BT3TickOneSecond();

	if (m_todJamArmCnt) {
		m_todJamArmCnt--;
		if (m_todJamArmCnt == 0) {
			CFpga::s_pFpga->SetTodArm(0);
		}
	}
#endif
}

void CSecondProcess::HackSecondProcess(unsigned long *param, Ccomm *pComm)
{
	char buff[150];
	if (*param == 1) {
		switch (param[1]) {
		case 1:
			sprintf(buff, "TimeStamp String(@0x%lx) %ld(0x%lX): 0x%02X\r\n", 
				m_asciiTimeStamp, m_timeStamp, m_timeStamp, m_asciiTimeStamp[TOD_LEAP_SECOND_OFFSET]);
			pComm->Send(buff);
			pComm->Send(m_asciiTimeStamp + TOD_STRING_OFFSET);
			pComm->Send("\r\n");
			sprintf(buff, "Leap(%d) @0x%lX\r\n"
				"TOD Sent on %d tick\r\n"
				, m_leapPending, m_leapTime
				, m_todSentTime);
			pComm->Send(buff);
			sprintf(buff, "UpdateRTC: %d\r\n"
				, m_updateRtc);
			pComm->Send(buff);
			sprintf(buff, "TodSrc:%d  Effective one:%d\r\n"
				, m_todSource, m_effectiveTodSource);
			pComm->Send(buff);
			return;
		}
	}
	if (*param == 2) {
		switch (param[1]) {
		case 2:
			m_timeStampDebug = param[2];
			return;
		}
	}

	char const *pHelp = "Syntax: <option>\r\n"
		"\t1: Display Time Stamp for output\r\n"
		"\t2 <on/off>: Enable/Disable timestamp debug\r\n"
		;
	pComm->Send(pHelp);
}

void CSecondProcess::GenerateAsciiTimeStamp(uint32 gpsSecond, int leaping)
{
	int32 localGps;
	uint8 tmp_exr;
	CRTC rtc;
	char *ptr;
	int year, month, day, hour, minute, second;
	char leapMark = '0';
	int  moreSecond = 0;

	tmp_exr = get_exr();
	set_exr(7);
	localGps = gpsSecond + m_timeZone - m_totalLeap;
	m_timeStamp = gpsSecond;
	set_exr(tmp_exr);

	if (leaping <= -1) {
		leapMark = '-';
		if (leaping == -1) {
			localGps++;
			// If we want leap flag cleared @ 0:0:0 of next month
			m_totalLeap--;
			m_leapPending = 0;
			leapMark = '0';
			m_updateRtc = 3;
		}
	} else if (leaping >= 1) {
		leapMark = '+';
		if (leaping == 1) {
			moreSecond = 1;
			localGps--;
			m_updateRtc = 3;
		}
	}
	
	rtc.SetGpsLocal(localGps);
	
	rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);
	
	second += moreSecond;

	ptr = m_asciiTimeStamp + TOD_STRING_OFFSET;
	sprintf(ptr, "*%05ld."
		"%04d/%02d/%02d.%02d:%02d:%02d."
		"%s.%c\015"
		, rtc.GetMjd()
		, year, month, day, hour, minute, second
		, m_timeZoneStr, leapMark
	);
	m_asciiTimeStamp[TOD_LEAP_SECOND_OFFSET] = m_totalLeap;

	memcpy(m_asciiTimeStamp + TOD_GPSSEC_OFFSET, &gpsSecond, 4);

}

void CSecondProcess::CheckRTC(void)
{
	if (m_updateRtc) {
		uint32 time;
		m_updateRtc--;
		time = CFpga::s_pFpga->GetAdjustedGpsSecond();
		g_pRTC->SetGPSSeconds(time);
	}
}

void CSecondProcess::CfLocalTimeZone(int32 second)
{
	uint8 tmp_exr;
	int hr;
	int half = 0;
	char buff[10];

	hr = (int)(second / 3600L);
	if (second % 3600) {
		half = 5;
	}
	if (second >= 0) {
		sprintf(buff, "+%02d.%d", hr, half); // %+03d won't work
	} else {
		sprintf(buff, "-%02d.%d", -hr, half);
	}

	tmp_exr = get_exr();
	set_exr(7);
	m_timeZone = second;
	strcpy(m_timeZoneStr, buff);
	set_exr(tmp_exr);
}

void CSecondProcess::CfTotalLeapSecond(int32 totalLeap)
{
	if (m_totalLeap == totalLeap) return;
	m_totalLeap = totalLeap;
	CalculateLeapTime();
}

void CSecondProcess::CfLeapPending(int pending)
{
	if (m_leapPending == pending) return;

	m_leapPending = pending;
	CalculateLeapTime();
}

void CSecondProcess::CalculateLeapTime()
{
	uint32 utc;
	CRTC rtc;
	int year, month, day, hour, minute, second;
	uint32 theTime;
	uint8 tmp_exr;

	if (m_leapPending == 0) {
		tmp_exr = get_exr();
		set_exr(7);
		m_leapTime = 0L;
		set_exr(tmp_exr);
		return;
	}
	utc = CFpga::s_pFpga->GetGpsSec() - m_totalLeap;
	rtc.SetGpsLocal(utc);
	rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);
	day = CRTC::DaysOfMonth(year, month);
	hour = 23;
	minute = 59;
	second = 59;
	rtc.SetDateTime(year, month, day, hour, minute, second);
	utc = rtc.GetGPSSeconds();
	if (m_leapPending > 0) {
		theTime = utc + m_totalLeap + 1;
	} else if (m_leapPending < 0) {
		theTime = utc + m_totalLeap;
	}

	tmp_exr = get_exr();
	set_exr(7);
	m_leapTime = theTime;
	set_exr(tmp_exr);
}

void CSecondProcess::BuildTimeStamp()
{
	uint32 gpsSec;

	gpsSec = CFpga::s_pFpga->GetGpsSec();
	if (m_leapTime == gpsSec) {
		GenerateAsciiTimeStamp(gpsSec, m_leapPending);
		//TRACE("Damn\r\n");
		m_totalLeap += m_leapPending;
		m_leapPending = 0;
		SET_MY_ISTATE(COMM_RTE_KEY_TOTAL_LEAP_SECOND, 0, 0, m_totalLeap);
		SET_MY_ISTATE(COMM_RTE_KEY_LEAP_PENDING, 0, 0, m_leapPending);
	} else {
		if (m_leapPending == 0) {
			GenerateAsciiTimeStamp(gpsSec, 0);
		} else if (m_leapPending > 0) {
			GenerateAsciiTimeStamp(gpsSec, 2);
		} else if (m_leapPending < 0) {
			GenerateAsciiTimeStamp(gpsSec, -2);
		} 
	}
	if (m_timeStampDebug) {
		char buff[100];
		sprintf(buff, "GPS timestamp: 0x%lX %ld:", gpsSec, gpsSec);
		CAllProcess::g_pAllProcess->DumpHex(buff, (unsigned char *)m_asciiTimeStamp,
			TOD_STRING_OFFSET);
		CAllProcess::g_pAllProcess->Print(m_asciiTimeStamp + 1 + 4);
		CAllProcess::g_pAllProcess->Print("\r\n");
	}

}

void CSecondProcess::SetTimeSettingMode(int mode)
{
	mode = (mode << 4) & 0xf0;
	if (m_timeSettingMode == mode) return;

	m_timeSettingMode = mode;

	m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
	m_asciiTimeStamp[TOD_STATUS_OFFSET] |= m_timeSettingMode;

}

void CSecondProcess::ApplyTodSource(int tod)
{
	uint8 tmp_exr;
    

	if (IS_ACTIVE) {
		SET_MY_ISTATE(COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE, 0, 0, tod);	
	}

	m_effectiveTodSource = tod;

	tmp_exr = get_exr();
	set_exr(7);
	switch (tod) {
	case CIstateDti::TOD_SOURCE_RTC:
		m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
		// m_asciiTimeStamp[TOD_STATUS_OFFSET] |= 0x00;
		m_timeSettingMode = 0;
		break;
	case CIstateDti::TOD_SOURCE_USER:
		m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
		m_asciiTimeStamp[TOD_STATUS_OFFSET] |= 0x10;
		m_timeSettingMode = 0x10;
		break;
	case CIstateDti::TOD_SOURCE_NTP:
		m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
		m_asciiTimeStamp[TOD_STATUS_OFFSET] |= 0x20;
		m_timeSettingMode = 0x20;
		break;
	case CIstateDti::TOD_SOURCE_DTI:
		// Should be decided by DTI input
		// Set it to m_timeSettingMode
		m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
		m_asciiTimeStamp[TOD_STATUS_OFFSET] |= m_timeSettingMode;
		break;
	case CIstateDti::TOD_SOURCE_HYBRID:  //GPZ June 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_GPS:
		m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf;
		m_asciiTimeStamp[TOD_STATUS_OFFSET] |= 0x30;
		m_timeSettingMode = 0x30;
		break;
	default:
		break;

	}
	set_exr(tmp_exr);
}

void CSecondProcess::CfTodSource(int tod)
{	
	if (m_todSource == tod) {
		return;
	}

	m_todSource = tod;
	if (tod == CIstateDti::TOD_SOURCE_DTI) {
		ApplyTodSource(tod);
		return;
	}

	if (m_effectiveTodSource < 0) {
		ApplyTodSource(tod);
		return;
	}

	// Wait for next Jam.
}

void CSecondProcess::SetTodValid(int valid)
{
	uint8 tmp_exr;
	uint8 todValid;

	todValid = (valid) ? (4) : (0);
	tmp_exr = get_exr();
	set_exr(7);
	m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xf3;
	m_asciiTimeStamp[TOD_STATUS_OFFSET] |= todValid;
	set_exr(tmp_exr);
}

void CSecondProcess::UpdateRtc()
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	if (m_updateRtc < 10) {
		m_updateRtc = 10;
	}
	set_exr(tmp_exr);
}

void CSecondProcess::MonitorTod()
{
	if (!IS_WARMUP) {
		if (m_todSource != CIstateDti::TOD_SOURCE_DTI) {
			if (CFpga::s_pFpga->GotImcJam(1)) {
				//UpdateRtc();
				if (IS_ACTIVE) {
					ApplyTodSource(m_todSource);
					SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_CALCULATE_LEAP_TIME);
					//CalculateLeapTime();
				}
				SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_SYNC_RTC_FROM_FPGA);
				TRACE("Detect PPC Tod Jam\r\n");

				CFpga::s_pFpga->SetTodArm(0);

			}
		} else {
			// From DTI input
		}
	} else if ((m_second % 10) == 5) {
		if (IS_ACTIVE) {
			ApplyTodSource(m_todSource);
		} 
	}
}

void CSecondProcess::CfOutputTodMode(int mode)
{
	uint8 tmp_exr;
	uint8 mode8 = 1;

	if (mode == CIstateDti::OUTPUT_TOD_SHORT) {
		mode8 = 0;
	}

	tmp_exr = get_exr();
	set_exr(7);

	m_asciiTimeStamp[TOD_STATUS_OFFSET] &= 0xfc;
	m_asciiTimeStamp[TOD_STATUS_OFFSET] |= mode8;
	
	set_exr(tmp_exr);

}

void CSecondProcess::ArmTodJam()
{
	m_todJamArmCnt = 10;
	CFpga::s_pFpga->SetTodArm(1);
	UpdateRtc();
}
