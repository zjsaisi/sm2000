// SecondProcess.h: interface for the CSecondProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: SecondProcess.h 1.3 2008/05/28 18:16:54PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
 */

#if !defined(AFX_SECONDPROCESS_H__270B797A_B15D_4AB1_B153_AA38E9A1C73D__INCLUDED_)
#define AFX_SECONDPROCESS_H__270B797A_B15D_4AB1_B153_AA38E9A1C73D__INCLUDED_

#include "BaseProcess.h"


class CSecondProcess : public CBaseProcess  
{
public:
	void ArmTodJam(void);
	void CalculateLeapTime(void);
	void ApplyTodSource(int tod);
	void SetTimeSettingMode(int mode);
	void SetTodValid(int valid);
	void CfOutputTodMode(int mode);
	void UpdateRtc(void);
	void CfTodSource(int tod);
	void CfLeapPending(int pending);
	void CfTotalLeapSecond(int32 totalLeap);
	void CfLocalTimeZone(int32 second);
	void HackSecondProcess(unsigned long *param, Ccomm * pComm);
	void TickOneSecondIsr(void);
	void Init(void);
	CSecondProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CSecondProcess();
	static CSecondProcess *s_pSecondProcess;
	int GetTodSource(void) { return m_todSource; };
private:
	void CheckRTC(void);
	void MonitorTod(void);
	void BuildTimeStamp(void);
	void GenerateAsciiTimeStamp(uint32 gpsSecond, int leaping);
	void TickTasks(void);
	enum { EVT_ONE_SECOND = 1};
	static void Entry(void);
	int m_second;
	int32 m_timeZone;
	uint32 m_timeStamp;
	char m_timeZoneStr[10];
	uint32 volatile m_leapTime;
	char m_asciiTimeStamp[50]; // make sure it's aligned to word boundary
	uint8 m_timeSettingMode;
	int volatile m_totalLeap;
	int volatile m_leapPending;
	int m_timeStampDebug;
	int m_todSource;
	int m_effectiveTodSource;
	int m_todSentTime;
	int m_updateRtc;
	int volatile m_todJamArmCnt;
protected:
	virtual void EventDispatch(uint16 event);
};

#ifndef TRACE_SECOND
#define TRACE_SECOND CSecondProcess::s_pSecondProcess->PrintDebugOutput
#endif

#endif // !defined(AFX_SECONDPROCESS_H__270B797A_B15D_4AB1_B153_AA38E9A1C73D__INCLUDED_)
