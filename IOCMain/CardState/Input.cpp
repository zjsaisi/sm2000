// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: Input.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: Input.cpp 1.19 2011/07/08 15:35:10PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.19 $
 */             

#include "Input.h"
#include "InputGps.h"
#include "InputDti.h"
#include "InputSpan.h"
#include "stdio.h"
#include "string.h"
#include "CardProcess.h"
#include "InputSelect.h"
#include "CardStatus.h"
#include "Fpga.h"
#include "IstateDti.h"
#include "BT3Support.h"
#include "IstateProcess.h" // GPZ June 2010 hack to gain control of GPS priority
#define TRACE TRACE_CARD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInput *CInput::s_pInput[NUM_INPUT];
int CInput::m_cardStatus = 0;
int CInput::m_isCardActive = 0;
int CInput::m_activeInput = -1;
int CInput::m_todSource = -1;

CInput::CInput(int chan, const char *pName, int noStandbyInput) : m_event(chan)
{
	m_chan = chan;

	m_noStandbyInput = noStandbyInput;
	m_chan256 = chan << 8;
	m_inputStat = INPUT_STAT_LOS;
	m_inputStatDebug = INPUT_STAT_LOS;
	m_inputDbgFlag = 0x00;
	strncpy(m_name, pName, 9);
	m_name[9] = 0;
	m_priority = 1;
	m_changed = 1;
	m_cnt = 0;
	m_qualDelay = m_qualDelayCf = 10;
	m_disqDelay = m_disqDelayCf = 10;
	m_inputQualified = INPUT_QUALIFY_IGNORE;
	m_cfEnable = 0;
	m_cfValid = 0;
	m_enable = INPUT_CONFIG_DISABLE;

	m_toBeSelected = 1;

	m_statNow = INPUT_STAT_LOS;
	m_statReported = INPUT_STAT_LOS;
	m_statCnt = 100;
}

CInput::~CInput()
{

}

void CInput::CreateInput()
{
	s_pInput[CHAN_GPS] = new CInputGps(CHAN_GPS, "GPS");
//	s_pInput[CHAN_GPS]->m_toBeSelected = 0; // GPS input not to be selected by an algorithm   GPZ June 2010 Allow

	s_pInput[CHAN_DTI1] = new CInputDti(CHAN_DTI1, "DTI1");
	s_pInput[CHAN_DTI2] = new CInputDti(CHAN_DTI2, "DTI2");

	CInputSpan::CreateSpan();
	s_pInput[CHAN_BITS1] = CInputSpan::s_pInputSpan[0];
	s_pInput[CHAN_BITS2] = CInputSpan::s_pInputSpan[1];
	//s_pInput[CHAN_BITS1] = new CInputSpan(CHAN_BITS1, "BITS1");
	//s_pInput[CHAN_BITS2] = new CInputSpan(CHAN_BITS2, "BITS2");
}

void CInput::Init()
{
	int chan;
	for (chan = 0; chan < NUM_INPUT; chan++) {
		if (s_pInput[chan] != NULL) {
			s_pInput[chan]->InitChan();
		}
	}
}

void CInput::InitChan()
{

}

