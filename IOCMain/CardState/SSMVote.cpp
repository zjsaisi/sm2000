// SSMVote.cpp: implementation of the CSSMVote class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: SSMVote.cpp 1.3 2011/07/26 17:42:37PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.3 $
 *
 */

#include "SSMVote.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPVOperation *CSSMVote::m_pPV = NULL;

CSSMVote::CSSMVote()
{       
	if (m_pPV == NULL) {
		m_pPV = new CPVOperation();
	}
	m_init = -1;
	m_currentQL = m_init;
	m_age = -2147483648L;
	for (int i = 0; i < FILTER_SIZE; i++) {
		m_qlList[i] = -1;
	}
	m_index = 0;
}

CSSMVote::~CSSMVote()
{

}

// Return QL 
int CSSMVote::Update(int ql)
{
	int i;
	int agreed = 1;
	int ret;
	
	// 11/21/2005 zmiao: For 10, it's not really defined.
	// 2/26/2011 jyang: Use 15 for any value greater than 9
	if (ql >= 10) {
		ql = 15;
	}

	m_pPV->P();

	m_qlList[m_index++] = ql;
	m_index %= FILTER_SIZE;
	
	if (ql > 0) {
		for (i = 0; i < FILTER_SIZE; i++) {
			if (m_qlList[i] != ql) {
				agreed = 0;
				break;
			}
		}
	} else {
		agreed = 0;
	}

	if (agreed) {
		// Enough valid QL
		m_currentQL = ql;
		m_age = 0;
	} else {
		m_age++;
		if (m_age > 10) {
			// Too old
			m_currentQL = -1;
		}
	}

	ret = m_currentQL;
	m_pPV->V();

	return ret;
}

void CSSMVote::InitSsm(int init)
{
	m_pPV->P();
	m_init = init;
	m_currentQL = m_init;
	m_age = -2147483648L;
	for (int i = 0; i < FILTER_SIZE; i++) {
		m_qlList[i] = -1;
	}
	m_index = 0;
	m_pPV->V();
}
