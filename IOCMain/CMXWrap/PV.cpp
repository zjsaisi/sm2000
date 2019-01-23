// PV.cpp: implementation of the CPVOperation class.
//
//////////////////////////////////////////////////////////////////////
/*
 * PV operation
 * $Header: PV.cpp 1.2 2009/05/05 13:30:06PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */

#include "iodefine.h"
#include "PV.h"
#include "CxFuncs.h"

CPVOperation::CPVOperation()
{
	m_resource = GetResourceAssignment(); 
}

CPVOperation::~CPVOperation()
{
}

// 2: CMX not started
// 1: OK
// -1: Timeout
// -2: Error;
int CPVOperation::P(int timeout)
{      
	uint8 ret;
	if (!g_cmx_started) return 2;
	
	ret = K_Resource_Wait(m_resource, timeout); 
	
	switch (ret) {
	case K_TIMEOUT: return -1;
	case K_OK: return 1;
	}
	return -2;
}

void CPVOperation::V(void) 
{ 
	if (g_cmx_started) K_Resource_Release(m_resource); 
}

void CPVOperation::Pause(uint16 ticks) 
{ 
	if (g_cmx_started) K_Task_Wait(ticks); 
}

// -1 : CMX not started
// -2 : Already owned.
// -3 : Error
// 1: OK
int CPVOperation::Lock(void)
{
	uint8 ret;
	if (!g_cmx_started) return -1;
	ret = K_Resource_Get(m_resource);
	if (ret == K_OK) return 1;
	if (ret == K_RESOURCE_OWNED) return -2;
	return -3;
}
