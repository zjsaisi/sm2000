// IstateDti.h: interface for the CIstateDti class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateDti.h 1.16 2011/04/22 10:46:19PDT Dongsheng Zhang (dzhang) Exp  $
 * $Revision: 1.16 $
 */

#if !defined(DEFINE_ISTATE_VARIABLE)
#if !defined(AFX_ISTATEDTI_H__A8790F11_7850_4E76_AF1C_CFE43C06FA4C__INCLUDED_)
#define AFX_ISTATEDTI_H__A8790F11_7850_4E76_AF1C_CFE43C06FA4C__INCLUDED_

#include "IstateApp.h"
#include "IstateKb.h"
#include "IstateDtiSpec.h"

class Ccomm;

class CIstateDti : public CIstateApp  
{
public:
	void Init(void);
	CIstateDti(CIstateKb *pKb);
	virtual ~CIstateDti();
	static CIstateDti *s_pIstateDti;
	enum { SYSTEM_MODE_ROOT = 0, SYSTEM_MODE_SUBTENDING = 1 };
	enum { TOD_SOURCE_GPS = 0, 
		TOD_SOURCE_USER,
		TOD_SOURCE_NTP,
		TOD_SOURCE_PTP,
		TOD_SOURCE_DTI,
		TOD_SOURCE_RTC, // It's not used in TP5000 1/14/2008
		TOD_SOURCE_HYBRID // GPZ June 2010 add mode to use GPS for Time and Cesium for Frequency      
		// Hybrid should be just like system but use GPS for time 
		};
	enum { AUTO_MODE = 0, MANUAL_MODE = 1};
	enum { CLOCK_STAT_WARMUP = 1, CLOCK_STAT_FREERUN = 2, CLOCK_STAT_FAST_TRACK = 3,
		CLOCK_STAT_NORMAL_TRACK = 4, CLOCK_STAT_HOLDOVER = 5, CLOCK_STAT_BRIDGE = 6,
		CLOCK_STAT_FAIL = 7, CLOCK_STAT_OFFLINE = 8, CLOCK_STAT_EXTENDED_HOLDOVER = 9 };
	enum { CLIENT_STAT_NA = 0, CLIENT_STAT_WARMUP = 1, CLIENT_STAT_FREERUN = 2,
		CLIENT_STAT_FAST_TRACK = 3, CLIENT_STAT_NORMAL_TRACK = 4,
		CLIENT_STAT_HOLDOVER = 5, CLIENT_STAT_BRIDGE = 6 };
	enum {OUTPUT_TOD_SHORT = 0, OUTPUT_TOD_VERBOSE = 1};
	enum {ISTATE_CARD_ACTIVE = 1	,ISTATE_CARD_STANDBY = 2 // for COMM_RTE_KEY_CARD_STATUS
		 ,ISTATE_CARD_FAILED = 3	,ISTATE_CARD_OFFLINE = 4
	};
	enum {
		SINGLE_ACTION_TOD_SETTLED = 1,
		SINGLE_ACTION_PPC_BOOT_UP = 2,
		SINGLE_ACTION_PPC_SHUT_DOWN = 3,
		SINGLE_ACTION_RESEND_ALARM_TO_PPC = 4,
		SINGLE_ACTION_FORCE_STANDBY = 90,
		SINGLE_ACTION_MAKE_THIS_ACTIVE,
		SINGLE_ACTION_MAKE_THIS_OFFLINE,
		SINGLE_ACTION_TURN_OFF_DEBUG_TUNNEL_AO,
		SINGLE_ACTION_AUTO_REBOOT,
		SINGLE_ACTION_TWIN_BOOTUP}; // COMM_RTE_KEY_SINGLE_ACTION

		enum {
			COMMON_ACTION_PPC_BOOT_UP = 1, // void
			COMMON_ACTION_PPC_SHUT_DOWN, // void
			COMMON_ACTION_SAVE_CONFIG_NOW = 3,
			COMMON_ACTION_JAM_ARM = 4, // void
			COMMON_ACTION_SYNC_RTC_FROM_FPGA = 100, // used by RTE only
			COMMON_ACTION_TOD_VALID_ON_STANDBY, // used by RTE only
			COMMON_ACTION_CALCULATE_LEAP_TIME // used by RTE only
		}; // COMM_RTE_KEY_COMMON_ACTION

