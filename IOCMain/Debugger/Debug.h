/*                   
 * Filename: Debug.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Debug.h 1.1 2007/12/06 11:41:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */                 

#ifndef _DEBUG_H780_ZHENG_MIAO
#define _DEBUG_H780_ZHENG_MIAO

#include "DataType.h"

class Ccomm;
class CFlash;
                               

class CDebug {
private:
typedef struct {
	const char *pCmdName;
	int cmdId;
	const char *pComment;
} CmdStruct;
public:  	
	void WaitSpace(void);
	static void PrintLine(char *pBuff, uint32 address, int16 len, uint8 *pContent, int8 bDisplayASCII = 1);
	void SetPrompt(const char *pPrompt);
  	CDebug(void);
  	~CDebug();
	int Start(Ccomm *port, CFlash *pFlash, int no_interrupt = 0); 
	void LWordToASCII(unsigned long l, char *buff);    
	void WordToASCII(uint16 wd, char *buff);
	int8 SetEcho(int8 echo); 
protected:
	enum { NUM_STRING_PARAM = 10, PARAM_TYPE_STRING = 0x12ae };
	typedef struct {
		int type;
		char *pStr;
	} SParam;
	SParam m_strParam[NUM_STRING_PARAM];
	int m_strParamIndex;
	char *GetParamStr(uint32 param);
	virtual int ExtendedGetCmd(const char *pCmd, int *pLen) { return CMD_END; };
	virtual void ExtendedExecCmd(int cmd, unsigned long *param) { };
	virtual void ExtendedPrintHelp(unsigned long *param) { };
	int ReadStr(char *ptr, int max_len, int8 trim);	
	virtual int BaudrateGoUp(void) { return 0; };
	virtual int BaudrateGoDown(void) { return 0; };
	void CmdDump(unsigned long *param,  int8 bDisplayASCII = 1);
	static void ByteToASCII(uint8 ch, char *buff);
private:
	char m_cmdBuff[100];
	int ReadCmd(int *pCmd, unsigned long *param);
	enum { KEY_BS = 8 };	                    
	static const CmdStruct m_debugCmdList[];  
private: 	// All the commands
	//void CmdCheckRam(unsigned long *param);
	void CmdQuickReset(unsigned long *param);
	void CmdReset(unsigned long *param);
	void CmdFill(unsigned long *param, int8 bTryFlash = 1, int unit = 1);
	void CmdBlank(unsigned long *param, uint8 against = 0xff);
	void CmdErase(unsigned long *param);
	unsigned long m_currentAddress;
	void CmdHelp(unsigned long *param);
protected:
	enum { CMD_HELP = 0x40, CMD_CHECK_RAM, CMD_WRITE_SINGLE, CMD_RESET, CMD_WRITE_WORD, CMD_FILL_WORD, 
		CMD_WRITE, CMD_FILL, CMD_BLANK, CMD_ERASE, CMD_READ, CMD_DUMP, CMD_QUICK_RESET, 
		CMD_WRITE_DOUBLE, CMD_END };
	void SendStr(const char *ptr, int len = -1);            
	CFlash *m_pFlash;	
	Ccomm *m_port;	 
	int m_noInterrupt;
	char m_prompt[21];

	// Automatic baud rate adjust
	int m_bAutoRateAllowed;

	volatile int8 m_bEcho;
};

#endif
