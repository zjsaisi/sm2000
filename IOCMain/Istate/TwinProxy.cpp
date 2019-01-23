// TwinProxy.cpp: implementation of the CTwinProxy class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: TwinProxy.cpp 1.4 2009/05/01 09:27:01PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */

#include "TwinProxy.h"
#include "string.h"
#include "Fpga.h" 
#include "PTPLink.h"
#include "RTC.h"
#include "stdio.h"
#include "DtiOutput.h"
#include "CardProcess.h"
#include "CardStatus.h"
#include "CardLoop.h"
#include "CardAlign.h"
#include "NewCode.h"
#include "AllProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTwinProxy *CTwinProxy::s_pTwinProxy = NULL;

CTwinProxy::CTwinProxy(CPTPLink *pIocLink, int port)
{
	s_pTwinProxy = this;
	m_port = port;
	m_pIocLink = pIocLink;
	m_inShelf = 0;
	m_twinInShelf = 0;
	m_twinRun = 0L;
	m_fresh = 0;
	m_twinLost = 100;
}

CTwinProxy::~CTwinProxy()
{

}

void CTwinProxy::Init()
{

}

void CTwinProxy::TickOneSecondTwinProxy()
{
	m_fresh++;
	m_inShelf++;
	if (CFpga::s_pFpga->IsTwinIn() == 1) {
		m_twinInShelf++;
		if (m_twinInShelf > 10) {
			m_twinLost = 0;
		} else if (m_twinInShelf == 10) {
			CPTPLink *pLink;
			pLink = CAllProcess::g_pAllProcess->GetIocLink();
			pLink->SetEvent(COMM_RTE_EVENT_RTE_COM_ERROR);
		}
	} else if (++m_twinLost >= 3) {
		m_twinInShelf = 0;
		m_twinRun = 0L;
		if (m_twinLost == 4) {
			CPTPLink *pLink;
			pLink = CAllProcess::g_pAllProcess->GetIocLink();
			pLink->SetEvent(0);
		} else if (m_twinLost > 20000) {
			m_twinLost = 10000;
		}
	}

	if ((m_inShelf & 0xff) == 1) {
		CorrectCnt();
	}
}

int CTwinProxy::SendShelfLife()
{
	SShelfLife life;
	
	if (m_fresh > 20000) m_fresh = 20000;
	memset(&life, 0, sizeof(life));
	life.cmd = CMD_SHELF_LIFE;
	life.this_in = m_inShelf;
	life.twin_in = m_twinInShelf;
	life.this_run = g_pRTC->GetAbsoluteSecond();
	return m_pIocLink->PostFrame((char *)&life, sizeof(life), m_port);
}

int CTwinProxy::ReceiveShelfLife(const char *ptr, int size)
{
	SShelfLife *pLife;
	
	if (size != sizeof(SShelfLife)) return -1;
	pLife = (SShelfLife *)ptr;
	if (pLife->twin_in > m_inShelf + 10) {
		if (m_inShelf < 10000) {
			m_inShelf = pLife->twin_in;
		}
	}
	if (pLife->this_in > m_twinInShelf + 10) {
		if (m_twinInShelf < 10000) {
			m_twinInShelf = pLife->this_in;
		}
	}
	m_twinRun = pLife->this_run;
	m_fresh = 0;
	if (m_twinRun < 70) return 10;
	return 0;
}

void CTwinProxy::CorrectCnt()
{
	if (m_inShelf > 20000) {
		m_inShelf = 10000;
	}
	if (m_twinInShelf > 20000) {
		m_twinInShelf = 10000;
	}
	if (CFpga::s_pFpga->IsTwinIn() == 0) {
		m_twinInShelf = 0;
		m_twinRun = 0L;
	}
	if (m_fresh > 300) {
		// Maybe twin is dead
		m_twinInShelf = 0;
		m_twinRun = 0L;
	}
}

