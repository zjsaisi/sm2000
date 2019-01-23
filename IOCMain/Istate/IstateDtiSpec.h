// IstateDtiSpec.h: interface for the CIstateDtiSpec class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateDtiSpec.h 1.1 2007/12/06 11:41:32PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_ISTATEDTISPEC_H__8C2F0534_DB13_4AAA_BB91_18C60128CDE9__INCLUDED_)
#define AFX_ISTATEDTISPEC_H__8C2F0534_DB13_4AAA_BB91_18C60128CDE9__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "IstateIntf.h"

class CIstateDtiSpec : public CIstateIntf
{
public:
	virtual void WriteFromImc(int16 index1, int16 index2, uint32 old, uint32 now);
	int GotFromTwin(int clear);
	virtual void ReceiveFromTwinIoc(int16 index1, int16 index2, uint32 value);
	CIstateDtiSpec(int16 id);
	virtual ~CIstateDtiSpec();
private:
	int m_gotFromTwin;
};

#endif // !defined(AFX_ISTATEDTISPEC_H__8C2F0534_DB13_4AAA_BB91_18C60128CDE9__INCLUDED_)
