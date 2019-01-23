// Sema.cpp: implementation of the CSema class.
//
//////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "Sema.h"
#include "cxconfig.h"
#include "cxfuncs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CSema::CSema(int init_count)
{
	uint8 ret;
	m_bValid = 0;
	m_sema = GetSemaAssignment();
	if (m_sema < C_MAX_SEMAPHORES) {
		ret = K_Semaphore_Create(m_sema, init_count);
		if (ret == K_OK) {
			m_bValid = 1;
		}
	}
}

// Should never be called.
CSema::~CSema()
{

}

void CSema::Reset(void)
{
	// Always reset semaphore
	K_Semaphore_Reset(m_sema, 1);
}

// 1: OK 
// -1: TimeOut or error
int CSema::Wait(int max_ticks)
{
	// Wait forever
	uint8 stat;
	stat = K_Semaphore_Wait(m_sema, max_ticks);
	if (stat == K_OK) return 1;
	return -1;
}

void CSema::Post(void)
{
	K_Semaphore_Post(m_sema);
}

void CSema::PostIntr()
{
	K_Intrp_Semaphore_Post(m_sema);
}