int CTwinProxy::ReceiveProxy(const char *ptr, int size)
{
	uint16 cmd;

	cmd = *((uint16 *)ptr);
	if ((cmd >= CMD_END) || (cmd <= CMD_START)) return -1;

	switch (cmd) {
	case CMD_SHELF_LIFE:
		return ReceiveShelfLife(ptr, size);
		break;
	case CMD_OUTPUT_CONTROL:
		CDtiOutput::ReceiveOutputControl(ptr, size);
		return 0;
	case CMD_CARD_STATE:
		CCardProcess::s_pCardProcess->ReceiveTwinCardState(ptr, size);
		return 0;
	}
	return -1;
}

void CTwinProxy::TwinProxyHack(unsigned long *param, Ccomm *pComm)
{
	char buff[250];
	if (*param == 1) {
		switch (param[1]) {
		case 1:
			// Print status
			sprintf(buff, "Running for %ld sec  In Shelf: %d\r\n" 
				"Twin Run for: %ld Twin in: %d\r\n"
				"Fresh: %d\r\n"
				,g_pRTC->GetAbsoluteSecond() ,m_inShelf
				,m_twinRun + m_fresh ,m_twinInShelf
				,m_fresh
				);
			pComm->Send(buff);
			sprintf(buff, "Status: %d (%s)" 
				" Loop: %d (%s)"
				" Align: %d (%s)\r\n"
				"Twin Status: %d (%s)"
				" Loop: %d (%s)"
				" Align: %d (%s)\r\n"
				, CCardStatus::GetCardStatus(), CCardStatus::GetName(CCardStatus::s_pCardStatus->GetCardStatus())
				, CCardLoop::s_pCardLoop->GetCardLoop(), CCardLoop::GetName(CCardLoop::s_pCardLoop->GetCardLoop())
				, CCardAlign::s_pCardAlign->GetCardAlign(), CCardAlign::GetName(CCardAlign::s_pCardAlign->GetCardAlign())
				, CCardStatus::s_pCardStatus->GetTwinStatus(), CCardStatus::GetName(CCardStatus::s_pCardStatus->GetTwinStatus())
				, CCardLoop::s_pCardLoop->GetTwinLoop(), CCardLoop::GetName(CCardLoop::s_pCardLoop->GetTwinLoop())
				, CCardAlign::s_pCardAlign->GetTwinAlign(), CCardAlign::GetName(CCardAlign::s_pCardAlign->GetTwinAlign())
				);
			pComm->Send(buff);
			return;
		case 2:
			ForceShelfLife(2000);
			if (m_twinInShelf < 2000) m_twinInShelf = 2000;
			pComm->Send("Force IOCs out of warmup\r\n");
			SendShelfLife();
			return;
		case 3:
			CNewCode::s_pNewCode->NewCodeHack(pComm);
			return;
		case 4:
			CCardLoop::s_pCardLoop->CardLoopHack(pComm);
			return;
		case 5: 
			CCardAlign::s_pCardAlign->CardAlignHack(pComm);
			return;
		case 6:
			CCardStatus::s_pCardStatus->CardStatusHack(pComm);
			return;
		}
	}
	const char *pHelp = "Syntax: <option>\r\n" "\t1: List card life status\r\n"
		"\t2: Skip warmup process\r\n"
		"\t3: Print New Code Detection\r\n"
		"\t4: Print CardLoop\r\n"
		"\t5: Print CardAlign\r\n"
		"\t6: Print CardStatus\r\n"
		;
	pComm->Send(pHelp);
}

void CTwinProxy::TwinOffline()
{
	m_twinRun = 0L;
	CCardStatus::s_pCardStatus->SetTwinStatus(CCardStatus::CARD_WARMUP);
	CCardLoop::s_pCardLoop->SetTwinLoop(CCardLoop::CARD_FREQ_WARMUP);
}

int CTwinProxy::GetShelfLife()
{
	return m_inShelf;	
}

void CTwinProxy::ForceShelfLife(int life)
{
	if (m_inShelf < life) {
		m_inShelf = life;
	}
}

int CTwinProxy::PostBuffer(const char *ptr, int size)
{
	return m_pIocLink->PostFrame(ptr, size, m_port);

}

int CTwinProxy::GetFresh()
{
	return m_fresh;
}
