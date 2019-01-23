// AdcFilter.h: interface for the CAdcFilter class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: AdcFilter.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: AdcFilter.h 1.2 2008/01/28 14:11:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ADCFILTER_H__4C326669_5EA9_40D3_8AED_312464BAEFBC__INCLUDED_)
#define AFX_ADCFILTER_H__4C326669_5EA9_40D3_8AED_312464BAEFBC__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "PV.h"

class CAdcFilter  
{
public:
	uint16 GetAverge(void);
	void PutData(uint16 data);
	CAdcFilter();
	virtual ~CAdcFilter();
private:
	static CPVOperation *m_pPV;
	enum {HISTORY_LENGTH = 5};
	uint16 m_history[HISTORY_LENGTH];
	int8 m_index;
};

#endif // !defined(AFX_ADCFILTER_H__4C326669_5EA9_40D3_8AED_312464BAEFBC__INCLUDED_)
