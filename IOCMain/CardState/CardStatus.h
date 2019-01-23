// CardStatus.h: interface for the CCardStatus class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardStatus.h 1.5 2009/05/01 09:18:56PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 * State Machine for active/standby
 */

#if !defined(AFX_CARDSTATUS_H__509AE203_24BE_4426_A24C_5AE3783D00A9__INCLUDED_)
#define AFX_CARDSTATUS_H__509AE203_24BE_4426_A24C_5AE3783D00A9__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Constdef.h"
#include "string.h"
#include "comm.h"

class CFpga;

class CCardStatus  
{
public:
	uint32 GetCardStatusCnt(void) const;
	void CardStatusHack(Ccomm *pComm);
	void CfTodSource(int tod);
	static const char * GetName(int state);
	int IsWarmup(void);
	void SetTwinStatus(int stat);
	static int GetCardStatus(void);
	int IsCardStandby(void);
	void CardStatusOffline(void);
	int IsCardActive(void);
	void Init(void);
	void CardStatusFail(void);
	unsigned int TickCardStatus(int isWarm = 1);
	CCardStatus();
	virtual ~CCardStatus();
	static CCardStatus *s_pCardStatus;
	enum { CARD_UNKNOWN = 0x1, 
		CARD_WARMUP = 0x10, CARD_HOT_ACTIVE, CARD_HOT_STANDBY, 
		CARD_FAIL, CARD_OFFLINE, CARD_COLD_ACTIVE, CARD_COLD_STANDBY};
	int GetTwinStatus(int *pFresh = NULL) const;
private:
	int TickHotActive(int active);
	void TickHotStandby(void);
	void ExitStatusState(int state, int new_state);
	void SetStatusState(int state);
	int m_state;
public:
	uint32 GetCardStateLife(void) { return m_stateCnt; } ;
private:
	uint32 m_stateCnt;
	int volatile m_twinState;
	int m_twinStateFresh;
	int m_todSource;
	CFpga *m_pFpga;
private:
	int m_isXsyncOk;
};

#ifndef IS_ACTIVE
#define IS_ACTIVE CCardStatus::s_pCardStatus->IsCardActive()
#endif

#ifndef IS_STANDBY
#define IS_STANDBY CCardStatus::s_pCardStatus->IsCardStandby()
#endif

#ifndef IS_WARMUP
#define IS_WARMUP CCardStatus::s_pCardStatus->IsWarmup()
#endif

#endif // !defined(AFX_CARDSTATUS_H__509AE203_24BE_4426_A24C_5AE3783D00A9__INCLUDED_)
