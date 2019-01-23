/*********************************************************

Copyright (c) CMX Company. 1999. All rights reserved

*********************************************************/
/* version 5.30, adds the following:
1. counting semaphores
2. priority inversion on resources
3. highest priority task waiting on resource, is the one
   that will get resource when it is released.
4. New interrupt pipe functionality
*/

#define CMX_INIT_MODULE
#define CMXMODULE

#define C_SPY 0	/* set to 1, if using C_SPY simulator */

#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include <cxfuncs.h>		/* get cmx include header file */
#endif

#include "cxconfig.h"	/* get user configuration file */
#include <stdio.h>		/* get cmx include header file */

#if CMXBUG_ENABLE > 0
//#include <cmxbug.h>		/* get cmx include header file */
#elif CMXTRACKER_ENABLE > 0
//#include <cmxtrack.h>	/* get cmx include header file */
#endif

#if (C_PIPE_SIZE > 255 || C_PIPE_SIZE < 1)
#error PIPE_SIZE MUST BE between 1 and 255 
#endif 

#if C_MAX_MESSAGES > 0
#define CMX_MAX_MESSAGES (C_MAX_MESSAGES + 2)
#endif


#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
#define BUG_STACK_SIZE	256
#define CC_TASK_STK_SIZE (C_TASK_STK_SIZE + BUG_STACK_SIZE)
#else
#define CC_TASK_STK_SIZE C_TASK_STK_SIZE
#endif

#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
#define CC_MAX_TASKS (C_MAX_TASKS + 1)
#else
#define CC_MAX_TASKS C_MAX_TASKS
#endif
          
#pragma section CMX
/***********************************************************
 WARNING: these are assumed to be initailized by the "C" compiler
 start up code. Also all non-initialized CMX variables are assumed
 to be zeroed out by the "C" compiler startup code. This is normally
 done. If NOT, then the user must set the CMX_RAM_INIT define to 1. 
 See cxconfig.h (or equivalent file).  
***********************************************************/

byte MAX_TASKS = CC_MAX_TASKS;	
byte MAX_RESOURCES = C_MAX_RESOURCES;
byte MAX_CYCLIC_TIMERS = C_MAX_CYCLIC_TIMERS;
byte MAX_MAILBOXES = C_MAX_MAILBOXES;
byte MAX_QUEUES = C_MAX_QUEUES;
byte MAX_SEMAPHORES = C_MAX_SEMAPHORES;
byte RTC_SCALE = C_RTC_SCALE;
byte TSLICE_SCALE = C_TSLICE_SCALE;

/*******************************************************
 The following sets up the necessary memory needed by CMX
*******************************************************/ 

/******************************************************
 The cxvendor.h header file will actually add additional
 CMX variables to ONLY this file. See that header file.
********************************************************/

struct _tcb cmx_tcb[CC_MAX_TASKS+1];

#if C_MAX_CYCLIC_TIMERS > 0

CYCLIC_TIMERS tcproc[C_MAX_CYCLIC_TIMERS];

#else

byte tcproc[1];

#endif


#if C_MAX_RESOURCES > 0

RESHDR res_que[C_MAX_RESOURCES];

#else

byte res_que[1];

#endif

#if C_MAX_MAILBOXES > 0

MAILBOX mail_box[C_MAX_MAILBOXES];

#else

byte mail_box[1];

#endif

#if C_MAX_MESSAGES > 0

MSG message_box[CMX_MAX_MESSAGES];

#else

byte message_box[1];

#endif


#if C_MAX_QUEUES > 0

QUEHDR queue[C_MAX_QUEUES];

#else

byte queue[1];

#endif

#if C_MAX_SEMAPHORES > 0

SEM sem_array[C_MAX_SEMAPHORES];

#else

byte sem_array[1];

#endif


struct {
	word16 stack_space[CC_TASK_STK_SIZE / 2];
	word16 dummy;
	} stack_storage;
