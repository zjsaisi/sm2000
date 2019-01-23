/*                   
 * Filename: Debug.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Debug.cpp 1.2 2009/04/30 16:39:30PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */                 
 
//#include "project.h"
#include "machine.h"
#include "comm.h"
#include "flash.h"
#include "Debug.h"
#include "string.h"
#include "AllProcess.h"
#include "stdio.h"

CDebug::CDebug(void)
{          
	m_cmdBuff[0] = 0;
	m_port = NULL;
	m_currentAddress = 0x800000;
	strcpy(m_prompt, "\r\nDebug >");
	m_bAutoRateAllowed = 0;
	m_bEcho = 1;
	m_pFlash = NULL;
}

CDebug::~CDebug(void)
{
}

const CDebug::CmdStruct CDebug::m_debugCmdList[] = {
	{"HELP ", CMD_HELP, "HELP\t? <option>\t: List commands available"},
	{"? ", CMD_HELP, NULL},
	{"DUMP ", CMD_DUMP, "DUMP\tD <range>\t: Dump memory content(two reading per byte)"}, 
	{"D ", CMD_DUMP, NULL},
	{"READ ", CMD_READ, "READ\tR <range>\t: Similar to DUMP without character(One read per byte)"},
	{"R ", CMD_READ, NULL},
	{"ERASE ", CMD_ERASE, "ERASE\tE <address>\t: Erase the external flash sector pointed by address"},
	{"BLANK ", CMD_BLANK, "BLANK\tB <range>\t: Verify if all are 0xff"},
	{"B ", CMD_BLANK, NULL},
	{"FILL ", CMD_FILL, "FILL\tF <range> <bytes>: Fill the memory with data pattern, can write to flash"},
	{"F ", CMD_FILL, NULL},
	{"W ", CMD_WRITE_SINGLE, "W\t <address> <byte>: Write byte to address"}, 
	{"WRITEL ", CMD_WRITE, "WRITEL\tWL <range> <words>: Similar to FILL except it won\'t write to flash"},
	{"WL ", CMD_WRITE, NULL},
	{"FW ", CMD_FILL_WORD,"FW\t<range> <wData>\t: FILL in word(16-bit)"},
	{"WW ", CMD_WRITE_WORD, "WW\t<range> <wData>\t: WRITE in word(16-bit)"},
	{"WD ", CMD_WRITE_DOUBLE, "WD\t <adr> <data>\t: Write double word(32-bit)"},
	{"RESET ", CMD_RESET, "RESET\tBOOT, REBOOT\t: Software restart"},
	{"REBOOT ", CMD_RESET, NULL },
	{"BOOT ", CMD_RESET, NULL },
	{"QBOOT ", CMD_QUICK_RESET, "QBOOT\t: Quick Software restart"},
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
			//CmdCheckRam(param);
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
		case CMD_QUICK_RESET:
			CmdQuickReset(param);
			break;
		case CMD_WRITE_DOUBLE:
			if (param[0] == 2) {
				*((uint32 *)param[1]) = param[2];
			}
			break;
    	default:
			SendStr("To be implemented\r\n");
    		break;
    	}
    } while (1);  
	return 1;
}

// See if the parameter can be interpreted as string
char *CDebug::GetParamStr(uint32 param)
{
	SParam *ptr;
	if (param & 1) return NULL; // Has to on 16-bit address to be a pointer;
	ptr = (SParam *)param;
	if (ptr->type == PARAM_TYPE_STRING) {
		return ptr->pStr;
	}
	return NULL;
}

int8 CDebug::SetEcho(int8 echo)
{
	int8 old = m_bEcho;
	m_bEcho = echo;
	return old;
}

