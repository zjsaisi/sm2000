// IstateItemTrans.cpp: implementation of the CIstateItemTrans class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateItemTrans.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateItemTrans.cpp 1.4 2009/05/01 09:14:03PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.4 $
 */

#include "IstateItemTrans.h"
#include "string.h"
#include "stdio.h"
#include "AllProcess.h"
#include "PTPLink.h"
#include "IstateKb.h"
#include "Istate.h"
#include "IstateProcess.h"
#include "IstateCheckSum.h"
#include "comm.h"
#include "machine.h"
              
#define TRACE TRACE_ISTATE
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateItemTrans::CIstateItemTrans(CIstateCheckSum *pCheckSum, CIstate *pIstate, 
		CPTPLink *pLink, uint8 peer)
{
	m_pRecvIstateAddress = NULL;
	m_pRecvName = NULL;
	m_recvMaxIndex1 = 0;
	m_recvMaxIndex2 = 0;
	m_recvSize = 0;
	m_recvFlag = 0;

	m_sendFlag = 0;
	m_sendMaxIndex1 = 0;
	m_sendMaxIndex2 = 0;
	m_sendIndex1 = 0;
	m_sendIndex2 = 0;
	m_sendSize = 0;
	m_pSendName = NULL;
	m_pSendIstateAddress = NULL;

	m_nIstateRecvCnt = 0L;
	m_pCheckSum = pCheckSum;
	m_nTransferCnt = 0L;
	m_pIstate = pIstate;
	m_pLink = pLink;
	m_peer = peer;
	m_pKB = m_pIstate->GetKB();
	m_pKB->GetItemRange(&m_istateIdStart, &m_istateIdEnd);

	m_sendTickCnt = 0;

	m_sendId = 0;
	m_sendSequence = 0L;

	m_recvId = -1;
	m_recvSequence = 0L;
	m_expectingNext = 0;
	m_pExtraIntf = NULL;

	m_state = CIstate::STATE_IDLE_ISTATE;
}

CIstateItemTrans::~CIstateItemTrans()
{

}

void CIstateItemTrans::StartItemTransfer()
{
	int ret;
	m_sendTickCnt = 0;
	m_sendId = m_istateIdStart + 1;
	m_sendSequence = 0L;
	m_sendIndex1 = 0;
	m_sendIndex2 = 0;
	m_sendFlag = 0;

	// Get the info about first item
	while (1) {
		m_pKB->GetItemInfo(m_sendId, &m_sendFlag, &m_sendSize, 
			&m_sendMaxIndex1, &m_sendMaxIndex2, (void **)(&m_pSendIstateAddress),
			&m_pSendName);
		if (m_sendFlag & (FLAG_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE)) {
			m_sendId++;
			continue;
		}
		break;
	}
	// The first is always valid and can be sent.
	// That means it's not read only, it's not out of istate block.
	ret = SendOneIstateItem();
	if (ret == 1) {
		m_state = CIstate::STATE_SENDING_ISTATE;
	}
}

int CIstateItemTrans::RelayOneItem(int16 item, int16 index1, int16 index2)
{
	int ret;
	m_sendId = item;
	m_sendIndex1 = index1;
	m_sendIndex2 = index2;

	m_sendSequence = 0xffffffff;
	m_sendTickCnt = 0;
	m_sendFlag = 0;

	// Get the info about first item
	m_pKB->GetItemInfo(m_sendId, &m_sendFlag, &m_sendSize, 
		&m_sendMaxIndex1, &m_sendMaxIndex2, (void **)(&m_pSendIstateAddress),
		&m_pSendName);

	ret = SendOneIstateItem();
#if 1 // 1/5/2005: This fight with copy function.
	if (ret == 1) {
		m_state = CIstate::STATE_SENDING_ISTATE;
	}
#endif
    return ret;
}