word16 * stack_blk;

struct {
	word16 interrupt_bytes[C_INTERRUPT_SIZE / 2];
	word16 dummy;
	} int_storage;
word16 * interrupt_stack;
word16 * stack_holder;

CYCLIC_LNK cyclic_buf;
CYCLIC_LNK *cyclic_lnk;
TSK_TIMER_LNK *tsk_timer_lnk;
TSK_TIMER_LNK tsk_timer_buf;
MSG *message_ptr;
word16 message_free;

byte tslice_count;
byte SLICE_ON;

struct _tcb * activetcb;
byte active_priority;
tcbpointer timertask;
byte rtc_count;

PIPE_STRUC *out_ptr;

PIPE_STRUC pipe[C_PIPE_SIZE];


#if CMX_RAM_INIT > 0
static void clear_ram(void *,word16);
static void set_magic_ram(void *addr_ptr,word16 size);
static void init_cmx_variables(void);
#endif

#if CMXTRACKER_ENABLE > 0
byte CMXTRACKER_ACTIVE;
byte CMXTRACKER_ON;
struct _tcb *previoustcb;
extern byte cmxtracker_slot;
extern void cmxtracker(void);
extern void cmxtracker_mode(byte,word16);
extern word16 cmxtracker_ctr;
extern byte *cmxtracker_in_ptr;
extern word16 num_records;
void init_cmxtracker(void);
byte cmxtracker_array[CMXTRACKER_SIZE];
#if (!defined(WINTRACKER))
#define LOG_DISPLAY_LEN 18
word16 rec_cnt[((sizeof (cmxtracker_array) / 4) / LOG_DISPLAY_LEN) + 2];
#endif
#endif

#if CMXBUG_ENABLE > 0
long stat_array[CC_MAX_TASKS+1];
#endif
#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
char *task_name[CC_MAX_TASKS + 1];
byte BUG_WAIT_TICKS;
void setup_bug(void);
byte K_Bug_Getchr(byte *ptr);
void K_Bug_Putchr(char);
#endif

word32 cmx_tick_count;	/* total number of CMX system ticks accumulated */
#if (defined(WINTRACKER))
word32 track_tick_count;	/* total number of CMX system ticks accumulated */
word16 compute_tick_count;
#endif
                   
#pragma section

void K_OS_Init(void)
{
#if C_MAX_MESSAGES > 0
	MSG *link;		/* scratch pointer */
#endif

	locked_out = 1;	/* set lock so it will never goto K_I_Scheduler */
	cmx_tick_count = 0;	/* reset CMX long counter */
#if (defined(WINTRACKER))
	track_tick_count = 0;
	compute_tick_count = 0;
#endif

#if CMX_RAM_INIT > 0
	init_cmx_variables();
#endif

#if C_MAX_MESSAGES > 0
	link = message_box; /* address of 1st block */
	message_free = CMX_MAX_MESSAGES;	/* use pipe_max to hold count */
	while (message_free--)
		{
		message_ptr = link;	/* point head at 1st block */
		link++;		/* compute addr of next block */
		message_ptr->env_link = link;	/* create link to it */
		}
	message_ptr->env_link = message_box;	/* last link in chain is null */
	message_ptr = message_box;	/* point head at 1st block */
	message_free = CMX_MAX_MESSAGES-2;
#endif

	in_ctr = out_ctr = cmx_flag1 = 0;
	/* set up interrupt pipe. */
	pipe_slots_avail = C_PIPE_SIZE;

	cyclic_lnk = &cyclic_buf;	/* set up cyclic timers time link. */
	cyclic_lnk->ftlink = cyclic_lnk->btlink = (struct _tcproc *)cyclic_lnk;
	tsk_timer_lnk = &tsk_timer_buf;	/* set up task timer link. */
	tsk_timer_lnk->ftlink = tsk_timer_lnk->btlink = (tcbpointer)tsk_timer_lnk;

	/* now set up stack block to release memory to task's stacks as
		they are created. Also interrupt stack if used. */
	stack_blk = &stack_storage.dummy; 
	interrupt_stack = &int_storage.dummy;
	activetcb = timertask = cmx_tcb;  /* dummy tcb */
	timertask->nxttcb = cmx_tcb;		/* set up link. */

#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
	setup_bug();
	BUG_WAIT_TICKS = 10;		/* Number of system ticks to wait, before checking
										to see if the + (plus key) has been received
										by UART. */
#endif

	rtc_count = C_RTC_SCALE;			/* number of timer ticks, before running
												CMX task timer if needed. */
}	/* K_OS_Init */

