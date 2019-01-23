// MemBlockList.cpp: implementation of the CMemBlockList class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemBlockList.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemBlockList.cpp 1.1 2007/12/06 11:41:40PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "MemBlockList.h"
#include "MemBlock.h"
#include "string.h"
#include "comm.h"
#include "stdio.h"
#include "machine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemBlockList::CMemBlockList(int number, int size)
{
	m_pList = new CMemBlock *[number + 1];
	m_pOrgList = new CMemBlock *[number + 1];
	m_max = number + 1;
	for (int i = 0; i < number; i++) {
		m_pOrgList[i] = m_pList[i] = new CMemBlock(this, size);
	}
	m_pOrgList[number] = NULL;
	m_pList[number] = NULL;

	m_head = 0;
	m_tail = number;
	m_size = size;
	m_total = number;
	m_used = 0;
}

void CMemBlockList::Reset()
{
	for (int i = 0; i < m_max - 1; i++) {
		m_pList[i] = m_pOrgList[i];
		m_pList[i]->SetBusy(0);
	}
	m_head = 0;
	m_tail = m_max - 1;
	// Not ncessary though
	m_pList[m_max -1] = NULL;
	m_used = 0;
}

CMemBlockList::~CMemBlockList()
{

}

CMemBlock * CMemBlockList::GetBlock(int size, int min_left /*5/24/2004: Minimum blocks left*/)
{
	CMemBlock *ptr;
	
	if (m_size < size) return NULL;
	
	if (min_left) {
		int left;
		left = (m_tail >= m_head) ? (m_tail - m_head) : (m_tail + m_max - m_head);
		if (left <= min_left) {
			return NULL; // too few blocks left. I won't give you any more.
		}
	}

	if (m_head != m_tail) {
		ptr = m_pList[m_head++];
		m_head %= m_max;
		m_used++;
		return ptr;
	} else {
		// No more block available
		return NULL;
	}
}

void CMemBlockList::Release(CMemBlock *ptr)
{
	uint8 tmp_exr;
	// Prevent multi process release data at the same time.
	tmp_exr = get_exr();
	set_exr(7);
	m_pList[m_tail++] = ptr;
	m_tail %= m_max;
	m_used--;
	set_exr(tmp_exr);
}


void CMemBlockList::PrintStatus(Ccomm *pComm)
{
	char buff[100];
	int avail;
	int next, tail;
	int cnt = 0;
	CMemBlock *pBlock;

	if (m_head <= m_tail) {
		avail = m_tail - m_head;
	} else {
		avail = m_max + m_tail - m_head;
	}
	sprintf(buff, "Blk(%04d)X%02d Avail=%02d\t:",
		m_size, m_max - 1, avail);
	pComm->Send(buff);
	next = m_head;
	tail = m_tail;
	while (next != tail) {

		if (cnt >= 10) {
			cnt = 0;
			pComm->Send("\r\n\t");
		}

		pBlock = m_pList[next];
		if (pBlock == NULL) {
			sprintf(buff, " !NULL@%d(%d--%d)",
				next, m_head, m_tail);
			pComm->Send(buff);
			break;
		} else {
			sprintf(buff, "%s%03d", (pBlock->m_bBusy) ? (" !") : (" "),
				pBlock->m_id);
			pComm->Send(buff);
		}
		cnt++;
		next++;
		next %= m_max;
	}
	pComm->Send(" \r\n");
	sprintf(buff, "Used block=%d, Avail=%d\r\n",
		m_used, m_total - m_used);
	pComm->Send(buff);
}

void CMemBlockList::GetUsage(int *pUsed, int *pAvail)
{
	if (pUsed) {
		*pUsed = m_used;
	}
	if (pAvail) {
		*pAvail = m_total - m_used;
	}
}

int CMemBlockList::GetUsagePerPort(int port) // for sending buffer only
{
	register CMemBlock *pBlock;
	int cnt = 0;
	int i;
	for (i = 0; i < m_max - 1; i++) {
		pBlock = m_pOrgList[i];
		if (!pBlock->m_bBusy) continue;
		if (pBlock->m_buffer[2] == port) { // PTPSendBuffer ctrl2 is the 3rd byte for sending buffer
			cnt++;
		}
	}
	return cnt;

}
