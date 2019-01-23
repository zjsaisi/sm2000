// IstateIntf.h: interface for the CIstateIntf class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateIntf.h 1.1 2007/12/06 11:41:33PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_ISTATEINTF_H__9C12FB0F_0F21_487B_9EAA_1D1DA71D21F2__INCLUDED_)
#define AFX_ISTATEINTF_H__9C12FB0F_0F21_487B_9EAA_1D1DA71D21F2__INCLUDED_

#include "Codeopt.h"
#include "DataType.h"
#include "ConstDef.h"

class CIstateIntf  
{
protected:
	CIstateIntf(int16 id);
	virtual ~CIstateIntf();
	int16 m_id;
public:
	virtual void WriteFromImc(int16 index1, int16 index2, uint32 old, uint32 now);
	virtual void ReceiveFromTwinIoc(int16 index1, int16 index2, uint32 value);
private:
};

#endif // !defined(AFX_ISTATEINTF_H__9C12FB0F_0F21_487B_9EAA_1D1DA71D21F2__INCLUDED_)
