// PhErrDetect.cpp: implementation of the CPhErrDetect class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: PhErrDetect.cpp 1.1 2007/12/06 11:41:36PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#include "PhErrDetect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define PHASE_ERR_THRESHOLD		154 	/* 4ns peak */
#define PHASE_ERR_MS_ALARM_SET	2304	/* 1.25ns rms; 1.25ns=48, ms is 48^2 */
#define PHASE_ERR_MS_ALARM_CLR	1083	/* 1ns rms; 1ns=38, ms is 38^2 */
#define PHASE_ERR_MS_LIMIT		5929	/* 2ns rms; 2ns=77, ms is 77^2 */
#define BUCKET_UP_LIMIT			256
#define BUCKET_UP				32
#define BUCKET_DOWN				16
#define BUCKET_ALARM_SET		128		/* 4 peak error to trigger alarm */
#define BUCKET_ALARM_CLR		64		/* 4 to 12 good data to clear alarm */

CPhErrDetect::CPhErrDetect(int chan)
{
	m_chan = chan;
	m_bucket = BUCKET_UP_LIMIT;
	m_bucketStable = 0;
	m_rmsStable = 0;
	m_rms = 0L;
	ResetPhaseError();
}

CPhErrDetect::~CPhErrDetect()
{

}

// 1: Stable
// 0: not stable
int CPhErrDetect::FeedPhaseError(int32 phase)
{
	int32 pos;
	int32 sq;

	if (phase < 0) {
		pos = -phase;
	} else {
		pos = phase;
	}
	
	if (pos > PHASE_ERR_THRESHOLD) {
		if (m_bucket < BUCKET_UP_LIMIT) {
			m_bucket += BUCKET_UP;
		} 
		if (pos > 0x4000) {
			pos = 0x4000;
		}
	} else if (m_bucket > 0) {
		m_bucket -= BUCKET_DOWN;
	}
	if (m_bucket >= BUCKET_ALARM_SET) {
		m_bucketStable = 0;
	} else if (m_bucket < BUCKET_ALARM_CLR) {
		m_bucketStable = 1;
	}
	
	// Root Mean Square
	sq = pos * pos;
	m_rms -= m_rms >> 4;
	m_rms += sq >> 4;
	if (m_rms > PHASE_ERR_MS_LIMIT) {
		m_rms = PHASE_ERR_MS_LIMIT;
		m_rmsStable = 0;
	} else if (m_rms > PHASE_ERR_MS_ALARM_SET) {
		m_rmsStable = 0;
	} else if (m_rms < PHASE_ERR_MS_ALARM_CLR) {
		m_rmsStable = 1;
	}

	if ((m_bucketStable) && (m_rmsStable)) return 1;
	return 0;
}

void CPhErrDetect::ResetPhaseError()
{
	m_bucket = BUCKET_ALARM_CLR;
	m_bucketStable = 0;
	m_rms = PHASE_ERR_MS_ALARM_CLR;
	m_rmsStable = 0;
	
}
