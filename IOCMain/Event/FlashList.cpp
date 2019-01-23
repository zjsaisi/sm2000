// FlashList.cpp: implementation of the CFlashList class.
//
//////////////////////////////////////////////////////////////////////
/* The class stores event into Flash */
/*
 * Filename: FlashList.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * $Header: FlashList.cpp 1.1 2007/12/06 11:41:24PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#include "string.h"
#include "Flash.h"
#include "FlashList.h"
#include "crc16.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAGIC_NUMBER  0xfb375a4d

// This classe is to support event(general term: item) deposit into flash.
// It can be used to manage similar incremental deposit of data.
// The class is not trying to understand the payload. In any given CFlashList,
// only one size of item is supported. It's decided by contructor.
// Main function is to:
// 1. Append the item at the end.
// 2. Get index of the current position. It's called point here to be different from block index.
//	  Block index is used to see which block has newer data. While item index is about item position 
//	  within a block.

// pDevice is the Flash device driver
// startAddress: start address of the flash list
// blockSize: is the total size of this block.
// itemSize: the size of each payload. 
CFlashList::CFlashList(CFlash *pDevice, unsigned long startAddress, 
					   unsigned long blockSize, short itemSize)
{
	m_pDevice = pDevice;
	m_startAddress = startAddress;
	m_blockSize = (blockSize + 1) & 0xfffffffe;
	m_itemSize = itemSize;
	m_bBlockFull = 0;
	m_blockIndex = 0L;
	// Check the status of the storage
	CheckStorage();
}

CFlashList::~CFlashList()
{

}

// Return 1 for success
int CFlashList::Wipe(unsigned long blockIndex)
{
	ListHeaderStruct header;
	CCRC16 crc16;

	int size;

	size = sizeof(ListHeaderStruct);

	int ret = m_pDevice->EraseSector(m_startAddress);
	if (ret != 1) {
		return ret;
	}
	
	memset(&header, 0, size/*sizeof(ListHeaderStruct)*/);

	header.TBD01 = 0;
	header.TBD02 = 0;
	header.magic = MAGIC_NUMBER;
	header.version = VERSION;
	header.blockIndex = blockIndex;
	// Added field together to get check sum
#if 0
	header.CheckSum = (unsigned short)(header.blockIndex) + 
		(unsigned short)(header.blockIndex >> 16) + header.version;
#else
	// Change to crc
	unsigned char *pCh;
	pCh = (unsigned char *)&header;
	for (int i = 0; i < size - 2; i++) {
		crc16.UpdateCrc(*pCh++);
	}
	header.CheckSum = crc16.GetCrc16();
#endif

	ret = m_pDevice->WriteBytes(m_startAddress, size/*sizeof(ListHeaderStruct)*/, 
		(unsigned char *)(&header));
	if (ret != 1) {
		return (ret - 10);
	}
	m_bBlockFull = 0;
	ret = CheckStorage();
	if (ret != 1) return (ret - 20);
	return 1;	
}

// Return 1 for success
// -1: mismatch magic number
// -2: crc error
// -3: version wrong
int CFlashList::CheckStorage()
{
	int success = 1, i;
	ListHeaderStruct *pHeader;
	unsigned char *pRec;

	pHeader = (ListHeaderStruct *)m_startAddress;

	m_pDevice->Open();

	// If magic is there
	if (pHeader->magic != MAGIC_NUMBER) {
		success = -1;
	}
	if (pHeader->version != VERSION) {
		success = -3;
	}

	// Look at the check sum
#if 0
	if (success == 1) {
		unsigned short *wPtr;
		unsigned short sum = 0;
		wPtr = (unsigned short *)pHeader;
		for (i = 0; i < 5; i++) {
			sum += *(wPtr++);
		}
		if (sum != pHeader->CheckSum) {
			success = -2;
		} else {
			m_blockIndex = pHeader->blockIndex;
		}
	}
#else
	// Use crc16 now
	CCRC16 crc16;
	if (success == 1) {
		unsigned char *pCh;
		pCh = (unsigned char *)pHeader;
		for (i = 0; i < sizeof(ListHeaderStruct); i++) {
			crc16.UpdateCrc(*pCh++);
		}
		if (crc16.GetCrc16() != 0) {
			// crc error
			success = -2;
		} else {
			m_blockIndex = pHeader->blockIndex;
		}
	}
#endif
	// Header is all right, check all the items
	m_recordCounter = 0;
	if (success == 1) {
		pRec = (unsigned char *)(m_startAddress + sizeof(ListHeaderStruct));
		do {
			int ret;
			ret = CheckRecord(pRec);
			if (ret == BLANK_RECORD) {
				m_pEndOfRecords = pRec;
				break;
			}
			if (ret == VALID_RECORD) m_recordCounter++;
			pRec += m_itemSize + 2;
			m_pEndOfRecords = pRec;
			if (IsFull()) break;
		} while (1);
	}

	m_pDevice->Close();
	if (success != 1) {
		m_blockIndex = 0;
		m_bBlockValid = 0;
		m_pEndOfRecords = NULL;
		return success;
	}
	m_bBlockValid = 1;
	return success;
}

