// YModemRecv.cpp: implementation of the CYModemRecv class.
//
//////////////////////////////////////////////////////////////////////
/* 
 * YModem receiving class
 * Author: Zheng Miao
 * Initial revision on Aug 14, 2002
 * Experimental, Don't distribute.
 *
 * $Header: YModemRecv.cpp 1.2 2009/04/29 16:55:41PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include <string.h> 
#include <stdio.h>
#include "YModemRecv.h"
#include "comm.h"
#include "Storage.h"
#include "AllProcess.h"

#ifndef WIN32
#define TRACE //
#endif

#ifndef DEBUG_YMODEM
#define DEBUG_YMODEM 0
#endif

// Starting of common part between PC & Hitachi firmware
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const uint8 CYModemRecv::SOH = 0x01;
const uint8 CYModemRecv::STX = 0x02;
const uint8 CYModemRecv::EOT = 0x04;
const uint8 CYModemRecv::ACK = 0x06;
const uint8 CYModemRecv::NAK = 0x15;
const uint8 CYModemRecv::CAN = 0x18; // ^X
const uint8 CYModemRecv::C = 0x43;
const uint8 CYModemRecv::TCPHEADER = 0xff;

CYModemRecv::CYModemRecv()
{
	m_pPort = NULL;
	m_pStorage = NULL;
	m_nTry = 0;
	m_mode = 0;
	m_fileLength = 0;
	m_checksum = 0;
}

CYModemRecv::~CYModemRecv()
{

}

// Return 1 for success
// return 2 for receiving ended.
// return 3 for cancelled by sender
// return -10 for too many error.
// return -11 
int CYModemRecv::Start(Ccomm *pPort, CCodeStorage *pStorage)
{
	int ret;
	char buffer[1026];
	uint8 blockCounter;
	int32 offset = 0L;
	m_pPort = pPort;
	m_pStorage = pStorage;
	// Receive Block 0
	m_nTry = 0;
	do {
		ret = RecvBlockZero(buffer);
		if (ret == 2) {
			return 2;
		} else if (ret == 3) {
			return 3;
		}
		m_nTry++;
#if DEBUG_YMODEM
		{
			char buf[100];
			sprintf(buf, "Error in recv block 0: %d\r\n", ret);
			g_pAll->DebugOutput(buf);
		}
#endif

	} while (ret != 1);

	m_nTry = 0;
	if (m_fileLength == 0) {
		return 2; 
	}
	
	// Receive file body
	m_pPort->Send((char *)&C, 1, 0);
	m_pPort->flush();
	blockCounter = 1;
	while (1) {
		int timeout;
		uint8 block = blockCounter;
		m_pPort->flush();
		if (m_nTry > 10) {
			// Too many error
			for (int i = 0; i < 10; i++) {
				m_pPort->Send((const char *)&CAN, 1, 0);
				m_pPort->flush();
			}
			return -10;
		}
		timeout = (m_fileLength == 0) ? (3000) : (31000); // For crappy remote desk top over Symmetricom VPN. Make it 5000 to 31000 
		ret = RecvBlock(&block, buffer, timeout);
		if (ret >= 128) {
			int flash_ok = 1;
			if (block + 1 == blockCounter) {
				// Repeated block;
				m_pPort->Send((const char *)&ACK, 1, 0);
				m_nTry++;
				continue;
			}
			// block must be the same as blockCounter
			int toSave = (m_fileLength < (uint32)ret) ? (m_fileLength) : (ret);
			if (offset == 0L) {
				// Check the header
				if (CAllProcess::CheckDldHeader(buffer)) {
					m_nTry = 20;
					m_pPort->Send((const char *)&NAK, 1, 0);
					continue;
				}
			}

			if (m_fileLength != (int32)toSave) {
				m_pPort->Send((const char *)&ACK, 1, 0);
			}
			
			if (m_pStorage)
				flash_ok = m_pStorage->Write(buffer, toSave);

			m_fileLength -= toSave;
			blockCounter++;
			if (m_nTry) m_nTry--;
			if (flash_ok != 1) {
				m_nTry = 20;
			}
			offset += toSave;

			if (m_nTry > 10) {
				m_pPort->Send((const char *)&NAK, 1, 0);
			} else if (m_fileLength == 0L) {
				m_pPort->Send((const char *)&ACK, 1, 0);
			}
			continue;
		} else if (ret == 1)  {
			// It's EOT
			if (m_fileLength == 0) {
				m_pPort->Send((const char *)&ACK, 1, 0);
				m_nTry = 0;
			} else {
				// Unexpected EOT
				Purge(); 
				m_pPort->Send((const char *)&NAK, 1, 0);
				m_nTry++;
			}
			continue;
		} else if (ret == 0) {
			if (m_fileLength == 0) {
				// Normal end.
				m_pPort->flush();
				return 1;
			};
		} else if (ret == 2) {
			// Cancelled.
			m_pPort->flush();
			return 3;
		}
		// Receiving error
		m_nTry++;
		if (m_nTry > 1) Purge();
		m_pPort->Send((const char *)&NAK, 1, 0);
		
	}
	return 1;
}

// Return 1 for success
// return 2 to end receiving 
// return 3 cancelled 
int CYModemRecv::RecvBlockZero(char *buff)
{
	int ret = 1;
	//char buff[1026];
	uint8 blockNum = 0;

	if (((m_nTry + 1) % 5) == 0) {
#if 1
		// Only support CRC16 version
		m_pPort->Send((const char *)&C, 1, 0);
		m_mode = MODE_CRC16;
#else
		// Support checksum option
		m_pPort->Send((const char *)&NAK, 1, 0);
		m_mode = MODE_CHECKSUM;
#endif
	} else {
		m_pPort->Send((const char *)&C, 1, 0);
		m_mode = MODE_CRC16;
	}
	m_pPort->flush();
	ret = RecvBlock(&blockNum, buff);
	
	if (blockNum != 0) return -1;

	if ((ret == 128) || (ret == 1024)) {
		// Analyze block 0
		m_pPort->Send((char *)&ACK, 1, 0);
		m_pPort->flush();
		if (buff[0] == 0) {
			// Filename is null
			return 2;
		}
		// Filename start from buff[0]
		int len;
		len = strlen(buff);
		// Get File length
		{
			uint8 *ptr = (uint8 *)(buff + len + 1);
			m_fileLength = 0;
			while (*ptr != ' ') {
				if ((*ptr >= '0') && (*ptr <= '9')) {
					m_fileLength = m_fileLength * 10 + *ptr - '0';
					ptr++;
				} else {
					break;
				}
			} 
		}
		if (m_fileLength >= MAX_CODE_SIZE) {
			return ret - 200;
		}
		// Modification date
		// mode
		// Serial number
		
	} else if (ret == 2) {
		return 3;
	} else {
		return (ret-200);
	}
	return 1;
}

// 1 for EOT
// 2 for two continous CAN
// 0 for receiving nothing
// -1 .. -99 receive error
// -100 out of sequence
// blocknum is the expected block number
// return length received if success
int CYModemRecv::RecvBlock(uint8 *blocknum, char *pBlock, int timeout)
{
	int ret = 0;
	int len = 0, i, chklen;
	uint8 head = 0;
	uint8 squence[2];

	// Get First byte
	int loop = 0;
	do {
		loop = 0;
		ret = m_pPort->Receive((char *)&head, 1, timeout);
		if (ret != 1) {
			if (Purge()) 
				Purge(); 
			return 0;
		}
		switch (head) {
		case STX: len = 1024; break;
		case SOH: len = 128; break;
		case EOT: 
			return 1;
		case CAN:
			ret = m_pPort->Receive((char *)&head, 1, 1000);
			if ((ret == 1) && (head == CAN)) {
				// Cancelled
				return 2;
			}
			len = 0;
			break;
		case TCPHEADER: {
				char tmp;
				for (i = 0; i < 2; i++) {
					m_pPort->Receive(&tmp, 1, 3000);
				}
				loop = 1;
				len = 0;
			}
			break;
		default: len = 0; break;
		}
	} while (loop);

	if ((ret != 1) || (len == 0)) {
		Purge(); Purge(); return -1;
	}

	// Get sequence
	for (i = 0; i < 2; i++) {
		ret = m_pPort->Receive((char *)(squence + i), 1, 3000);
		if (ret != 1) {
			Purge(); return -2;
		}
	}
	if ((squence[0] + squence[1]) != 0xff) {
		Purge(); return -3;
	}
	if (squence[0] != *blocknum) {
		if ((squence[0] + 1) == *blocknum) {
			// previous block
			*blocknum = squence[0];
		} else {
			// Complete out of squence
			m_pPort->Send((const char *)&CAN, 1, 0);
			m_pPort->Send((const char *)&CAN, 1, 0);
			m_pPort->flush();
			return -100;
		}
	}

	// Get data body
	chklen = (m_mode == MODE_CRC16) ? (2) : (1);
	m_crcCheck.Reset();
	m_checksum = 0;
	for (i = 0; i < len + chklen; i++) {
		ret = m_pPort->Receive(pBlock + i, 1, 3000);
		if (ret != 1) {
			Purge(); return -4;
		}
		if (i < len) {
			uint8 ch;
			ch = (uint8)(*(pBlock+i));
			m_checksum += ch;
			m_crcCheck.UpdateCrc(ch);
		}	
	}
	switch (m_mode) {
	case MODE_CRC16: {
		uint16 crc;
		uint8 *ptr = (uint8 *)(pBlock + len);
		crc = (((uint16)*ptr) << 8) + *(ptr+1);
		if (crc == m_crcCheck.GetCrc16()) {
			// CRC16 check sum passed
			return len;
		}
#if DEBUG_YMODEM
		{
			char buff[100];
			sprintf(buff, "CRC error. Expected = 0x%X, Received = 0x%X\r\n",
				m_crcCheck.GetCrc16(), crc); 
			g_pAll->DebugOutput(buff);					
		}
		return len; // Skip checksum
#endif 
		// Crc16 check sum failed;
		return -10;
		}
		break;
	case MODE_CHECKSUM: {
		uint8 *ptr = (uint8 *)(pBlock + len);
		if ((*ptr + m_checksum) == 0xff) {
			return len;
		}
		// Checksum failed.
		return -11;
		}
		break;
	}
#if DEBUG_YMODEM
		{
			char buff[100];
			sprintf(buff, "Unknown mode: %d\r\n",
				m_mode); 
			g_pAll->DebugOutput(buff);					
		}
#endif 
	return -1;
}

int CYModemRecv::Purge()
{
	unsigned char buff;
	int ret;
	int cnt = 0;

	m_pPort->flush();
	do {
		ret = m_pPort->Receive((char *)&buff, 1, 1000);
		if (ret == 1) {
			cnt++;
			if (cnt == 1) {
				TRACE("Bytes purged\r\n");
			}
			TRACE("%02X ", buff);
		}
	} while (ret ==1);
	TRACE("[%d] bytes purged\r\n", cnt);
	return cnt;
}


