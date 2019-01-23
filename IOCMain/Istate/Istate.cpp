// Istate.cpp: implementation of the CIstate class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Istate.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Istate.cpp 1.7 2009/04/30 11:33:24PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.7 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "Istate.h"
#include "IstateKb.h"
#include "NVRAM.h"
#include "string.h"
#include "comm.h"
#include "stdio.h"
#include "IstateProcess.h"
#include "errorcode.h"
#include "machine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstate::CIstate(CIstateProcess *pProcess, CIstateKb *pKB, CNVRAM *pNVRAM, char const *pName)
{
	m_pProcess = pProcess;
	strncpy(m_name, pName, sizeof(m_name));
	m_pKB = pKB;
	m_pNVRAM = pNVRAM;
	m_bModified = 0;
	m_modificationCnt = 0L;

	m_pNonVolatileIstateLocation = pKB->GetNonVolatileIstateLocation(&m_nonVolatileIstateSize, &m_istateVersion);
	m_NVRAMBlockId = CNVRAM::BLOCK_ID_ISTATE + m_istateVersion;

}

CIstate::~CIstate()
{

}

// Called when Istate process startup. To read init value from NVRAM.
void CIstate::InitIstate()
{
	int ret;
	// Init from ROM first
	m_pKB->InitAllIstate();
	// Read istate from NVRAM, force it pointer to be non const
	ret = m_pNVRAM->RtrvBlock(m_NVRAMBlockId, m_nonVolatileIstateSize, (uint8 *)m_pNonVolatileIstateLocation);
	if (ret != 1) {
		// Cannot retrieve, write it back
		m_pNVRAM->StoreBlock(m_NVRAMBlockId, m_nonVolatileIstateSize, m_pNonVolatileIstateLocation);
	}
	
	// Call actions
	m_pKB->VerifyIntegrity();

	m_pKB->InitOldValue();
	m_pKB->ActionAll();
}

void CIstate::SetModified()
{
	m_bModified = 1;
}

void CIstate::OpenIstate()
{
	m_pv.P();
}

void CIstate::CloseIstate()
{
	m_pv.V();
}


void CIstate::TickPerSecond()
{
	if (m_bModified) {
		m_pNVRAM->StoreBlock(m_NVRAMBlockId, m_nonVolatileIstateSize, m_pNonVolatileIstateLocation);
		m_bModified = 0;
		m_modificationCnt++;
	}
}

// return 1: success in rtrv info from RTE Istate
// ERROR_UNKNOWN_IOC_ISTATE_ID: Cannot find it from knowledge base.
// ERROR_INDEX1_OOR: Index1 out of range
// ERROR_INDEX2_OOR: Index2 out of range
// ERROR_IOC_ISTATE_INTERNAL01: Internal error, unexpected item size from CIstateDesc
// ERROR_IOC_ISTATE_INTERNAL02: Istate Item point to nothing
int CIstate::ReadOneIstateItem(int16 item, int16 index1, int16 index2, uint32 *pResult)
{
	uint16 flag; 
	int16 itemSize;
	int16 maxIndex1;
	int16 maxIndex2;
	uint8 *ptr8;
	char const *pName;
	int ret;
	int index;
	uint8 tmp_exr;

    // Get the knowledge base
	ret = m_pKB->GetItemInfo(item, &flag, &itemSize, &maxIndex1, &maxIndex2,
		(void **)(&ptr8), &pName);

	if (ret != 1) {
		return ERROR_UNKNOWN_IOC_ISTATE_ID; // Cannot find that item;
	}

    // If index1, index2 is within range
    if ((index1 < 0) || (index1 >= maxIndex1) ) {
		return ERROR_INDEX1_OOR;
	}
	
	if ((index2 < 0) || (index2 >= maxIndex2) ) {
		return ERROR_INDEX2_OOR;
	}

	if (ptr8 == NULL) return ERROR_IOC_ISTATE_INTERNAL02;

	ret = 1;
	index = index2 * maxIndex1 + index1;
	OpenIstate();
	// So far we support BIT8, BIT32 
	// Entry 32-bit
	tmp_exr = get_exr();
	set_exr(7);
	switch (itemSize) {
	case 1:
		// Convert from bit8 to bit32
		*pResult = (uint32)(*(ptr8 + index));
		break;
	case 4:
		*pResult = *(((uint32 *)ptr8) + index);
		break;
	default:
		ret = ERROR_IOC_ISTATE_INTERNAL01;
		break;
	}
	set_exr(tmp_exr);
	
	CloseIstate();
	return ret;

}