int CIstateItemTrans::SendOneIstateItem()
{
	SIstateTransItem item;
	int ret;
	uint8 tmp_exr;

	memset(&item, 0, sizeof(item));
	item.cmd = CMD_ITEM_SEND;
	item.version = ITEM_TRANS_VERSION;
	item.sequence = m_sendSequence;
	item.itemId = m_sendId;
	item.itemSize = m_sendSize;
	item.index1 = m_sendIndex1;
	item.index2 = m_sendIndex2;
	// if m_sendSize is 0, it's the last one. No more info contains here. Just to tell 
	// its twin that's the end of item transfer.
	if (m_sendSize != 0) {
		m_pIstate->OpenIstate();
		tmp_exr = get_exr();
		set_exr(7);
		memcpy(item.itemValue, 
			m_pSendIstateAddress + 
			m_sendIndex2 * m_sendMaxIndex1 * m_sendSize + 
			m_sendIndex1 * m_sendSize,
			m_sendSize);
		set_exr(tmp_exr);
		m_pIstate->CloseIstate();
	}
	ret = m_pLink->PostFrame((char *)&item, sizeof(item), m_peer);
	//m_pIstateRAM->SendIstateMsg(&item, sizeof(item));
	return ret;
}

// 1: OK
// 0x101: Last one
// -1: Wrong version
// -2: upexected length
// -3: out of sequence
int CIstateItemTrans::RecvOneItem(uint8 const *pData, uint16 len)
{
	const SIstateTransItem *pItem;
	uint32 sequence;
	uint8 bValueOOR = 1;
	uint8 tmp_exr;
	int accepted = 0;

	pItem = (SIstateTransItem *)pData;
	if (len != sizeof(SIstateTransItem)) return -2;
	if (pItem->version != ITEM_TRANS_VERSION) return -1;

	sequence = pItem->sequence;

	// 1/25/2005: If I am master card, and the other card is not a matching card. I don't want to be messed up with.
	if (m_pCheckSum->IsMaster() == 1) {
		if (m_pCheckSum->IsTwinSameVersion() != 1) {
			// Don't hold the other guy
			SIstateTransItemAck ack;
			ack.cmd = CMD_ITEM_ACK;
			ack.sequence = sequence;
			ack.version = ITEM_TRANS_VERSION;
			m_pLink->PostFrame((char *)&ack, sizeof(ack), m_peer);
			return -3; // Pretend it's out of sequence
		}
	}
	
	if (sequence == 0) {
		m_recvSequence = 0;
		m_expectingNext = 0;
		m_recvId = pItem->itemId;
		RtrvRecvInfo();
	}

	if (sequence == 0xffffffff) {
		m_recvSequence = 0xffffffff;
		m_expectingNext = 0;
		m_recvId = pItem->itemId;
		RtrvRecvInfo();
	}
	
	if (sequence != m_recvSequence) {
		if (sequence == m_recvSequence + m_expectingNext) {
			m_recvSequence = sequence;
		} else {
			// out of sequence
			return -3;
		}
	}

	if (pItem->itemSize == 0) {
		// Last one received without further info
		//m_pIstateRAM->m_bIstateInited = 1;
		m_nIstateRecvCnt++; // complete item transfer counter
		return 0x101;
	}
	if (m_recvId != pItem->itemId) {
		m_recvId = pItem->itemId;
		RtrvRecvInfo();
	}
	
	// Check if the value if OOR
	if (!(m_recvFlag & FLAG_IS_TOGGLE_ISTATE)) {
		// Only check entry istate
		//int32 min, max;
		int ret;
		int32 value = *((int32 *)(pItem->itemValue));
		
		// ret = m_pKB->GetEntryMinMax(m_recvId, &min, &max);
		ret = m_pKB->IsValueOK(m_recvId, pItem->index1, pItem->index2, value);

		if ((ret == 1) /* && (value >= min) && (value <= max) */) { 
			bValueOOR = 0;
		} else {
			TRACE("%s item OOR(%s[%d][%d]): Seq=%ld Value=%ld\r\n",
				m_pIstate->GetName(),
				m_pRecvName, pItem->index2, pItem->index1,
				sequence, value);
		}
	} else {
		// For Toggle Istate, don't check range
		bValueOOR = 0;
	}

	// Store incoming item
	if ((pItem->itemSize == m_recvSize) && (m_recvSize != 0) &&
		(pItem->index1 < m_recvMaxIndex1) && 
		(pItem->index2 < m_recvMaxIndex2) &&
		(m_pRecvIstateAddress != NULL) &&
		(!(m_recvFlag & (FLAG_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE))) &&
		(!bValueOOR)
		) {
		// both side agree on the same size, store the item
		m_pIstate->OpenIstate();
		tmp_exr = get_exr();
		set_exr(7);
		memcpy((char *)(m_pRecvIstateAddress) + 
			pItem->index2 * m_recvMaxIndex1 * m_recvSize +
			pItem->index1 * m_recvSize,
			pItem->itemValue, m_recvSize);
		set_exr(tmp_exr);
		if (!(m_recvFlag & FLAG_VOLATILE_ISTATE)) {
			// For volatile, don't tell it's changed so that the block won't be saved to flash.
			m_pIstate->SetModified();
		}
		//CIstateRAM::s_bIstateModified = 1;
		m_pIstate->CloseIstate();

		if (m_pExtraIntf != NULL) {
			m_pExtraIntf->ReceiveFromTwinIoc(pItem->index1, pItem->index2, *((uint32 *)pItem->itemValue));
		}

		if (sequence == 0xffffffff) {
			// Single setting. Call action now.
			m_pKB->ExecAction(pItem->itemId, pItem->index1, pItem->index2, *((uint32 *)pItem->itemValue));
			if (m_recvFlag & FLAG_AUTO_RESET_ISTATE) {
				m_pKB->ResetIstateValue(pItem->itemId);
			}
		}
		
		accepted = 1;
	}

	if ((int32)sequence >= 0) { 
		TRACE("%s recv item: %04ld id=%d idx1=%d idx2=%d accept=%d\r\n", 
			m_pIstate->GetName(), sequence, m_recvId, pItem->index1, pItem->index2, accepted );
	}

	// Send ACK
	SIstateTransItemAck ack;
	ack.cmd = CMD_ITEM_ACK;
	ack.sequence = sequence;
	ack.version = ITEM_TRANS_VERSION;

	m_pLink->PostFrame((char *)&ack, sizeof(ack), m_peer);
	//m_pIstateRAM->SendIstateMsg(&ack, sizeof(ack));

	m_expectingNext = 1;
	
	return 1;

}