// Return 1 for success
// 2 for extended command
int CDebug::ReadCmd(int *pCmd, unsigned long *param)
{               
	int index = 0, ret, i, eq;
	uint8 ch;
	int8 bPrompt = 1;
	unsigned long argc = 0;                                 
	int cmdLen;
	int curCmd;
	int error;

	m_port->ResetStopError();

 	do {            
 		if (bPrompt) {
			SendStr(m_prompt);
			m_port->flush();
			bPrompt = 0;
 		}
		curCmd = 1;
		ch = 0;
		ret = m_port->Receive((char *)&ch, 1, 1000); // 5/17/2004: This is only needed by boot code. Recv((char *)&ch, 1, 1000, m_noInterrupt);
		
		error = m_port->GetStopError();

		if (m_bAutoRateAllowed) {
			if (error >= 1) {
				int tmp;
				BaudrateGoDown();
				// Read any residual incoming data
				do {
					tmp = m_port->Receive((char *)&ch, 1, 100); // 5/17/2004: Get rid of this. only needed for boot code. Recv((char *)&ch, 1, 100, m_noInterrupt);
				} while (tmp == 1);
				m_port->ResetStopError();
				continue;
			//} else if ((ch & 0xfc) == 0xfc) {
			//	BaudrateGoUp();
			//	continue;
			} else if ((ch & 0x80) == 0x80) {
				int tmp;
				tmp = m_port->Receive((char *)&ch, 1, 100); // 5/17/2004: This is only needed by boot code. Recv((char *)&ch, 1, 100, m_noInterrupt);
				if (tmp == 1) {
					// there is immediate data follwing. One byte is broken into two or more
					BaudrateGoDown();
					continue;
				}
				BaudrateGoUp();
				continue;
			}
		}

		if (ret != 1) {
 			continue;
 		}
		if (ch == KEY_BS) {
			if (index) index--;
			if (m_bEcho) 
				SendStr("\010 \010", 3);
			continue;
		}
		if (m_bEcho) {
			SendStr((char *)&ch, 1);
			m_port->flush();
		}

		// To add string parameter, don't do conversion
 		// if ((ch >= 'a') && (ch <= 'z')) ch = ch - 'a' + 'A';
 		
		m_cmdBuff[index++] = ch;
 		if ((index >= 99) || (ch == '\r') || (ch == '\n')) {
 			if (index == 1) { // NULL command
 				bPrompt = 1;
 				index = 0;
 				continue;
 			} 
 			SendStr("\r\n\000", 2); 			           
 			m_cmdBuff[index-1] = ' ';
 			m_cmdBuff[index] = 0;
			// Delete the starting space
			{
				// A patch
				char *ptr;
				int len = index + 1;
				ptr = m_cmdBuff;
				while (*ptr == ' ') { 
					ptr++;
					len--;
				}
				if (ptr != m_cmdBuff) {
					memmove(m_cmdBuff, ptr, len);
				}
				if (m_cmdBuff[0] == 0) {
					bPrompt = 1;
					index = 0;
					continue; // Empty command
				}
			}

			// Convert command name to upper case
			for (i = 0; i < 32; i++) {
				ch = m_cmdBuff[i];
				if ((ch >= 'a') && (ch <= 'z')) {
					m_cmdBuff[i] = ch - 'a' + 'A';
					continue;
				}
				if ((ch == ' ') || (ch == 0)) break;
			}

 			// Find command 
 			i = 0;
 			while (m_debugCmdList[i].cmdId != CMD_END) {     
 				cmdLen = strlen(m_debugCmdList[i].pCmdName);
 				eq = strncmp(m_cmdBuff, m_debugCmdList[i].pCmdName, cmdLen);
 				if (eq == 0) break; 
 				i++;
 			}   
 			*pCmd = m_debugCmdList[i].cmdId;
			if (*pCmd == CMD_END) {
				*pCmd = ExtendedGetCmd(m_cmdBuff, &cmdLen);
				curCmd = 2;
			}
 			if (*pCmd == CMD_END) {
 				SendStr("\r\nInvalid command!");
 				bPrompt = 1;
 				index = 0;
 				continue;
 			}

 			// convert parameter

			// Clear string parameter list,
			m_strParamIndex = 0;
			for (i = 0; i < NUM_STRING_PARAM; i++) {
				m_strParam[i].type = 0;
				m_strParam[i].pStr = NULL;
			}

 			argc = 0;
 			char *ptr;
 			int8 bDataError = 0;
 			unsigned long base = 10;
 			long lTmp = 0;
			long sign;
			int digits;
 			ptr = m_cmdBuff + cmdLen;
 			i = 0;
 			while (*ptr != 0) {
 				lTmp = 0;
				digits = 0;
 				base = 10;
 				i++;
				sign = 1;
 			    while ((*ptr == ' ') || (*ptr == '\t')) {
 			    	ptr++;
 			    }    
				if (*ptr == 0) break;

				// convert a string
				if ((*ptr == '\"') || (*ptr == '\'')) {
					char delimit = *ptr++;
					char *pConverted;
					int converting = 0;
					int len = 0;
					m_strParam[m_strParamIndex].pStr = ptr;
					pConverted = ptr;
					while (((*ptr != delimit) && (*ptr != 0)) 
						|| (converting)) {
						// Get the string
						if (converting) {
							char ch;
							converting = 0;
							switch (*ptr) {
							case 'r':
								ch = '\r';
								break;
							case 'n':
								ch = '\n';
								break;
							case 't':
								ch = '\t';
								break;
							default:
								ch = *ptr;
								break;
							}
							*pConverted = ch;
							pConverted++;
							len++;
						} else if (*ptr == '\\') {
							converting = 1;
						} else {
							len++;
							*pConverted = *ptr;
							pConverted++;
						}
						ptr++;
					}
#if 0		// Allow null string
					if (len <= 0) {
						ptr++; // 0 length. Skip this one
						continue;
					}
#endif
					*pConverted = 0; //*ptr = 0;
					ptr++;
					if (m_strParamIndex >= NUM_STRING_PARAM) {
						// too many string parameter
						bDataError = 1;
						break;
					}
					m_strParam[m_strParamIndex].type = PARAM_TYPE_STRING;
					param[i] = (uint32)(m_strParam + m_strParamIndex);
					argc++;
					m_strParamIndex++;
					continue;
				}

				// Convert a number parameter
				if ((*ptr == '+') || (*ptr == '-')) {
					sign = (*ptr == '-') ? (-1) : 1;
					ptr++;
				}
 			    if ((*ptr == '0') && ((ptr[1] == 'X') || (ptr[1] == 'x')) ) {
 			    	ptr += 2; base = 16;
 			    }                                   
 			    while ((*ptr != 0) && (*ptr != ' ') && (*ptr != '\t')) {
 			    	if ((*ptr >= 'A') && (*ptr <= 'F') && (base == 16)) {
 			    		lTmp = lTmp * base + *ptr - 'A' + 10;
						digits++;
					} else if ((*ptr >= 'a') && (*ptr <= 'f') && (base == 16)) {
 			    		lTmp = lTmp * base + *ptr - 'a' + 10;
						digits++;
					}else if ((*ptr >= '0') && (*ptr <= '9')) {
 			    		lTmp = lTmp * base + *ptr - '0';
						digits++;
 			    	} else {
 			    		bDataError = 1;
						break;
 			    	}
 			    	ptr++;                                     
 			    }
				// No digit are treated as error
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
	if (*param == 0) {
		char buff[100];
		SendStr("Debug command list:\r\n");
		while (ptr->cmdId != CMD_END) {
			if (ptr->pComment) {
				SendStr(ptr->pComment);
				SendStr("\r\n");
			}
			ptr++;
		}
		ExtendedPrintHelp(param);
		// K_Task_Wait(100);
		SendStr("Data format:\r\n");
		SendStr("\tDecimal data and Hex starting with 0x are supported.\r\n");
		SendStr("\t<range>: <start address> <len>, len is in byte\r\n");
		SendStr("\t<data>: List of data in byte\r\n");
		SendStr("\t<wData>: List of data in word\r\n");
		sprintf(buff, "Stop Error: %d\r\n", m_port->GetStopError());
		SendStr(buff);
	} else {
		ExtendedPrintHelp(param);
		// Clear it here in a Lazy way
		m_port->ResetStopError();
	}
}

void CDebug::CmdDump(unsigned long *param, int8 bDisplayASCII)
{
	unsigned long address;
	unsigned long length = 0x80;
#if 0
	uint8 *bPtr;
	char buff[30];
	int i;
#endif
	if (*param >= 1) {
		m_currentAddress = param[1];
	}
	if (*param >= 2) {
		length = param[2];
	}
	address = m_currentAddress;
	m_currentAddress = address + length;
	
#if 1
	while (length > 0) {
		char dispBuff[300];
		int block;
		block = (length > 16) ? (16) : (length);
		PrintLine(dispBuff, address, block, (uint8 *)address, bDisplayASCII);
		SendStr(dispBuff);
		address += block;
		length -= block;
	}
#else
	// SendStr("\r\n\000");
	// SendStr("Dumping data from memory\r\n");
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
				ch = ((ch <= 0x1f) || (ch >= 0x7f)) ? '.' : ch;
				SendStr((char *)&ch, 1);
			}
		}

		SendStr("\r\n", 2);
		address += line;
		length -= line;
	};
#endif
}

