/*                   
 * Filename: Debug.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Debug.cpp 1.2 2008/03/12 17:02:23PDT Zheng Miao (zmiao) Exp  $
 */                 
 
//#include "project.h"
#include "machine.h"
#include "comm.h"
#include "flash.h"
#include "Debug.h"
#include "string.h"
#include "ConstDef.h"
#include "AllProcess.h"

CDebug::CDebug(void)
{                  
	m_port = NULL;
	m_currentAddress = 0x800000;
	strcpy(m_prompt, "\r\nBoot >");
}

CDebug::~CDebug(void)
{
}

const CmdStruct CDebug::m_debugCmdList[] = {
	{"HELP ", CMD_HELP, "HELP\t?\t\t: List commands available"},
	{"? ", CMD_HELP, NULL},
	{"DUMP ", CMD_DUMP, "DUMP\tD <range>\t: Dump memory content(two reading per byte)"}, 
	{"D ", CMD_DUMP, NULL},
	{"READ ", CMD_READ, "READ\tR <range>\t: Similar to DUMP without character(One read per byte)"},
	{"R ", CMD_READ, NULL},
	{"ERASE ", CMD_ERASE, "ERASE\tE <address>\t: Erase the external flash sector pointed by address"},
	{"E ", CMD_ERASE, NULL},
	{"BLANK ", CMD_BLANK, "BLANK\tB <range>\t: Verify if all are 0xff"},
	{"B ", CMD_BLANK, NULL},
	{"FILL ", CMD_FILL, "FILL\tF <range> <bytes>: Fill the memory with data pattern, can write to flash"},
	{"F ", CMD_FILL, NULL},
	{"W ", CMD_WRITE_SINGLE, "W\t <address> <byte>: Write byte to address"}, 
	{"WRITEL ", CMD_WRITE, "WRITEL\tWL <range> <words>: Similar to FILL except it won\'t write to flash"},
	{"WL ", CMD_WRITE, NULL},
	{"FW ", CMD_FILL_WORD,"FW\t<range> <wData>\t: FILL in word"},
	{"WW ", CMD_WRITE_WORD, "WW\t<range> <wData>\t: WRITE in word"},
	{"RESET ", CMD_RESET, "RESET\tBOOT, REBOOT\t: Software restart"},
	{"REBOOT ", CMD_RESET, NULL },
	{"BOOT ", CMD_RESET, NULL },
	{"MONITOR ", CMD_MONITOR, "Enter monitor to work with Hitachi debugger"},
	{"MON ", CMD_MONITOR, NULL },
	{"CKRAM", CMD_CHECK_RAM, "CKRAM\t\t\t: RAM self test"},
	{NULL, CMD_END, NULL}
};

int CDebug::Start(Ccomm *port, CFlash *pFlash, int no_interrupt)
{   
	int cmd;
	unsigned long param[50];
	int ret;
	 
	m_port = port;  
	m_pFlash = pFlash;  
	m_noInterrupt = no_interrupt;
#if 0
	for (ret = 0; ret < 10; ret++) {
		SendStr("*Debugger started*\r\n");
	}
#endif	
    do {
    	ret = ReadCmd(&cmd, param);
		if (ret == 2) {
			ExtendedExecCmd(cmd, param);
		}
    	if (ret != 1) continue;
		SendStr("\r\n");
    	switch (cmd) {
		case CMD_CHECK_RAM:
			CmdCheckRam(param);
			break;
    	case CMD_HELP:
    		CmdHelp(param);
    		break;
		case CMD_FILL_WORD:
			CmdFill(param, 1, 2);
			break;
		case CMD_WRITE_SINGLE:
			if (*param == 2) {
				*param = 3;
				param[3] = param[2];
				param[2] = 1;
				CmdFill(param, 0, 1);
			} else {
				SendStr("Invalid data\r\n");
			}
			break;
		case CMD_WRITE_WORD:
			CmdFill(param, 0, 2);
			break;
		case CMD_WRITE:
			CmdFill(param, 0);
			break;
		case CMD_FILL:
			CmdFill(param);
			break;
		case CMD_DUMP:
			CmdDump(param);
			break;
		case CMD_READ:
			if (*param == 1) {
				*param = 2;
				param[2] = 1;
			}
			CmdDump(param, 0);
			break;
		case CMD_ERASE:
			CmdErase(param);
			break;
		case CMD_BLANK:
			CmdBlank(param);
			break;
		case CMD_RESET:
			CmdReset(param);
			break;
		case CMD_MONITOR:
			CmdMonitor(param);
    	default:
			SendStr("To be implemented\r\n");
    		break;
    	}
    } while (1);  
	return 1;
}

