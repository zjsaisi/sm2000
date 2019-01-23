// IstateDti.cpp: implementation of the CIstateDti class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateDti.cpp 1.29 2011/06/17 11:39:38PDT Dongsheng Zhang (dzhang) Exp  $
 * $Revision: 1.29 $
 */

#include "IocKeyList.h"
#include "IstateDti.h"
#include "string.h"
#include "IstateKbIoc.h"
#include "DtiOutput.h"
#include "Fpga.h"
#include "IstateProcess.h"
#include "errorcode.h"
#include "AllProcess.h"
#include "SecondProcess.h"
#include "Input.h"
#include "InputSelect.h"
#include "version.h"
#include "DtiOutputTrace.h"
#include "EventProcess.h"
#include "Warmup.h"
#include "CardStatus.h"
#include "RmtComm.h"
#include "NVRAM.h"
#include "MinuteProcess.h"
#include "BT3SUpport.h"
#include "CardLoop.h"
#include "machine.h"
#include "InputTod.h"   
#include "TeleOutput.h"
#include "TenMHz.h"
#include "OneHz.h"
#include "HouseProcess.h"
#include "Factory.h"
#include "CardAlign.h"

#define TRACE  TRACE_ISTATE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIstateDti *CIstateDti::s_pIstateDti = NULL;

SIstateItem const CIstateDti::m_istateList[] = {
	{COMM_RTE_KEY_SPAN_IO_SELECT, (void *)&de_spanIOSelect},
	{COMM_RTE_KEY_SPAN_OUTPUT_ENABLE, (void *)&de_spanOutputEnable},
	{COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE, (void *)&de_spanOutputFrameType},
	{COMM_RTE_KEY_SPAN_OUTPUT_CRC4, (void *)&de_spanOutputCrc4},
	{COMM_RTE_KEY_TEN_MHZ_ENABLE, (void *)&de_tenMHzEnable},
	{COMM_RTE_KEY_ONE_HZ_ENABLE, (void *)&de_oneHzEnable},
	{COMM_RTE_KEY_TEN_MHZ_GENERATION, (void *)&de_tenMHzGeneration},
	{COMM_RTE_KEY_ONE_HZ_GENERATION, (void *)&de_oneHzGeneration},
	{COMM_RTE_KEY_OUTPUT_CHOICE, (void *)&de_outputChoice },
	{COMM_RTE_KEY_SPAN_OUTPUT_GENERATION, (void *)&de_spanOutputGeneration},
	{COMM_RTE_KEY_SPAN_INPUT_PRIORITY, (void *)&de_spanInputPriority},
	{COMM_RTE_KEY_SPAN_INPUT_ENABLE, (void *)&de_spanInputEnable},
	{COMM_RTE_KEY_SPAN_INPUT_FRAME_TYPE, (void *)&de_spanInputFrameType},
	{COMM_RTE_KEY_SPAN_INPUT_CRC4, (void *)&de_spanInputCrc4},
	{COMM_RTE_KEY_SPAN_INPUT_SSM_BIT, (void *)&de_spanInputSsmBit},
	{COMM_RTE_KEY_SPAN_OUTPUT_SSM_ENABLE, (void *)&de_spanOutputSsmEnable},
	{COMM_RTE_KEY_SPAN_OUTPUT_SSM_BIT, (void *)&de_spanOutputSsmBit},
	{COMM_RTE_KEY_REFERENCE_SELECT_METHOD, (void *)&de_refSelectMethod},
	{COMM_RTE_KEY_MANUAL_INPUT, (void *)&de_manualInput},
	{COMM_RTE_KEY_ACTIVE_FREQUENCY_INPUT, (void *)&de_activeFrequencyInput},
	{COMM_RTE_KEY_SPAN_INPUT_SSM_READ_ENABLE, (void *)&de_spanInputSsmReadEnable},
	{COMM_RTE_KEY_SPAN_INPUT_ASSUMED_QUALITY_LEVEL, (void *)&de_spanInputAssumedQualityLevel},
	{COMM_RTE_KEY_SPAN_INPUT_CURRENT_QUALITY_LEVEL, (void *)&de_spanInputCurrentQualityLevel},
	{COMM_RTE_KEY_CURRENT_ADC_VALUE,	(void *)&de_currentAdcValue},
	{COMM_RTE_KEY_SPAN_OUTPUT_T1_PULSE, (void *)&de_spanOutputT1Pulse},
	{COMM_RTE_KEY_OSC_QUALITY_LEVEL, (void *) &de_oscQualityLevel },

	{COMM_RTE_KEY_SINGLE_ACTION, (void *)&de_singleAction },
	{COMM_RTE_KEY_COMMON_ACTION, (void *)&de_commonAction },
	{COMM_RTE_KEY_SYSTEM_MODE, (void *)&de_systemMode },
	{COMM_RTE_KEY_TOD_SOURCE,  (void *)&de_todSource },
	{COMM_RTE_KEY_PORT_TYPE,	(void *)&de_portType },
	{COMM_RTE_KEY_OUTPUT_STATE, (void *)&de_outputState },
	{COMM_RTE_KEY_OUTPUT_CABLE_ADVANCE, (void *)&de_outputCableAdvance },
	{COMM_RTE_KEY_OUTPUT_MODE, (void *)&de_outputMode },
	{COMM_RTE_KEY_INPUT_STATE, (void *)&de_inputState },
	{COMM_RTE_KEY_INPUT_PRIORITY, (void *)&de_inputPriority },
	{COMM_RTE_KEY_INPUT_DISQUALIFY_DELAY, (void *)&de_disqualifyDelay },
	{COMM_RTE_KEY_INPUT_QUALIFY_DELAY, (void *)&de_qualifyDelay },
	{COMM_RTE_KEY_CLOCK_LOOP_STATUS, (void *)&de_clockStatus },
	{COMM_RTE_KEY_CLIENT_STATUS, (void *)&de_clientStatus },
	{COMM_RTE_KEY_OUTPUT_CABLE_DELAY, (void *)&de_outputCableDelay },
	{COMM_RTE_KEY_TIME_ZONE, (void *)&de_timeZone },
	{COMM_RTE_KEY_USER_BRIDGER, (void *)&de_userBridge },
	{COMM_RTE_KEY_ACTIVE_INPUT, (void *)&de_activeInput },
	{COMM_RTE_KEY_COMPATIBILITY_ID, (void *)&de_compatibilityId },
	{COMM_RTE_KEY_TOTAL_LEAP_SECOND, (void *)&de_totalLeapSecond },
	{COMM_RTE_KEY_LEAP_PENDING, (void *)&de_leapPending },
	{COMM_RTE_KEY_OUTPUT_TEST, (void *)&de_outputTest },
	{COMM_RTE_KEY_PATH_TRACE_IP_FOUR, (void *)&de_pathTraceIpFour },
	{COMM_RTE_KEY_PATH_TRACE_OUTPUT_PORT, (void *)&de_pathTraceOutputPort },
	{COMM_RTE_KEY_PATH_TRACE_IP_SIX, (void *)&de_pathTraceIpSix },
	{COMM_RTE_KEY_HOLDOVER_TIME, (void *)&de_holdoverTime },
	{COMM_RTE_KEY_OUTPUT_TOD_MODE, (void *)&de_outputTodMode },
	{COMM_RTE_KEY_BT3_REPORT, (void *)&de_bt3Report },
	{COMM_RTE_KEY_OSC_READY, (void *)&de_oscReady },
	{COMM_RTE_KEY_CARD_STATUS, (void *)&de_cardStatus },
	{COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE, (void *)&de_okDownloadFirmware},
	{COMM_RTE_KEY_CLIENT_CLOCK_TYPE, (void *)&de_clientClockType},
	{COMM_RTE_KEY_CLIENT_DTI_VERSION, (void *)&de_clientDtiVersion},
	{COMM_RTE_KEY_OUTPUT_PHASE_ERROR, (void *)&de_outputPhaseError},
	{COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR, (void *)&de_inputPathRootIpFour},
	{COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX, (void *)&de_inputPathRootIpSix },
	{COMM_RTE_KEY_INPUT_PATH_ROOT_PORT, (void *)&de_inputPathRootPort },
	{COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION, (void *)&de_inputPathRootVersion },
	{COMM_RTE_KEY_INPUT_TOD_SETTING_MODE, (void *)&de_inputTodSettingMode },
	{COMM_RTE_KEY_INPUT_TOD_STATE, (void *)&de_inputTodState },
	{COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE, (void *)&de_inputServerClockType },
	{COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE, (void *)&de_inputServerExternalTimingSource },
	{COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS, (void *)&de_inputServerLoopStatus },
	{COMM_RTE_KEY_INPUT_CABLE_ADVANCE, (void *)&de_inputCableAdvance},
	{COMM_RTE_KEY_OUTPUT_CLIENT_TRANSITION_COUNT, (void *)&de_outputClientTransitionCount},
	{COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT, (void *)&de_ghostOutputClientTransitionCount },
	{COMM_RTE_KEY_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME, (void *)&de_outputClientNormalHoldoverTime },
	{COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME, (void *)&de_ghostOutputClientNormalHoldoverTime },
	{COMM_RTE_KEY_THIS_CLOCK_TYPE, (void *)&de_thisClockType },
	{COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE, (void *)&de_effectiveTodSource },
	{COMM_RTE_KEY_FPGA_VERSION, (void *)&de_fpgaVersion },
	{COMM_RTE_KEY_GPS_PHASE_ERROR, (void *)&de_gpsPhaseError },
	{COMM_RTE_KEY_IMC_COMMON_MARK, (void *)&de_imcCommonMark },
	{COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE, (void *)&de_activeOutputCableAdvance },
	{COMM_RTE_KEY_WARMUP_STATE, (void *)&de_warmupState },
	{COMM_RTE_KEY_SYSTEM_SSM, (void *)&de_systemSsm },
	{COMM_RTE_KEY_OUTPUT_SWITCH_STATE, (void *)&de_outputSwitchState},
	{COMM_RTE_KEY_WARMUP_TIME, (void *)&de_warmupTime },
	{COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL, (void *)&de_inputAssumedQualityLevel},

	// Add new one here. Ready to change istate version. IOC_ISTATE_VERSION
	{0, NULL}
};

CIstateDti::CIstateDti(CIstateKb *pKb)
{
	s_pIstateDti = NULL;
	AddIstateList(pKb, m_istateList);
}


CIstateDti::~CIstateDti()
{

}

static uint32 defaultOutputState[NUM_OUTPUT_CHAN] = {1, 1, 1, 1, 1,  
	1, 1, 1, 1, 1,		0, 0};

