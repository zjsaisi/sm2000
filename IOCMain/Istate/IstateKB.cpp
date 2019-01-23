// IstateKB.cpp: implementation of the CIstateKb class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateKB.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateKB.cpp 1.3 2008/05/05 11:29:31PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "IstateKb.h"
#include "string.h"
#include "errorcode.h"
#include "EventId.h"
#include "IstateProcess.h"
#include "machine.h"
#include "AllProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TRACE  TRACE_ISTATE

CIstateKb::CIstateKb(uint16 version, int16 start, int16 end)
{
	m_istateStart = start;
	m_istateEnd = end;
	m_istateVersion = version;
	m_keyList = new void *[m_istateEnd - m_istateStart + 1];
	memset(m_keyList, 0, sizeof(void *) * (m_istateEnd - m_istateStart + 1));
}

CIstateKb::~CIstateKb()
{

}

// Init entry istate item 
void CIstateKb::InitEntryIstate(void *ptr)
{
	uint8 tmp_exr;

	SEntryIstate *pEntry;
	int i, size;
	pEntry = (SEntryIstate *)ptr;
	int32 init = pEntry->initParam;
	int32 *dest = (int32 *)(pEntry->pVariable);
	size = pEntry->max_first_index * pEntry->max_second_index;

	tmp_exr = get_exr();
	set_exr(7);
	for (i = 0; i < size; i++) {
		*dest++ = init;
	}
	set_exr(tmp_exr);
}

// Init toggle istate item
void CIstateKb::InitToggleIstate(void *ptr)
{
	uint8 tmp_exr;
	SToggleIstate *pToggle;
	int i, size;
	pToggle = (SToggleIstate *)ptr;
	size = pToggle->max_first_index * pToggle->max_second_index;
	switch (pToggle->size_type) {
	case BIT8: {
		int8 init = (int8)(pToggle->initParam);
		int8 *dest = (int8 *)(pToggle->pVariable);
		tmp_exr = get_exr();
		set_exr(7);
		for (i = 0; i < size; i++) {
			*dest++ = init;
		}
		set_exr(tmp_exr);
			   }
		break;
	case BIT32: {
		int32 init = pToggle->initParam;
		int32 *dest = (int32 *)(pToggle->pVariable);
		tmp_exr = get_exr();
		set_exr(7);
		for (i = 0; i < size; i++) {
			*dest++ = init;
		}
		set_exr(tmp_exr);
				}
		break;
	default:
		// Don't know how to init it. Should never happen 
		break;
	}
}

int CIstateKb::InitOneIstate(int16 item)
{
	FUNC_INIT pInit;
	SToggleIstate *pToggle;
	int ret = 0;

	pToggle = (SToggleIstate *)GetDesc(item);
	// Point to nowhere, no initialization
	if (pToggle == NULL) return -1; // Cannot find it
#if 0	// Let it init read-only istate. Shouldn't hurt anything.
	if (pToggle->flag & FLAG_READ_ONLY_ISTATE) {
		// Read only Istate
		return -1;
	}
#endif
	
	if (pToggle->flag & FLAG_IS_TOGGLE_ISTATE) {
		// Toggle Istate
		pInit = pToggle->pInit;
		if (pInit != NULL) {
			// If initialization code exists at all
			(*pInit)(pToggle);
			ret = 1;
		}
	} else {
		// Entry istate
		SEntryIstate *pEntry = (SEntryIstate *)pToggle;
		pInit = pEntry->pInit;
		if (pInit != NULL) {
			// If initialization code exists at all.
			(*pInit)(pEntry);
			ret = 1;
		}
	}
	return ret;
}

void CIstateKb::InitAllIstate()
{
	int16 item;
	
	TRACE("Init all Istate\r\n");
	for (item = m_istateStart + 1; item < m_istateEnd; item++) {
		InitOneIstate(item);
	} 

}

// 1: OK
// -1: cannot find
// -2: Internal error
int CIstateKb::GetItemInfo(int16 id, uint16 *pFlag, int16 *pItemSize, 
		int16 *pIndex1,	int16 *pIndex2, void **pAddress, const char **pName
		, CIstateIntf **pExtraIntf /*= NULL*/)
{
	int ret = 1;
	
	SToggleIstate *pToggle;
	SEntryIstate *pEntry;

	
	pToggle = (SToggleIstate *)GetDesc(id);
	
	if (pToggle == NULL) return -1;

	pEntry = (SEntryIstate *)pToggle;
	
	if (pFlag) *pFlag = pToggle->flag;

	if (pToggle->flag & FLAG_IS_TOGGLE_ISTATE) {
		// Toggle Istate
		switch (pToggle->size_type) {
		case BIT8:
			*pItemSize = 1;
			break;
		case BIT32:
			*pItemSize = 4;
			break;
		default:
			// Unexpected
			*pItemSize = 0;
			ret = -2;
			break;
		}
		*pIndex1 = pToggle->max_first_index;
		*pIndex2 = pToggle->max_second_index;
		*pAddress = pToggle->pVariable;
		if (pName) *pName = pToggle->name;
	} else {
		// Entry Istate
		*pItemSize = 4; // 32-bit entry
		*pIndex1 = pEntry->max_first_index;
		*pIndex2 = pEntry->max_second_index;
		*pAddress = pEntry->pVariable;
		if (pName) *pName = pEntry->name;
	}

	if (pExtraIntf) {
		*pExtraIntf = pToggle->pInterface;
	}
	return ret;

}

