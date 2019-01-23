// ZModemRecv.cpp: implementation of the CZModemRecv class.
//
//////////////////////////////////////////////////////////////////////

// MFC specific
#include "stdafx.h"

#include "ZModemRecv.h"
#include "comm.h"
#include "Storage.h"
#include "CRC16.h"
#include "CRC32.h"
#include <stdio.h>
#include <string.h>
#include "PV.h"
// #include <assert.h>

#ifdef WIN32
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif


#ifndef WIN32
#define TRACE //
#define assert //
#endif

#define CHAR_XON	0x11
#define CHAR_XOFF	0x13
#define CHAR_CR	    0xd
#define CHAR_LF		0xa
#define CHAR_SOH	1
#define CHAR_STX	2
#define CHAR_ETX    3
#define CHAR_DLE	0x10
#define CHAR_CAN	0x18
#define CHAR_PAD	0x2a
  
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const char zmodem_test_data[20] = {
	0, 0x18, 0x18, '*', '*', 0x18, 1, 0, 0 , 0 , 
	0x18, 0x18, 1, 2, 3, 4, 5, 6, 7, 8 
};

void CZModemRecv::Dummy(void)
{
	m_inputHead = 0;
	for (int i = 0; i < zmodem_test_data[0]; i++) {
		m_inputHead++;
	}
}

CZModemRecv::CZModemRecv()
{
	Dummy();
	m_inputBuffer[0] = 0;
	m_inputHead = m_inputTail = 0;
	m_pPort = NULL;
	m_pStorage = NULL;
	m_state = STATE_INIT;
	m_currentRecvPacketType = PACKET_HEX16;
	m_filePosition = 0;
	m_endOfSession = SESSION_START;
	m_currentErrorCnt = 0;
	m_totalErrorCnt = 0;
	m_contentStarted = 0;
	m_cancelRequest = 0;

}

CZModemRecv::~CZModemRecv()
{

}

void CZModemRecv::ClearInputBuffer(void)
{
	m_inputHead = m_inputTail = 0;
	m_inputBuffer[INPUT_BUFFER_SIZE-1] = 0;
}

int CZModemRecv::Start(Ccomm *pPort, CStorage *pStorage)
{
	int ret = -1;
	int result;
	m_pPort = pPort;
	m_pStorage = pStorage;
	m_totalErrorCnt = 0;
	m_currentErrorCnt = 0;
	m_contentStarted = 0;
	m_cancelRequest = 0;

	ClearInputBuffer();
	m_endOfSession = SESSION_START;
	SendReceiveCapability();
	while (1) {
		result = WaitPacket(1000);
		switch (result) {
		case PACKET_BIN16:
			m_currentRecvPacketType = PACKET_BIN16;
			result = DecodeBin16Packet(5000);
			if (result < 0) {
				TRACE("Decoding Bin16 packet failed %d\r\n", result);
				m_totalErrorCnt++;
				m_currentErrorCnt++;
			} else {
				m_currentErrorCnt = 0;
			}
			break;
		case PACKET_HEX16:
			m_currentRecvPacketType = PACKET_HEX16;
			result = DecodeHex16Packet(3000);
			if (result < 0) {
				TRACE("Decoding Hex16 packet failed %d\r\n", result);
				m_totalErrorCnt++;
				m_currentErrorCnt++;
			} else {
				m_currentErrorCnt = 0;
			}
			break;
		case PACKET_BIN32:
			m_currentRecvPacketType = PACKET_BIN32;
			result = DecodeBin32Packet(3000);
			if (result < 0) {
				TRACE("Decoding Bin32 packet failed %d\r\n", result);
				m_totalErrorCnt++;
				m_currentErrorCnt++;
			} else {
				m_currentErrorCnt = 0;
			}
			break;
		}
		if (m_endOfSession >= SESSION_COMPLETE) {
			TRACE("Sesssion done: Err: %ld %ld\r\n"
				,m_totalErrorCnt ,m_currentErrorCnt);
			ret = 1;
			break;
		}
		if (m_totalErrorCnt > MAX_TOTAL_ERROR) {
			TRACE("Too many error: %ld\r\n", m_totalErrorCnt);
			ret = -1;
			break;
		}
		if (m_currentErrorCnt > MAX_CURRENT_ERROR) {
			TRACE("Too many bursty error: %ld\r\n", m_currentErrorCnt);
			ret = -2;
			break;
		}
	}

	// absorb the "OO" and other possible garbage
	m_pPort->Receive((char *)m_inputBuffer, 100, 500);
	return ret;
}


