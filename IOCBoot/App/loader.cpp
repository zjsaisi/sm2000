/*                   
 * Filename: loader.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: loader.cpp 1.2 2008/03/12 11:58:52PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             
                   
#include "loader.h" 
#include "string.h"
#include "asm.h"
#include "machine.h"
#include "flash.h"
#include "WatchDog.h"
              
//CLoader *g_loader = NULL;

CLoader::CLoader(CFlash *pFlash)
{
	m_pFlash = pFlash;
}

CLoader::~CLoader()
{
}

// 1 for success
// -1 for error                
int CLoader::ReadDLDRecord(uint8 *ptr, dld_struct *pRec)
{              
	int i;
	uint8 *pTmp;            
	uint8 chksum = 0;

	m_pFlash->Open();

	if (*ptr != '%') { 
		m_pFlash->Close();
		return -1;
	}
	pTmp = (uint8 *)pRec;
	for (i = 0; i < 6; i++) {
		*pTmp++ = *ptr;
		chksum += *ptr++;
	}                    
	for (i = 0; i < pRec->Data_Size; i++) {
		pRec->Data_Field[i] = *ptr;
		chksum += *ptr++;
	}                    
	chksum -= '%';
	chksum -= pRec->Data_Size;
	chksum -= pRec->Chk_Sum;
	if (pRec->Data_Size == 0) {
		m_pFlash->Close();
		return 1;
	}
	if (chksum != pRec->Chk_Sum) {
		m_pFlash->Close();
		return -1;
	}
	m_pFlash->Close();
	return 1;
}

// 1 for success
// -1, -2 for failure           
int CLoader::CheckDLDBlock(unsigned long address, unsigned long *pTotal /* =NULL */)
{
	uint8 *ptr;
	dld_struct rec;
	int i, ret;               
	uint32 total = 0;
	uint16 wTmp, dataAcc = 0;
	int len;

	ptr = (uint8 *)address;
	do { 
		ret = ReadDLDRecord(ptr, &rec);
		if (ret != 1) return ret;                 
		// Assuming DLD record has even number of bytes
		for (i = 0; i < rec.Data_Size; i++) {
			if ((i & 1) == 0) {
				wTmp = (uint16)(rec.Data_Field[i]) << 8;
				dataAcc += wTmp;
			} else {            
				wTmp = (uint16)(rec.Data_Field[i]);
				dataAcc += wTmp;
			}
		}
		len = 6 + rec.Data_Size;
		total += len;
		ptr += len;
		if (rec.Data_Size == 0) {
			wTmp = ((uint16)(rec.Address[0]) << 8) + (uint16)(rec.Address[1]);
			if (dataAcc != wTmp * 2) {
				return -2;
			}           
			if (pTotal != NULL) {
				*pTotal = total;
			}
			return 1;
		}    
	} while (1);
	if (pTotal != NULL) {
		*pTotal = total;
	}
	return 1;
}  

// 1 for success
// -1 DLD record error
// -2 section record error
// -3, -4 Checksum in section record error                   
int CLoader::Load(unsigned long address, unsigned long *pStartAddress, 
	unsigned long *pNextBlock)
{                      
	dld_struct rec;
	uint8 *ptr;
	int ret, i;
	uint32 lTmp;
	uint16 sectionSum = 0;
	uint8 ch;
	
	do {     
		ptr = (uint8 *)address;
		ret = ReadDLDRecord(ptr, &rec);
		if (ret != 1) return ret;      
		if (rec.Data_Size == 0) {
			// Should never get here
			return -1;
		}
		lTmp = (((uint32) rec.Address[0]) << 16) + (((uint32)rec.Address[1]) << 8)
				+ ((uint32)rec.Address[2]);
		if (lTmp == 0L) {    
			*pStartAddress = (((uint32) rec.Data_Field[0]) << 24) + 
				(((uint32) rec.Data_Field[1]) << 16) + 
				(((uint32) rec.Data_Field[2]) << 8) +
				(((uint32) rec.Data_Field[3]));
			*pNextBlock = address + 6 + rec.Data_Size; 
			for (i = 0; i < 4; i += 2) {
				sectionSum += ((uint16)(rec.Data_Field[i])) << 8;  
				sectionSum += (uint16)(rec.Data_Field[i+1]);
			}
			if (rec.Data_Size != 6) return -2;
			if (rec.Data_Field[4] != (sectionSum >> 8)) 
				return -3;
			if (rec.Data_Field[5] != (sectionSum & 0xff)) 
				return -4;		
			return 1;		
		}
		ptr = (uint8 *)lTmp; 		
		for (i = 0; i < rec.Data_Size; i++) {
			*ptr++ = ch = rec.Data_Field[i];
			if (i & 1) {
				sectionSum += (uint16)ch;
			} else {                     
				sectionSum += (uint16)ch << 8;
			}
		}			
		address += 6 + rec.Data_Size;	
	} while (1);
	return 1;
}

// Never return if all right                  
int CLoader::Start(unsigned long startAddress, int block_number, uint8 bCheckIntegrity)
{                           
	int i, ret;                  
	unsigned long start, filePtr;
	FUNC_PTR func_ptr;
	unsigned long entry, nextBlock;
	unsigned char old;
	
	// Check for 10 blocks
	old = get_exr();
	set_exr(7);
	
	for (i = 0; i < block_number; i++) {
		filePtr = start = startAddress + 0x10000 * i; 
		if (bCheckIntegrity) {
			ret = CheckDLDBlock(start);
		} else {
			ret = 1;
		}
		if (ret != 1) continue;
		// Start to load it
		entry = 0;
		ret = Load(start, &entry, &nextBlock);
		if (ret != 1) continue;
		start = (*(uint32 *)0x200000);
		start &= 0xffffff;
		func_ptr = (entry == 0L) ? ((FUNC_PTR)start) : ((FUNC_PTR)entry);
		SetNextBlock_Step1(nextBlock, filePtr); 
		SetNextBlock_Step2();
		CWatchDog::StartWatchDog();
		(*func_ptr)();
	}            
	set_exr(old);
	return -1;
}