/*
param[0]: # of parameters (not including param[0], param[1]~param[n])
param[1]: channel number, 0=gps,3=BITS1,4=BITS4
param[2]: sub commands, 1, 2, or 10
*/
int CInput::InputHack(unsigned long *param, Ccomm *pComm)
{
	char buff[256];
	int  numofparams;
	int  chn;
	int  subcmd;
	
	numofparams = param[0]; //param[1]~param[n]
	chn = param[1];
	subcmd = param[2];

	if (numofparams < 2) return -1;

	if (chn != m_chan) return -2;

	if (numofparams == 2) {
		switch (subcmd) {
		case 1:
			sprintf(buff, "Input[%d,%s]: Status: %d(%s) %d(%s) Enable: %d\r\n"
				"Qualified=%d GetPriority()=0x%X\r\n"
				"Changed=%d ToBeSelected=%d\r\n"
				, m_chan, m_name, m_inputStat, GetStatName(m_inputStat), m_statNow, GetStatName(m_statNow), m_enable
				, m_inputQualified, GetPriority()
				, m_changed, m_toBeSelected
				);
			pComm->Send(buff);
			CInputSelect::s_pInputSelect->InputSelectPrint(pComm);
			sprintf(buff, "Valid: %d\r\n", m_cfValid);
			pComm->Send(buff);
			sprintf(buff, "SSM QL: %d\r\n", GetCurrentQL());
			pComm->Send(buff);
			return 1;
		case 2:
			CInputSelect::s_pInputSelect->MakeSelect();
			return 1;
		}
	}
	
	if (numofparams == 3) {
		switch (subcmd) {
		case 10:
			if (param[3] != 0) {
				m_inputStatDebug = (int)param[3];
			}
			sprintf(buff, "Set input stat to %d(%s)\r\n",
				m_inputStatDebug, GetStatName(m_inputStatDebug));
			pComm->Send(buff);
			return 1;

		//added by don zhang:
		//input 0 11 1
		case 11:
			CInputSelect::s_pInputSelect->InputSetDebug((int)param[3]);
			sprintf(buff, "Set input selection debug flag to %d (0x%x)\r\n", (int)param[3], (int)param[3]);
			pComm->Send(buff);
			return 1;
			
	 //added by don zhang:
	 //input 0/3/4 12 1
		case 12:
			m_inputDbgFlag = param[3];
			sprintf(buff, "Set input debug flag to %d (0x%x)\r\n", (int)param[3], (int)param[3]);
			pComm->Send(buff);
			return 1;
		}
	}

	{
		const char *pHelp;
		pHelp = "Syntax: <chan> <options>\r\n"
			"option=1: Print input status\r\n"
			"\t2: Call Select process\r\n"
			"\t10 <stat>: Set Input stat\r\n" 
			"\t11 <debugflag>: Set Input selection debug flag\r\n" 
			;
		pComm->Send(pHelp);
	}
	return 0;
}

void CInput::AllInputHack(unsigned long *param, Ccomm *pComm)
{
	int chan;
	int ret;
	unsigned long error[3] = {2, 0 , 0};
	for (chan = 0; chan < NUM_INPUT; chan++) {
		ret = s_pInput[chan]->InputHack(param, pComm);
		if (ret >= 0) return;
	}
	s_pInput[0]->InputHack(error, pComm);
}


int CInput::ReadStatus()
{
	return m_inputStatDebug; // INPUT_STAT_LOS;
}

int CInput::GetPriority(void) 
{ 
	return m_priority; 
}

int CInput::IsToBeSelected(void)
{
	return m_toBeSelected;
}

// non-0: changed.
unsigned int CInput::AllInputTick()
{
	int chan;
	unsigned int ret = 0;

	for (chan = 0; chan < NUM_INPUT; chan++) {
		ret |= s_pInput[chan]->InputTick();
	}
	CInputSelect::s_pInputSelect->InputSelectTick();
	return ret;
}

