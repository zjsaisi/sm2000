// IstateBlockTrans.cpp: implementation of the CIstateBlockTrans class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateBlockTrans.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateBlockTrans.cpp 1.2 2009/04/30 17:59:59PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "IstateBlockTrans.h"
#include "string.h"
#include "PTPLink.h"
#include "stdio.h"
#include "AllProcess.h"
#include "Istate.h"
#include "IstateKb.h"
#include "IstateProcess.h" // For debug output
#include "comm.h"
#include "machine.h"

#define TRACE TRACE_ISTATE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateBlockTrans::CIstateBlockTrans(CIstate *pIstate, CPTPLink *pLink, uint8 peer)
{
	m_nTransferCnt = 0L;
	m_peer = peer;
	m_pLink = pLink;
	m_pIstate = pIstate;
	m_pKB = m_pIstate->GetKB(); //pKB;
	m_pIstateLocation = m_pKB->GetIstateLocation(&m_istateSize, &m_istateVersion);
	m_pIstateBuffer = (uint8 *)( new uint16[(m_istateSize + 1) / 2] );
	
	m_currentOffset = 0;
	m_sendTimeout = 0;
	m_recvOffset = 0;
	m_prevRecvSize = 0;
	m_state = CIstate::STATE_IDLE_ISTATE;
	m_nIstateRecvCnt = 0;
}

CIstateBlockTrans::~CIstateBlockTrans()
{

}

void CIstateBlockTrans::StartBlockTransfer()
{
	m_currentOffset = 0;
	SendOneBlock();
	m_sendTimeout = 0;
	m_state = CIstate::STATE_SENDING_ISTATE;
}

// 1 for OK
// 2 for done
int CIstateBlockTrans::SendOneBlock()
{
	SIOCIstateTransBlock block;
	int16 left;
	int16 size;
	uint8 tmp_exr;

	left = m_istateSize - m_currentOffset;
	if (left <= 0) return 2;

	block.cmd = CMD_BLOCK_SEND;
	block.version = m_istateVersion;
	block.offset = m_currentOffset;
	block.size = size = (left >= BLOCK_SIZE) ? (BLOCK_SIZE) : (left); 

	m_pIstate->OpenIstate();
	tmp_exr = get_exr();
	set_exr(7);
	memcpy(block.content, 
		m_pIstateLocation + m_currentOffset,
		size);
	set_exr(tmp_exr);
	m_pIstate->CloseIstate();

	m_pLink->PostFrame((const char *)&block, sizeof(SIOCIstateTransBlock), m_peer);	
	return 1;

}

// 1: for valid
// 0x101: for last one
// -1: invalid
// -2: Different version
// -3: unexpected offset.
int CIstateBlockTrans::ReceiveOneBlock(const void *pMsg, int len)
{
//#if DEBUG_ISTATE
//	char debugBuff[100];
//#endif

	const SIOCIstateTransBlock *pBlock;
	int16 offset;
	int16 size;
	int16 end;
	int ret = 1;
	uint8 tmp_exr;

	pBlock = (SIOCIstateTransBlock *)pMsg;
	if (len != sizeof(SIOCIstateTransBlock)) {
		// Size different, should never happen
		return -1;
	}
	if (pBlock->version != m_istateVersion) {
		return -2;
	}
	offset = pBlock->offset;
	if (offset == 0) {
		m_recvOffset = 0;
	}
	if (offset != m_recvOffset) {
		if (offset == m_recvOffset + m_prevRecvSize) {
			m_recvOffset = offset;
		} else {
			return -3;
		}
	}

	size = pBlock->size;
	end = offset + size;

	if (end > m_istateSize) {
		// Pass the max size of istate block, should never happen
		return -1;
	}

	// Store to intermediate buffer first
	memcpy(m_pIstateBuffer + offset, pBlock->content, size);

	if (end == m_istateSize) {		
		// Last block received
		m_nIstateRecvCnt++;

		ret = 0x101;
		// Copy the buffer to Istate
		m_pIstate->OpenIstate();
		tmp_exr = get_exr();
		set_exr(7);
		memcpy((uint8 *)m_pIstateLocation, // Force to write to Istate
			m_pIstateBuffer, m_istateSize);
		set_exr(tmp_exr);
		// Istate modified  
		m_pIstate->SetModified();
		m_pIstate->CloseIstate();	
	}

	// Give out ACK
	SIOCIstateTransBlockAck ack;
	ack.cmd = CMD_BLOCK_ACK;
	ack.offset = offset;
	ack.version = m_istateVersion;
	ack.size = m_prevRecvSize = size;
	m_pLink->PostFrame((const char *)&ack, sizeof(ack), m_peer);
	//m_pIstateRAM->SendIstateMsg(&ack, sizeof(ack));
	TRACE("%s recv Block: Offset=%d Len=%d\r\n", 
			m_pIstate->GetName(), offset, size);
	if (ret == 0x101)	{
		TRACE("%s Block recv done\r\n", m_pIstate->GetName());
	}
	return ret;
}

