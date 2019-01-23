// BT3Support.cpp
//
//////////////////////////////////////////////////////////////////////
/* 
 * BT3 support functions
 * $Header: BT3Support.cpp 1.14 2010/10/14 00:09:33PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.14 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "BT3Support.h"
#include "Fpga.h"
#include "IstateKbIoc.h"
#include "string.h"
#include "comm.h"
#include "stdio.h"
#include "AllProcess.h"   
#include "Input.h"
#include "assert.h"
#include "string.h"
#include "SecondProcess.h"
#include "machine.h"
#include "IstateDti.h"
#include "InputSelect.h"
#include "CardLoop.h"
#include "Warmup.h"
#include "InputGps.h"
#include "CardStatus.h"
#include "bt3_def.h"

#ifndef TRACE
#define TRACE TRACE_SECOND
#endif

CBT3InBound *CBT3InBound::s_pBT3InBound = NULL;
int CBT3InBound::m_todSource = -1;
int CBT3InBound::m_efcOk = 0;

CBT3InBound::CBT3InBound()
{
	int i;

	m_rgpInCnt = 0L;
	m_rgpOutCnt = 0L;
	m_rgpInRequestCnt = 0L;
	m_rgpOutRequestCnt = 0L;
	m_sysMode = 2;// 0; // default SSU mode.// 3/2/2006: for now it's GPS mode always. TBD
	for (i = 0; i < CInput::NUM_INPUT_CHAN; i++) {
		m_inputState[i].alarm = 1;
		m_inputState[i].priority = 1;
		m_inputState[i].ql = 1;
		m_inputState[i].m_bEnable = 0;
	}
	memset(&m_gps, 0, sizeof(m_gps));
	m_rgpIncoming = 0;
	m_rgpOutcoming = 0;
	m_inputChanged = 1;
	m_gpsPhysicalError = 0;
	m_todSource = -1;
	m_activeInput = -1;

	m_rgpDebugCnt = 0;
	m_rgpExpectedCnt = 0;

	m_traceSource = -1;
}

CBT3InBound::~CBT3InBound()
{
}

void CBT3InBound::Create(void)
{
	if (s_pBT3InBound == NULL) {
		s_pBT3InBound = new CBT3InBound();
	}
}

void CBT3InBound::SetSysMode(int mode) 
{
	m_sysMode = (uint8)mode;
}

void CBT3InBound::ResetBT3(int option /* = RESET_NORMAL_COLD */)
{
	unsigned short reset;
	reset = (GetOscillatorType() == RB) ? (0x200) : (0);
	reset |= option;
	PutBt3ResetControlWord(reset);
	switch (option) {
	case RESET_NORMAL_COLD:
		REPORT_EVENT(COMM_RTE_EVENT_BT3_COLD_RESET, 1);
		break;
	case RESET_WARM_START:
		REPORT_EVENT(COMM_RTE_EVENT_BT3_WARM_RESET, 1);
		break;
	}
	TRACE("BT3 reset: %d\r\n", option);
	SetBt3TraceSource();
}


void CBT3InBound::TickBT3InBound1Second()
{
	register int error;

	m_rgpExpectedCnt++;

	if (m_inputChanged) {
		SendInputStatusBT3();
		m_inputChanged = 0;
	}

	if (m_gpsPhysicalError < 100) {
		error = m_gpsPhysicalError + 1;
		if (error < 100) {
			m_gpsPhysicalError = error;
		}
	} else {
		error = m_gpsPhysicalError - 1;
		m_gpsPhysicalError = error;
	}
	
	if (ReadEfcOk()) {
		if (m_efcOk++ > 2000) {
			m_efcOk = 1000;
		}
	} else {
		m_efcOk = 0;
	}
}

// From Istate
void CBT3InBound::SetInputEnable(int chan, int enable)
{
	if ((chan >= CInput::NUM_INPUT_CHAN) || (chan < 0)) return;
	enable = (enable) ? (1) : (0);
	if (m_inputState[chan].m_bEnable != enable) {
		m_inputState[chan].m_bEnable = enable;
		m_inputChanged = 1;
	}
}

// Set active input BT3
void CBT3InBound::SetActiveInput(int chan)
{
	m_activeInput = chan;
	SetBt3TraceSource();
	CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);  //GPZ June 2010 Force warm restart
}

