// EventStore.cpp: implementation of the CEventStore class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventStore.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventStore.cpp 1.1 2007/12/06 11:41:24PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */
                  
#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "Flash.h"
#include "EventStore.h"
#include "FlashList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventStore::CEventStore(CFlash *pDevice)
{
	int i = 0;
	m_blockList[i++] = new CFlashList(pDevice, 0x9e0000, 0x10000, EVENT_ITEM_SIZE);
	m_blockList[i++] = new CFlashList(pDevice, 0x9f0000, 0x10000, EVENT_ITEM_SIZE);
#if (MAX_EVENT_BLOCK_NUM >= 3)
	m_blockList[i++] = new CFlashList(pDevice, 0x9d0000, 0x10000, EVENT_ITEM_SIZE); 
#endif
	ResetEventStore();
}

CEventStore::~CEventStore()
{

}

// return number of events retrieved.
// 0 is no more and error
// It's up to CEventStore to interpret index.
// 8-bit(Last eight bit of block index) + 24-bit(index of item within block)
// If index == 0xffffffff, index not specified. Starting from top of event stack
int CEventStore::RtrvItems(unsigned long index, unsigned long *pNextIndex, 
	unsigned char *pRec, int max)
{   
	int cnt = 0;
	int ret;
	CFlashList *pList;
	unsigned long blockId;

	if (index == 0x00ffffff) return 0;

	*pNextIndex = 0x00ffffff;
	
	m_pv.P();
	// Find out which block that index is pointing to
	if (index == 0xffffffff) {
		// Start from the latest block
		pList = m_pLatest;
		ret = pList->GetCurrentPoint(&index);
		if (ret != 1) {
			// Should never be here
			ResetEventStore();
			m_pv.V();
			return 0;
		}
		blockId = pList->GetBlockIndex();
		blockId &= 0xff;
	} else {
		// Find the right block to start with
		blockId = index >> 24;
		index &= 0xffffff;
		pList = FindBlock(blockId);
		if (pList == NULL) {
			// Cannot find the block, Incoming index is not right
			m_pv.V();
			return 0;
		}
	}

	// pList pointing to the right block
	while (cnt < max) {
		while (index == 0) {
			// Go to older one
			blockId--;
			pList = FindBlock(blockId);
			if (pList == NULL) {
				// Cannot find it anymore
				m_pv.V();
				return cnt;
			}
			ret = pList->GetCurrentPoint(&index);
			if (ret != 1) {
				// An invalid block. should never happen.
				ResetEventStore();
				m_pv.V();
				return 0;
			}
		}
		index--;
		ret = pList->ReadItem(index, pRec);
		if (ret == 1) {
			// Get it right.
			cnt++;
			pRec += EVENT_ITEM_SIZE;
		}  
	}
	m_pv.V();
	*pNextIndex = index | (blockId << 24);
    return cnt;
}

// Return 1: OK
// -1: Internal error. Don't know latest block. Should never happen
// -2: Internal error. Cannot find the oldest block when current one is full.
int CEventStore::AppendItem(const unsigned char *pItem)
{   
	int ret;
	int result = 1;
	CFlashList *pList;

	if (m_pLatest == NULL) {
		// Should never happen
		ResetEventStore();
		return -1;
	}

	m_pv.P();
	do {
		ret = m_pLatest->AppendItem(pItem);
		if (ret != 1) {
			// Somehow it fails.
			// If it's full, find another block. If not try again, maybe there is some defect on the chip.
			if (m_pLatest->IsFull()) {
				// Find the oldest one, and wipe it.
				pList = FindEldestBlock();
				if (pList) {
					pList->Wipe(m_pLatest->GetBlockIndex()+1);
					m_pLatest = pList;
					// Do appending again.
				} else {
					result = -2;
					ResetEventStore();
					break;
				}
			}
			// Do appending again
		} else {
			// It's done all right
			break;
		}
	} while (1);
	m_pv.V();
    return result;
}

CFlashList * CEventStore::FindLatestBlock(int *pIndex)
{
	unsigned long maxBlockIndex = 0, index;
	int latest = -1;
	for (int i = 0; i < MAX_EVENT_BLOCK_NUM; i++) {
		index = m_blockList[i]->GetBlockIndex();
		if ( index > maxBlockIndex) {
			latest = i;
			maxBlockIndex = index;
		}
	}
	if (pIndex) *pIndex = latest;
	return((latest >= 0) ? (m_blockList[latest]) : (NULL));
}

CFlashList * CEventStore::FindEldestBlock(int *pIndex)
{
	unsigned long blockIndex = 0xffffffff, index;
	int eldest = -1;
	for (int i = 0; i < MAX_EVENT_BLOCK_NUM; i++) {
		index = m_blockList[i]->GetBlockIndex();
		if ((index < blockIndex) && (index != 0)) {
			eldest = i;
			blockIndex = index;
		}
	}
	if (pIndex) *pIndex = eldest;
	return ((eldest >= 0) ? (m_blockList[eldest]) : (NULL));
}

// Find the block that match the index. The MSB(byte) should match the LSB of Block index
// return NULL: If not found
CFlashList * CEventStore::FindBlock(unsigned long index)
{
	int i;
	unsigned long block;

	index &= 0xff;
	for (i = 0; i < MAX_EVENT_BLOCK_NUM; i++) {
		block = m_blockList[i]->GetBlockIndex();
		if ((block & 0xff) == index) {
			return m_blockList[i];
		}
	}
	return NULL;
}

void CEventStore::ResetEventStore()
{
	// Initialize invalid ones if any
	// first find the latest one
	CFlashList *pList;
	CFlashList *pLatest;  
	int i;
	unsigned long blockIndex = 1;
	pLatest = FindLatestBlock();
	if (pLatest) {
		// there is valid one
		blockIndex = pLatest->GetBlockIndex();
	}
	// Go thru all the invalid ones & wipe each one of them
	for (i = 0; i < MAX_EVENT_BLOCK_NUM; i++) {
		pList = m_blockList[i];
		if (!pList->IsValid()) {
			pList->Wipe(++blockIndex);
			pLatest = pList; // obviously, this becomes the latest one.
		}
	}

	// Now we have the latest block
	m_pLatest = pLatest;

}
