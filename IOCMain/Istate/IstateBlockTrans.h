// IstateBlockTrans.h: interface for the CIstateBlockTrans class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateBlockTrans.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateBlockTrans.h 1.2 2009/04/30 18:00:00PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ISTATEBLOCKTRANS_H__1C031D3F_D7D0_4333_9255_A2E905B64D88__INCLUDED_)
#define AFX_ISTATEBLOCKTRANS_H__1C031D3F_D7D0_4333_9255_A2E905B64D88__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CIstateKb;
class CIstate;
class CPTPLink;
class Ccomm;

class CIstateBlockTrans  
{
private:
	enum {BLOCK_SIZE = 230 };
	typedef struct {
		uint16 cmd;
		uint16 version;
		uint16 offset;
		uint16 size;
		uint8 content[BLOCK_SIZE];
	} SIOCIstateTransBlock;

	typedef struct {
		uint16 cmd;
		uint16 version;
		uint16 offset;
		uint16 size;
	} SIOCIstateTransBlockAck;

public:
	int32 GetReceiveCnt(void) const;
	void LinkDown(void);
	void PrintIstateBlockTrans(Ccomm *pComm);
	int32 GetTransferCnt(void) const;
	int IsAvailable(void);
	int DispatchMessage(const uint8 *ptr, uint16 len);
	void TickPerSecond(void);
	void StartBlockTransfer(void);
	CIstateBlockTrans(CIstate *pIstate, CPTPLink *pLink, uint8 peer);
	virtual ~CIstateBlockTrans();
	enum { CMD_START= 0x10, CMD_BLOCK_SEND, CMD_BLOCK_ACK, CMD_END };
private:
	int SendOneBlock(void);
	int ReceiveBlockAck(const void *pMsg, int len);
	int ReceiveOneBlock(const void *pMsg, int len);

	// Count to copy to twin
	int32 m_nTransferCnt;

	int m_currentOffset;
	int m_sendTimeout;
	int m_recvOffset;
	int m_prevRecvSize;
	int16 m_state;
	uint8 *m_pIstateBuffer;
	uint32 m_nIstateRecvCnt;

	CIstateKb *m_pKB;
	const uint8 *m_pIstateLocation;
	uint16 m_istateSize;
	uint16 m_istateVersion;
	CIstate *m_pIstate;
	
	CPTPLink *m_pLink;
	uint8 m_peer;
};

#endif // !defined(AFX_ISTATEBLOCKTRANS_H__1C031D3F_D7D0_4333_9255_A2E905B64D88__INCLUDED_)