void CBT3InBound::SetBt3TraceSource(void)
{
	set_TOD_mode(m_todSource); //GPZ June 2010 pass TOD information
	switch (m_activeInput) {
	case CHAN_BITS1:
		set_trace_source(0);
		m_traceSource = 0;
		break;
	case CHAN_BITS2:
		set_trace_source(1);
		m_traceSource = 1;
		break;
	case CHAN_GPS:
		set_trace_source(3);
		m_traceSource = 3;
		break;
	}
}

// Put back input info into BT3 engine
void CBT3InBound::SendInputStatusBT3()
{
	bestime_engine_ensemble_control(
		m_inputState[CInput::INPUT_CHAN_SPAN1].ql,	// unsigned short SPAQ, 
		m_inputState[CInput::INPUT_CHAN_SPAN1].priority, //unsigned short SPAP,
		m_inputState[CInput::INPUT_CHAN_SPAN1].alarm, //unsigned short SPAA,
		m_inputState[CInput::INPUT_CHAN_SPAN1].m_bEnable,  //unsigned short SPAE, 
		m_inputState[CInput::INPUT_CHAN_SPAN2].ql, //unsigned short SPBQ, 
		m_inputState[CInput::INPUT_CHAN_SPAN2].priority, //unsigned short SPBP,
		m_inputState[CInput::INPUT_CHAN_SPAN2].alarm, //unsigned short SPBA,
		m_inputState[CInput::INPUT_CHAN_SPAN2].m_bEnable, //unsigned short SPBE,
		m_inputState[CInput::INPUT_CHAN_PRS].ql,  //unsigned short PRRQ, 
		m_inputState[CInput::INPUT_CHAN_PRS].priority, //unsigned short PRRP,
		m_inputState[CInput::INPUT_CHAN_PRS].alarm, //unsigned short PRRA,
		m_inputState[CInput::INPUT_CHAN_PRS].m_bEnable //unsigned short PRRE
	);
}

// updated when input status changes
void CBT3InBound::SetInputAlarm(int chan, int alarm)
{
	if ((chan >= CInput::NUM_INPUT_CHAN) || (chan < 0)) return;
	if (m_inputState[chan].alarm != alarm) {
		m_inputState[chan].alarm = alarm;
		m_inputChanged = 1;
	}
}

// From Istate
void CBT3InBound::SetInputPriority(int chan, int priority)
{
	if ((chan >= CInput::NUM_INPUT_CHAN) || (chan < 0)) return;
	if (m_inputState[chan].priority != priority) {
		m_inputState[chan].priority = priority;
		m_inputChanged = 1;
	}
}

// Every second it's updated for span1 and span2
void CBT3InBound::SetInputQualityLevel(int chan, int ql)
{
	if ((chan >= CInput::NUM_INPUT_CHAN) || (chan < 0)) return;
	if (m_inputState[chan].ql != ql) {
		m_inputState[chan].ql = ql;
		m_inputChanged = 1;
	}
}

void CBT3InBound::SetGPSReport(SReportGPS *pGPS)
{
	static uint32 lastGpsSec = 0;
	uint8 tmp_exr;
	int next;

	m_rgpDebugCnt++;

	tmp_exr = get_exr();
	set_exr(7);

	if ((pGPS->status < 1) || (pGPS->status > 2)) {
		m_gpsPhysicalError = 104;
		pGPS->status = 0; // Force it to 0 for now. zmiao 5/20/2008. 3 is possible when attenna disc.
	} else {
		m_gpsPhysicalError = 0;
	}
 
	m_rgpInRequestCnt++;
	next = (m_rgpIncoming + 1) % NUM_RGP;
	m_gps[m_rgpIncoming] = *pGPS;
	if (next != m_rgpOutcoming) {
		m_rgpIncoming = next;
		m_rgpInCnt++;
	}
	
	set_exr(tmp_exr);

	if (++lastGpsSec != pGPS->offsetTime) {
		TRACE("GPS second shift: %ld ==> %ld\r\n", lastGpsSec - 1, pGPS->offsetTime);
		lastGpsSec = pGPS->offsetTime;
	}
}

void CBT3InBound::ResetRgpFifo()
{
	// reset fifo
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	m_rgpOutcoming = m_rgpIncoming;
	m_rgpInCnt = 0L;
	m_rgpOutCnt = 0L;
	m_rgpInRequestCnt = 0L;
	m_rgpOutRequestCnt = 0L;
	set_exr(tmp_exr);
}

