// Warmup.h: interface for the CWarmup class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: Warmup.h 1.7 2009/09/25 13:54:07PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.7 $
 */

#if !defined(AFX_WARMUP_H__04BEE478_2A4E_49EC_B9A0_B27F9C2EBD21__INCLUDED_)
#define AFX_WARMUP_H__04BEE478_2A4E_49EC_B9A0_B27F9C2EBD21__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "timer.h"
#include "comm.h"

class CAdcConverted;

class CWarmup  
{
public:
	void WarmupHack(unsigned long *param, Ccomm *pComm);
	static const char * GetWarmName(int state);
	int IsOvenWarm(void);
	void CfWarmupTodSource(int source);
	void CardWarmupOffline(void);
	void CardWarmupFail(void);
	void IstateDone(void);
	int IsWarm(void);
	void Init(void);
	int TickWarmup(void);
	CWarmup();
	virtual ~CWarmup();
	static CWarmup *s_pWarmup;
private:
	void SetWarmupState(int state);
	int GotTod(int clear);
	enum { WARM_INIT = 1, WARM_GPS, WARM_OSC, 
		WARM_POST_GPS,
		WARM_COMPLETE, WARM_FAIL, WARM_OFFLINE, WARM_UNKNOWN}; 
	void InitTimer(void);
	int m_leastDelay;
	int	m_isWarm;
	int m_istateInited;
	int m_todSource;
	int m_subtendLocked;
	uint32 m_twinHotCnt;
	int m_toResetBt3;
	int m_twinHotStandbyCnt;
	uint32 m_warmupStateCnt;
	CCheapTimer m_fixTimer;
	CAdcConverted *m_pAdc;
	int m_rubLockCnt;

private:
#define RUB_LOCK_MAXIMUM 3600
	int m_rubTimeout;
	void TickRubTimeout(void);

public:
	enum { INDEX_SUBSTATE = 0, INDEX_TOTAL_WARMUP = 1};
	static uint32 m_timeSpentInWarmup[2];
};

#endif // !defined(AFX_WARMUP_H__04BEE478_2A4E_49EC_B9A0_B27F9C2EBD21__INCLUDED_)
