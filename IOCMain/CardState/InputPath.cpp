// InputPath.cpp: implementation of the CInputPath class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputPath.cpp 1.3 2008/03/11 10:12:07PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */             

#include "InputPath.h"
#include "IstateDti.h"
#include "IstateProcess.h"
#include "stdio.h"
#include "string.h"
#include "DtiOutputTrace.h"
#include "CardProcess.h"

#define TRACE TRACE_CARD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
uint32 const CInputPath::m_invalidIpv6[4] = {0L, 0L, 0L, 0L};
int CInputPath::m_activeInputPath = -1;

CInputPath::CInputPath(int chan)
{
	m_chan = chan;
	m_index = chan - CHAN_DTI1;
	ResetPathInfo();
	m_validCnt = 0;
	m_change = 1;
	m_errorCnt = 0L;
	m_totalValidCnt = 0L;
	m_isValid = 0;
}

CInputPath::~CInputPath()
{

}

void CInputPath::ReportRootIpv4(uint32 ip)
{
	if (m_ipv4 != ip) {
		m_ipv4 = ip;
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR, m_index, 0, m_ipv4);
		m_change = 1;
	}
}

void CInputPath::ReportRootIpv6(char *ptr)
{
	if (memcmp(m_ipv6, ptr, sizeof(m_ipv6)) != 0) {
		memcpy(m_ipv6, ptr, sizeof(m_ipv6));
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 0, m_index, m_ipv6[0]);
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 1, m_index, m_ipv6[1]);
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 2, m_index, m_ipv6[2]);
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 3, m_index, m_ipv6[3]);
		m_change = 1;
	}
}

void CInputPath::ReportRootDtiVersion(int version)
{
	if (m_version != version) {
		m_version = version;
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION, m_index, 0, (uint32)m_version);
		m_change = 1;
	}
}

void CInputPath::ReportRootPort(int port)
{
	if (m_portNumber != port) {
		m_portNumber = port;
		SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_PORT, m_index, 0, (uint32)m_portNumber);
		m_change = 1;
	}
}

void CInputPath::PathInvalid(int isLos)
{
	m_errorCnt++;
	if (m_validCnt >= 0) {
		m_validCnt = -1;
		return;
	}
	m_validCnt--;
	if (m_validCnt == -5) {
		m_change = 1;
		ReportRootDtiVersion(0);
		ReportRootIpv4(0L);
		ReportRootIpv6((char *)m_invalidIpv6);
		ReportRootPort(0);
		ReassignIstate();
		PostPath();
		TRACE("Clear all path traceability on chan%d\r\n", m_chan);
	} else if (m_validCnt <= -20000) {
		m_validCnt = -10000;
	} else if ((m_validCnt & 7) == 0) {
		if (isLos) {
			m_isValid = 0;
		} else {
			m_isValid = -1;
		}
	}

}

void CInputPath::ResetPathInfo()
{
	m_ipv4 = 0;
	memset(m_ipv6, 0, sizeof(m_ipv6));
	m_portNumber = 0;
	m_version = 0;
}

int CInputPath::IsPathValid()
{
	if (m_isValid) return m_isValid;
	if (m_validCnt >= 0) return 1;
	return 0;
	//return m_validCnt;
}

void CInputPath::PostPath()
{
	if (m_activeInputPath != m_chan) {
		m_change = 1;
		return;
	}
	if (m_change == 0) return;
	m_change = 0;

	CDtiOutputTrace::CfRootDtiVersion(m_version);
	CDtiOutputTrace::CfRootIpFour(m_ipv4);
	CDtiOutputTrace::CfRootIpSix((char *)m_ipv6);
	CDtiOutputTrace::CfRootPortNumber(m_portNumber);
}

void CInputPath::PathValid()
{
	if (m_validCnt <= 0) {
		m_validCnt = 0;
		ReassignIstate();
	} else if (m_validCnt >= 20000) {
		m_validCnt = 10000;
	} else if ((m_validCnt & 3) == 0) {
		m_isValid = 1;
	}
	m_validCnt++;
	m_totalValidCnt++;
	PostPath();
}

void CInputPath::SetActiveInput(int chan)
{
	if (m_activeInputPath != chan) {
		m_activeInputPath = chan;
	}
}

void CInputPath::CardInactive()
{
	m_change = 1;
	m_validCnt = 0;
	m_errorCnt = 0L;
	m_totalValidCnt = 0L;
	m_isValid = 0;
}

void CInputPath::InputPathHack(unsigned long *param, Ccomm *pComm)
{
	char buff[150];
	

	sprintf(buff, "Path Traceability validCnt=%d  change=%d ErrorCnt=%ld TotalValidCnt=%ld\r\n",
		m_validCnt, m_change, m_errorCnt, m_totalValidCnt);
	pComm->Send(buff);
}

void CInputPath::ReassignIstate()
{
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_PORT, m_index, 0, (uint32)m_portNumber);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION, m_index, 0, (uint32)m_version);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR, m_index, 0, m_ipv4);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 0, m_index, m_ipv6[0]);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 1, m_index, m_ipv6[1]);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 2, m_index, m_ipv6[2]);
	SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, 3, m_index, m_ipv6[3]);
}