// Return 1 for success
// 2 for extended command
int CDebug::ReadCmd(int *pCmd, unsigned long *param)
{               
	char buff[100];
	int index = 0, ret, i, eq;
	uint8 ch;
	int8 bPrompt = 1;
	unsigned long argc = 0;                                 
	int cmdLen;
	int curCmd;

 	do {            
 		if (bPrompt) {
			SendStr(m_prompt);
			bPrompt = 0;
 		}
		curCmd = 1;
		ret = m_port->Recv((char *)&ch, 1, 1000, m_noInterrupt);
 		if (ret != 1) {
 			continue;
 		}
		if (ch == KEY_BS) {
			if (index) index--;
			SendStr("\010 \010", 3);
			continue;
		}
 		SendStr((char *)&ch, 1);
 		if ((ch >= 'a') && (ch <= 'z')) ch = ch - 'a' + 'A';
 		buff[index++] = ch;
 		if ((index >= 99) || (ch == '\r') || (ch == '\n')) {
 			if (index == 1) { // NULL command
 				bPrompt = 1;
 				index = 0;
 				continue;
 			} 
 			SendStr("\r\n\000", 2); 			           
 			buff[index-1] = ' ';
 			buff[index] = 0;
 			// Find command 
 			i = 0;
 			while (m_debugCmdList[i].cmdId != CMD_END) {     
 				cmdLen = strlen(m_debugCmdList[i].pCmdName);
 				eq = strncmp(buff, m_debugCmdList[i].pCmdName, cmdLen);
 				if (eq == 0) break; 
 				i++;
 			}   
 			*pCmd = m_debugCmdList[i].cmdId;
			if (*pCmd == CMD_END) {
				*pCmd = ExtendedGetCmd(buff, &cmdLen);
				curCmd = 2;
			}
 			if (*pCmd == CMD_END) {
 				SendStr("\r\nInvalid command!");
 				bPrompt = 1;
 				index = 0;
 				continue;
 			}
 			// convert parameter
 			argc = 0;
 			char *ptr;
 			int8 bDataError = 0;
 			unsigned long base = 10;
 			long lTmp = 0;
			long sign;
			int digits = 0;
 			ptr = buff + cmdLen;
 			i = 0;
 			while (*ptr != 0) {
 				digits = 0;   
 				lTmp = 0;
 				base = 10;
 				i++;
				sign = 1;
 			    while ((*ptr == ' ') || (*ptr == '\t')) {
 			    	ptr++;
 			    }    
				if (*ptr == 0) break;
				if ((*ptr == '+') || (*ptr == '-')) {
					sign = (*ptr == '-') ? (-1) : 1;
					ptr++;
				}
 			    if ((*ptr == '0') && (ptr[1] == 'X')) {
 			    	ptr += 2; base = 16;
 			    }                                   
 			    while ((*ptr != 0) && (*ptr != ' ') && (*ptr != '\t')) {
 			    	if ((*ptr >= 'A') && (*ptr <= 'F') && (base == 16)) {
 			    		lTmp = lTmp * base + *ptr - 'A' + 10;
						digits++;
 			    	} else if ((*ptr >= '0') && (*ptr <= '9')) {
 			    		lTmp = lTmp * base + *ptr - '0';
						digits++;
 			    	} else {
 			    		bDataError = 1;
						break;
 			    	}
 			    	ptr++;                                     
 			    }
 			    // No digit means error
 			    if (digits == 0) bDataError = 1;
				if (bDataError) break;
				if (sign == -1) lTmp = -lTmp;
				param[i] = lTmp;
				argc++;
 			}
 			*param = argc;     
 			if (bDataError) {
 				SendStr("\r\nInvalid data!");
 				bPrompt = 1;
 				index = 0;
 				continue;
 			}
 			return curCmd;
 			   
 			
 		}   
 	} while (1); 
 	return curCmd;
}

