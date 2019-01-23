// ZModemRecv.h: interface for the CZModemRecv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZMODEMRECV_H__522DCD29_0B2E_419D_B70A_267048ACC887__INCLUDED_)
#define AFX_ZMODEMRECV_H__522DCD29_0B2E_419D_B70A_267048ACC887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataType.h"

class Ccomm;
class CStorage;

class CZModemRecv  
{
private:
	enum { MAX_TOTAL_ERROR = 100, MAX_CURRENT_ERROR = 10 };
	int m_currentErrorCnt;
	int m_totalErrorCnt;
private:
	typedef enum { STATE_INIT = 1, STATE_DONE } Z_STATE;
	Z_STATE m_state;
	
private:
	Ccomm *m_pPort;
	CStorage *m_pStorage;
public:
	int Start(Ccomm *pPort, CStorage *pStorage);
public:
	CZModemRecv();
	virtual ~CZModemRecv();

private:
	enum { PACKET_BIN16 = 1, PACKET_HEX16, PACKET_BIN32 };
	int m_currentRecvPacketType;
	enum { PACKET_TYPE_ZRQINIT = 0, 
		PACKET_TYPE_ZRINIT = 1,
		PACKET_TYPE_ZSINIT = 2,
		PACKET_TYPE_ZACK = 3,
		PACKET_TYPE_ZFILE = 4,
		PACKET_TYPE_ZSKIP = 5,
		PACKET_TYPE_ZNAK = 6,
		PACKET_TYPE_ZABORT = 7,
		PACKET_TYPE_ZFIN = 8,
		PACKET_TYPE_ZRPOS = 9,
		PACKET_TYPE_ZDATA = 10,
		PACKET_TYPE_ZEOF = 11,
		PACKET_TYPE_ZFERR = 12,
		PACKET_TYPE_ZCRC = 13, 
		PACKET_TYPE_ZCHALLENGE = 14, 
		PACKET_TYPE_ZCOMPL = 15,
		PACKET_TYPE_ZCAN = 16,
		PACKET_TYPE_ZFREECNT = 17, 
		PACKET_TYPE_ZCOMMAND = 18
	};

	int WaitPacket(long timeout);
private:
	void PrintHex(unsigned char ch, char *pBuff);
	int SendReceiveCapability(void);

private:
	void DisplayByteStream(char *pBuff, int len);
	enum { INPUT_BUFFER_SIZE = 5120 };
	unsigned char m_inputBuffer[INPUT_BUFFER_SIZE];
	int volatile m_inputHead;
	int volatile m_inputTail;
	inline int NextIndex(int cur) {
		register int next_index;
		next_index = cur + 1;
		if (next_index >= INPUT_BUFFER_SIZE) {
			next_index = 0;
		}
		return next_index;
	}
	inline int GetInputSize(void) {
		if (m_inputHead <= m_inputTail) return m_inputTail - m_inputHead;
		return INPUT_BUFFER_SIZE - m_inputHead + m_inputTail;
	}
	inline int ConvertHex(char *pBuff) {
		unsigned char ch;
		if ((pBuff[0] >= '0') && (pBuff[0] <= '9')) {
			ch = pBuff[0] - '0';
		} else if ((pBuff[0] >= 'a') && (pBuff[0] <= 'f')) {
			ch = pBuff[0] - 'a' + 10;
		} else {
			return -1;
		}
		ch <<= 4; pBuff++;
		if ((pBuff[0] >= '0') && (pBuff[0] <= '9')) {
			ch += pBuff[0] - '0';
		} else if ((pBuff[0] >= 'a') && (pBuff[0] <= 'f')) {
			ch += pBuff[0] - 'a' + 10;
		} else {
			return -2;
		}
		return ch;
	}
	int ReadLink(long timeout, int max_len = INPUT_BUFFER_SIZE);
	int WriteLink(char *pBuff, int size);

private:
	void Dummy(void);
	int DecodeHex16Packet(long timeout);
	int DecodeBin16Packet(long timeout);
	int DecodeBin32Packet(long timeout);
	int DecodeBinaryPacket(long timeout, int crcbits);
	int ProcessPacket(unsigned char type, unsigned long parameter, long timeout);
private:
	int32 m_filePosition;
	int m_contentStarted;
	enum {SESSION_START = 0, SESSION_EOF,
		SESSION_COMPLETE, SESSION_ABORT};
	int m_endOfSession;
	int m_cancelRequest;
	int ReceiveData(CStorage *pStore, long timeout, int *pLength);
	int SendBPacket(unsigned char type, unsigned long parameter);
	void ClearInputBuffer(void);
};

#endif // !defined(AFX_ZMODEMRECV_H__522DCD29_0B2E_419D_B70A_267048ACC887__INCLUDED_)