#if CMX_RAM_INIT > 0
static void init_cmx_variables(void)
{
	MAX_TASKS = CC_MAX_TASKS;	
	MAX_RESOURCES = C_MAX_RESOURCES;
	MAX_CYCLIC_TIMERS = C_MAX_CYCLIC_TIMERS;
	MAX_MAILBOXES = C_MAX_MAILBOXES;
	MAX_QUEUES = C_MAX_QUEUES;
	MAX_SEMAPHORES = C_MAX_SEMAPHORES;
	TSLICE_SCALE = C_TSLICE_SCALE;
	RTC_SCALE = C_RTC_SCALE;
	clear_ram(cmx_tcb,sizeof cmx_tcb);
	set_magic_ram(&stack_storage.stack_space[0],sizeof stack_storage);

#if C_MAX_CYCLIC_TIMERS > 0
	clear_ram(tcproc,sizeof tcproc);
#endif

#if C_MAX_RESOURCES > 0
	clear_ram(res_que,sizeof res_que);
#endif

#if C_MAX_MAILBOXES > 0
	clear_ram(mail_box,sizeof mail_box);
#endif

#if C_MAX_MESSAGES > 0
	clear_ram(message_box,sizeof message_box);
#endif

#if C_MAX_QUEUES > 0
	clear_ram(queue,sizeof queue);
#endif

#if C_MAX_SEMAPHORES > 0
	clear_ram(sem_array,sizeof sem_array);
#endif

#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
	clear_ram(task_name,sizeof task_name);
#endif

#if CMXBUG_ENABLE > 0
	clear_ram(stat_array, sizeof stat_array);
#endif

#if ( CMXTRACKER_ENABLE > 0 && !defined(WINTRACKER) )
	clear_ram(rec_cnt, sizeof rec_cnt);
#endif
}

static void clear_ram(void *addr_ptr,word16 size)
{
	byte *crap_ptr;
	crap_ptr = (byte *)addr_ptr;
	while(size--)
		{
		*crap_ptr++ = 0;
		}
}  

static void set_magic_ram(void *addr_ptr,word16 size)
{
	byte *crap_ptr;
	crap_ptr = (byte *)addr_ptr;
	while(size--)
		{
		*crap_ptr++ = 0xAA;
		}
}

#endif

#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0

void setup_bug(void)
{
	/* Do NOT change the "cmxbug_slot" or "cmxbug", for
		CMXBug expects these names. You could change the priority
		or stack size if need be, but we highly suggest you do not. */

#if CMXBUG_ENABLE > 0 
	K_Task_Create(1,&cmxbug_slot,cmxbug,BUG_STACK_SIZE);	/* create CMXBug task */
	K_Task_Name(cmxbug_slot,"CMXBug");		/* Name for task */
	K_Task_Start(cmxbug_slot);					/* start CMXBug task */
#else
	K_Task_Create(1,&cmxtracker_slot,cmxtracker,BUG_STACK_SIZE);	/* create CMXTracker task */
	K_Task_Name(cmxtracker_slot,"CMXTracker");		/* Name for task */
	K_Task_Start(cmxtracker_slot);					/* start CMXTracker task */
	init_cmxtracker();
#endif
#if (!defined(WINBUG) && !defined(WINTRACKER))
#if __CPU__ > 3
		/* The following is for serial channel 1, H8S/2357 processor */
		MSTPCRL &= 0xBF;	/* clear module stop for SCI 1 */
		SCI1_SMR = 0x00;	/* set BAUD rate to 9600, @ 20MHZ */
		SCI1_BRR = 64;		/* set bit rate register */
		SCI1_SCR = 0x30;	/* Enable reciever and transmitter, NO interrupts */
								/* Note that TDRE bit should now be set to 1 */
#else
		/* The following is for serial channel 0, H83048 processor */
		SCI0_SMR = 0x00;	/* set BAUD rate to 9600, @ 13MHZ */
		SCI0_BRR = 41;		/* set bit rate register */
		SCI0_SCR = 0x30;	/* Enable receiver and transmitter, NO interrupts */
								/* Note that TDRE bit should now be set to 1 */
#endif
#else
		setbaud();
#endif	/* #if (!defined(WINBUG) && !defined(WINTRACKER)) */
}

