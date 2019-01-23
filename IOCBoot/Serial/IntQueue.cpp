// IntQueue.cpp: implementation of the CIntQueue class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IntQueue.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IntQueue.cpp 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "IntQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntQueue::CIntQueue(int16 size)
{
	m_pQueue = new int16[size];
	m_head = 0;
	m_tail = 0;
	m_size = size;
	m_bOverFlow = 0;
	m_bUnderFlow = 0;
	for (int i = 0; i < size; i++) {
		m_pQueue[i] = INVALID;
	}
}

// Should never be called;
CIntQueue::~CIntQueue()
{
	delete []m_pQueue;
}

int16 CIntQueue::Top()
{
	if (m_head == m_tail) return INVALID;
	return m_pQueue[m_head];

}

void CIntQueue::Append(int16 n)
{
	int16 next;
	next = (m_tail >= (m_size - 1)) ? (0) : (m_tail + 1);
	if (next == m_head) {
		// exceed maximum
		m_bOverFlow = 1;
		return;
	}
	m_pQueue[m_tail] = n;
	m_tail = next;
}

void CIntQueue::DeleteTop()
{   
	if (m_head == m_tail) {
		m_bUnderFlow = 1;
		return;
	}
	// A precaution
	m_pQueue[m_head] = INVALID;
	m_head = (m_head >= (m_size - 1)) ? (0) : (m_head + 1);
}
