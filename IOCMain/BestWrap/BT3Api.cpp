// BT3Api.cpp: implementation of the CBT3Api class.
//
//////////////////////////////////////////////////////////////////////

#include "BT3Api.h"
#include "BT3Support.h"
#include "bt3_dat.h"
#include "NVRAM.h"
#include "BestCal.h"
#include "CardProcess.h"
#include "smartclkdef.h"
#include "AllProcess.h"
#include "Factory.h"
#include "BT3Debug.h"
#include "stdio.h"
#include "CardLoop.h"
#include "RTC.h"

EXTERN void BesTime_init(int isRubidium);
EXTERN void BT3_1second(void);
EXTERN void BT3_1minute(void);
EXTERN int  get_smartclk(void);

#ifndef TRACE
#define TRACE CCardProcess::s_pCardProcess->PrintDebugOutput
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
struct MPLL_Meas_Data mpll;
struct Span_Meas_Data GPS_Fifo;
struct Span_Meas_Data PRS_Fifo;
struct Span_Meas_Data SPANA_Fifo;
struct Span_Meas_Data SPANB_Fifo;

CBT3Api *CBT3Api::s_pBT3Api;

extern "C" void set_phase_control(unsigned int mode);

CBT3Api::CBT3Api(CNVRAM *pNVRAM)
{
	CBT3Debug::Create();
	CBT3InBound::Create();
	CBT3OutBound::Create();
	m_pBestCal = new CBestCal(pNVRAM);
	s_pBT3Api = this;
	InitBt3Data();
	m_smartClk = SMART_OFF;
	m_toStoreCal = 0;
	m_bt3PhaseMode = -100;
	m_forceBt3PhaseMode = -1;
	m_realBt3PhaseMode = -2;
}

CBT3Api::~CBT3Api()
{

}

void CBT3Api::BT3TickOneSecond()
{
	CBT3InBound::s_pBT3InBound->TickBT3InBound1Second();
	BT3_1second();
	CBT3OutBound::s_pBT3OutBound->TickBT3OutBound1Second();
}

void CBT3Api::BT3TickOneMinute()
{
	static uint32 minuteCnt = 0L;

	minuteCnt++;
	CBT3OutBound::s_pBT3OutBound->TickBT3OutBound1Minute();
	BT3_1minute();

	if (minuteCnt < 1440) {
		s_pBT3Api->CheckCalData();
	} else if ((minuteCnt % 60) == 55) {
		s_pBT3Api->CheckCalData();		
	}
}

void CBT3Api::Init(int isRubidium)
{
	CBestCal::s_pBestCal->Init();
	BesTime_init(isRubidium);
	CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_NORMAL_COLD);

	// 3/5/2009 zmiao: Do it in CWarmup so that the event module is up by then
	//SetPhaseControl(0);
}

void CBT3Api::InitBt3Data()
{
	InitFifo();	
}

void CBT3Api::InitFifo()
{
	memset(&mpll, 0, sizeof(mpll));
	memset(&GPS_Fifo, 0, sizeof(GPS_Fifo));
	memset(&PRS_Fifo, 0, sizeof(PRS_Fifo));
	memset(&SPANA_Fifo, 0, sizeof(SPANA_Fifo));
	memset(&SPANB_Fifo, 0, sizeof(SPANB_Fifo));
	
	GPS_Fifo.Xsp = 0.0;
	GPS_Fifo.Ysp = 0.0;
	GPS_Fifo.Yspavg = 0.0;
	GPS_Fifo.Yspsum = 0.0;
	GPS_Fifo.Sfilt1 = 0.0;
	GPS_Fifo.Sfilt2 = 0.0;
	GPS_Fifo.DeltaBias = 0.0;
	
	PRS_Fifo.Xsp = 0.0;
	PRS_Fifo.Ysp = 0.0;
	PRS_Fifo.Yspavg = 0.0;
	PRS_Fifo.Yspsum = 0.0;
	PRS_Fifo.Sfilt1 = 0.0;
	PRS_Fifo.Sfilt2 = 0.0;
	PRS_Fifo.DeltaBias = 0.0;	
	
	SPANA_Fifo.Xsp = 0.0;
	SPANA_Fifo.Ysp = 0.0;
	SPANA_Fifo.Yspavg = 0.0;
	SPANA_Fifo.Yspsum = 0.0;
	SPANA_Fifo.Sfilt1 = 0.0;
	SPANA_Fifo.Sfilt2 = 0.0;
	SPANA_Fifo.DeltaBias = 0.0;
	
	SPANB_Fifo.Xsp = 0.0;
	SPANB_Fifo.Ysp = 0.0;
	SPANB_Fifo.Yspavg = 0.0;
	SPANB_Fifo.Yspsum = 0.0;
	SPANB_Fifo.Sfilt1 = 0.0;
	SPANB_Fifo.Sfilt2 = 0.0;
	SPANB_Fifo.DeltaBias = 0.0;

}

