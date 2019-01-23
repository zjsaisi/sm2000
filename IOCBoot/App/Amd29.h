/*                   
 * Filename: Amd29.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Amd29.h 1.2 2009/05/05 15:46:51PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */             

#ifndef _Amd29_h
#define _Amd29_h

#include "DataType.h"
#include "flash.h"
#include "timer.h"
#include "PV.h"

class CAmd29lv800bb: public CFlash {
public:
	virtual void Close(void);
	virtual void Open(void);
	CAmd29lv800bb(unsigned long address, CCheapTimer *pTimer, CPVOperation *pPV);
	~CAmd29lv800bb(void);  
	int IsWithinRange(unsigned long address);
	int EraseSector(unsigned long address);
	int WriteWord(unsigned long address, uint16 data);  
	int WriteBytes(unsigned long address, int len, const unsigned char *pData);          
	int CheckDevice(void);
	virtual int ReadWords(unsigned long address, unsigned short *pBuffer, unsigned long length);
private:  
	static const unsigned long m_SectorList[40];                     
	inline uint16 ReadBack(unsigned long offset);
	inline void WriteCmd(unsigned long offset, uint16 data);  
	inline void Unlock(void);
	enum { AMD_ID = 0x1, DEVICE_ID = 0x225b, AMD_PROTECTED = 0x1};
	// Where device is located 	
	unsigned long m_deviceStart;
	// number of logic sectors
	int m_sectors; 
	CCheapTimer *m_pTimer;
	unsigned char m_skipLocking;
	CPVOperation *m_pPV;
};

#endif