int CBT3InBound::GetPhysicalError()
{
	uint8 tmp_exr;
	int error;

	tmp_exr = get_exr();
	set_exr(7);
	error = m_gpsPhysicalError;
	set_exr(tmp_exr);

	if (error < 3) {
		return 0;
	}
	return error;
}

void CBT3InBound::SetTodSource(int tod)
{
	m_todSource = tod;
	set_TOD_mode(m_todSource); //GPZ JUNE 2010 set TOD source info   
// GPZ June 2010 do not change trace source	
//	if (m_todSource != tod) {
//		switch (tod) { 
//		
//		case CIstateDti::TOD_SOURCE_GPS:
//#if IGNORE_RGP
//			set_trace_source(4);
//#else
//			set_trace_source(3);
//#endif
//			break;
//		case CIstateDti::TOD_SOURCE_DTI:  
//			break;
//		}
//		m_todSource = tod;
//	}
}

void CBT3InBound::PrintInBoundStatus(Ccomm *pComm, unsigned long *param)
{
	volatile int i;
	volatile int j;
	volatile int outIndex;
	char buff[200];

	if (param != NULL) {
		if ((*param == 1) && (param[1] == 59)) {
			ResetRgpFifo();
			pComm->Send("RPG FIFO reset\r\n");
		}
		return;
	}

	sprintf(buff, "OscType: %d SysMode=(%s)%d\r\n"
		, GetOscillatorType(), GetSysModeName(m_sysMode), m_sysMode);
	pComm->Send(buff);
	
	for (i = 0; i < CInput::NUM_INPUT_CHAN; i++) {
		sprintf(buff, "Ch[%d, %s]: QL = %d\tPriority = %d\tAlarm = %d\tEnable = %d\r\n",
			i, CInput::GetInputChanName(i), m_inputState[i].ql, m_inputState[i].priority, 
			m_inputState[i].alarm,
			m_inputState[i].m_bEnable);
		pComm->Send(buff);
	}
	sprintf(buff, "Input Changed: %d\r\n", m_inputChanged);
	pComm->Send(buff);
	i = outIndex = m_rgpOutcoming; 
	j = m_rgpIncoming;
	if (i == j) {
		outIndex = (outIndex + NUM_RGP - 1) % NUM_RGP;
	}
	sprintf(buff, "GPS stat: time = %ld(0x%08lX), offset = %d, status = %d  Index=%d PPC=%d BT3=%d\r\n",
		m_gps[outIndex].offsetTime, m_gps[outIndex].offsetTime, m_gps[outIndex].offset, m_gps[outIndex].status, outIndex
		,j, i);
	pComm->Send(buff);
	sprintf(buff, "In Cnt: %ld  Out Cnt: %ld\r\nIn Request: %ld Out Request: %ld\r\n",
		m_rgpInCnt, m_rgpOutCnt, m_rgpInRequestCnt, m_rgpOutRequestCnt);
	pComm->Send(buff);
	sprintf(buff, "Physical Error Cnt: %d\r\n", m_gpsPhysicalError);
	pComm->Send(buff);
	sprintf(buff, "Is_EFC_OK() = %d\r\n", Is_EFC_OK());
	pComm->Send(buff);

	sprintf(buff, "RGP Total: %ld Expected=%ld\r\n", m_rgpDebugCnt, m_rgpExpectedCnt);
	pComm->Send(buff);

	sprintf(buff, "Active input for BT3 is %d\r\n", m_activeInput);
	pComm->Send(buff);

	sprintf(buff, "BT3 Trace Source = %d\r\n", m_traceSource);
	pComm->Send(buff);
}

int CBT3InBound::ReadEfcOk()
{
	if (IS_STANDBY) {
		int twin;
		twin = CCardStatus::s_pCardStatus->GetTwinStatus();
		if (twin == CCardStatus::CARD_HOT_ACTIVE) {
			return 1;
		}
		return 0;
	}
	
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_GPS:
		return 0;
	case CIstateDti::TOD_SOURCE_DTI: {
		int myLoop;

		myLoop = CCardLoop::s_pCardLoop->GetCardLoop();
		if ((myLoop == CCardLoop::CARD_FREQ_FAST_TRACK) || (myLoop == CCardLoop::CARD_FREQ_NORMAL_TRACK)) {
			return 1;
		} else {
			return 0;
		}
									 }
		break;
	default:
		return 0;
	}
	return 0;

}