// non-0: changed.
unsigned int CInput::InputTick()
{
	int stat;
	unsigned int ret;
	unsigned int ql;

	if (m_statCnt < 10000) {
		m_statCnt++;
	}
	
	ret = 0;

	if (m_enable == INPUT_CONFIG_DISABLE) {
		stat = INPUT_STAT_DISABLED;
	} else if ((!m_isCardActive) && (m_noStandbyInput)){
		if (m_chan == CInputSelect::s_pInputSelect->GetActiveInput()) {
			stat = INPUT_STAT_OK;
		} else {
			stat = INPUT_STAT_DISABLED;
		}
	} else {
		stat = ReadStatus();
	}

	if (stat != m_inputStat) {
		ret |= 2;
		m_changed++;
		TRACE("Input[%d] stat: %s\r\n", m_chan, GetStatName(stat));
	}
    
	if ((stat > INPUT_PHYSICAL_OK) && (stat != INPUT_STAT_DISABLED)) {
		CBT3InBound::s_pBT3InBound->SetInputAlarm(m_chan, 0);
		CFpga::s_pFpga->SetInputQualified(m_chan, 1);
	} else {
		CBT3InBound::s_pBT3InBound->SetInputAlarm(m_chan, 1);
		CFpga::s_pFpga->SetInputQualified(m_chan, 0);
	}

	ql = Qualify(stat);
	
#if 0 //GPZ June 2010 Special case for GPS channel
		if (m_chan == CHAN_GPS) {
			if ((m_priority!=1)&&(m_todSource == CIstateDti::TOD_SOURCE_GPS)
			 && (m_isCardActive == 1)) {    
				SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PRIORITY, 0, 0, 1);
//				CInput::s_pInput[m_chan]->CfPriority(1);  //GPZ June 2010 Force P1 for GPS case
				CBT3InBound::s_pBT3InBound->SetInputPriority(m_chan, 1);     
				TRACE("GPZ June 2010 Set GPS Priority to 1\r\n");
				// TRACE("GPS input set to %d\r\n", qq);
			} else if ((m_priority!=5)&&((m_todSource == CIstateDti::TOD_SOURCE_USER)||(m_todSource == CIstateDti::TOD_SOURCE_HYBRID))
					 && (m_isCardActive == 1)) {
				SET_MY_ISTATE(COMM_RTE_KEY_INPUT_PRIORITY, 0, 0, 5);
//				CInput::s_pInput[m_chan]->CfPriority(5);  //GPZ June 2010 Force P5 for GPS case     
				CBT3InBound::s_pBT3InBound->SetInputPriority(m_chan, 5); 
				TRACE("GPZ June 2010 Set GPS Priority to 5\r\n");
				// TRACE("GPS input set to %d\r\n", qq);
			} 
		} 
#endif
	
	
#if 0
	if (ql) {
		if (m_chan == CHAN_GPS) {
			int qq;
			qq = (m_inputQualified == INPUT_QUALIFY_OK) ? (1) : (0);
			if ((m_todSource == CIstateDti::TOD_SOURCE_GPS) && (m_isCardActive == 1)) {
				CFpga::s_pFpga->SetInputQualified(m_chan, qq);
				// TRACE("GPS input set to %d\r\n", qq);
			} else {
				CFpga::s_pFpga->SetInputQualified(m_chan, 0);
			}
		} else {
			CFpga::s_pFpga->SetInputQualified(m_chan, 
				(m_inputQualified == INPUT_QUALIFY_OK) ? (1) : (0));
		}
	}
#endif

	ret |= ql;
	
	if (m_inputQualified == INPUT_QUALIFY_OK) {
		stat = INPUT_STAT_OK;
	}
	m_inputStat = stat;

	m_event.ReportInputStatus(m_inputStat);
	m_event.ReportQualified(m_inputQualified);

	if (++m_cnt >= 0x7000) {
		m_cnt = 0x700;
	}

	if ((m_cnt & 0xfff) == m_chan256) {
		m_changed++;	
	} else if (m_changed) {
		m_changed--;
	}
	return(ret | m_changed);
}

void CInput::CfPriority(int priority)
{
	if (m_priority == priority) return;
	m_priority = priority;
	m_changed = 1;
	RefreshInputSetup();
}

void CInput::CfQualDelay(int delay)
{
	if (m_qualDelayCf == delay) return;
	m_qualDelayCf = delay;
	if (m_qualDelay > delay) {
		m_qualDelay = delay;
	}
	m_changed = 1;
	RefreshInputSetup();
}

void CInput::CfDisqDelay(int delay)
{
	if (m_disqDelayCf == delay) return;
	m_disqDelayCf = delay;
	if (m_disqDelay > delay) {
		TRACE("Input Disqualification delay changed: %d to %d\r\n", m_disqDelay, delay);
		m_disqDelay = delay;
	}
	m_changed = 1;
	RefreshInputSetup();
}

void CInput::CfInputEnable(int enable)
{
	if (m_cfEnable == enable) return;
	m_cfEnable = enable;
	DriveEnable();
	RefreshInputSetup();
}

