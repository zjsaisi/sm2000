// MemHeap.cpp: implementation of the CMemHeap class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemHeap.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemHeap.cpp 1.2 2009/05/05 16:16:11PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "MemHeap.h"
#include "MemBlockList.h"
#include "MemBlock.h"
#include <string.h>
#include "comm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemHeap::CMemHeap()
{
	memset(m_pList, 0, sizeof(m_pList));
	m_numberList = 0;
}

CMemHeap::~CMemHeap()
{

}

void CMemHeap::SetupList(int number, int size)
{
	int pos = m_numberList, i;
	CMemBlockList *newPtr;
	
	if (m_numberList >= MAX_LIST) return;

	newPtr = new CMemBlockList(number, size); 
	for (i = 0; i < m_numberList; i++) {
		if (m_pList[i]->GetSize() > size) {
			pos = i;
		}
	}
	for (i = MAX_LIST -1; i > pos; i--) {
		m_pList[i] = m_pList[i-1];
	}
	m_pList[pos] = newPtr;
	m_numberList++;
}

CMemBlock * CMemHeap::Allocate(int size)
{
	CMemBlock *pBlock;

	for (int i = 0; i < m_numberList; i++) {
		pBlock = m_pList[i]->GetBlock(size);
		if (pBlock)  {
			pBlock->SetBusy(1);
			return pBlock;
		}
	}
	// Should never happen
	return NULL;
}

void CMemHeap::Reset()
{
	for (int i = 0; i < m_numberList; i++) {
		m_pList[i]->Reset();
	}
}

void CMemHeap::PrintStatus(Ccomm *pComm)
{
	for (int i = 0; i < m_numberList; i++) {
		m_pList[i]->PrintStatus(pComm);
	}
}