CBT3OutBound *CBT3OutBound::s_pBT3OutBound = NULL;

CBT3OutBound::CBT3OutBound()
{                                                                   
	memset(&m_BT3AlmStatus, 0, sizeof(m_BT3AlmStatus));
	m_backupReg = 0;
	m_bt3Mode = -1;
	m_bt3Gear = -1;
	m_bt3ModeForced = -1;
	m_bt3GearForced = -1;
	m_prsReg = 0;
	m_spanAReg = 0;
	m_spanBReg = 0;
	m_prsLoss = 0;
	m_spanALoss = 0;
	m_spanBLoss = 0;
	m_backAnyLoss = 3;
	m_backAnyLossReported = 0;
	m_pBT3EngineReport = (uint32 *)CIstateDti::de_bt3Report.pVariable; // CIstateKbIoc::m_istateInRam.ve_bt3Report; 
	m_weightPrs = 0;
	m_weightSpan1 = 0;
	m_weightSpan2 = 0;
	m_todMode = -1;

	assert(sizeof(struct MSG_BesTime_Engine_Report) <= NUM_BT3_REPORT_WORD * 4);
}

CBT3OutBound::~CBT3OutBound()
{
}

void CBT3OutBound::Create(void)
{
	if (s_pBT3OutBound == NULL) {
		s_pBT3OutBound = new CBT3OutBound();
	}

}

void CBT3OutBound::CheckBackupInput(int chan, uint8 bEnable)
{   
	int8 alm = 0;

	switch (chan) {
	case CInput::INPUT_CHAN_PRS:
		if (bEnable) {
			// 2/3/2005:  use reg from George Z now.
			alm = (m_backupReg & 4) ? (0) : (1);
			m_backAnyLoss &= alm;
		}
		if (alm != m_prsLoss) {
			m_prsLoss = alm;
		}
		break;
	case CInput::INPUT_CHAN_SPAN1:
		if (bEnable) {

			// 2/3/2005: use reg from George Z now
			alm = (m_backupReg & 1) ? (0) : (1);
			m_backAnyLoss &= alm;
		}
		if (alm != m_spanALoss) {
			m_spanALoss = alm;
		}
		break;
	case CInput::INPUT_CHAN_SPAN2:
		if (bEnable) {
			// 2/3/2005: use reg from George Z now
			alm = (m_backupReg & 2) ? (0) : (1);
			m_backAnyLoss &= alm;
		}
		if (alm != m_spanBLoss) {
			m_spanBLoss = alm;
		}
		break;
	case CInput::INPUT_CHAN_GPS:
		alm = m_backAnyLoss;
		m_backAnyLoss = 3; // bit1: no one touched. bit0: & of enabled input.
		if (alm != m_backAnyLossReported) {
			m_backAnyLossReported = alm;
			// Do this event per card
			if (alm == 1) {
				REPORT_EVENT(COMM_RTE_EVENT_BACKUP_LOSS, 1);
			} else {
				REPORT_EVENT(COMM_RTE_EVENT_BACKUP_LOSS, 0);
			}
		}
	}


}

void CBT3OutBound::ClearBackupAlarm()
{
	if (m_prsLoss) {
		m_prsLoss = 0;
	}

	if (m_spanALoss) {
		m_spanALoss = 0;
	}

	if (m_spanBLoss) {
		m_spanBLoss = 0;
	}
	if (m_backAnyLossReported == 1) {
		m_backAnyLossReported = 0;
		REPORT_EVENT(COMM_RTE_EVENT_BACKUP_LOSS, 0);
	}
}

EXTERN void get_var_control(unsigned short *pHigh,unsigned short *pLow,unsigned short *pDither);