// return 1: success in set info to RTE Istate
// return 2: Write to stand alone variable
// return 5: success in set info to RTE Istate. But need to reset it after it's relayed to twin RTE.
// 0: same value (obsolete for now 4/8/2003)
// ERROR_VALUE_OOR: Entry value out of range
// ERROR_UNKNOWN_IOC_ISTATE_ID -2: Cannot find it from knowledge base.
// ERROR_INDEX1_OOR -3: Index1 out of range
// ERROR_INDEX2_OOR: Index2 out of range
// ERROR_IOC_ISTATE_INTERNAL01: Internal error, unexpected item size from CIstateDesc
// ERROR_IOC_ISTATE_INTERNAL02: Istate Item point to nothing
// ERROR_READ_ONLY_ISTATE: Write to read only Istate
// This function can only be called by Istate process. Other process should 
// try to call this even though it's public.
// It writes the Istate item to local storage.
int CIstate::WriteOneIstateItemByIstateProcess(int16 cmd, int16 item, int16 index1, int16 index2, uint32 value)
{
	uint16 flag; 
	int16 itemSize;
	int16 maxIndex1;
	int16 maxIndex2;
	int index;
	uint8 *ptr8; uint32 *ptr32;
	char const *pName;
	int ret;
	int32 minEntry, maxEntry;
	uint8 tmp_exr;
	uint32 oldValue;
	CIstateIntf *pItemIntf = NULL;

    // Get the knowledge base
	ret = m_pKB->GetItemInfo(item, &flag, &itemSize, &maxIndex1, &maxIndex2,
		(void **)(&ptr8), &pName, &pItemIntf);

	// Check for value range
	int tmp = m_pKB->IsValueOK(item, index1, index2, (int32)value);
	if (tmp != 1) return tmp;
	
	if (flag & FLAG_READ_ONLY_ISTATE) {
		return ERROR_READ_ONLY_ISTATE;
	}

	if (ret != 1) {
		return ERROR_UNKNOWN_IOC_ISTATE_ID; // Cannot find that item;
	}

    // If index1, index2 is within range
    if ((index1 < 0) || (index1 >= maxIndex1) ) {
		return ERROR_INDEX1_OOR;
	}
	
	if ((index2 < 0) || (index2 >= maxIndex2) ) {
		return ERROR_INDEX2_OOR;
	}

	if (ptr8 == NULL) return ERROR_IOC_ISTATE_INTERNAL02;
	
	ret = (flag & FLAG_STANDALONE_ISTATE) ? (2) : (1);

	index = index2 * maxIndex1 + index1;

	OpenIstate();
	// So far we support BIT8 & BIT32 toggle
	// Entry 32-bit
	switch (itemSize) {
	case 1: {
		uint8 old;
		old = *(ptr8 + index);
		if (old == (uint8)value) ret = 0; 
		*(ptr8+index) = value;
		oldValue = (uint32)old;
	}
		break;
	case 4: {
		uint32 old;

		ptr32 = (uint32 *)ptr8;
		old = *(ptr32 + index);
		if (old == value) ret = 0;
		tmp_exr = get_exr();
		set_exr(7);
		*(ptr32 + index) = value;
		set_exr(tmp_exr);
		oldValue = old;
	}
		break;
	default:
		ret = ERROR_IOC_ISTATE_INTERNAL01;
		break;
	}
	CloseIstate();
	if (ret > 0) {
		// Exec the action associated
		m_pKB->ExecAction(item, index1, index2, value);
		// Store it into NVRAM. If write to stand alone Istate, don't bother with NVRAM.
		// 4/14/2004: For volatile istate, it shouldn't go to flash.
		if ((ret == 1) && (!(flag & FLAG_VOLATILE_ISTATE))) {
			SetModified();
		}
		if (flag & FLAG_AUTO_RESET_ISTATE) {
			ret |= 4;
		}
		if ((pItemIntf != NULL) && (cmd == CMD_WRITE_ISTATE_FROM_IMC)) {
			pItemIntf->WriteFromImc(index1, index2, oldValue, value);
		}
	}
	return ret;
}

