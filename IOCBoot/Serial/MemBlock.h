// MemBlock.h: interface for the CMemBlock class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: MemBlock.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MemBlock.h 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_MEMBLOCK_H__39D16017_66F2_494D_A78C_D93B24B09DD4__INCLUDED_)
#define AFX_MEMBLOCK_H__39D16017_66F2_494D_A78C_D93B24B09DD4__INCLUDED_

#include "DataType.h"

class CMemBlockList;

class CMemBlock  
{
	friend class CMemBlockList;
	friend class CMemHeap;
public: // Interface to outside
	// Object of this class contains the payload received.
	// Release() releases the block back to CMemList.
	// When PTP client is done with the data, it should release
	// the memory by calling this function.
	void Release(void);
	// Get the ctrl2 byte for the frame
	uint8 GetCtrl2(void);
	// Get the starting address of payload
	uint8 *GetBuffer(void) { return m_buffer + m_offset; };
	// Get the length of the payload
	uint16 GetCurSize(void) { return m_curSize; };
public: // Interface within PTP module
	// Set the offset to point to payload
	void SetOffset(int offset);
	// Set the length of the payload
	int SetCurSize(uint16 size);
	// pList: points to the CMemBlockList that this object belong to.
	// size: Size of this block.
	CMemBlock(CMemBlockList *pList, int size);
	virtual ~CMemBlock();

	//uint8 GetBusy(void) { return m_bBusy; };
private:
	void SetBusy(uint8 bBusy);
	uint8 *m_buffer;
	uint16 m_curSize;
	uint16 m_maxSize;
	CMemBlockList *m_pParent;
	uint8 m_bBusy;
	uint16 m_id;
	static uint16 m_idCnt;
	int m_offset;
};

#endif // !defined(AFX_MEMBLOCK_H__39D16017_66F2_494D_A78C_D93B24B09DD4__INCLUDED_)