void CBT3OutBound::TickBT3OutBound1Second(void)
{
	unsigned short high;
	unsigned short low;
	unsigned short dither;
	int engineChange = 0;

	GetBt3AlarmStatus(&m_BT3AlmStatus);

	if (m_bt3Mode != m_BT3AlmStatus.bt3_mode) {
		m_bt3Mode = m_BT3AlmStatus.bt3_mode;
		engineChange = 1;
	}
	if (m_bt3Gear != m_BT3AlmStatus.Gear) {
		m_bt3Gear = m_BT3AlmStatus.Gear;
		engineChange = 1;
	}
	if (engineChange) {
		REPORT_EVENT(COMM_RTE_EVENT_BT3_ENGINE, 1, m_bt3Mode, m_bt3Gear);
	}
	m_prsReg = m_BT3AlmStatus.reg.ROA;
	m_spanAReg = m_BT3AlmStatus.reg.SPA;
	m_spanBReg = m_BT3AlmStatus.reg.SPB;

	switch (m_todMode) {
	case CIstateDti::TOD_SOURCE_GPS:
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		break;
	case CIstateDti::TOD_SOURCE_DTI:
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		//CFpga::s_pFpga->SetHighLowDither(0, 0, 0);
		break;
	case CIstateDti::TOD_SOURCE_RTC:
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		//CFpga::s_pFpga->SetHighLowDither(0x8000, 0x8000, 99);
		break;
	case CIstateDti::TOD_SOURCE_USER:
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		//CFpga::s_pFpga->SetHighLowDither(0x8000, 0x8000, 99);
		break;
	case CIstateDti::TOD_SOURCE_NTP:
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		//CFpga::s_pFpga->SetHighLowDither(0x8000, 0x8000, 99);
		break;    
	case CIstateDti::TOD_SOURCE_HYBRID: //GPZ June 2010 added o handle hybrid case
		get_var_control(&high, &low, &dither);
		CFpga::s_pFpga->SetHighLowDither(high, low, dither);
		//CFpga::s_pFpga->SetHighLowDither(0x8000, 0x8000, 99);
		break;
	default:
		break;
	}
}

void CBT3OutBound::TickBT3OutBound1Minute()
{
	uint8 tmp_exr;
	struct MSG_BesTime_Engine_Report *ptr = 
		(struct MSG_BesTime_Engine_Report *)m_pBT3EngineReport;
	int changed = 0;

	tmp_exr = get_exr();
	set_exr(7);
	Get_BesTime_Engine_Report((struct MSG_BesTime_Engine_Report *)m_pBT3EngineReport);

	if (m_weightPrs != ptr->PRR_Weight) {
		m_weightPrs = ptr->PRR_Weight;
		changed = 1;
	}
	if (m_weightSpan1 != ptr->SPA_Weight) {
		m_weightSpan1 = ptr->SPA_Weight;
		changed = 1;
	}
	if (m_weightSpan2 != ptr->SPB_Weight) {
		m_weightSpan2 = ptr->SPB_Weight;
		changed = 1;
	}
	m_backupReg = ptr->Backup_Reg;
	set_exr(tmp_exr);

	if (changed) {
		CInputSelect::s_pInputSelect->RefreshSelect();
	}
}

uint16 CBT3OutBound::GetInputWeight(int chan)
{

	switch (chan) {
	case CInput::INPUT_CHAN_PRS:
		return m_weightPrs; 
	case CInput::INPUT_CHAN_SPAN1:
		return m_weightSpan1; 
	case CInput::INPUT_CHAN_SPAN2:
		return m_weightSpan2; 
	}
	return 0;
}

int CBT3OutBound::GetGear(void)
{
#if ALLOW_FAKE_BT3
	if (m_bt3GearForced < 0) {
		return m_bt3Gear;  // m_BT3AlmStatus.Gear;
	} else {
		return m_bt3GearForced;
	}
#else
	return m_bt3Gear;  // m_BT3AlmStatus.Gear;
#endif
}

int CBT3OutBound::GetMode(void) 
{
	int iq;
#if ALLOW_FAKE_BT3
	if (m_bt3ModeForced >= 0) {
		return m_bt3ModeForced;
	}
#endif

#if 0
	iq = CInputGps::s_pInputGps->GetQualify();
	if (iq != CInput::INPUT_QUALIFY_OK) {
		if (m_bt3Mode == BT3_MODE_NORMAL) {
			return BT3_MODE_HOLDOVER;
		}
	}
#endif
	int activeInput = CInputSelect::s_pInputSelect->GetActiveInput();
	switch (activeInput) {
	case CHAN_GPS:
	case CHAN_BITS1:
	case CHAN_BITS2:
		break;
	default:
		if (m_bt3Mode == BT3_MODE_NORMAL) {
			return BT3_MODE_HOLDOVER;
		}
	}
	return m_bt3Mode;
}

