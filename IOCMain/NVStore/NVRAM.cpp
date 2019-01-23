// NVRAM.cpp: implementation of the CNVRAM class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: NVRAM.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: NVRAM.cpp 1.2 2009/05/01 09:33:26PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "NVRAM.h"
#include "crc16.h"
#include "string.h"
#include "PV.h"
#include "sci.h"
#include "stdio.h"
#include "rtc.h"
#include "AllProcess.h"
#include "NVRAM.h"

#define COPY_MAGIC32 0x5652414d // VRAM
#define COPY_MAGIC16 0x434E // CN
#define BLOCK_MAGIC16 0x424B // BK

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
uint32 const CNVRAM::MAGIC_FACTORY_SETTING = 0x464c5354; // FCST 
uint32 const CNVRAM::MAGIC_ISTATE = 0x49535454; // ISTT
//uint32 const CNVRAM::MAGIC_TIME_STAMP = 0x54535450; // TSTP
uint32 const CNVRAM::MAGIC_BESTIME_CAL = 0x4254494D; //BTIM
uint32 const CNVRAM::BLOCK_ID_ISTATE			= 0x1000000;
uint32 const CNVRAM::BLOCK_ID_FACTORY_SETTING	= 0x2000000;
//uint32 const CNVRAM::BLOCK_ID_TIME_STAMP	= 0x3000000;
uint32 const CNVRAM::BLOCK_ID_BESTIME_CAL	= 0x4000000;

CNVRAM::CNVRAM(CRTC *pRTC, CFlash *pFlash, uint32 copy1, uint32 copy2)
{
	m_debugBuff[0] = 0;
	m_curSequence = 0L;

	m_pFlash = pFlash;
	m_copyIndex = 0;
	m_copyAddress[0] = copy1;
	m_copyAddress[1] = copy2;
	m_blockCnt = 0;
	m_writeTimer = 0L;
	memset(m_blockList, 0, sizeof(m_blockList));
	m_pPV = new CPVOperation();
	m_bNew = 0;
	m_pRTC = pRTC;
	m_lastTimeStamp = 0L;
	m_writeCnt = 0;
}

CNVRAM::~CNVRAM()
{

}

// 1: OK
// -1: cannot find the block
// -2: No valid block read from NVRAM
int CNVRAM::StoreBlock(uint32 blockId, int16 blockSize, const uint8 *ptr)
{
	m_pPV->P();
	for (int i = 0; i < m_blockCnt; i++) {
		if ((blockId == m_blockList[i].header.block_id) && 
			(blockSize == m_blockList[i].header.block_size) ) {
			memcpy(m_blockList[i].pBuffer, ptr, blockSize);			
			m_blockList[i].header.flag |= FLAG_VALID;
			m_bNew = 1;
			m_pPV->V();
			return 1;
		}
	}
	m_pPV->V();
 	return -1;
}

// 1: OK
// -1: cannot find the block
// -2: No valid block read from NVRAM
int CNVRAM::RtrvBlock(uint32 blockId, int16 blockSize, uint8 *ptr)
{
	for (int i = 0; i < m_blockCnt; i++) {
		if ((blockId == m_blockList[i].header.block_id) && 
			(blockSize == m_blockList[i].header.block_size) ) {
			if (m_blockList[i].header.flag & FLAG_VALID) {
				// Buffer is valid
				memcpy(ptr, m_blockList[i].pBuffer, blockSize);
				return 1;
			} else {
				// Buffer is invalid
				return -2;
			}
		}
	}
 	return -1;
}

void CNVRAM::NVRAMTickPerSecond()
{
	if (m_writeTimer++ >= 59) {
		m_writeTimer = 0L;
		SaveNVRAMCopy();			
	} else if (!m_bNew) {
		// nothing new, clear timer
		m_writeTimer = 0L;
	}
}

// 1: for OK
// -1: exceed maximum
// This function should be called right after new
int CNVRAM::SetupBlock(uint32 blockId, uint16 blockSize, uint32 magic)
{   
	uint16 size;
	if (m_blockCnt >= NUM_BLOCK) return -1;
	m_blockList[m_blockCnt].header.block_id = blockId;
	m_blockList[m_blockCnt].header.block_size = blockSize;
	m_blockList[m_blockCnt].header.flag = 0;
	m_blockList[m_blockCnt].header.magic = magic;
	size = (blockSize + 1 + BLOCK_TAIL_SIZE) >> 1;
	m_blockList[m_blockCnt].pBuffer = new uint16[size];
	memset(m_blockList[m_blockCnt].pBuffer, 0, size << 1);
	m_blockList[m_blockCnt].headerOffset = 0L;
	m_blockCnt++;
	return 1;
}

