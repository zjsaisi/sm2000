/* for IAR H8/3XXH and H8S */

#define PROCESSOR 8300

#define PROC_DISABLE_INT K_OS_Disable_Interrupts()
#define PROC_ENABLE_INT K_OS_Enable_Interrupts()

#define PROC_SAVE_INT K_OS_Save_Interrupts()
#define PROC_RESTORE_INT K_OS_Restore_Interrupts()

typedef void (*CMX_FP)(void); 
/* or 
typedef void (far *CMX_FP)(void); 
typedef void (*CMX_FP)(void); 
*/

/* load in tasks STACK address. */
/* adjust stack memory block. */

#define CXTCRE_SPECIFIC() {tcbptr->stk_start = stack_blk; stack_blk -= stack_size / 2; } 

#ifdef CMX_INIT_MODULE 

byte locked_out;			/* disable task switching */
byte pipe_slots_avail;	/* slots free in CMX interrupt pipe */ 
byte in_ctr;				/* position within interrupt pipe to place bytes */
byte out_ctr;				/* position within interrupt pipe to remove bytes */
byte cmx_flag1;			/* contains flags, for CMX internal use */
byte int_count;			/* interrupt count */
byte ie_holder;

#else

extern byte locked_out;	
extern byte pipe_slots_avail;
extern byte in_ctr;
extern byte out_ctr;
extern byte cmx_flag1;
extern byte int_count;
extern byte ie_holder;

#if (defined(CMXTRACKER) || defined(WINTRACKER))
extern byte CMXTRACKER_ON;
#endif

#endif

#define preempted 0x01		/* preempted flag */
#define do_timer_tsk 0x02	/* do timer task flag */
#define do_time_slice 0x04	/* do time slice, next task to slice */
#define slice_enable	0x08	/* time slicing enabled */
#define do_coop_sched 0x10	/* do a cooperative schedule, to NEXT task that can run */
#define do_int_pipe 0x20	/* process interrupt pipe */
#define idle_flag 0x40		/* helps determines power down mode */
#define cmx_active 0x80		/* identifies that CMX RTOS entered */


#define TEST_NOT_PREEMPTED !(cmx_flag1 & preempted)
#define CMX_ACTIVE cmx_flag1 |= cmx_active
#define TEST_CMX_ACTIVE cmx_flag1 & cmx_active
#define CLR_DO_INT_PIPE cmx_flag1 &= ~do_int_pipe
#define TEST_SLICE_ENABLE cmx_flag1 & slice_enable

/* 
	The following must use instructions that are ATOMIC. If NOT then
   CMX must go out side it's normal thinking and put a fence around
	the bit setting and clearing within the cmx_flag1 variable. This
	is because the CMX functions int_pipe and/or K_OS_Tick_Update can be called 
	at any time by an interrupt, thus setting the CMX 'do_int_pipe' flag 
	or 'do_timer_tsk' flag and possibly having the flag NOT be taken, 
	for instructions that were manipulating the cmx_flag1 variable was 
	going to write it back, thus destroying the flags!

	Note that CMX tries to use the processor memory area that allows 
	manipulation of bits, using one of the instructions, thus making it
	atomic. Some times the compiler will not either allow CMX to take 
	advantage of this memory are OR because of optimization (none or 
	little that is).

	The user should check the code generated by compiler (and compiler
	options [optimize, etc] used), to ensure that ATOMIC instructions
	are used. Most likely, you will see instructions that either set or 
	clear the cmx_flag1 bits directly or instructions that ACT on the 
	cmx_flag1 variable directly such as follows:
		AND instructions (AND,ANL, etc) cmx_flag1,#??h. 
		OR instructions (OR,ORL, etc) cmx_flag1,#??h.

	Failure to ensure this may result in stange behavior!
  
*/

#if __CPU__ > 3	/* H8S */
#define ATOMIC 1	/* if 0, then FENCE will be placed around cmx_flag1 
							bit manipulation coding. Normally will be 1 (one). */
#else
#define ATOMIC 0
#endif


#if ATOMIC > 0

#define PREEMPTED cmx_flag1 |= preempted
#define DO_TIMER_TSK cmx_flag1 |= do_timer_tsk
#define DO_TIME_SLICE cmx_flag1 |= do_time_slice
#define SLICE_DISABLE cmx_flag1 &= ~slice_enable
#define SLICE_ENABLE	cmx_flag1 |= slice_enable
#define DO_COOP_SCHED cmx_flag1 |= do_coop_sched
#define DO_INT_PIPE cmx_flag1 |= do_int_pipe

#else

#define PREEMPTED {K_OS_Save_Interrupts(); cmx_flag1 |= preempted; K_OS_Restore_Interrupts();}
#define DO_TIMER_TSK {K_OS_Save_Interrupts(); cmx_flag1 |= do_timer_tsk; K_OS_Restore_Interrupts();}
#define DO_TIME_SLICE {K_OS_Save_Interrupts(); cmx_flag1 |= do_time_slice; K_OS_Restore_Interrupts();}
#define SLICE_DISABLE {K_OS_Save_Interrupts(); cmx_flag1 &= ~slice_enable; K_OS_Restore_Interrupts();}
#define SLICE_ENABLE	{K_OS_Save_Interrupts(); cmx_flag1 |= slice_enable; K_OS_Restore_Interrupts();}
#define DO_COOP_SCHED {K_OS_Save_Interrupts(); cmx_flag1 |= do_coop_sched; K_OS_Restore_Interrupts();}
#define DO_INT_PIPE {K_OS_Save_Interrupts(); cmx_flag1 |= do_int_pipe; K_OS_Restore_Interrupts();}

#endif