uint16 CBT3OutBound::GetGPSReg()
{
	return m_BT3AlmStatus.reg.GPS;
}

int8 CBT3OutBound::IsGPSClear()
{
	// zmiao 5/7/2009: Changed from 0x23 to 0x2f
	// June 2010 GPZ back to 0x21 so drift alarms don't create churn
	// June 2010 GPZ force to 0x00 so all GPS churn eliminated
	return (m_BT3AlmStatus.reg.GPS & 0x00) ? (0) : (1);
}

void CBT3OutBound::PrintBT3Reg(const char *pName, unsigned short value, char *pBuffer)
{
	int i;
	char buff[2];
	buff[1] = 0;
	sprintf(pBuffer, "Reg %s: \t0x%04X ", pName, value);
	for (i = 15; i >= 0; i--) {
		buff[0] = '0' + ((value >> i) & 1);
		strcat(pBuffer, buff);
		if (i == 8) {
			strcat(pBuffer, " - ");
		}
	}
	strcat(pBuffer, "\r\n");
}

int CBT3OutBound::IsInputOk(int chan)
{
	switch (chan) {
	case CHAN_BITS1:
		//GPZ June 2010 
		return (m_BT3AlmStatus.reg.SPA & 0x21) ? (0) : (1);
	
//		if (m_BT3AlmStatus.reg.SPA == 0) return 1;
//		return 0;
	case CHAN_BITS2: 
			//GPZ June 2010 
			return (m_BT3AlmStatus.reg.SPB & 0x21) ? (0) : (1);

//		if (m_BT3AlmStatus.reg.SPB == 0) return 1;
//		return 0;
	}
	return 0;
}

void CBT3OutBound::PrintOutBoundStatus(Ccomm *pComm)
{
	char buff[200];
	PrintBT3Reg("GPS", m_BT3AlmStatus.reg.GPS, buff);
	pComm->Send(buff);
	PrintBT3Reg("SPANA", m_BT3AlmStatus.reg.SPA, buff);
	pComm->Send(buff);
	PrintBT3Reg("SPANB", m_BT3AlmStatus.reg.SPB, buff);
	pComm->Send(buff);
	PrintBT3Reg("ROA", m_BT3AlmStatus.reg.ROA, buff);
	pComm->Send(buff);
	PrintBT3Reg("ROB", m_BT3AlmStatus.reg.ROB, buff);
	pComm->Send(buff);
	sprintf(buff, "BT3 Mode: %d Dur:%d \tGear: %d\r\n", 
		m_bt3Mode, m_BT3AlmStatus.bt3_mode_dur, m_bt3Gear);
	pComm->Send(buff);
#if ALLOW_FAKE_BT3
	sprintf(buff, "Forced Mode: %d Gear: %d\r\n"
		, m_bt3ModeForced, m_bt3GearForced);
	pComm->Send(buff);
#endif
	sprintf(buff, "Weight PRS = %d  SPANA = %d SPANB = %d\r\n", GetInputWeight(CInput::INPUT_CHAN_PRS),
		GetInputWeight(CInput::INPUT_CHAN_SPAN1), GetInputWeight(CInput::INPUT_CHAN_SPAN2));
	pComm->Send(buff);
	sprintf(buff, "Backup Reg: %X\r\n", m_backupReg);
	pComm->Send(buff);
}


