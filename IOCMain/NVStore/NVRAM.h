// NVRAM.h: interface for the CNVRAM class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: NVRAM.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: NVRAM.h 1.1 2007/12/06 11:41:35PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_NVRAM_H__F0CD7680_BE2D_4D73_A039_C0F7B6EB5631__INCLUDED_)
#define AFX_NVRAM_H__F0CD7680_BE2D_4D73_A039_C0F7B6EB5631__INCLUDED_

#include "CodeOpt.h"
#include "flash.h"

#ifndef DEBUG_NVRAM
#define DEBUG_NVRAM 1
#endif

typedef struct {
	uint32 block_id;
	uint16 block_size;
	uint16 flag; // Init with 0.

	uint16 block_magic; // Always BLOCK_MAGIC16
	uint32 magic;
	uint16 crc16;
} BlockHeader;

typedef struct {
	uint32 sequence;
	uint32 time_stamp;

	uint16 copy_magic; // Always COPY_MAGIC16
	uint32 magic;
	uint16 crc16;
} CopyHeader;

typedef struct {
	BlockHeader header;
	uint16 *pBuffer;
	uint32 headerOffset; // When the BlockHeader in flash relative to start of the copy.
						// This offset could change from version to version.
} BlockDescription;

class CPVOperation;
class CRTC;
class Ccomm;

class CNVRAM  
{
public:
	void Init(void);
	void PrintStatus(Ccomm *pComm);
	int SaveNVRAMCopy(void);
	uint32 GetWriteCnt(void);
	uint32 GetLastUpdateTimeStamp(void);
	void InitSetup(void);
	int SetupBlock(uint32 blockId, uint16 blockSize, uint32 magic);
	void NVRAMTickPerSecond(void);
	int RtrvBlock(uint32 blockId, int16 blockSize, uint8 *ptr);
	int StoreBlock(uint32 blockId, int16 blockSize, const uint8 *ptr);
	CNVRAM(CRTC *pRTC, CFlash *pFlash, uint32 copy1, uint32 copy2);
	virtual ~CNVRAM();
	static uint32 const BLOCK_ID_ISTATE;
	static uint32 const BLOCK_ID_FACTORY_SETTING;
	//static uint32 const BLOCK_ID_TIME_STAMP;
	static uint32 const BLOCK_ID_BESTIME_CAL;
	static uint32 const MAGIC_ISTATE;
	static uint32 const MAGIC_FACTORY_SETTING;
	//static uint32 const MAGIC_TIME_STAMP;
	static uint32 const MAGIC_BESTIME_CAL;
private:
	uint16 CalCRC(void *ptr, int len);
	BlockHeader * NextBlockHeader(BlockHeader *pBlockHeader);
	int ReadBlock(uint32 id, uint16 size, uint32 magic, uint16 *buffer);
	uint32 ReadSequence(CopyHeader *ptr);
	int FindLatestCopy(uint32 *pSequence);
	enum { BLOCK_TAIL_SIZE = 2 };
	enum { FLAG_VALID = 1 };
	enum { NUM_BLOCK = 3, NUM_COPY = 2 };
	CFlash *m_pFlash;
	
	int m_copyIndex;
	uint32 m_copyAddress[2];
	uint32 m_curSequence;

	// Block list
	BlockDescription m_blockList[NUM_BLOCK];
	int m_blockCnt;
	// Timer to trigger writing
	int32 m_writeTimer;
	CPVOperation *m_pPV;
	int8 volatile m_bNew;
	CRTC *m_pRTC;
	uint32 m_writeCnt;

#if DEBUG_NVRAM
	char m_debugBuff[200];
#endif
	uint32 volatile m_lastTimeStamp;
};

#endif // !defined(AFX_NVRAM_H__F0CD7680_BE2D_4D73_A039_C0F7B6EB5631__INCLUDED_)
