// IstateParser.cpp: implementation of the CIstateParser class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateParser.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateParser.cpp 1.3 2009/04/30 11:35:04PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#include "IstateParser.h"
#include "IstateKb.h"
#include "Istate.h"
#include "Storage.h"
#include "IstateProcess.h"
#include "stdio.h"
#include "PTPDnldProcess.h"
//#include "IstateIdIOC.h"

#define TRACE  TRACE_PTP_DNLD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateParser::CIstateParser(CStorage *pStorage, CIstateProcess *pIstateProcess)
{
	m_pStorage = pStorage;
	m_pIstateProcess = pIstateProcess;
	m_pIstate = pIstateProcess->GetIOCIstate();
	m_pIstateKB = m_pIstate->GetKB();
}

CIstateParser::~CIstateParser()
{

}

// return N: N Item written
// return -1: Storage busy
// Print current Istate into storage
int CIstateParser::PrintIstate(int bWithOpen/* = 1 */)
{
	int16 start, end;
	int16 item; int16 itemSize;
	int16 index1, index2, i, j;
	uint16 flag;
	uint8 *ptr8;
	const char *pName;
	char buff[100];
	int ret;
	uint32 value;
	const char *pHeader;
	int indexType;
	int cnt = 0;
	uint16 istate_version;

	if (bWithOpen) {
		ret = m_pStorage->OpenStorage((uint32)this);
		if (ret != 1) return -1;
	}

	m_pIstateKB->GetIstateLocation(NULL, &istate_version);

	m_pStorage->Rewind();

	sprintf(buff, "RTE ISTATE IN ASCII\r\nFORMAT: %d\r\nISTATE: %d\r\n", 
		ISTATE_PARSER_VERSION, istate_version);
	m_pStorage->Write(buff, strlen(buff));
	m_pIstateKB->GetItemRange(&start, &end);
	for (item = start + 1; item < end; item++) {
		// Get index range
		ret = m_pIstateKB->GetItemInfo(item, &flag, &itemSize, &index1, &index2, 
			(void **)(&ptr8), &pName);
		if (ret != 1) {
			TRACE("Cannot find Istate item %d in parser\r\n",
				item);
			continue; // Skip this one
		}
		if (flag & (FLAG_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE | FLAG_VOLATILE_ISTATE | FLAG_IMC_READ_ONLY_ISTATE)) {
			// Skip read only Istate item.
			// Skip Standalone istate
			// 4/14/2004. Skip volatile istate. Don't send it to PPC.
			continue;
		}
		sprintf(buff, "N: %d %s\r\n", item, pName);
		m_pStorage->Write(buff, strlen(buff));
		if ((index1 == 1) && (index2 == 1)) {
			pHeader = "A";
			indexType = 1;
		} else if (index2 == 1) {
			pHeader = "B";
			indexType = 2;
		} else if (index1 == 1) {
			pHeader = "C"; // This is weird. Shouldn't be here
			indexType = 3;
		} else {
			// Two dimensional
			pHeader = "D";
			indexType = 4;
		}
		for (j = 0; j < index2; j++) {
			for (i = 0; i < index1; i++) {
				m_pIstate->ReadOneIstateItem(item , i, j, &value);
				switch (indexType) {
				case 1:
					// Single value
					sprintf(buff, "%s: %ld\r\n", // value 
						pHeader, value);
					break;
				case 2:
					sprintf(buff, "%s: %d %ld\r\n", // index1 value
						pHeader, i, value);
					break;
				case 3:
					sprintf(buff, "%s: %d %ld\r\r\n", // index2 value
						pHeader, j, value);
					break;
				case 4:
					sprintf(buff, "%s: %d %d %ld\r\n", // index1 index2 value
						pHeader, i, j, value);
					break;
				}
				m_pStorage->Write(buff, strlen(buff));
				cnt++;
			}
		}
	}

	m_pStorage->EndOfCollection();

	if (bWithOpen) {
		m_pStorage->CloseStorage((uint32)this);
	}
  	return cnt;
}

// number of Istate item read/understood.
// 0: nothing to read
// -1: fail to open storage
// Read Istate from Storage and use it.
int CIstateParser::ReadIstate()
{
#define BUFF_LEN 300
	int ret;
	char buff[BUFF_LEN + 1];
	int c1 = 0, c2 = 0;
	uint32 total;
	int cnt = 0;
	int16 item = 0;
	int16 index1, index2;
	uint32 value;

	buff[BUFF_LEN] = 0;
	ret = m_pStorage->CheckIntegrity(&total);
	if (total <= 0) {
		return 0;
	}
	ret = m_pStorage->OpenStorage((uint32)this);
	if (ret != 1) {
		// Cannot open storage
		return -1;
	}
	m_pStorage->Rewind();
	
	do {
		int len;
		char *ptr;
		char *pLineEnd;
		len = c2 - c1;
		if (len < 100) {
			// Time to replennish the buff
			if (len > 0) {
				memcpy(buff, buff + c1, len);
				c1 = 0; c2 = len;
			}
			buff[c2] = 0;
			len = BUFF_LEN - c2;
			if (len > total) len = total;
			len = m_pStorage->Read(buff + c2, len);
			if (len > 0) {
				c2 += len;
				buff[c2] = 0;
				total -= len;
			} else {
				total = 0;
			}
			len = c2 - c1;
		}
		// Read one line
		pLineEnd = strchr(buff + c1, '\r');
		if (pLineEnd == NULL) {
			// cannot find end of line.
			break;
		}
		*(pLineEnd++) = 0;
		if (*pLineEnd < ' ') {
			pLineEnd++;
		}
		switch (buff[c1]) {
		case 'A':
			ret = sscanf(buff + c1, "A: %ld", &value);
			if (ret == 1) {
				// Storage Istate Item
				m_pIstateProcess->WriteOneIOCIstateItem(item, 0, 0, value);
				cnt++;
			}
			break;
		case 'B':
			ret = sscanf(buff + c1, "B: %d %ld", &index1, &value);
			if (ret == 2) {
				m_pIstateProcess->WriteOneIOCIstateItem(item, index1, 0, value);
				cnt++;
			}
			break;
		case 'C':
			ret = sscanf(buff + c1, "C: %d %ld", &index2, &value);
			if (ret == 2) {
				m_pIstateProcess->WriteOneIOCIstateItem(item, 0, index2, value);
				cnt++;
			}
			break;
		case 'D':
			ret = sscanf(buff + c1, "D: %d %d %ld", &index1, &index2, &value);
			if (ret == 3) {
				m_pIstateProcess->WriteOneIOCIstateItem(item, index1, index2, value);
				cnt++;
			}
			break;
		case 'N':
			ret = sscanf(buff + c1, "N: %d", &item);
			if (ret != 1) {
				// Not Getting new id
				item = 0;
			}
			break;		
		}
		c1 = pLineEnd - buff; // Go to next line
	} while ((c2 > c1) || (total > 0));

	m_pStorage->CloseStorage((uint32)this);
    return cnt;
}
