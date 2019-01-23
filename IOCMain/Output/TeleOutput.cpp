// TeleOutput.cpp: Implementation of CTeleOutput
/////////////////////////////////////////////////
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: TeleOutput.cpp 1.12 2010/11/23 11:12:50PST Jining Yang (jyang) Exp  $
 * $Revision: 1.12 $
 */

#include "TeleOutput.h"
#include <string.h>
#include "OutputProcess.h"
#include "Fpga.h"
#include "IstateDti.h"
#include "CardLoop.h"

#define TRACE  TRACE_OUTPUT

CTeleOutput *CTeleOutput::s_pTeleOutput[NUM_TELE_OUTPUT_CHAN] = {NULL, NULL, NULL, NULL};
int CTeleOutput::m_currentLoop = -1;

CTeleOutput::CTeleOutput(int chan)
{
	m_chan = chan;
	m_warmupChoice = OUTPUT_CHOICE_OFF;
	m_freerunChoice = OUTPUT_CHOICE_OFF;
	m_holdoverChoice = OUTPUT_CHOICE_ON;
	m_fastTrackChoice = OUTPUT_CHOICE_ON;
	m_enabled = -1;
	m_valid = -1;
	m_frame = -1;
	m_t1Pulse = 0;
	m_crc = -1;
	//m_generation = -1;
	m_enableSsmOutput = -1;
	m_ssmBit = -1;
	// m_on = 0;
	m_bestTrackingLevel = 0;

	if ((chan >= 0) && (chan < NUM_TELE_OUTPUT_CHAN + 2)) {
		m_pOutputSwitchState = ((uint32 *)CIstateDti::de_outputSwitchState.pVariable) + chan;
	} else {
		m_pOutputSwitchState = NULL;
	}
}

CTeleOutput::~CTeleOutput()
{

}

int CTeleOutput::CreateTeleOutput()
{
	int i;
	int ret = 0;
	for (i = 0; i < NUM_TELE_OUTPUT_CHAN; i++) {
		if (s_pTeleOutput[i] == NULL) {
			s_pTeleOutput[i] = new CTeleOutput(i);
			if (s_pTeleOutput[i] != NULL) {
				ret++;
			}
		}
	}
	return ret;
}

void CTeleOutput::ReportOutputSwitchState(int on)
{
	if (m_pOutputSwitchState) {
		*m_pOutputSwitchState = on;
	}
}

void CTeleOutput::RefreshSetup(void)
{
	COutputProcess::s_pOutputProcess->ToSetupOutput();
}