// 1: Valid
// 2: Last one
// -1: Invalid
// -2: Different verions.
int CIstateBlockTrans::ReceiveBlockAck(const void *pMsg, int len)
{
	const SIOCIstateTransBlockAck *pAck;
	pAck = (SIOCIstateTransBlockAck *)pMsg;

	if (pAck->version != m_istateVersion) return -2;
	if (pAck->offset != m_currentOffset) {
		// Error condition
		return -1;
	}
	m_currentOffset += pAck->size;
	if (m_currentOffset >= m_istateSize) {
		// Last block OK
		m_state = CIstate::STATE_IDLE_ISTATE;
		m_pIstate->ReportIdle();
		TRACE("%s Block trans done\r\n", m_pIstate->GetName());
		m_nTransferCnt++;
		return 2;
	}
	// Continue to the next block
	SendOneBlock();
	m_sendTimeout = 0;
	return 1;
}

void CIstateBlockTrans::TickPerSecond()
{
	// It's idle, then don't do anything
	if (m_state == CIstate::STATE_IDLE_ISTATE) 
		return;

	if ((m_sendTimeout++) % 4 >= 3) {
		// Resend it
		SendOneBlock();
	} else if (m_sendTimeout > 8) {
		// Cannot be done. Drop it
		m_state = CIstate::STATE_IDLE_ISTATE;
		m_pIstate->ReportIdle();
	}
}

// return 1 for success
// 2 for last one.
int CIstateBlockTrans::DispatchMessage(const uint8 *ptr, uint16 len)
{
	uint16 *pCmd;
	pCmd = (uint16 *)ptr;
	switch (*pCmd) {
	case CMD_BLOCK_SEND:
		return ReceiveOneBlock(ptr, len);
	case CMD_BLOCK_ACK:
		return ReceiveBlockAck(ptr, len);
	default:
		return -1;
	}
}

// 1: Idle
// 0: Busy
int CIstateBlockTrans::IsAvailable()
{
	return((m_state == CIstate::STATE_IDLE_ISTATE) ? (1) : (0));
}

int32 CIstateBlockTrans::GetTransferCnt() const
{
	return m_nTransferCnt;
}

void CIstateBlockTrans::PrintIstateBlockTrans(Ccomm *pComm)
{
	char buff[400];

	sprintf(buff, "Block Istate Size: %d  Send Cnt: %ld RECV Cnt: %ld\r\n"
		,m_istateSize, GetTransferCnt(), GetReceiveCnt() );
	pComm->Send(buff);
}

void CIstateBlockTrans::LinkDown()
{
	m_nIstateRecvCnt = 0L;
	m_nTransferCnt = 0L;
}

int32 CIstateBlockTrans::GetReceiveCnt() const 
{
	return m_nIstateRecvCnt;
}
