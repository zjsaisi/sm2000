// IstateItemTrans.h: interface for the CIstateItemTrans class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateItemTrans.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateItemTrans.h 1.2 2009/04/30 18:00:00PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ISTATEITEMTRANS_H__398DCE67_CCAF_476E_B087_FAFEAFA1B025__INCLUDED_)
#define AFX_ISTATEITEMTRANS_H__398DCE67_CCAF_476E_B087_FAFEAFA1B025__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "IstateIntf.h"

class CIstate;
class CPTPLink;
class CIstateKb;
class CIstateCheckSum;
class Ccomm;

// This class is to support Item transfer between cards.
#define ITEM_TRANS_VERSION 1
class CIstateItemTrans  
{
private:
	CIstateCheckSum *m_pCheckSum;
	typedef struct {
		uint16 cmd; // CMD_ITEM_SEND from CIstateRAM
		int16 version; // ITEM_TRANS_VERSION
		uint32 sequence;
		int16 itemId;
		int16 itemSize;
		int16 index1;
		int16 index2;
		uint8 itemValue[8]; // Maximum length for an item is 8-byte. It will be
							// enough to hold double, 64-bit data
	} SIstateTransItem;

	typedef struct {
		uint16 cmd; // CMD_ITEM_ACK
		int16 version; // ITEM_TRANS_VERSION
		uint32 sequence;
	} SIstateTransItemAck;

public:
	int32 GetReceiveCnt(void) const;
	void LinkDown(void);
	void PrintIstateItemTrans(Ccomm *pComm);
	int32 GetTransferCnt(void) const;
	int RelayOneItem(int16 item, int16 index1, int16 index2);
	int IsAvailable(void);
	int DispatchMessage(const uint8 *ptr, uint16 len);
	void TickPerSecond(void);
	void StartItemTransfer(void);
	CIstateItemTrans(CIstateCheckSum *pCheckSum, CIstate *pIstate, CPTPLink *pLink, uint8 peer);
	virtual ~CIstateItemTrans();
	enum { CMD_START = 0x20, CMD_ITEM_SEND, CMD_ITEM_ACK,
		CMD_END };
private:
	int RecvItemAck(uint8 const *pData, uint16 len);
	int RecvOneItem(uint8 const *pData, uint16 len);
	int RtrvRecvInfo(void);
	int SendOneIstateItem(void);
	int16 m_state;
	CIstate *m_pIstate;
	CPTPLink *m_pLink;
	uint8 m_peer;
	CIstateKb *m_pKB;
	int16 m_istateIdStart;
	int16 m_istateIdEnd;

	// count for copying to twin
	int32 m_nTransferCnt;

	// Sending side

	int16 m_sendId;
	uint32 m_sendSequence;
	int16 m_sendIndex1;
	int16 m_sendIndex2;

	const char *m_pSendName;
	int16 m_sendSize;
	int16 m_sendMaxIndex1;
	int16 m_sendMaxIndex2;
	uint8 *m_pSendIstateAddress;
	uint16 m_sendFlag;

	// Timeout check
	int16 m_sendTickCnt;

	// Recv side
	int16 m_recvId;
	uint32 m_recvSequence;
	int16 m_expectingNext;

	char const *m_pRecvName;
	int16 m_recvSize;
	int16 m_recvMaxIndex1;
	int16 m_recvMaxIndex2;
	uint8 *m_pRecvIstateAddress;
	uint16 m_recvFlag;
	CIstateIntf *m_pExtraIntf;
	uint32 m_nIstateRecvCnt;
};

#endif // !defined(AFX_ISTATEITEMTRANS_H__398DCE67_CCAF_476E_B087_FAFEAFA1B025__INCLUDED_)