// 1: OK
// 2: Done
// -1: invalid verion
// -2: invalid size
// -3: invalid sequence
int CIstateItemTrans::RecvItemAck(uint8 const *pData, uint16 len)
{
//#if DEBUG_ISTATE
//	char debugBuff[100];
//#endif
	const SIstateTransItemAck *pAck;
	pAck = (SIstateTransItemAck *)pData;
	if (len != sizeof(SIstateTransItemAck)) return -2;
	if (pAck->version != ITEM_TRANS_VERSION) return -1;
	if (pAck->sequence != m_sendSequence) {
#if 0
		if (pAck->sequence == 0xffffffff) {
			// single transaction mingled with copy
			return 2;
		}
#endif
		return -3;
	}

	if (m_sendSequence == 0xffffffff) {
		// Single setting
		m_state = CIstate::STATE_IDLE_ISTATE;
		m_pIstate->ReportIdle();
		TRACE("Single %s Item %d[%d,%d] done\r\n", 
			m_pIstate->GetName(), m_sendId, m_sendIndex2, m_sendIndex1 );
		return 2;
	}

	// Valid ACK, go ahead with next item
	m_sendSequence++;
	m_sendIndex1++;
	if (m_sendIndex1 >= m_sendMaxIndex1) {
		m_sendIndex1 = 0;
		m_sendIndex2++;
	}
	if (m_sendIndex2 >= m_sendMaxIndex2) {
		m_sendIndex2 = 0;
		// Switch next Item
		do {
			m_sendId++;
			if (m_sendId >= m_istateIdEnd) {
				// Last one is done.
				m_sendSize = 0;
				if (m_sendId == m_istateIdEnd) {
					SendOneIstateItem();
					TRACE("Last %s Item sent m_sendId=%d\r\n", m_pIstate->GetName(), m_sendId);
				} else {
					TRACE("Last ack received m_sendId=%d\r\n", m_sendId);
				}
				//m_pIstateRAM->GoIdle();
				m_state = CIstate::STATE_IDLE_ISTATE;
				m_pIstate->ReportIdle();
				m_nTransferCnt++;
				return 2;
			}

			m_pKB->GetItemInfo(m_sendId, &m_sendFlag, &m_sendSize, 
				&m_sendMaxIndex1, &m_sendMaxIndex2, (void **)(&m_pSendIstateAddress),
				&m_pSendName);

			if (m_sendFlag & FLAG_READ_ONLY_ISTATE) {
				// This Istate is read only. Don't send
				continue;
			}

			if (m_sendFlag & FLAG_STANDALONE_ISTATE) {
				// This Istate is out of istate block. Don't send it out
				// to the other.
				continue;
			}
			
			// For FLAG_VOLATILE_ISTATE, it needs to be sent to twin.

			if (m_pSendIstateAddress == NULL) {
				// This Istate is stored nowhere
				// no point to send to the other one
				TRACE("Skip sending %s Item[%d]: %s\r\n", 
					m_pIstate->GetName(), m_sendId, m_pSendName);
				continue;
			} else {
				break;
			}
		} while (1);
	}
	
	SendOneIstateItem();

    return 1;
}