/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: return GPS phase word from FPGA
*Description:  
*************************************************************************/  
EXTERN uint32 Get_GPS_Phase(void) 
{	// In TC1000, bit31 = 1 to say it's out of measurement range. 
	// For now, make it looks the old way even though FPGA is change for TP5000.
	// It's always valid for TP5000
	return CFpga::s_pFpga->GetGPSPhase() & 0x7fffffff;
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: NONE
*Description:  Send a pulse to "Align GPS Phase" bit in FPGA
*************************************************************************/  
EXTERN void Align_GPS_Phase(void)
{
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: True if Tracking
*Description:  Will check the Phase error and LOS on Input reference and Holdover
* mode bit
*************************************************************************/  
EXTERN uint8 Is_MPLL_Tracking(void)
{
	int stat = CCardLoop::s_pCardLoop->GetCardLoop();

	if (stat == CCardLoop::CARD_FREQ_NORMAL_TRACK) {
		return 1;
	}
	return 0;

#if 0
	int stat = CCardState::s_pCardState->GetLoopState();
	if (stat == CCardState::CARD_LOOP_NORMAL_TRACKING) {
		return 1;
	}
	return 0;   
#endif	
}

/************************************************************************
*TITLE:        Is_OCXO_warm
*IN:           NONE
*              
*OUT:          NONE
*Return value: TRUE (1) if OCXO is warm, FALSE (0) if OCXO is not warm.
*Description:  
*************************************************************************/  
EXTERN uint8 Is_OCXO_warm(void)
{
	int stat = CCardLoop::s_pCardLoop->GetCardLoop();

	switch (stat) {
	// case CCardLoop::CARD_FREQ_WARMUP: // 4/18/2006: If OSC is warm, let it go.
	case CCardLoop::CARD_FREQ_FAIL:
	case CCardLoop::CARD_FREQ_OFFLINE:
		return 0;
	}
	if (!CWarmup::s_pWarmup->IsOvenWarm()) return 0;
	return 1;

#if 0	
	int stat;
	stat = CCardState::s_pCardState->GetCardState();
	if ((stat == CCardState::CARD_STATE_WARMUP) ||
		(stat == CCardState::CARD_STATE_FAILED)) {
		return 0; // Still in warmup or failed.
	}
	return 1; // past warmup
#endif
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: True if within window
*Description:  Will check if MPLL is within 1usec
*************************************************************************/  
EXTERN uint8 In_MPLL_Phase_Window(void)
{
	return 1;
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: 0: SSU mode  1: Subtending   2: PRR mode.
*Description:  return system mode from Istate
*************************************************************************/  
EXTERN uint8 Get_Clock_Mode(void)
{
	uint8 sysmode;

	sysmode = CBT3InBound::s_pBT3InBound->GetSysMode();
	return sysmode;
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: 
*Description:  Return RGP packet from PPC to BT3 engine
*************************************************************************/  
EXTERN void Get_RGP(SReportGPS *pRGP)
{
	uint8 tmp_exr;
	int next;
	int out;

	tmp_exr = get_exr();
	set_exr(7);
	out = CBT3InBound::s_pBT3InBound->m_rgpOutcoming;
	next = (out + 1) % CBT3InBound::NUM_RGP;
	CBT3InBound::s_pBT3InBound->m_rgpOutRequestCnt++;
	if (out != CBT3InBound::s_pBT3InBound->m_rgpIncoming) {
		// not empty
		*pRGP = CBT3InBound::s_pBT3InBound->m_gps[out];
		CBT3InBound::s_pBT3InBound->m_rgpOutcoming = next;
		CBT3InBound::s_pBT3InBound->m_rgpOutCnt++;
	} else {
		*pRGP = CBT3InBound::s_pBT3InBound->m_gps[(out + CBT3InBound::NUM_RGP -1) % CBT3InBound::NUM_RGP];
	}
	
	set_exr(tmp_exr);
}

// Reset RGP fifo
EXTERN void Reset_RGP_Fifo(void) 
{
	CBT3InBound::s_pBT3InBound->ResetRgpFifo();
}

EXTERN int Is_EFC_OK(void)
{
	if (CBT3InBound::m_efcOk >= 5) {
		return 1;
	}
	return 0;
}

EXTERN int GetOscillatorType(void)
{
	if (CFpga::s_pFpga->IsRubidium()) {
		return RB;
	} else {
		return DOCXO;
	}
}

const char * CBT3InBound::GetSysModeName(int mode)
{
	switch (mode) {
	case BT3_SYS_MODE_SSU:
		return "SSU";
	case BT3_SYS_MODE_PRR:
		return "PRR";
	case BT3_SYS_MODE_SUBTEND:
		return "SUBTEND";
	default:
		return "UNKONWN SYS MODE";
	}
}

void CBT3OutBound::SetTodSource(int mode)
{
	m_todMode = mode;
}


void CBT3OutBound::ForceBT3Mode(int mode)
{
	m_bt3ModeForced = mode;
}

void CBT3OutBound::ForceBT3Gear(int gear)
{
	m_bt3GearForced = gear;
}

