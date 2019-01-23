// DtiOutput.cpp: implementation of the CDtiOutput class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutput.cpp 1.5 2009/05/01 09:44:50PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 */

#include "DtiOutput.h"
#include "machine.h"
#include "CardLoop.h"
#include "OutputProcess.h"
#include "Debug.h"
#include "comm.h" 
#include "string.h"
#include "stdio.h"
#include "Fpga.h"
#include "IstateDti.h"
#include "CardStatus.h"
#include "IstateProcess.h"
#include "DtiOutputPm.h"
#include "TwinProxy.h"
#include "DtiOutputTrace.h"
#include "RTC.h"
#include "DtiDelayStable.h"
#include "AllProcess.h"
#include "PTPLink.h"

#define TRACE  TRACE_OUTPUT

uint8 outputDummy[512];

#define CABLE_ADV_THRESHOLD 32
#define PHASE_ERR_THRESHOLD 128

#define DEVICE_TYPE	((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x87))
#define THIS_DEVICE_TYPE ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x8c))

#define LED			((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x91))
#define CABLE_ADVANCE ((int32 volatile *)(OUTPUT_FPGA_BASE + 0x92))

#define PORT_ACTION_CLEAR_JITTER	4
#define PORT_ACTION_CLEAR_CRC_COUNTER 2
#define PORT_ACTION_ENABLE_TEST 1
#define PORT_CONTROL ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x96))

#define SERVER_STATUS	((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x97))
#define CABLE_DELAY ((uint32 volatile *)(OUTPUT_FPGA_BASE + 0x98))
#define CLIENT_DEVICE_TYPE ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x9c))
#define CLIENT_VERSION ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x9e))
#define CLIENT_PHASE_ERROR ((uint32 volatile *)(OUTPUT_FPGA_BASE + 0xa0))
#define CRC_ERROR_COUNT  ((uint16 volatile *)(OUTPUT_FPGA_BASE + 0xa4))
#define JITTER		((uint16 volatile *)(OUTPUT_FPGA_BASE + 0xa8))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define INVALID_PHASE_ERROR (0x7fffffffL)

CDtiOutput *CDtiOutput::m_dtiOutput[NUM_OUTPUT_CHAN];
uint8 CDtiOutput::m_deviceType;
uint8 CDtiOutput::m_thisDeviceType = 0xf0;
uint8 CDtiOutput::m_serverMode; // Port N Server status flag @0x95 (bit4 .. 0)
uint16 CDtiOutput::m_serviceCnt;
uint32 CDtiOutput::m_outputPortStatusAnd; // Combined port status AND
uint32 CDtiOutput::m_outputPortStatusOr; // Combined port status OR
CDtiOutputPm  *CDtiOutput::m_pOutputPm = NULL;
CDtiOutput::SOutputControl CDtiOutput::m_twinControl;

int CDtiOutput::m_activeCnt;
int CDtiOutput::m_inActiveCnt;
int CDtiOutput::m_cardStatus;
int CDtiOutput::m_isActive;
int CDtiOutput::m_fpgaActive;
uint32 CDtiOutput::m_pmEdge;
int32 *CDtiOutput::m_pReportTCounters;
uint32 *CDtiOutput::m_pReportNHDuration;
CPTPLink *CDtiOutput::m_pIOCLink;
uint8 CDtiOutput::m_controlUpdated;

CDtiOutput::CDtiOutput(int chan): m_eventStatus(chan), m_phaseErrorDetect(chan)
{
	m_clientDeviceType = 0;
	m_clientPhaseError = 0L;
	m_clientStatusFlag = 0;
	m_clientVersion = 0;
	m_crcErrorCount = 0;
	m_jitter = 0;
	m_pDelayStable = NULL;

	m_clientLockCnt = 0;
	if (m_pOutputPm == NULL) {
		// Create only one instance
		m_pOutputPm = new CDtiOutputPm();
	}
	m_chan = chan;
	m_losBit = 1L << (16 + chan);
	m_lofBit = 1L << chan;
	m_valid = -1;
	m_led = 3;
	m_control = 0;
	m_enable = -1;
	m_testMode = 0;
	m_serverStatusFlag = 0;
	m_cableAdvance = 0L;
	m_cableAdvancePrinted = 0L;
	m_cableDelay = 0L;
	m_oldCableDelay = 0L;
	m_init = 0x1000;
	m_cableAdvanceMode = CIstateDti::AUTO_MODE;
	m_cableAdvanceManualSet = 0L;
	SetNLed();

	*OUTPUT_PORT_SELECT = m_chan;
	*SERVER_STATUS = m_serverStatusFlag | m_serverMode;
	m_pIstatePhaseError = NULL;
}

void CDtiOutput::Create()
{
	m_fpgaActive = 0;
	m_activeCnt = 0;
	m_inActiveCnt = 0;
	m_outputPortStatusAnd = 0L;
	m_outputPortStatusOr = 0L;
	m_deviceType = 0;
	m_serverMode = SERVER_WARMUP;
	m_serviceCnt = 0;
	for (int i = 0; i < NUM_OUTPUT_CHAN; i++) {
		m_dtiOutput[i] = new CDtiOutput(i);
	}
	memset(&m_twinControl, 0, sizeof(m_twinControl));
	m_cardStatus = CCardStatus::CARD_WARMUP;
	m_isActive = 0;

	m_pReportTCounters = (int32 *)CIstateDti::de_outputClientTransitionCount.pVariable;
	m_pReportNHDuration = (uint32 *)CIstateDti::de_outputClientNormalHoldoverTime.pVariable;
}