int CZModemRecv::DecodeBin16Packet(long timeout)
{
	return DecodeBinaryPacket(timeout, 16);
}

int CZModemRecv::DecodeBin32Packet(long timeout)
{
	return DecodeBinaryPacket(timeout, 32);
}

int CZModemRecv::DecodeBinaryPacket(long timeout, int crcbits)
{
	CCRC16 crc16;
	CCRC32 crc32;
	int cnt = 0;
	int escape = 0;
	int len;
	unsigned char ch;
	int max = 7 + ((crcbits == 32) ? (2) : (0));
	unsigned char frame[100];
	unsigned char type;
	unsigned long parameter;

	while (1) {
		while (GetInputSize() < 1) {
			len = ReadLink(timeout, 1);
			if (len <= 0) {
				TRACE("Recv bin pack timeout, fileposition: %ld\r\n", m_filePosition);
				return -1;
			}
		}
		ch = m_inputBuffer[m_inputHead];
		if (escape) {
			switch (ch) {
			case 'l':
				ch = 0x7f;
				break;
			case 'm':
				ch = 0xff;
				break;
			default:
				ch ^= 0x40;
				break;
			}
			escape = 0;
		} else {
			if (ch == CHAR_CAN) {
				escape = 1;
				m_inputHead = NextIndex(m_inputHead);
				continue;
			}
		}
		frame[cnt] = ch;
		cnt++;
		if (crcbits == 16) {
			crc16.UpdateCrc(ch);
		} else {
			crc32.UpdateCRC32(ch);
		}
		m_inputHead = NextIndex(m_inputHead);
		if (cnt >= max) {
			break;
		}
	}

	if (crcbits == 16) {
		if (crc16.GetCrc16() != 0) {
			TRACE("CRC error in bin16 packet FilePosition:%ld InputBuffSize:%d\r\n"
				, m_filePosition, GetInputSize());
			return -100;
		}
	} else {
		if (crc32.GetCRC32() != 0) {
			TRACE("CRC error in bin32 packet FilePosition:%ld\r\n", m_filePosition);
			return -200;
		}
	}

	type = frame[0];
	parameter = frame[1] + ((unsigned long)frame[2] << 8) + ((unsigned long)frame[3] << 16) 
		+ ((unsigned long)frame[4] << 24);
	return ProcessPacket(type, parameter, timeout);
}

int CZModemRecv::DecodeHex16Packet(long timeout)
{
	int len;
	int next;
	char buff[10];
	int ch;
	int cnt = 0;
	unsigned char frame[100];
	CCRC16 crc;
	unsigned char type;
	unsigned long parameter;

	while (1) {
		while (GetInputSize() < 2) {
			len = ReadLink(timeout, 2);
			if (len <= 0) return -1;
		}
		buff[0] = m_inputBuffer[m_inputHead];
		next = NextIndex(m_inputHead);
		buff[1] = m_inputBuffer[next];
		m_inputHead = NextIndex(next);
		ch = ConvertHex(buff);
		if (ch < 0) return -2;
		crc.UpdateCrc((uint8)ch);
		frame[cnt] = (unsigned char)ch;
		cnt++;
		if (cnt >= 7) {
			unsigned short checksum;
			checksum = crc.GetCrc16();
			if (checksum != 0) {
				return -3;
			}
			break;
		}
	}
	
	type = frame[0];
	parameter = frame[1] + ((unsigned long)frame[2] << 8) + ((unsigned long)frame[3] << 16) 
		+ ((unsigned long)frame[4] << 24);
	return ProcessPacket(type, parameter, timeout);
}

