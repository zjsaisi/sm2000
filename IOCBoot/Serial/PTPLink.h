// PTPLink.h: interface for the CPTPLink class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPLink.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: PTPLink.h 1.1 2007/12/06 11:39:17PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_PTPLINK_H__D9A4BA8D_7B37_47D7_9824_C92D85F998BB__INCLUDED_)
#define AFX_PTPLINK_H__D9A4BA8D_7B37_47D7_9824_C92D85F998BB__INCLUDED_

#include "CodeOpt.h"
#include "ConstDef.h"
#include "DataType.h"
#include "PV.h"

class CPTPIsr;
class Ccomm;
class CMemHeap;
class CMemBlock;
class CBaseProcess;

#ifndef DEBUG_PTP
#define DEBUG_PTP 1
#endif

typedef struct {
	uint16 len;
	uint8 ctrl2;
	uint8 buffer[2];
} PTPSendBuffer;

typedef struct {
	uint8 client_id;
	CBaseProcess *pClient;
} ClientInfo;

class CPTPLink  
{
public: // Interface to outside
	// Client process calls PostFrame to send out a frame to 
	// other process in another card.
	// Return int: 1 OK. The data pointed by ptr will be stored
	//		in buffer and send out when possible.
	// -1 link busy
	// -2 Data too long
	// -3 link is not connected
	// -4 no more buffer
	// -5 no more buffer pointer
	// Input parameter: ptr points to the buffer to be sent.
	//		len: length of the buffer in byte
	//		ctrl2: ctrl2 byte in link layer indicating where
	//			 the frame to send.
	int PostFrame(const char *ptr, uint16 len, uint8 ctrl2);
	// For debug purpose. Print out internal data on serial
	// port.
	void PrintStatus(Ccomm *pComm);
	// Reset the link to DISC state. The link will start to
	// reconnect to its peer.
	void Restart(void);
	// Direct the link where to post the data when received.
	// client_id is the ctrl2 byte in link.
	// pClient is the client process. When a frame matching
	// received, pClient->Notify(this, memBlock) will be called.
	void SetupClient(uint8 client_id, CBaseProcess *pClient);
public: // For init
	// pName is the name of the link.
	CPTPLink(const char *pName);
	// Setup lower module that support this class.
	void SetDriver(CPTPIsr *pDriver);
public: // Within module
	void SetEvent(int evt);
	void EventTimeOut(void);
	void EventDispatch(uint16 events);
	void SetTaskSlot(uint8 slot);
	void ReportError(int tail, int len);
	void ReportFrame(int tail, int len);
	virtual ~CPTPLink();
	enum { DISC_STATE = 1, CONN_STATE = 2 };
	enum { DATA_INDICATION = 1, DATA_ERROR_INDICATION = 2, CMD_INDICATION = 4,
	DATA_REQUEST = 8, RESET_INDICATION = 16};
	enum { CMD_ACK = 0x20, CMD_NAK, CMD_RESET_LINK, CMD_PING };
private:
	CBaseProcess * FindClient(uint8 client_id);
	void DataRequest(void);
	void SendData(const char *ptr, int len, uint8 ctrl2);
	void DebugOutput(const char *str, int len = -1, int no_header = 0);
	void SendCmd(uint8 cmd, uint8 ss, uint8 as);
	void EnterConn(void);
	void EnterDisc(int bToReset = 1);
	uint8 NextSequence(uint8 cur);
	void IndicateCmd(void);
	void IndicateData(void);
	void IndicateDataError(void);
	enum { NUM_DATA_BLOCK = 31, NUM_CMD_BLOCK = 30, NUM_SEND_BLOCK = 29,
		NUM_CLIENT = 5 };
	CPTPIsr *m_pDriver;

	// Receiving buffers
	volatile int m_dataHead;
	volatile int m_dataTail;
	//volatile int m_dataCnt; // To efficiently use data buffer
							// ISR increase it and Task will deduct it. 
							// Has to be careful when access it from Task.
	//char m_dataHeap[NUM_DATA_BLOCK][FRAME_LENGTH + 5];
	//int m_dataLength[NUM_DATA_BLOCK];
	CMemHeap *m_pRecvMemHeap;
	CMemBlock *m_dataList[NUM_DATA_BLOCK];

	volatile int m_cmdHead;
	volatile int m_cmdTail;
	//int m_cmdCnt; // For command, we don't have to be efficient.
	char m_cmdHeap[NUM_CMD_BLOCK][4];
	

	// Where to send the indicator
	uint8 m_taskSlot;

	// Link layer info
	uint8 m_SSR;
	uint8 m_ASR;
	int8 m_state;
	uint32 m_tick;

	// Sending buffer
	//PTPSendBuffer *m_pSendBuffer;
	//char m_sendBuffer[FRAME_LENGTH + 6];
	//uint16 m_len;
	//uint8 m_ctrl2;
	volatile int8 m_pending;
	//CSema m_sendBufferSema;
	CPVOperation m_sendPV;
	CMemHeap *m_pSendMemHeap;
	CMemBlock *m_sendList[NUM_SEND_BLOCK];
	volatile int m_sendHead;
	volatile int m_sendTail;
	uint32 m_resendTimer;

	// Link name
	char m_name[9];

	// Disconnect event
	int m_discEvent;
	int m_discEventAsserted;

	// Client setup
	int m_clientCnt;
	ClientInfo m_clientList[NUM_CLIENT];
};

#endif // !defined(AFX_PTPLINK_H__D9A4BA8D_7B37_47D7_9824_C92D85F998BB__INCLUDED_)