void CDtiOutput::Init()
{        
	int chan;

	m_pIOCLink = CAllProcess::g_pAllProcess->GetIocLink();

	if (m_pOutputPm) {
		m_pOutputPm->Init();
	}
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		m_dtiOutput[chan]->m_pDelayStable = CDtiDelayStable::s_pDtiDelayStable[chan];
		m_dtiOutput[chan]->m_pIstatePhaseError = (int32 *)(CIstateDti::de_outputPhaseError.pVariable) + chan;
	}
	SetServerHop(0);
}

CDtiOutput::~CDtiOutput()
{

}

void CDtiOutput::SetClockType(int type)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	m_deviceType = (m_deviceType & 0xf8) | (type & 7);
	*DEVICE_TYPE = m_deviceType;
	set_exr(tmp_exr);
}

void CDtiOutput::SetMyClockType(int type)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	m_thisDeviceType = (m_thisDeviceType & 0xf0) | (type & 0xf);
	*THIS_DEVICE_TYPE = m_thisDeviceType;
	set_exr(tmp_exr);
}

void CDtiOutput::SetServerHop(int hop)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	m_deviceType = (m_deviceType & 0xe7) | ((hop & 3) << 3);
	*DEVICE_TYPE = m_deviceType;
	set_exr(tmp_exr);
}

// On Jan 18, 2006, George Z said: Use 000 for subtending mode. Hop count should tell you it's subtending
// "from a network" is for Span input mode.
void CDtiOutput::SetExternalTimeSource(int source)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	m_deviceType = (m_deviceType & 0x1f) | ((source & 7) << 5);
	*DEVICE_TYPE = m_deviceType;

	set_exr(tmp_exr);
}


void CDtiOutput::SetNLed(int8 yellow, int8 green)
{
	uint8 tmp_exr;

	if (!m_valid) return;

	switch (yellow) {
	case 0:
		m_led &= 0xfd;
		break;
	case 1:
		m_led |= 2;
		break;
	}

	switch (green) {
	case 0:
		m_led &= 0xfe;
		break;
	case 1:
		m_led |= 1;
		break;
	}

	tmp_exr = get_exr();
	set_exr(7);
	*OUTPUT_PORT_SELECT = m_chan;
	*LED = m_led;
	set_exr(tmp_exr);
}

// read server status from state machine.
void CDtiOutput::GetServerMode()
{
	int state;

	state = CCardLoop::s_pCardLoop->GetCardLoop();

	switch (state) {
	case CCardLoop::CARD_FREQ_FREERUN:
		m_serverMode = SERVER_FREERUN;
		break;
	case CCardLoop::CARD_FREQ_FAST_TRACK:
		m_serverMode = SERVER_FAST_TRACK;
		break;
	case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		m_serverMode = SERVER_NORMAL_TRACK;
		break;
	case CCardLoop::CARD_FREQ_HOLDOVER:
	case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
	case CCardLoop::CARD_FREQ_BRIDGE:
		m_serverMode = SERVER_HOLDOVER;
		break;
	default:
	case CCardLoop::CARD_FREQ_WARMUP:
		m_serverMode = SERVER_WARMUP;
		break;

	}
}

void CDtiOutput::DoLoopChange()
{
	int chan;

	GetServerMode();
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		m_dtiOutput[chan]->SetNServerStatusFlag();
	}
}

void CDtiOutput::DoStatusChange()
{
	int cardStatus;

	cardStatus = CCardStatus::GetCardStatus();
	if (m_cardStatus != cardStatus) {
		m_cardStatus = cardStatus;
		m_serviceCnt = 0;
	}
	m_isActive = IS_ACTIVE;
}

void CDtiOutput::SetNServerStatusFlag()
{
	uint8 tmp_exr;
	
	if (!m_valid) return;

	tmp_exr = get_exr();
	set_exr(7);
	*OUTPUT_PORT_SELECT = m_chan;
	*SERVER_STATUS = m_serverStatusFlag | m_serverMode;
	set_exr(tmp_exr);
}

void CDtiOutput::PowerOn()
{
	int chan;
	DoLoopChange();
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		m_dtiOutput[chan]->SetNLed(0, 0);
	}
}

void CDtiOutput::ReadRegisterIsr(int active)
{
	int chan;
	CDtiOutput *pOutput;

	if (m_isActive) {
		CFpga::s_pFpga->GetOutputStatus(m_outputPortStatusAnd, m_outputPortStatusOr);
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			pOutput = m_dtiOutput[chan];
			*OUTPUT_PORT_SELECT = chan;
			pOutput->m_cableDelay = *CABLE_DELAY;
			pOutput->m_clientDeviceType = *CLIENT_DEVICE_TYPE;
			pOutput->m_clientStatusFlag = *CLIENT_STATUS_FLAG;
			pOutput->m_clientVersion = *CLIENT_VERSION;
			pOutput->m_clientPhaseError = *CLIENT_PHASE_ERROR;
			pOutput->m_crcErrorCount = *CRC_ERROR_COUNT;
			pOutput->m_jitter = *JITTER;
			*PORT_CONTROL = (pOutput->m_control | PORT_ACTION_CLEAR_CRC_COUNTER | PORT_ACTION_CLEAR_JITTER);
			*PORT_CONTROL = pOutput->m_control;
		}
		m_fpgaActive = active;
	}
	COutputProcess::s_pOutputProcess->ToServiceOutputIsr();
}