#if CMXTRACKER_ENABLE > 0
void init_cmxtracker(void)
{
	num_records = 0;
	cmxtracker_in_ptr = cmxtracker_array;
	cmxtracker_ctr = sizeof (cmxtracker_array);
}
#endif

byte K_Bug_Getchr(byte *ptr)
{
#if C_SPY > 0
/*
If using CSPY, it uses the return key to actually inform the simulator
that you have entered a character. So, to enter a "null" character
and return a 0 (indicating no key pressed, you must enter a 'c'
character.
*/

	int c;
	c = getchar();
	if (c == 'c')
		return(0);
	if (c == 0x0A)
		c = '\r';
	*ptr = c & 0x7F;
	return(1);
#else
#if __CPU__ > 3
	if ( SCI1_SSR & 0x40 )
		{
		*ptr = SCI1_RDR & 0x7F;
		SCI1_SSR &= ~0x40;	/* Clear RDRF bit */
		return(1);
		}
	else
		return(0);
#else
	if ( SCI0_SSR & 0x40 )
		{
		*ptr = SCI0_RDR & 0x7F;
		SCI0_SSR &= ~0x40;	/* Clear RDRF bit */
		return(1);
		}
	else
		return(0);
#endif
#endif
}

void K_Bug_Putchr(char c)
{
#if C_SPY > 0
	if (c == '\n')
		c = '\n';
	putchar(c);
#else
#if __CPU__ > 3
	while (!(SCI1_SSR & 0x80))	/* wait for TDRE bit to be set to one */
		;
	SCI1_TDR = c;
	SCI1_SSR &= ~0x80;	/* Now clear TDRE bit */
#else
	while (!(SCI0_SSR & 0x80))	/* wait for TDRE bit to be set to one */
		;
	SCI0_TDR = c;
	SCI0_SSR &= ~0x80;	/* Now clear TDRE bit */
#endif
#endif
}
#endif

byte K_I_Intrp_Pipe_In(byte a, byte b, byte c, word16 d, void *mesg)
{
	PIPE_STRUC *in_ptr;

#if CMXTRACKER_ENABLE > 0
	if ((!(TEST_CMX_ACTIVE)) || (CMXTRACKER_ACTIVE))
		return(K_ERROR);
#else
	if (!(TEST_CMX_ACTIVE))
		return(K_ERROR);
#endif
	PROC_SAVE_INT;	/* disable interrupts */
	if (pipe_slots_avail)
		{
		in_ptr = &pipe[in_ctr];
		--pipe_slots_avail;
		if (++in_ctr == C_PIPE_SIZE)
			in_ctr = 0;
		PROC_RESTORE_INT;	/* enable interrupts */
		DO_INT_PIPE;		/*	Set the do interrupt pipe flag. */
		in_ptr->identifier = a;
		in_ptr->p1 = b;
		switch(a) {
			case 5:
			case 6:
				in_ptr->pipe_u.p3 = d;
				break;
			case 7:
				in_ptr->pipe_u.p4 = mesg;
				break;
			case 8:
				in_ptr->p2 = c;
				in_ptr->pipe_u.p3 = d;
				break;
				}
			return(K_OK);
			}
		else
			{
			PROC_RESTORE_INT;	/* enable interrupts */
			return(K_ERROR);
			}
}

