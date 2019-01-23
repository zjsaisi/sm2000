// CardLoop.h: interface for the CCardLoop class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardLoop.h 1.5 2009/05/01 09:29:42PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 */

#if !defined(AFX_CARDLOOP_H__D41B35C6_AA3A_4BC5_BCAE_B20D6D135B18__INCLUDED_)
#define AFX_CARDLOOP_H__D41B35C6_AA3A_4BC5_BCAE_B20D6D135B18__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Constdef.h"
#include "string.h"
#include "comm.h"

class CCardLoop  
{
public:
	void CfBridgeTime(uint32 time);
	void CardLoopHack(Ccomm *pComm);
	void CfTodSource(int source);
	static const char * GetName(int state);
	int GetTwinLoop(int *pFresh = NULL) const;
	void SetTwinLoop(int loop);
	void CfMaxHoldTime(int32 time);
	void CardLoopOffline(void);
	static CCardLoop *s_pCardLoop;
	int GetCardLoop(void) const;
	void Init(void);
	void CardLoopFail(void);
	int TickCardLoop(void);
	CCardLoop();
	virtual ~CCardLoop();
	enum { CARD_FREQ_UNKNOWN = 1, 
		CARD_FREQ_WARMUP = 0x20, CARD_FREQ_FREERUN, CARD_FREQ_FAST_TRACK, 
		CARD_FREQ_NORMAL_TRACK,
		CARD_FREQ_BRIDGE, CARD_FREQ_HOLDOVER, CARD_FREQ_EXTENDED_HOLDOVER, 
		CARD_FREQ_FAIL,
		CARD_FREQ_OFFLINE
	};
private:
	void MonitorFineGpsPhase(void);
	void GetGpsModeGear(void);
	void MonitorGpsPhase(int clear);
	void TransitLoop(int loop);
	int m_todSource;
	void TickExtendedHoldover(void);
	void TickOffline(void);
	void ExitLoopState(int state, int to);
	void TickFail(void);
	void TickHoldover(void);
	void TickBridge(void);
	void TickNormalLock(void);
	void TickFastLock(void);
	void TickFreerun(void);
	void TickWarmup(void);
	void SetLoopState(int state, int from);
	int m_loopState;
	int m_cardStatus;
	int m_twinLoop;
	int m_twinLoopFresh;
	int32 m_maxHoldTime;
	int32 m_maxBridgeTime;
	int32 m_bridgeTime;
	int32 m_loopCnt;
	int m_gpsPhaseErrorOverLimit;
	int m_gpsPhaseFine;
	int m_bt3Mode;
	int m_bt3Gear;
};

#endif // !defined(AFX_CARDLOOP_H__D41B35C6_AA3A_4BC5_BCAE_B20D6D135B18__INCLUDED_)