void CDebug::ByteToASCII(uint8 ch, char *buff)
{
	buff[0] = (ch >> 4) + '0';
	buff[0] = (buff[0] > '9') ? (buff[0] + 'A' - '0' - 10) : (buff[0]);
	buff[1] = (ch & 0xf) + '0';
	buff[1] = (buff[1] > '9') ? (buff[1] + 'A' - '0' - 10) : (buff[1]);
	buff[2] = ' ';
}

// (12/21/2004) This function should be in CmdDump. Later.
void CDebug::PrintLine(char *pBuff, uint32 address, int16 len, uint8 *pContent, int8 bDisplayASCII /*= 1*/)
{
	int16 i;
	uint8 ch;
	// Print address
	ByteToASCII(address >> 16, pBuff);
	ByteToASCII(address >> 8, pBuff + 2);
	ByteToASCII(address, pBuff + 4);
	pBuff += 7;
	for (i = 1; i <= len; i++) {
		ByteToASCII(*(pContent + i - 1), pBuff);
		if (((i % 8) == 0) && (i != len)) {
			pBuff[2] = '-';
		}
		pBuff += 3;
	}
	if (len < 16) {
		int16 fill;
		fill = 3 * (16 - len);
		memset(pBuff, ' ', fill);
		pBuff += fill;
	}
	*pBuff++ = ' ';
	*pBuff++ = ' ';
	if (bDisplayASCII) {
		for (i = 0; i < len; i++) {
			ch = *(pContent+ i);
			if ((ch <= 0x1f) || (ch >= 0x7f)) {
				ch = '.';
			}
			*pBuff++ = ch;
		}
	}
	*pBuff++ = '\r';
	*pBuff++ = '\n';
	*pBuff = 0;
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
	CAllProcess::g_pAllProcess->RebootSystem(0, m_port);
}

