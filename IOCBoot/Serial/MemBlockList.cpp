// MemBlockList.cpp: implementation of the CMemBlockList class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemBlockList.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemBlockList.cpp 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
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
}

CMemBlockList::~CMemBlockList()
{

}

CMemBlock * CMemBlockList::GetBlock(int size)
{
	CMemBlock *ptr;
	
	if (m_size < size) return NULL;

	if (m_head != m_tail) {
		ptr = m_pList[m_head++];
		m_head %= m_max;
		return ptr;
	} else {
		// No more block available
		return NULL;
	}
}

void CMemBlockList::Release(CMemBlock *ptr)
{
	uint8 tmp;
	// Prevent multi process release data at the same time.
	tmp = get_exr();
	set_exr(7);
	m_pList[m_tail++] = ptr;
	m_tail %= m_max;
	set_exr(tmp);
}


void CMemBlockList::PrintStatus(Ccomm *pComm)
{
	char buff[100];
	int avail;
	int next;
	int cnt = 0;
	CMemBlock *pBlock;

	if (m_head < m_tail) {
		avail = m_tail - m_head;
	} else {
		avail = m_max + m_tail - m_head;
	}
	sprintf(buff, "Blk(%04d)X%02d Avail=%02d\t:",
		m_size, m_max - 1, avail);
	pComm->Send(buff);
	next = m_head;
	while (next != m_tail) {
		pBlock = m_pList[next];
		if (pBlock == NULL) {
			sprintf(buff, " !NULL@%d(%d--%d)",
				next, m_head, m_tail);
			pComm->Send(buff);
			break;
		} else {
			sprintf(buff, "%s%d", (pBlock->m_bBusy) ? (" !") : (" "),
				pBlock->m_id);
			pComm->Send(buff);
		}
		cnt++;
		next++;
		next %= m_max;
	}
	pComm->Send(" \r\n");
}