void CDtiOutput::ServiceOutput()
{
	int chan;
	int reportPm = 0;

	if (IS_ACTIVE == HOT_ACTIVE) {
		if (m_activeCnt < 10000) {
			m_activeCnt++;
		} else {
			m_activeCnt = 1001;
		}
		m_inActiveCnt = 0;
		reportPm |= 1;
	} else {
		m_activeCnt = 0;
		m_inActiveCnt++;
		if (m_inActiveCnt >= 10000) {
			m_inActiveCnt = 1000;
		} else if (m_inActiveCnt < 30) {
			InactiveClearance(m_inActiveCnt);

		}


	}

	if (++m_serviceCnt >= 10000) {
		m_serviceCnt = 1000;
	}

	if (m_activeCnt == 1) {
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			m_dtiOutput[chan]->m_eventStatus.RefreshLoopStatus();
		}
		SendOutputControl();
	}
	if (m_activeCnt > 0) {
		// Service for active RTE
		if (m_activeCnt > 1) {
			for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
				m_dtiOutput[chan]->ServiceChanActive();		
			}
		}

		if (m_activeCnt > 3) {
			if (m_fpgaActive) {
				m_pOutputPm->SetActive(1);
			}
		}

		if (((m_activeCnt & 7) == 2) || (m_controlUpdated)) {
			SendOutputControl();
		}
		CDtiOutputTrace::SendTrace();

		CFpga::s_pFpga->FillTCounter(m_pReportTCounters);
		CFpga::s_pFpga->FillNHDuration(m_pReportNHDuration);
	} else if (IS_STANDBY == HOT_STANDBY) {
		reportPm |= 2;
		if (m_inActiveCnt == 3) {
			ClearEvents();
			for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
				m_dtiOutput[chan]->m_init = 0x1000;
				m_dtiOutput[chan]->m_pDelayStable->Reset();
				*m_dtiOutput[chan]->m_pIstatePhaseError = INVALID_PHASE_ERROR;
				m_dtiOutput[chan]->m_phaseErrorDetect.ResetPhaseError();
			}
			//CLed::s_pLed->ClearLed(CLed::LED_OUTPUT_GREEN);

		}
		CDtiOutputTrace::SendTrace();
	} else if (IS_STANDBY == COLD_STANDBY) {
		if (m_inActiveCnt == 3) {
			ClearEvents();
		}
	} else if (IS_ACTIVE == COLD_ACTIVE) {
		m_inActiveCnt = 0;
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			m_dtiOutput[chan]->ServiceChanWarmup();
		}
		CDtiOutputTrace::SendTrace();
	} else {
		// clear Led on the front panel
		//CLed::s_pLed->ClearLed(CLed::LED_OUTPUT_GREEN);
		//CLed::s_pLed->ClearLed(CLed::LED_OUTPUT_YELLOW);
		
		if (m_inActiveCnt == 3) {
			ClearEvents();
		}
	}
	
	if (reportPm) {
		int ret;
		uint32 now;
		uint32 diff;
		int residual;

		ret = m_pOutputPm->TickPerSecond();
		if (ret) {
			now = CFpga::s_pFpga->GetGpsSec(); //g_pRTC->GetComputerSeconds();
			if (m_activeCnt > 0) {
				m_pmEdge = now;
			} else {
				diff = now + 10L - m_twinControl.pmEdge;
				residual = (int)(diff % 10);
				if ((residual != 0) && (diff < 30)) {
					m_pOutputPm->ResetPhase(residual);
					TRACE("Standy phase reset to %d\r\n", residual);
				}
			}
		}
	}
}

