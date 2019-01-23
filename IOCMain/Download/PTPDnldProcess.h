// PTPDnldProcess.h: interface for the CPTPDnldProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldProcess.h 1.2 2008/01/29 11:29:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */

#if !defined(AFX_PTPDNLDPROCESS_H__E83BDC03_7354_4759_8285_90BF285C8D0B__INCLUDED_)
#define AFX_PTPDNLDPROCESS_H__E83BDC03_7354_4759_8285_90BF285C8D0B__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "BaseProcess.h"
#include "PTPDnldRecv.h"
#include "MailBox.h"

class CPTPLink;
class CCodeStorage;
class CFlash;
class CLoader;
class CPTPDnldSend;
class CBlockStorage;
class Ccomm;
class CIstateProcess;
class CIstateParser;
class CFactory;
class CPTPFactory;
class CPTPDnldReport;
class CIOCIstateStorage;

#ifndef TRACE_PTP_DNLD
#if 1                  
#define TRACE_PTP_DNLD CPTPDnldProcess::s_pPTPDnldProcess->PrintDebugOutput
#else
#define TRACE_PTP_DNLD 1 ? (void)0 : CPTPDnldProcess::s_pPTPDnldProcess->PrintDebugOutput
#endif
#endif

class CPTPDnldProcess : public CBaseProcess  
{
public:
	static void DnldHack(unsigned long *param, Ccomm *pComm);
	void StartSendCodeToTwinIOC(void);
	//static void DebugOutput(const char *str);
	virtual int Notify(uint32 from, uint32 what);
	CPTPDnldProcess(CFactory *pFactory,
		CIstateProcess *pIstateProcess, CLoader *pLoader, 
		CFlash *pFlash, CPTPLink *pIOCLink, 
		CPTPLink *pIMCLink,
		// uint8 inPort, uint8 outPort,
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CPTPDnldProcess();
private:
	/* Size changed on 7/21/2003 */
#define MAX_IMC_ISTATE_SIZE  (0x20000L)
	enum { /* MAX_IMC_ISTATE_SIZE = 0x4000, */ /* 0x1000, */
		MAX_IMC_ISTATE_VERSION_SIZE = 64,
		 MAX_IOC_ISTATE_SIZE = 0x4000 /* 0x1000 */
		};
	enum { CMD_CODE_DOWNLOAD = 0x150,
		CMD_GET_FACTORY = 0x160,
		CMD_SET_FACTORY = 0x161,
		// PPC Istate/Istate version Read/Write
		CMD_IMC_ISTATE_DOWNLOAD = 0x170, // Send PPC Istate from PPC to RTE 
		CMD_IMC_ISTATE_VERSION_DOWNLOAD = 0x171, // Send PPC Istate version from PPC to RTE
		CMD_GET_IMCISTATE_MAIN = 0x172, // Read PPC Istate from RTE to PPC
		CMD_GET_IMCISTATE_VERSION = 0x173, // Read PPC Istate version from RTE to PPC
		// RTE Istate read/write
		CMD_IOC_ISTATE_DOWNLOAD = 0x180, // Send RTE Istate from PPC to RTE
		CMD_GET_IOC_ISTATE = 0x181 // Read RTE Istate from RTE to PPC
	};

	enum { EVT_IOC_CODE_DNLD = 1, EVT_IMC_CODE_DNLD = 2, 
		EVT_START_SEND_CODE_TO_TWIN_IOC = 4
	};
	enum {
	// 12/29/2004 Warning!!!: RTE 1.1.2(first release) is defective. It cannot accept message format unless on different port. 	
	PTP_PORT_IOC_SENDER = 0x68, PTP_PORT_IOC_RECEIVER = 0x38,
	PTP_PORT_IMC_CODE_SENDER = 0x30, PTP_PORT_IMC_RECEIVER = 0x60,
	PTP_PORT_IMC_IMCISTATE_SENDER = 0x30,
	PTP_PORT_IMC_IOCISTATE_SENDER = 0x30,
	PTP_PORT_IMC_FACTORY_SENDER = 0x30,
	PTP_PORT_IMC_IMCISTATE_REPORT_PEER = 0x30,
	PTP_PORT_IMC_IMCISTATE_VERSION_SENDER = 0x30,
	PTP_PORT_IMC_IMCISTATE_VERSION_REPORT_PEER = 0x30,
	PTP_PORT_IMC_IOCISTATE_REPORT_PEER = 0x30};
	void entry(void);
	static void Entry(void);
	CPTPLink *m_pIOCLink;
	CPTPLink *m_pIMCLink;

	// Firmware download stuff
	CCodeStorage *m_pStorage;
	CPTPDnldRecv *m_pIOCReceiver;
	CPTPDnldSend *m_pIOCSender;
	CPTPDnldRecv *m_pIMCReceiver;

	// Mail boxes to hold message from PPC/RTE link
	CMailBox m_iocDnldBox;
	CMailBox m_imcDnldBox;
	
	// PPC Istate stuff
	CBlockStorage *m_pIMCIstateStorage;
	CPTPDnldRecv *m_pIMCIstateReceiver;
	CPTPDnldReport *m_pIMCIstateReporter;

	// PPC Istate version stuff
	CBlockStorage *m_pIMCIstateVersionStorage;
	CPTPDnldRecv *m_pIMCIstateVersionReceiver;
	CPTPDnldReport *m_pIMCIstateVerionReporter;

	// RTE Istate stuff
	CIOCIstateStorage *m_pIOCIstateStorage;
	CPTPDnldRecv *m_pIOCIstateReceiver;
	CIstateProcess *m_pIstateProcess;
	CIstateParser *m_pIstateParser;
	CPTPDnldReport *m_pIOCIstateReporter;

	// Factory setting stuff
	CPTPFactory *m_pPTPFactory;
protected:
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
public:
	void Init(void);
	static CPTPDnldProcess *s_pPTPDnldProcess;
};

#endif // !defined(AFX_PTPDNLDPROCESS_H__E83BDC03_7354_4759_8285_90BF285C8D0B__INCLUDED_)