/*********************************************************************
 The following function executes the functions that the interrupts
 place into the interrupt pipe. called only by CMX K_I_Scheduler.
*********************************************************************/
void K_I_Intrp_Pipe_Out(void)
{
	byte parm1;
	byte parm2;

	while(1)
		{
#if CMXTRACKER_ENABLE
		if (!CMXTRACKER_ACTIVE)
			{
			cmxtracker_in1(INT_ACTION);
			}
#endif
		out_ptr = &pipe[out_ctr];
		switch (out_ptr->identifier) {
			case 0:
				K_Task_Wake(out_ptr->p1);
				break;
			case 1:
				K_Task_Wake_Force(out_ptr->p1);
				break;
			case 2:
				K_Task_Start(out_ptr->p1);
				break;
			case 3:
				K_Timer_Stop(out_ptr->p1);
				break;
			case 4:
				K_Timer_Restart(out_ptr->p1);
				break;
			case 5:
				parm1 = out_ptr->p1;
				K_Timer_Initial(parm1,out_ptr->pipe_u.p3);
				break;
			case 6:
				parm1 = out_ptr->p1;
				K_Timer_Cyclic(parm1,out_ptr->pipe_u.p3);
				break;
			case 7:
#if C_MAX_MAILBOXES > 0
				parm1 = out_ptr->p1;
				K_Mesg_Send(parm1,out_ptr->pipe_u.p4);
#endif
				break;
			case 8:
				parm1 = out_ptr->p1;
				parm2 = out_ptr->p2;
				K_Event_Signal(parm1,parm2,out_ptr->pipe_u.p3);
				break;
			case 9:
#if C_MAX_SEMAPHORES > 0
				K_Semaphore_Post(out_ptr->p1);
#endif
				break;
			default:
				/* What should we do here, possibly reset pipe ? to be beginning */
				break;
			}
		PROC_DISABLE_INT;	/* disable interrupts */
		pipe_slots_avail++;
		if (++out_ctr == C_PIPE_SIZE)
			out_ctr = 0;
		if (out_ctr == in_ctr)
			{
			CLR_DO_INT_PIPE;	/* no, reset do interrupt pipe flag. */
			break;		/* exit. */
			}
		PROC_ENABLE_INT;	/* re-enable interrupts and process more. */
		}
	PROC_ENABLE_INT;	/* re-enable interrupts and exit. */
}

