// MemHeap.cpp: implementation of the CMemHeap class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemHeap.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemHeap.cpp 1.2 2009/05/01 10:05:11PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "MemHeap.h"
#include "MemBlockList.h"
#include "MemBlock.h"
#include "string.h"
#include "comm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemHeap::CMemHeap()
{
	m_pList[0] = NULL;
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

CMemBlock * CMemHeap::Allocate(int size, int min_left /*= 0 Added on 5/27/2004. minimum block left*/)
{
	CMemBlock *pBlock;

	for (int i = 0; i < m_numberList; i++) {
		pBlock = m_pList[i]->GetBlock(size, min_left);
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

void CMemHeap::GetUsage(int *pUsed, int *pAvail)
{
	int used = 0, avail = 0;
	int u, a;
	for (int i = 0; i < m_numberList; i++) {
		m_pList[i]->GetUsage(&u, &a);
		used += u;
		avail += a;
	}
	if (pUsed) *pUsed = used;
	if (pAvail) *pAvail = avail;
}

int CMemHeap::GetUsagePerPort(int port) // for sending buffer only
{
	int cnt = 0;
	int i;

	for (i = 0; i < m_numberList; i++) {
		cnt += m_pList[i]->GetUsagePerPort(port);
	}

	return cnt;
}
