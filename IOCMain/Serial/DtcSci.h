// DtcSci.h: interface for the CDtcSci class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DtcSci.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DtcSci.h 1.1 2007/12/06 11:41:38PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_)
#define AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "iodefine.h"
#include "ConstDef.h"

class Ccomm;

class CDtcSci  
{
private:

public:
	void DtcSciHack(Ccomm *pComm);
	static int CheckVector(unsigned int vector);
	int GetDtcCh(char *pCh);
	CDtcSci(unsigned int vector);
	virtual ~CDtcSci();
private:
	enum { BUFF_SIZE = 256 };
	uint8 m_repeatBuffer[BUFF_SIZE+2];
	SDTCReg *m_pReg;
	int m_rxHead;
};

#endif // !defined(AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_)
