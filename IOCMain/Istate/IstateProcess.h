// IstateProcess.h: interface for the CIstateProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateProcess.h 1.2 2008/01/29 11:29:17PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ISTATEPROCESS_H__A799AD83_4DCF_48AA_9E80_F0DEC436A7BD__INCLUDED_)
#define AFX_ISTATEPROCESS_H__A799AD83_4DCF_48AA_9E80_F0DEC436A7BD__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "BaseProcess.h"
#include "queue.h"
//#include "Output.h"
#include "MailBox.h"
#include "IocKeyList.h"

class CPTPLink;
class CIstate;
class CNVRAM;
class CIstateExec;
class CIstateAccess;
class CIstateKb;
class Ccomm;
class CIstateCheckSum;
class CIstateBasic;

#ifndef TRACE_ISTATE
#if 1                  
#define TRACE_ISTATE CIstateProcess::s_pIstateProcess->PrintDebugOutput
#else
#define TRACE_ISTATE 1 ? (void)0 : CIstateProcess::s_pIstateProcess->PrintDebugOutput
#endif
#endif

#ifndef SET_MY_ISTATE
#define SET_MY_ISTATE CIstateProcess::s_pIstateProcess->WriteOneIOCIstateItem
#endif

#ifndef SET_TWIN_ISTATE
#define SET_TWIN_ISTATE CIstateProcess::s_pIstateProcess->WriteTwinOneIOCIstateItem
#endif

class CIstateProcess : public CBaseProcess  
{
public:
	int IsMaster(void);
	int IsTwinActive(void);
	void IstateHack(unsigned long *param, Ccomm *pComm);
	void TickIstateProcess(void);
	int IsTwinSameIstateVersion(void);
	static CIstateProcess *s_pIstateProcess;
	int WriteTwinOneIOCIstateItem(int16 item, int16 index1, int16 index2, uint32 value);
	void Init(void);
	void PrintIstateStatus(Ccomm *pComm);
	//static void sDebugOutput(const char *format, ...);
	//static void DebugOutput(const char *ptr);
	void ResendIOCIstateToTwin(void);
	void ReportProcessIdle(void);
	// This function can be private. Make public so that debug process can access it.
	// Writing is only allowed to RTE Istate in RTE card
	int WriteOneIOCIstateItem(int16 item, int16 index1, int16 index2, uint32 value, int16 cmd = CMD_WRITE_ISTATE);
	// Get the pointer to RTE Istate
	CIstate * GetIOCIstate(void);
#if 0 // Take out PPC Istate from RTE
	// Get the pointer to PPC Istate
	CIstate * GetIMCIstate(void);
#endif
	// Add Istate init steps
	virtual int StartProcess(void);
	// LINK_IOCISTATE_IOC_IOC: 
	//		Link to transfer and check RTE Istate between RTE/RTE
	enum { LINK_IOCISTATE_ACCESS = 0x40, // Access from PPC
		LINK_IOCISTATE_IOC_IOC = 0x31
		/* , LINK_IOCISTATE_IMC_IOC, 
		LINK_IMCISTATE_IOC_IOC, LINK_IMCISTATE_IMC_IOC 
		*/
	};

	// Callback from PTP module
	virtual int Notify(uint32 from, uint32 what);
	CIstateProcess( //COutput **pOutputs, CFPGA *pFPGA, CCardState *pCardState, 
		CPTPLink *pIMCLink, CPTPLink *pIOCLink, 
		CNVRAM *pNVRAM,
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CIstateProcess();
private:
	uint32 m_houseKeepingCnt;
	typedef struct {
		int16 cmd; //  CMD_WRITE_ISTATE
		int16 item;
		int16 index1;
		int16 index2;
		uint32 value;
	} SWriteIstateItem;
	
	enum {CMD_TWIN_START = 0x40,
		CMD_TWIN_WRITE_NO_RESPONSE,
		CMD_TWIN_SEND_VERSIONS,
		CMD_TWIN_END };
	typedef struct {
		int16 cmd; // enum above CMD_TWIN_WRITE_NO_RESPONSE
		SWriteIstateItem data;
	} SIOCIstateTwinWrite;

	typedef struct {
		int16 cmd; // enum above CMD_TWIN_SEND_VERSIONS
		uint32 firmware_version;
		char hardware_version[MAX_SERIAL_LENGTH];
		char part_number[MAX_SERIAL_LENGTH];
	} SIOCVersions;

	int ReceiveTwinMessage(const uint8 *ptr, uint16 len);

	enum { EVENT_IOC_LINK = 1, EVENT_IMC_LINK = 2,
		EVENT_ISTATE_WRITE_REQUEST = 4, 
		EVENT_HOUSE_KEEPING = 8};
	void entry(void);
	static void Entry(void);

protected:
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
private:
	void WaitIocLink(void);
	void HouseKeeping(void);
	int m_busyCnt; // If PPC is crazy and keep sending istate command, RTE doesn't want to be killed by it.
	int ReceiveIocVersions(const uint8 *ptr, uint16 len);
	int SendIocVersions(void);
	void IocLinkUp(int link);
	void IocLinkDown(int link);
	void ClearTwinVersions(void);
	void CheckCompatibility(void);
	void ExecuteWritingRequest(void);
	CIstateKb *m_pIOCKB;

	// Mail boxes to receive message from PTP link
	//unsigned char m_iocBox;
	//unsigned char m_imcBox;
	CMailBox m_iocBox;
	CMailBox m_imcBox;

	// Queue to hold Istate change request
	CQueue m_queue;

	CPTPLink *m_pIMCLink; // Link to PPC card
	CPTPLink *m_pIOCLink; // Link to twin RTE card
	int m_iocLinkStatus;

	CIstate *m_pIOCIstate; // RTE Istate  
	CIstateExec *m_pIOCIstateExec; // RTE Istate exec
	CIstateCheckSum *m_pIocCheckSum; // for convenience. Object created by CIstateExec

#if 0 // Take out PPC Istate from RTE card
	CIstate *m_pIMCIstate; // PPC Istate 
	CIstateExec *m_pIMCIstateExec; // PPC Istate exec
#endif
	
	CNVRAM *m_pNVRAM; // Fake NVRAM driver
	
	// PPC access RTE-Istate
	CIstateAccess *m_pImcAccess;

	// Output stuff
	//COutput **m_pOutputs;

	// FPGA 
	//CFPGA *m_pFPGA;

	// Extra twin info
	uint32 m_twinFirmwareVersion;
	char m_twinHardwareVersion[MAX_SERIAL_LENGTH+1];
	char m_twinPartNumber[MAX_SERIAL_LENGTH+1];
};

#endif // !defined(AFX_ISTATEPROCESS_H__A799AD83_4DCF_48AA_9E80_F0DEC436A7BD__INCLUDED_)