void CDebug::CmdQuickReset(unsigned long *param)
{
	CAllProcess::g_pAllProcess->RebootSystem(1, m_port);
}

int CDebug::ReadStr(char *ptr, int max_len, int8 trim)
{
	int i, ret, len;
	char *pBuff;
	
	pBuff = ptr;

	m_port->flush();
	for (i = 0; i < max_len;) {
		ret = m_port->Receive(ptr, 1, 1000);
		if (ret == 1) {
			if (*ptr == '\r') {
				break;
			}
			if (*ptr == KEY_BS) {
				if (i) { 
					i--; ptr--; 
					if (m_bEcho) {
						SendStr("\010 \010", 3);
					}
				}
				continue;
			}
			if (m_bEcho) {
				SendStr(ptr, 1);
			}
			i++; ptr++;			
		}
	}
	*ptr = 0;
	SendStr("\r\n");
	len = i;

	// If need to trim the space from the starting and at the end
	if (trim) {
		ptr = pBuff;
		// Find the starting space
		while (*ptr == ' ') {
			ptr++;
			len--;
		}
		// Trim the start space if any
		if (ptr != pBuff) {
			i = 0;
			while (1) {
				pBuff[i] = *ptr;
				if (*ptr == 0) break;
				i++; ptr++;
			}
		}
		// Trim space at the end
		while ((len > 0) && (pBuff[len-1] == ' ')) {
			len--;
		}
		pBuff[len] = 0;
	}
	return len;
}


void CDebug::SetPrompt(const char *pPrompt)
{
	strncpy(m_prompt, pPrompt, 20);
	m_prompt[20] = 0;
}

void CDebug::WaitSpace()
{
	char ch = 0;
	while (m_port->Receive((char *)&ch, 1, 100) > 0);
	SendStr("Press SPACE to continue\r"); ch = 0;
	while (ch != ' ') m_port->Receive((char *)&ch, 1, 1000);
	while (m_port->Receive((char *)&ch, 1, 100) > 0);
}
