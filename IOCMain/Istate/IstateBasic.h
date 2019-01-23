// IstateBasic.h: interface for the CIstateBasic class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateBasic.h 1.2 2008/01/28 17:16:47PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#if !defined(DEFINE_ISTATE_VARIABLE)
#if !defined(AFX_ISTATEBASIC_H__DB0AD3CF_C45C_4A20_A04E_640AC764C50A__INCLUDED_)
#define AFX_ISTATEBASIC_H__DB0AD3CF_C45C_4A20_A04E_640AC764C50A__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "IstateApp.h"
#include "IstateKb.h"

class CEventKb;

class CIstateBasic : CIstateApp 
{
public:
	static CIstateBasic *s_pIstateBasic;
	void Init(void);
	CIstateBasic(CIstateKb *pKb);
	virtual ~CIstateBasic();
private:
	enum { NUM_ISTATE = 8};
	static SIstateItem const m_istateList[NUM_ISTATE + 1];
	CEventKb *m_pEventKb;
private:
	static void InitServiceAffect(void *pEntry);
	static void InitEventLog(void *pEntry);
	static void InitAlarmLevel(void *pEntry);
	static void ActionServiceAffect(int16 index1, int16 index2, uint32 value);
	static void ActionAlarmLevel(int16 index1, int16 index2, uint32 value);
	static void ActionMisc(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_miscAction;
	static uint32 ve_miscAction;
	static SToggleIstate const dt_eventLog;
	static SEntryIstate const de_eventAlarmLevel;
	static SToggleIstate const dt_eventServiceAffect;
	static SEntryIstate const de_minorUpgradeTime;
	static SEntryIstate const de_majorUpgradeTime;
	static SEntryIstate const de_firmwareVersion;
	static uint32 ve_firmwareVersion;
	static SEntryIstate const de_istateVersion;
	static uint32 ve_istateVersion;
};

#endif // !defined(AFX_ISTATEBASIC_H__DB0AD3CF_C45C_4A20_A04E_640AC764C50A__INCLUDED_)

#elif defined(ISTATE_SHARE_FLASH)  // defined(DEFINE_ISTATE_VARIABLE)
// Here : Define data shared and non-volatile item in Istate
	uint32 ve_minorUpgradeTime;
	uint32 ve_majorUpgradeTime;

#elif defined(ISTATE_SHARE_VOLATILE)
// Here : Defined shared and volatile Istate items

#else
// Not shared.
	uint8 vt_eventServiceAffect[COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE];
	uint8 vt_eventLog[COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE];
	uint32 ve_eventAlarmLevel[COMM_RTE_EVENT_MAX - COMM_RTE_EVENT_NONE];

#endif // defined(DEFINE_ISTATE_VARIABLE)
