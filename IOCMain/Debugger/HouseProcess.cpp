// HouseProcess.cpp: implementation of the CHouseProcess class.
//
//////////////////////////////////////////////////////////////////////


#include "HouseProcess.h"
#include "string.h"
#include "AllProcess.h"
#include "AdcConverted.h"
#include "EventId.h"
#include "Fpga.h"
#include "CardLoop.h"
#include "TwinProxy.h"  
#include "stdio.h"
#include "comm.h"


#ifndef TRACE
#define TRACE TRACE_HOUSE
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHouseProcess *CHouseProcess::s_pHouseProcess = NULL;

// COMM_RTE_KEY_CURRENT_ADC_VALUE
int32 CHouseProcess::m_reportedAdc[NUM_REPORTED_ADC_CHAN];

CHouseProcess::CHouseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry)
: CBaseProcess(name, priority, stack_size, entry)
{
	memset(m_voltOOR, 0, sizeof(m_voltOOR));

	m_pllUnlockChan = -1;
	if (s_pHouseProcess == NULL) 
		s_pHouseProcess = this;
	
	m_pAdc = new CAdcConverted(0x7f);
	m_timeout = 0;
	m_maxTimeout = 0;
	m_failed = 0;
	m_pllUnlockCnt = 0;

	m_voltCheckDelay = 30;

	m_skipTimeoutChecking = 60;

	m_adcPostCycle = 0;
	m_rubUnlock = 0;
}

CHouseProcess::~CHouseProcess()
{

}

void CHouseProcess::Entry()
{
	s_pHouseProcess->EventEntry((TICK_1SEC / HOUSE_TICK_PER_SECOND) + 5);
}

void CHouseProcess::TickHouseProcessPerSecond()
{
	s_pHouseProcess->SignalEvent(EVT_ONE_SECOND_ADC);	
}

void CHouseProcess::Init()
{
//	m_pAdc->SetCardRevisionId(CAllProcess::GetHardwareRevision() & 0xf);
	m_pAdc->Init();
}

void CHouseProcess::EventDispatch(uint16 event)
{
	if (event & EVT_ONE_SECOND_ADC) {
		CAllProcess::g_pAllProcess->TaskPerSecond();
		m_pAdc->StartAdc();
		m_timeout = 0;

		CheckPllUnlock();
		CheckOcxo();
		if (m_voltCheckDelay == 0) {
			CheckVoltage();
		} else {
			m_voltCheckDelay--;
		}

		if (m_skipTimeoutChecking) {
			m_skipTimeoutChecking--;
		}

		if (++m_adcPostCycle >= ADC_POST_CYCLE) {
			m_adcPostCycle = 0;
			PostAdcValue();
		}

		if (CFpga::s_pFpga->IsRubidium() == 1) {
			CheckRubLock();
		}
	}
}

void CHouseProcess::EventTimeOut()
{
	if (m_skipTimeoutChecking) {
		m_skipTimeoutChecking--;
		return;
	}

	if (m_timeout++ > 60*HOUSE_TICK_PER_SECOND) {
		REPORT_EVENT(COMM_RTE_EVENT_INTERRUPT_LOSS, 1);
		CAllProcess::g_pAllProcess->CardFail(CAllProcess::CARD_FAIL_INDEX_FPGA_INTERRUPT_LOSS);
		TRACE("FPGA intr lost for %d\r\n", m_timeout);
		if (m_timeout > 1000) {
			m_timeout = 0;
		}
		m_failed = 1;
	}
	if (m_timeout > m_maxTimeout) {
		m_maxTimeout = m_timeout;
	}
}

void CHouseProcess::PostAdcValue(void)
{
	int chan;
	double val;
	for (chan = 0; chan < NUM_REPORTED_ADC_CHAN; chan++) {
		val = m_pAdc->ReadConverted(chan, CAdcConverted::OPTION_ISTATE_READ);
		m_reportedAdc[chan] = (int32)val;
	}
}

