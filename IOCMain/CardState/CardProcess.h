// CardProcess.h: interface for the CCardProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardProcess.h 1.2 2008/01/29 11:29:19PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#if !defined(AFX_CARDPROCESS_H__CCE258E3_E95B_48BC_9C99_8F958DB1FD2B__INCLUDED_)
#define AFX_CARDPROCESS_H__CCE258E3_E95B_48BC_9C99_8F958DB1FD2B__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Constdef.h"
#include "BaseProcess.h"
#include "Queue.h"

class CCardStatus;
class CCardLoop;
class CCardAlign;
class CWarmup;
class CFpga;
class CPTPLink;

#define TRACE_CARD CCardProcess::s_pCardProcess->PrintDebugOutput

class CCardProcess : public CBaseProcess  
{
public:
	void PostSetupInput(void);
	virtual int Notify(uint32 from, uint32 what);
	int ReceiveTwinCardState(const char *ptr, int size);
	void StateChange(int which = 0);
	void ReportCardOneSecondIsr(void);
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
	void Init(void);
	CCardProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = ENTRY);
	virtual ~CCardProcess();
	static CCardProcess *s_pCardProcess;
private:
	enum {CMD_PTP_RGP = 1, PTP_IOC_PORT_RGP = 0x20 };
	typedef struct {        // Command from PPC on port PTP_IOC_PORT_RGP = 0x20
		uint16 cmd;         // CMD_PTP_RGP = 0x01
		uint32 handle;      // Handle used by PPC
		uint32 offsetTime;  // the time the offset is to be applied (nominally next 1PPS)
		uint8  offset;      // predicted offset of GPS 1PPS for timeStamp second,
                            //   0 to 85ns if GPS qualified by PPC,
                            //   255 if GPS not qualified by PPC
        uint8  status;       // status of GPS on PPC:
                            //   0 = GPS disqualified by PPC
                            //   1 = Position Survey
                            //   2 = Position Fixed, TRAIM On
                            //   3 = Position Fixed, TRAIM Off
	} SImcRgpReport;
	CQueue m_rgpQueue;
private:
	void DispatchRgpReport(void);
	void SendCardState(void);
	typedef struct {
		int16 cmd; // CMD_CARD_STATE
		int16 version; // 1
		int16 cardState;
		int16 cardLoop;
		int16 cardAlign;
	} SCardState;
	void CardDoOneSecond(void);
	enum { EVT_ONE_SECOND = 1, EVT_RGP_REPORT = 2, EVT_SETUP_INPUT = 4 };
	static void ENTRY(void);
	int m_stateMachineDelay;
	CWarmup *m_pWarmup;
	CFpga *m_pFpga;
	CCardStatus *m_pCardStatus;
	CCardLoop *m_pCardLoop;
	CCardAlign *m_pCardAlign;
	SCardState m_twinCardState;
	int m_sendCardStateCnt;
	CPTPLink *m_pImcLink;
	CPTPLink *m_pIocLink;
protected:
	virtual void MessageHandler(uint16 message_type, uint32 param);
};

#endif // !defined(AFX_CARDPROCESS_H__CCE258E3_E95B_48BC_9C99_8F958DB1FD2B__INCLUDED_)
