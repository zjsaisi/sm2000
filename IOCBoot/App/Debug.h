/*                   
 * Filename: Debug.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Debug.h 1.1 2007/12/06 11:38:51PST Zheng Miao (zmiao) Exp  $
 */                 

#ifndef _DEBUG_H780_ZHENG_MIAO
#define _DEBUG_H780_ZHENG_MIAO

#include "DataType.h"

class Ccomm;
class CFlash;
                               
typedef struct {
	const char *pCmdName;
	int cmdId;
	const char *pComment;
} CmdStruct;

class CDebug {
public:  	
	void WaitSpace(void);
  	CDebug(void);
  	~CDebug();
	int Start(Ccomm *port, CFlash *pFlash, int no_interrupt = 0); 
	void LWordToASCII(unsigned long l, char *buff);    
	void WordToASCII(uint16 wd, char *buff);
	static void CmdMonitor(unsigned long *param);
	void CDebug::SetPrompt(const char *pPrompt);
protected:
	virtual int ExtendedGetCmd(const char *pCmd, int *pLen) { return CMD_END; };
	virtual void ExtendedExecCmd(int cmd, unsigned long *param) { };
	virtual void ExtendedListHelp(void) { };
private:  
	int ReadCmd(int *pCmd, unsigned long *param);
	enum { KEY_BS = 8 };	                    
	static const CmdStruct m_debugCmdList[];  
private: 	// All the commands
	void CmdCheckRam(unsigned long *param);
	void CmdReset(unsigned long *param);
	void CmdFill(unsigned long *param, int8 bTryFlash = 1, int unit = 1);
	void CmdBlank(unsigned long *param, uint8 against = 0xff);
	void CmdErase(unsigned long *param);
	void ByteToASCII(uint8 ch, char *buff);
	void CmdDump(unsigned long *param,  int8 bDisplayASCII = 1);
	unsigned long m_currentAddress;
	void CmdHelp(unsigned long *param);
	char m_prompt[21];
protected:
	enum { CMD_HELP = 0x40, CMD_CHECK_RAM, CMD_WRITE_SINGLE, CMD_RESET, CMD_WRITE_WORD, CMD_FILL_WORD, 
		CMD_WRITE, CMD_FILL, CMD_BLANK, CMD_ERASE, CMD_READ, CMD_DUMP, CMD_MONITOR,
		CMD_END };
	void SendStr(const char *ptr, int len = -1);            
	CFlash *m_pFlash;	
	Ccomm *m_port;	 
	int m_noInterrupt;
};

#endif