void CDebug::SendStr(const char *ptr, int len)
{
	len = (len <= 0) ? strlen(ptr) : len;
	if (m_port) {
		if (m_noInterrupt) {
			m_port->ForceSend(ptr, len);
		} else {
			m_port->Send(ptr, len);
			//m_port->WaitTxDone(100);
		}
	}
}            


void CDebug::CmdHelp(unsigned long *param)
{
	const CmdStruct *ptr = m_debugCmdList;
	SendStr("Debug command list:\r\n");
	while (ptr->cmdId != CMD_END) {
		if (ptr->pComment) {
			SendStr(ptr->pComment);
			SendStr("\r\n");
		}
		ptr++;
	}
	ExtendedListHelp();
	SendStr("Data format:\r\n");
	SendStr("\tDecimal data and Hex starting with 0x are supported.\r\n");
	SendStr("\t<range>: <start address> <len>, len is in byte\r\n");
	SendStr("\t<data>: List of data in byte\r\n");
	SendStr("\t<wData>: List of data in word\r\n");
}

void CDebug::CmdDump(unsigned long *param, int8 bDisplayASCII)
{
	unsigned long address;
	unsigned long length = 0x80;
	uint8 *bPtr;
	char buff[30];
	int i;

	if (*param >= 1) {
		m_currentAddress = param[1];
	}
	if (*param >= 2) {
		length = param[2];
	}
	address = m_currentAddress;
	m_currentAddress = address + length;
	
	// SendStr("\r\n\000");
	while (length > 0) {
		unsigned long line;
		uint8 ch;

		// Show address
		ByteToASCII((uint8)(address >> 16), buff); SendStr(buff, 2);
		ByteToASCII((uint8)(address >> 8), buff); SendStr(buff, 2);
		ByteToASCII((uint8)(address), buff); SendStr(buff, 3);

		line = (length > 16) ? (16) : (length);
		for (i = 0; i < (long)line; i++) {
			bPtr = (uint8 *)(address + i);
			ch = *bPtr;
			ByteToASCII(ch, buff);
			if ((i == 7) && (i < (long)line)) {
				buff[2] = '-';
			}
			SendStr(buff, 3);
		}
		for (i = line; i < 16; i++) {
			SendStr("   ");
		}
		SendStr("  ");
		
		if (bDisplayASCII) {
			for (i = 0; i < (long)line; i++) {
				bPtr = (uint8 *)(address + i);
				ch = *bPtr;
				ch = ((ch <= 13) || (ch >= 0x7f)) ? '.' : ch;
				SendStr((char *)&ch, 1);
			}
		}

		SendStr("\r\n", 2);
		address += line;
		length -= line;
	};
}

void CDebug::ByteToASCII(uint8 ch, char *buff)
{
	buff[0] = (ch >> 4) + '0';
	buff[0] = (buff[0] > '9') ? (buff[0] + 'A' - '0' - 10) : (buff[0]);
	buff[1] = (ch & 0xf) + '0';
	buff[1] = (buff[1] > '9') ? (buff[1] + 'A' - '0' - 10) : (buff[1]);
	buff[2] = ' ';
}

void CDebug::CmdErase(unsigned long *param)
{
	int ret;
	if (*param != 1)  {
		SendStr("Address to be specified\r\n");
		return;
	}
	
	ret = m_pFlash->EraseSector(param[1]);
	if (ret == 1) {
		SendStr("OK\r\n");
		return;
	}
	if (ret == -1) {
		SendStr("Address out of range\r\n");
		return;
	}
	SendStr("Device error\r\n");
	return;
}

