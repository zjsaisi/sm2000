// IOCIstateStorage.cpp: implementation of the CIOCIstateStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: IOCIstateStorage.cpp 1.2 2008/01/29 11:29:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */

#include "IOCIstateStorage.h"
#include "IstateParser.h"
#include "IstateProcess.h"
#include "stdio.h"
 
#define TRACE TRACE_ISTATE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCIstateStorage::CIOCIstateStorage(const char *pName, int32 maxSize) :
CBlockStorage(pName, maxSize)
{
	m_pParser = NULL;
}

CIOCIstateStorage::~CIOCIstateStorage()
{

}

void CIOCIstateStorage::SetParser(CIstateParser *pParser)
{
	m_pParser = pParser;
}

#if 0
int CIOCIstateStorage::Read(char *pBuffer, int len)
{
	int ret;
	char buff[100];
	if (m_offset == 0L) {
		// First time reading. Get it from parser first.
		if (m_pParser) {
			// Print live Istate into storage
			ret = m_pParser->PrintIstate(0); // Assume it already opened.
			Rewind();
			sprintf(buff, "RTE Istate converted = %d\r\n", ret);
			TRACE(buff);
		} else {
			return 0;
		}
	}
	return CBlockStorage::Read(pBuffer, len);
}
#endif

// Return 1 for success
int CIOCIstateStorage::StartOfCollection()
{   
	int ret;
	//char buff[100];
	if (m_pParser) {
		// Print live Istate into storage
		ret = m_pParser->PrintIstate(0); // Assume it already opened.
		Rewind();
		TRACE("RTE Istate converted = %d\r\n", ret);
	} else {
		return -1;
	}
	return 1;
}

// After PPC send RTE Istate to RTE, it should be take effect
int CIOCIstateStorage::EndOfDownload()
{
	int ret;
	// char buff[80];


	TRACE("End of RTE Istate download. OFFSET=%ld\r\n",
		m_offset);

	// Set size first.
	ret = CBlockStorage::EndOfDownload();


	TRACE("Recv %ld bytes from PPC\r\n", m_currentSize);


	if (m_pParser) {
		ret = m_pParser->ReadIstate();

		TRACE("[%d] Istate Item converted\r\n", ret);

	}
	return ret;

}