// return lines printed
int CIstate::PrintStatus(Ccomm *pComm)
{
	int16 start, end;
	int16 item;
	int ret, total = 0, cnt = 0;
	char ch;
	int limit = 10;

	m_pKB->GetItemRange(&start, &end);

	for (item = start + 1; item < end; item++) {
		ret = PrintIstateItem(pComm, item);
		total += ret;
		cnt += ret;
		if (cnt > limit) {
			int8 bShow = 1;

			do {
				if (bShow) { 
					pComm->Send("Press any key(Q=quit, G=Go without stop, <Space>=Next item other=Next Page)...");
					bShow = 0;
					pComm->flush();
				}
				ret = pComm->Receive(&ch, 1);
				if (ret != 1) continue;
				if ((ch == 'Q') || (ch == 'q')) {
					pComm->Send("Quit\r\n");
					return total;
				}
				if ((ch == 'G') || (ch == 'g')) {
					limit = 0x7fff;
				} else if (ch ==' ') {
					limit = 1;
				} else {
					limit = 10;
				}
				if ((ch == '\r') || (ch == '\n')) {
					ret = 0;
					continue;
				}
			} while (ret != 1);
			pComm->Send("\r                                                                                    \r");
			cnt = 0;
		}
	}
	{
		char buff[100];
		sprintf(buff, "Istate(NVRAM) modification cnt: %ld\r\n", m_modificationCnt);
		pComm->Send(buff);
	}
	return total;
}

const char * CIstate::GetNextHelp(const char *pHelp)
{
	const char *pNext = NULL;
	int len;
	if (pHelp == NULL) return NULL;
	len = strlen(pHelp);
	if ((pHelp[len -2] == 'M') && (pHelp[len-1] == '\r')) {
		pNext = pHelp + len + 1;
	}
	return pNext;
}

// return lines printed
// One of three formats. 
// 1. Single value.
// 2. one dimension value
// 3. Two dimension value
int CIstate::PrintIstateItem(Ccomm *pComm, int16 item, int bPrintHelp /* == 0 */)
{
	uint16 flag; int16 itemSize;
	int16 index1, index2;
	uint8 *ptr8;
	char const *pName;
	char const *pHelpHeader = "========================== Help ===========================\r\n";
	char const *pHelp = NULL;
	char const *pNextHelp = NULL;
	int helpLen;
	int ret;
	char buff[150];
	char vBuff[30];
	int lines = 0, i, j, cnt = 0;
	uint32 value;

	ret = m_pKB->GetItemInfo(item, &flag, &itemSize, &index1, &index2, 
		(void **)(&ptr8), &pName);
	// Cannot fine the istate item
	if (ret != 1) {
		sprintf(buff, "Cannot find %d in %s [%d]\r\n", item, GetName(), ret);
		pComm->Send(buff);
		return -1;
	}
	
	if (bPrintHelp) {
		pHelp = m_pKB->GetHelpString(item);
		helpLen = (pHelp != NULL) ? strlen(pHelp) : 0;
#if 1
		pNextHelp = GetNextHelp(pHelp);
#else
		if ((pHelp != NULL)  && (pHelp[helpLen - 2] == 'M') && (pHelp[helpLen -1] =='\r') ) {
			pNextHelp = pHelp + helpLen + 1;
		}
#endif
	}

	// Single value
	if ((index1 == 1) && (index2 == 1)) {
		ReadOneIstateItem(item, 0, 0, &value);
		sprintf(buff, "%s[Id=%d]\t:\t%ld\t0x%lX\r\n", pName, item, value, value);
		pComm->Send(buff);
		if (pHelp) {
			pComm->Send(pHelpHeader);			
			pComm->Send(pHelp);
			while (pNextHelp != NULL) {
				pComm->Send(pNextHelp);
				pNextHelp = GetNextHelp(pNextHelp);
			}
		}
		return 1;
	}

	// One/Two dimension
	for (j = 0; j < index2; j++)  {
		sprintf(buff, "%s[Id=%d][Indx2=%d, Indx1=0..%d]:\t", pName, item, j, index1 - 1 );
		for (i = 0; i < index1; i++) {
			ReadOneIstateItem(item , i, j, &value);
			sprintf(vBuff, "%ld, ", value);
			strcat(buff, vBuff);
			cnt++;
			if (cnt >= 10) {
				cnt = 0;
				lines++;
				pComm->Send(buff);
				strcpy(buff, "\r\n\t");
			}
		}
		if (cnt != 0) {
			pComm->Send(buff);
			pComm->Send("\r\n");
			lines++;
			cnt = 0;
		} else {
			lines++;
			pComm->Send("\r\n");
		}
	}
	if (pHelp) {
		pComm->Send(pHelpHeader);
		pComm->Send(pHelp);
		while (pNextHelp != NULL) {
			pComm->Send(pNextHelp);
			pNextHelp = GetNextHelp(pNextHelp);
		}
	}
	return lines;
}

