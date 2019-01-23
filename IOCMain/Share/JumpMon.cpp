/* JumpMon.cpp
 * Author: Zheng Miao
 * $Header: JumpMon.cpp 1.1 2008/06/02 15:37:18PDT Zheng Miao (zmiao) Exp  $
 */

#include "JumpMon.h"
#include "comm.h"
#include <stdio.h>

#define MIN_VALUE -2147483648L
#define MAX_VALUE 2147483647L

CJumpMon::CJumpMon(int size)
{
	m_size = size;
	m_pMin = new int32[size];
	m_pMax = new int32[size];
	m_current = 0;
	m_min = MAX_VALUE;
	m_max = MIN_VALUE;
	m_tail = 0;
	m_running = 1;
	m_cnt = 0;
}

CJumpMon::~CJumpMon()
{
	delete []m_pMin;
	delete []m_pMax;
}

void CJumpMon::Update(int32 value)
{
	if (!m_running) return;
	if (m_min > value) m_min = value;
	if (m_max < value) m_max = value;
	m_current= value;
}

void CJumpMon::GoNext(void)
{
	if (!m_running) return;
	m_pMin[m_tail] = m_min;
	m_pMax[m_tail] = m_max;
	m_min = m_current;
	m_max = m_current;
	m_tail++;
	if (m_tail >= m_size) {
		m_tail = 0;
	}
	if (m_cnt < (m_size - 1)) {
		m_cnt++;
	}
}

void CJumpMon::JumpMonHack(Ccomm *pComm, uint32 *param)
{
	int cnt;
	int i;
	char buff[100];
	int index;
	int32 min;
	int32 max;
	min = MAX_VALUE;
	max = MIN_VALUE;
	int32 limit;

	if (m_running) {
		sprintf(buff, "Efc data running: %d seconds\r\n", m_cnt);
		pComm->Send(buff);
	} else {
		sprintf(buff, "Efc data stopped: %d seconds\r\n", m_cnt);
		pComm->Send(buff);
	}
	if ((param[0] == 3) && (param[2] == 1)) {
		cnt = param[3];
		index = m_tail + m_size - cnt - 1;
		for (i = 0; i < cnt;) {
			index++;
			index %= m_size;
			i++;
			sprintf(buff, "[%08ld,%08ld]", m_pMin[index], m_pMax[index]);
			pComm->Send(buff);
			if (m_pMin[index] < min) {
				min = m_pMin[index];
			}
			if (m_pMax[index] > max) {
				max = m_pMax[index];
			}
			if ((i & 3) == 0) {
				pComm->Send("\r\n");
			}
		}
		if (i & 3) {
			pComm->Send("\r\n");
		}
		sprintf(buff, "Min: %08ld  Max: %08ld\r\n", min, max);
		pComm->Send(buff);
		return;
	}

	if ((param[0] == 4) && (param[2] == 2)) {
		int32 diff;
		int outCnt = 0;
		cnt = param[3];
		limit = param[4];
		index = m_tail + m_size - cnt - 1;
		for (i = 0; i < cnt;) {
			i++;
			index++;
			index %= m_size;
			diff = m_pMax[index] - m_pMin[index];
			if (diff > limit) {
				sprintf(buff, "%d: Min=%ld Max=%ld\r\n"
					, i, m_pMin[index], m_pMax[index]);
				pComm->Send(buff);
				outCnt++;
			}
		}
		sprintf(buff, "%d seconds out of limit\r\n", outCnt);
		pComm->Send(buff);
		return;
	}

	if ((param[0] == 3) && (param[2] == 3)) {
		if (param[3]) {
			m_running = 1;
			pComm->Send("EFC monitor started\r\n");
		} else {
			m_running = 0;
			pComm->Send("EFC monitor stopped\r\n");
		}
		return;
	}
}