SEntryIstate const CIstateDti::de_outputState = { // COMM_RTE_KEY_OUTPUT_STATE
	"Output State (enable/disable)\r\n"
	"Index1: 0..11 for Port1.12\r\nIndex2: 0\r\nValue 1=Enable 0=Disable\r\n",
	FLAG_NULL, // IstateFlag flag;
	"OUTSTATE", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputState, // void *pVariable;	
	ActionOutputState, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultOutputState, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOutputState(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	pValue = (int32 *)(de_outputState.pVariable);
	if (index1 < 0) {
		// Apply to all channel
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			CDtiOutput::CfEnable(chan, pValue[chan]);
		}
		return;
	}
	CDtiOutput::CfEnable(index1, value);
}

SEntryIstate const CIstateDti::de_manualInput = { // COMM_RTE_KEY_MANUAL_INPUT
	"Manual Input Selection\r\n"
	"Index1: 0 Index2: 0\r\nValue \t0=BITS1 1=BITS2\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"MANINP", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_manualInput, // void *pVariable;	
	ActionManualInput, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionManualInput(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_manualInput.pVariable);
		value = *pValue;
	}
	
	value += CHAN_BITS1;
	CInputSelect::s_pInputSelect->CfManualInput(value);
	
}

SEntryIstate const CIstateDti::de_activeFrequencyInput = { // COMM_RTE_KEY_ACTIVE_FREQUENCY_INPUT
	"Current active frequency input\r\n"
	"Index1: 0 Index2: 0\r\nValue \t-1=NA 0=BITS1 1=BITS2\r\n",
	(IstateFlag)(FLAG_READ_ONLY_ISTATE), // IstateFlag flag;
	"ACTFINP", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_activeFrequencyInput, // void *pVariable;	
	NULL, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)-1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::PostActiveFrequencyInput(int input)
{
	CIstateKbIoc::m_istateInRam.ve_activeFrequencyInput = input;
}

SEntryIstate const CIstateDti::de_refSelectMethod = { // COMM_RTE_KEY_REFERENCE_SELECT_METHOD
	"Reference selection method\r\n"
	"Index1: 0 Index2: 0\r\nValue \t0=Manual 1=Priority 2=SSM\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"REFSEL", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_refSelectMethod, // void *pVariable;	
	ActionRefSelectMethod, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)REFERENCE_SELECT_SSM, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	(int32)(REFERENCE_SELECT_LAST - 1) // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionRefSelectMethod(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_refSelectMethod.pVariable);
		value = *pValue;
	}
	
	CInputSelect::s_pInputSelect->CfRefSelectMethod(value);
	
}

SEntryIstate const CIstateDti::de_spanIOSelect = { // COMM_RTE_KEY_SPAN_IO_SELECT
	"Span Input/Output select\r\n"
	"Index1: 0..1 for SPAN1..SPAN2 Index2: 0\r\nValue \t0=Input 1=Output\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SIOSEL", // char const * const name;
	NUM_DUAL_SPAN_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanIOSelect, // void *pVariable;	
	ActionSpanIOSelect, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanIOSelect(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanIOSelect.pVariable);
		for (chan = 0; chan < 2; chan++) {
			if (pValue[chan]) {
				CInput::s_pInput[CHAN_BITS1 + chan]->CfValidChanInput(0);
				CTeleOutput::CfValidateTeleOutput(chan, 1);
			} else {
				CTeleOutput::CfValidateTeleOutput(chan, 0);
				CInput::s_pInput[CHAN_BITS1 + chan]->CfValidChanInput(1);
			}
		}
		return;
	}
	if (value) {
		// Disable input.
		CInput::s_pInput[CHAN_BITS1 + index1]->CfValidChanInput(0);
		CTeleOutput::CfValidateTeleOutput(index1, 1);
	} else {
		CTeleOutput::CfValidateTeleOutput(index1, 0);
		// enable input. TBD
		CInput::s_pInput[CHAN_BITS1 + index1]->CfValidChanInput(1);
	}
}

SEntryIstate const CIstateDti::de_spanOutputEnable = { // COMM_RTE_KEY_SPAN_OUTPUT_ENABLE
	"Span Output Enable\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\nValue \t0=disable 1=enable\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOENABLE", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputEnable, // void *pVariable;	
	ActionSpanOutputEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputEnable.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfEnableTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfEnableTeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_spanOutputFrameType = { // COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE
	"Span Output Frame type\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\n"
	"Value \t0=1.5Mhz 1=2Mhz 2=CCS 3=CAS 4=D4 5=ESF\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOFRAME", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputFrameType, // void *pVariable;	
	ActionSpanOutputFrameType, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	SPAN_OUTPUT_FRAME_LAST -1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputFrameType(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputFrameType.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfFrameTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfFrameTeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_spanOutputT1Pulse = { // COMM_RTE_KEY_SPAN_OUTPUT_T1_PULSE
	"Enable T1 Pulse on Span Output\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\n"
	"Value \t0=Normal 1=T1 Pulse\r\n",
	(IstateFlag)(FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"T1PUL", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputT1Pulse, // void *pVariable;	
	ActionSpanOutputT1Pulse, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputT1Pulse(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputT1Pulse.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfT1PulseTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfT1PulseTeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_spanOutputCrc4 = { // COMM_RTE_KEY_SPAN_OUTPUT_CRC4
	"Span Output CRC4 choice\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\n"
	"Value \t0=No 1=CRC4\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOCRC4", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputCrc4, // void *pVariable;	
	ActionSpanOutputCrc4, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputCrc4(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputCrc4.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfSelCrc4TeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfSelCrc4TeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_spanOutputSsmEnable = { // COMM_RTE_KEY_SPAN_OUTPUT_SSM_ENABLE
	"Span Output SSM Enable\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\n"
	"Value \t0=No 1=SSM\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOSSMENB", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputSsmEnable, // void *pVariable;	
	ActionSpanOutputSsmEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputSsmEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputSsmEnable.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfEnableSsmTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfEnableSsmTeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_spanOutputSsmBit = { // COMM_RTE_KEY_SPAN_OUTPUT_SSM_BIT
	"Span Output SSM Bit\r\n"
	"Index1: 0..3 for SPAN1..SPAN4 Index2: 0\r\n"
	"Value \t4..8 for SA4..SA8\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOSSMBIT", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputSsmBit, // void *pVariable;	
	ActionSpanOutputSsmBit, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)SPAN_OUTPUT_SSM_SA4, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)SPAN_OUTPUT_SSM_SA4, // int32 minEntryValue; inclusive
	(int32)SPAN_OUTPUT_SSM_LAST - 1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputSsmBit(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputSsmBit.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfSsmBitTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfSsmBitTeleOutput(index1, value);
}

SEntryIstate const CIstateDti::de_tenMHzEnable = { // COMM_RTE_KEY_TEN_MHZ_ENABLE
	"10Mhz output enable/disable\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value \t0=disable 1=enable\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"TENMHZ", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_tenMHzEnable, // void *pVariable;	
	ActionTenMHzEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionTenMHzEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_tenMHzEnable.pVariable);
		value = *pValue;
	}
	CTenMHzOutput::s_pTenMHzOutput->CfEnableChan(value);
}

SEntryIstate const CIstateDti::de_oneHzEnable = { // COMM_RTE_KEY_ONE_HZ_ENABLE
	"1PPS output enable/disable\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value \t0=disable 1=enable\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"ONEHZ", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_oneHzEnable, // void *pVariable;	
	ActionOneHzEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOneHzEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_oneHzEnable.pVariable);
		value = *pValue;
	}
	COneHzOutput::s_pOneHzOutput->CfEnableChan(value);
}

SEntryIstate const CIstateDti::de_tenMHzGeneration = { // COMM_RTE_KEY_TEN_MHZ_GENERATION
	"10MHz output Generation Condition (obsolete)\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value \t0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMLOCK\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"TENMGEN", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_tenMHzGeneration, // void *pVariable;	
	ActionTenMHzGeneration, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)3, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x3L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionTenMHzGeneration(int16 index1, int16 index2, uint32 value)
{
#if 0 // zmiao 1/18/2010: obsolete in 1.2 release
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_tenMHzGeneration.pVariable);
		value = *pValue;
	}
	CTenMHzOutput::s_pTenMHzOutput->CfGenerationChan(value);
#endif
}

SEntryIstate const CIstateDti::de_oneHzGeneration = { // COMM_RTE_KEY_ONE_HZ_GENERATION
	"1PPS output Generation Condition (obsolete)\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value \t0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMLOCK\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"ONEHZGEN", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_oneHzGeneration, // void *pVariable;	
	ActionOneHzGeneration, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)3, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x3L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOneHzGeneration(int16 index1, int16 index2, uint32 value)
{
#if 0 // zmiao 1/18/2010: obsolete
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_oneHzGeneration.pVariable);
		value = *pValue;
	}
	COneHzOutput::s_pOneHzOutput->CfGenerationChan(value);
#endif
}