void CIstateItemTrans::TickPerSecond()
{
	if (m_state == CIstate::STATE_IDLE_ISTATE) return;

	if (m_sendTickCnt++ >= 3) {
		// Give up sending
		//m_pIstateRAM->GoIdle();
		m_state = CIstate::STATE_IDLE_ISTATE;
		m_pIstate->ReportIdle();
	}
}

// Retrieve item identified by m_recvId
int CIstateItemTrans::RtrvRecvInfo()
{
	int ret;
	m_recvSize = 0;
	ret = m_pKB->GetItemInfo(m_recvId, &m_recvFlag, &m_recvSize, 
		&m_recvMaxIndex1, &m_recvMaxIndex2, (void **)(&m_pRecvIstateAddress),
		&m_pRecvName, &m_pExtraIntf);
	return ret;
}

int CIstateItemTrans::DispatchMessage(const uint8 *ptr, uint16 len)
{
	const uint16 *pCmd;
	pCmd = (const uint16 *)ptr;
	switch (*pCmd) {
	case CMD_ITEM_SEND:
		return RecvOneItem(ptr, len);
	case CMD_ITEM_ACK:
		return RecvItemAck(ptr, len);
	}
	return -1;
}

int CIstateItemTrans::IsAvailable()
{
	return((m_state == CIstate::STATE_IDLE_ISTATE) ? (1) : (0));
}


int32 CIstateItemTrans::GetTransferCnt() const
{
	return m_nTransferCnt;
}

void CIstateItemTrans::PrintIstateItemTrans(Ccomm *pComm)
{
	char buff[400];

	sprintf(buff, "Item Send Cnt: %ld Recv Cnt: %ld\r\n"
		, GetTransferCnt(), GetReceiveCnt() );
	pComm->Send(buff);
}

void CIstateItemTrans::LinkDown()
{
	m_nIstateRecvCnt = 0L;
	m_nTransferCnt = 0L;
}

int32 CIstateItemTrans::GetReceiveCnt() const
{
	return m_nIstateRecvCnt;
}
