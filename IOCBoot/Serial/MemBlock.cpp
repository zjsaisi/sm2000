// MemBlock.cpp: implementation of the CMemBlock class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemBlock.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemBlock.cpp 1.2 2009/05/05 16:13:02PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "MemBlock.h"
#include "MemBlockList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
uint16 CMemBlock::m_idCnt = 1;

CMemBlock::CMemBlock(CMemBlockList *pList, int size)
{
	// For Hatachi, 2-byte boudary is enough.
	// For PowerPC, it should be @ 4-byte boundary.
	m_offset = 0;
	size = (size + 3) & 0xfffc;
	m_buffer = (uint8 *) (new uint32[size >> 2]);
	for (int i = 0; i < size; i++) {
		m_buffer[i] = 'A';
	}
	m_curSize = 0;
	m_maxSize = size;
	m_pParent = pList;
	m_bBusy = 0;
	m_id = m_idCnt++;
}

CMemBlock::~CMemBlock()
{

}

void CMemBlock::Release()
{
	if (m_bBusy) {
		m_bBusy = 0;
		m_offset = 0;
		m_pParent->Release(this);
	}
}

// 1: for OK
// -1: Exceed max
int CMemBlock::SetCurSize(uint16 size)
{
	if (size > m_maxSize) return -1;
	m_curSize = size;
	return 1;
}

void CMemBlock::SetOffset(int offset)
{
	m_offset = offset;
}

void CMemBlock::SetBusy(uint8 bBusy) 
{ 
	m_bBusy = bBusy;
	if (!bBusy) {
		m_offset = 0;
	}
}

uint8 CMemBlock::GetCtrl2()
{
	return m_buffer[1];
}
