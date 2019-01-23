// PTPDnldProcess.h: interface for the CPTPDnldProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldProcess.h 1.1 2007/12/06 11:39:07PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
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
//class CBlockStorage;
class Ccomm;
//class CIstateProcess;
//class CIstateParser;

class CPTPDnldProcess : public CBaseProcess  
{
public:
#if 0
	static void PrintStatus(Ccomm *pComm, unsigned long *param);
#endif
	void StartSendCodeToTwinIOC(void);
	static void DebugOutput(const char *str);
	virtual int Notify(uint32 from, uint32 what);
	CPTPDnldProcess(/* CIstateProcess *pIstateProcess, */
		CLoader *pLoader, CFlash *pFlash, CPTPLink *pIOCLink, 
		CPTPLink *pIMCLink,
		// uint8 inPort, uint8 outPort,
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CPTPDnldProcess();
private:
	void VersionStringQuery(CMemBlock *pBlock);
	enum {CMD_GET_FACTORY = 0x160, PTP_PORT_IMC_FACTORY_SENDER = 0x30 };
	enum { FACTORY_ID_VERSION_STRING = 7 };
	enum {PTP_STRING_SIZE = 32};
	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 id;
	} SCmdReadFactory;

	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 error_code;
		char value[PTP_STRING_SIZE];
	} SRespReadFactory;


	int m_timeoutCnt;
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 istate_id;
		int16 index1;
		int16 index2;
	} SCmdGetIstate;

	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error;
		int16 istate_id;
		int16 index1;
		int16 index2;
		uint32 value;
	} SRespGetIstate;


	void FirmwareVersionQuery(CMemBlock *pBlock);
	enum { CMD_IMC_ISTATE_DOWNLOAD = 0x170, MAX_IMC_ISTATE_SIZE = 0x1000,
		CMD_IOC_ISTATE_DOWNLOAD = 0x180, MAX_IOC_ISTATE_SIZE = 0x1000,
		CMD_IOC_ISTATE_READING = 0x130
		};
	enum { CMD_CODE_DOWNLOAD = 0x150 };

	enum { EVT_IOC_CODE_DNLD = 1, EVT_IMC_CODE_DNLD = 2, 
		EVT_START_SEND_CODE_TO_TWIN_IOC = 4
	};
	enum { PTP_PORT_IOC_SENDER = 0x68, PTP_PORT_IOC_RECEIVER = 0x38,
	PTP_PORT_IMC_CODE_SENDER = 0x30, PTP_PORT_IMC_RECEIVER = 0x60,
	PTP_PORT_IMC_IMCISTATE_SENDER = 0x30,
	PTP_PORT_IMC_IOCISTATE_SENDER = 0x30,
	PTP_PORT_IMC_IOCISTATE_ACCESS = 0x30,
	PTP_PORT_IOC_IOCISTATE_ACCESS = 0x40 };
	void entry(void);
	static void Entry(void);
	CPTPLink *m_pIOCLink;
	CPTPLink *m_pIMCLink;
	CCodeStorage *m_pStorage;
	CPTPDnldRecv *m_pIOCReceiver;
	CPTPDnldSend *m_pIOCSender;
	CPTPDnldRecv *m_pIMCReceiver;
	CMailBox m_iocDnldBox;
	CMailBox m_imcDnldBox;
	// CBlockStorage *m_pIMCIstateStorage;
	// CPTPDnldRecv *m_pIMCIstateReceiver;
	// CBlockStorage *m_pIOCIstateStorage;
	// CPTPDnldRecv *m_pIOCIstateReceiver;
	// CIstateProcess *m_pIstateProcess;
	// CIstateParser *m_pIstateParser;

protected:
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
};

#endif // !defined(AFX_PTPDNLDPROCESS_H__E83BDC03_7354_4759_8285_90BF285C8D0B__INCLUDED_)
