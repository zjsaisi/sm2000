// FlashList.h: interface for the CFlashList class.
//
//////////////////////////////////////////////////////////////////////
/*
/*
 * Filename: FlashList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * $Header: FlashList.h 1.1 2007/12/06 11:41:24PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_FLASHLIST_H__0F03A85A_FD9B_40B7_B529_F61EBC9DA4B9__INCLUDED_)
#define AFX_FLASHLIST_H__0F03A85A_FD9B_40B7_B529_F61EBC9DA4B9__INCLUDED_

class CFlash;

#include "PV.h"

// Should be 16 bytes
typedef struct {
	unsigned long blockIndex;
	unsigned short version;
	unsigned short	TBD01;
	unsigned short	TBD02;
	unsigned long	magic;
	unsigned short  CheckSum;
} ListHeaderStruct;

class CFlashList  
{
public:
	int GetCurrentPoint(unsigned long *pCurrent);
	int GetHistoryPoint(unsigned long rollBack, unsigned long *pStart);
	unsigned long GetBlockStart(void) { return m_startAddress; }; // Starting address of the block
	unsigned long GetBlockIndex(void) { return m_blockIndex; }; // The index for the block
	int IsValid(void) {return m_bBlockValid; }; // Is block valid
	unsigned long GetItemCounter(void) { return m_recordCounter; }; // Total valid item in this block
	int ReadItem(unsigned long index, unsigned char *pBuff);
	int IsFull(void);
	int AppendItem(const unsigned char *pItem);
	int Wipe(unsigned long blockIndex);
	CFlashList(CFlash *pDevice, unsigned long startAddress, unsigned long blockSize, short itemSize);
	virtual ~CFlashList();
private:
	int CheckRecord(unsigned char *pRecord);
	int CheckStorage(void);
	enum { VERSION = 2 };
	enum { VALID_RECORD = 1, BLANK_RECORD = 2, INVALID_RECORD = -1 };
	// Config data
	CFlash *m_pDevice;
	unsigned long m_startAddress; // starting address of this block
	unsigned long m_blockSize; // the size of this block
	short m_itemSize; // Item size
	// Status data, Updated by CheckStorage()
	int m_bBlockValid; // If this block is valid
	unsigned long m_blockIndex; // BlockIndex. So that we which block is newer
	unsigned char *m_pEndOfRecords; 
	unsigned long m_recordCounter; // How many valid items in this block.
	int m_bBlockFull;
};

#endif // !defined(AFX_FLASHLIST_H__0F03A85A_FD9B_40B7_B529_F61EBC9DA4B9__INCLUDED_)