	enum {CLIENT_CLOCK_TYPE_ONE = 0, CLIENT_CLOCK_TYPE_TWO = 1, CLIENT_CLOCK_TYPE_THREE = 2,
		CLIENT_CLOCK_TYPE_ST3 = 3, CLIENT_CLOCK_TYPE_MINMUM = 4, 
		CLIENT_CLOCK_TYPE_UNKNOWN= 16 };

private:
	static SIstateItem const m_istateList[];
	enum { PORT_TYPE_SERVER_OUTPUT = 0, PORT_TYPE_CLIENT_INPUT = 1};

public: 
	// Special case.
	static SEntryIstate const de_outputSwitchState; // COMM_RTE_KEY_OUTPUT_SWITCH_STATE
	static SEntryIstate const de_gpsPhaseError; // COMM_RTE_KEY_GPS_PHASE_ERROR
	static SEntryIstate const de_bt3Report; // COMM_RTE_KEY_BT3_REPORT
	static SEntryIstate const de_outputPhaseError; // COMM_RTE_KEY_OUTPUT_PHASE_ERROR
	static SEntryIstate const de_inputTodSettingMode; // COMM_RTE_KEY_INPUT_TOD_SETTING_MODE
	static SEntryIstate const de_inputTodState; // COMM_RTE_KEY_INPUT_TOD_STATE
	static SEntryIstate const de_inputServerLoopStatus; // COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS
	static SEntryIstate const de_inputCableAdvance; // COMM_RTE_KEY_INPUT_CABLE_ADVANCE
	static SEntryIstate const de_outputClientTransitionCount; // COMM_RTE_KEY_OUTPUT_CLIENT_TRANSITION_COUNT
	static SEntryIstate const de_outputClientNormalHoldoverTime; // COMM_RTE_KEY_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME

private:
	static SEntryIstate const de_warmupState; // COMM_RTE_KEY_WARMUP_STATE
	static SEntryIstate const de_warmupTime; // COMM_RTE_KEY_WARMUP_TIME

	static SEntryIstate const de_activeOutputCableAdvance; // COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE

	static SEntryIstate const de_imcCommonMark; // COMM_RTE_KEY_IMC_COMMON_MARK

	static SEntryIstate const de_fpgaVersion; // COMM_RTE_KEY_FPGA_VERSION