int CTeleOutput::CfEnableChan(int enable)
{
	if (m_enabled == enable) return 0;
	m_enabled = enable;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfGenerationChan(int gen)
{ 
#if 0 // zmiao 1/19/2010: No more generation 
	if (m_generation == gen) return 0;
	m_generation = gen;
	RefreshSetup();
	return 1;
#else
	return 1;	
#endif	
}

int CTeleOutput::CfValidateChan(int valid)
{
	if (m_valid == valid) return 0;
	m_valid = valid;
	RefreshSetup();
	return 1;
}

int CTeleOutput::QuerySwitch(void)
{
	int ret = -1;

	if (m_valid == 0) {
		return OUTPUT_CHOICE_OFF;
	} else if (m_enabled == 0) {
		return OUTPUT_CHOICE_OFF;
	}
	switch (m_currentLoop) {
		case CCardLoop::CARD_FREQ_WARMUP:
			return m_warmupChoice;
		case CCardLoop::CARD_FREQ_FREERUN:
			return m_freerunChoice;
		case CCardLoop::CARD_FREQ_HOLDOVER:
		case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
			return m_holdoverChoice;
		case CCardLoop::CARD_FREQ_FAIL:
		case CCardLoop::CARD_FREQ_OFFLINE:
			return OUTPUT_CHOICE_OFF;
		case CCardLoop::CARD_FREQ_FAST_TRACK:
			return m_fastTrackChoice;
		case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		case CCardLoop::CARD_FREQ_BRIDGE:
			return OUTPUT_CHOICE_ON;
	}
	return OUTPUT_CHOICE_OFF;
#if 0 // change from generation to output choice
	switch (m_currentLoop) {
	case CCardLoop::CARD_FREQ_WARMUP:
		m_bestTrackingLevel = TRACK_LEVEL_WARMUP;
		if (m_generation <= CIstateDti::OUTPUT_GENERATION_WARMUP) {
			m_on = 1;
		} else {
			m_on = 0;
		}
		break;
	case CCardLoop::CARD_FREQ_FREERUN:
		m_bestTrackingLevel = TRACK_LEVEL_FREERUN;
		if (m_generation <= CIstateDti::OUTPUT_GENERATION_FREERUN) {
			m_on = 1;
		} else {
			m_on = 0;
		}
		break;
	case CCardLoop::CARD_FREQ_FAST_TRACK:
		if (m_bestTrackingLevel < TRACK_LEVEL_FAST_TRACK) {
			m_bestTrackingLevel = TRACK_LEVEL_FAST_TRACK;
		}
		if (m_generation <= CIstateDti::OUTPUT_GENERATION_FASTLOCK) {
			m_on = 1;
		} else if (m_bestTrackingLevel < TRACK_LEVEL_NORMAL_TRACK) {
			m_on = 0;
		} else {
			m_on = 1;
		}
		break;
	case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		m_bestTrackingLevel = TRACK_LEVEL_NORMAL_TRACK;
		if (m_generation <= CIstateDti::OUTPUT_GENERATION_NORMALLOCK) {
			m_on = 1;
		} else {
			m_on = 0;
		}
		break;
	case CCardLoop::CARD_FREQ_FAIL:
	case CCardLoop::CARD_FREQ_OFFLINE:
		m_bestTrackingLevel = 0;
		m_on = 0;
		break;
	default:
		if (m_generation <= CIstateDti::OUTPUT_GENERATION_FREERUN) {
			m_on = 1;
		}
		break;	
	}

	if (ret < 0) {
		ret = m_on;
	}
	// Further logic here
	return ret;
#endif
}

int CTeleOutput::CfOutputChoice(int state, int choice)
{
	switch (state) {
	case 0: 
		if (m_warmupChoice == choice) return 0;
		m_warmupChoice = choice;
		RefreshSetup();
		return 1;
	case 1:
		if (m_freerunChoice == choice) return 0;
		m_freerunChoice = choice;
		RefreshSetup();
		return 1;
	case 2:
		if (m_holdoverChoice == choice) return 0;
		m_holdoverChoice = choice;
		RefreshSetup();
		return 1;
	case 3:
		if (m_fastTrackChoice == choice) return 0;
		m_fastTrackChoice = choice;
		RefreshSetup();
		return 1;
	default:
		return -1;
	}

}

int CTeleOutput::CfGenerationTeleOutput(int chan , int gen)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfGenerationChan(gen);
}

int CTeleOutput::CfEnableTeleOutput(int chan, int enable)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfEnableChan(enable);	
}

int CTeleOutput::CfValidateTeleOutput(int chan, int valid)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfValidateChan(valid);
}

int CTeleOutput::CfFrameChan(int frame)
{
	if (m_frame == frame) return 0;
	m_frame = frame;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfFrameTeleOutput(int chan, int frame)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfFrameChan(frame);
}

int CTeleOutput::CfT1Pulse(int t1Pulse)
{
	if (m_t1Pulse == t1Pulse) return 0;
	m_t1Pulse = t1Pulse;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfT1PulseTeleOutput(int chan, int t1Pulse)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfT1Pulse(t1Pulse);
}

int CTeleOutput::CfSelCrc4Chan(int crc)
{
	if (m_crc == crc) return 0;
	m_crc = crc;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfSelCrc4TeleOutput(int chan, int crc)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfSelCrc4Chan(crc);
}

int CTeleOutput::CfEnableSsmChan(int enableSsm)
{
	if (m_enableSsmOutput == enableSsm) return 0;
	m_enableSsmOutput = enableSsm;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfEnableSsmTeleOutput(int chan, int enableSsm)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfEnableSsmChan(enableSsm);
}

int CTeleOutput::CfSsmBitChan(int ssmBit)
{
	if (m_ssmBit == ssmBit) return 0;
	m_ssmBit = ssmBit;
	RefreshSetup();
	return 1;
}

