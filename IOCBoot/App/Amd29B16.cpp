// Amd29B16.cpp: implementation of the CAmd29B16 class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: Amd29B16.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: Amd29B16.cpp 1.2 2009/05/05 16:05:20PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */             

#include "Amd29B16.h"
#include "timer.h"
#include "machine.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "cxfuncs.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAmd29B16::CAmd29B16(uint16 deviceId, long *eraseDelay, const unsigned long *sectList, unsigned long start, CCheapTimer *pTimer, CPVOperation *pPV)
{
	m_SectorList = sectList;
	m_deviceId = deviceId;

	m_deviceStart = start & 0xf00000;
	m_sectors = 19;		// replace this in derived class
	m_length = 0x100000; // replace this in derived class
	m_pTimer = pTimer;
	m_skipLocking = 0;
	m_pPV = pPV;  //new CPVOperation();
	m_pEraseDelay = eraseDelay;
}

CAmd29B16::~CAmd29B16()
{

}

// return 1 for success
// -1 address out of range
// -2 internal error
// -3, -4, -5 Erase failure
int CAmd29B16::EraseSector(unsigned long address)
{      
	unsigned long offset;
	unsigned long max_delay;
	long left;       
	uint16 tmp;       
	uint8 old_exr;
	int i; 
	int sector = -1; 
	int ret;
	
	if ((address < m_deviceStart) || (address >= (m_deviceStart + m_length))) { 
		return -1;
	}                    
	offset = address - m_deviceStart;
	for (i = 0; i < m_sectors; i++) {
		if ((offset >= m_SectorList[i * 2]) && (offset < m_SectorList[i*2+1])) {
			sector = i;
			break;
		}
	}  
	if (sector < 0) {
		// Shouldn't happen
		return -2;
	}
	m_pPV->P();
	offset = m_SectorList[sector*2];	
	//old_exr = get_exr();
	//DISABLE_INTR;
	WriteCmd(0, 0xf0f0);
	Unlock();
	WriteCmd(0xaaa, 0x8080);
	WriteCmd(0xaaa, 0xaaaa);
	WriteCmd(0x554, 0x5555);
	WriteCmd(offset, 0x3030);
	
	// Wait for DQ3 to show it started
#if 0	 
	tmp = ReadBack(offset);     
	set_exr(old_exr); // ENABLE_INTR;
	
	if (tmp & 0x8) { 
		m_pPV->V();
		return -5;
	}
	i = 1000;
	do {                       
		if (i--) break;
		tmp = ReadBack(offset);
	} while ((tmp & 0x8) == 0);      
#endif	 
	
	// Wait for erase to be done
	if (m_pTimer) {
		m_pTimer->Start(MAX_ERASE_DELAY); // Changed from 2000 to 30000 on 11/30/2005
	}
	max_delay = 500000;
	while (max_delay--) {
		m_pPV->Pause(1); // in some case, the task could be stuck here forever.
							// To be invetigate later.
		if (m_pTimer) {
			if (m_pTimer->TimeOut()) { 
				m_pPV->V();
				return -4;
			}
		}
		tmp = ReadBack(offset);
		if (tmp != 0xffff) continue;
		tmp = ReadBack(offset);
		if (tmp == 0xffff) { 
			if (m_pTimer) {
				left = m_pTimer->TimeLeft();
				if (m_pEraseDelay) {
					m_pEraseDelay[sector] = MAX_ERASE_DELAY - left;
				}
			}       
			m_pPV->V();
			return 1;       
		}
	} 	
	m_pPV->V();
	return -3;
}
 
// return 1 for success
// -1 Invalid address
// -2 writing failure
int CAmd29B16::WriteWord(unsigned long address, uint16 data)
{   
	unsigned long offset;                        
    uint16 tmp;        
    uint8 old_exr;
    int max_delay;
    
	if ((address < m_deviceStart) || (address >= (m_deviceStart + m_length))) { 
		return -1;
	} 
	if (address & 1) 
		return -1;
	if (!m_skipLocking) m_pPV->P();
	offset = address - m_deviceStart;  
	//old_exr = get_exr();
	//DISABLE_INTR;
	WriteCmd(0, 0xf0f0);
	Unlock();
	WriteCmd(0xaaa, 0xa0a0);
	WriteCmd(offset, data);
	//set_exr(old_exr); //ENABLE_INTR;
	max_delay = 1000;
	while (max_delay--) {
		tmp = ReadBack(offset);
		if (tmp != data) continue;
		tmp = ReadBack(offset);
		if (tmp == data) { 
			WriteCmd(0, 0xf0f0);
			if (!m_skipLocking) m_pPV->V();
			return 1;
		}
	}       
	WriteCmd(0, 0xf0f0);
	if (!m_skipLocking) m_pPV->V();
	return -2;
}            
                   
