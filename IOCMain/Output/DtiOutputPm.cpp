// DtiOutputPm.cpp: implementation of the CDtiOutputPm class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputPm.cpp 1.2 2008/01/29 11:29:26PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 * Collection of routines to send out PM data about output. Phase, Jitter and FER
 */

#include "DtiOutputPm.h"
#include "stdio.h"
#include "string.h"
#include "AllProcess.h"
#include "PTPLink.h"
#include "rtc.h"
#include "Fpga.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define INVALID_PHASE 0x7fff
#define INVALID_JITTER 0xffff

CDtiOutputPm *CDtiOutputPm::s_pDtiOutputPm;

CDtiOutputPm::CDtiOutputPm()
{
	s_pDtiOutputPm = this;
	m_pLink = NULL;
	m_cnt = 0;
	memset(&m_phase, 0, sizeof(m_phase));
	memset(&m_jitter, 0, sizeof(m_jitter));
	m_phase.cmd = CMD_PHASE_FER;
	m_jitter.cmd = CMD_JITTER;
	m_debugJitter = 0;
	m_debugPhaseFer = 0;
	m_jitterSentCnt = 0L;
	m_jitterRejectCnt = 0L;
	m_phaseSentCnt = 0L;
	m_phaseRejectCnt = 0L;
	ResetBuffer();
}

CDtiOutputPm::~CDtiOutputPm()
{

}

void CDtiOutputPm::SetEnable(int chan, int enable)
{
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	m_jitter.enabled[chan] = (enable) ? (1) : (0);
}

void CDtiOutputPm::SetActive(int active)
{
	if (active) {
		m_jitter.iocActive = 1;
		m_phase.iocActive |= (uint16)1 << m_cnt;
	} else {
		m_jitter.iocActive = 0;
	}
}

void CDtiOutputPm::SendPhase(int chan, int16 phase)
{
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	m_phase.phase[chan][m_cnt] = phase;
}

int CDtiOutputPm::TickPerSecond()
{
	int chan, i;
	uint32 curTime;
	int ret = 0;
	int sent;
	int oktoDebug = 0;

	m_cnt++;
	curTime = m_jitter.timeStamp = CFpga::s_pFpga->GetGpsSec(); //g_pRTC->GetComputerSeconds();
	
	// Send Jitter packet.
	sent = m_pLink->PostFrame((char *)&m_jitter, sizeof(m_jitter), PTP_PORT_PM_REPORT);
	if (sent == 1) {
		m_jitterSentCnt++;
		oktoDebug = 1;
	} else {
		if (sent == -3) oktoDebug = 1; // not connected.
		m_jitterRejectCnt++;
	}

	// Print Jitter packet
	if (m_debugJitter && oktoDebug) {			
		FORCE_TRACE("Cmd=0x%X TimeStamp=0x%08lX(%ld)\r\nJitter:", 
			m_jitter.cmd, m_jitter.timeStamp, m_jitter.timeStamp);
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			FORCE_TRACE("[%d]=%u ", 
				chan, m_jitter.jitter[chan]);
		}
		FORCE_TRACE("\r\nFER: ");
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			FORCE_TRACE("%d ", m_jitter.fer[chan]);
		}
		FORCE_TRACE("\r\nEnable: ");
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			FORCE_TRACE("%d ", m_jitter.enabled[chan]);
		}
		FORCE_TRACE("iocAcive=%d\r\n", m_jitter.iocActive);
		FORCE_TRACE("\r\n");
	}

	if (m_cnt >= NUM_PHASE_POINT) {
		m_phase.timeStamp = curTime;
		oktoDebug = 0;
		// report Phase/FER packet to PPC
		sent = m_pLink->PostFrame((char *)&m_phase, sizeof(m_phase), PTP_PORT_PM_REPORT);
		if (sent == 1) {
			m_phaseSentCnt++;
			oktoDebug = 1;
		} else {
			if (sent == -3) oktoDebug = 1;
			m_phaseRejectCnt++;
		}
		// print phase fer
		if (m_debugPhaseFer && oktoDebug) {
			FORCE_TRACE("Cmd=0x%X TimeStamp=0x%lX(%ld) iocActive=0x%X\r\n",
				m_phase.cmd, m_phase.timeStamp, m_phase.timeStamp, m_phase.iocActive);
			for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
				//FORCE_TRACE("Chan[%d]: FER=%lu ", chan, m_phase.fer[chan]);
				for (i = 0; i < NUM_PHASE_POINT; i++) {
					FORCE_TRACE("[%d]=%d ", i, m_phase.phase[chan][i]);
				}
				FORCE_TRACE("\r\n");
			}
		}
		ResetBuffer();
		m_cnt = 0;
		ret = 1;
	} else {
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			m_jitter.jitter[chan] =  INVALID_JITTER;
			m_jitter.fer[chan] = 0;
		}
		m_jitter.iocActive = 0;
	}
	return ret;
}

void CDtiOutputPm::SendFer(int chan, uint16 cnt)
{
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	//m_phase.fer[chan] += cnt;
	m_jitter.fer[chan] = cnt;
}

void CDtiOutputPm::ResetBuffer()
{
	int chan, i;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		for (i = 0; i < NUM_PHASE_POINT; i++) {
			m_phase.phase[chan][i] = INVALID_PHASE;
		}
		// m_phase.fer[chan] = 0L;
		m_jitter.jitter[chan] = INVALID_JITTER;
		m_jitter.fer[chan] = 0;
	}
	m_phase.iocActive = 0;
	m_jitter.iocActive = 0;
	
}

void CDtiOutputPm::Init()
{
	m_pLink = CAllProcess::g_pAllProcess->GetImcLink();
}

void CDtiOutputPm::SendJitter(int chan, uint16 jitter)
{
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	m_jitter.jitter[chan] = jitter;
}

void CDtiOutputPm::DtiOutputPmHack(unsigned long *param, Ccomm *pComm)
{
	int ok = 0;
	char buff[150];
	if (*param == 1) {
		switch (param[1]) {
		case 1:
			m_debugPhaseFer = 1;
			pComm->Send("Phase debug enabled\r\n");
			return;
		case 2:
			m_debugPhaseFer = 0;
			pComm->Send("Phase debug disabled\r\n");
			return;
		case 3:
			m_debugJitter = 1;
			pComm->Send("Jitter/FER debug enabled\r\n");
			return;
		case 4:
			m_debugJitter = 0;
			pComm->Send("Jitter/FER debug disabled\r\n");
			return;
		case 5:
			sprintf(buff, "Jitter Packet Sent=%ld  Rejected=%ld\r\n"
				"Phase Packet Sent=%ld Rejected=%ld\r\n"
				, m_jitterSentCnt, m_jitterRejectCnt
				, m_phaseSentCnt, m_phaseRejectCnt
				);
			pComm->Send(buff);
			return;
		}
	}

	if (!ok) {
		const char *pHelp;
		pHelp = "Syntax: <option>\r\n"
			"\t1 : Enable Phase data print(10s)\r\n"
			"\t2 : Disable Phase data print(10s)\r\n"
			"\t3 : Enable Jitter/FER print(1s)\r\n"
			"\t4 : Disable Jitter/FER print(1s)\r\n"
			"\t5 : Status\r\n"
			;
		pComm->Send(pHelp);
	}
}

void CDtiOutputPm::ResetPhase(int phase)
{
	m_cnt = phase;
}
