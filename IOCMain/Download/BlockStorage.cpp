// BlockStorage.cpp: implementation of the CBlockStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: BlockStorage.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: BlockStorage.cpp 1.1 2007/12/06 11:41:19PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#include "BlockStorage.h"
#include "comm.h"
#include "stdio.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPVOperation *CBlockStorage::m_pPV = NULL;

CBlockStorage::CBlockStorage(const char *pName, int32 maxSize)
: CStorage(pName)
{
	m_userId = 0;
	m_maxSize = maxSize;
	m_buffer = (uint8 *)new uint16[(maxSize + 1) / 2];
	m_currentSize = 0;
	m_offset = 0;
	if (m_pPV == NULL) {
		m_pPV = new CPVOperation();
	}

#if 0 // debug
	{
		int32 i;
		m_currentSize = maxSize;
		for (i = 0; i < m_currentSize; i++) {
			m_buffer[i] = (uint8)i;
		}
	}
#endif
}

CBlockStorage::~CBlockStorage()
{

}

void CBlockStorage::Rewind()
{
	m_offset = 0L;
}

// Return 1 on success
int CBlockStorage::Write(const char *pBuffer, int len)
{
	if (m_offset + len > m_maxSize) return -1;
	memcpy(m_buffer + m_offset, pBuffer, len);
	m_offset += len;
	return 1;
}

// Success 1
int CBlockStorage::OpenStorage(uint32 id)
{   
	m_pPV->P();
	if (m_userId == 0L) {
		m_userId = id;
		m_pPV->V();
		return 1;
	}
	m_pPV->V();
  	return -1;
}

int CBlockStorage::CloseStorage(uint32 id)
{    
	m_pPV->P();	
	if (m_userId == id) {
		m_userId = 0L;
        // m_currentSize = m_offset; // It's bad idea to put it here.
        m_pPV->V();        
        return 1;
	}
	m_pPV->V();        
	return -1;
}

int CBlockStorage::CheckIntegrity(uint32 *pTotal)
{
	if (pTotal) {
		*pTotal = m_currentSize;
	}
	return 1;
}

// Return read length
int CBlockStorage::Read(char *pBuffer, int len)
{
	int32 left;
	left = m_currentSize - m_offset;
	if (left < len) len = left;
	memcpy(pBuffer, m_buffer + m_offset, len);
	m_offset += len;
	return len;
}

void CBlockStorage::PrintContent(Ccomm *pComm, int bASCII /* = 0 */)
{
	int32 index;
	int32 left;
	int len;
	int k;
	char buff[200];
	sprintf(buff, "%s Size=%ld\r\n", m_name, m_currentSize);
	pComm->Send(buff);
	index = 0;
	if (bASCII) {
		// Plain print
		while (index < m_currentSize) {
			left = m_currentSize - index;
			len = (left > 80) ? (80) : (left);
			strncpy(buff, (char *)m_buffer + index, len);
			buff[len] = 0;
			pComm->Send(buff);
			index += len;
		}
		return;
	}

	while (index < m_currentSize) {
		left = m_currentSize - index;
		len = (left > 16L) ? (16) : (left);
		sprintf(buff, "%05lX ", index);
		pComm->Send(buff);
		for (k = 0; k < 16; k++) {
			if (k < len) {
				sprintf(buff, "%02X ", m_buffer[index + k]);
				pComm->Send(buff);
			} else {
				pComm->Send("   ");
			}
			if (k == 7) {
				pComm->Send("- ");
			}
		}
		for (k = 0; k < len; k++) {
			uint8 ch;
			ch = m_buffer[index + k];
			if ((ch > 0x20) && (ch < 0x7f)) {
				buff[0] = ch;
				buff[1] = 0;
				pComm->Send(buff);
			} else {
				pComm->Send(".");
			}
		}
		pComm->Send("\r\n");
		index += len;
		//pComm->flush();
	}
}

int CBlockStorage::Seek(int32 offset)
{
	int32 pos;
	pos = m_offset + offset;
	if ((pos < 0) || (pos > m_currentSize)) {
		return -1;
	}
	m_offset = pos;
	return 1;
}

int CBlockStorage::EndOfCollection()
{
	m_currentSize = m_offset;
	return 1;
}

int CBlockStorage::EndOfDownload()
{
	m_currentSize = m_offset;
	return 1;
}