// This should be called right after SetupBlock
void CNVRAM::InitSetup()
{
	int ret;
	m_copyIndex = FindLatestCopy(&m_curSequence);
	m_pFlash->Open();
	for (int i = 0; i < m_blockCnt; i++) {
		ret = ReadBlock(m_blockList[i].header.block_id,
			m_blockList[i].header.block_size,
			m_blockList[i].header.magic,
			m_blockList[i].pBuffer);
		if (ret == 1) {
			m_blockList[i].header.flag = FLAG_VALID;
		} else {
			m_blockList[i].header.flag = 0;
		}
	}
	m_pFlash->Close();
}

int CNVRAM::FindLatestCopy(uint32 *pSequence)
{
	CopyHeader *pHeader;
	int ret = 0;
	uint32 sequence = 0;
	uint32 s;

	m_pFlash->Open();
	for (int i = 0; i < NUM_COPY; i++) {
		pHeader = (CopyHeader *)(m_copyAddress[i]);
		s = ReadSequence(pHeader);
		if (s >= sequence) {
			ret = i;
			sequence = s;
		}
	}
	m_pFlash->Close();
	*pSequence = sequence;
  	return ret;
}

// Return 0 for invalid header
uint32 CNVRAM::ReadSequence(CopyHeader *ptr)
{
	int i;
	CCRC16 crc16;
	uint8 *bPtr;

	if (ptr->magic != COPY_MAGIC32) return 0L;
	if (ptr->copy_magic != COPY_MAGIC16) return 0L;
	bPtr = (uint8 *)ptr;
	for (i = 0; i < sizeof(CopyHeader); i++) {
		crc16.UpdateCrc(*bPtr++);
	}
	if (crc16.GetCrc16() != 0) return 0L;
	return (ptr->sequence);
}

// Return 1 if valid block found
// -1: Invalid copy
// -2: not found
int CNVRAM::ReadBlock(uint32 id, uint16 size, uint32 magic, uint16 *buffer)
{
	CCRC16 crc16;
	int ret = 0;
	CopyHeader *pHeader = (CopyHeader *)(m_copyAddress[m_copyIndex]);
	BlockHeader *pBlock;
	BlockHeader *pNextBlock;

	uint32 s = ReadSequence(pHeader);
	if (s == 0L) return -1;
	pBlock = (BlockHeader *)(pHeader + 1);

	do {
		pNextBlock = NextBlockHeader(pBlock);
		if (pNextBlock == NULL) {
			// cannot find it.
			return -2;
		}
		if ((pBlock->block_id == id) && (pBlock->block_size == size) &&
			 (pBlock->magic == magic) && (pBlock->flag & FLAG_VALID)
			) {
			// Found the right one.
			uint16 *wPtr = (uint16 *)(pBlock + 1);
			uint16 *dest = buffer;
			uint16 w;
			crc16.Reset();
			for (int i = 0; i < (((size + 1) >> 1) + (BLOCK_TAIL_SIZE >> 1)); i++) {
				w = *wPtr++;
				*dest++ = w;
				crc16.UpdateCrc(w >> 8);
				crc16.UpdateCrc(w & 0xff);
			}
			if (crc16.GetCrc16() == 0) return 1;
			// CRC check error
			// continue to find
		}
		pBlock = pNextBlock;
	} while (1);
	
	return 0;
}

// Return the next block if available, otherwise return NULL
BlockHeader * CNVRAM::NextBlockHeader(BlockHeader *pBlockHeader)
{
	CCRC16 crc16;
	uint8 *ptr;
	int i;
	uint16 size;

	ptr = (uint8 *)pBlockHeader;
	for (i = 0; i < sizeof(BlockHeader); i++) {
		crc16.UpdateCrc(*ptr++);
	}
	if (crc16.GetCrc16() != 0) return NULL;
	if (pBlockHeader->block_magic != BLOCK_MAGIC16) return NULL;

	ptr = (uint8 *)pBlockHeader;
	size = (pBlockHeader->block_size + 1) & 0xfffe;
	ptr += sizeof(BlockHeader) + size + BLOCK_TAIL_SIZE;
	ptr = (uint8 *)((uint32)(ptr + 15) & 0xfffffff0);
	return ((BlockHeader *)ptr);
}