void CDtiOutput::ServiceChanActive()
{
	int yellow = -1, green = -1;
	int32 cableAdvance = m_cableAdvance;
	uint8 status = 0;
	int32 phaseError;
	uint8 tmp_exr;
	int32 delayDiff;

	if (IsDisabled()) {
		m_serverStatusFlag = 0;
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		*SERVER_STATUS = m_serverStatusFlag | m_serverMode;
		*m_pIstatePhaseError = INVALID_PHASE_ERROR;
		set_exr(tmp_exr);
		ReportStatus(); // report events
		return;
	}
	
	if (m_outputPortStatusAnd & m_losBit) { // loss of signal
		if (m_init < 0x3330) {
			m_init += 0x10;
		} else {
			m_init &= 0x1fff;
		}
		m_pDelayStable->Reset();
	}                       
	
	if (IsLos()) {
		m_serverStatusFlag = 0;
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		*SERVER_STATUS = m_serverStatusFlag | m_serverMode;
		*m_pIstatePhaseError = INVALID_PHASE_ERROR;
		set_exr(tmp_exr);
		ReportStatus(); // report events
		return;
	}

	ReportStatus(); // report events

#if 0 // 1/31/2006: Don't do dumb wait. Find out the cable delay change. If it's stable. use it.
	if (m_init) { // to be inited. Wait 2 second for cable delay calculation in FPGA done.
		if ((m_init & 0x3) != 2) {
			m_init++;
			return;
		}
	}
#endif

	// Cable delay ==> cable advance
	if (m_cableAdvanceMode == CIstateDti::AUTO_MODE) {
		status = m_serverStatusFlag & CABLE_ADVANCE_STABLE;
		if (AutoMode(status, cableAdvance) != 1) {
			status = m_serverStatusFlag & CABLE_ADVANCE_STABLE; // Keep current status
		}
	} else if (m_cableAdvanceMode == CIstateDti::MANUAL_MODE) { // it's manual mode
		if (m_cableAdvanceManualSet == 0xffffff) {
			// Freeze current value
			status = CABLE_ADVANCE_STABLE;
		} else if (m_serverStatusFlag & CABLE_ADVANCE_STABLE) {
			status = CABLE_ADVANCE_STABLE;
			if (cableAdvance > m_cableAdvanceManualSet) {
				cableAdvance--;
			} else if (cableAdvance < m_cableAdvanceManualSet) {
				cableAdvance++;
			}
		} else {
			status = CABLE_ADVANCE_STABLE;
			cableAdvance = m_cableAdvanceManualSet;
			TRACE("Set CableAdvance to manual set: 0x%lX", cableAdvance);
		}
		m_init = 0; // don't re-init it. Auto mode will continue with old manual setting.
	} else { // Unknown mode. future mode. Make it automatic for me. 
		status = m_serverStatusFlag & CABLE_ADVANCE_STABLE;
		if (AutoMode(status, cableAdvance) != 1) {
			status = m_serverStatusFlag & CABLE_ADVANCE_STABLE; // keep current status;
		}
	}


	// Check phase error
	phaseError = m_clientPhaseError;
	if (phaseError & 0x80000000) {
		phaseError |= 0xff000000;
		m_clientPhaseError = phaseError;
	}

#if 1
	if (m_phaseErrorDetect.FeedPhaseError(phaseError) == 1) {
		status |= CLIENT_PERFORMANCE_STABLE;
		if (!(m_serverStatusFlag & CLIENT_PERFORMANCE_STABLE)) {
			TRACE("Out[%d]: PhaseError: %ld\r\n", m_chan, phaseError);	
		}
	} else {
		TRACE("Out[%d]: PhaseError: %ld\r\n", m_chan, phaseError);
	}
#else
	if ((phaseError > -PHASE_ERR_THRESHOLD) && (phaseError < PHASE_ERR_THRESHOLD)) {
		status |= CLIENT_PERFORMANCE_STABLE;
		if (!(m_serverStatusFlag & CLIENT_PERFORMANCE_STABLE)) {
			TRACE("Out[%d]: PhaseError: %ld\r\n", m_chan, phaseError);	
		}
	} else {
		TRACE("Out[%d]: PhaseError: %ld\r\n", m_chan, phaseError);
	}
#endif
	
	if (m_serverMode & SERVER_WARMUP) { // in warmup
		cableAdvance = 0L;
		status = 0;
	}

	m_serverStatusFlag = status;
	
	// Set cable advance and server status
	tmp_exr = get_exr();
	set_exr(7);
	*OUTPUT_PORT_SELECT = m_chan;
	*CABLE_ADVANCE = cableAdvance;
	*SERVER_STATUS = m_serverStatusFlag | m_serverMode;
	set_exr(tmp_exr);

	{
		int32 advanceDiff;
		int32 printDiff;
		advanceDiff = m_cableAdvance - cableAdvance;
		printDiff = m_cableAdvancePrinted - cableAdvance;
		if (advanceDiff) {
			if ((advanceDiff >= 2) || (advanceDiff <= -2)) {
				TRACE("OUT[%d] CableAdvance=0x%lX\r\n", m_chan, cableAdvance);
			} else if ((printDiff >= 2) || (printDiff <= -2)) {
				TRACE("OUT[%d] CableAdvance=0x%lX\r\n", m_chan, cableAdvance);
				m_cableAdvancePrinted = cableAdvance;
			}
			m_cableAdvance = cableAdvance;
			SET_MY_ISTATE(COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE, m_chan, 0, m_cableAdvance);
			m_controlUpdated = 1;
		} else if (m_activeCnt == 2) {
			TRACE("Reset Active cable advance in Output[%d]\r\n", m_chan);
			SET_MY_ISTATE(COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE, m_chan, 0, m_cableAdvance);
		}
	}

	if (m_oldCableDelay != m_cableDelay) {
		delayDiff = m_cableDelay - m_oldCableDelay;
		if ((delayDiff >= 2) || (delayDiff <= -2)) {
			TRACE("Out[%d] Cable Delay: 0x%08lX\r\n", m_chan, m_cableDelay);
			m_oldCableDelay = m_cableDelay;
			if (m_activeCnt > 0) {
				SET_MY_ISTATE(COMM_RTE_KEY_OUTPUT_CABLE_DELAY, m_chan, 0, m_cableDelay);
			}
		}
	} else if (m_activeCnt == 3) {
		SET_MY_ISTATE(COMM_RTE_KEY_OUTPUT_CABLE_DELAY, m_chan, 0, m_cableDelay);
	}

	if (m_clientStatusFlag & CLIENT_NORMAL_TRACK) {
		m_clientLockCnt++;
		if (m_clientLockCnt >= 10000) {
			m_clientLockCnt = 1000;
		}
	} else {
		m_clientLockCnt = 0;
	}

	if (m_clientLockCnt > 10) {
		m_pOutputPm->SendPhase(m_chan, m_clientPhaseError);
		m_pOutputPm->SendFer(m_chan, m_crcErrorCount);
		m_pOutputPm->SendJitter(m_chan, m_jitter);
		phaseError = m_clientPhaseError;
	} else {
		phaseError = INVALID_PHASE_ERROR;
	}

	tmp_exr = get_exr();
	set_exr(7);
	*m_pIstatePhaseError = phaseError;
	set_exr(tmp_exr);
}

