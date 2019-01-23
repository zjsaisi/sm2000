/*                   
 * Filename: CodeOpt.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Compiling option for code generation
 * Simple Data type definition for the project
 * RCS: $Header: CodeOpt.h 1.4 2008/03/11 10:44:28PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.4 $
 */             

#pragma pack 2

#ifndef _CODEOPT_H_06_12_2003
#define _CODEOPT_H_06_12_2003

/* The goal is to take off all TO_BE_COMPLETE */
#define TO_BE_COMPLETE  1

// When 1, strip off application level stuff.
#define BASIC_ISTATE_ONLY 	0
#define BASIC_EVENT_ONLY	0

//1: Enable watch dog
//0: Disable watch dog 
#define ENABLE_WATCHDOG		1

// Should be 0 so RGP message will be used by Bestime
#define IGNORE_RGP		0

// Define Warmup behaviour. 1: Fixed time 30second warmup. 0: Look at current.
#define FIXED_TIME_WARMUP 0

// Dynamically change output amplitude or not. Doing that is only experiment 
// at this point.
#define DO_DYNAMIC_OUTPUT 0

// Istate code debug option
#ifndef DEBUG_ISTATE
#define DEBUG_ISTATE 1
#endif

// ISTATE ACTION debug option
#ifndef DEBUG_ISTATE_ACTION
#define DEBUG_ISTATE_ACTION 1
#endif

// Enable the function to fake phase delta on debug port                              
#ifndef  USE_FAKE_DELTA
#define USE_FAKE_DELTA 1
#endif

// Enable rubidium code       
#ifndef ENABLE_RUBIDIUM
#define ENABLE_RUBIDIUM 0
#endif

// Enable Bestime
#ifndef ENABLE_BESTIME
#define ENABLE_BESTIME	1
#endif
                  
// Enable remote debug from PPC.
#ifndef ENABLE_REMOTE_DEBUG
#define ENABLE_REMOTE_DEBUG 1
#endif

//#ifndef FORCE_JIO_ON
//#define FORCE_JIO_ON  1
//#endif

#define USE_DTC0   	1
#define USE_DTC1   	1
#define USE_DTC3	1
#define USE_DTC4	1

#if _DEBUG
#define ENABLE_SCI0 0
#endif

#ifndef ENABLE_SCI0
#define ENABLE_SCI0 1
#endif

#if (ENABLE_SCI0 == 0)
#define DEBUG_CHECK_POINT 0
#endif

#ifndef DEBUG_CHECK_POINT 
#define DEBUG_CHECK_POINT 1
#endif

#ifndef DEFAULT_BAUD0
#define DEFAULT_BAUD0	9600
#endif

#ifndef INT_LEVEL_SCI0
#define INT_LEVEL_SCI0 	2
#endif

#ifndef ENABLE_SCI1
#define ENABLE_SCI1 1
#endif

#if ENABLE_RUBIDIUM
#ifndef DEFAULT_BAUD1
#define DEFAULT_BAUD1	9600
#endif

#endif

#ifndef DEFAULT_BAUD1
#define DEFAULT_BAUD1	9600
#endif

#ifndef INT_LEVEL_SCI1
#define INT_LEVEL_SCI1 	2
#endif

#ifndef ENABLE_SCI3
#define ENABLE_SCI3 1
#endif

#ifndef FAKE_SCI3  // Fake SCI3 to SCI1
#define FAKE_SCI3 0
#endif
                     
// For Polestar, it goes to PPC8313
#ifndef DEFAULT_BAUD3   // actually 115200 == 125000
#define DEFAULT_BAUD3	38400
#endif

#ifndef INT_LEVEL_SCI3
#define INT_LEVEL_SCI3	4
#endif

#ifndef ENABLE_SCI4
#define ENABLE_SCI4	1
#endif
         
// To Twin RTE
#ifndef DEFAULT_BAUD4
#define DEFAULT_BAUD4	115200 // 115200 == 125000 // 57600 == 55555
#endif

#ifndef INT_lEVEL_SCI4
#define INT_LEVEL_SCI4	4
#endif

#ifndef MEASURE_CPU_SPEED
#define MEASURE_CPU_SPEED 1
#endif

// Pick either TIMING_CPU or TIMING_CSM
#ifndef TIMING_CPU
#define TIMING_CPU		0
#endif

#ifndef TIMING_CSM
#define TIMING_CSM		1
#endif

// Standby RTE Copy time from Active or not
#ifndef COPY_TIME_FROM_ACTIVE
#define COPY_TIME_FROM_ACTIVE	1
#endif

#ifndef INT_LEVEL_OS_TIMER
#define INT_LEVEL_OS_TIMER 5
#endif

#ifndef INT_LEVEL_FPGA
#define INT_LEVEL_FPGA   6
#endif

// No intr for TimeCreator
#ifndef INT_LEVEL_RETIMER
#define INT_LEVEL_RETIMER 0
#endif
  
#ifndef INT_LEVEL_ADC
#define INT_LEVEL_ADC 1
#endif
                        
#ifndef RUN_ON_TIMEPROVIDER
#define RUN_ON_TIMEPROVIDER 0
#endif

#endif // _CODEOPT_H_06_12_2003