// Get the range of Istate item id. non-inclusive.
void CIstateKb::GetItemRange(int16 *pStart, int16 *pEnd)
{
	*pStart = m_istateStart;
	*pEnd = m_istateEnd;
}

void CIstateKb::InitCopyIstate(void *ptr)
{
	const SToggleIstate *pToggle;
	uint8 *pDest;
	uint8 *pSrc;

	int16 size = 4; // Entry Istate is always 4-byte

	pToggle = (SToggleIstate *)ptr;
	if (pToggle->flag & FLAG_IS_TOGGLE_ISTATE) {
		switch (pToggle->size_type) {
		case BIT8:
			size = 1;
			break;
		case BIT32:
			size = 4;
			break;
		default:
			// Should never happen
			size = 1;
			break;
		}
	}

	size *= pToggle->max_first_index * pToggle->max_second_index;
	pSrc = (uint8 *)(pToggle->initParam);
	pDest = (uint8 *)(pToggle->pVariable);
	if (pDest != NULL) {
		memcpy(pDest, pSrc, size);
	}
}

// 1 : OK
// < 0: error or no found
#if 0
int CIstateKb::GetEntryMinMax(int16 id, int32 *pMin, int32 *pMax)
{
	SEntryIstate *pEntry;

	pEntry = (SEntryIstate *)GetDesc(id);
	if (pEntry == NULL) return -1;
	if (pEntry->flag & FLAG_IS_TOGGLE_ISTATE) return -2;
	*pMin = pEntry->minEntryValue;
	*pMax = pEntry->maxEntryValue;
	return 1;
}
#endif

void CIstateKb::ExecAction(int16 id, int16 index1, int16 index2, uint32 value)
{
	SToggleIstate *pToggle;
	pToggle = (SToggleIstate *)GetDesc(id);
	if (pToggle != NULL) {
		(*(pToggle->pAction))(index1, index2, value);
	}

}

void CIstateKb::ActionNull(int16 index1, int16 index2, uint32 value)
{
	// Don't do anything here
}

int CIstateKb::AddIstateItemDesc(int16 evtId, void *pDesc)
{
	int index;
	if ((evtId <= m_istateStart) || (evtId >= m_istateEnd)) {
		return -1;
	}
	index = evtId - m_istateStart - 1;
	m_keyList[index] = pDesc;
}

void CIstateKb::ActionAll(void) 
{
	SToggleIstate *pItem;
	int16 id;
	ActionRoutine pAction;
    
	TRACE("Take all action in CIstateKb\r\n");
	for (id = m_istateStart + 1; id < m_istateEnd; id++) {
		pItem = (SToggleIstate *)GetDesc(id);
		if (pItem != NULL) {
			pAction = pItem->pAction;
			if (pAction != NULL) {
				(*pAction)(-1, -1, 0L);
			}
		}
	}

	CAllProcess::AllAction();
}

void *CIstateKb::GetDesc(int16 id) 
{
	if ((id <= m_istateStart) || (id >= m_istateEnd)) {
		return NULL;
	}
	return m_keyList[id - m_istateStart -1 ];
}

void CIstateKb::VerifyIntegrity(void)
{
}

// return 1: For OK
// return ERROR_UNKNOWN_IOC_ISTATE_ID: Cannot find id
// return ERROR_INDEX1_OOR: index1 out of range
// return ERROR_INDEX2_OOR: index2 out of range
// return ERROR_VALUE_OOR: value out of range
int CIstateKb::IsValueOK(uint16 id, int16 index1, int16 index2, int32 value, int16 cmd)
{
	SEntryIstate *pDesc;

	pDesc = (SEntryIstate *)GetDesc(id);
	if (pDesc == NULL) {
		return ERROR_UNKNOWN_IOC_ISTATE_ID;
	}
	if ((index1 < 0) || (index1 >= pDesc->max_first_index)) {
		return ERROR_INDEX1_OOR;
	}
	if ((index2 < 0) || (index2 >= pDesc->max_second_index)) {
		return ERROR_INDEX2_OOR;
	}
	if (pDesc->flag & FLAG_IS_TOGGLE_ISTATE) {
		// Toggle, limit it to 0 or 1 so far
		if (value & 0xfffffffe) return ERROR_VALUE_OOR;
		if (pDesc->pSpecialValueCheck) {
			int tmp;
			tmp = pDesc->pSpecialValueCheck(index1, index2, value, cmd);
			if (tmp != 1) return tmp;
		}
		// Add any further check here.
		return 1;
	}
	if ((value < pDesc->minEntryValue) || (value > pDesc->maxEntryValue)) {
		return ERROR_VALUE_OOR;
	}
	if (pDesc->pSpecialValueCheck) {
		return(pDesc->pSpecialValueCheck(index1, index2, value, cmd));
	} 
		
	return 1;
}

char const * CIstateKb::GetHelpString(int16 id)
{
	SToggleIstate *pToggle;

	pToggle = (SToggleIstate *)GetDesc(id);	
	if (pToggle == NULL) return NULL;

	return pToggle->pIstateHelp;
}

void CIstateKb::ResetIstateValue(int16 id)
{
	InitOneIstate(id);
}

uint16 CIstateKb::GetIstateFlag(int16 id)
{
	SToggleIstate *pItem;

	pItem = (SToggleIstate *)GetDesc(id);
	if (pItem == NULL) return 0;
	return pItem->flag;
}

