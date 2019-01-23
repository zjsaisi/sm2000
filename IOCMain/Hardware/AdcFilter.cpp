// AdcFilter.cpp: implementation of the CAdcFilter class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: AdcFilter.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: AdcFilter.cpp 1.1 2007/12/06 11:41:28PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */
#include "CodeOpt.h"
#include "DataType.h"
#include "AdcFilter.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPVOperation *CAdcFilter::m_pPV = NULL;

CAdcFilter::CAdcFilter()
{
	m_index = 0;
	memset(m_history, 0, sizeof(m_history));
	if (m_pPV == NULL) {
		m_pPV = new CPVOperation();
	}
}

CAdcFilter::~CAdcFilter()
{

}

void CAdcFilter::PutData(uint16 data)
{
	m_pPV->P();
	m_history[m_index++] = data;
	m_index %= HISTORY_LENGTH;
	m_pPV->V();
}

uint16 CAdcFilter::GetAverge(void)
{
	uint16 min, max;
	uint32 sum;
	uint16 *ptr = m_history;
	int i;
	m_pPV->P();
	sum = (uint32)(*ptr);
	min = max = *ptr;
	for (i = 0; i < HISTORY_LENGTH - 1; i++) {
		ptr++;
		sum += (uint32)(*ptr);
		if (max < *ptr) {
			max = *ptr;
		} else if (min > *ptr) {
			min = *ptr;
		}
	}
	m_pPV->V();
	sum = (sum - max - min) / (HISTORY_LENGTH - 2);
	
  	return((uint16)sum);
}