void CDebug::CmdBlank(unsigned long *param, uint8 against)
{
	unsigned long address, length = 0x10000;
	unsigned long i;
	uint8 *ptr;
	uint8 ch;
	char buff[10];

	if (*param < 1) {
		SendStr("range to be specified\r\n");
		return;
	}
	address = param[1];
	if (*param >= 2) {
		length = param[2];
	}
	for (i = 0; i < length; i++) {
		ptr = (uint8 *)(address + i);
		ch = *ptr;
		if (ch != against) {
			unsigned long adr = address + i;
			SendStr("Check failed @");
#if 0
			ByteToASCII((uint8)(adr >> 24), buff); SendStr(buff, 2);
			ByteToASCII((uint8)(adr >> 16), buff); SendStr(buff, 2);
			ByteToASCII((uint8)(adr >> 8), buff); SendStr(buff, 2);
			ByteToASCII((uint8)(adr), buff); SendStr(buff, 3);
#else
			LWordToASCII(adr, buff); SendStr(buff, 9);
#endif
			SendStr("is ");
			ByteToASCII(ch, buff); SendStr(buff, 3);
			SendStr("against ");
			ByteToASCII(against, buff); SendStr(buff, 2);
			SendStr("\r\n");
			return;
		}
	}
	SendStr("Check start @");
	LWordToASCII(address, buff); SendStr(buff, 9);
	SendStr("Length=");
	LWordToASCII(length, buff); SendStr(buff, 9);
	SendStr("OK\r\n");
	return;

}

void CDebug::LWordToASCII(unsigned long l, char *buff)
{
	ByteToASCII((uint8)(l >> 24), buff);
	ByteToASCII((uint8)(l >> 16), buff + 2);
	ByteToASCII((uint8)(l >> 8), buff + 4);
	ByteToASCII((uint8)(l), buff + 6);
}

void CDebug::WordToASCII(uint16 wd, char *buff)
{
	ByteToASCII((uint8)(wd >> 8), buff);
	ByteToASCII((uint8)(wd), buff + 2);
}

void CDebug::CmdFill(unsigned long *param, int8 bTryFlash, int unit)
{
	unsigned long startAddress, adr;
	unsigned long length;
	uint8 data[5];
	uint16 wData;
	unsigned long i;
	uint8 *ptr; uint16 *wPtr;
	char buff[20];
    int ret;
	unsigned long pattern_length;
    
	if (*param < 3) {
		SendStr("Improper address or data pattern\r\n");
		return;
	}
	pattern_length = *param - 2;
	startAddress = param[1];
	length = param[2];
	if ((unit == 2) && ((startAddress & 1) || (length & 1))) {
		SendStr("Address & length should be even for word operation\r\n");
		return;
	}
	if (length == 0) {
		SendStr("0 length, Nothing performed\r\n");
		return;
	}
	for (i = 0; i < length; i += unit) {
		adr = startAddress + i;
		data[0] = (uint8)param[((i%pattern_length) + 3)];
		wData = (uint16)param[(((i/2) % pattern_length) + 3)];
		if ((m_pFlash->IsWithinRange(adr) == 1) && (bTryFlash)) {
			if (unit == 1) {
				ret = m_pFlash->WriteBytes(adr, 1, data);
			} else {
				ret = m_pFlash->WriteWord(adr, wData);
			}
		} else {
			if (unit == 1) {
				ptr = (uint8 *)adr;
				*ptr = data[0];
			} else {
				wPtr = (uint16 *)adr;
				*wPtr = wData;
			}
			ret = 1;
			
		}
		if (ret != 1) {
			// Writing to address error
			SendStr("Writing @");
			LWordToASCII(adr, buff); SendStr(buff, 9);
			SendStr("=");
			if (unit == 1) {
				ByteToASCII(data[0], buff); SendStr(buff, 3);
			} else {
				ByteToASCII(wData>>8, buff); SendStr(buff, 2);
				ByteToASCII(wData, buff); SendStr(buff, 3);
			}
			SendStr("failed\r\n");
			return;
		}
	}
	SendStr("Filling from ");
	LWordToASCII(startAddress, buff); SendStr(buff, 9);
	SendStr("to ");
	LWordToASCII(startAddress + length - 1, buff); SendStr(buff, 9);
	SendStr("OK\r\n");
	return;
}               

void CDebug::CmdReset(unsigned long *param)
{
	FUNC_PTR fPtr;
#ifdef _DEBUG
	unsigned long *lPtr = (unsigned long *)0; //0x200000;
#else
	// Release version
	unsigned long *lPtr = (unsigned long *)0;
#endif
	
	g_pAll->Reboot();

	fPtr = (FUNC_PTR)((*lPtr) & 0xffffff);
	(*fPtr)();
}