int CTeleOutput::CfSsmBitTeleOutput(int chan, int ssmBit)
{
	if ((chan < 0) || (chan >= NUM_TELE_OUTPUT_CHAN)) return -1;
	return s_pTeleOutput[chan]->CfSsmBitChan(ssmBit);
}

void CTeleOutput::SetupOutput(void)
{
	int i;
	CTeleOutput *pOutput;
	int on;
	int done;
	for (i = 0; i < NUM_TELE_OUTPUT_CHAN; i++) {
		done = 0;
		pOutput = s_pTeleOutput[i];
		on = pOutput->QuerySwitch();
		if (pOutput->m_t1Pulse) {
			if (on == OUTPUT_CHOICE_ON) {
				CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_T1_PULSE);
				pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
			} 
		} else if ((on == OUTPUT_CHOICE_ON) || (on == OUTPUT_CHOICE_AIS)) {
			switch (pOutput->m_frame) {
			case CIstateDti::SPAN_OUTPUT_FRAME_1544MHZ:
				if (on == OUTPUT_CHOICE_ON) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_1544MHZ);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} 
				break;
			case CIstateDti::SPAN_OUTPUT_FRAME_2048MHZ:
				if (on == OUTPUT_CHOICE_ON) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_2048MHZ);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} 
				break;
			case CIstateDti::SPAN_OUTPUT_FRAME_CCS:
				if (on == OUTPUT_CHOICE_ON) {
					if (pOutput->m_crc) {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_CCS_CRC4);
					} else {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_CCS_NCRC4);
					}
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} else if (on == OUTPUT_CHOICE_AIS) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_E1_AIS);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_AIS); done = 1;
				}
				break;
			case CIstateDti::SPAN_OUTPUT_FRAME_CAS:
				if (on == OUTPUT_CHOICE_ON) {
					if (pOutput->m_crc) {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_CAS_CRC4);
					} else {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_CAS_NCRC4);
					}
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} else if (on == OUTPUT_CHOICE_AIS) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_E1_AIS);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_AIS); done = 1;
				}
				break;
			case CIstateDti::SPAN_OUTPUT_FRAME_D4:
				if (on == OUTPUT_CHOICE_ON) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_D4);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} else if (on == OUTPUT_CHOICE_AIS) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_T1_AIS);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_AIS); done = 1;
				}
				break;
			case CIstateDti::SPAN_OUTPUT_FRAME_ESF:
				if (on == OUTPUT_CHOICE_ON) {
					if (pOutput->m_enableSsmOutput) {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_ESF);
					} else {
						CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_ESF_NSSM);
					}
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_ON); done = 1;
				} else if (on == OUTPUT_CHOICE_AIS) {
					CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_T1_AIS);
					pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_AIS); done = 1;
				}
				break;	
			default:
				CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_DISABLE);
				pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_OFF); done = 1;
				break;
			}

			if (pOutput->m_enableSsmOutput == 0) {
				CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_NO_SSM);
			} else {
				switch (pOutput->m_ssmBit) {
				case CIstateDti::SPAN_OUTPUT_SSM_SA4:
					CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_SA4);
					break;
				case CIstateDti::SPAN_OUTPUT_SSM_SA5:
					CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_SA5);
					break;
				case CIstateDti::SPAN_OUTPUT_SSM_SA6:
					CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_SA6);
					break;
				case CIstateDti::SPAN_OUTPUT_SSM_SA7:
					CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_SA7);
					break;
				case CIstateDti::SPAN_OUTPUT_SSM_SA8:
					CFpga::s_pFpga->SetE1SsmChoice(i, CFpga::FPGA_SPAN_OUTPUT_SA8);
					break;
				}
			}

		} 
		
		if (!done) {
			CFpga::s_pFpga->SetSpanOutput(i, CFpga::FPGA_SO_DISABLE);
			pOutput->ReportOutputSwitchState(OUTPUT_CHOICE_OFF);
			// TRACE("Output%d disabled\r\n", i);
		}
	}

}

void CTeleOutput::DoLoopChange(void)
{
	m_currentLoop = CCardLoop::s_pCardLoop->GetCardLoop();
	RefreshSetup();
}