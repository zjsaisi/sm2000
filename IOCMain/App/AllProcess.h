// AllProcess.h: interface for the CAllProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: AllProcess.h 1.12 2009/05/05 10:06:34PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.12 $
 */

#if !defined(AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_)
#define AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Constdef.h"
#include "string.h"

#ifdef __cplusplus
#include "g_cmx.h"
#include "stdarg.h"
#include "IOCEvt.h"

class Ccomm;
class CDebugProcess;
class CUserProcess;
class CNVRAM;
class CPTPLink;
class CPTPProcess;
class CLed;
class CDebugMask;
class CRmtComm;
class CRubProcess;

#ifndef REPORT_EVENT     
#if 1
#define REPORT_EVENT CAllProcess::g_pAllProcess->ReportEvent
#else 
#define REPORT_EVENT (1) ? ((void)0) : CAllProcess::g_pAllProcess->ReportEvent
#endif
#endif

#ifndef FORCE_TRACE
#define FORCE_TRACE CAllProcess::g_pAllProcess->Print
#endif

class CAllProcess  
{
public:
	enum {
		CARD_FAIL_INDEX_START = 0,
		CARD_FAIL_INDEX_ADC_1V8 = 0,
		CARD_FAIL_INDEX_ADC_10M_EFC,
		CARD_FAIL_INDEX_ADC_20M_EFC,
		CARD_FAIL_INDEX_25M_EFC,
		CARD_FAIL_INDEX_24M5_EFC,
		CARD_FAIL_INDEX_24M7_EFC,
		CARD_FAIL_INDEX_OCXO_CURRENT_OOR,
		CARD_FAIL_INDEX_FPGA_INTERRUPT_LOSS,
		CARD_FAIL_INDEX_RUBIDIUM_UNLOCK,
		CARD_FAIL_INDEX_UNCLASSIFIED,
		CARD_FAIL_INDEX_END
	};
private:
	CRubProcess *m_pRubProcess;

public:
	static int m_oscQL;
	static int GetOscQL(void);
	static void SetOscQL(int level);
public:
	CAllProcess();
	virtual ~CAllProcess();

	static void MaskedPrint(uint32 mask, const char *format, ...);
	static void vMaskedPrint(uint32 mask, const char *format, va_list list);
	void vPrint(const char *format, va_list list);
	static CAllProcess *g_pAllProcess;
	
	int Init(void);
	int StartAll(void);
	int CheckMask(uint32 mask);
	void OsSecondAnnounce(void); // Called by rtc/timer to announce a second
	void CsmSecondAnnounce(); // CAlled by ISR from CSM
	void TaskPerSecond(void); // routines to be called roughly every second. This is not supposed to be accurate
public:
	uint32 GetAbsoluteSecond(void);
	uint32 GetComputerSeconds(void);
	void ReportVersion(Ccomm *pSci, int bForceIt = 0);
	void RebootSystem(int quick = 0, Ccomm *pComm = NULL, int evt = COMM_RTE_EVENT_RTE_RESET);
	uint32 GetEndOfRam(uint16 *pValue);
	int ReportEvent(int16 id, uint8 active, uint8 aid1 = 0, uint8 aid2 = 0);
	void Pause(int ticks);
	void DumpHex(const char *pHeader, unsigned char const *ptr, int len);
	void ClearStandingEvent(void);
	CNVRAM *GetNVRAM(void) { return m_pNVRAM; };
private:

	int m_bBooting;
	int m_bIstateInited;
	int m_failedOffline; // It's failed or offline
	enum { THIS_CARD_FAIL = 1, THIS_CARD_OFFLINE = 2 };
private:
	void MarkNormalReboot(void);
public:
	static void AllAction(void);
	void GraceShutDown(int grace = 10);
	void Print(const char *format, ...);
	static int CheckDldHeader(const char *pHead);
	void CardOffline(void);
	void CardFail(int index);
	void IstateInited(void);
	CNVRAM *GetNvram(void) { return m_pNVRAM; };
	static uint8 GetHardwareRevision(void);
	static int8 GetSlotId(void);
	static int8 GetFpgaDone();

	int AmIMaster(void);
	void SaveBaudrate(void);
	CPTPLink *GetIocLink(void) { return m_pLinkIocIoc; };
	CPTPLink *GetImcLink(void) { return m_pLinkImcIoc; };
	CDebugMask *GetDebugMask(void) { return m_pDebugMask; };
	CRmtComm *GetRmtComm(void) { return m_pRmtComm; };
	enum {
		HIGH_PROC_PRIORITY = 0x10,

		FIRST_PROC_PRIORITY, // Only used by idle process temperarily to measure CPU speed.
		SECOND_PROC_PRIORITY,
		CARD_PROC_PRIORITY,
		IMC_IOC_PROC_PRIORITY,
		IOC_IOC_PROC_PRIORITY,
		EVENT_PROC_PRIORITY, 
		OUTPUT_PROC_PRIORITY,

		ISTATE_PROC_PRIORITY,
		PTP_DNLD_PROC_PRIORITY,
		HOUSE_PROC_PRIORITY,
		RUBIDIUM_PROC_PRIORITY,
		MINUTE_PROC_PRIORITY,
		DEBUG_PROC_PRIORITY,
		USER_PROC_PRIORITY,
		REMOTE_DEBUG_PROC_PRIORITY,

		// Insert above
		IDLE_PROC_PRIORITY, 
		LOW_PROC_PRIORITY
	};
	static char g_versionString[MAX_SERIAL_LENGTH + 1];

private:
	void SetupPtp(void);
	void InitBaudrate(void);
	CDebugProcess *m_pDebugProcess; // Debug process may not be a singular
	CUserProcess *m_pUserProcess; 
	CNVRAM *m_pNVRAM;
	CPTPLink *m_pLinkImcIoc;
	CPTPLink *m_pLinkIocIoc;
	CPTPProcess *m_pImcIocProcess;
	CPTPProcess *m_pIocIocProcess;
	CDebugMask *m_pDebugMask;
	CRmtComm *m_pRmtComm;
	int m_shutdown;

private:
	int m_bPassEvent;
};

#endif // __cplusplus

EXTERN void DebugPrint(const char *str, int group);

#endif // !defined(AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_)
