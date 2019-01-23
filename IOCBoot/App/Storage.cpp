// Storage.cpp: implementation of the CCodeStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Storage.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Storage.cpp 1.1 2007/12/06 11:38:52PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#include "DataType.h"
#include "Storage.h"
#include "sci.h"
#include "Flash.h"
#include "string.h"
#include "AllProcess.h"
#include "Loader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStorage::CStorage()
{
}

CStorage::~CStorage()
{
}

CCodeStorage::CCodeStorage(CLoader *pLoader, uint32 startAddress /* = 0x810000 */)
{
	m_pFlash = pLoader->GetFlash();
	m_pLoader = pLoader;
	m_initStartAddress = startAddress;
	m_startAddress = m_initStartAddress;
	m_user = 0L;
}

CCodeStorage::~CCodeStorage()
{

}

// 1 for success
int CCodeStorage::Write(const char *pBuffer, int len)
{
#if 0
	int i;
	const char *ptr = pBuffer;
	for (i = 0; i < len; i++) {
		g_pAll->DebugOutput(ptr, 1);
		ptr++;
	}
#endif

	unsigned long nextAddress;
	int recordLen = len;
	int8 bUsingNewBlock = 0;
	int ret;

	nextAddress = m_startAddress + recordLen - 1;
	if ((m_startAddress & 0xffff) == 0) {
		bUsingNewBlock = 1;
	} else if ((m_startAddress & 0xff0000) != (nextAddress & 0xff0000)) {
		bUsingNewBlock = 1;
	}                                        

	if (bUsingNewBlock) {
		unsigned long begin, end, adr;
		int8 bEraseOnce = 0;
		begin = nextAddress & 0xff0000;
		end = nextAddress | 0xffff;
		adr = begin;

		m_pFlash->Open();
		do { 
			if (*((uint16 *)adr) != 0xffff) {
				if (bEraseOnce) {
					m_pFlash->Close();
					return -4;
				}
				m_pFlash->Close();
				m_pFlash->EraseSector(begin);
				m_pFlash->Open();
				bEraseOnce = 1;
				adr = begin;
				continue;
			}               
			adr += 2;
		} while (adr < end);
		m_pFlash->Close();
	}                
	// Write the record to flash    
	ret = m_pFlash->WriteBytes(m_startAddress, recordLen, (const unsigned char *)pBuffer);
	if (ret != 1) return -2;    
	m_startAddress = nextAddress + 1;    
	//m_dataAcc += wAcc;
	//if (rec.Data_Size == 0) 
	//	return 2;
	return 1;

#if 0
	int i;
	const uint8 *ptr = (uint8 *)pBuffer;
	for (i = 0; i < len;) {
		TRACE("%02X ", *ptr++);
		i++;
		if ((i % 16) == 0) {
			TRACE("\r\n");
		} else if ((i % 8) == 0) {
			TRACE("- ");
		}
	}
#endif
	return 1;
}

#if 0
void CCodeStorage::SetInit(CFlash *pFlash, unsigned long startAddress)
{
	m_pFlash = pFlash;
	m_startAddress = startAddress;
	m_initStartAddress = startAddress;
}
#endif

void CCodeStorage::Rewind()
{
	m_startAddress = m_initStartAddress;
}

// Return 1 for OK
// PTotal can be NULL. 
int CCodeStorage::CheckIntegrity(uint32 *pTotal)
{
	return m_pLoader->CheckDLDBlock(m_initStartAddress, pTotal);
}
    
// return read length
int CCodeStorage::Read(char *pBuffer, int len)
{
	char *pSrc;
	pSrc = (char *)m_startAddress;
	m_pFlash->Open();
	memcpy(pBuffer, pSrc, len);
	m_pFlash->Close();
	m_startAddress += len;

 	return len;
}

// 1 success
// 0 busy
int CCodeStorage::OpenStorage(uint32 id)
{
	m_pFlash->Open();
	if (m_user) {
		m_pFlash->Close();
		return 0;
	}
	m_user = id;
	m_pFlash->Close();
	Rewind();
	return 1;
}

// 1 success
// 0 Cannot close
int CCodeStorage::CloseStorage(uint32 id)
{
	m_pFlash->Open();
	if (m_user == id) {
		m_user = 0L;
		m_pFlash->Close();
		return 1;
	}
	m_pFlash->Close();
	return 0;
}

int CCodeStorage::EndOfStorage(void) 
{
	g_pAll->Reboot();
	// Should never be here
	return 1;
}
