// MemHeap.h: interface for the CMemHeap class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemHeap.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemHeap.h 1.1 2007/12/06 11:39:17PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_MEMHEAP_H__577B71BE_D35F_4022_AF48_61E3205BE45B__INCLUDED_)
#define AFX_MEMHEAP_H__577B71BE_D35F_4022_AF48_61E3205BE45B__INCLUDED_

class CMemBlockList;
class CMemBlock;
class Ccomm;

class CMemHeap  
{
public:
	// Allocate from heap number of block with length of size.
	// There should be multiple calls to this function to reserve 
	// space from heap.
	void SetupList(int number, int size);
	// Allocate from CMemHeap a block with size of size.
	// This function can be called from ISR.
	// return NULL if no more space is available.
	CMemBlock * Allocate(int size);
	// Print debug info
	void PrintStatus(Ccomm *pComm);
	// Reclaim all blocks.
	void Reset(void);
	CMemHeap();
	virtual ~CMemHeap();
private:
	enum { MAX_LIST = 4 };
	int m_numberList;
	CMemBlockList *m_pList[MAX_LIST];
};

#endif // !defined(AFX_MEMHEAP_H__577B71BE_D35F_4022_AF48_61E3205BE45B__INCLUDED_)