void CBT3Api::CheckCalData()
{
	int year, month, day, hour, minute, second;
	int loop;

	loop = CCardLoop::s_pCardLoop->GetCardLoop();

	// Check Besttime Calibration data
	if ((Cal_Data.cal_state == CAL_Update) && (loop == CCardLoop::CARD_FREQ_NORMAL_TRACK)) {
		Cal_Data.cal_state = CAL_Stable;
		m_pBestCal->SaveChange();
		g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
		TRACE("Bestime Cal Data saved @%04d-%02d-%02d %02d:%02d:%02d\r\n"
			, year, month, day, hour, minute, second);
	} else if (m_toStoreCal) {
		m_toStoreCal = 0;
		if (Cal_Data.cal_state == CAL_Update) {
			Cal_Data.cal_state = CAL_Stable;
		}
		m_pBestCal->SaveChange();
		g_pRTC->GetDateTime(&year, &month, &day, &hour, &minute, &second);
		TRACE("Force Bestime Cal Data saved @%04d-%02d-%02d %02d:%02d:%02d\r\n"
			, year, month, day, hour, minute, second);
	}
}

void CBT3Api::CheckSmartClock()
{
	int smart;

	smart = get_smartclk();
	
	// CCardState::ReportSmartClock(smart); // to be completed

	// 12/13/2004: George Z said: for Rub RTE, degraded SMT CLK is not smart enough, don't report degrade SMART as SMART event.
	if (smart != m_smartClk) {
		// Clear 
		switch (m_smartClk) {
		case SMART_OFF:
			break;
		case SMART_ON:
			TRACE("EVENT: End of Smart Clock\r\n");
			break;
		case SMART_DEGRADE:
			TRACE("EVENT: End of Degraded-Smart Clock\r\n");
			break;
		}

		switch (smart) {
		case SMART_OFF:
			// When report event, don't differentiate how smart RTE is.
			REPORT_EVENT(COMM_RTE_EVENT_SMART_CLOCK, 0);
			break;
		case SMART_ON:
			TRACE("EVENT: Smart Clock ON\r\n");
			REPORT_EVENT(COMM_RTE_EVENT_SMART_CLOCK, 1);
			break;
		case SMART_DEGRADE:
			TRACE("EVENT: Degraded Smart Clock ON\r\n");
			{
				int8 isRub;
				CFactory::s_pFactory->GetOscType(&isRub);
				if (isRub) {
					// Rub RTE, make event off
					REPORT_EVENT(COMM_RTE_EVENT_SMART_CLOCK, 0);
				} else {
					REPORT_EVENT(COMM_RTE_EVENT_SMART_CLOCK, 1);
				}
			}
			break;
		default:
			TRACE("Fatal: Unknown smart type[%d]\r\n", smart);
			smart = SMART_OFF;
			break;
		}
		m_smartClk = smart;
	}

}

void CBT3Api::StoreCalData()
{
	m_toStoreCal = 1;
}

// 1: Phase control. 
// 0: frequency only
void CBT3Api::SetPhaseControl(int phaseMode)
{
	if (m_bt3PhaseMode == phaseMode) return;

	m_bt3PhaseMode = phaseMode;

	DoPhaseControl();
}

void CBT3Api::PrintBT3Api(Ccomm *pComm)
{
	char buff[100];

	sprintf(buff, "BT3 Effective Phase Control: %d  AutoMode: %d  Manual: %d\r\n", 
		m_realBt3PhaseMode, m_bt3PhaseMode, m_forceBt3PhaseMode);
	pComm->Send(buff);
}

void CBT3Api::ForcePhaseControl(int force)
{
	if (force < 0) {
		force = -1;
	} else if (force > 2) {
		return; // invalid choice
	}
	
	if (m_forceBt3PhaseMode == force) {
		return;
	}
	m_forceBt3PhaseMode = force;

	DoPhaseControl();
}

void CBT3Api::DoPhaseControl(void)
{
	int phaseMode;

	if ((m_forceBt3PhaseMode >= 0) && (m_forceBt3PhaseMode <= 2)) {
		phaseMode = m_forceBt3PhaseMode;
	} else {
		phaseMode = m_bt3PhaseMode;
	}

	set_phase_control(phaseMode);

	if (m_realBt3PhaseMode >= 0) {
		REPORT_EVENT(COMM_RTE_EVENT_BT3_PHASE_CONTROL, 0, m_realBt3PhaseMode);
	}
	REPORT_EVENT(COMM_RTE_EVENT_BT3_PHASE_CONTROL, 1, phaseMode);

	m_realBt3PhaseMode = phaseMode;

}