void CInput::DriveEnable()
{
	if (!m_cfValid) {
		m_enable = INPUT_CONFIG_DISABLE;
	} else if (!m_cfEnable) {
		m_enable = INPUT_CONFIG_DISABLE;
	} else {
		m_enable = INPUT_CONFIG_ENABLE;
	}

	switch (m_enable) {
	case INPUT_CONFIG_DISABLE:
		m_inputQualified = INPUT_QUALIFY_IGNORE;
		break;
	}
	m_changed = 10;

	CBT3InBound::s_pBT3InBound->SetInputEnable(m_chan, (m_enable == INPUT_CONFIG_DISABLE) ? (0) :(1));
}

unsigned int CInput::Qualify(int stat)
{
	if ((m_enable == INPUT_CONFIG_DISABLE) || (stat == INPUT_STAT_DISABLED)) {
		if (m_inputQualified != INPUT_QUALIFY_IGNORE) {
			m_inputQualified = INPUT_QUALIFY_IGNORE;
			return 1;
		}
		return m_changed;
	}

	switch (m_inputQualified) {
	case INPUT_QUALIFY_OK:
		if (stat == INPUT_STAT_OK) {
			m_disqDelay = m_disqDelayCf;
			return m_changed;
		}
		if (m_disqDelay > 0) {
			m_disqDelay--;
			return m_changed;
		} else {
			m_inputQualified = INPUT_QUALIFY_DISQ;
			m_qualDelay = m_qualDelayCf;
			m_changed++;
			return 1;
		}
		break;
	case INPUT_QUALIFY_DISQ:
		if (stat != INPUT_STAT_OK) {
			m_qualDelay = m_qualDelayCf;
			return 0;
		}
		if (m_qualDelay > 0) {
			m_qualDelay--;
			return m_changed;
		} else {
			m_inputQualified = INPUT_QUALIFY_OK;
			m_disqDelay = m_disqDelayCf;
			m_changed++;
			return 1;
		}
		break;
	case INPUT_QUALIFY_IGNORE:
		m_qualDelay = m_qualDelayCf;
		m_disqDelay = m_disqDelayCf;
		m_inputQualified = INPUT_QUALIFY_DISQ;
		m_changed++;
		return 1;
	default:
		m_inputQualified = INPUT_QUALIFY_IGNORE;
		m_changed++;
		return 1;
	}
	return 1; // not needed.
}

const char * CInput::GetStatName(int stat)
{
	switch (stat) {
	case INPUT_STAT_OK: return "OK";
	case INPUT_STAT_DISABLED: return "DISABLE";
	case INPUT_STAT_LOS: return "LOS";
	case INPUT_STAT_OOF: return "OOF";
	case INPUT_STAT_AIS: return "AIS";
	case INPUT_STAT_PHASE_ERROR: return "PhaseERR";
	case INPUT_STAT_FREQUENCY_ERROR: return "FreqErr";
	case INPUT_STAT_IPLL_UNSETTLED: return "IPLLUNST";
	case INPUT_STAT_CABLE_ADVANCE_INVALID: return "CBL-ADV-INV";
	case INPUT_STAT_SERVER_WARMUP: return "SVR-WARMUP";
	case INPUT_STAT_INVALID_PATH_TRACEABILITY: return "INV-PATH";
	case INPUT_STAT_INVALID_TOD_MESSAGE: return "INV-TOD";
	case INPUT_STAT_NO_RGP: return "NO-RGP";
	case INPUT_STAT_GPS_ACQUIRING: return "GPS-ACQ";
	case INPUT_STAT_TA_TUNING_OUT_OF_RANGE: return "TATUNE-OOR";
	case INPUT_STAT_TPIU_TUNING_OUT_OF_RANGE: return "TPIUTUNE-OOR";
	case INPUT_STAT_GPS_PATH_CRC_ERROR: return "GPSCRC-ERR";
	case INPUT_STAT_BT3_UNSETTLED: return "BT3UNSET";
	case INPUT_STAT_POOR_SSM: return "POORSSM";
	default: return "INP-UNKNOWN";
	}
}

void CInput::TouchInput()
{
	for (int i = 0; i < NUM_INPUT; i++) {
		s_pInput[i]->m_changed = 5;
	}
}