int CZModemRecv::ProcessPacket(unsigned char type, unsigned long parameter, long timeout)
{
	int result;
	int len;

	TRACE("Processing Packet type: %d InputPipe=%ld\r\n", type, GetInputSize());

	switch (type) {
	case PACKET_TYPE_ZRQINIT:
		SendReceiveCapability();
		break;
	case PACKET_TYPE_ZFILE: 
		m_filePosition = 0;
		while (1) {
			len = 0;
			result = ReceiveData(NULL, timeout, &len);
			switch (result) {
			case 1: // ZCRCE
				m_contentStarted = 1;
				break;
			case 2: // ZCRCG
				m_contentStarted = 1;
				break;
			case 3: // ZCRCQ
				m_contentStarted = 1;
				SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
				return 3;
			case 4: // ZCRCW
				m_contentStarted = 1;
				SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
				break;
			case 0:
				return 0;
			default:
				return -1;
			}
		}
		break;
	case PACKET_TYPE_ZDATA:
		if (m_filePosition != (int32)parameter) {
			ClearInputBuffer();
			SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
			return -1;
		}
		while (1) {
			len = 0;
			result = ReceiveData(m_pStorage, timeout, &len);
			switch (result) {
			case 1: // ZCRCE
				m_filePosition += len;
				m_currentErrorCnt = 0;
				//SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
				break;
			case 2: // ZCRCG
				m_filePosition += len;
				m_currentErrorCnt = 0;
				//SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
				break;
			case 3: // ZCRCQ
				m_filePosition += len;
				TRACE("ZCRCQ File position: %ld\r\n", m_filePosition);
				SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
				m_currentErrorCnt = 0;
				break;
			case 4: // ZCRCW
				m_filePosition += len;
				TRACE("ZCRCW File Position: %ld\r\n", m_filePosition);
				SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
				m_currentErrorCnt = 0;
				break;
			case 0:
				// SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
				if (0) {
					char buff[100];
					sprintf(buff, "result is zero %d\r\n", result);
					m_pPort->Send(buff, -1, 0);
				}
				return 0;
			default:
				if (0) {
					char buff[100];
					sprintf(buff, "result is %d\r\n", result);
					m_pPort->Send(buff, -1, 0);
				}
				ClearInputBuffer();
				SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
				return -1;
			}
		}
		break;
	case PACKET_TYPE_ZEOF:
		TRACE("ZEOF parameter=%ld\r\n", parameter);
		if ((int32)parameter != m_filePosition) {
			SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
			return -1;
		}
		m_endOfSession = SESSION_EOF;
		SendReceiveCapability();
		break;
	case PACKET_TYPE_ZFIN:
		if (m_endOfSession != SESSION_EOF) {
			SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
			return -1;
		}
		SendBPacket(PACKET_TYPE_ZFIN, 0);
		m_endOfSession = SESSION_COMPLETE;
		TRACE("File length : %ld\r\n", m_filePosition);
		break;
	default:
		TRACE("Do not know packet type %d 0x%08lX\r\n", type, parameter);
		return -100;
	}
	return 1;
}