// When disable -> enable or invalid -> valid, reset cable advance.
// when los for 2 seconds, reset cable advance.
// when change mode from manual to auto, reset cable advance.
// return 1: status updated.
// return 0: status not updated.
int CDtiOutput::AutoMode(uint8 &status, int32 &cableAdvance)
{
	uint32 cableDelay;
	int32 diff;

	cableDelay = m_cableDelay;
	if (cableDelay & 0x80000000) {
		cableDelay |= 0xff000000;
	}
	diff = m_pDelayStable->Feed(cableDelay);
	if (m_init || diff >= 0x7fffffffL) {
		FORCE_TRACE("OUT[%d] CableDelay Data: %ld(0x%lX) --> Diff=%ld\r\n"
			,m_chan , cableDelay, cableDelay, diff);
		if (diff >= 0x7fffffffL) {
			return 0;
		}
	} else if (diff >= 1000) {
		m_init++; 
	}

	if (m_init) {
		if (diff < 10) {
			TRACE("Out[%d] Cable Advance Set to: 0x%08lX\r\n", m_chan, cableAdvance);
			cableAdvance = cableDelay;
			status = CABLE_ADVANCE_STABLE;
			m_init = 0;
			return 1;
		} else {
			return 0;
		}
	}

	if (status & CABLE_ADVANCE_STABLE) {
		if (cableDelay > (m_cableAdvance + 1)) {
			cableAdvance = m_cableAdvance + 1;
		} else if (cableDelay < (m_cableAdvance - 1)) {
			cableAdvance = m_cableAdvance - 1;
		} else {
			cableAdvance = m_cableAdvance;
		}
	} else { // should never be here
		cableAdvance = cableDelay;
		status = CABLE_ADVANCE_STABLE;
	}

#if 0 // old code 6/20/2006
	if ((m_serverMode & SERVER_FAST_TRACK) || (m_init /*>= 2*/)) { // Fast mode
		if (m_serverMode & SERVER_FAST_TRACK) {
			cableAdvance = cableDelay;
			status = CABLE_ADVANCE_STABLE;
			m_init = 0;
			return 1;
		}
		if (m_init) {
			if (diff < 10) {
				TRACE("Out[%d] Cable Advance Set to: 0x%08lX\r\n", m_chan, cableAdvance);
				cableAdvance = cableDelay;
				status = CABLE_ADVANCE_STABLE;
				m_init = 0;
				return 1;
			} else if (diff >= 1000) {
				//m_init++;
				status = 0;
				return 1;
			} else if (diff >= 10) {
				m_init++;
				return 0;
			} 
			return 0;
		}
	} else if (m_serverMode & (SERVER_FREERUN | SERVER_NORMAL_TRACK | SERVER_HOLDOVER )) { // freerun normal and holdover
		// step move in those case
		if (cableDelay > (m_cableAdvance + 1)) {
			cableAdvance = m_cableAdvance + 1;
#if 0		// The eval firmware does do the following. But Bob said "Don't do that" 11/1/2005
			diff = cableDelay - cableAdvance;
			if (diff < CABLE_ADV_THRESHOLD) {
				status = CABLE_ADVANCE_STABLE;
			}
#else
			status = CABLE_ADVANCE_STABLE;
#endif
		} else if (cableDelay < (m_cableAdvance - 1)) {
			cableAdvance = m_cableAdvance - 1;
#if	0		// The eval firmware does do the following. But Bob said "Don't do that" 11/1/2005
			diff = cableAdvance - cableDelay;
			if (diff < CABLE_ADV_THRESHOLD) {
				status = CABLE_ADVANCE_STABLE;
			}
#else
			status = CABLE_ADVANCE_STABLE;
#endif
		} else {
			cableAdvance = m_cableAdvance;
			status = CABLE_ADVANCE_STABLE;
		}
	}
#endif // old code 6/20/2006

	return 1;
}

void CDtiOutput::CfEnable(int chan, int enable)
{
	CDtiOutput *pOutput;
	
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pOutput = m_dtiOutput[chan];
	if (pOutput->m_enable != enable) {
		pOutput->m_enable = enable;
		COutputProcess::s_pOutputProcess->ToSetupOutput();
		m_pOutputPm->SetEnable(chan, enable); // Trikle it to PM part.
	}
}

