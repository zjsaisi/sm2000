// RubProcess.h: interface for the CRubProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * 
 * Author: Zheng Miao
 * All rights reserved.
 *
 * $Header: RubProcess.h 1.1 2008/05/16 15:20:04PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */
#if !defined(AFX_RUBPROCESS_H__7967E4EB_B200_4A1B_8FAD_8515B2546E89__INCLUDED_)
#define AFX_RUBPROCESS_H__7967E4EB_B200_4A1B_8FAD_8515B2546E89__INCLUDED_

#include "BaseProcess.h"
#include "MailBox.h"

class CSci;
class Ccomm;
class CPTPLink;

class CRubProcess : public CBaseProcess  
{
private:
	enum { RUB_CMD_SIZE = 16 };
	enum { NUM_RUB_CMD = 20 };

public:
	static CRubProcess *s_pRubProcess;
	void Init(void);
	void PrintRubStatus(Ccomm *pComm);
	int AppendRubCmdFromDebugPort(char *pCmd);
	virtual int Notify(uint32 from, uint32 what);
	CRubProcess(CPTPLink *pIMCLink, CSci *pSci, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CRubProcess();
	CSci *GetSci(void) { return m_pSciPort; };
	int GetNumRequestPending(void) { return ((m_cmdTail - m_cmdHead + NUM_RUB_CMD) % NUM_RUB_CMD); };
protected:
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
private:
	enum { RUB_RESPONSE_SIZE = 1024 };
	void DealIMCRequest(void);
	int AppendCmd(char *pCmd, int len, int *pNext);
	enum { LINK_RUB_IMC_IOC = 0x74, // port to receive rubidium request from IMC to IOC. 
		LINK_RUB_IOC_IMC = 0x30, // port to send response from IOC to IMC.
		PTP_CMD_RUB_X72 = 0x210,
		LINK_NULL
		};
	typedef struct { // Command from IMC on port LINK_RUB_IMC_IOC = 0x74
		uint16 cmd; // PTP_CMD_RUB_X72 = 0x210
		uint32 handle; // Handle used by IMC
		int16 len; // bytes in buffer
		char buffer[RUB_CMD_SIZE]; // RUB_CMD_SIZE = 16
	} SRubCmd;
	typedef struct { // response back to port LINK_RUB_IOC_IMC = 0x30. Response size = len + 10 bytes
		uint16 cmd; // PTP_CMD_RUB_X72 = 0x210
		uint32 handle; // Handle returned to IMC without changing
		int16 error_code;  // 1 for no error. So far no error code defined.
		int16 len; //  bytes in buffer
		char buffer[RUB_RESPONSE_SIZE]; // RUB_RESPONSE_SIZE = 1024
	} SRubResp;
	void ReportToSelf(void);
	int AppendRubCmdFromSelf(char *pCmd);
	void ReportToIMC(void);
	void ReportToDebugPort(void);
	int DealResponse(void);
	int DealOutputRequest(void);
	void entry(void);
	enum { EVT_SERIAL_INPUT = 1, EVT_SERIAL_OUTPUT_REQUEST = 2,
		EVT_IMC_CMD = 4};
	static void Entry(void);
	CSci *m_pSciPort;

	/* Output part, send out command */
	enum { CMD_SOURCE_DEBUG_PORT = 0x19, CMD_SOURCE_IMC, CMD_SOURCE_SELF };
	typedef struct {
		uint32 handle; // Used by IMC request
		int16 source;
		int16 len;
		char cmdBuff[RUB_CMD_SIZE];
	} SCmdSource;
	// char m_rubCmdList[NUM_RUB_CMD][RUB_CMD_SIZE];
	SCmdSource m_rubCmdSource[NUM_RUB_CMD];
	int m_cmdHead;
	int volatile m_cmdTail;
	int m_lastCmd;

	/* Receive from serial port. Called input */
	int m_responseIndex;
	char m_responseBuffer[RUB_RESPONSE_SIZE + 1];

	// baudrate setttled
	int m_baudrateSettled;

	// Link to IMC
	CPTPLink *m_pIMCLink;
	CMailBox m_imcCmdBox;

	// command count
	int32 m_nIMCCmdCnt;
	int32 m_nDbgCmdCnt;
	int32 m_nInternalCmdCnt;
};

#endif // !defined(AFX_RUBPROCESS_H__7967E4EB_B200_4A1B_8FAD_8515B2546E89__INCLUDED_)
