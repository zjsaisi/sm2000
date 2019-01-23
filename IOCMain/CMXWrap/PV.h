// PV.h: interface for the CPVOperation class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: PV.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: PV.h 1.3 2009/05/05 13:36:57PDT Zheng Miao (zmiao) Exp  $
 */             

#if !defined(AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_)
#define AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_

#include "resource.h"
#include "g_cmx.h" // Import g_cmx_started

class CPVOperation  
{
public:
	CPVOperation();
	virtual ~CPVOperation();

public:
	int P(int timeout = 0);
	void V(void);
	int Lock(void);
	static void Pause(uint16 ticks);
private:
	uint8 m_resource;
};

#endif // !defined(AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_)
