/*                   
 * Filename: ConstDef.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Constant defined for the project
 * RCS: $Header: ConstDef.h 1.4 2008/01/29 11:29:25PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */             

#ifndef _ConstDef_H_09_19
#define _ConstDef_H_09_19

#include "CodeOpt.h"

/* About Hitachi 2633F setting */
#define SYST_CLK  16000000
#define CHEAP_TMR_CLK   (SYST_CLK/8192)    

// 16000000 / 64 / 100 / 25 / SOFT = 50
// divided by 100
#define RTOS_FIRST_DIV 99
// divided by 50
#define RTOS_SECOND_DIV 24
// Software division. Don't change before scan for the symbol
#define SOFT_DIVISION 2

#define TICK_1SEC 50

#define FRAME_LENGTH 1050

#define JUMP_INSTR (0x5a000000)

// OSC type list
#define OSC_PART_MIN	1 /* Inclusive */
#define OSC_PART_NUMBER_023_63001_02 1
#define OSC_PART_MTI260		6 /* Tom F wants to make 1.. 5 for single OCXO. 6 .. 10 for Double. 11 .. 15 for Rubidium */
// So far this is only for test
#define OSC_PART_RUBIDIUM_X72	11 /* This is impcomplete. No part number assigned for such a card */
// Add new type here.

#if ENABLE_RUBIDIUM
#define OSC_PART_MAX	11 /* Inclusive*/
#else
#define OSC_PART_MAX	10 /* Inclusive*/
#endif

// Clock Type list
#define CLOCK_TYPE_I 0
#define CLOCK_TYPE_II 1
// #define CLOCK_TYPE_STRATUM_II 1
#define CLOCK_TYPE_III 2
#define CLOCK_TYPE_STRATUM_3E 3
#define CLOCK_TYPE_MINIMUM 4
#define CLOCK_TYPE_MAX 4

// Add new clock type here

// SSM QL for each clock type
//#define CLOCK_TYPE_I_SSM_QL 4
//#define CLOCK_TYPE_III_SSM_QL 6 /* Changed from 5 to 6 according to Tom F' table */
//#define CLOCK_TYPE_STRATUM_II_SSM_QL 3
//#define CLOCK_TYPE_STRATUM_3E_SSM_QL 6
//#define CLOCK_TYPE_II_SSM_QL 3
//#define CLOCK_TYPE_STRATUM_UNKNOWN_SSM_QL 2

// Const about input output
#define NUM_INPUT 5
#define CHAN_GPS  0
#define CHAN_DTI1 1
#define CHAN_DTI2 2

#define NUM_SPAN_INPUT 2
#define CHAN_BITS1 3
#define CHAN_BITS2 4
#define NUM_DTI_INPUT 2

/* Span input in TP5000 */
#define NUM_DUAL_SPAN_CHAN 2
#define NUM_TELE_OUTPUT_CHAN  4 

// Port11 and Port12 are dual use ports.
#define NUM_DUAL_USE_PORT 2 

// #define NUM_OUTPUT_GROUP 4

#define PANEL_LED_NUM 6
// number of word(32 bit) ion BT3 report structure 
#define NUM_BT3_REPORT_WORD 12 

// Forced disqualification by PPC.
#define FORCE_PM_DISQUALIFY  0 		
#define NUM_FORCE_DISQUALIFY_REASON 2

// number of compatibility masks exported to PPC
#define NUM_COMP_MASKS_ISTATE 5

// Max length of factory strings
#define MAX_SERIAL_LENGTH 32


// number of output group
#define NUM_OUTPUT_CHAN 	12

#define METER_PER_LSB ((double)0.00546)

#define HEADER_PRODUCT_NAME_LENGTH 16
#define HEADER_FIRMWARE_VERSION_LENGTH 32
#define HEADER_MAGIC_LENGTH 16
#define HEADER_SIGNATURE_LENGTH (HEADER_PRODUCT_NAME_LENGTH + HEADER_FIRMWARE_VERSION_LENGTH + HEADER_MAGIC_LENGTH)

#define FPGA_START_ADDRESS 0x400000

// 3 64K sectors for event
// First 64K for setting
#define MAX_CODE_SIZE  (1024L * 64 * 28) 

#define HOT_ACTIVE  1
#define COLD_ACTIVE 2

#define HOT_STANDBY 1
#define COLD_STANDBY 2

#define MAIN_CODE_START 0x810000

// Istate stuff
#define CMD_WRITE_ISTATE  1
#define CMD_WRITE_ISTATE_FROM_IMC 2
	

// Initial version
#define HARDWARE_REV_ID0  0
// R60 changed from 43.2K to 20K   8/4/2006
#define HARDWARE_REV_ID1  1

#endif /* _ConstDef_H_09_19 */