/******************************************************************
	K_OS_Tick_Update:  Called from timer interrupt routine to signal that
	the CMX task timer should execute if need be.
	THIS CODE EXECUTES ONLY AT INTERRUPT LEVEL.
**********************************************************************/
void K_OS_Tick_Update(void)
{
	if (TEST_CMX_ACTIVE)	/* see if CMX OS has been entered. */
		{
#if CMXBUG_ENABLE > 0
		if (!CMXBUG_ACTIVE)
			{
			if (activetcb->tcbstate & RUNNING)
				stat_array[activetcb - cmx_tcb] += 1;
			else
				stat_array[0] += 1;
			}
#endif

#if CMXTRACKER_ENABLE
	if (!CMXTRACKER_ACTIVE)
		{
#if (defined(WINTRACKER))
#if CMXTRACKER_TICK_UPDATE
		track_tick_count++;	/* increment long counter */
#endif	/* #if CMXTRACKER_TICK_UPDATE */
#endif	/* #if (defined(WINTRACKER)) */
#endif	/* #if CMXTRACKER_ENABLE */

		cmx_tick_count++;	/* increment long counter */

		if (!(--rtc_count))	/* converts hardware tics to CMX delay tics */
			{
#if CMXTRACKER_ENABLE
			{
			cmxtracker_in1(CMX_TIC_CALL);
			}
#endif
			rtc_count = RTC_SCALE;	/* reset counter with rtc prescaler */
			/* the following will see if a task timer needs servicing or
				a cyclic timer needs servicing. */
			if ((tsk_timer_lnk->ftlink != (tcbpointer)tsk_timer_lnk) ||
				(cyclic_lnk->ftlink != (struct _tcproc *)cyclic_lnk))
				{
				DO_TIMER_TSK;	/* yes, set flag to indicate that CMX timer
									task should execute. */
				}
  			}

			if (TEST_SLICE_ENABLE)	/* see if time slicing active. */
				{
				if (!(--tslice_count))	/* if so, decrement and test time slice
												counter. */
					{
					DO_TIME_SLICE;		/* set flag indicating time count expired. */
					SLICE_DISABLE;		/* disable time slicing. */
					}
				}
#if CMXTRACKER_ENABLE
		}
#if (defined(WINTRACKER))
	else
		compute_tick_count++;
#endif			/* #if (defined(WINTRACKER)) */
#endif			/* #if CMXTRACKER_ENABLE */
		}
}	/* K_OS_Tick_Update */

#if CMXBUG_ENABLE > 0 || CMXTRACKER_ENABLE > 0
byte K_Task_Name(byte task_slot,char *name)
{
	tcbpointer tcbptr;

	if(K_I_Get_Ptr(task_slot,&tcbptr))	/* send address of pointer */
		return(K_ERROR);
	task_name[task_slot] = name;
	return(K_OK);
}
#endif

byte K_OS_Task_Slot_Get(void)
{
	return ((struct _tcb *)activetcb - cmx_tcb);
}

word32 K_OS_Tick_Get_Ctr(void)
{
	word32 tick_count;
	PROC_SAVE_INT;				/* disable interrupts */
	tick_count = cmx_tick_count;
	PROC_RESTORE_INT;			/* restore interrupts */
	return(tick_count);
}

void K_I_Func_Return(void)
{
	/* this function will decrement the task block (lock) counter.
		Also see if the interrupt count is 0 and if so, will
		test to see any of the CMX 5 flags are set, which will then call
		the K_I_Scheduler to invoke possibly a task switch */

#if (defined(WINBUG))
	if (bug_step_one)
		{
		if ((activetcb	- cmx_tcb) != cmxbug_slot)
			{
			if (!(--bug_step_count))
				{
				bug_step_one = 0;
				if (cmx_tcb[cmxbug_slot].tcbstate & WAIT)
					{
					cmx_tcb[cmxbug_slot].tcbstate = RESUME;
					PREEMPTED;	/* yes, set the preempted scheduling flag */
					}
				}
			}
		}
#endif		/* #if (defined(WINBUG)) */
	K_I_Enable_Sched();
}

/*****************************************************************
 The following is the CPU reduced power function. The user
 MUST write their own and they must ensure that the proceesor
 returns to the caller, without modifying the stack.

 NOTE: the CMX K_I_Scheduler calls this function which is in the assembly
 module supplied. The user may find it necessary to modify the
 assembly file, so every thing is correct when the CPU
 comes out of the reduced power state. ALSO the CPU must let
 the interrupts wake it up, out of the reduced power state
 so as the task's timers and cyclic timers may have their
 time counters decremented if need be.
*****************************************************************/
void K_OS_Low_Power_Func(void)
{
}

#if (defined(WINTRACKER))
#if CMXTRACKER_TICK_UPDATE == 0
void K_OS_Update_Track_Tick(void)
{
	if (!CMXTRACKER_ACTIVE)
		{
		track_tick_count++;	/* increment long counter */
		}
}
#endif		/* #if CMXTRACKER_TICK_UPDATE == 0 */
#endif		/* #if (defined(WINTRACKER)) */

