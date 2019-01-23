// DebugDL.h: interface for the CDebugDL class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DebugDL.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DebugDL.h 1.4 2008/05/22 22:29:54PDT Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.4 $
 */

#if !defined(AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_)
#define AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_

#include "Debug.h"

class CBtmonDL;
class CYModemRecv;
class CCodeStorage;
class CLoader;
class CBaseProcess;
class CZModemRecv;

class CDebugDL : public CDebug  
{
private:
	void LoadE1AIS(void);
typedef struct {
	int option;
	const char *pCmdName;
	int cmdId;
	const char *pComment;
} CmdStruct;
public:
	void AllowAutoRate(int16 bAllowed);
	int16 GetAutoAllowed(void) { return m_bAutoRateAllowed; };
	void Start(CLoader *pLoader, Ccomm *pUserComm, Ccomm *pDebugComm, CFlash *pFlash);
	CDebugDL(const char *name, CLoader *pLoader, CBaseProcess *pProcess);
	virtual ~CDebugDL();
protected:
	virtual int ExtendedGetCmd(const char *pCmd, int *pLen);
	virtual void ExtendedExecCmd(int cmd, unsigned long *param);
	virtual void ExtendedPrintHelp(unsigned long *param);
protected:
	virtual int BaudrateGoDown(void);
	virtual int BaudrateGoUp(void);
	enum {CMD_START_OF_LIST = 1, CMD_ADC, CMD_SET_TIME, CMD_SET_DATE, CMD_PROCESS,
		CMD_YMODEM, CMD_ZMODEM, CMD_BAUDRATE,  CMD_VERSION,
		CMD_READ_SERIAL, CMD_WRITE_SERIAL,
		CMD_SYS, CMD_CONDITION,
		CMD_EVENT, CMD_DEBUG, CMD_EVENT_LOG,
		CMD_ISTATE, CMD_NVRAM,
		CMD_TEST, CMD_WHOAMI, CMD_SWITCH_ACTIVE,
		CMD_INPUT, CMD_ICW, CMD_DEBUG_MASK,
		CMD_DOWNLOAD, CMD_OVENELAPSE, CMD_DIR,
		CMD_READ_WAVE,  CMD_FPGA, CMD_CARD, CMD_OUTPUT,
		CMD_FLASH, CMD_SECOND, CMD_OUTPUT_PM, 
		CMD_REMOTE_DEBUG, CMD_PM, CMD_DUAL_RAM,
		CMD_WARMUP, CMD_HOUSE, CMD_RUBIDIUM,
		CMD_CONTINUE_READ, CMD_MANF, CMD_BT3 
	};
	static const CmdStruct m_cmdList[];
	Ccomm *m_pDebugComm;
	Ccomm *m_pUserComm;

private:
	void CmdWarmup(unsigned long *param);
	void CmdFlash(unsigned long *param);
	void CmdFpga(unsigned long *param);
	void CmdPm(unsigned long *param);
	void CmdRemoteDebug(unsigned long *param);
	void CmdBT3(unsigned long *param);
	void CmdManf(unsigned long *param);
	void CmdRub(unsigned long *param);
	void CmdContinueRead(unsigned long *param);
	void CmdDir(unsigned long *param);
	void ReportBootVersion(void);
	void CmdOvenElapse(unsigned long *param);
	void CmdDownload(unsigned long *param);
	void CmdReadWave(unsigned long *param);
	void CmdDebugMask(unsigned long *param);
	void CmdICW(unsigned long *param);
	void CmdInput(unsigned long *param);
	void CmdSwitchActive(unsigned long *param);
	void CmdWHOAMI(unsigned long *param);
	void CmdNVRAM(unsigned long *param);
	void CmdIstate(unsigned long *param);
	void PauseTick(int ticks);
	void CmdEventLog(unsigned long *param);
	void CmdDebug(unsigned long *param);
	void CmdEvent(unsigned long *param);
	void CmdCondition(unsigned long *param);
	void CmdDac8801(unsigned long *param);
	void CmdSys(unsigned long *param);
	void CmdWriteSerial(unsigned long *param);
	void CmdReadSerial(unsigned long *param);
	void CmdBTMON(unsigned long *param);
	void CmdBaud(unsigned long *param);
	void CmdProc(unsigned long *param);
	void CmdYModemRecv(unsigned long *param);
	void CmdZModemRecv(unsigned long *param);
	void CmdTest(unsigned long *param);
	void CmdAdc(unsigned long *param);
	void CmdSetTime(unsigned long *param);
	void CmdSetDate(unsigned long *param);
	
	// CBtmonDL *m_pBTMON;
	CYModemRecv *m_pYModem;
	CZModemRecv *m_pZModem;
	CCodeStorage *m_pStore;
	CLoader *m_pLoader;
	long m_startingBaudrate;
	char m_name[33];
	CBaseProcess *m_pProcess;

};

#endif // !defined(AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_)
