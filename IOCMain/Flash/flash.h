/*                   
 * Filename: flash.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: flash.h 1.1 2007/12/06 11:41:25PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */                 
#ifndef _FLASH_H_ZHENG_MIAO
#define _FLASH_H_ZHENG_MIAO  

#include "DataType.h"

class CFlash {
public:
	CFlash(void){};
	~CFlash(){};             
	// Ret 1 is the address is within the range of flash chip
	virtual int IsWithinRange(unsigned long address) = 0;           
	// Erase the sector pointed by address
	virtual int EraseSector(unsigned long address) = 0;
	// Write a word to an address
	virtual int WriteWord(unsigned long address, uint16 data) = 0;
	// Write bytes to flash  
	virtual int WriteBytes(unsigned long address, int len, const unsigned char *pData) = 0;
	// Check if the chip matches expectation          
	virtual int CheckDevice(void) = 0;
	// Read words from Flash.
	virtual int ReadWords(unsigned long address, 
		unsigned short *pBuffer, unsigned long length) = 0;
	virtual void Open(void) = 0;
	virtual void Close(void) = 0;
};

#endif
