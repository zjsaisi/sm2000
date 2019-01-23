// DtiDelayStable.cpp: implementation of the CDtiDelayStable class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiDelayStable.cpp 1.2 2009/05/01 09:39:32PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include <string.h>
#include "DtiDelayStable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDtiDelayStable *CDtiDelayStable::s_pDtiDelayStable[NUM_OUTPUT_CHAN];

CDtiDelayStable::CDtiDelayStable(int chan)
{
	m_chan = chan;
	Reset();
	memset(m_data, 0, sizeof(m_data));
	memset(m_diff, 0, sizeof(m_diff));
}

CDtiDelayStable::~CDtiDelayStable()
{

}

void CDtiDelayStable::Create()
{
	int chan;

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		s_pDtiDelayStable[chan] = new CDtiDelayStable(chan);
	}
}

void CDtiDelayStable::Init()
{

}

// return max diff
int32 CDtiDelayStable::Feed(int32 data)
{
	int i;
	int pre;
	int cnt;
	int next;
	int32 diff;

	next = (m_last + 1) % NUM_DATA;
	m_data[next] = data;
	diff = data - m_data[m_last];
	if (diff < 0) {
		m_diff[next] = -diff;
	} else {
		m_diff[next] = diff;
	}
	cnt = (m_last - m_first + NUM_DATA) % NUM_DATA;

	if (cnt < MINI_NUM) {
		if (cnt == 0) {
			m_diff[next] = 0L;
		}
		m_last = next;
		return 0x7fffffffL;
	}
	if (cnt >= NUM_DATA - 1) {
		m_first = (m_first + 1) % NUM_DATA;
	}

	pre = m_last;
	for (i = 0; (i < MAX_NUM - 2) && (i < cnt - 1); i++) {
		if (diff < m_diff[pre]) {
			diff = m_diff[pre];
		}
		pre = (pre + NUM_DATA - 1) % NUM_DATA;
	}
	m_last = next;
	return diff & 0x3fffffffL;
}

void CDtiDelayStable::Reset()
{
	m_first = m_last = 0;
}