void CDtiOutput::CfValidate(int chan, int valid)
{
	CDtiOutput *pOutput;
	
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pOutput = m_dtiOutput[chan];
	if (pOutput->m_valid != valid) {
		pOutput->m_valid = valid;
		COutputProcess::s_pOutputProcess->ToSetupOutput();
		CFpga::s_pFpga->SetPortMode(pOutput->m_chan, valid);
	}
}

void CDtiOutput::CfTestMode(int chan, int test)
{
	CDtiOutput *pOutput;
	
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pOutput = m_dtiOutput[chan];
	if (pOutput->m_testMode != test) {
		pOutput->m_testMode = test;
		COutputProcess::s_pOutputProcess->ToSetupOutput();
	}
}

void CDtiOutput::CfCableAdvanceValue(int chan, uint32 advance)
{
	CDtiOutput *pOutput;
	
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pOutput = m_dtiOutput[chan];
	if (pOutput->m_cableAdvanceManualSet != advance) {
		pOutput->m_cableAdvanceManualSet = advance; 
		COutputProcess::s_pOutputProcess->ToSetupOutput();
	}
}

void CDtiOutput::CfCableAdvanceMode(int chan, int mode)
{
	CDtiOutput *pOutput;
	
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pOutput = m_dtiOutput[chan];
	if (pOutput->m_cableAdvanceMode != mode) {
		pOutput->m_cableAdvanceMode = mode;
		if (mode != CIstateDti::AUTO_MODE) { // it's not auto. 
			// pOutput->m_init = 0x1000; // 6/20/2006 adjust slow slow
		}
		COutputProcess::s_pOutputProcess->ToSetupOutput();
	}
}

void CDtiOutput::SetupOutput()
{
	int chan;

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		m_dtiOutput[chan]->SetupOutputChan();
	}
}

void CDtiOutput::SetupOutputChan()
{
	uint8 tmp_exr;

	if (m_testMode && (m_valid)) {
		// test mode
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		m_control |= PORT_ACTION_ENABLE_TEST;
		*PORT_CONTROL = m_control;
		set_exr(tmp_exr);
		ReportStatus(1);
		m_eventStatus.ReportTesting(1);
		return;
	} else { // non-test mode
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		m_control &= ~PORT_ACTION_ENABLE_TEST;
		*PORT_CONTROL = m_control;
		set_exr(tmp_exr);
		m_eventStatus.ReportTesting(0);
	}

	if ((!m_enable) || (!m_valid)) {
		if (m_valid) {
			SetNLed(0, 0);
		} else {
			// Not valid. How to do led?
			SetNLed(0, 0);
		}
		m_init = 0x1000;
		m_serverStatusFlag = 0;
		SetNServerStatusFlag();
		ReportStatus();
		return;
	}

#if 0 // 6/21/2006. Don't do it anymore.
	if (m_cableAdvanceMode == CIstateDti::MANUAL_MODE) {
		m_cableAdvance = m_cableAdvanceManualSet;
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		*CABLE_ADVANCE = m_cableAdvance;
		set_exr(tmp_exr);
	}
#endif
}

void CDtiOutput::DtiOutputHack(unsigned long *param, Ccomm *pComm)
{
	int ok = 0;
	char buff[300];
	uint8 hex[100];
	uint8 tmp_exr;
	uint8 chan;  
	int i;
	uint8 *pAddr;
	CDtiOutput *pOutput = NULL;

	chan = param[2];
	if (chan < NUM_OUTPUT_CHAN) {
		pOutput = m_dtiOutput[chan];	
	}

	if (*param == 2) {
		switch (param[1]) {
		case 1: // Read registers
			if (chan >= NUM_OUTPUT_CHAN) {
				pComm->Send("Invalid channel\r\n");
				return;
			}
			pAddr = (uint8 *)0x400080;
			tmp_exr = get_exr();
			set_exr(7);
			*OUTPUT_PORT_SELECT = chan;
			memcpy(hex, pAddr, 0x40);
			set_exr(tmp_exr);
			sprintf(buff, "DTI OUTPUT[%d] registers:\r\n", chan);
			pComm->Send(buff);
			for (i = 0; i < 0x40; i += 0x10) {
				CDebug::PrintLine(buff, (uint32)pAddr + i, 0x10, hex + i);
				pComm->Send(buff);
			}
			return;
		case 2:
			if (pOutput != NULL) {
				sprintf(buff, "CableDelay=0x%08lX CableAdvance=0x%08lX\r\n"
					"Enable=%d m_init=%d m_control=%d\r\n"
					"CardStatus: %d\r\n"
					"StatusFlag: 0x%02X\r\n"
					"ActiveCnt=%d\r\n"
					, pOutput->m_cableDelay, pOutput->m_cableAdvance
					, pOutput->m_enable, pOutput->m_init, pOutput->m_control
					, m_cardStatus
					, pOutput->m_serverStatusFlag
					, CDtiOutput::m_activeCnt

					);
				pComm->Send(buff);
				sprintf(buff, "DeviceType: 0x%X\r\n"
					, m_deviceType);
				pComm->Send(buff);
				return;
			}
			break;
		case 3:
			SetServerHop((int)param[2]);
			return;
		case 4:
			CDtiOutputTrace::TraceHack((int)param[2], pComm);
			return;
		}
	} else if (*param == 4) {
		
		if ((param[3] < 0x400000) || (param[3] > 0x401000)) {
			pComm->Send("Invalid address\r\n");
			return;
		}

		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = (uint8)param[2];
		switch (param[1]) {
		case 10:  // Write a byte
			*(uint8 volatile *)param[3] = (uint8)param[4];
			ok = 1;
			pComm->Send("8-bit written\r\n");
			break;
		case 11:  // Write 16-bit
			*(uint16 volatile *)param[3] = (uint16)param[4];
			ok = 1;
			pComm->Send("16-bit written\r\n");
			break;
		case 12:  // Write 32-bit
			*(uint32 volatile *)param[3] = (uint32)param[4];
			ok = 1;
			pComm->Send("32-bit written\r\n");
			break;
		}
		set_exr(tmp_exr);

	}


	if (!ok) {
		const char *pHelp;
		pHelp = "Syntax: <option> <chan> [<data>]\r\n"
			"option =\t1 <chan>: Read <chan> registers\r\n"
			"\t2 <chan>: Read <chan> status\r\n"
			"\t3 <hop>: Force hop\r\n"
			"\t4 <chan>: Print Trace info\r\n"
			"\t10 <chan> <address> <data>: Write a byte for output <chan>\r\n"
			"\t11 <chan> <address> <data>: Write a 16-bit for output <chan>\r\n"
			"\t12 <chan> <address> <data>: Write a 32-bit for output <chan>\r\n"
			;
		pComm->Send(pHelp);
	}
}

