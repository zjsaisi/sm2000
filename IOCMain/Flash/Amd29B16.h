// Amd29B16.h: interface for the CAmd29B16 class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Amd29B16.h 1.2 2009/05/05 14:25:30PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */             
/* Special note for flash: zmiao 10/17/2006.
 * We found we are short of Am29DL161DB. So we need to support M29W160EB 
 * and S29AL016D(bottom boot). Sector list are different. We don't want to make 
 * a new RC. It's OK for now. Later a new class is necessary to support new chip.
 */
#if !defined(AFX_AMD29B16_H__5DED5232_1AC1_45EB_AC49_1CA7674890DD__INCLUDED_)
#define AFX_AMD29B16_H__5DED5232_1AC1_45EB_AC49_1CA7674890DD__INCLUDED_

#include "DataType.h"
#include "CodeOpt.h"
#include "flash.h"
#include "timer.h"
#include "PV.h"

class CAmd29B16 : public CFlash  
{
public:
	long GetEraseDelay(int sect);
	CAmd29B16(uint16 deviceId, long *eraseDelay, const unsigned long *sectList, unsigned long start, CCheapTimer *pTimer, CPVOperation *pPV);
	virtual ~CAmd29B16();
	virtual void Close(void);
	virtual void Open(void);
	int IsWithinRange(unsigned long address);
	int EraseSector(unsigned long address);
	int WriteWord(unsigned long address, uint16 data);  
	int WriteBytes(unsigned long address, int len, const unsigned char *pData);          
	int CheckDevice(void);
	virtual int ReadWords(unsigned long address, unsigned short *pBuffer, unsigned long length);
protected:
	uint32 m_length;
	// number of logic sectors
	int m_sectors; 
private:
	enum {MAX_ERASE_DELAY = 30000 };
	const unsigned long *m_SectorList;
	uint16 m_deviceId;
	
	inline uint16 ReadBack(unsigned long offset);
	inline void WriteCmd(unsigned long offset, uint16 data);  
	inline void Unlock(void);
	enum { AMD_ID = 0x1, /*DEVICE_ID = 0x225b,*/ AMD_PROTECTED = 0x1};
	// Where device is located 	
	unsigned long m_deviceStart;
	CCheapTimer *m_pTimer;
	unsigned char m_skipLocking;
	CPVOperation *m_pPV;
	long *m_pEraseDelay;

};

#endif // !defined(AFX_AMD29B16_H__5DED5232_1AC1_45EB_AC49_1CA7674890DD__INCLUDED_)
