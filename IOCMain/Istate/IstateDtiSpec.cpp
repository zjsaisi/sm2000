// IstateDtiSpec.cpp: implementation of the CIstateDtiSpec class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateDtiSpec.cpp 1.3 2008/01/28 17:18:24PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
 */

#include "IstateDtiSpec.h"
#include "IstateProcess.h"
#include "IstateDti.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACE TRACE_ISTATE

int s_istateInterfaceObjects = 0;

CIstateDtiSpec::CIstateDtiSpec(int16 id) : CIstateIntf(id)
{
	s_istateInterfaceObjects++;
	m_gotFromTwin = 0;
}

CIstateDtiSpec::~CIstateDtiSpec()
{

}

void CIstateDtiSpec::ReceiveFromTwinIoc(int16 index1, int16 index2, uint32 value)
{
	TRACE("Istate cp from twin id=%d index1=%d index2=%d value=%ld\r\n",
		m_id, index1, index2, value);
	m_gotFromTwin = 1;
}

int CIstateDtiSpec::GotFromTwin(int clear)
{
	int ret;

	ret = m_gotFromTwin;
	if (clear) {
		m_gotFromTwin = 0;
	}
	return ret;
}

void CIstateDtiSpec::WriteFromImc(int16 index1, int16 index2, uint32 old, uint32 now)
{
	switch (m_id) {
	case COMM_RTE_KEY_TOD_SOURCE:
		TRACE("Tod source from %ld to %ld\r\n", old, now);
		if (old == now) return;
		if (old == CIstateDti::TOD_SOURCE_DTI) {
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_DTI1, 0, 0);
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_DTI2, 0, 0);
		} else if (old == CIstateDti::TOD_SOURCE_GPS) {
			// SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_GPS, 0, 0);

		}
		if (now == CIstateDti::TOD_SOURCE_DTI) {
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_GPS, 0, 0);
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_BITS1, 0, 0);
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_STATE, CHAN_BITS2, 0, 0);			
		}

		break;
	}
}
