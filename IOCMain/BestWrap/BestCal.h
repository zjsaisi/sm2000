// BestCal.h: interface for the CBestCal class.
//
//////////////////////////////////////////////////////////////////////
/*
 * 
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: BestCal.h 1.1 2007/12/06 11:41:05PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#if !defined(AFX_BESTCAL_H__90976415_6889_4F44_84F6_5F1AD2768CF3__INCLUDED_)
#define AFX_BESTCAL_H__90976415_6889_4F44_84F6_5F1AD2768CF3__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "NVRAM.h"
#include "bt3_dat.h"

// Change the version number if the structure of Calibration_Data changes
#define BESTIME_CAL_VERSION 0x0001
class Ccomm;

class CBestCal  
{
public:
	void RegisterNvram(void);
	void PrintBestCal(Ccomm *pComm);
	void StopFactoryCal(void);
	void StartFactoryCal(void);
	void Init(void);
	CBestCal(CNVRAM *pNVRAM);
	virtual ~CBestCal();
	void SaveChange(void);
	static CBestCal *s_pBestCal;
private:
	CNVRAM *m_pNVRAM;
	uint32 m_blockId;
	int m_size;
};

extern struct Calibration_Data Cal_Data;

#endif // !defined(AFX_BESTCAL_H__90976415_6889_4F44_84F6_5F1AD2768CF3__INCLUDED_)
