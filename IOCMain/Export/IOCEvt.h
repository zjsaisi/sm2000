/*                   
 * Filename: IOCEvt.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: IOCEvt.h 1.19 2009/04/28 13:00:44PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.19 $
 */             
// This file is shared between RTE/PPC for event ids of RTE

#ifndef _IOCEVT_H_05_04_2003
#define _IOCEVT_H_05_04_2003

typedef enum {
	COMM_RTE_EVENT_NONE = 0, // Invalid Event id

	// Name: RTERST
	// AID1, AID2: N/A
	// RTE reboot in a controlled way. e.g. Triggerred by boot command.
	// Transient event. 
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_RTE_RESET,

	// Name: PPCNCOM
	// AID1, AID2: N/A
	// Communication failed with PPC.
	// This event should for RTE only because it cannot talk with PPC.
	// Default Alarm Level: Minor-Alarm
	COMM_RTE_EVENT_PPC_COM_ERROR,

	// Name: RTENCOM
	// AID1, AID2: N/A
	// Communication failed with twin RTE
	// Default Alarm Level: Minor-Alarm
	COMM_RTE_EVENT_RTE_COM_ERROR,

	// Name: FIRMCOPY
	// AID1, AID2: N/A
	// Event is on when RTE is sending out RTE code to twin.
	// off when it's done or failed.
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_FIRMWARE_COPY,

	// Name: COPYFAIL
	// AID1, AID2: N/A
	// Transient event to indicate copying firmware between RTE failed.
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_FIRMWARE_COPY_FAIL,

	// Name: CARDACTV
	// AID1, AID2: N/A
	// RTE active
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CARD_ACTIVE,

	// NAME: CARDSTBY
	// AID1, AID2: N/A
	// RTE standby
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CARD_STANDBY ,

	// Name: CLKWARM
	// AID1, AID2: N/A
	// RTE warmup 
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_WARMUP ,

	// Name: CLKFREE
	// AID1, AID2: N/A
	// RTE freerun
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_FREERUN ,

	// Name: CLKFSTRK
	// AID1, AID2: N/A
	// RTE in Fast tracking
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_FAST_TRACK,

	// Name: CLKNMTRK
	// AID1, AID2: N/A
	// RTE in Normal tracking
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_NORM_TRACK,

	// Name: CLKBRG
	// RTE in bridging
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_BRIDGE,

	// Name: CLKHOLD
	// AID1, AID2: N/A
	// RTE in holdover
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_HOLDOVER,

	// Name: CLKEXTHOLD
	// AID1, AID2: N/A
	// RTE in extended holdover
	// Default Alarm Level: Minor-Alarm
	COMM_RTE_EVENT_EXTENDED_HOLDOVER,

	// Name: CARDFAIL
	// AID1: Starting 1.1 release.
	//	 0:1V8     1:10M EFC   2:20M EFC
	//   3:25M EFC 4:24M5 EFC  5:24M7 EFC
	//   6:OCXO current  7:1PPS intr 8:Rubidium Unlock
	//   9:Unclassified(Should never happen)
	// zmiao 4/28/2009: Bob H and George Z say EFC cannot fail card.
	//   So AID1 1..5 won't happen any more. 
	// AID2: NA
	// RTE card failed.
	// Default Alarm Level: Critical-Alarm
	COMM_RTE_EVENT_CARD_FAIL,

	// Name: CARDOFF
	// AID1, AID2: NA
	// RTE card offline(diabled)
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CARD_OFFLINE,

	// Name: INTRLOS
	// AID1, AID2: NA
	// Routine Interruption from FPGA missing. This will trigger CARDFAIL
	// Default Alarm Level: Major-Alarm
	COMM_RTE_EVENT_INTERRUPT_LOSS,

	// Name: RTEINCOMP
	// AID1: 0
	// AID2: 0
	// RTE is incompatible with its twin RTE
	// Default Alarm Level: Major-Alarm
	COMM_RTE_EVENT_RTE_INCOMPATIBLE,

	// Name: INPACTV
	// AID1: 0..4 for GPS, UTIA, DTIB, BITS1, BITS2
	// AID2: 0
	// Input by AID1 is selected as active input.
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_INPUT_ACTIVE,

	// Name: SMTAVAIL
	// AID1, AID2: N/A
	// Smart Clock available
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_SMART_CLOCK,

	// Name: BT3ENGINE
	// AID1: 0..2 for BT3 mode
	// AID2: 0..7 for BT3 gear
	// BT3 engine gear shift. Transient
	// Default alarm_level: Non-Alarm
	COMM_RTE_EVENT_BT3_ENGINE, 

	// Name: BT3WMRST
	// AID1: 0
	// AID2: 0
	// Bestime engine warm reset. Transient event
	// Default Alarm level: Non-alarm
	COMM_RTE_EVENT_BT3_WARM_RESET, 

	// Name: BT3COLDRST
	// AID1: 0
	// AID2: 0
	// Bestime engine cold reset. Transient event
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_BT3_COLD_RESET,

	// Name: XSYNCLOS
	// Indicate LOS of Xsync. Only standby RTE can generate this alarm.
	// Aid1: 0    Aid2: 0
	// Default Alarm level: Major
	COMM_RTE_EVENT_XSYNC_LOSS,

	// Name: OVENOOR
	// OCXO current out of range
	// Once true, it will cause RTE failure.
	// AID1: 0 AID2: 0
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_OCXO_OUT_OF_RANGE,

	// Name: VOLTOOR
	// Adc measurement out of range. 
	// Once true, it will cause RTE failure.
	// zmiao: 4/28/2009: As mentioned, EFC adc won't cause failure any more.
	// AID1: 0=1.8V 1=10MEFC 2=20MEFC 3=25MEFC 4=24.5MEFC
	//		5=24.7MEFC
	// AID2: 0
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_VOLT_OUT_OF_RANGE,

	// Name: INPDISQ
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input Disqualified
	// Default Alarm Level: Minor-Alarm
	COMM_RTE_EVENT_INPUT_DISQUALIFIED,

	// Name: INPDISC
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input disconnected
	// Default Alarm level: Minor-Alarm
	COMM_RTE_EVENT_INPUT_DISCONNECTED,

	// Name: INPPHERR
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input Phase error
	// For now, it should only happen to BITS1 and BITS2
	// Default alarm level: Major
	COMM_RTE_EVENT_INPUT_PHASE_ERROR,

	// Name: INPIPLL
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input IPLL not settled.
	// For now, can only happen on BITS1, BITS2
	// default alarm level: minor
	COMM_RTE_EVENT_INPUT_IPLL_UNSETTLED,

	// Name: INPFERR
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input Frequency error
	// For now, can only happen on BITS1, BITS2
	// default alarm level: minor
	COMM_RTE_EVENT_INPUT_FREQUENCY_ERROR,

	// Name: INPOOF
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// Input Out Of Frame
	// for now, it only applies to BITS1, BITS2
	// default alarm level: minor
	COMM_RTE_EVENT_INPUT_OUT_OF_FRAME,

	// Name: INPAIS
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0.
	// Input AIS
	// Only happens on BITS1, BITS2
	COMM_RTE_EVENT_INPUT_AIS,

	// Name: PLLUNLOCK
	// PLL unlock . This will cause card failure.
	// Aid1: 0=20.48Mhz  1=25M 2=10M 3=24.576M 4=24.704M 
	// Aid2: 0
	// Default Alarm level: Major
	COMM_RTE_EVENT_PLL_UNLOCK,

	// Name: RUBUNLOCK
	// Rubdium unlock. This will cause card failure
	// Aid1: 0 Aid2: 0
	// Default Alarm level: Major
	COMM_RTE_EVENT_RUBIDIUM_UNLOCK,

	// Name: INPBT3UN
	// Input not qualified by BT3
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	// For now, it only applies to BITS1, BITS2
	COMM_RTE_EVENT_INPUT_BT3_UNSETTLED,

	// Name: POORSSM
	// Input SSM quality level too lower comparing to OSC
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	COMM_RTE_EVENT_INPUT_POOR_SSM,

	// Name: INPSSMCHG
	// Input SSM quality level change. Transient event
	// AID1: 0..4 for GPS, UTIA, UTIB, BITS1, BITS2
	// AID2: 0
	COMM_RTE_EVENT_INPUT_SSM_CHANGE,

	// Name: SYSSSMCHG
	// System SSM change. Transient event.
	// AID1: 0
	// AID2: 0
	COMM_RTE_EVENT_SYSTEM_SSM_CHANGE,

	// Name: GPSACQ
	// AID1: 0 for GPS chan
	// AID2: 0
	// GPS status in RGP is 0
	// Default alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_GPS_ACQUIRING,

	// Name: NORGP
	// AID1: 0 (For GPS chan)
	// AID2: 0
	// RTE is not receiving RGP packet.
	// Default alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_GPS_NO_RGP,

	// Name: CLKFREQ
	// AID1: 0
	// AID2: 0
	// Clock is in Frequency mode. Phase not aligned to possible input
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLOCK_FREQUENCY,

	// Name: PHPULL
	// AID1: 0
	// AID2: 0
	// RTE is pulling 1pps phase to GPS by adjusting frequency.
	// Default alarm level: Non-alarm
	COMM_RTE_EVENT_GPS_PHASE_PULLING,

	// Name: BT3PHCTRL
	// This event is for RTE only for now.
	// AID1: 0..2 for BT3 phase control mode
	// AID2: 0
	// 0: Frequency mode. 1=normal phase control 2=Aggressive phase control.
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_BT3_PHASE_CONTROL,

	// Any event below this line is not verified for TP5000.
	// Thus should NOT be used. 
	COMM_RTE_EVENT_MAX,

	////////////////////////////////////////////////////
	/////////////// Old stuff
	/////////////////////////////////////////////////////
	// Name: OUTDISC
	// AID1: 0..11 for output port 1 ..12
	// AID2: 0
	// Active: OUTPUT disconnected.  Cleared: output connected or disabled.
	// Default Alarm Level: Major-Alarm
	COMM_RTE_EVENT_OUTPUT_DISCONNECT,

	// Name: CLTWARM
	// AID1: 0..11 for output port 1..12
	// AID2: 0
	// The Client connected to output port is in WARMUP
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_WARMUP,

	// Name: CLTFREE
	// AID1: 0..11 for output port Port 1..12
	// AID2: 0
	// The client connected to output port is in Freerun
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_FREERUN,

	// Name: CLTFSTRK
	// AID1: 0..11 for output port Port 1..12
	// AID2: 0
	// The client connected to output port is in FAST TRACK
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_FAST_TRACK,

	// Name: CLTNMTRK
	// AID1: 0..11 for output port Port 1..12
	// AID2: 0
	// The client connected to output port is in NORMAL TRACK
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_NORMAL_TRACK,

	// Name: CLTBRG
	// AID1: 0..11 for output port Port 1..12
	// AID2: 0
	// The client connected to output port is in BRIDGE
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_BRIDGE,

	// Name: CLTHOLD
	// AID1: 0..11 for output port Port 1..12
	// AID2: 0
	// The client connected to output port is in HOLDOVER
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_HOLDOVER,

	// Name: CLTADVINV
	// AID1: 0..11 for output port PORT1 .. 12
	// AID2: 0
	// If true, the cable advance of the port is not valid.
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_CLIENT_CABLE_ADVANCE_INVALID,

	// Name: CLTUNSTBL
	// AID1: 0..11 for output port PORT1 .. 12
	// AID2: 0
	// if true, Performance of the client on the port is not stable
	// Default alarm level: Non-Alarm
	COMM_IOC_EVNET_CLIENT_PERFORMANCE_UNSTABLE,


	// Name: INPADVINV
	// AID1: 0..4 for GPS, PORT11(DTIA), PORT12(DTIB), BITS1, BITS2
	// AID2: 0
	// Input Cable advance invalid
	// Default Alarm level: Major-Alarm
	COMM_RTE_EVENT_INPUT_CABLE_ADVANCE_INVALID,

	// Name: INPUNSTBL
	// AID1: 0..4 for GPS, PORT11(DTIA), PORT12(DTIB), BITS1, BITS2
	// AID2: 0
	// Server indicate this input is not stable
	// Default Alarm Level: Minor-Alarm
	COMM_RTE_EVENT_INPUT_PERFORMANCE_UNSTABLE,



	// Name: OUTTEST
	// AID1: 0..11 for PORT1 .. 12
	// AID2: 0
	// Output port in test mode
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_OUTPUT_TESTING,

	// Name: BKUPLOSS
	// AID1, AID2: 0
	// Indicate Backup is not available for GPS input.
	// For now, it doesn't apply to TC1000. Left over for TimeProvider
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_BACKUP_LOSS,

	// Name: AUTORST
	// AID1, AID2: N/A
	// RTE is rebooted under automatic logic
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_AUTO_RESET,

	// Nmae: INSVRWARM
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The server connected to DTI input port is in WARMUP
	// Default alarm level: Non-ALarm
	COMM_RTE_EVENT_INPUT_SERVER_WARMUP,

	// Name: INSVRFREE
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The server connected to DTI input port is in FREERUN
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_SERVER_FREERUN,

	// Name: INSVRFAST
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The server connected to DTI input port is in Fast Mode
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_SERVER_FAST_TRACK,

	// Name: INSVRNORM
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The server connected to DTI input port is in Normal Mode
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_SERVER_NORMAL_TRACK,

	// Name: INSVRHOLD
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The server connected to DTI input port is in Holdover Mode
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_SERVER_HOLDOVER,

	// Name: INPATHINV
	// AID1: 1..2 for DTI1(PORT11), DTI2(PORT12)
	// AID2: 0
	// The path traceability received on input is invalid
	// Default alarm level: Non-alarm
	COMM_RTE_EVENT_INPUT_PATH_INVALID,

	// Name: INTODINV
	// AID1: 1..2 for DTI1(PORT11, DT2(PORT12)
	// AID2: 0
	// TOD message received on input is invalid
	// Default alarm level: Non-Alarm
	COMM_RTE_EVENT_INPUT_TOD_INVALID,









	// Name: ARMJAM
	// Sync Jam is armed by PPC.
	// AID1: 0
	// AID2: 0
	// Default Alarm Level: Non-Alarm
	COMM_RTE_EVENT_ARM_SYNC_JAM,

	// Name: DCMUNLOCK
	// DCM unlock. This will cause card failure.
	// Aid1: 0 Aid2: 0
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_DCM_UNLOCK,

	// Name: TPIUTUNE
	// TPIU tuning voltage outside 10%--90% range
	// once true, it will cause GPS input disqualified
	// Aid1: 0  Aid2: 0
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_TPIU_TUNE_OUT_OF_RANGE,

	// Name: TATUNE
	// Time Antenna tuning voltage outside 10% --90% range
	// Once true, it will cause GPS input disqualified
	// Aid1: 0   Aid2: 0
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_TA_TUNE_OUT_OF_RANGE,

	// Name: GPSCRC
	// Indicate CRC error from TimeAntenna to RTE, including CRC error cnt from TPIU and Time Antenna
	// Once asserted, it will trigger GPS input disqualified.
	// Aid1: 0  Aid2: 0
	// Default Alarm level: Non-Alarm
	COMM_RTE_EVENT_GPS_CRC_ERROR,


	// Add new one here
	// Also be sure to change Istate version. IOC_ISTATE_VERSION
	// Because alarm level for events are configrable and part of Istate 
	// If it's an alarm, make sure it's squelched when card is Out-Of-Service.
	// Default Alarm Level: Non-Alarm

	COMM_RTE_EVENT_MAKE_IT_16BIT = 1000
} CommRTEEventEnum;

#endif // _IOCEVT_H_05_04_2003
