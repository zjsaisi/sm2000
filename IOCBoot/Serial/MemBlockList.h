// MemBlockList.h: interface for the CMemBlockList class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemBlockList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemBlockList.h 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_MEMBLOCKLIST_H__876350C2_1268_4B84_B4C1_CC6CCAEFCC86__INCLUDED_)
#define AFX_MEMBLOCKLIST_H__876350C2_1268_4B84_B4C1_CC6CCAEFCC86__INCLUDED_

class CMemBlock;
class Ccomm;

// This is only used by CMemBlock and CMemHeap.
// The purpose is to connect these two classes.
class CMemBlockList  
{
	friend class CMemHeap;
	friend class CMemBlock;
public:
	// Object of this class contains multiple CMemBlock.
	// number: specifies how many CMemBlock will be allocated.
	// size: bytes of buffer in CMemBlock.
	CMemBlockList(int number, int size);
	virtual ~CMemBlockList();
private:
	void PrintStatus(Ccomm *pComm);
	void Release(CMemBlock *ptr);
	void Reset(void);
	CMemBlock * GetBlock(int size);
	int GetSize(void) { return m_size; };
	CMemBlock **m_pList;
	CMemBlock **m_pOrgList;
	int m_max;
	//int m_2max;
	int m_size;
	volatile int m_head;
	volatile int m_tail;
};

#endif // !defined(AFX_MEMBLOCKLIST_H__876350C2_1268_4B84_B4C1_CC6CCAEFCC86__INCLUDED_)