void CHouseProcess::CheckPllUnlock()
{
	int loop;
	int pllUnlock;

	pllUnlock = CFpga::s_pFpga->GetPllUnlock();
	if (!m_failed) {
		if (pllUnlock >= 0) {
			loop = CCardLoop::s_pCardLoop->GetCardLoop();
			switch (loop) {
			case CCardLoop::CARD_FREQ_WARMUP:
			case CCardLoop::CARD_FREQ_OFFLINE:
			case CCardLoop::CARD_FREQ_FAIL:
				return;
			}
			if (m_pllUnlockCnt++ > 30000) {
				m_pllUnlockCnt = 29000;
			}
			// zmiao 2/17/2009: Talked with Gary J. Make it an alarm without triggering card failure.
			CFpga::s_pFpga->UnlockPll();
			// zmiao 11/19/2008: For experiment. enable it again. This section should be cleaned up later
			// after Bob H is sure about what to do. 
			if (m_pllUnlockCnt < 5) { // 8/24/2006 zmiao: Make it more forgiving according Bob H and Gary J.
				// 11/18/2008 zmiao: For TP5000 1.1 release. Virtually ignore the PLL unlock according to Bob H.
				// CFpga::s_pFpga->UnlockPll();
				return;
			}
			// m_failed = 1;
			if (m_pllUnlockCnt == 5) { // ??? change it later
				if (m_pllUnlockChan != pllUnlock) {
					if (m_pllUnlockChan >= 0) {
						REPORT_EVENT(COMM_RTE_EVENT_PLL_UNLOCK, 0, m_pllUnlockChan);
					}
					REPORT_EVENT(COMM_RTE_EVENT_PLL_UNLOCK, 1, pllUnlock);
					m_pllUnlockChan = pllUnlock;
				}
			}
			// CAllProcess::g_pAllProcess->CardFail();
		} else if (CFpga::s_pFpga->GetDcmUnlock()) {
			loop = CCardLoop::s_pCardLoop->GetCardLoop();
			switch (loop) {
			case CCardLoop::CARD_FREQ_WARMUP:
			case CCardLoop::CARD_FREQ_OFFLINE:
			case CCardLoop::CARD_FREQ_FAIL:
				return;
			}
			m_failed = 1;
			REPORT_EVENT(COMM_RTE_EVENT_DCM_UNLOCK, 1);
			CAllProcess::g_pAllProcess->CardFail(CAllProcess::CARD_FAIL_INDEX_UNCLASSIFIED);
		}
		if (pllUnlock < 0) {
			// zmiao 11/18/2008: if it doesn't cause failure, it could be cleared.
			if (m_pllUnlockChan >= 0) {
				REPORT_EVENT(COMM_RTE_EVENT_PLL_UNLOCK, 0, m_pllUnlockChan);
				m_pllUnlockChan = -1;
			}
			m_pllUnlockCnt = 0;
		}
	}
}

void CHouseProcess::CheckOcxo()
{
	int loop;
	if (!m_failed) {
		if (m_pAdc->IsWithinRange(CAdc::ADC_CHAN_OCXO_CURRENT) < 0) {
			loop = CCardLoop::s_pCardLoop->GetCardLoop();
			switch (loop) {
			case CCardLoop::CARD_FREQ_WARMUP:
			case CCardLoop::CARD_FREQ_OFFLINE:
			case CCardLoop::CARD_FREQ_FAIL:
				break;
			default:
				REPORT_EVENT(COMM_RTE_EVENT_OCXO_OUT_OF_RANGE, 1);
				m_failed = 1;
				CAllProcess::g_pAllProcess->CardFail(CAllProcess::CARD_FAIL_INDEX_OCXO_CURRENT_OOR);
				break;
			}
		}
	}

}

void CHouseProcess::CheckRubLock(void)
{
	int loop;
	if (!m_failed) {
		loop = CCardLoop::s_pCardLoop->GetCardLoop();
		switch (loop) {
		case CCardLoop::CARD_FREQ_WARMUP:
		case CCardLoop::CARD_FREQ_OFFLINE:
		case CCardLoop::CARD_FREQ_FAIL:
			return;
		}

		if (CFpga::s_pFpga->IsRubLocked()) {
			if (m_rubUnlock) {
				m_rubUnlock--;
			}
			return;
		}
		m_rubUnlock += 100;
		if (m_rubUnlock >= 500) {
			REPORT_EVENT(COMM_RTE_EVENT_RUBIDIUM_UNLOCK, 1);
			m_failed = 1;
			CAllProcess::g_pAllProcess->CardFail(CAllProcess::CARD_FAIL_INDEX_RUBIDIUM_UNLOCK);
			return;
		}
	}
}