int CFlashList::CheckRecord(unsigned char *pRecord)
{
	int i;
	int ret = BLANK_RECORD;
	unsigned char sum = 0;
	unsigned char flag = pRecord[m_itemSize + 1];

	// This usually is the most quick way of finding the bad one.
	if ((flag != 0xff) && (flag != '|')) return INVALID_RECORD;

	for (i = 0; i < m_itemSize; i++) {
		if (*pRecord != 0xff) {
			ret = INVALID_RECORD;
		}
		sum += *(pRecord++);
	}
	if (sum != *pRecord) {
		if ((*pRecord == 0xff) && (pRecord[1] == 0xff)) {
			return ret;
		}
		return INVALID_RECORD;
	}
	pRecord++;
	if (*pRecord == '|') {
		return VALID_RECORD;
	} else if (*pRecord == 0xff) {
		return ret;
	}
	return INVALID_RECORD;
}

// Append a new item at the end.
// return 1 for OK
// <= 0 for failed.
int CFlashList::AppendItem(const unsigned char *pItem)
{
	unsigned char tail[2];
	unsigned char sum = 0;
	unsigned char const  *ptr;
	int ret, i;

	if (!m_bBlockValid) return -1;
	if (m_bBlockFull) return -2;

	// Calculate check sum of incoming data
	tail[1] = '|';
	ptr = pItem;
	for (i = 0; i < m_itemSize; i++) {
		sum += *(ptr++);
	}
	tail[0] = sum;

	// Write the record
	ret  = m_pDevice->WriteBytes((unsigned long)m_pEndOfRecords, m_itemSize, pItem);
	if (ret == 1) {
		ret = m_pDevice->WriteBytes((unsigned long)m_pEndOfRecords + m_itemSize, 2, tail);
	}

	// Push to the next record, and make sure it points to blank one
	m_pDevice->Open();
	do {
		int blank;
		m_pEndOfRecords += (m_itemSize + 2);
		if (IsFull()) break;
		blank = CheckRecord(m_pEndOfRecords);
		if (blank == BLANK_RECORD) break;
		if (blank == VALID_RECORD) {
			// This is a weird case, wipe the record.
			m_pDevice->Close();
			m_pDevice->WriteBytes((unsigned long)(m_pEndOfRecords + m_itemSize), 
					2, (const unsigned char *)"\000\000");
			m_pDevice->Open();
		}
	} while (1);
	m_pDevice->Close();

	if (ret != 1) {
		// Writing failed.		
		return ret;
	}
	m_recordCounter++;
	return 1;
}

// If the block is full
// 1: is full
// 0: not full
int CFlashList::IsFull()
{
	if (m_bBlockFull) return 1;
	if (((unsigned long)m_pEndOfRecords) > m_startAddress + m_blockSize 
		 - m_itemSize * 2) {
		m_bBlockFull = 1;
	}
	return m_bBlockFull;
}


// Return 1 for success
// -1 for invalid record
// -2 index exceed limit.
int CFlashList::ReadItem(unsigned long index, unsigned char *pBuff)
{
	unsigned char *pRec;
    int ret;
    
	pRec = (unsigned char *)
		(m_startAddress + sizeof(ListHeaderStruct) + (m_itemSize + 2) * index);
	if (pRec >= m_pEndOfRecords) {
		// end of block reached
		return -2;
	}
	m_pDevice->Open();
	ret = CheckRecord(pRec);
	if (ret == VALID_RECORD) {
		memcpy(pBuff, pRec, m_itemSize);
	} else {
		m_pDevice->Close();
		return -1;
	}
	m_pDevice->Close();
	return 1;
}

// This carried over from RTHC. we may not need it after. 
// For now I will just leave it in.
// find the start point for the past "rollBack" number of records
// return 1 for success
// return 2 for the maximum reached.
// -1 for failure.
int CFlashList::GetHistoryPoint(unsigned long rollBack, unsigned long *pStart)
{
	unsigned long i;
	unsigned char *ptr = m_pEndOfRecords;
	unsigned long index = 
		((unsigned long)ptr - m_startAddress - sizeof(ListHeaderStruct)) / (m_itemSize + 2);
	if (!m_bBlockValid) return -1;

	m_pDevice->Open();
	for (i = 0; i < rollBack;) {
		// If reach the very beginning
		if ((unsigned long)ptr <= m_startAddress + sizeof(ListHeaderStruct)) {
			*pStart = 0L;
			m_pDevice->Close();
			return 2;
		}
		// Rool back to the previous one
		ptr -= (m_itemSize + 2);
		index--;
		if (CheckRecord(ptr) == VALID_RECORD) {
			i++;
		}
	}
	*pStart = index;
	m_pDevice->Close();
	return 1;
}

// Similar to GetHistoryPoint except it doesn't count backward.
// return 1: for OK
// -1: Block invalid
int CFlashList::GetCurrentPoint(unsigned long *pCurrent)
{
	unsigned char *ptr = m_pEndOfRecords;
	unsigned long index = 
		((unsigned long)ptr - m_startAddress - sizeof(ListHeaderStruct)) / (m_itemSize + 2);
	if (!m_bBlockValid) return -1;
	*pCurrent = index;
	return 1;

}