// Return 0: No need to write to flash
// 1: written
int CNVRAM::SaveNVRAMCopy(void)
{
	int nextCopy = (m_copyIndex + 1) % NUM_COPY;
	CopyHeader *pCopyHeader = (CopyHeader *)(m_copyAddress[nextCopy]);
	CopyHeader copyHeader;
	BlockHeader *pBlockHeader = (BlockHeader *)(pCopyHeader + 1); 
	uint32 adr;
	int size;
	uint16 *buffer;
	uint16 w;

	// Lock the the buffer so they won't be changed by other processes
	m_pPV->P();

	if (!m_bNew) {
		m_pPV->V();
		return 0;
	}

	// Erase the block first
	m_pFlash->EraseSector((uint32)pCopyHeader);

	m_bNew = 0;

	// Write Blocks
	for (int i = 0; i < m_blockCnt; i++) {
		// If content not valid. don't write
		if (!(m_blockList[i].header.flag & FLAG_VALID)) continue;
		m_blockList[i].header.block_magic = BLOCK_MAGIC16;
		m_blockList[i].header.crc16 = CalCRC(&(m_blockList[i].header), sizeof(BlockHeader) - 2);
		// Write the content of the block
		adr = (uint32)(pBlockHeader + 1);
		size = (m_blockList[i].header.block_size + 1) & 0xfffe;
		buffer = m_blockList[i].pBuffer;
		m_pFlash->WriteBytes(adr, size, (uint8 *)buffer);
		w = CalCRC(buffer, size);
		m_pFlash->WriteWord(adr + size, w);
		// Write block header
		adr = (uint32)pBlockHeader;
		m_pFlash->WriteBytes(adr, sizeof(BlockHeader), (uint8 *)(&(m_blockList[i].header)));
		m_pFlash->Open();
		pBlockHeader = NextBlockHeader(pBlockHeader);
		m_pFlash->Close();
	}

	// CRC of the copy header
	memset(&copyHeader, 0, sizeof(CopyHeader));
	copyHeader.sequence = ++m_curSequence;
	copyHeader.magic = COPY_MAGIC32;
	copyHeader.copy_magic = COPY_MAGIC16;
	if (m_pRTC) {
		copyHeader.time_stamp = m_lastTimeStamp = m_pRTC->GetGPSSeconds();
	}
	copyHeader.crc16 = CalCRC(&copyHeader, sizeof(CopyHeader) - 2);
	m_pFlash->WriteBytes((uint32)pCopyHeader, sizeof(CopyHeader), (uint8 *)(&copyHeader));

	// Write copy header
	m_copyIndex = nextCopy;
	m_writeCnt++;
	m_pPV->V();
	return 1;
}

uint16 CNVRAM::CalCRC(void *ptr, int len)
{
	CCRC16 crc16;
	uint8 *bPtr = (uint8 *)ptr;
	for (int i = 0; i < len; i++) {
		crc16.UpdateCrc(*bPtr++);
	}
	return (crc16.GetCrc16());
}


uint32 CNVRAM::GetLastUpdateTimeStamp()
{
	uint32 time;
	m_pPV->P();
	time = m_lastTimeStamp;
	m_pPV->V();
	return time;
}

// Return how many writing to FLASH since power up.
uint32 CNVRAM::GetWriteCnt()
{
	return m_writeCnt;
}

// Print NVRAM status
void CNVRAM::PrintStatus(Ccomm *pComm)
{
	char buff[200];
	CRTC rtc;
	uint32 time;
	int year, month, day, hour, minute, second;
	uint32 cnt;

	time = GetLastUpdateTimeStamp();
	cnt = GetWriteCnt();
	
	if (time != 0) {
		rtc.SetGPSSeconds(time);
		rtc.GetDateTime(&year, &month, &day, &hour, &minute, &second);
		sprintf(buff, "NVRAM updating[PowerOn Cnt:%ld - Life Cnt:%ld]\r\nLast NVRAM written @ %04d-%02d-%02d %02d-%02d-%02d\r\n",
			cnt, m_curSequence,
			year, month, day, hour, minute, second);
		pComm->Send(buff);
	} else {
		sprintf(buff,"NVRAM updating[PowerOn Cnt:%ld - Life Cnt:%ld]\r\nNo NVRAM updating so far\r\n", 
			cnt, m_curSequence); 
		pComm->Send(buff);
	}
	return;

}

void CNVRAM::Init()
{
	sprintf(m_debugBuff, "Block cnt = %d Latest=%d @ %lx Copy Sequence =%ld\r\n",
		m_blockCnt, m_copyIndex, m_copyAddress[m_copyIndex],
		m_curSequence);
	CAllProcess::MaskedPrint(0xffffffff, m_debugBuff);
}