// Check if the istate item exists and writable with value.
// It's called by Istate process only
// 1: Passed.
// -1ERROR_VALUE_OOR: Entry value out of range
// -2ERROR_UNKNOWN_IOC_ISTATE_ID: Cannot find it from knowledge base.
// -3ERROR_INDEX1_OOR: Index1 out of range
// -4ERROR_INDEX2_OOR: Index2 out of range
// -5ERROR_IOC_ISTATE_INTERNAL01: Internal error, unexpected item size from CIstateDesc
// -6ERROR_IOC_ISTATE_INTERNAL02: Istate Item point to nothing
// -10ERROR_READ_ONLY_ISTATE: Write to read only Istate 
int CIstate::CheckWritingIstateItem(int16 item, int16 index1, int16 index2, uint32 value, int16 cmd)
{
	uint16 flag; 
	int16 itemSize;
	int16 maxIndex1;
	int16 maxIndex2;
	int index;
	uint8 *ptr8; uint32 *ptr32;
	char const *pName;
	int ret;
	uint32 currentValue;
	uint8 tmp_exr;

	// int32 minEntry, maxEntry;

    // Get the knowledge base
	ret = m_pKB->GetItemInfo(item, &flag, &itemSize, &maxIndex1, &maxIndex2,
		(void **)(&ptr8), &pName);

	if (ret != 1) {
		return ERROR_UNKNOWN_IOC_ISTATE_ID; // Cannot find that item;
	}

	int tmp = m_pKB->IsValueOK(item, index1, index2, value, cmd);
	if (tmp != 1) return tmp;
	
	if (flag & FLAG_READ_ONLY_ISTATE) {
		return ERROR_READ_ONLY_ISTATE;
	}


    // If index1, index2 is within range
    //if ((index1 < 0) || (index1 >= maxIndex1) ) {
	//	return ERROR_INDEX1_OOR;
	//}
	
	//if ((index2 < 0) || (index2 >= maxIndex2) ) {
	//	return ERROR_INDEX2_OOR;
	//}

	if (ptr8 == NULL) return ERROR_IOC_ISTATE_INTERNAL02;

	if ((itemSize != 1) && (itemSize != 4)) return ERROR_IOC_ISTATE_INTERNAL01;

	// Get current value and compare it with value
	index = index2 * maxIndex1 + index1;

	OpenIstate();
	tmp_exr = get_exr();
	set_exr(7);
	switch (itemSize) {
	case 1:
		currentValue = (uint32)(*(ptr8 + index)); 
		break;
	case 4:
		ptr32 = (uint32 *)ptr8;
		currentValue = *(ptr32 + index);
		break;
	default:
		// Should never be here.
		set_exr(tmp);
		CloseIstate();
		return ERROR_IOC_ISTATE_INTERNAL03;
	}
	set_exr(tmp_exr);
	CloseIstate();
	
	if (currentValue == value) return OK_IOC_ISTATE_NO_CHANGE;
	return 1;

}

void CIstate::ReportIdle()
{
	m_pProcess->ReportProcessIdle();
}

void CIstate::AddToNvram()
{
	// Set it up with NVRAM
	m_pNVRAM->SetupBlock(m_NVRAMBlockId, m_nonVolatileIstateSize, CNVRAM::MAGIC_ISTATE);
}
