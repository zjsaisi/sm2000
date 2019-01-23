// PV.h: interface for the CPVOperation class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: PV.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: PV.h 1.2 2009/05/05 15:34:26PDT Zheng Miao (zmiao) Exp  $
 */             

#ifndef AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_
#define AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_

#include "resource.h"
#include "iodefine.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "CxFuncs.h"
#endif
#include "g_cmx.h" // Import g_cmx_started

class CPVOperation  
{
public:
	CPVOperation() { m_resource = GetResourceAssignment(); };
	virtual ~CPVOperation() {};

public:
	void P(void) { if (g_cmx_started) K_Resource_Wait(m_resource, 0); };
	void V(void) { if (g_cmx_started) K_Resource_Release(m_resource); };
	void Pause(uint16 ticks) { if (g_cmx_started) K_Task_Wait(ticks); };
private:
	uint8 m_resource;
};

#endif // !defined(AFX_PV_H__5F6859CD_A025_445B_AC46_35D8FB885546__INCLUDED_)