void CDebug::SetPrompt(const char *pPrompt)
{
	strncpy(m_prompt, pPrompt, 20);
	m_prompt[20] = 0;
}

EXTERN void mon_startup(void);
EXTERN void RecoverIsr(void);

#pragma section MONFLAG
unsigned long g_monFlag1;
unsigned long g_monFlag2;
unsigned int  g_quitCnt;
#pragma section

void CDebug::CmdMonitor(unsigned long *param)
{
	set_exr(7);
	g_monFlag1 = DBUG_FLAG1; // 0x44425547; "DBUG"
	g_monFlag2 = DBUG_FLAG2; //
	g_quitCnt = 0; 
	RecoverIsr();
	mon_startup();	
}

EXTERN int CheckRam(unsigned long start, unsigned long len, int8 bStrict);
#if RUN_IN_ROM  
EXTERN void LoadRAMVect(void);
#endif

void CDebug::CmdCheckRam(unsigned long *param)
{
	int ret;
	char *ptr;
	char buff[50];
	int backup_tmp = m_noInterrupt;
	uint32 length = 0L;

	
	if (*param == 0) {
		uint8 tmp;
		ptr = "Syntax: <option> <loops>\r\n"
			"\tOption 1: Check 512K from 0x200000\r\n"
			"\tOption 2: Check 1M from 0x200000\r\n"
			"\tOption 3: Check 2M-128K from 0x200000\r\n"
			"\tOption 4: Check 2M from 0x200000\r\n";
		SendStr(ptr);
		tmp = get_exr();
		strcpy(buff, "EXR= 0x");
		ByteToASCII(tmp, buff + 7);
		buff[9] = 0;
		SendStr(buff);
		return;
	}
	if (*param == 2) {
		length = 0L;
		uint32 loop;
		switch (param[1]) {
		case 1:
			length = 0x80000;
			break;
		case 2:
			length = 0x100000;
			break;
		case 3:
			length = 0x200000 - 0x20000;
			break;
		case 4:
			length = 0x200000;
			break;
		}
		if (length != 0L) {
			char loopHex[10];
			uint16 failCnt = 0;
			m_noInterrupt = 1;
			SendStr("\r\nRAM checking loop started, Please wait...\r\n");

			for (loop = 0; loop < param[2]; loop++) {
				ret = CheckRam(0x200000, length, 1);
				switch (ret) {
				case 1:
					// sprintf(buff, "\rLoop: 0x%08X \r", loop);
					strcpy(buff, "\rLoop: 0x");
					WordToASCII((uint16)(loop >> 16), loopHex);
					WordToASCII((uint16)loop, loopHex + 4);
					loopHex[8] = 0;
					strcat(buff, loopHex);
					strcat(buff, " FailCnt: 0x");
					WordToASCII(failCnt, loopHex);
					loopHex[4] = 0;
					strcat(buff, loopHex);
					strcat(buff, "\r");
					ptr = buff;
					break;
				case -1:
					ptr = "\r\nRAM data error\r\n";
					failCnt++;
					if (failCnt == 0) failCnt = 0xffff;
					break;
				case -2:
					ptr = "\r\nRAM Address error\r\n";
					failCnt++;
					if (failCnt == 0) failCnt = 0xffff;
					break;
				}
				if (ret != 1) {
#if RUN_IN_ROM  
					LoadRAMVect();
#endif
				}
				SendStr(ptr);
				if (ret != 1) {
#if RUN_IN_ROM  
					LoadRAMVect();
#endif
				}
			}
		}
		SendStr("\r\nRAM test ended\r\n");
	}

	if (length == 0L) 
		SendStr("Invalid option\r\n");
	m_noInterrupt = backup_tmp;
}

void CDebug::WaitSpace()
{
	char ch = 0;
	while (m_port->Receive((char *)&ch, 1, 100) > 0);
	SendStr("Press SPACE to continue\r"); ch = 0;	
	while (ch != ' ') m_port->Receive((char *)&ch, 1, 1000);
	while (m_port->Receive((char *)&ch, 1, 100) > 0);
}
