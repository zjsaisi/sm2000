// IstateIntf.cpp: implementation of the CIstateIntf class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateIntf.cpp 1.1 2007/12/06 11:41:33PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#include "IstateIntf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateIntf::CIstateIntf(int16 id)
{
	m_id = id;
}

CIstateIntf::~CIstateIntf()
{

}

// Called by item transfer.
void CIstateIntf::ReceiveFromTwinIoc(int16 index1, int16 index2, uint32 value)
{
}

void CIstateIntf::WriteFromImc(int16 index1, int16 index2, uint32 old, uint32 now)
{

}