void CHouseProcess::CheckVoltage()
{
	int loop;
	int aid1;
	int failCode = CAllProcess::CARD_FAIL_INDEX_ADC_1V8;
	loop = CCardLoop::s_pCardLoop->GetCardLoop();
	if (loop == CCardLoop::CARD_FREQ_WARMUP) {
		int inSlot;
		inSlot = CTwinProxy::s_pTwinProxy->GetShelfLife();
		if (inSlot < 100) {
			return;
		}
	}

	aid1 = 1;
	if ((m_pAdc->IsWithinRange(CAdc::ADC_CHAN_10M_EFC) < 0) && (m_voltOOR[aid1] != 1)) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		m_voltOOR[aid1] = 1;
	} else if (m_voltOOR[aid1] != 0) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 0, aid1);
		m_voltOOR[aid1] = 0;
	}
		
	aid1 = 2;
	if ((m_pAdc->IsWithinRange(CAdc::ADC_CHAN_20M_EFC) < 0) && (m_voltOOR[aid1] != 1)) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		m_voltOOR[aid1] = 1;
	} else if (m_voltOOR[aid1] != 0) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 0, aid1);
		m_voltOOR[aid1] = 0;
	}
		
	aid1 = 3;
	if ((m_pAdc->IsWithinRange(CAdc::ADC_CHAN_25M_EFC) < 0) && (m_voltOOR[aid1] != 1)) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		m_voltOOR[aid1] = 1;
	} else if (m_voltOOR[aid1] != 0) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 0, aid1);
		m_voltOOR[aid1] = 0;
	}
		
	aid1 = 4;
	if ((m_pAdc->IsWithinRange(CAdc::ADC_CHAN_24M5_EFC) < 0) && (m_voltOOR[aid1] != 1)) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		m_voltOOR[aid1] = 1;
	} else if (m_voltOOR[aid1] != 0){
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 0, aid1);
		m_voltOOR[aid1] = 0;
	} 
		
	aid1 = 5;
	if ((m_pAdc->IsWithinRange(CAdc::ADC_CHAN_24M7_EFC) < 0) && (m_voltOOR[aid1] != 1)) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		m_voltOOR[aid1] = 1;
	} else if (m_voltOOR[aid1] != 0) {
		REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 0, aid1);
		m_voltOOR[aid1] = 0;
	}
		
	if (!m_failed) {
		int adcStat = 1;
		aid1 = 0;
		if (m_pAdc->IsWithinRange(CAdc::ADC_CHAN_1V8) < 0) {
			adcStat -= 100;
			failCode = CAllProcess::CARD_FAIL_INDEX_ADC_1V8;
			REPORT_EVENT(COMM_RTE_EVENT_VOLT_OUT_OF_RANGE, 1, aid1);
		} 
		
		if (adcStat < 0) {		
			switch (loop) {
			case CCardLoop::CARD_FREQ_OFFLINE:
			case CCardLoop::CARD_FREQ_FAIL:
				return;
			}

			m_failed = 1;
			CAllProcess::g_pAllProcess->CardFail(failCode);
		}
	}

}

void CHouseProcess::HackHouseProcess(unsigned long *param, Ccomm *pComm)
{
	int8 ok = 0;
	char buff[200];

	if (*param == 1) {
		switch (param[1]) {
		case 0:
			sprintf(buff, "Current Timeout Cnt: %d Max: %d\r\n", m_timeout, m_maxTimeout);
			pComm->Send(buff);
			sprintf(buff, "PLL unlock Cnt: %d\r\n", m_pllUnlockCnt);
			pComm->Send(buff);
			return;
		case 1: 
			m_maxTimeout = 0;
			pComm->Send("Max cnt cleared\r\n");
			return;
		}
	}

	if (!ok) {
		pComm->Send("option\r\n"
			"\t0: Print timeout cnts\r\n"
			"\t1: Clear max timeout\r\n");
	}
}
