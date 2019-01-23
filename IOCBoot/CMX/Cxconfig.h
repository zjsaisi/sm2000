/*********************************************************************
 The following will be included when the cmx_init.c program gets
 compiled. These are set the maximums for a variety of things. This way
 CMX can pre-allocate the needed memory to accomplish the user
 needs. This is so no memory allocation functions are needed by
 CMX, and no delays are introduced to obtain the needed memory during
 run time.

 The user is free to make a copy of this file for each application
 program, so each application program will have their own "cxconfig.h"
 file. Also the user should probally copy the cmx_init.c file to a "user
 name file", for each application program. This way the user does not
 have to change "this" file each time a different application program
 is worked with. Just remember to make sure you change the 
 #include <cxconfig.h> line in the user named ("cmx_init.c") file
 to include the proper user named "cxconfig.h" file.

 Also if any of these defines ARE changed, then the user must be
 sure to compile the cmx_init.c file (or equalivent user file)
 so these NEW values take effect.
*********************************************************************/  
/*
 * $Header: Cxconfig.h 1.1 2007/12/06 11:39:04PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */
                              
#ifndef _CXCONFIG_H_08_15    
#define _CXCONFIG_H_08_15

#pragma pack 2
#define C_MAX_TASKS 13	/* Maximum number of tasks for this program. */
#define C_MAX_RESOURCES 5	/* Maximum number of resources. */
#define C_MAX_CYCLIC_TIMERS 5	/* Maximum number of cyclic timers. */
#define C_MAX_MESSAGES 20	/* Maximum number of messages. */
#define C_MAX_QUEUES 6	/* Maximum number of queues. */
#define C_MAX_MAILBOXES 6	/* Maximum number of mailboxes. */
#define C_MAX_SEMAPHORES 3	/* Maximum number of mailboxes. */
#define C_TASK_STK_SIZE 4800	/* Maximum stack size of ALL tasks stacks
										added together, does NOT include timer task */
#define C_INTERRUPT_SIZE 4000	/* Size of interrupt stack. The CMX timer task
											scheduler, and int_action function use this. */
#define C_RTC_SCALE 1		/* number of timer interrupt ticks, before CMX 
										tick. All time counters based on this.  */
#define C_TSLICE_SCALE 4	/* The number of timer interrupt ticks, before
										forcing task to do time slice task switch. */

#define C_PIPE_SIZE 32		/* the number of slots for interrupt pipe */

#define CMXBUG_ENABLE 0		/* ENABLE or DISABLE CMXBug(TM) inclusion.
									Set to 1 to ENABLE, set to 0 (zero) to DISABLE.
									The use still must link in cmxbug.obj, for the 
									CMXBug code to be functional */

#define CMXTRACKER_ENABLE 0 	/* ENABLE or DISABLE CMXTracker(TM) inclusion.
									Set to 1 to ENABLE, set to 0 (zero) to DISABLE */

#define CMXTRACKER_TICK_UPDATE 1	/* Set to 1 to have SYSTEM TICK update
												CMXTracker 'track_tick_count' */

#define CMXTRACKER_SIZE 4096 	/* SIZE IN BYTES of the buffer to hold the 
									CMXTracker COMPRESSED information. */

#define CMX_RAM_INIT 1		/* ENABLE or DISABLE the initialization of CMX RAM
									variables. Usually done by Compiler start up code
									in most cases. If not, MUST be ENABLED. Normally
									compiler code will set GLOBAL variables that are
									initialized and set non-initialize variables to
									0 (zero). Set to 1 to ENABLE CMX code to initialize
									its variables, set to 0 (zero) to DISABLE */
#endif // _CXCONFIG_H_08_15
