// DebugDL.h: interface for the CDebugDL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_)
#define AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_

#include "Debug.h"

class CBtmonDL;
class CYModemRecv;
class CStorage;
class CLoader;
class CBaseProcess;

class CDebugDL : public CDebug  
{
public:
	void Start(CLoader *pLoader, Ccomm *pUserComm, Ccomm *pDebugComm, CFlash *pFlash);
	CDebugDL(const char *name, CLoader *pLoader, CBaseProcess *pProcess);
	virtual ~CDebugDL();
protected:
	virtual int ExtendedGetCmd(const char *pCmd, int *pLen);
	virtual void ExtendedExecCmd(int cmd, unsigned long *param);
	virtual void ExtendedListHelp(void);
protected:
	enum {CMD_BTMON = 1, CMD_SET_TIME, CMD_SET_DATE, 
		CMD_PROCESS, CMD_YMODEM, CMD_BAUDRATE, CMD_LOAD_DLD, CMD_VERSION,
		CMD_DIR, CMD_FLASH,
		CMD_SYSTEM };
	static const CmdStruct m_cmdList[];
	Ccomm *m_pDebugComm;
	Ccomm *m_pUserComm;
	//CFlash *m_pFlash;

private:
	void CmdFlash(unsigned long *param);
	void CmdDir(unsigned long *param);
	void CmdSystem(unsigned long *param);
	void CmdSetTime(unsigned long *param);
	void CmdSetDate(unsigned long *param);
	CBtmonDL *m_pBTMON;
	CYModemRecv *m_pYModem;
	CStorage *m_pStore;
	CLoader *m_pLoader;
	long m_startingBaudrate;
	CBaseProcess *m_pProcess;

};

#endif // !defined(AFX_DEBUGDL_H__AB7D31E9_FA71_441A_9307_B3F5437B861A__INCLUDED_)
