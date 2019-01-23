// IdleProcess.h: interface for the CIdleProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IdleProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * $Header: IdleProcess.h 1.4 2009/04/29 18:54:31PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */

#if !defined(AFX_IDLEPROCESS_H__8DBC5BBE_B580_4D0F_AB24_05BB70BFEB5A__INCLUDED_)
#define AFX_IDLEPROCESS_H__8DBC5BBE_B580_4D0F_AB24_05BB70BFEB5A__INCLUDED_

#include <string.h>
#include "BaseProcess.h"

class Ccomm;
class CWatchDog;

class CIdleProcess : public CBaseProcess  
{
public:
	void PrintIdle(Ccomm *pComm);
	void Init(void);
	void MeasureProcessSpeed(void);
	void SetMark(void);
	void OneSecondCut(void);
	CIdleProcess(const char *name, Ccomm *pComm, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CIdleProcess();
	enum {RECORD_SIZE = 100};
	static CIdleProcess *s_pIdleProcess;

private:
	long GetIdleMS(int index, uint16 *pRaw = NULL, uint16 *pTimer = NULL,uint16 *pLoop = NULL);
	void entry(void);
	static uint16 const MAX_CNT;
	void IdleEntry(uint8 limited);
	static void Entry(void);
	volatile uint16 m_cnt;
	// volatile uint8 m_bStartOver;
	uint8 m_bCounting;
	uint16 m_performance[RECORD_SIZE];
	int16  m_tickDiff[RECORD_SIZE];
	int16  m_timerDiff[RECORD_SIZE];
	uint16 m_preTimer;
	int m_index;
	uint32 m_preTick;
	// For retrieving
	int m_mark;
	long m_msPer1000;
	// For debug info
	Ccomm *m_pComm;

	// Watch Dog
	enum {MAX_BUSY_PERIOD = 120 };
	int volatile m_watchDogTied;

	volatile int m_i;
	volatile int m_j;
};

#endif // !defined(AFX_IDLEPROCESS_H__8DBC5BBE_B580_4D0F_AB24_05BB70BFEB5A__INCLUDED_)