SEntryIstate const CIstateDti::de_currentAdcValue = { // COMM_RTE_KEY_CURRENT_ADC_VALUE
	"Current ADC value from hitachi\r\n"
	"Index1: 0=1V8 1=10MEFC 2=20MEFC 3=25MEFC\r\n"
	"4=24M5EFC 5=24M7EFC 6=OSC-CURR\r\n"
	"Value \tVoltage in mV Current in mA",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"CADCVAL", // char const * const name;
	NUM_REPORTED_ADC_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CHouseProcess::m_reportedAdc, // void *pVariable;	
	CIstateKb::ActionNull, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_spanInputPriority = { // COMM_RTE_KEY_SPAN_INPUT_PRIORITY
	"Span Input priority\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t1..5 1=high",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SIPRI", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputPriority, // void *pVariable;	
	ActionSpanInputPriority, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	1, // int32 minEntryValue; inclusive
	0x5L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputPriority(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputPriority.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfPriority(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfPriority(value);
}

SEntryIstate const CIstateDti::de_spanInputEnable = { // COMM_RTE_KEY_SPAN_INPUT_ENABLE
	"Span Input enable/disable\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t0=disable 1=enable",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SIENABLE", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputEnable, // void *pVariable;	
	ActionSpanInputEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputEnable.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfInputEnable(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfInputEnable(value);
}

SEntryIstate const CIstateDti::de_spanInputFrameType = { // COMM_RTE_KEY_SPAN_INPUT_FRAME_TYPE
	"Span Input Frame Type\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t0=1.544mhz 1=2Mhz 2=CCS 3=CAS 4=D4 5=ESF",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SIFRAME", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputFrameType, // void *pVariable;	
	ActionSpanInputFrameType, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x5L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputFrameType(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputFrameType.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfFrameChanInput(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfFrameChanInput(value);
}

SEntryIstate const CIstateDti::de_spanInputCrc4 = { // COMM_RTE_KEY_SPAN_INPUT_CRC4
	"Span Input Crc4 enable/disable\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t0=NO-Crc4 1=CRC4",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SICRC4", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputCrc4, // void *pVariable;	
	ActionSpanInputCrc4, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputCrc4(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputCrc4.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfCrc4ChanInput(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfCrc4ChanInput(value);
}

SEntryIstate const CIstateDti::de_spanInputSsmReadEnable = { // COMM_RTE_KEY_SPAN_INPUT_SSM_READ_ENABLE
	"Enable reading SSM from Span input if possible\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t0=disable 1=enable",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SISSMRD", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputSsmReadEnable, // void *pVariable;	
	ActionSpanInputSsmReadEnable, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputSsmReadEnable(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputSsmReadEnable.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfSsmReadChanInput(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfSsmReadChanInput(value);
}

SEntryIstate const CIstateDti::de_spanInputSsmBit = { // COMM_RTE_KEY_SPAN_INPUT_SSM_BIT
	"Span Input SSM BIT Selection\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t4..8 for SA4..SA8",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SISSMBIT", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputSsmBit, // void *pVariable;	
	ActionSpanInputSsmBit, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)SPAN_INPUT_SSM_SA4, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)SPAN_INPUT_SSM_SA4, // int32 minEntryValue; inclusive
	(int32)SPAN_INPUT_SSM_LAST - 1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanInputSsmBit(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanInputSsmBit.pVariable);
		for (chan = 0; chan < NUM_SPAN_INPUT; chan++) {
			CInput::s_pInput[CHAN_BITS1 + chan]->CfSsmBitChanInput(pValue[chan]);
		}
		return;
	}
	CInput::s_pInput[CHAN_BITS1+index1]->CfSsmBitChanInput(value);
}

/*this has been deprecated: don zhang, April 20, 2011:*/
SEntryIstate const CIstateDti::de_spanInputAssumedQualityLevel = {
	"Assumed SSM Quality level of Span Input\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t1..9",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SIASSM", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputAssumedQualityLevel, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)2, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)1, // int32 minEntryValue; inclusive
	(int32)9 // int32 maxEntryValue; inclusive
};




/*don zhang, April 20, 2011:*/
SEntryIstate const CIstateDti::de_inputAssumedQualityLevel = { // COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL
	"Assumed SSM Quality level of Input\r\n"
	"Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2\r\n"
	"Value \t1..9",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"INPSSM", // char const * const name;
	NUM_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputAssumedQualityLevel, // void *pVariable;	
	ActionInputAssumedQualityLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)2, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)1, // int32 minEntryValue; inclusive
	(int32)9 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputAssumedQualityLevel(int16 index1, int16 index2, uint32 value)
{
	int chan;
	int32 *pValue;

	if(index1 < 0) 
	{
		pValue = (int32 *)(de_inputAssumedQualityLevel.pVariable);
		for (chan = 0; chan < NUM_INPUT; chan++) 
		{
			CInput::s_pInput[chan]->CfAssumedQLChanInput(pValue[chan]);
		}

		return;
	}

	//0..4 for GPS(0), PORT11, PORT12, BITS1(3), BITS2(4)
	if(index1 < NUM_INPUT) 
	{
		// Send to input object
	    CInput::s_pInput[index1]->CfAssumedQLChanInput(value);
	}
}


SEntryIstate const CIstateDti::de_spanInputCurrentQualityLevel = { // COMM_RTE_KEY_SPAN_INPUT_CURRENT_QUALITY_LEVEL
	"Current SSM Quality level of Span Input\r\n"
	"Index1: 0..1 for INP1..INP2 Index2: 0\r\n"
	"Value \t1..9 15=NA",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE), // IstateFlag flag;
	"SICSSM", // char const * const name;
	NUM_SPAN_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanInputCurrentQualityLevel, // void *pVariable;	
	CIstateKb::ActionNull, // FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)9, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)1, // int32 minEntryValue; inclusive
	(int32)15 // int32 maxEntryValue; inclusive
};

void CIstateDti::PostCurrentQualityLevel(int chan, int level)
{
	if ((chan < 0) || (chan >= NUM_SPAN_INPUT)) return;
	if ((level <= 0) || (level >= 10)) {
		level = 15;
	}
	CIstateKbIoc::m_istateInRam.ve_spanInputCurrentQualityLevel[chan] = level;
}

int32 const CIstateDti::s_outputChoiceDefault[4][NUM_TELE_OUTPUT_CHAN+2] = {
	{CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF},
	{CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF, CTeleOutput::OUTPUT_CHOICE_OFF},
	{CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON},
	{CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON, CTeleOutput::OUTPUT_CHOICE_ON}
};
SEntryIstate const CIstateDti::de_outputChoice = { // COMM_RTE_KEY_OUTPUT_CHOICE
	"Output choice in states\r\n"
	"Index1: 0..3 for Span1..4 4=1PPS 5=10MHZ\r\nIndex2: 0=WARMUP 1=FREERUN 2=HOLDOVER 3=FASTTRACK\r\n"
	"Value: 0=Squelch 1=ON 2=AIS",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"OUTCHOICE", // char const * const name;
	NUM_TELE_OUTPUT_CHAN + 2, // int16 max_first_index;
	4, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputChoice, // void *pVariable;	
	ActionOutputChoice, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)s_outputChoiceDefault, //int32	 initParam;
	CheckOutputChoice, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x2L // int32 maxEntryValue; inclusive
};

int CIstateDti::CheckOutputChoice(int16 index1, int16 index2, int32 value, int16 cmd)
{
	if (value == CTeleOutput::OUTPUT_CHOICE_AIS) {
		if (index1 >= 4) {
			return ERROR_VALUE_OOR;
		}
	}
	return 1;
}

void CIstateDti::ActionOutputChoice(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		for (index1 = 0; index1 < NUM_TELE_OUTPUT_CHAN; index1++) {
			CTeleOutput::s_pTeleOutput[index1]->CfOutputChoice(0,CIstateKbIoc::m_istateInRam.ve_outputChoice[0][index1]); 
			CTeleOutput::s_pTeleOutput[index1]->CfOutputChoice(1,CIstateKbIoc::m_istateInRam.ve_outputChoice[1][index1]); 
			CTeleOutput::s_pTeleOutput[index1]->CfOutputChoice(2,CIstateKbIoc::m_istateInRam.ve_outputChoice[2][index1]); 
			CTeleOutput::s_pTeleOutput[index1]->CfOutputChoice(3,CIstateKbIoc::m_istateInRam.ve_outputChoice[3][index1]); 
		}
		COneHzOutput::s_pOneHzOutput->CfOutputChoice(0, CIstateKbIoc::m_istateInRam.ve_outputChoice[0][4]);
		COneHzOutput::s_pOneHzOutput->CfOutputChoice(1, CIstateKbIoc::m_istateInRam.ve_outputChoice[1][4]);
		COneHzOutput::s_pOneHzOutput->CfOutputChoice(2, CIstateKbIoc::m_istateInRam.ve_outputChoice[2][4]);
		COneHzOutput::s_pOneHzOutput->CfOutputChoice(3, CIstateKbIoc::m_istateInRam.ve_outputChoice[3][4]);
		CTenMHzOutput::s_pTenMHzOutput->CfOutputChoice(0, CIstateKbIoc::m_istateInRam.ve_outputChoice[0][5]);
		CTenMHzOutput::s_pTenMHzOutput->CfOutputChoice(1, CIstateKbIoc::m_istateInRam.ve_outputChoice[1][5]);
		CTenMHzOutput::s_pTenMHzOutput->CfOutputChoice(2, CIstateKbIoc::m_istateInRam.ve_outputChoice[2][5]);
		CTenMHzOutput::s_pTenMHzOutput->CfOutputChoice(3, CIstateKbIoc::m_istateInRam.ve_outputChoice[3][5]);
		return;
	}

	if ((index1 >= 0) && (index1 < NUM_TELE_OUTPUT_CHAN)) {
		CTeleOutput::s_pTeleOutput[index1]->CfOutputChoice(index2, value);
		return;
	}
	if (index1 == 4) {
		COneHzOutput::s_pOneHzOutput->CfOutputChoice(index2, value);
	} else if (index1 == 5) {
		CTenMHzOutput::s_pTenMHzOutput->CfOutputChoice(index2, value);
	}
}

SEntryIstate const CIstateDti::de_spanOutputGeneration = { // COMM_RTE_KEY_SPAN_OUTPUT_GENERATION
	"Span output Generation Condition (obsolete)\r\n"
	"Index1: 0..3 for Span1..4 Index2: 0\r\n"
	"Value \t0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMLOCK\r\n",
	(IstateFlag)(FLAG_NULL), // IstateFlag flag;
	"SOGEN", // char const * const name;
	NUM_TELE_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_spanOutputGeneration, // void *pVariable;	
	ActionSpanOutputGeneration, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)3, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x3L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSpanOutputGeneration(int16 index1, int16 index2, uint32 value)
{
#if 0 // zmiao 1/18/2010: Obsolete in 1.2 project
	int chan;
	int32 *pValue;

	if (index1 < 0) {
		pValue = (int32 *)(de_spanOutputGeneration.pVariable);
		for (chan = 0; chan < NUM_TELE_OUTPUT_CHAN; chan++) {
			CTeleOutput::CfGenerationTeleOutput(chan, pValue[chan]);
		}
		return;
	}
	CTeleOutput::CfGenerationTeleOutput(index1, value);
#endif
}

uint32 CIstateDti::ve_singleAction = NULL;
SEntryIstate const CIstateDti::de_singleAction = { // COMM_RTE_KEY_SINGLE_ACTION
	"Single action applies to this RTE\r\n"
	"Index1: 0 Index2: 0\r\nValue \t1=TOD set in FPGA\r\n"
	"\t2PPC is booting up\r\n"
	"\t3PPC is shutting down\r\n"
	,
	(IstateFlag)(FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"SNGACT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&ve_singleAction, // void *pVariable;	
	ActionSingle, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionSingle(int16 index1, int16 index2, uint32 value)
{
	if (index1 != 0) return;

	if ((value & 0xffffff00L) == 0x100L) { // Set time setting mode
		uint8 mode;
		mode = value & 0xff;
		CSecondProcess::s_pSecondProcess->SetTimeSettingMode(mode); 
		
		ve_singleAction = 0L;
		return;
	}

	switch (value) {
	case SINGLE_ACTION_TOD_SETTLED:
		TRACE("Istate: PPC jammed PPS to RTE\r\n");
		CFpga::s_pFpga->FakeJam();
		CSecondProcess::s_pSecondProcess->UpdateRtc();
		break;
	case SINGLE_ACTION_PPC_BOOT_UP:
		// During boot up, PPC reload FPGA. So RTE should reboot.
		CAllProcess::g_pAllProcess->GraceShutDown(2);
		break;
	case SINGLE_ACTION_PPC_SHUT_DOWN:
		CAllProcess::g_pAllProcess->CardOffline();
		CAllProcess::g_pAllProcess->GraceShutDown(10); // zmiao 7/11/2008: Suggested by Jining.
		break;
	case SINGLE_ACTION_FORCE_STANDBY: // transfer to the other guy.
		// The other has to be ready
		do {
			int twin, me;
			twin = CCardStatus::s_pCardStatus->GetTwinStatus();
			me = CCardStatus::GetCardStatus();
			if (twin == CCardStatus::CARD_HOT_STANDBY) {
				CFpga::s_pFpga->Transfer();
			} else if (me != CCardStatus::CARD_HOT_ACTIVE) {
				CFpga::s_pFpga->Transfer();
			}
		} while (0);
		break;
	case SINGLE_ACTION_MAKE_THIS_ACTIVE: // Make this card active
		SET_TWIN_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, SINGLE_ACTION_FORCE_STANDBY);
		break;
	case SINGLE_ACTION_MAKE_THIS_OFFLINE: // Make this card offline
		CAllProcess::g_pAllProcess->CardOffline();
		break;
	case SINGLE_ACTION_RESEND_ALARM_TO_PPC: // resend alarm to PPC
		CEventProcess::s_pEventProcess->ResendEventToImcTrigger();
		break;
	case SINGLE_ACTION_TURN_OFF_DEBUG_TUNNEL_AO: // Shut off AO debug message to PPC
		CRmtComm *pComm;
		pComm = CAllProcess::g_pAllProcess->GetRmtComm();
		pComm->SetAoAllowed(0L);
		break;
	case SINGLE_ACTION_AUTO_REBOOT:
		CMinuteProcess::s_pMinuteProcess->SetAutoReboot();
		break;
	case SINGLE_ACTION_TWIN_BOOTUP:
		CInputTod::ForceChange();
		CInput::TouchInput();
		break;
	default:
		TRACE("Invalid SINGLE action: %ld\r\n", value);
		break;
	}
	ve_singleAction = 0L;
}

SEntryIstate const CIstateDti::de_commonAction = { // COMM_RTE_KEY_COMMON_ACTION
	"Common action applies to both RTE\r\n"
	"Index1: 0 Index2: 0\r\nValue "
	"\t3=Save config now\r\n"
	"\t100=Sync RTC to gpsSec in FPGA\r\n"
	"\t101=Set TOD valid\r\n\t102=Calculate leap time\r\n"
	,
	(IstateFlag)(FLAG_VOLATILE_ISTATE | FLAG_AUTO_RESET_ISTATE), // IstateFlag flag;
	"COMMONACT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_commonAction, // void *pVariable;	
	ActionCommon, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	200 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionCommon(int16 index1, int16 index2, uint32 value)
{
	if (index1 != 0) return;
	switch (value) {
	case COMMON_ACTION_PPC_BOOT_UP: // PPC is booting up
		TRACE("PPC booting up. obsolete\r\n");
		break;
	case COMMON_ACTION_PPC_SHUT_DOWN: // PPC is shutting down
		TRACE("PPC shutting down. obsolete\r\n");
		break;
	case COMMON_ACTION_SAVE_CONFIG_NOW: { // Save config to flash now
		int ret;
		CNVRAM *pNvram;
		pNvram = CAllProcess::g_pAllProcess->GetNVRAM();
		ret = pNvram->SaveNVRAMCopy();
		TRACE("Saving config to flash %s\r\n", (ret == 1) ? ("Done") : ("Skipped") );
			}
		break;
	case COMMON_ACTION_SYNC_RTC_FROM_FPGA:
		CSecondProcess::s_pSecondProcess->UpdateRtc();
		TRACE("Sync RTC\r\n");
		break;
	case COMMON_ACTION_TOD_VALID_ON_STANDBY:
		if (CCardStatus::s_pCardStatus->IsCardActive() == 0) {
			TRACE("Set TOD valid\r\n");
			CSecondProcess::s_pSecondProcess->SetTodValid(1);
		}
		break;
	case COMMON_ACTION_CALCULATE_LEAP_TIME:
		CSecondProcess::s_pSecondProcess->CalculateLeapTime();
		break;
	case COMMON_ACTION_JAM_ARM: { int tod;
		tod = CIstateKbIoc::m_istateInRam.ve_todSource;
		if (tod == TOD_SOURCE_DTI) {
			TRACE("Unexpected Jam request from PPC\r\n");
		} else {
			CSecondProcess::s_pSecondProcess->ArmTodJam();
		}
			}
		break;
	default:
		TRACE("Invalid COMMON action %ld\r\n", value);
		break;
	}
}

SEntryIstate const CIstateDti::de_systemMode = { // COMM_RTE_KEY_SYSTEM_MODE
	"System Mode of Operation. Read-only for PPC\r\n"
	"Index1: 0 Index2: 0\r\nValue 0=ROOT 1=SERVER(SUBTEND)\r\n",
	(IstateFlag)(FLAG_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"SYSMODE", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_systemMode, // void *pVariable;	
	CIstateKb::ActionNull, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	NULL, // Init by CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

// Global object is enabled. 
CIstateDtiSpec CIstateDti::m_intfTodSource(COMM_RTE_KEY_TOD_SOURCE);

SEntryIstate const CIstateDti::de_todSource = { // COMM_RTE_KEY_TOD_SOURCE
	"TOD Source\r\n"
	"Index1: 0 Index2: 0\r\nValue \t0=GPS 1=USER/SYS\r\n"
	"\t2=NTP 3=PTP 4=UTI 5=RTC 6=HYBRID\r\n",
	FLAG_NULL, // IstateFlag flag;
	"TODSRC", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_todSource, // void *pVariable;	
	ActionTodSource, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	&m_intfTodSource, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	6 // int32 maxEntryValue; inclusive GPZ June 2010 allow upto hybrid mode
};

void CIstateDti::ActionTodSource(int16 index1, int16 index2, uint32 source)
{
	if (index1 < 0) {
		source = CIstateKbIoc::m_istateInRam.ve_todSource;
	} else {
		CFpga::s_pFpga->SetTodSource((int)source);
	}

	CSecondProcess::s_pSecondProcess->CfTodSource((int)source);
	CWarmup::s_pWarmup->CfWarmupTodSource((int)source);
	CCardLoop::s_pCardLoop->CfTodSource((int)source);
	CDtiOutputTrace::CfTodSource((int)source);
	CCardStatus::s_pCardStatus->CfTodSource((int)source);
	CInput::CfTodSource((int)source);
	CInputSelect::CfTodSource((int)source);
	CCardAlign::CfTodSource((int)source);

	switch ((int)source) {
	case TOD_SOURCE_HYBRID:  //GPZ June 2010 added hybrid case
	case TOD_SOURCE_GPS:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_PRR);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_ROOT;
		CDtiOutput::SetExternalTimeSource(1);
		CDtiOutput::SetServerHop(0);
		break;
	case TOD_SOURCE_DTI:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_PRR);//GPZ June 2010
//		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_SUBTEND);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_SUBTENDING;
		CDtiOutput::SetServerHop(1);
		break;
	case TOD_SOURCE_RTC:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_PRR);//GPZ June 2010
//		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_SSU);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_ROOT;
		CDtiOutput::SetExternalTimeSource(0);
		CDtiOutput::SetServerHop(0);
		break;
	case TOD_SOURCE_USER:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_PRR);//GPZ June 2010
//		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_SSU);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_ROOT;
		CDtiOutput::SetExternalTimeSource(0);
		CDtiOutput::SetServerHop(0);
		break;
	case TOD_SOURCE_NTP:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_SSU);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_ROOT;
		CDtiOutput::SetExternalTimeSource(0);
		CDtiOutput::SetServerHop(0);
		break;
	default:
		CBT3InBound::s_pBT3InBound->SetSysMode(CBT3InBound::BT3_SYS_MODE_SSU);
		CIstateKbIoc::m_istateInRam.ve_systemMode = SYSTEM_MODE_ROOT;
		CDtiOutput::SetExternalTimeSource(0);
		CDtiOutput::SetServerHop(0);
		break;
	}

	CBT3OutBound::s_pBT3OutBound->SetTodSource((int)source);
	CBT3InBound::SetTodSource((int)source);

	TRACE("Istate TOD Source is: %ld\r\n", source);
}

SEntryIstate const CIstateDti::de_oscQualityLevel = { // COMM_RTE_KEY_OSC_QUALITY_LEVEL
	"OSC Quality Level\r\n"
	"Index1: 0 Index2: 0\r\nValue:4 for Quartz, 3 for Rub \r\n"
	,
	FLAG_STANDALONE_ISTATE, // IstateFlag flag;
	"OSCQL", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_oscQualityLevel, // void *pVariable;	
	ActionOscQualityLevel, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	InitOscQualityLevel, //FUNC_INIT pInit;
	(int32)4, //int32	 initParam;
	CheckOscQualityLevel, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	9 // int32 maxEntryValue; inclusive
};

void CIstateDti::InitOscQualityLevel(void *pEntry)
{
	int32 *pVar;

	pVar = (int32 *)de_oscQualityLevel.pVariable;
	if (CFpga::s_pFpga->IsRubidium() == 1) {
		*pVar = 3;
	} else {
		// Get it from factory setting. 
		// 1/29/2009: zmiao: For Quartz card, always Type I.
		*pVar = 4; // CFactory::s_pFactory->GetOscQualityLevel();
	}
}

int CIstateDti::CheckOscQualityLevel(int16 index1, int16 index2, int32 value, int16 cmd)
{
	if (CFpga::s_pFpga->IsRubidium() == 1) {
		if (value != 3) return ERROR_VALUE_OOR;
		return 1;
	}
	// zmiao 1/29/2009: Gary J said For Quartz RTE, only Type I. 
	if ((value < 4) || (value > 4)) return ERROR_VALUE_OOR;
	//if ((value < 4) || (value > 7)) return ERROR_VALUE_OOR;
	return 1;
}

void CIstateDti::ActionOscQualityLevel(int16 index1, int16 index2, uint32 level)
{
	if (CFpga::s_pFpga->IsRubidium() != 1) {
		if (index1 < 0) {
			level = *(uint32 *)de_oscQualityLevel.pVariable;
		}
		CFactory::s_pFactory->SetOscQualityLevel(level);
		CAllProcess::SetOscQL(level);
		CInput::TouchInput();
	}
}

SEntryIstate const CIstateDti::de_portType = { // COMM_RTE_KEY_PORT_TYPE
	"Port11 & Port12 type \r\n"
	"Index1: 0..1 for Port11, PORT12 Index2: 0\r\n"
	"Value 0=SERVER  1=CLIENT\r\n",
	FLAG_NULL, // IstateFlag flag;
	"PORTTYPE", // char const * const name;
	NUM_DUAL_USE_PORT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_portType, // void *pVariable;	
	ActionPortType, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionPortType(int16 index1, int16 index2, uint32 value)
{
	uint32 *pType;
	pType = (uint32 *)de_portType.pVariable;
	if (index1 == -1) {
		CDtiOutput::CfValidate(10, ((int)pType[0] == PORT_TYPE_SERVER_OUTPUT)?(1):(0));
		CDtiOutput::CfValidate(11, ((int)pType[1] == PORT_TYPE_SERVER_OUTPUT)?(1):(0));
		CInput::s_pInput[CHAN_DTI1]->CfValidChanInput( ((int)pType[0] == PORT_TYPE_CLIENT_INPUT) ? (1) : (0));
		CInput::s_pInput[CHAN_DTI2]->CfValidChanInput( ((int)pType[1] == PORT_TYPE_CLIENT_INPUT) ? (1) : (0));
		return;
	}
	switch (index1) {
	case 0:
		CDtiOutput::CfValidate(10, ((int)value == PORT_TYPE_SERVER_OUTPUT)?(1):(0));
		CInput::s_pInput[CHAN_DTI1]->CfValidChanInput( ((int)value == PORT_TYPE_CLIENT_INPUT) ? (1) : (0));
		break;
	case 1:
		CDtiOutput::CfValidate(11, ((int)value == PORT_TYPE_SERVER_OUTPUT)?(1):(0));
		CInput::s_pInput[CHAN_DTI2]->CfValidChanInput( ((int)value == PORT_TYPE_CLIENT_INPUT) ? (1) : (0));
		break;
	}
}

SEntryIstate const CIstateDti::de_outputCableAdvance = { // COMM_RTE_KEY_OUTPUT_CABLE_ADVANCE
	"Output Cable Advance Length in meter\r\n"
	"Index1: 0..11 for Port1..12\r\nIndex2: 0\r\nValue: LSB per FPGA definition\r\n",
	FLAG_NULL, // IstateFlag flag;
	"OUTADV", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputCableAdvance, // void *pVariable;	
	ActionOutputCableAdvance, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOutputCableAdvance(int16 index1, int16 index2, uint32 value)
{

	if (index1 == -1) {
		uint32 *pAdvance;
		pAdvance = (uint32 *)de_outputCableAdvance.pVariable;
		for (int i = 0; i < NUM_OUTPUT_CHAN; i++) {
			CDtiOutput::CfCableAdvanceValue(i, pAdvance[i] /*ConvertMeterToLsb((int)pAdvance[i])*/);
		}
		return;
	}
	CDtiOutput::CfCableAdvanceValue(index1, value /*ConvertMeterToLsb((int)value)*/);
}


uint32 CIstateDti::ConvertMeterToLsb(int meter)
{
	uint32 lsb;

	lsb = (uint32)((double)meter / METER_PER_LSB + 0.5);
	return lsb;
}

SEntryIstate const CIstateDti::de_outputMode = { // COMM_RTE_KEY_OUTPUT_MODE
	"Output Cable Advance Mode\r\n"
	"Index1: 0..11 for Port1..12\r\nIndex2: 0\r\nValue 0=AUTO 1=MANUAL\r\n",
	FLAG_NULL, // IstateFlag flag;
	"OUTMODE", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputMode, // void *pVariable;	
	ActionOutputMode,  //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOutputMode(int16 index1, int16 index2, uint32 value)
{
	if (index1 == -1) {
		int chan;
		uint32 *pValue = (uint32 *)de_outputMode.pVariable;
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			CDtiOutput::CfCableAdvanceMode(chan, pValue[chan]);
		}
		return;
	}
	
	CDtiOutput::CfCableAdvanceMode(index1, value);
}

static int32 defaultInputState[NUM_INPUT] = { 0, 0, 0, 0, 0};
SEntryIstate const CIstateDti::de_inputState = { // COMM_RTE_KEY_INPUT_STATE
	"Input State\r\n"
	"Index1: 0..4 For GPS, PORT11, PORT12, BITS1, BITS2\r\n"
	"Index2: 0\r\nValue 1=enable  0=disable\r\n",
	FLAG_NULL, // IstateFlag flag;
	"INSTATE", // char const * const name;
	NUM_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputState, // void *pVariable;	
	ActionInputState, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultInputState, //int32	 initParam;
	CheckInputState, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputState(int16 index1, int16 index2, uint32 value)
{
	int chan;
	if (index1 < 0) {
		for (chan = 0; chan < NUM_INPUT; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_inputState[chan];
			// Send to input object
			CInput::s_pInput[chan]->CfInputEnable((int)value);
		}
	} else if (index1 < NUM_INPUT) {
		// Send to input object
		CInput::s_pInput[index1]->CfInputEnable((int)value);
	}
}

int CIstateDti::CheckInputState(int16 index1, int16 index2, int32 value, int16 cmd)
{
	int register todSource;

	if (cmd != CMD_WRITE_ISTATE_FROM_IMC) return 1;
	if (value == 0L) return 1;

	todSource = CIstateKbIoc::m_istateInRam.ve_todSource;
	if (todSource == TOD_SOURCE_DTI) {
		if ((index1 != CHAN_DTI1) && (index1 != CHAN_DTI2)) {
			return ERROR_VALUE_OOR;
		}
	} else {
		if ((index1 == CHAN_DTI1) || (index1 == CHAN_DTI2)) {
			return ERROR_VALUE_OOR;
		}
	}

	return 1;
}

static int32 defaultInputPriority[NUM_INPUT] = { 1, 2, 3, 4, 5};
SEntryIstate const CIstateDti::de_inputPriority = { // COMM_RTE_KEY_INPUT_PRIORITY
	"Input Priority\r\n"
	"Index1: 0..4 For GPS, PORT11, PORT12, BITS1, BITS2\r\n"
	"Index2: 0\r\nValue (high)1..16(low)\r\n",
	FLAG_NULL, // IstateFlag flag;
	"INPRI", // char const * const name;
	NUM_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputPriority, // void *pVariable;	
	ActionInputPriority, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultInputPriority, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	1, // int32 minEntryValue; inclusive
	16 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputPriority(int16 index1, int16 index2, uint32 value)
{	
	int chan;
	if (index1 < 0) {
		for (chan = 0; chan < NUM_INPUT; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_inputPriority[chan];
			// Send to input object
			CInput::s_pInput[chan]->CfPriority((int)value);
			
			#if 0 //GPZ June 2010 Special case for GPS channel. zmiao 8/13/2010: Not the ideal spot to do this.
			if (chan == CHAN_GPS) {
				CBT3InBound::s_pBT3InBound->SetInputPriority(chan, (int)value);     
				TRACE("GPZ June 2010 Set GPS Priority \r\n");
			} 
			#endif
			
		}
	} else if (index1 < NUM_INPUT) {
		// Send to input object
		CInput::s_pInput[index1]->CfPriority((int)value);
			#if 0 //GPZ June 2010 Special case for GPS channel. zmiao 8/13/2010: Not the ideal spot to do this.
			if (index1 == CHAN_GPS) {
				CBT3InBound::s_pBT3InBound->SetInputPriority(index1, value);     
				TRACE("GPZ June 2010 Set GPS Priority\r\n");
			} 
			#endif
		
	}
}

static uint32 defaultDisqualifyDelay[NUM_INPUT] = {10, 0, 0, 10, 10};
SEntryIstate const CIstateDti::de_disqualifyDelay = { // COMM_RTE_KEY_INPUT_DISQUALIFY_DELAY
	"Input Disqualify delay\r\n"
	"Index1: 0..4 For GPS, PORT11, PORT12, BITS1, BITS2\r\n"
	"Index2: 0\r\nValue 5..1000s or 0=DISABLE\r\n",
	FLAG_NULL, // IstateFlag flag;
	"DISQDELAY", // char const * const name;
	NUM_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_disqualifyDelay, // void *pVariable;	
	ActionDisqualifyDelay, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultDisqualifyDelay, //int32	 initParam;
	CheckDisqualifyDelay, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1000 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionDisqualifyDelay(int16 index1, int16 index2, uint32 value)
{
	int chan = index1;
	if (index1 < 0) {
		for (chan = 0; chan < NUM_INPUT; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_disqualifyDelay[chan];
			CInput::s_pInput[chan]->CfDisqDelay((int)value);
		}
	} else if (index1 < NUM_INPUT) {
		CInput::s_pInput[chan]->CfDisqDelay((int)value);
	}

}

int CIstateDti::CheckDisqualifyDelay(int16 index1, int16 index2, int32 value, int16 cmd)
{
	if ((value >= 1) && (value <= 4)) return ERROR_VALUE_OOR;
	if ((index1 == CHAN_DTI1) || (index1 == CHAN_DTI2)) {
		if (value != 0L) return ERROR_VALUE_OOR;
	}
	return 1;
}

static uint32 defaultQualifyDelay[NUM_INPUT] = {10, 0, 0, 10, 10};
SEntryIstate const CIstateDti::de_qualifyDelay = { // COMM_RTE_KEY_INPUT_QUALIFY_DELAY
	"Input Qualify delay\r\n"
	"Index1: 0..4 For GPS, PORT11, PORT12, BITS1, BITS2\r\n"
	"Index2: 0\r\nValue 5..1000s or 0=DISABLE\r\n",
	FLAG_NULL, // IstateFlag flag;
	"QUALDELAY", // char const * const name;
	NUM_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_qualifyDelay, // void *pVariable;	
	ActionQualifyDelay, //ActionEventAlarmLevel, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultQualifyDelay, //int32	 initParam;
	CheckQualifyDelay, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	1000 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionQualifyDelay(int16 index1, int16 index2, uint32 value)
{
	int chan;
	if (index1 < 0) {
		for (chan = 0; chan < NUM_INPUT; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_qualifyDelay[chan];
			// Send to input object
			CInput::s_pInput[chan]->CfQualDelay((int)value);
		}
	} else if (index1 < NUM_INPUT) {
		// Send to input object
		CInput::s_pInput[index1]->CfQualDelay((int)value);
	}
}

int CIstateDti::CheckQualifyDelay(int16 index1, int16 index2, int32 value, int16 cmd)
{
	if ((value >= 1) && (value <= 4)) return ERROR_VALUE_OOR;
	if ((index1 == CHAN_DTI1) || (index1 == CHAN_DTI2)) {
		if (value != 0L) return ERROR_VALUE_OOR;
	}
	return 1;
}

static uint32 ve_clockStatus = (uint32)CIstateDti::CLOCK_STAT_WARMUP;
SEntryIstate const CIstateDti::de_clockStatus = { // COMM_RTE_KEY_CLOCK_LOOP_STATUS
	"Clock status of this card. Read-only\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value \t1=WARMUP 2=FREERUN 3=FAST_TRACK 4=NORMAL_TRACK\r\n"
	"\t5=HOLDOVER 6=BRIDGE 7=FAIL 8=DISABLED(OFFLINE)\r\n"
	"\t9=EXTENDED_HOLDOVER\r\n"
	,
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLKSTAT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&ve_clockStatus, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	1, // int32 minEntryValue; inclusive
	9 // int32 maxEntryValue; inclusive
};

static uint32 ve_systemSsm = (uint32)2;
SEntryIstate const CIstateDti::de_systemSsm = { // COMM_RTE_KEY_SYSTEM_SSM
	"System SSM. Read-only\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value: 1..9\r\n"
	,
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"SYSSSM", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&ve_systemSsm, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //FUNC_INIT pInit;
	(int32)2, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	1, // int32 minEntryValue; inclusive
	9 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_clientStatus = { // COMM_RTE_KEY_CLIENT_STATUS
	"Client status. Read-only\r\n"
	"Index1: 0..11 for PORT1..PORT12 Index2: 0\r\n"
	"Value \t0=NA 1=WARMUP 2=FREERUN 3=FAST_TRACK\r\n"
	"\t4=NORMAL_TRACK 5=HOLDOVER 6=BRIDGE",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTSTAT", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_clientStatus, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0, // int32 minEntryValue; inclusive
	6 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_outputCableDelay = { // COMM_RTE_KEY_OUTPUT_CABLE_DELAY
	"Output Cable Length\r\n"
	"Index1: 0..11 for Port1..12\r\nIndex2: 0\r\nValue: cable length 1lsb=26ps\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CBLEN", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputCableDelay, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1000000L, // int32 minEntryValue; inclusive
	0x1000000L // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_timeZone = { // COMM_RTE_KEY_TIME_ZONE
	"Time Zone Setting\r\n"
	"Time Zone in second\r\n",
	FLAG_NULL, // IstateFlag flag;
	"TIMEZONE", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_timeZone, // void *pVariable;	
	ActionTimeZone, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	CheckTimeZone, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-46800L, // int32 minEntryValue; inclusive
	46800L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionTimeZone(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_timeZone;
	}
	CSecondProcess::s_pSecondProcess->CfLocalTimeZone(value);
	CSecondProcess::s_pSecondProcess->UpdateRtc();
}

int CIstateDti::CheckTimeZone(int16 index1, int16 index2, int32 value, int16 cmd)
{
	if (value % 60) {
		return ERROR_VALUE_OOR;
	}
	return 1;
}

SEntryIstate const CIstateDti::de_userBridge = { // COMM_RTE_KEY_USER_BRIDGER
	"User Bridge time in second\r\n"
	"Range: 100 .. 100000 or 0x7fffffff\r\n",
	FLAG_NULL, // IstateFlag flag;
	"BRGTIME", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_userBridge, // void *pVariable;	
	ActionUserBridge, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)900, //int32	 initParam;
	CheckUserBridge, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	100L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionUserBridge(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_userBridge;
	}
	// Send to state machine
	CCardLoop::s_pCardLoop->CfBridgeTime(value);
}

int CIstateDti::CheckUserBridge(int16 index1, int16 index2, int32 value, int16 cmd)
{
	// zmiao 8/23/2006: Gary J want it from 100 to 100,000
	if (value == 0x7fffffff) return 1;
	if (value > 100000) return ERROR_VALUE_OOR;
	if (value < 100L) return ERROR_VALUE_OOR;
	return 1;
}

SEntryIstate const CIstateDti::de_activeInput = { // COMM_RTE_KEY_ACTIVE_INPUT
	"Current Active Input\r\n"
	"Value: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2.  -1 for no active input\r\n",
	FLAG_IMC_READ_ONLY_ISTATE, // IstateFlag flag;
	"ACTINP", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_activeInput, // void *pVariable;	
	ActionActiveInput, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)-1, // done in Init() //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1L, // int32 minEntryValue; inclusive
	NUM_INPUT // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionActiveInput(int16 index1, int16 index2, uint32 value)
{
	int select;
	int index;

	if (index1 < 0) {
		select = CIstateKbIoc::m_istateInRam.ve_activeInput;
	} else {
		select = (int)value;
	}

	CInputSelect::s_pInputSelect->SetTwinSelect(select);
	if ((select == CHAN_DTI1) || (select == CHAN_DTI2)) {
		index = select - CHAN_DTI1;
		ActionInputPathRootIpFour(index, 0, 
			CIstateKbIoc::m_istateInRam.ve_inputPathRootIpFour[index]);
		ActionInputPathRootIpSix(3, index, 0L);
		ActionInputPathRootPort(index, 0, 
			CIstateKbIoc::m_istateInRam.ve_inputPathRootPort[index]);
		ActionInputPathRootVersion(index, 0, 
			CIstateKbIoc::m_istateInRam.ve_inputPathRootVersion[index]);

		ActionInputServerClockType(index, 0, 
			CIstateKbIoc::m_istateInRam.ve_inputServerClockType[index]);

		ActionInputServerExternalTimingSource(index, 0, 
			CIstateKbIoc::m_istateInRam.ve_inputServerExternalTimingSource[index]);
	}

}

uint32 CIstateDti::ve_compatibilityId;

SEntryIstate const CIstateDti::de_compatibilityId = { // COMM_RTE_KEY_COMPATIBILITY_ID
	"Compability Id\r\n",
	FLAG_READ_ONLY_ISTATE, // IstateFlag flag;
	"COMPID", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&ve_compatibilityId, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	InitCompatibilityId, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000UL, // int32 minEntryValue; inclusive
	0x7fffffffUL // int32 maxEntryValue; inclusive
};

void CIstateDti::InitCompatibilityId(void *ptr)
{
	// For the first release.
	ve_compatibilityId = COMPATIBILITY_ID;
}

SEntryIstate const CIstateDti::de_totalLeapSecond = { // COMM_RTE_KEY_TOTAL_LEAP_SECOND
	"Current Accumulative Leap Second\r\n",
	FLAG_NULL, // IstateFlag flag;
	"LEAPSEC", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_totalLeapSecond, // void *pVariable;	
	ActionTotalLeapSecond, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)14, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0L, // int32 minEntryValue; inclusive
	0xffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionTotalLeapSecond(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_totalLeapSecond;
	}
	CSecondProcess::s_pSecondProcess->CfTotalLeapSecond((int)value);
	CSecondProcess::s_pSecondProcess->UpdateRtc();
}

SEntryIstate const CIstateDti::de_leapPending = { // COMM_RTE_KEY_LEAP_PENDING
	"Pending Leap Second at the end of month\r\n"
	"Value 0=nothing 1=Positive Leap -1=negative leap\r\n",
	FLAG_NULL, // IstateFlag flag;
	"LEAPPEND", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_leapPending, // void *pVariable;	
	ActionLeapPending, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1L, // int32 minEntryValue; inclusive
	1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionLeapPending(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_leapPending;
	}
	CSecondProcess::s_pSecondProcess->CfLeapPending((int)value);
	CSecondProcess::s_pSecondProcess->UpdateRtc();
}

SEntryIstate const CIstateDti::de_outputTest = { // COMM_RTE_KEY_OUTPUT_TEST
	"Output Test Mode\r\nIndex1 0..11 for Port1..12.\r\nValue 1=test 0=normal",
	FLAG_NULL, // IstateFlag flag;
	"OUTTEST", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputTest, // void *pVariable;	
	ActionOutputTest, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0L, // int32 minEntryValue; inclusive
	1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOutputTest(int16 index1, int16 index2, uint32 value)
{
	int chan;

	if (index1 < 0) {
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_outputTest[chan];
			CDtiOutput::CfTestMode(chan, (int)value);
		}
		return;
	}
	CDtiOutput::CfTestMode(index1, (int)value);
}

SEntryIstate const CIstateDti::de_pathTraceIpFour = { // COMM_RTE_KEY_PATH_TRACE_IP_FOUR
	"IPV4 address for Path Traceability\r\n",
	FLAG_NULL, // IstateFlag flag;
	"PTIP4", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_pathTraceIpFour, // void *pVariable;	
	ActionPathTraceIpFour, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionPathTraceIpFour(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_pathTraceIpFour;
	}
	CDtiOutputTrace::CfSelfIpFour(value);
}

static int32 defaultPathTraceOutputPort[NUM_OUTPUT_CHAN] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
	};
SEntryIstate const CIstateDti::de_pathTraceOutputPort = { // COMM_RTE_KEY_PATH_TRACE_OUTPUT_PORT
	"Output port numbering for Path Traceability\r\n",
	FLAG_NULL, // IstateFlag flag;
	"PTOUTPORT", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_pathTraceOutputPort, // void *pVariable;	
	ActionPathTraceOutputPort, //FUNC_PTR pAction;
	CIstateKb::InitCopyIstate, //FUNC_INIT pInit;
	(int32)defaultPathTraceOutputPort, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	0xffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionPathTraceOutputPort(int16 index1, int16 index2, uint32 value)
{
	int chan = index1;
	if (index1 < 0) {
		for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
			value = CIstateKbIoc::m_istateInRam.ve_pathTraceOutputPort[chan];
			CDtiOutputTrace::CfSelfPortNumber(chan, (int)value);
		}
		return;
	}
	CDtiOutputTrace::CfSelfPortNumber(chan, (int)value);
}

SEntryIstate const CIstateDti::de_pathTraceIpSix = { // COMM_RTE_KEY_PATH_TRACE_IP_SIX
	"IPV6 address for Path Traceability.\r\n"
	"Writing to last double word will make it effective\r\n",
	FLAG_NULL, // IstateFlag flag;
	"PTIP6", // char const * const name;
	4, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_pathTraceIpSix, // void *pVariable;	
	ActionPathTraceIpSix, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionPathTraceIpSix(int16 index1, int16 index2, uint32 value)
{
	if ((index1 < 0) || (index1 == 3)) {
		CDtiOutputTrace::CfSelfIpSix((char *)CIstateKbIoc::m_istateInRam.ve_pathTraceIpSix);
	}
}

SEntryIstate const CIstateDti::de_holdoverTime = { // COMM_RTE_KEY_HOLDOVER_TIME
	"Maximum Holdover time before it transit into extended holdover\r\n",
	FLAG_NULL, // IstateFlag flag;
	"HOLDTIME", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_holdoverTime, // void *pVariable;	
	ActionHoldoverTime, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)(0x7fffffffL), //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionHoldoverTime(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_holdoverTime;
	}
	CCardLoop::s_pCardLoop->CfMaxHoldTime(value);
}

SEntryIstate const CIstateDti::de_outputTodMode = { // COMM_RTE_KEY_OUTPUT_TOD_MODE
	"Output Tod Mode.\r\nValue 0=Short 1=Verbose\r\n",
	FLAG_NULL, // IstateFlag flag;
	"OUTTODMOD", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_outputTodMode, // void *pVariable;	
	ActionOutputTodMode, //FUNC_PTR pAction;
	CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionOutputTodMode(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_outputTodMode;
	}
	CFpga::s_pFpga->CfOutputTodMode((int)value);
	CSecondProcess::s_pSecondProcess->CfOutputTodMode((int)value);
}

#undef HELP1 
#define HELP1 \
	"BT3 Report"\
	"struct MSG_BesTime_Engine_Report {\r\n"\
	"\tunsigned short LO_Weight;  //Current Weight of Local Oscillator\r\n"\
	"\tunsigned short SPA_Weight;  //Current Weight of Span-A\r\n"\
	"\tunsigned short SPB_Weight;  //Current Weight of Span-B\r\n"\
	"\tunsigned short PRR_Weight;  //Current Weight of PRR\r\n"\
	"M\r"

#undef HELP2
#define HELP2 \
	" \tlong LO_Prediction;  //Current Bridging Prediction in ns for local oscillator\r\n"\
	"\tlong SPA_Prediction;  //Current Bridging Prediction in ns for of Span-A channel\r\n"\
	"\tlong SPB_Prediction;  //Current Bridging Prediction in ns of Span-B channel\r\n"\
	"\tlong PRR_Prediction;  //Current Bridging Prediction in ns of PRR channel\r\n"\
	"\tlong TFOM;\t//Estimated (2sigma) of accuracy of 1 PPS output\r\n"\
	"M\r"

#undef HELP3
#define HELP3 \
	" \tlong GPS_mdev;\t//current modified allan standard deviation noise estimate for GPS\r\n"\
	"\tlong PA_mdev;\t//current estimate for ensemble channel 1 (either span or remote)\r\n"\
	"\tlong PB_mdev;\r\n"\
	"\tlong LO_mdev\r\n"\
	"\tunsigned short Backup_Reg;	//Backup assigments 0000 0 PRR SPB SPA\r\n"\
	"};\r\n"

typedef struct {     
	char const str1[sizeof(HELP1)];
	char const str2[sizeof(HELP2)];
	char const str3[sizeof(HELP3)];
} SBt3Help;    

static const SBt3Help bt3ReportHelp = {
	HELP1,
	HELP2,
	HELP3
};

SEntryIstate const CIstateDti::de_bt3Report = { // COMM_RTE_KEY_BT3_REPORT
	(const char *)&bt3ReportHelp,
	FLAG_READ_ONLY_ISTATE , // IstateFlag flag;
	"BT3RPT", // char const * const name;
	NUM_BT3_REPORT_WORD, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_bt3Report, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};


SEntryIstate const CIstateDti::de_oscReady = { // COMM_RTE_KEY_OSC_READY
	"OSC Ready\r\n 1=ready  0=OSC COLD",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"OSCRDY", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_oscReady, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	0x7fffffffL // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_cardStatus = { // COMM_RTE_KEY_CARD_STATUS
	"Card Status read-only for PPC.\r\n"
	"0=UNKNOWN 1=ACTIVE 2=STANDBY 3=FAILED 4=OFFLINE\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CRDSTAT", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_cardStatus, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	0x4L // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_okDownloadFirmware = { // COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE
	"Ok to download firmware\r\n"
	"0=not ready 1=OK\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"OKLOAD", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_okDownloadFirmware, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1, // Done in Init(); //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	0x1L // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_clientClockType = { // COMM_RTE_KEY_CLIENT_CLOCK_TYPE
	"Client Clock Type on output\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Value 0=TypeI 1=TypeII 2=TypeIII 3=ST3 4=Minimum\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTCLKTYPE", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_clientClockType, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() // CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)CLIENT_CLOCK_TYPE_UNKNOWN, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)CLIENT_CLOCK_TYPE_UNKNOWN // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_clientDtiVersion = { // COMM_RTE_KEY_CLIENT_DTI_VERSION
	"Client DTI version on output\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Value -1=unknown 0..0xff=version\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTVER", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_clientDtiVersion, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)-1, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1L, // int32 minEntryValue; inclusive
	(int32)0xff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_outputPhaseError = { // COMM_RTE_KEY_OUTPUT_PHASE_ERROR
	"Output Phase Error\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTPERR", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputPhaseError, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0x7fffffffL, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_inputPathRootIpFour = { // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR
	"Input Path IP Four\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"IPV4 info in path traceability\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"IPIP4", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputPathRootIpFour, // void *pVariable;	
	ActionInputPathRootIpFour, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputPathRootIpFour(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) return;
	if (CCardStatus::s_pCardStatus->IsCardActive() != 0) return;
	if ((index1 + CHAN_DTI1)!= CIstateKbIoc::m_istateInRam.ve_activeInput) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;

	CDtiOutputTrace::CfRootIpFour(value);

}

SEntryIstate const CIstateDti::de_inputPathRootIpSix = { // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX
	"Input Path IPV6 \r\n"
	"Index2: 0..1 for Port11..12\r\n"
	"IPV6 info in path traceability\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"IPIP6", // char const * const name;
	4, // int16 max_first_index;
	NUM_DTI_INPUT, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputPathRootIpSix, // void *pVariable;	
	ActionInputPathRootIpSix, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputPathRootIpSix(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) return;
	if (CCardStatus::s_pCardStatus->IsCardActive() != 0) return;
	if ((index2 + CHAN_DTI1)!= CIstateKbIoc::m_istateInRam.ve_activeInput) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;

	if (index1 != 3) return;
	CDtiOutputTrace::CfRootIpSix((char *)CIstateKbIoc::m_istateInRam.ve_inputPathRootIpSix[index2]);
}

SEntryIstate const CIstateDti::de_inputPathRootPort = { // COMM_RTE_KEY_INPUT_PATH_ROOT_PORT
	"Input Path Root port \r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Root port number in path traceability\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"IPPN", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputPathRootPort, // void *pVariable;	
	ActionInputPathRootPort, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputPathRootPort(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) return;
	if (CCardStatus::s_pCardStatus->IsCardActive() != 0) return;
	if ((index1 + CHAN_DTI1)!= CIstateKbIoc::m_istateInRam.ve_activeInput) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;
	
	CDtiOutputTrace::CfRootPortNumber((int)value);
}

SEntryIstate const CIstateDti::de_inputPathRootVersion = { // COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION
	"Input Path Root Version\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Root DTI version in path traceability\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"IPVER", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputPathRootVersion, // void *pVariable;	
	ActionInputPathRootVersion, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputPathRootVersion(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) return;
	if (CCardStatus::s_pCardStatus->IsCardActive() != 0) return;
	if ((index1 + CHAN_DTI1)!= CIstateKbIoc::m_istateInRam.ve_activeInput) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;

	CDtiOutputTrace::CfRootDtiVersion((int)value);
}

SEntryIstate const CIstateDti::de_inputTodSettingMode = { // COMM_RTE_KEY_INPUT_TOD_SETTING_MODE
	"Input Tod setting mode\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=Default 1=USER 2=NTP 3=GPS 16=unknown\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"INPTODMDE", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputTodSettingMode, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)16L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)16 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_inputTodState = { // COMM_RTE_KEY_INPUT_TOD_STATE
	"Input Tod state\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=invalid 1=valid 16=unknown\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"INPTODSTAT", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputTodState, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)16L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)16 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_inputServerClockType = { // COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE
	"Input Server clock type\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=ITU-I 1=ITU-II 2=ITU-III 3=ST3 16=Unknown\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"INPSVRCLK", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputServerClockType, // void *pVariable;	
	ActionInputServerClockType, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)16L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)16 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputServerClockType(int16 index1, int16 index2, uint32 value)
{
	int clock;

	if (index1 < 0) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;
	
	clock = (int)value;
	if ((index1 + CHAN_DTI1) == CIstateKbIoc::m_istateInRam.ve_activeInput) {
		CDtiOutput::SetClockType(clock);
	}
}

SEntryIstate const CIstateDti::de_inputServerExternalTimingSource = { // COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE
	"Input Server timing source\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=no external 1=GPS 16=Unknown\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"INPTIMSRC", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputServerExternalTimingSource, // void *pVariable;	
	ActionInputServerExternalTimingSource, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)16L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)16 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionInputServerExternalTimingSource(int16 index1, int16 index2, uint32 value)
{
	int timing;
	if (index1 < 0) return;
	if (CIstateKbIoc::m_istateInRam.ve_todSource != TOD_SOURCE_DTI) return;

	timing = (int)value;
	if ((index1 + CHAN_DTI1) == CIstateKbIoc::m_istateInRam.ve_activeInput) {
		CDtiOutput::SetExternalTimeSource(timing);
	}
}

SEntryIstate const CIstateDti::de_inputServerLoopStatus = { // COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS
	"Input Server Loop status\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=unknown 1=warmup 2=freerun 3=fast\r\n"
	"\t4=normal 5=holdover\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"INPSVRLOOP", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputServerLoopStatus, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	0x0L, // int32 minEntryValue; inclusive
	(int32)16 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_inputCableAdvance = { // COMM_RTE_KEY_INPUT_CABLE_ADVANCE
	"Input Cable Advance\r\n"
	"Index1: 0..1 for Port11..12\r\n"
	"Value: 0=unknown lsb=26ps\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"INPCBADV", // char const * const name;
	NUM_DTI_INPUT, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_inputCableAdvance, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
 	NULL, // CIstateIntf *pInterface;

	0x80000000L, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_outputClientTransitionCount = { // COMM_RTE_KEY_OUTPUT_CLIENT_TRANSITION_COUNT
	"Client Transition counter\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Index2: 0=T3  1=T4  2=T6  3=T7\r\n"
	"T3: Fast->Free  T4: Fast->Normal\r\n"
	"T6: Bridge->Normal  T7: Bridge->Holdover\r\n",
	(IstateFlag)(FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTFSMCNT", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	4, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputClientTransitionCount, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_ghostOutputClientTransitionCount = { // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT
	"Used by RTE to copy CLTFSMCNT to active RTE\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Index2: 0=T3  1=T4  2=T6  3=T7\r\n"
	"T3: Fast->Free  T4: Fast->Normal\r\n"
	"T6: Bridge->Normal  T7: Bridge->Holdover\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_SILENT_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"GFSMCNT", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	4, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_ghostOutputClientTransitionCount, // void *pVariable;	
	ActionGhostOutputClientTransitionCount, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionGhostOutputClientTransitionCount(int16 index1, int16 index2, uint32 value)
{
	uint8 tmp_exr;

	if ((index1 < 0) || (index2 < 0)) return;
	if (value != 0) {
		//TRACE("Ghost recved Index1=%d Index2=%d Value=%ld\r\n"
		//	, index1 , index2, value);
		if ((index1 < NUM_OUTPUT_CHAN) && (index2 < 4)) {
			tmp_exr = get_exr();
			set_exr(7);
			CIstateKbIoc::m_istateInRam.ve_outputClientTransitionCount[index2][index1]
				+= value;
			CIstateKbIoc::m_istateInRam.ve_ghostOutputClientTransitionCount[index2][index1] = 0L;
			set_exr(tmp_exr);
		}
	}
}

SEntryIstate const CIstateDti::de_outputClientNormalHoldoverTime = { // COMM_RTE_KEY_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME
	"Client Normal & Holdover duration\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Index2: 0=Normal  1=Holdover\r\n"
	"Value: Cycles of 10Khz clock\r\n",
	(IstateFlag)(FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLTNHTIME", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	2, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_outputClientNormalHoldoverTime, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_ghostOutputClientNormalHoldoverTime = { // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME
	"Use by RTE to update NH duration to active RTE\r\n"
	"Index1: 0..11 for Port1..12\r\n"
	"Index2: 0=Normal  1=Holdover\r\n"
	"Value: Cycles of 10Khz clock\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_SILENT_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"GCLTNHTIME", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	2, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_ghostOutputClientNormalHoldoverTime, // void *pVariable;	
	ActionGhostOutputClientNormalHoldoverTime, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionGhostOutputClientNormalHoldoverTime(int16 index1, int16 index2, uint32 value)
{
	uint8 tmp_exr;

	if ((index1 < 0) || (index2 < 0)) return;
	if (value == 0L) return;
	if ((index1 < NUM_OUTPUT_CHAN) && (index2 < 2)) {
		// TRACE("Ghost NH index1=%d index2=%d value=%ld\r\n", index1, index2, value);
		tmp_exr = get_exr();
		set_exr(7);
		CIstateKbIoc::m_istateInRam.ve_outputClientNormalHoldoverTime[index2][index1] += value;
		CIstateKbIoc::m_istateInRam.ve_ghostOutputClientNormalHoldoverTime[index2][index1] = 0L;
		set_exr(tmp_exr);
	}

}

SEntryIstate const CIstateDti::de_thisClockType = { // COMM_RTE_KEY_THIS_CLOCK_TYPE
	"Clock type of this server\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value: 0=ITU-I  1=ITU-II 2=ITU-III\r\n"
	"\t3=ST3\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"CLKTYPE", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_thisClockType, // void *pVariable;	
	ActionThisClockType, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionThisClockType(int16 index1, int16 index2, uint32 value)
{
	if (index1 < 0) return;
	TRACE("This clock type is %ld\r\n", value);
	
	// work fine with root server. But not subtend server.
	CDtiOutput::SetClockType(value);

	CDtiOutput::SetMyClockType(value);
}

// Global object is enabled. 
CIstateDtiSpec CIstateDti::m_intfEffectiveTodSource(COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE);

SEntryIstate const CIstateDti::de_effectiveTodSource = { // COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE
	"Effective TOD source on output\r\n"
	"Index1: 0 Index2: 0\r\n"
	"Value: 0=DEFAULT(RTC) 1=USER\r\n"
	"\t2=NTP 3=DTI 4=GPS\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"EFFTODSRC", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_effectiveTodSource, // void *pVariable;	
	ActionEffectiveTodSource, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	&m_intfEffectiveTodSource, // CIstateIntf *pInterface;

	(int32)0, // int32 minEntryValue; inclusive
	(int32)4 // int32 maxEntryValue; inclusive
};

void CIstateDti::ActionEffectiveTodSource(int16 index1, int16 index2, uint32 value)
{

	if (index1 < 0) {
		value = CIstateKbIoc::m_istateInRam.ve_effectiveTodSource;
	}

	TRACE("Effective Tod Source: %ld\r\n", value);

	if (IS_ACTIVE) return;

	if (CIstateProcess::s_pIstateProcess->IsTwinSameIstateVersion() == 1) { // Same version Istate
		CSecondProcess::s_pSecondProcess->ApplyTodSource((int)value);
	} else if (m_intfEffectiveTodSource.GotFromTwin(1) == 1) { // Different and both suppurt this.
		CSecondProcess::s_pSecondProcess->ApplyTodSource((int)value);
	} else {
		// In case active ioc doesn't support this Istate, write it myself.
		int todSource;
		todSource = CIstateKbIoc::m_istateInRam.ve_todSource;
		CIstateKbIoc::m_istateInRam.ve_effectiveTodSource = todSource;
		CSecondProcess::s_pSecondProcess->ApplyTodSource(todSource);		
	}

}

SEntryIstate const CIstateDti::de_fpgaVersion = { // COMM_RTE_KEY_FPGA_VERSION
	"FPGA version number\r\n"
	"Index1: 0 Index2: 0\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"FPGAVER", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_fpgaVersion, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0, // int32 minEntryValue; inclusive
	(int32)0 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_gpsPhaseError = { // COMM_RTE_KEY_GPS_PHASE_ERROR
	"GPS phase error again this 1PPS.\r\n"
	"1LSB=1 cylce of 10.24Mhz*256/35(13.35ns) clock\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"GPSPHERR", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_gpsPhaseError, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0, // int32 minEntryValue; inclusive
	(int32)0 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_outputSwitchState = { // COMM_RTE_KEY_OUTPUT_SWITCH_STATE
	"Output switch query(read-only) 1=ON 0=OFF\r\n"
	"Index1: 0..3 for Span1..4, 4 for 1PPS, 5 for 10Mhz\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"OUTSW", // char const * const name;
	NUM_TELE_OUTPUT_CHAN + 2, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_outputSwitchState[0], // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0, // int32 minEntryValue; inclusive
	(int32)2 // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_imcCommonMark = { // COMM_RTE_KEY_IMC_COMMON_MARK
	"Mark used by PPC. Shared between RTE\r\n",
	(IstateFlag)(FLAG_VOLATILE_ISTATE), // IstateFlag flag;
	"IMCMARK", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_imcCommonMark, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_activeOutputCableAdvance = { // COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE
	"Active Output Cable Advance\r\n"
	"Index1: 0..11 for Port1..12\r\nIndex2: 0\r\nValue: current cable advance in FPGA 1lsb=26ps\r\n",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"ACBADV", // char const * const name;
	NUM_OUTPUT_CHAN, // int16 max_first_index;
	1, // int16 max_second_index;
	CIstateKbIoc::m_istateInRam.ve_activeOutputCableAdvance, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)0, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	-1000000L, // int32 minEntryValue; inclusive
	0x1000000L // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_warmupState = { // COMM_RTE_KEY_WARMUP_STATE
	"Warmup State:\r\n"
	"1=Init 2=Wait Gear three 3=Wait Jam 4=Wait Gear four\r\n5=Complete 6=fail 7=offline",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"WARMSTATE", // char const * const name;
	1, // int16 max_first_index;
	1, // int16 max_second_index;
	&CIstateKbIoc::m_istateInRam.ve_warmupState, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

SEntryIstate const CIstateDti::de_warmupTime = { // COMM_RTE_KEY_WARMUP_TIME
	"Warmup Time:\r\n"
	"Current substate time & Total time in seconds",
	(IstateFlag)(FLAG_IMC_READ_ONLY_ISTATE | FLAG_STANDALONE_ISTATE), // IstateFlag flag;
	"WARMTIME", // char const * const name;
	2, // int16 max_first_index;
	1, // int16 max_second_index;
	CWarmup::m_timeSpentInWarmup, // void *pVariable;	
	CIstateKb::ActionNull, //FUNC_PTR pAction;
	NULL, // Done in Init() //CIstateKb::InitEntryIstate, //FUNC_INIT pInit;
	(int32)1000L, //int32	 initParam;
	NULL, // CheckFunc *pSpecialValueCheck;
	NULL, // CIstateIntf *pInterface;

	(int32)0x80000000, // int32 minEntryValue; inclusive
	(int32)0x7fffffff // int32 maxEntryValue; inclusive
};

// Should be the latest item for this class
void CIstateDti::Init()
{
	int i;

	CIstateKbIoc::m_istateInRam.ve_warmupState = 1;

	CIstateKbIoc::m_istateInRam.ve_activeInput = -1;
	CIstateKbIoc::m_istateInRam.ve_okDownloadFirmware = 1;
	for (i = 0; i < NUM_OUTPUT_CHAN; i++) {
		CIstateKbIoc::m_istateInRam.ve_clientClockType[i] = (int32)CLIENT_CLOCK_TYPE_UNKNOWN;
	}
	for (i = 0; i < NUM_OUTPUT_CHAN; i++) {
		CIstateKbIoc::m_istateInRam.ve_clientDtiVersion[i] = -1;
	}
	for (i = 0; i < NUM_OUTPUT_CHAN; i++) {
		CIstateKbIoc::m_istateInRam.ve_outputPhaseError[i] = 0x7fffffff;
	}
        
	// COMM_RTE_KEY_INPUT_TOD_SETTING_MODE
	CIstateKbIoc::m_istateInRam.ve_inputTodSettingMode[0] = 16L;
	CIstateKbIoc::m_istateInRam.ve_inputTodSettingMode[1] = 16L;

	// COMM_RTE_KEY_INPUT_TOD_STATE
	CIstateKbIoc::m_istateInRam.ve_inputTodState[0] = 16L;
	CIstateKbIoc::m_istateInRam.ve_inputTodState[1] = 16L;

	// COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE
	CIstateKbIoc::m_istateInRam.ve_inputServerClockType[0] = 16L;
	CIstateKbIoc::m_istateInRam.ve_inputServerClockType[1] = 16L;

	// COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE
	CIstateKbIoc::m_istateInRam.ve_inputServerExternalTimingSource[0] = 16L;
	CIstateKbIoc::m_istateInRam.ve_inputServerExternalTimingSource[1] = 16L;

	// COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS
	CIstateKbIoc::m_istateInRam.ve_inputServerLoopStatus[0] = 0L;
	CIstateKbIoc::m_istateInRam.ve_inputServerLoopStatus[1] = 0L;

	// COMM_RTE_KEY_THIS_CLOCK_TYPE
	CIstateKbIoc::m_istateInRam.ve_thisClockType = -1;

	// COMM_RTE_KEY_FPGA_VERSION
	CIstateKbIoc::m_istateInRam.ve_fpgaVersion = (uint32)CFpga::s_pFpga->GetVersion();
}