void CDtiOutput::ReportStatus(int clearIt)
{

	if ((!m_valid) || (!m_enable) || (clearIt) || (m_testMode)) {
		m_eventStatus.ReportLoopStatus(0);
		m_eventStatus.ReportCableAdvanceValid(1);
		m_eventStatus.ReportPerformanceStable(1);
		m_eventStatus.ReportConnect(1); // Clear the alarm.
		m_eventStatus.ReportClockType(CIstateDti::CLIENT_CLOCK_TYPE_UNKNOWN);
		m_eventStatus.ReportDtiVersion(-1);
	} else if (m_outputPortStatusAnd & m_losBit) { // It's disconnected
		m_eventStatus.ReportLoopStatus(0);
		m_eventStatus.ReportCableAdvanceValid(1);
		m_eventStatus.ReportPerformanceStable(1);
		m_eventStatus.ReportConnect(0);
		m_eventStatus.ReportClockType(CIstateDti::CLIENT_CLOCK_TYPE_UNKNOWN);
		m_eventStatus.ReportDtiVersion(-1);
	} else if ((m_outputPortStatusOr & (m_losBit | m_lofBit)) == 0L) {
		m_eventStatus.ReportConnect(1);
		m_eventStatus.ReportCableAdvanceValid(m_serverStatusFlag & CABLE_ADVANCE_STABLE);
		m_eventStatus.ReportPerformanceStable(m_serverStatusFlag & CLIENT_PERFORMANCE_STABLE);
		m_eventStatus.ReportLoopStatus(m_clientStatusFlag & CLIENT_STATUS_MASK);
		m_eventStatus.ReportClockType(m_clientDeviceType & 0xf);
		m_eventStatus.ReportDtiVersion(m_clientVersion);
	}
}


int CDtiOutput::ReceiveOutputControl(const char *ptr, int size)
{
	SOutputControl *pCtrl;
	uint8 tmp_exr;

	pCtrl = (SOutputControl *)ptr;
	if (pCtrl->version == 1) {
		tmp_exr = get_exr();
		set_exr(7);
		if (size < sizeof(SOutputControl)) {
			memset(&m_twinControl, 0, sizeof(SOutputControl));
		}
		memcpy(&m_twinControl, pCtrl, sizeof(SOutputControl));
		set_exr(tmp_exr);
		COutputProcess::s_pOutputProcess->SetupStandbyOutput();
	}
	return 0;
}

int CDtiOutput::SendOutputControl()
{
	SOutputControl ctrl;
	int chan;
	CDtiOutput *pOutput;

	ctrl.cmd = CTwinProxy::CMD_OUTPUT_CONTROL;
	ctrl.version = 1;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pOutput = m_dtiOutput[chan];
		ctrl.cableAdvance[chan] = pOutput->m_cableAdvance;
		ctrl.outputLed[chan] = pOutput->m_led;
		ctrl.serverStatus[chan] = pOutput->m_serverStatusFlag | m_serverMode;
	}
	ctrl.pmEdge = m_pmEdge;
	m_controlUpdated = 0;
	return CTwinProxy::s_pTwinProxy->PostBuffer((char *)&ctrl, sizeof(ctrl));
}