int CZModemRecv::ReceiveData(CStorage *pStore, long timeout, int *pLength) 
{
	CCRC16 crc16;
	CCRC32 crc32;
	register unsigned char ch;
	int escape = 0;
	char hex[10];
	int hexIndex = 0;
	int len;
	int max = 1524;
	int cnt = 0;
	int proceed = 1;
	int crcStart = 0;
	unsigned char dataBuff[1532];
	int dataSize = 0;
	int linkBufferSize = 64;
	int idleCnt = 0;
	int reAckCnt = 0;

	TRACE("Start to receive data\r\n");
	while (1) {
		// proceed = 1;
		if (m_inputHead == m_inputTail) /*(GetInputSize() < 1) */ {
			linkBufferSize = m_pPort->GetRxSize();
			if (linkBufferSize < 0) {
				linkBufferSize = INPUT_BUFFER_SIZE;
			} else if (linkBufferSize == 0) {
				linkBufferSize = INPUT_BUFFER_SIZE >> 1; // zmiao: 5/27/2008 weird. It cannot be 1
				CPVOperation::Pause(1);
				idleCnt += 1000 / TICK_1SEC;
				reAckCnt += 1000 / TICK_1SEC;
				if (idleCnt > timeout) return -2;
				if (reAckCnt > 1500) {
					SendBPacket(PACKET_TYPE_ZACK, m_filePosition);
					reAckCnt = 0;
				}
				continue;
			} else if (linkBufferSize > INPUT_BUFFER_SIZE) {
				linkBufferSize = INPUT_BUFFER_SIZE;
			} else if (linkBufferSize > 2) {
				linkBufferSize -= 1;
			}
			len = ReadLink(timeout, /* INPUT_BUFFER_SIZE */ linkBufferSize );
			if (len <= 0) {
				return -1;
			}
			continue;
		} 
		idleCnt = 0;
		reAckCnt = 0;

		ch = m_inputBuffer[m_inputHead];

		if ((escape) && ((ch >='A') && (ch <= 'C'))) {
			int back;
			unsigned char bch;
			back = m_inputHead + INPUT_BUFFER_SIZE - 2;
			back %= INPUT_BUFFER_SIZE;
			bch = m_inputBuffer[back];
			if (bch == CHAR_PAD) {
				// Frame header found. go back
				m_inputHead = back;
				TRACE("Packet started unexpectedly File Offset=%ld dataSize=%ld inputHead=%d\r\n"
					, m_filePosition ,dataSize, m_inputHead );
				return 0;
			}
		}
		m_inputHead = NextIndex(m_inputHead);

		switch (m_currentRecvPacketType) {
		case PACKET_BIN16:
			if (escape) {
				escape = 0;
				switch(ch) {
				case 'l':
					ch = 0x7f;
					break;
				case 'm':
					ch = 0xff;
					break;
				case 'h':
					crcStart = 1;
					max = 2;
					proceed = 0;
					cnt = 0;
					break;
				case 'i':
					crcStart = 2;
					max = 2;
					proceed = 0;
					cnt = 0;
					break;
				case 'j':
					crcStart = 3;
					max = 2;
					proceed = 0;
					cnt = 0;
					break;
				case 'k':
					crcStart = 4;
					max = 2;
					proceed = 0;
					cnt = 0;
					break;
				default:
					ch ^= 0x40;
					break;

				}
				crc16.UpdateCrc(ch);
				if (proceed) {
					cnt++;
					dataBuff[dataSize++] = ch;
				}
				proceed = 1;
			} else if (ch == CHAR_CAN) {
				escape = 1;
				// proceed = 0;
			} else {
				crc16.UpdateCrc(ch);
				cnt++;
				dataBuff[dataSize++] = ch;
			}
			break;
		case PACKET_HEX16:
			hex[hexIndex++] = ch;
			hexIndex %= 2;
			break;
		case PACKET_BIN32:
			break;
		}
			
		if (cnt >= max) {
			uint32 crcResult;
			if (!crcStart) {
				// something wrong.
				TRACE("CRC lost %d\r\n", m_currentRecvPacketType);
				return -100; // Should send nak.
			}
			dataSize -= max;

			if (m_currentRecvPacketType == PACKET_BIN32) {
				crcResult = crc32.GetCRC32();
			} else {
				crcResult = (uint32)crc16.GetCrc16();
			}
			if (crcResult != 0) {
				TRACE("CRC error in data %d\r\n", m_currentRecvPacketType);
			}

			// Save data if needed
			if (dataSize > 0) {
				if (pStore) {
					TRACE("Saving received data: %ld offset:%ld(%lx)\r\n"
						, dataSize, m_filePosition, m_filePosition);
					TRACE("Input pipe: %d\r\n", GetInputSize());
					pStore->Write((char *)dataBuff, dataSize);
					TRACE("\r\n");
				}
			}
			
			max = 1524; 

			if (crcStart) {
				TRACE("Data receive %d crc=%d\r\n", dataSize, crcStart);
				*pLength = dataSize;
				return crcStart;
			}

			dataSize = 0;
			crcStart = 0;
		}
	}

	return -1000; // should never be here.
}

