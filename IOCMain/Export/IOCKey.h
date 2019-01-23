/*                   
 * Filename: IOCKey.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: IOCKey.h 1.28 2011/06/17 11:38:53PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.28 $
 */             
// This File is to be shared between PPC and RTE.
// It defines RTE keyword ids.

#ifndef _IOCKEY_H_12_07_2007
#define _IOCKEY_H_12_07_2007

// Procedure for adding keyword
// 1. Add item in CommIOCKeyEnum
// 2. Define dt_* or de_* in CIstateDti
//	  Including variable, action, flags etc.
// 3. Add d?_* from step above to m_istateList
// 4. Add action function into CAllProcess::ActionAll if necessary
//    Please note that the action rountine is called with negative index.
//	  The action function should take care of ActionAll() trigger after istate memory copy.
// 5. Change the version of istate. IOC_ISTATE_VERSION


/* Input channel numbering:
 *		GPS: 0		PRS: 1		SPAN1: 2	SPAN2: 3		OSC: 4
 * Output group numbering:
 *		A:	0		B: 1		C: 2		D: 3
 */ 
typedef enum {
	COMM_RTE_KEY_START = 0,
	
	/* Name: FIRMVER
	 * Read only Firmware version
	 * Format is: 0x00XXYYZZ for version XX.YY.ZZ (showing decimal on UI)
	 * TP5000: PPC can also use Factory interface to get similar info.
	 */
	COMM_RTE_KEY_FIRMWARE_VERSION, /* should be 1 to be consistent with boot code */ 

	/* Name: ISTVER
	 * Read only Istate item. Istate version 0..0xffff
	 */
	COMM_RTE_KEY_ISTATE_VERSION, //2 

	/* Name: MISC
	 * Write 1: To reset RTE Istate
	 *		 2: Reboot the card. Or Re-enable RTE from offline state.
	 *		 3: Copy firmware to twin RTE
	 *		 4: Save Istate/Factory setting to Flash
	 * Index1, Index2: NA
	 */
	COMM_RTE_KEY_MISC_ACTION, //3	

	/* Name: LOG		applies to each event
	 * 1: Log	0: no log
	 * index1 should be event_id
	 * index2 NA
	 * Specify if the event should be logged in RTE.
	 * Read-only for now. In TP5000, it's of no use to PPC part.
	 * default: depend on event/alarm
	 */								
	COMM_RTE_KEY_EVENT_LOG,	// 4 Log the event or not

	/* Name: ALARM		applies to each event
	 * Alarm level of events
	 * 0: Null/Not report 
	 * 1: Non Alarm
	 * 2: Minor Alarm
	 * 3: Major alarm
	 * 4: Critical alarm
	 * index1 should be event_id
	 * index2 NA
	 * read-only for now. For TP5000, it's of no use to PPC.
	 * Default: depend on event/alarm
	 */								
	COMM_RTE_KEY_ALARM_LEVEL,	// 5 Initial Alarm level of the event

	/* Name: SA			applies to each event
	 * 1: event is SA	0: event is NSA
	 * index1 should be event_id
	 * index2 NA
	 * Read-only for now. Doesn't apply to anything for TimeCreator
	 * specify if the event is SA or NSA. For TP5000, it's of no use to PPC. 
	 * default: depending on event/alarm
	 */									
	COMM_RTE_KEY_SERVICE_AFFECT,  // 6 If Event is Serive Affecting per event

	/* Name: MNUPT		applies to card
	 * Minutes to upgrade minor alarm to major alarm.
	 * index1 NA. Should be 0
	 * index2 NA. Should be 0
	 * Value: 1..0x7fffffff
	 * default: 0x7fffffff
	 * In TP5000, it's of no use to PPC part.
	 */								
	COMM_RTE_KEY_MINOR_UPGRADE_TIME, // 7 Time to take to updrade minor alarm to major alarm

	/* Name: MJUPT		applies to card
	 * Minutes to upgrade MAJOR to critical
	 * index1 NA. Should be 0.
	 * index2 NA. Should be 0.
	 * Value: 1..0x7fffffff
	 * default: 0x7fffffff
	 * In TP5000, it's of no use to PPC part.
	 */								
	COMM_RTE_KEY_MAJOR_UPGRADE_TIME, // 8 Time to take to upgrade major alarm to critical alarm

	/* TODSRC
	 * TOD Source
	 * index1: 0   index2: 0
	 * Value: 0=GPS   1=USER/SYS
	 *		2=NTP	3=PTP	4=UTI 5=RTC 6=Hybrid
	 * default: 1
	 * For TP5000 phase 1: only support USER and GPS mode.
	 * Correspond to ISTATE_RTE_TOD_SOURCE
	 */
	COMM_RTE_KEY_TOD_SOURCE,   //9

	/* Name: BRGTIME
	 * Bridge Time
	 * Index1: 0
	 * Index2: 0
	 * Value: int32 in second. 
	 *		Valid range = 100 .. 100000. Or 0x7fffffff for unlimited bridging
	 * default: 900
	 * Corespond to ISTATE_BRIDGE_TIME
	 */
	COMM_RTE_KEY_USER_BRIDGER,    //10

	/* MANINP
	 * Manual input selection
	 * Index1: 0 Index2:0
	 * Value: 0=INP1/BITS1 1:INP2/BITS2 
	 * Correspond to ISTATE_INP_REF
	 */
	COMM_RTE_KEY_MANUAL_INPUT,    //11

	/* ACTFINP
	 * Current active frequency input. Read-only
	 * For now, only valid for system TOD mode.
	 * Index1: 0 Index2: 0
	 * Value: -1=NA 0=INP1/BITS1  1=INP2/BITS2
	 * Correspond to ISTATE_CURR_INP_REF
	 */
	COMM_RTE_KEY_ACTIVE_FREQUENCY_INPUT, //12

	/* REFSEL
	 * Reference selection method
	 * index1: 0 index2: 0
	 * Value: 0: Manual  1: Priority 2: SSM
	 * Correspond to ISTATE_INP_REF_MODE
	 */
	COMM_RTE_KEY_REFERENCE_SELECT_METHOD,    //13

	/* SIPRI
	 * Span input priority
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 1..5 1 is the highest priority
	 * Correspond to ISTATE_INP_PRIORITY
	 */
	COMM_RTE_KEY_SPAN_INPUT_PRIORITY,  //14

	/* SIOSEL
	 * Span input/output selection
	 * Index1: 0..1 for span1(port1) and span2(port2)
	 * index2: 0
	 * Value: 0: Input 1: output
	 * Correspond to ISTATE_IO_CONFIG
	 */
	COMM_RTE_KEY_SPAN_IO_SELECT,      //15

	/* SIENABLE  
	 * Span input enable/disable
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 0=disable 1=enable
	 * Correspond to ISTATE_INP_STATE
	 */
	COMM_RTE_KEY_SPAN_INPUT_ENABLE,   //16

	/* SIFRAME 
	 * Span Input frame type
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 0=1.544Mhz 1=2Mhz 2=CCS 3=CAS 4=D4 5=ESF
	 * Correspond to ISTATE_INP_FRAME_TYPE
	 */
	COMM_RTE_KEY_SPAN_INPUT_FRAME_TYPE,  //17

	/* SICRC4
	 * Span Input CRC4 enable/disable
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 0=No-CRC4   1=CRC4
	 * Correspond to ISTATE_INP_CRC 
	 */
	COMM_RTE_KEY_SPAN_INPUT_CRC4,      //18

	/* SISSMRD
	 * Enable Reading SSM from Span input if possible
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 0=disable 1=enable
	 * Correspond to ISTATE_INP_SSM
	 */
	COMM_RTE_KEY_SPAN_INPUT_SSM_READ_ENABLE, //19

	/* SISSMBIT
	 * Span Input SSM bit selection
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 4..8 for SA4 to SA8
	 * Correspond to ISTATE_INP_SSM_BIT
	 */
	COMM_RTE_KEY_SPAN_INPUT_SSM_BIT,   //20


	/* SIASSM, obsoleted, don zhang, April 20, 2011:
	 * Assumed SSM quality level of span input
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 1..9
	 * Correspond to ISTATE_INP_SSM_VALUE
	 */
	COMM_RTE_KEY_SPAN_INPUT_ASSUMED_QUALITY_LEVEL, //21

// SSM table from Tom F on 7/7/2003
/*
    DS1                   E1
 QL SSM  Text Desc        SSM  Text Desc
--- ---- ---------------- ---- ----------------
  1 0x04 PRS              0x02 PRC/G.811
  2 0x08 STU              0x00 STU
  3 0x0C ST2              0x04 Type II
  4 0x0C Type I           0x04 Type I
  5 0x78 TNC              0x04 G.812 Transit
  6 0x7C ST3E             0x08 Type III
  7 0x10 ST3              0x0B Type IV
  8 0x22 SMC              0x0F G.813 Opt2
  9 0x30 DUS              0x0F DUS
*/
	/* SICSSM
	 * Current SSM quality level of span input
	 * Index1: 0..1 for INP1 and INP2
	 * Index2: 0
	 * Value: 1..9, 15
	 * 15: SSM is not available from input, For example: disabled, oof, no ssm.  
	 * Correspond to ISTATE_CURR_INP_SSM_VALUE
	 */
	COMM_RTE_KEY_SPAN_INPUT_CURRENT_QUALITY_LEVEL,    //22

	/* SOENABLE
	 * Span output enable
	 * Index1: 0..3 for span1 .. span4
	 * Index2: 0
	 * value: 0: diabled 1: enable
	 * Correspond to ISTATE_OUT_STATE
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_ENABLE,     //23

	/* SOFRAME
	 * Span Output Frame Type
	 * Index1: 0..3 for span1 .. span4
	 * Index2: 0
	 * value: 0:1.5Mhz, 1:2Mhz 2:CCS 3:CAS 4:D4 5:ESF
	 * Correspond to ISTATE_OUT_FRAME_TYPE
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE,     //24

	/* SOCRC4
	 * Span Output CRC4 setting
	 * Index1: 0..3 for span1 .. span4
	 * Index2: 0
	 * value: 0=No 1=CRC4
	 * Correspond to ISTATE_OUT_CRC
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_CRC4,          //25

	/* SOSSMENB
	 * Span Output SSM enable
	 * Index1: 0..3 for span1 .. span4
	 * Index2: 0
	 * value: 0=No SSM  1=Enable SSM
	 * Correspond to ISTATE_OUT_SSM
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_SSM_ENABLE,      //26

	/* SOSSMBIT
	 * Span Output SSM Bit
	 * Index1: 0..3 for Span1 .. span4
	 * Index2: 0
	 * value: 4..8 for SA4..SA8
	 * Corespond to ISTATE_OUT_SSM_BIT
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_SSM_BIT,         //27

	/* SOGEN
	 * Span output generation condition
	 * Index1: 0..3 for Span1 .. span4
	 * Index2: 0
	 * Value : 0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMTRACK
	 * Correspond to ISTATE_OUT_GENERATION
	 * zmiao: Obsolete 1/18/2010
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_GENERATION,       //28

	/* TENMHZ
	 * 10Mhz output Enable/Disable
	 * Index1: 0
	 * Index2: 0
	 * Value: 0=disable 1=enable
	 * Correspond to ISTATE_OUT_10M_STATE
	 */
	COMM_RTE_KEY_TEN_MHZ_ENABLE,                //29

	/* TENMGEN
	 * 10Mhz output generation condition
	 * index1: 0
	 * index2: 0
	 * Value: 0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMTRACK
	 * Correspond to ISTATE_OUT_10M_GENERATION
	 * zmiao: Obsolete 1/18/2010
	 */
	COMM_RTE_KEY_TEN_MHZ_GENERATION,          //30

	/* ONEHZ
	 * 1pps output enable/disable
	 * Index1: 0
	 * Index2: 0
	 * Value: 0=disable  1=enable
	 * Correspond to ISTATE_OUT_PPS_STATE
	 */
	COMM_RTE_KEY_ONE_HZ_ENABLE,                //31

	/* ONEHZGEN
	 * 1pps output generation condition
	 * Index1: 0
	 * Index2: 0
	 * Value: 0=WARMUP 1=FREERUN 2=FASTLOCK 3=NORMTRACK
	 * Correspond to ISTATE_OUT_PPS_GENERATION
	 * zmiao: Obsolete 1/18/2010
	 */
	COMM_RTE_KEY_ONE_HZ_GENERATION,             //32

	/* CADCVAL
	 * Read only current adc value from hitachi
	 * Index1: 0=1V8 1=10M-EFC 2=20M-EFC 3=25M-EFC
	 *	  4=24M5-EFC 5=24M7-EFC 6=OSC Current
	 * Index2: 0
	 * All voltage is in mV. Current is in mA.
	 * Per request from Gary J on 1/28/2008
	 */
	COMM_RTE_KEY_CURRENT_ADC_VALUE,              //33

	/* Name: CLKSTAT
	 * This RTE clock status. Read only for PPC.
	 * Index1: 0  Index2: 0
	 * Value: 1=WARMUP 2=FRERUN 3=FAST_TRACK 4=NORMAL_TRACK 
	 *		5=HOLDOVER 6=BRIDGE 7=FAIL 8=DISABLED(OFFLINE)
	 *		9=Extended Holdover
	 */
	COMM_RTE_KEY_CLOCK_LOOP_STATUS,                //34

	/* Name: OSCRDY 
	 * Oscillator warmup
	 * Read only for PPC. 
	 * Index1: 0, Index2: 0
	 * Value: 0 OSC is not ready. Should not be jammed by PPC.
	 *		  1 OSC is ready to be jammed by PPC and need to be jammed.
	 */
	COMM_RTE_KEY_OSC_READY,                          //35

	/* Name: HOLDTIME
	 * RTE stays in holdover for HOLDTIME, it will transfer to extended holdover
	 * Index1: 0
	 * Index2: 0
	 * Value: Time in second. Valid range: 0..0x7fffffff
	 * Default: 7fffffff
	 */
	COMM_RTE_KEY_HOLDOVER_TIME,                   //36

	/* Name: WARMSTATE
	 * Warmup progress.
	 * Read only for PPC. 
	 * Index1: 0 Index2: 0
	 * Value: 
	 *		1: Init state. Heat up OSC. It takes about 15 minutes for current quartz cards.
	 *		2: GPS mode only. If not, go back to Init state.
	 *			Active card: Wait for Best time getting to Gear 3
	 *			Standby card: Wait for 15 minutes to tracking active card which is out of warmup.
	 *		3: Wait for Jam from PPC/DTI input.
	 *			Standby RTE in GPS mode: Go back to Init state.
	 *			For Standby and non-GPS mode: Wait for 15 minutes.
	 *		4: GPS mode only. If not, go back to Init state.
	 *			Active card: Wait for Bestime getting to Gear 4
	 *			Standby card: N/A. Go back to Init state
	 *		5: Warmup complete.
	 *		6: Card failure
	 *		7: Card offline
	 */
	COMM_RTE_KEY_WARMUP_STATE,                    //37

	/* Name: SNGACT
	 * Application action that applies to this RTE only.
	 * This istate is not shared between RTE.
	 * This is a write only Istate.
	 * Index1: 0. Index2: 0.
	 * Value 1: Tell RTE that TOD is set in FPGA.
	 *		2: Inform RTE that PPC is booting up. RTE needs to reboot.
	 *		3: Inform RTE that PPC is shutting down. RTE need to put itself offline.
	 *		4: Resend active event list to PPC.
	 */
	COMM_RTE_KEY_SINGLE_ACTION,                //38

	/* Name: COMMONACT
	 * Application action that applies to both RTE. 
	 * Should be set to active RTE.
	 * This is a write only Istate
	 * Index1: 0   Index2: 0
	 * Value 1: void
	 *		 2: void
	 *		 3: Save config to flash now
	 *		 4: void. 
	 *		 100: Sync RTC with gpssec from FPGA. Used by RTE only.
	 *		 101: Set TOD valid on standby. For RTE only.
	 *		 102: Calculate leap time. For RTE only.
	 */
	COMM_RTE_KEY_COMMON_ACTION,                 //39

	/* Name: T1PUL
	 * Enable T1 pulse test mode on Span Output 
	 * Index1: 0..3 for span1 .. span4
	 * Index2: 0
	 * value: 0: Normal. The output is decided by COMM_RTE_KEY_SPAN_OUTPUT_FRAME_TYPE
	 *   1: Output is set to T1 pulse
	 * Correspond to 
	 */
	COMM_RTE_KEY_SPAN_OUTPUT_T1_PULSE,          //40

	/* SYSSSM
	 * System SSM value. Read only for PPC.
	 * Index1: 0
	 * Index2: 0
	 * Value: 1..9
	 */
	COMM_RTE_KEY_SYSTEM_SSM,                   //41

	/* OSCQL
	 * OSC quality level. This item is NOT shared between RTE.
	 * For Rubidium, it is fixed at 3 and cannot be changed.
	 * For Quartz, Gary J said it can only be 4. (Type I)
	 */
	COMM_RTE_KEY_OSC_QUALITY_LEVEL,            //42

	/* Name: GPSPHERR
	 * phase error between GPS and this 1PPS
	 * both ioc report independent number. Should be close to each other.
	 * Index1: 0  Index2: 0
	 * Value: 32-bit signed integer
	 *		LSB=1 cycle of 20.48Mhz clock. Roughly 48.83ns
	 *		When it's out of measurement range, it's 0x7fffffff
	 */
	COMM_RTE_KEY_GPS_PHASE_ERROR,               //43

	/* Name: OUTSW
	 * Read only istate for PPC to query if a specific output is turned on/off.
	 * Index1: 0..3 for SPAN1..4
	 *		   4 for 1PPS, 5 for 10Mhz
	 * Index2: 0
	 * Value: 0 output is off.
	 *		  1 output is on.
	 *		  2 AIS
	 */
	COMM_RTE_KEY_OUTPUT_SWITCH_STATE,             //44

	/* Name: CRDSTAT
	 * RTE card status is read-only for PPC. 
	 * Index1: 0	Index2: 0
	 * Value: 0=Unknown
	 *		  1=ACTIVE		2=STANDBY
	 *		  3=FAILED	    4=OFFLINE
	 */
	COMM_RTE_KEY_CARD_STATUS,                     //45

	/* Name: ACTINP
	 * Index1: 0
	 * Index2: 0
	 * Value: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 *		  -1 for no active input
	 * Read-only for PPC to find current active input channel.
	 * This will only tell what active RTE selects.
	 * default: NA
	 */
	COMM_RTE_KEY_ACTIVE_INPUT,                    //46

	/* Name: WarmTime
	 * Index1: 0..1
	 * Index2: 0
	 * Value: Index1=0: Time(seconds) spent in current warmup substate
	 *		Index1=1: Total time(seconds) spent in warmup
	 * Read-only for PPC to find the duration of warmup state.
	 * default: NA
	 */
	COMM_RTE_KEY_WARMUP_TIME,                      //47

	/* Name: OUTCHOICE 
	 * Output choice during Warmup/Freerun/Holdover/Fasttrack
	 * Index1: 0 .. 3 for Span1 .. Span4. 4=1PPS 5=10Mhz
	 * Index2: 0=WARMUP  1=FREERUN 2=HOLDOVER 3=FASTTRACK
	 * For 1PPS and 10Mhz output, the choice is limited on 0 and 1.
	 * For signals that cannot generate AIS, it will be equal to Squelch.
	 * Value: 0=Squelch 1=On 2=AIS   
	 */
	COMM_RTE_KEY_OUTPUT_CHOICE,                   //48
	
	/* Name: INPRI  GPZ June 2010 Added one more RTE Istate so I can change GPS priority
	 * Input Priority
	 * Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 * Index2: 0
	 * Value: 1..16
	 * default: 1, 2, 3, 4, 5 for GPS, PORT11, PORT12, BITS1, BITS2
	 */
	COMM_RTE_KEY_INPUT_PRIORITY,                         //49

	/* Name: INSTATE
	 * Input STATE (enable/disable)
	 * Index1: 0..4 for GPS, PORT11(DTIA), PORT12(DTIB), BITS1, BITS2
	 * Index2: 0
	 * Value: 1 enable  0 disable
	 * Default: 0 for all channels
	 */
	COMM_RTE_KEY_INPUT_STATE,   // 50


	/* INPSSM, added on April 20, 2011, don zhang
	 * Assumed quality level of input
	 * Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 * Index2: 0
	 * Value: 1..9
	 */
	COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL, //51


	/* Not real istate. The istate following this are not verified yet. 
	 */	
	COMM_RTE_KEY_END,                             //52

/***************************************************************************************/
/*                              Old TimeCreator stuff                                  */
/***************************************************************************************/

	/* ////////////// Start of DTI specific Istate items */



	/* SYSMODE
	 * System mode of operation. Read-only for PPC.
	 * index1: 0  Index2: 0
	 * Value 0=ROOT
	 *       1=SERVER(SUBTENDING)
	 * default: 0
	 */
	COMM_RTE_KEY_SYSTEM_MODE,


	/* PORTTYPE
	 * PORT11 and PORT12 type
	 * Index1: 0..1 for PORT11(DTIA), PORT12(DTIB)
	 * Index2: 0
	 * Value: 0=SERVER(OUTPUT)   1=CLIENT(INPUT)
	 * default: 0
	 */
	COMM_RTE_KEY_PORT_TYPE,

	/* Name: OUTSTATE 
	 * Output enable/disable
	 * Index1: 0..11 for channel number
	 * index2: NA. Should be 0.
	 * value: 1 enable  0 disable
	 * default: 1 for first 10 channels and 0 for the rest
	 */
	COMM_RTE_KEY_OUTPUT_STATE, 

	/* Name: OUTADV
	 * Cable advance in meter. It's used to set cable advance when in manual mode.
	 * index1: 0..1 for Port 1..12
	 * index2: 0
	 * value: LSB per FPGA definition. about 26ps per LSB
	 * Default: 0 for all channels
	 */
	COMM_RTE_KEY_OUTPUT_CABLE_ADVANCE,

	/* Name: OUTMODE 
	 * Cable advance mode
	 * index1: 0..11 for Port 1..12
	 * index2: 0
	 * value 0=AUTO   1=MANUAL
	 * default: 0 for all channels
	 */
	COMM_RTE_KEY_OUTPUT_MODE,

	/* Name: INSTATE
	 * Input STATE (enable/disable)
	 * Index1: 0..4 for GPS, PORT11(DTIA), PORT12(DTIB), BITS1, BITS2
	 * Index2: 0
	 * Value: 1 enable  0 disable
	 * Default: 0 for all channels
	 */
//	COMM_RTE_KEY_INPUT_STATE,   //GPZ June 2010 Move to last RTE Istate                        

	/* Name: INPRI 
	 * Input Priority
	 * Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 * Index2: 0
	 * Value: 1..5
	 * default: 1, 2, 3, 4, 5 for GPS, PORT11, PORT12, BITS1, BITS2
	 */
//	COMM_RTE_KEY_INPUT_PRIORITY,     GPZ June 2010 Move to 2nd to last RTE Istate                    

	/* Name: DISQDELAY
	 * Input Disqualified Set timer. 
	 * Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 * Index2: 0
	 * Value: 5..1000  Delay in seconds or 0=No delay
	 * Default: 10 for all channels.
	 */
	COMM_RTE_KEY_INPUT_DISQUALIFY_DELAY,

	/* Name: QUALDELAY
     * Input Qualified Clear timer
	 * Index1: 0..4 for GPS, PORT11, PORT12, BITS1, BITS2
	 * Index2: 0
	 * Value: 5..1000 Delay in seconds. or 0=no delay
	 * default: 5 for all channels
	 */
	COMM_RTE_KEY_INPUT_QUALIFY_DELAY,


	/* Name: CLTSTAT
	 * Remote Client status. Read only for PPC.
	 * Index1: 0  Index2: 0
	 * Value: 0=NA 1=WARMUP 2=FREERUN 3=FAST_TRACK 4=NORMAL_TRACK
	 *		5=HOLDOVER  6=BRIDGE
	 *		Only active RTE holds valid number.
	 */
	COMM_RTE_KEY_CLIENT_STATUS,

	/* Name: CBLEN
	 * Cable Length between Server and Client. It's from cable delay.
	 * Read only for PPC.
	 * Index1: 0..11 for PORT11..PORT12
	 * Index2: 0
	 * Value: Cable Length 1LSB=26ps. Only active RTE holds valid number.
	 */
	COMM_RTE_KEY_OUTPUT_CABLE_DELAY,

	/* Name: TIMEZONE
	 * Local time offset in second from GMT
	 * Index1: 0
	 * Index2: 0
	 * Value: int32 in second. 
	 *     Should be multiple of 1800 and between -46800 & 46800
	 * Default: 0
	 */
	COMM_RTE_KEY_TIME_ZONE,



	/* Name: COMPID
	 * Index1: 0
	 * Index2: 0
	 * Value: 1 for first release
	 */
	COMM_RTE_KEY_COMPATIBILITY_ID,

	/* Name: LEAPSEC
	 * Index1: 0
	 * Index2: 0
	 * Value: Accumulative leap second.
	 * In Subtending mode, PPC should read from this.
	 * For other modes, PPC should set it right.
	 * Default: 14
	 */
	COMM_RTE_KEY_TOTAL_LEAP_SECOND,

	/* Name: LEAPPEND
	 * Index1: 0
	 * Index2: 0
	 * Value: 1 , -1 or 0 
	 * Leap second pending at the end of this month.
	 * 1: Leap second to be added. There will be 23:59:60 second.
	 * -1: Leap second to be dropped. 23:59:59 will be skipped.
	 * 0: No leap second.
	 * For Subtending mode, PPC should read from this.
	 * for other modes, PPC should set it right.
	 * Default: 0
	 */
	COMM_RTE_KEY_LEAP_PENDING,

	/* Name: OUTTEST
	 * Index1: 0..11 for Port 1 .. 12
	 * Index2: 0
	 * Value: 1 for enable. 0: disable
	 * Enable/Disable Test mode on output.
	 * For Port11 and Port12, COMM_RTE_KEY_PORT_TYPE has to be SERVER to be in test mode
	 * It doesn't matter what enable/disable of output port. (COMM_RTE_KEY_OUTPUT_STATE)
	 * Default: 0
	 */
	COMM_RTE_KEY_OUTPUT_TEST,

	/* Name: PTIP4
	 * Server IPV4 address for path traceability. 
	 * It's message Type 1 if this is root server.
	 * When acting as subtending, 
	 * Apply to all output channel with single value. 
	 * Index1: 0
	 * Index2: 0
	 * Value: Ip address. MSB will be sent out first.
	 * Default: 0x0
	 */
	COMM_RTE_KEY_PATH_TRACE_IP_FOUR,

	/* Name: PTOUTPORT
	 * Output port number used for path traceability
	 * The content goes out as message Type 2 if this RTE is a root.
	 * If it's subtending server, it goes as Type 4
	 * Index1: 0..11 for Port 1 to 12
	 * Index2: 0
	 * Value: Port number ranges from 0..255
	 * Default: 1 for Port1, 2 for Port2 ... 12 for Port12
	 */
	COMM_RTE_KEY_PATH_TRACE_OUTPUT_PORT,

	/* Name: PTIP6
	 * Server IPV6 addres for path traceability
	 * It's message type 5 if the RTE is root server.
	 * If it's subtending server, it goes out as message type 6.
	 * When last double word is written by PPC, it will be sent out to client.
	 * Index1: 0..3
	 * Index2: 0
	 * value: 4 X 32 bit to hold IPV6 address. MSB of first double word
	 *		is sent first.
	 * Default: all 0
	 */
	COMM_RTE_KEY_PATH_TRACE_IP_SIX,


	/* Name: OUTTODMOD 
	 * Output Tod mode. Short format or verbose
	 * Index1: 0
	 * Index2: 0
	 * Value: 0= Short   1=Verbose
	 * default: 0
	 */
	COMM_RTE_KEY_OUTPUT_TOD_MODE,

	/* Name: BT3RPT
	 * Present this structure within uint32 array. Total is 46 bytes.
	 * Read only for PPC.
	 * Index1: 0..12.  Index2: 0
	 * PPC read 4 byes each time.
		struct MSG_BesTime_Engine_Report
		{
		unsigned short LO_Weight;  //Current Weight of Local Oscillator
		unsigned short SPA_Weight;  //Current Weight of Span-A 
		unsigned short SPB_Weight;  //Current Weight of Span-B 
		unsigned short PRR_Weight;  //Current Weight of PRR 
		long LO_Prediction;  //Current Bridging Prediction in ns for local oscillator
		long SPA_Prediction;  //Current Bridging Prediction in ns for of Span-A channel
		long SPB_Prediction;  //Current Bridging Prediction in ns of Span-B channel
		long PRR_Prediction;  //Current Bridging Prediction in ns of PRR channel
		long TFOM;                    //Estimated (2sigma) of accuracy of 1 PPS output
		long GPS_mdev;		//current modified allan standard deviation noise estimate for GPS
		long PA_mdev;    //current estimate for ensemble channel 1 (either span or remote)
		long PB_mdev;   
		long LO_mdev;  
		unsigned short Backup_Reg;	//Backup assigments 0000 0 PRR SPB SPA 
		};
	 
	 */
	COMM_RTE_KEY_BT3_REPORT, 



	/* Name: OKLOAD
	 * RTE card is ready to receive firmware download.
	 * Read-only for PPC. Each RTE may report different status.
	 * Index1: 0, Index2: 0
	 * Value: 1=OK for PPC to download firmware to this RTE.
	 *		  0=Don't download firmware.
	 */
	COMM_RTE_KEY_OK_DOWNLOAD_FIRMWARE,

	/* Name: CLTCLKTYPE
	 * Client clock type connecting to output port.
	 * Only Active Clock present this information.
	 * Index1: 0..11 for output port 1..12
	 * Index2: 0
	 * Value:  0=ITU Type I   1=ITU Type II
	 *		   2=ITU Type III 3=Stratum 3
	 *		   4=Minumum Clock
	 *		   16=Unknown
	 */
	COMM_RTE_KEY_CLIENT_CLOCK_TYPE,

	/* Name: CLTVER
	 * Client DTI version connecting to output port
	 * Only active Clock present this information.
	 * Index1: 0..11 for Port 1..12
	 * Index2: 0
	 * Value: -1=unknown
	 *		  0..0xff=CLIENT DTI version
	 */
	COMM_RTE_KEY_CLIENT_DTI_VERSION,

	/* Name: OUTPERR
	 * Current Output Phase error updated every second.
	 * Only active Clock present this information
	 * Index1: 0..11 for Port1..12
	 * Index2: 0
	 * Value:  Client phase error as defined in FPGA register 0xa0..0xa3
	 *		   0x7fffffff: Invalid phase error due to LOS or something
	 */
	COMM_RTE_KEY_OUTPUT_PHASE_ERROR, 

	/* Name: IPIP4
	 * Input root path traceability IPV4
	 * Only active RTE present this information.
	 * Read-only for PPC
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: IPV4 address from path traceability
	 */
	COMM_RTE_KEY_INPUT_PATH_ROOT_IP_FOUR, 

	/* Name: IPIP6 
	 * Input root path traceability IPV6
	 * Only active RTE present this information.
	 * Read-only for PPC
	 * Index1: 0..3 to hold 4 double word of IPV6
	 * Index2: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Value: IPV6 address from path traceability
	 */
	COMM_RTE_KEY_INPUT_PATH_ROOT_IP_SIX,

	/* Name: IPPN 
	 * Input root path traceability port number
	 * Only active RTE present this information.
	 * Read-only for PPC
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: Port number
	 */
	COMM_RTE_KEY_INPUT_PATH_ROOT_PORT,

	/* Name: IPVER 
	 * Input root path traceability 
	 * Read-only for PPC
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: DTI version
	 */
	COMM_RTE_KEY_INPUT_PATH_ROOT_VERSION,

	/* Name: INPTODMDE
	 * Input timing setting mode in Tod status field. 
	 * Read-only for PPC. Only active RTE present this info.
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: As presented in Bits7:4 of TOD status byte
	 *		0=Default(RTC)  1:User  2: NTP	3: GPS
	 *		16=Unknown
	 */
	COMM_RTE_KEY_INPUT_TOD_SETTING_MODE,

	/* Name: INPTODSTAT 
	 * Input TOD state as shown on TOD status filed.
	 * Read-only for PPC. Only active RTE present this info
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(PORT12)
	 * Index2: 0
	 * Value: As presented in Bits3..2 of TOD status byte
	 *		0=invalid	1=valid		
	 *		16=unknown
	 */
	COMM_RTE_KEY_INPUT_TOD_STATE,

	/* Name: INPSVRCLK 
	 * Clock type of server connected to input.
	 * Read-only for PPC. Only active RTE present this info.
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: As presented in Bits2..0 of Deveice type byte from server.
	 *		0=ITU-I  1=ITU-II  2= ITU-III  3=ST3
	 *		16=unknown
	 */
	COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE,

	/* Name: INPTIMSRC 
	 * Input timing source as shown on server device type byte.
	 * Read-only for PPC. Only active RTE present this info
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: 0=No external timeing.  1=GPS  2=Network
	 *		  16=unknown
	 */
	COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE,

	/* Name: INPSVRLOOP
	 * Loop status of Server connected to input.
	 * Read-only for PPC. Only active RTE present this info.
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: 0=unknown 1=warmup 2=freerun 3=fast 4=normal
	 *		  5=holdover
	 */
	COMM_RTE_KEY_INPUT_SERVER_LOOP_STATUS,

	/* Name: INPCBADV
	 * Cable advance set by server connected on input
	 * Read-only for PPC. Only active RTE present this info
	 * Index1: 0..1 for DTI1(Port11) .. DTI2(Port12)
	 * Index2: 0
	 * Value: 0=unknown 
	 *		  1LSB=1/256 of cycle of 10.24Mhz*512/35 clock
	 */
	COMM_RTE_KEY_INPUT_CABLE_ADVANCE,

	/* Name: CLTFSMCNT
	 * Client state machine change counter
	 * Index1: 0..11 for Port1 .. Port12
	 * Index2: 0=T3  1=T4  2=T6  3=T7
	 * T3: Fast --> Freerun
	 * T4: Fast --> Normal
	 * T6: Bridge --> Normal
	 * T7: Bridge --> Fast
	 * Value: 32-bit Counter
	 */
	COMM_RTE_KEY_OUTPUT_CLIENT_TRANSITION_COUNT,

	/* Name: GFSMCNT
	 * Used by RTE only. 
	 */
	COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_TRANSITION_COUNT,

	/* Name: CLTNHTIME
	 * Client in Normal & Holdover time in 10Khz clock cycles
	 * Index1: 0..11 for Port1 .. Port12
	 * Index2: 0=Normal   1=Holdover
	 * Value: 32-bit counter
	 */
	COMM_RTE_KEY_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME,

	/* Name: GCLTNHTIME
	 * Used by RTE only
	 */
	COMM_RTE_KEY_GHOST_OUTPUT_CLIENT_NORMAL_HOLDOVER_TIME,

	/* Name: CLKTYPE
	 * Clock type of this RTE.
	 * Read-only for PPC.
	 * Index1: 0   Index2: 0
	 * Value: 0=ITU-I  1=ITU-II  2=ITU-III
	 *		3=ST-3  
	 */
	COMM_RTE_KEY_THIS_CLOCK_TYPE,

	/* Name: EFFTODSRC
	 * Effective Tod source currently sent out on output
	 * Index1: 0, Index2=0
	 * Value: 0=DEFAULT(RTC)   1=USER
	 *		2=NTP	3=DTI	4=GPS
	 * Read-only
	 */
	COMM_RTE_KEY_EFFECTIVE_TOD_SOURCE,

	/* Name: FPGAVER
	 * Read only FPGA version number
	 * Index1: 0  Index2: 0
	 * Value: FPGA version number
	 */
	COMM_RTE_KEY_FPGA_VERSION, 


	/* Name: IMCMARK
	 * Mark used by PPC to figure out RTE status.
	 * Defined by PPC. 
	 * It's auto copied between RTE. Should only set to active RTE by PPC.
	 * Index1: 0  Index2: 0
	 * Value: Default = 0
	 */
	COMM_RTE_KEY_IMC_COMMON_MARK,

	/* Name: ACBADV
	 * Cable Advance used in hardware control
	 * Read only for PPC.
	 * Index1: 0..11 for PORT11..PORT12
	 * Index2: 0
	 * Value: Cable Length 1LSB=26ps. Only active RTE holds valid number.
	 */
	COMM_RTE_KEY_ACTIVE_OUTPUT_CABLE_ADVANCE,


	COMM_RTE_KEY_TO_BE_16BIT = 1000
} CommIOCKeyEnum;

#endif // _IOCKEY_H_12_07_2007
