/*                   
 * Filename: ConstDef.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Constant defined for the project
 * RCS: $Header: ConstDef.h 1.2 2008/03/13 11:36:58PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */             

#ifndef _ConstDef_H_09_19
#define _ConstDef_H_09_19

/* About Hitachi 2633F setting */
#define SYST_CLK  16000000
//#define TMR_CLK   SYST_CLK/8192    
//#define INTR_RATE   TMR_CLK/80 /* 80 Hz */
//#define TICK_1SEC  80

#define CHEAP_TMR_CLK   (SYST_CLK/8192)    

// 16000000 / 64 / 100 / 50 = 50
// divided by 100
#define RTOS_FIRST_DIV 99
// divided by 50
#define RTOS_SECOND_DIV 49
#define TICK_1SEC 50

#define FRAME_LENGTH 1050

#define JUMP_INSTR (0x5a000000)

#ifndef DBUG_FLAG1
// "DBUG"
#define DBUG_FLAG1	0x44425547
#endif
#ifndef DEBUG_FLAG2
// "CONT"
#define DBUG_FLAG2	0x434f4e54 
#endif

#define HEADER_PRODUCT_NAME_LENGTH 16
#define HEADER_FIRMWARE_VERSION_LENGTH 32
#define HEADER_MAGIC_LENGTH 16

// 3 64K sectors for event
// First 64K for setting
#define MAX_CODE_SIZE  (1024L * 64 * 28) 

// Cannot reboot in 10 minutes, otherwise it's counted as a failure.
#define MIN_REBOOT_PERIOD (3600L*24*TICK_1SEC)
#define MAX_APP_FAIL_ALLOWED 10

#endif /* _ConstDef_H_09_19 */
