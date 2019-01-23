// RTC.h: interface for the CRTC class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: RTC.h 1.2 2009/04/29 18:52:02PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */
#if !defined(AFX_RTC_H__F598CDC0_6174_42DB_A593_48450BF914F3__INCLUDED_)
#define AFX_RTC_H__F598CDC0_6174_42DB_A593_48450BF914F3__INCLUDED_

#include "DataType.h"

#ifdef __cplusplus
class CRTC  
{
public:
	uint32 GetMjd(void);
	void SetGpsLocal(int32 seconds);
	void SetComputerSeconds(uint32 seconds);
	unsigned long GetComputerSeconds(void);
	uint32 GetAbsoluteSecond(void);
	void SetGPSSeconds(uint32 seconds);
	int SetDateTime(int year, int month, int day, int hour, int minute, int second);
	void GetDateTime(int *year, int *month, int *day, int *hour, int *minute, int *second);
	unsigned long GetGPSSeconds(void);
	void SetTime(uint32 seconds);
	void TickOfOneSecond(void);
	CRTC();
	virtual ~CRTC();
	static int DaysOfMonth(int year, int month);
private:
	static int IsLeapYear(int year);
	int DaysOfYear(int year);
	volatile uint32 m_seconds_1970; // seconds since 1/1/1970
	uint32 volatile m_absoluteSecond;
};     

extern CRTC *g_pRTC;
#endif

EXTERN void RTOS_TICK(void);
EXTERN uint32 GetRawTick(void);

#endif // !defined(AFX_RTC_H__F598CDC0_6174_42DB_A593_48450BF914F3__INCLUDED_)