void CInput::RefreshInputSetup(void)
{
	CCardProcess::s_pCardProcess->PostSetupInput();
}

const char * CInput::GetInputChanName(int chan)
{
	CInput *pInput;
	if ((chan < 0) || (chan >= NUM_INPUT)) return "UKN_INPUT";
	pInput = s_pInput[chan];
	if (pInput == NULL) return "NON_INPUT";
	return pInput->m_name;
}

int CInput::AllInputSetup()
{
	int chan;
	unsigned int ret = 0;

	m_cardStatus = CCardStatus::GetCardStatus();
	if ((m_cardStatus == CCardStatus::CARD_HOT_ACTIVE) 
		|| (m_cardStatus == CCardStatus::CARD_COLD_ACTIVE)) {
		m_isCardActive = 1;
	} else {
		m_isCardActive = 0;
	}

	for (chan = 0; chan < NUM_INPUT; chan++) {
		ret |= s_pInput[chan]->InputSetup();
	}
	return ret;

}

int CInput::InputSetup()
{
	return 0;
}

int CInput::IsLocked()
{
	return LOCK_LEVEL_NULL;
}

void CInput::SetActiveInput(int chan)
{
	m_activeInput = chan;
}

void CInput::CfValidChanInput(int valid)
{
	if (m_cfValid == valid) return;
	m_cfValid = valid;
	DriveEnable();
	RefreshInputSetup();
}

// static
void CInput::CfTodSource(int tod)
{
	int chan;
	if (m_todSource != tod) {
		m_todSource = tod;
		TouchInput();
	}
	
	for (chan = 0; chan < NUM_INPUT; chan++) {
		s_pInput[chan]->m_toBeSelected = 0;
	}
#if 0 //GPZ June 2010 let Istate control GPS enable	
	if (m_todSource == CIstateDti::TOD_SOURCE_GPS) {
		CInputGps::s_pInputGps->CfInputEnable(1);  
	}
	//GPZ JUNE 2010 Enable GPS in user mode	
	else if (m_todSource == CIstateDti::TOD_SOURCE_USER) {
		CInputGps::s_pInputGps->CfInputEnable(1);
		
	} else {
		CInputGps::s_pInputGps->CfInputEnable(0);
	}
#endif
	switch (m_todSource) {
	case CIstateDti::TOD_SOURCE_GPS:
		// only gps could be the active input
		s_pInput[CHAN_GPS]->m_toBeSelected = 1;
		s_pInput[CHAN_BITS1]->m_toBeSelected = 1; // GPZ June 2010 allow spans to be selected
		s_pInput[CHAN_BITS2]->m_toBeSelected = 1; // GPZ June 2010 allow spans to be selected     
		
		break;       
	case CIstateDti::TOD_SOURCE_HYBRID:  //GPZ JUNE 2010 added hybrid case
	case CIstateDti::TOD_SOURCE_USER:
		// Only bits input could the active input
		s_pInput[CHAN_BITS1]->m_toBeSelected = 1;
		s_pInput[CHAN_BITS2]->m_toBeSelected = 1;
		s_pInput[CHAN_GPS]->m_toBeSelected = 1;  //GPZ June 2010 Allow GPS to be selected
		break;
	}
}

int CInput::CfFrameChanInput(int frame)
{
	return 0;
}

int CInput::CfCrc4ChanInput(int crc4)
{
	return 0;
}

int CInput::CfSsmBitChanInput(int ssmBit)
{
	return 0;
}

int CInput::CfSsmReadChanInput(int ssmRead)
{
	return 0;
}

int CInput::CfAssumedQLChanInput(int assumed)
{
	return 0;
}

int CInput::UpdateInputStatus(int stat)
{
	m_statNow = stat;
	if (stat > 0) {
		m_statReported = stat;
	} else if (m_statReported != stat) {
		if (m_statCnt > m_qualDelayCf) {
			m_statReported = stat;
			m_statCnt = 0;
		}
		stat = m_statReported;
	}
	return stat;
}

int CInput::GetReportedStatus(void)
{
	return m_statReported;
}

int CInput::GetCurrentQL(void)
{
	return 14;
}