int CZModemRecv::WaitPacket(long timeout)
{
	int len;
	int next;
	int32 bytesDropped = 0;
	int dropMask = 0xf;
	int32 shoutBack = 0;
	int timeoutCnt = 0;
	unsigned char ch;

	while (1) {
		if ((bytesDropped & dropMask) == dropMask) {
			TRACE("Input dropped %ld\r\n", bytesDropped);
			if (dropMask < 0x1000) {
				dropMask <<= 1; dropMask |= 1;
			}
		}
		if ((bytesDropped - shoutBack) > 10240) {
			if (m_contentStarted) {
				SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
				shoutBack = bytesDropped;
			}
		}

		while (GetInputSize() <= 3) {
			len = ReadLink(timeout, 3);
			if (len > 0) break;
			timeoutCnt++;
			if ((timeoutCnt & 1) == 0) {
				if (m_contentStarted) {
					SendBPacket(PACKET_TYPE_ZRPOS, m_filePosition);
				}
			}
			if (timeoutCnt >= 10) {
				return 0;
			}
		}

		ch = m_inputBuffer[m_inputHead];

		if (ch == CHAR_CAN) {
			if (++m_cancelRequest >= 5) {
				m_endOfSession = SESSION_ABORT;
				return 0;
			}
			m_inputHead = NextIndex(m_inputHead);
			bytesDropped++;
			continue;
		}
		
		m_cancelRequest = 0;
		if ( ch != CHAR_PAD) {
			m_inputHead = NextIndex(m_inputHead);
			bytesDropped++;
			continue;
		}
		next = NextIndex(m_inputHead);
		if (m_inputBuffer[next] != CHAR_CAN) {
			m_inputHead = NextIndex(m_inputHead);
			bytesDropped += 2;
			continue;
		}
		next = NextIndex(next);
		switch (m_inputBuffer[next]) {
		case 'A': // Binary packet CRC16 
			m_inputHead = NextIndex(next);
			TRACE("Found BIN16\r\n");
			return PACKET_BIN16;
		case 'B': // hexadecimal CRC16
			m_inputHead = NextIndex(next);
			TRACE("Found Hex16\r\n");
			return PACKET_HEX16;
		case 'C': // Binary packet CRC32
			m_inputHead = NextIndex(next);
			TRACE("Found Bin32\r\n");
			return PACKET_BIN32;
		default:
			m_inputHead = NextIndex(m_inputHead);
			bytesDropped += 3;
			break;
		}

	}
}

int CZModemRecv::SendBPacket(unsigned char type, unsigned long parameter)
{
	char buff[32];
	CCRC16 crc;
	unsigned short checksum;
	char *ptr;

	strcpy(buff, "**\x18" "B");

	crc.UpdateCrc(type);
	crc.UpdateCrc((unsigned char)(parameter & 0xff));
	crc.UpdateCrc((unsigned char)((parameter >> 8) & 0xff));
	crc.UpdateCrc((unsigned char)((parameter >> 16) & 0xff));
	crc.UpdateCrc((unsigned char)((parameter >> 24) & 0xff));
	checksum = crc.GetCrc16();

	ptr = buff + 4;
	PrintHex(type, ptr); ptr += 2;
	PrintHex((unsigned char)(parameter & 0xff), ptr); ptr += 2;
	PrintHex((unsigned char)((parameter >> 8) & 0xff), ptr); ptr += 2;
	PrintHex((unsigned char)((parameter >> 16) & 0xff), ptr); ptr += 2;
	PrintHex((unsigned char)((parameter >> 24) & 0xff), ptr); ptr += 2;

	PrintHex((checksum >> 8) & 0xff, ptr); ptr += 2;
	PrintHex((checksum & 0xff), ptr); ptr += 2;
	strcpy(ptr, "\x0d\x0a\x11"); 

	WriteLink(buff, strlen(buff));

	return 1;
}