	static CIstateDtiSpec m_intfEffectiveTodSource;
	static void ActionEffectiveTodSource(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_effectiveTodSource; // COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE

	static void ActionThisClockType(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_thisClockType; // COMM_RTE_KEY_THIS_CLOCK_TYPE

	static void ActionGhostOutputClientNormalHoldoverTime(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_ghostOutputClientNormalHoldoverTime; // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME
	static void ActionGhostOutputClientTransitionCount(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_ghostOutputClientTransitionCount; // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT

	static void ActionInputServerExternalTimingSource(int16 index1,  int16 index2, uint32 value);
	static SEntryIstate const de_inputServerExternalTimingSource; // COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE

	static void ActionInputServerClockType(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputServerClockType; // COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE

	static void ActionInputPathRootVersion(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputPathRootVersion; // COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION

	static void ActionInputPathRootPort(int16 index1, int16 index2,  uint32 value);
	static SEntryIstate const de_inputPathRootPort; // COMM_RTE_KEY_INPUT_PATH_ROOT_PORT

	static void ActionInputPathRootIpSix(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputPathRootIpSix; // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX

	static void ActionInputPathRootIpFour(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputPathRootIpFour; // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR

	static SEntryIstate const de_clientDtiVersion;  // COMM_RTE_KEY_CLIENT_DTI_VERSION

	static SEntryIstate const de_clientClockType; // COMM_RTE_KEY_CLIENT_CLOCK_TYPE
	
	static SEntryIstate const de_okDownloadFirmware; // COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE

	static SEntryIstate const de_cardStatus; // COMM_RTE_KEY_CARD_STATUS

	static SEntryIstate const de_oscReady; // COMM_RTE_KEY_OSC_READY

	static void ActionOutputTodMode(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputTodMode; // COMM_RTE_KEY_OUTPUT_TOD_MODE

	static void ActionHoldoverTime(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_holdoverTime; // COMM_RTE_KEY_HOLDOVER_TIME

	static void ActionPathTraceIpSix(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_pathTraceIpSix; // COMM_RTE_KEY_PATH_TRACE_IP_SIX

	static void ActionPathTraceOutputPort(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_pathTraceOutputPort; // COMM_RTE_KEY_PATH_TRACE_OUTPUT_PORT

	static void ActionPathTraceIpFour(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_pathTraceIpFour; // COMM_RTE_KEY_PATH_TRACE_IP_FOUR

	static void ActionOutputTest(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputTest; // COMM_RTE_KEY_OUTPUT_TEST

	static void ActionLeapPending(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_leapPending; // COMM_RTE_KEY_LEAP_PENDING
	
	static void ActionTotalLeapSecond(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_totalLeapSecond; // COMM_RTE_KEY_TOTAL_LEAP_SECOND

	static void InitCompatibilityId(void *ptr);
	static uint32 ve_compatibilityId;
	static SEntryIstate const de_compatibilityId; // COMM_RTE_KEY_COMPATIBILITY_ID

	static void ActionActiveInput(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_activeInput; // COMM_RTE_KEY_ACTIVE_INPUT

	static int CheckUserBridge(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionUserBridge(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_userBridge; // COMM_RTE_KEY_USER_BRIDGER

	static int CheckTimeZone(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionTimeZone(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_timeZone; // COMM_RTE_KEY_TIME_ZONE

	static SEntryIstate const de_outputCableDelay; // COMM_RTE_KEY_OUTPUT_CABLE_DELAY
	static SEntryIstate const de_clientStatus; // COMM_RTE_KEY_CLIENT_STATUS
	static SEntryIstate const de_clockStatus; // COMM_RTE_KEY_CLOCK_LOOP_STATUS
	
	static SEntryIstate const de_systemSsm; // COMM_RTE_KEY_SYSTEM_SSM

	static uint32 ConvertMeterToLsb(int meter);
	static void ActionOutputCableAdvance(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputCableAdvance; // COMM_RTE_KEY_OUTPUT_CABLE_ADVANCE

	static int CheckQualifyDelay(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionQualifyDelay(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_qualifyDelay; // COMM_RTE_KEY_INPUT_QUALIFY_DELAY
	
	static int CheckDisqualifyDelay(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionDisqualifyDelay(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_disqualifyDelay; // COMM_RTE_KEY_INPUT_DISQUALIFY_DELAY

	static int CheckInputState(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionInputState(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputState; // COMM_RTE_KEY_INPUT_STATE

	static void ActionInputPriority(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputPriority; // COMM_RTE_KEY_INPUT_PRIORITY

	static void ActionOutputMode(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputMode; // COMM_RTE_KEY_OUTPUT_MODE

	static void ActionPortType(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_portType; // COMM_RTE_KEY_PORT_TYPE

	static CIstateDtiSpec m_intfTodSource;
	static void ActionTodSource(int16 index1, int16 index2, uint32 source);
	static SEntryIstate const de_todSource; // COMM_RTE_KEY_TOD_SOURCE

	static void InitOscQualityLevel(void *pEntry);
	static int CheckOscQualityLevel(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionOscQualityLevel(int16 index1, int16 index2, uint32 level);
	static SEntryIstate const de_oscQualityLevel; // COMM_RTE_KEY_OSC_QUALITY_LEVEL

	static SEntryIstate const de_systemMode; // COMM_RTE_KEY_SYSTEM_MODE

	static void ActionCommon(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_commonAction; // COMM_RTE_KEY_COMMON_ACTION

	static uint32 ve_singleAction;
	static void ActionSingle(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_singleAction; // COMM_RTE_KEY_SINGLE_ACTION

	static void ActionOutputState(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputState; // COMM_RTE_KEY_OUTPUT_STATE


/******************************** TP5000 **********************/
	static void ActionSpanIOSelect(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanIOSelect; // COMM_RTE_KEY_SPAN_IO_SELECT

public:
	enum { REFERENCE_SELECT_MANUAL = 0, REFERENCE_SELECT_PRIORITY,
		REFERENCE_SELECT_SSM,
		REFERENCE_SELECT_LAST };
private:
	static void ActionRefSelectMethod(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_refSelectMethod; // COMM_RTE_KEY_REFERENCE_SELECT_METHOD

	static void ActionManualInput(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_manualInput; // COMM_RTE_KEY_MANUAL_INPUT

public:
	static void PostActiveFrequencyInput(int input);
private:
	static SEntryIstate const de_activeFrequencyInput; // COMM_RTE_KEY_ACTIVE_FREQUENCY_INPUT

	static void ActionSpanOutputEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputEnable; // COMM_RTE_KEY_SPAN_OUTPUT_ENABLE

public:
	enum {SPAN_OUTPUT_FRAME_1544MHZ, SPAN_OUTPUT_FRAME_2048MHZ,
		SPAN_OUTPUT_FRAME_CCS, SPAN_OUTPUT_FRAME_CAS, SPAN_OUTPUT_FRAME_D4,
		SPAN_OUTPUT_FRAME_ESF,
		SPAN_OUTPUT_FRAME_LAST};

private:
	static void ActionSpanOutputFrameType(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputFrameType; // COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE

	static void ActionSpanOutputCrc4(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputCrc4; //COMM_RTE_KEY_SPAN_OUTPUT_CRC4

	static void ActionSpanOutputSsmEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputSsmEnable; // COMM_RTE_KEY_SPAN_OUTPUT_SSM_ENABLE

public:
	enum {SPAN_OUTPUT_SSM_SA4 = 4, SPAN_OUTPUT_SSM_SA5,
		SPAN_OUTPUT_SSM_SA6, SPAN_OUTPUT_SSM_SA7,
		SPAN_OUTPUT_SSM_SA8, 
		SPAN_OUTPUT_SSM_LAST };
private:
	static void ActionSpanOutputSsmBit(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputSsmBit; // COMM_RTE_KEY_SPAN_OUTPUT_SSM_BIT

	static void ActionTenMHzEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_tenMHzEnable; // COMM_RTE_KEY_TEN_MHZ_ENABLE

	static void ActionOneHzEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_oneHzEnable; // COMM_RTE_KEY_ONE_HZ_ENABLE

private:
	static int32 const s_outputChoiceDefault[4][NUM_TELE_OUTPUT_CHAN+2];
	static int CheckOutputChoice(int16 index1, int16 index2, int32 value, int16 cmd);
	static void ActionOutputChoice(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_outputChoice; // COMM_RTE_KEY_OUTPUT_CHOICE

public:
	enum { OUTPUT_GENERATION_WARMUP = 0, OUTPUT_GENERATION_FREERUN,
		OUTPUT_GENERATION_FASTLOCK, OUTPUT_GENERATION_NORMALLOCK };
private:
	static void ActionTenMHzGeneration(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_tenMHzGeneration; // COMM_RTE_KEY_TEN_MHZ_GENERATION

	static void ActionOneHzGeneration(int16 index1, int16 index2, uint32 value32);
	static SEntryIstate const de_oneHzGeneration; // COMM_RTE_KEY_ONE_HZ_GENERATION

	static void ActionSpanOutputGeneration(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputGeneration; // COMM_RTE_KEY_SPAN_OUTPUT_GENERATION

	static void ActionSpanInputPriority(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputPriority; // COMM_RTE_KEY_SPAN_INPUT_PRIORITY

	static void ActionSpanInputEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputEnable; // COMM_RTE_KEY_SPAN_INPUT_ENABLE

	static SEntryIstate const de_currentAdcValue; // COMM_RTE_KEY_CURRENT_ADC_VALUE

public:
	enum {SPAN_INPUT_FRAME_1544MHZ, SPAN_INPUT_FRAME_2MHZ,
		SPAN_INPUT_FRAME_CCS, SPAN_INPUT_FRAME_CAS, 
		SPAN_INPUT_FRAME_D4, SPAN_INPUT_FRAME_ESF};
private:
	static void ActionSpanInputFrameType(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputFrameType; // COMM_RTE_KEY_SPAN_INPUT_FRAME_TYPE

	static void ActionSpanInputCrc4(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputCrc4; // COMM_RTE_KEY_SPAN_INPUT_CRC4

	static void ActionSpanInputSsmReadEnable(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputSsmReadEnable; // COMM_RTE_KEY_SPAN_INPUT_SSM_READ_ENABLE
public:
	// SAx is supposed to be continuous
	enum { SPAN_INPUT_SSM_SA4 = 4, SPAN_INPUT_SSM_SA5,
		SPAN_INPUT_SSM_SA6, SPAN_INPUT_SSM_SA7,
		SPAN_INPUT_SSM_SA8,
		SPAN_INPUT_SSM_LAST
	};
private:
	static void ActionSpanInputSsmBit(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputSsmBit; // COMM_RTE_KEY_SPAN_INPUT_SSM_BIT

	static void ActionInputAssumedQualityLevel(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_inputAssumedQualityLevel; // COMM_RTE_KEY_SPAN_INPUT_ASSUMED_QUALITY_LEVEL

	static void ActionSpanInputAssumedQualityLevel(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanInputAssumedQualityLevel; // COMM_RTE_KEY_SPAN_INPUT_ASSUMED_QUALITY_LEVEL

public: static void PostCurrentQualityLevel(int chan, int level);
private: static SEntryIstate const de_spanInputCurrentQualityLevel; // COMM_RTE_KEY_SPAN_INPUT_CURRENT_QUALITY_LEVEL

private:
	static void ActionSpanOutputT1Pulse(int16 index1, int16 index2, uint32 value);
	static SEntryIstate const de_spanOutputT1Pulse; // COMM_RTE_KEY_SPAN_OUTPUT_T1_PULSE

};

#endif // !defined(AFX_ISTATEDTI_H__A8790F11_7850_4E76_AF1C_CFE43C06FA4C__INCLUDED_)

#elif defined(ISTATE_SHARE_FLASH)  // defined(DEFINE_ISTATE_VARIABLE)
// Here : Define data shared and non-volatile item in Istate
	uint32 ve_todSource; // COMM_RTE_KEY_TOD_SOURCE
	uint32 ve_spanIOSelect[NUM_DUAL_SPAN_CHAN]; // COMM_RTE_KEY_SPAN_IO_SELECT
	uint32 ve_spanOutputEnable[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_ENABLE
	uint32 ve_spanOutputFrameType[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE
	uint32 ve_spanOutputCrc4[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_CRC4
	uint32 ve_tenMHzEnable; // COMM_RTE_KEY_TEN_MHZ_ENABLE
	uint32 ve_oneHzEnable;	 // COMM_RTE_KEY_ONE_HZ_ENABLE
	uint32 ve_tenMHzGeneration; // COMM_RTE_KEY_TEN_MHZ_GENERATION
	uint32 ve_oneHzGeneration; // COMM_RTE_KEY_ONE_HZ_GENERATION
	uint32 ve_spanOutputGeneration[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_GENERATION
	uint32 ve_spanInputPriority[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_PRIORITY
	uint32 ve_spanInputEnable[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_ENABLE
	uint32 ve_spanInputFrameType[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_FRAME_TYPE
	uint32 ve_spanInputCrc4[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_CRC4
	uint32 ve_spanInputSsmBit[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_SSM_BIT
	uint32 ve_spanOutputSsmEnable[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_SSM_ENABLE
	uint32 ve_spanOutputSsmBit[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_SSM_BIT
	uint32 ve_refSelectMethod; // COMM_RTE_KEY_REFERENCE_SELECT_METHOD
	uint32 ve_manualInput; // COMM_RTE_KEY_MANUAL_INPUT
	uint32 ve_spanInputSsmReadEnable[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_SSM_READ_ENABLE
	uint32 ve_spanInputAssumedQualityLevel[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_ASSUMED_QUALITY_LEVEL, obsoleted,4/20/2011

	uint32 ve_portType[NUM_DUAL_USE_PORT];  // COMM_RTE_KEY_PORT_TYPE
	uint32 ve_outputState[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_STATE
	uint32 ve_outputMode[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_MODE
	uint32 ve_inputState[NUM_INPUT];	// COMM_RTE_KEY_INPUT_STATE
	uint32 ve_inputPriority[NUM_INPUT];  // COMM_RTE_KEY_INPUT_PRIORITY
	uint32 ve_disqualifyDelay[NUM_INPUT]; // COMM_RTE_KEY_INPUT_DISQUALIFY_DELAY
	uint32 ve_qualifyDelay[NUM_INPUT]; // COMM_RTE_KEY_INPUT_QUALIFY_DELAY
	uint32 ve_outputCableAdvance[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_CABLE_ADVANCE
	int32	ve_timeZone;		// COMM_RTE_KEY_TIME_ZONE
	int32	ve_userBridge;		// COMM_RTE_KEY_USER_BRIDGER
	int32	ve_totalLeapSecond; // COMM_RTE_KEY_TOTAL_LEAP_SECOND
	int32	ve_leapPending;		// COMM_RTE_KEY_LEAP_PENDING
	int32	ve_outputTest[NUM_OUTPUT_CHAN];		// COMM_RTE_KEY_OUTPUT_TEST
	uint32	ve_pathTraceIpFour;		// COMM_RTE_KEY_PATH_TRACE_IP_FOUR
	int32	ve_pathTraceOutputPort[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_PATH_TRACE_OUTPUT_PORT
	uint32	ve_pathTraceIpSix[4]; // COMM_RTE_KEY_PATH_TRACE_IP_SIX
	int32	ve_holdoverTime;	// COMM_RTE_KEY_HOLDOVER_TIME
	int32	ve_outputTodMode;	// COMM_RTE_KEY_OUTPUT_TOD_MODE
	uint32  ve_outputChoice[4][NUM_TELE_OUTPUT_CHAN + 2]; // COMM_RTE_KEY_OUTPUT_CHOICE
	uint32  ve_inputAssumedQualityLevel[NUM_INPUT]; // COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL

#elif defined(ISTATE_SHARE_VOLATILE)
// Here : Defined shared and volatile Istate items
	// TP5000
	uint32 ve_spanOutputT1Pulse[NUM_TELE_OUTPUT_CHAN]; // COMM_RTE_KEY_SPAN_OUTPUT_T1_PULSE

	uint32 ve_commonAction; // COMM_RTE_KEY_COMMON_ACTION
	int32 ve_activeInput; // COMM_RTE_KEY_ACTIVE_INPUT
	uint32 ve_inputPathRootIpFour[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR
	uint32 ve_inputPathRootIpSix[NUM_DTI_INPUT][4]; // COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX
	uint32 ve_inputPathRootPort[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_PATH_ROOT_PORT
	uint32 ve_inputPathRootVersion[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION
	int32 ve_inputServerClockType[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE
	int32 ve_inputServerExternalTimingSource[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE
	int32 ve_effectiveTodSource; // COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE
	int32 ve_imcCommonMark;		// COMM_RTE_KEY_IMC_COMMON_MARK
#else
// Here not shared.
	// TP5000
	int32 ve_activeFrequencyInput; // COMM_RTE_KEY_ACTIVE_FREQUENCY_INPUT
	int32 ve_spanInputCurrentQualityLevel[NUM_SPAN_INPUT]; // COMM_RTE_KEY_SPAN_INPUT_CURRENT_QUALITY_LEVEL
	int32 ve_oscQualityLevel; // COMM_RTE_KEY_OSC_QUALITY_LEVEL

	uint32 ve_systemMode;	// COMM_RTE_KEY_SYSTEM_MODE
	uint32 ve_clientStatus[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_CLIENT_STATUS
	uint32 ve_outputCableDelay[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_CABLE_DELAY
	uint32 ve_bt3Report[NUM_BT3_REPORT_WORD]; // COMM_RTE_KEY_BT3_REPORT
	uint32 ve_oscReady; // COMM_RTE_KEY_OSC_READY
	uint32 ve_cardStatus; // COMM_RTE_KEY_CARD_STATUS
	uint32 ve_okDownloadFirmware; // COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE
	int32  ve_clientClockType[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_CLIENT_CLOCK_TYPE
	int32  ve_clientDtiVersion[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_CLIENT_DTI_VERSION
	int32  ve_outputPhaseError[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_PHASE_ERROR
	int32 ve_inputTodSettingMode[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_TOD_SETTING_MODE
	int32 ve_inputTodState[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_TOD_STATE
	int32 ve_inputServerLoopStatus[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS
	int32 ve_inputCableAdvance[NUM_DTI_INPUT]; // COMM_RTE_KEY_INPUT_CABLE_ADVANCE
	int32 ve_outputClientTransitionCount[4][NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_CLIENT_TRANSITION_COUNT
	int32 ve_ghostOutputClientTransitionCount[4][NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT
	uint32 ve_outputClientNormalHoldoverTime[2][NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME
	uint32 ve_ghostOutputClientNormalHoldoverTime[2][NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME
	int32 ve_thisClockType; // COMM_RTE_KEY_THIS_CLOCK_TYPE
	uint32 ve_fpgaVersion; // COMM_RTE_KEY_FPGA_VERSION
	int32 ve_gpsPhaseError; // COMM_RTE_KEY_GPS_PHASE_ERROR
	uint32 ve_activeOutputCableAdvance[NUM_OUTPUT_CHAN]; // COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE
	uint32 ve_warmupState; // COMM_RTE_KEY_WARMUP_STATE
	uint32 ve_outputSwitchState[NUM_TELE_OUTPUT_CHAN + 2]; // COMM_RTE_KEY_OUTPUT_SWITCH_STATE

#endif // defined(DEFINE_ISTATE_VARIABLE)