void CDtiOutput::SetupStandby()
{
	int chan;
	CDtiOutput *pOutput;
	uint8 tmp_exr;
	uint8 flag;

	tmp_exr = get_exr();
	set_exr(7);

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pOutput = m_dtiOutput[chan];
		
		*OUTPUT_PORT_SELECT = chan;
		*CABLE_ADVANCE = pOutput->m_cableAdvance = m_twinControl.cableAdvance[chan];
		pOutput->m_serverStatusFlag = flag = m_twinControl.serverStatus[chan] & ~SERVER_STATUS_MASK;
		*SERVER_STATUS = flag | m_serverMode;
		*LED = pOutput->m_led = m_twinControl.outputLed[chan];
		if (IS_STANDBY == HOT_STANDBY) {
			pOutput->m_init = 0; // 6/23/ 2006 For standby. don't reinit once refreshed by active ioc
		}
	}

	set_exr(tmp_exr);

	// For standby card, cable advance/delay becomes one thing.
#if 0
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pOutput = m_dtiOutput[chan];
		SET_MY_ISTATE(COMM_RTE_KEY_OUTPUT_CABLE_DELAY, chan, 
			0, pOutput->m_cableAdvance);
	}

	TRACE("Receive Output Ctrl packet\r\n");
#endif
}

void CDtiOutput::ServiceChanWarmup()
{
	uint8 tmp_exr;

	if (m_serviceCnt >= 2) {
		ReportStatus();
	}
	
	if (IsDisabled()) { 
		return;
	}

	if (IsLos()) {
		return;
	}

	tmp_exr = get_exr();
	set_exr(7);
	*OUTPUT_PORT_SELECT = m_chan;
	*SERVER_STATUS = /*m_serverStatusFlag |*/ m_serverMode;
	set_exr(tmp_exr);


}

void CDtiOutput::ClearEvents()
{        
	int chan;
	CDtiOutput *pChan;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pChan = m_dtiOutput[chan];
		pChan->ReportStatus(1); // Clear all event
		pChan->m_clientLockCnt = 0;
	}

}

int CDtiOutput::IsDisabled()
{
	uint8 tmp_exr;
	int yellow = -1, green = -1;

	if (m_testMode && m_valid) {
		// Blink LED
		if (m_serviceCnt & 1) {
			yellow = 0; green = 0;
		} else {
			yellow = 1; green = 1;
		}
		SetNLed(yellow, green);
		return 1;
	}

	if ((!m_valid) || (!m_enable)) {
		tmp_exr = get_exr();
		set_exr(7);
		*OUTPUT_PORT_SELECT = m_chan;
		*SERVER_STATUS = /*m_serverStatusFlag |*/ m_serverMode;
		set_exr(tmp_exr);
		return 1;
	}
	return 0;
}

int CDtiOutput::IsLos()
{
	int yellow  = -1, green = -1;
	int serverStatus = 0;

	if ((m_serverStatusFlag & CABLE_ADVANCE_STABLE) 
		&& (m_serverStatusFlag & CLIENT_PERFORMANCE_STABLE)) {
		serverStatus = 1;
	}

	// Setup LED according to status
	if (m_outputPortStatusOr & (m_losBit | m_lofBit)) { // Los or LOF
		yellow = 1; green = 0;
	} else if ((!(m_clientStatusFlag & CLIENT_NORMAL_TRACK)) 
		|| (!serverStatus)) { // Not in normal track
		yellow = 1; green = 1;
	} else { // looks everything ok
		yellow = 0; green = 1;
	}
	SetNLed(yellow, green);
    if (green != 1) { // This is not a stable second.
    	return 1;
    }
	return 0;

}

int CDtiOutput::SendTwinTCounters(int index2)
{
	int one, two;
	uint32 value;
	int ret = 0;

	two = index2;
	for (one = 0; one < NUM_OUTPUT_CHAN; one++) {
		value = *(m_pReportTCounters + two * NUM_OUTPUT_CHAN + one);
		if (value == 0L) continue;
		ret = WaitForIstateTrasnfer();
		SET_TWIN_ISTATE(COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT,
			one, two, value);
	}
	return ret;
}

void CDtiOutput::InactiveClearance(int cnt)
{
	int i;

	switch (cnt) {
	case 1: // Send current copy to active twin and clear myself
		for (i = 0; i < 4; i++) {
			SendTwinTCounters(i);
		}
		memset(m_pReportTCounters, 0, NUM_OUTPUT_CHAN * 4 * 4);
		break;
	case 2:
		for (i = 0; i < 2; i++) {
			SendNHDuration(i);
		}
		memset(m_pReportNHDuration, 0, NUM_OUTPUT_CHAN * 2 * 4);
		break;
	}

}

int CDtiOutput::SendNHDuration(int nh)
{
	int one, two;
    uint32 value;
    int ret = 0;

	two = nh;
	for (one = 0; one < NUM_OUTPUT_CHAN; one++) {
		value = *(m_pReportNHDuration + two * NUM_OUTPUT_CHAN + one);
		if (value == 0L) continue;
		ret = WaitForIstateTrasnfer();
		SET_TWIN_ISTATE(COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME,
			one, two, value);
	}
	return ret;
}

int CDtiOutput::WaitForIstateTrasnfer()
{
	int load;
	load = m_pIOCLink->GetBusySendingBufferCnt(CIstateProcess::LINK_IOCISTATE_IOC_IOC);
	if (load > 5) {
		if (load > 10) {
			CAllProcess::g_pAllProcess->Pause(TICK_1SEC / 10);
		}
		CAllProcess::g_pAllProcess->Pause(1);
	}
	return load;
}