// Return 1 for success
// -1 for invalid address
// -2 writing failure                              
int CAmd29B16::WriteBytes(unsigned long address, int len, const unsigned char *pData)
{                 
	int ret;
	uint16 wData;
	unsigned long wAddress;

	m_pPV->P();
	m_skipLocking = 1;

	while (len > 0) {        
	 	if ((address & 1) || (len == 1)) {
	 		wAddress = address & 0xfffffe;
	 		wData = *((uint16 *)wAddress);
	 		if (address & 1) {
	 			wData &= 0xff00;
	 			wData |= *pData;
	 		} else {
	 			wData &= 0xff;
	 			wData |= (uint16)(*pData) << 8;
	 		}                                  
	 		ret = WriteWord(wAddress, wData); 
			if (ret != 1) {
				m_skipLocking = 0;
				m_pPV->V();
				return ret;
			}
	 		pData++;
	 		len--;
	 		address++;
	 	} else {      
	 		wData = ((uint16)(*pData) << 8) | pData[1];
	 		ret = WriteWord(address, wData);
			if (ret != 1) {
				m_skipLocking = 0;
				m_pPV->V();
				return ret;
			}
	 		pData += 2;
	 		len -= 2;
	 		address += 2;
	 	}
	};
	m_skipLocking = 0;
	m_pPV->V();
	return 1;
}	
	
// Return 1 for success, 
// -1 for wrong manufacure id
// -2 for wrong device part number
// -3 for sector protected
int CAmd29B16::CheckDevice(void)
{              
	uint16 tmp; 
	uint8 old_exr;
	int i;
	unsigned long offset;
	
	// Read manufacture id.
	m_pPV->P();
	old_exr = get_exr();
	set_exr(7); //DISABLE_INTR;  
	WriteCmd(0, 0xf0f0);
	Unlock();
	WriteCmd(0xaaa, 0x9090);
	tmp = ReadBack(0);
	if ((tmp & 0xff) != AMD_ID) {
		set_exr(old_exr); // ENABLE_INTR;
		m_pPV->V();
		return -1;
	}             
	tmp = ReadBack(2);
	if (tmp != m_deviceId) {
		set_exr(old_exr); // ENABLE_INTR;
		m_pPV->V();
		return -2;
	}                    
	for (i = 0; i < m_sectors; i++) {
		offset = m_SectorList[i*2];	
		tmp = ReadBack(offset + 4);
		if ((tmp & 0xff) == AMD_PROTECTED) {
		    set_exr(old_exr); //ENABLE_INTR;
			m_pPV->V();
			return -3;                
		}
	}
	
	WriteCmd(0, 0xf0f0);
	set_exr(old_exr); // ENABLE_INTR;
	m_pPV->V();
	return 1;
}                   

// Read 1 for success
int CAmd29B16::ReadWords(unsigned long address, unsigned short *pBuffer, unsigned long length)
{
	unsigned short *ptr;
	ptr = (unsigned short *)address;
	m_pPV->P();
	while (length) {
		*(pBuffer++) = *(ptr++);
		length -= 2;
	}
	m_pPV->V();
	return 1;
}

void CAmd29B16::Unlock(void)
{ 
	WriteCmd(0xaaa, 0xaaaa);
	WriteCmd(0x554, 0x5555);
}

void CAmd29B16::WriteCmd(unsigned long offset, uint16 data)
{   
	volatile uint16 *ptr;
	ptr = (uint16 *)(offset + m_deviceStart);
	*ptr = data;
}
           
uint16 CAmd29B16::ReadBack(unsigned long offset)
{      
	volatile uint16 *ptr;
	ptr = (uint16 *)(offset + m_deviceStart);
	return(*ptr);
}

// 1 for within flash range
// -1 out of range
int CAmd29B16::IsWithinRange(unsigned long address)
{
	if ((address < m_deviceStart) || (address >= (m_deviceStart + m_length))) { 
		return -1;
	} 
	return 1;
}

void CAmd29B16::Open()
{
	m_pPV->P();
}

void CAmd29B16::Close()
{
	m_pPV->V();
}

long CAmd29B16::GetEraseDelay(int sect)
{
	if ((sect < 0) || (sect >= m_sectors)) return 0L;
	if (m_pEraseDelay) {
		return m_pEraseDelay[sect];
	}
	return 0L;
}