int CZModemRecv::SendReceiveCapability(void)
{
	return SendBPacket(1, 0x03000000 + 0x7000 /* 0x27000000 + 0x100*/);
}

void CZModemRecv::PrintHex(unsigned char ch, char *pBuff)
{
	static const char *pList = "0123456789abcdef";

	*pBuff++ = pList[ch >> 4];
	*pBuff = pList[ch & 0xf];
}


int CZModemRecv::WriteLink(char *pBuff, int size)
{   
	int result;
	result = m_pPort->Send(pBuff, size, 0);
	m_pPort->flush();
	return result;
}

int CZModemRecv::ReadLink(long timeout, int max_len /* = INPUT_BUFFER_SIZE */)
{
	int next = NextIndex(m_inputTail);
	int len;
	int block;

#if 0
	if (next == m_inputHead) {
		// it's full
		return 0;
	}
#endif

	if (m_inputHead == m_inputTail) {
		m_inputHead = m_inputTail = 0;
		block = INPUT_BUFFER_SIZE - 6;
		if (block > max_len) {
			block = max_len;
		}
		len = m_pPort->Receive((char *)m_inputBuffer, block, timeout);
		if (len > 0) {
			m_inputTail += len;
			assert(m_inputTail < INPUT_BUFFER_SIZE);
			DisplayByteStream((char *)m_inputBuffer, len);
			return len;
		}
		return 0;
	}

	if (m_inputHead > m_inputTail) {
		block = m_inputHead - m_inputTail - 7;
		if (block <= 0) return 0;
		if (block > max_len) {
			block = max_len;
		}
		len = m_pPort->Receive((char *)m_inputBuffer + m_inputTail, block, timeout);
		if (len > 0) {
			DisplayByteStream((char *)m_inputBuffer + m_inputTail, len);
			m_inputTail += len;
			assert(m_inputTail < m_inputHead);
			return len;
		}
		return 0;
	}

	if (m_inputHead < m_inputTail) {
		int total = 0;
		if (m_inputTail <= (INPUT_BUFFER_SIZE - 1)) {
			block = INPUT_BUFFER_SIZE - m_inputTail;
			if (m_inputHead <= 10) {
				block -= 3;
			}
			if (block > max_len) {
				block = max_len;
			}
			if (block <= 0) return 0;
			len = m_pPort->Receive((char *)m_inputBuffer + m_inputTail, block, timeout);
			if (len <= 0) return 0;
			DisplayByteStream((char *)m_inputBuffer + m_inputTail, len);
			next = m_inputTail + len;		
			if (next <= INPUT_BUFFER_SIZE - 1) {
				m_inputTail = next;
				return len;
			} else {
				m_inputTail = 0;
			}
			max_len -= len;
			total += len;
			if (max_len <= 0) {
				return total;
			}
		}
		if ((m_inputHead > 10) && (m_inputTail == 0)) {
			block = m_inputHead - 7;
			if (block > max_len) {
				block = max_len;
			}
			len = m_pPort->Receive((char *)m_inputBuffer, block, timeout);
			if (len <= 0) return total;
			DisplayByteStream((char *)m_inputBuffer, len);
			m_inputTail = len;
			assert(m_inputTail < m_inputHead);
			return total + len;
		}
		return total;
	}

	return 0;
}

void CZModemRecv::DisplayByteStream(char *pBuff, int len)
{
#if 0
	char buff[10];
	int lineCut = 0;
	for (int i = 0; i < len; i++) {
		PrintHex((unsigned char)(*pBuff++), buff);
		buff[2] = 0x20; buff[3] = 0;
		TRACE(buff);
		lineCut++;

		if (lineCut >= 20) {
			TRACE("\r\n");
			lineCut = 0;
		} else if (lineCut == 10) {
			TRACE("- ");
		}
	}
	TRACE("\r\n");
#endif
}
