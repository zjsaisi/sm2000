#ifndef __cxfuncs_h_defined
#define __cxfuncs_h_defined

#include <cxdefine.h>
#include <cxvendor.h>
#include <cxstruct.h>

#ifdef __cplusplus
extern "C" {
#endif

byte K_Task_Create(byte,byte *,CMX_FP,word16);	/* create task. */
byte K_Task_Create_Stack(byte,byte *,CMX_FP,word16 *); /* create task, using passing stack address */
byte K_Task_Start(byte);		/* start task */
byte K_Task_Wait(word16);	/*	have task wait, with or without time period */
byte K_I_Wake_Common(byte,byte);	/* common wake routine */	
#define K_Task_Wake(A) K_I_Wake_Common(A,0)	 /* wake a task that was waiting. */
#define K_Task_Wake_Force(A) K_I_Wake_Common(A,1) /* wake a task, regardless of what task waiting on */
byte K_Task_Priority(byte,byte);	/* change a task priority. */
void K_Task_Lock(void);		/* disable task switching */
void K_Task_Unlock(void);		/* re-enable task switching. */
void K_Task_Coop_Sched(void);		/* do a cooperative task switch. */
void K_Task_End(void);		/* must be called by all task prior to end brace. */
byte K_Task_Delete(byte);		/* remove a task. */
byte K_Task_Name(byte,char *);	/* declare a name for task. */

word16 K_Event_Wait(word16,word16,byte);	/* wait on events, with/without time period */
byte K_Event_Signal(byte,byte,word16);	/* signal an event */
byte K_Event_Reset(byte tskid,word16 event);	/* reset events bits */

byte K_I_Resource_Common(byte,word16,byte);			/* common resource reserve/get function */
#define K_Resource_Wait(A,B) K_I_Resource_Common(A,B,0)	/* reserve a resource, with or without time period */
#define K_Resource_Get(A) K_I_Resource_Common(A,0,1)		/* get a resource if free */
byte K_Resource_Release(byte);		/* release a resource */

byte K_Que_Create(sign_word16,byte,byte *,byte);	/* create a circular queue */
byte K_Que_Reset(byte);	/* reset a queue */
byte K_I_Que_Add_Common(byte,void *,sign_word16);	/* common add to queue routine */
#define K_Que_Add_Top(A,B) K_I_Que_Add_Common(A,B,1)	/* add to queues top */
#define K_Que_Add_Bottom(A,B) K_I_Que_Add_Common(A,B,0)	/* add to queue's bottom */
byte K_I_Que_Get_Common(byte,void *,sign_word16);	/* common remove from queue routine */
#define K_Que_Get_Top(A,B) K_I_Que_Get_Common(A,B,1)	/* remove from queue's top */
#define K_Que_Get_Bottom(A,B) K_I_Que_Get_Common(A,B,0)	/* remove from queue's bottom */

void * K_I_Mesg_Wait_Common(byte,word16,byte);		/* common message get/wait function */
#define K_Mesg_Wait(A,B) K_I_Mesg_Wait_Common(A,B,0)	/* wait for message, with or without time period */
#define K_Mesg_Get(A) K_I_Mesg_Wait_Common(A,0,1)		/* get a message if one is available */
byte K_I_Mesg_Send_Common(byte,word16,void *,byte);	/* common message send/wait function */
#define K_Mesg_Send(A,B) K_I_Mesg_Send_Common(A,0,B,0)	/* send message to a mailbox */
#define K_Mesg_Send_Wait(A,B,C) K_I_Mesg_Send_Common(A,B,C,1)	/* send message, wait for reciever to ACK */
byte K_Mbox_Event_Set(byte,byte,word16);		/* mailbox event setting parameters */
byte K_Mesg_Ack_Sender(void);		/* wake task that sent message */

byte K_Timer_Create(byte,byte,byte,word16);	/* create cyclic timer */
byte K_I_Cyclic_Common(byte,word16,word16,byte);	/* common function for next 4 functions */
#define K_Timer_Start(A,B,C) K_I_Cyclic_Common(A,B,C,3)	/* start cyclic timer */
#define K_Timer_Restart(A) K_I_Cyclic_Common(A,0,0,0)		/* re-start cyclic timer */
#define K_Timer_Initial(A,B) K_I_Cyclic_Common(A,B,0,1)		/* re-start timer, with new initial time */
#define K_Timer_Cyclic(A,B) K_I_Cyclic_Common(A,0,B,2)		/* re-start timer, with new cyclic time */
byte K_Timer_Stop(byte);				/* stop cyclic timer */

void K_Mem_FB_Create(void *,word16,word16);	/* create fixed memory block */
byte K_Mem_FB_Get(void *,byte **);			/* get a block of memory */
void K_Mem_FB_Release(void *,byte *); 			/* release a block of memory */

byte K_I_Semaphore_Get_Common(byte,word16,byte);
#define K_Semaphore_Get(A) K_I_Semaphore_Get_Common(A,0,1)
#define K_Semaphore_Wait(A,B) K_I_Semaphore_Get_Common(A,B,0)
byte K_Semaphore_Post(byte);
byte K_Semaphore_Create(byte,word16);
byte K_Semaphore_Reset(byte,byte);	
byte K_OS_Task_Slot_Get(void);
word32 K_OS_Tick_Get_Ctr(void);

void K_OS_Slice_On(void);		/* Enable time slicing */
void K_OS_Slice_Off(void);		/* Disable time slicing */

void K_OS_Init(void);		/* initialize CMX */
void K_OS_Low_Power_Func(void);	/* reduced CPU power down function */
void K_I_Intrp_Pipe_Out(void);	/* executes the interrupts CMX function requests */
void K_OS_Start(void);		/* enter CMX OS */

void K_I_Timer_Task(void);	/* CMX's timer task */
void K_OS_Tick_Update(void);		/* called by interrupt, system tick */
void K_OS_Update_Track_Tick(void);

/****************************************************
 The following functions are for the CMX UART module
****************************************************/
void K_Init_Xmit(void);		/* initialize transmitter and buffer */
void K_Init_Recv(void);		/* initailize reciever and buffer */
void K_Update_Xmit(void);	/* called by transmitter interrupt handler. */
void K_Update_Recv(void);	/* called by reciever interrupt handler. */
byte K_Put_Str(void *,word16);	/* put chars into transmit buffer */
byte K_Put_Str_Wait(void *,word16,word16);	/* same as above and wait if need be*/
byte K_Put_Char(void *);		/* put 1 char into transmit buffer */
byte K_Put_Char_Wait(void *,word16);	/* same as above and wait if need be */
word16 K_Get_Str(void *,word16);	/* get chars from reciever buffer */
word16 K_Get_Str_Wait(void *,word16,word16); /* same as above with wait */
word16 K_Get_Char(void *);	/* get single char from recieve buffer */
word16 K_Get_Char_Wait(void *,word16); /* same as above with wait */
word16 K_Get_Str_Wait_Return(void *,word16,word16); /* get requested number with wait */
word16 K_Get_Str_Return(void *);	/* get variable number from buffer */
word16 K_Recv_Count(void);	/* count number of characters in receive buffer */
/***************************************************************
	external assembly routines
***************************************************************/
void K_OS_Disable_Interrupts(void);	/* clears JUST I BIT */
void K_OS_Enable_Interrupts(void);		/* sets JUST I BIT */
void K_OS_Save_Interrupts(void);	/* clears JUST I BIT */
void K_OS_Restore_Interrupts(void);		/* sets JUST I BIT */

extern void K_I_Sched(void);
extern void K_I_Scheduler(void);
extern void K_OS_Intrp_Entry(void);
extern void K_OS_Intrp_Exit(void);

/*******************************************************
* the following functions are not to be called by user *
* used ONLY by other cmx functions  						 *
*******************************************************/

void K_I_Unlink(tcbpointer);
void K_I_Priority_In(tcbpointer,byte);
void K_I_Copy(byte *,byte *,byte);
byte K_I_Get_Ptr(byte,tcbpointer *);
void K_I_Func_Return(void);
byte K_I_Time_Common(word16,byte);

byte K_I_Intrp_Pipe_In(byte,byte,byte,word16,void *);
#define K_Intrp_Task_Wake(A) K_I_Intrp_Pipe_In(0,A,0,0,0)
#define K_Intrp_Task_Wake_Force(A) K_I_Intrp_Pipe_In(1,A,0,0,0)
#define K_Intrp_Task_Start(A) K_I_Intrp_Pipe_In(2,A,0,0,0)
#define K_Intrp_Timer_Stop(A) K_I_Intrp_Pipe_In(3,A,0,0,0)
#define K_Intrp_Timer_Restart(A) K_I_Intrp_Pipe_In(4,A,0,0,0)
#define K_Intrp_Timer_Initial(A,B) K_I_Intrp_Pipe_In(5,A,0,B,0)
#define K_Intrp_Timer_Cyclic(A,B) K_I_Intrp_Pipe_In(6,A,0,B,0)
#define K_Intrp_Mesg_Send(A,B) K_I_Intrp_Pipe_In(7,A,0,0,B)
#define K_Intrp_Event_Signal(A,B,C) K_I_Intrp_Pipe_In(8,A,B,C,0)
#define K_Intrp_Semaphore_Post(A) K_I_Intrp_Pipe_In(9,A,0,0,0)

#ifdef __cplusplus
}
#endif

#endif
