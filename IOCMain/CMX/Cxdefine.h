#ifdef byte
#undef byte
#endif
#ifdef word16
#undef word16
#endif
#ifdef word32
#undef word32
#endif
typedef unsigned short word16;
#ifndef byte
typedef unsigned char byte;
#endif
typedef unsigned int bit_word16;
typedef signed short sign_word16;
typedef unsigned long word32;
#ifndef NULL
#define NULL 0x00
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#define RESOURCE 0x01	/* waiting on resource */
#define WAITCCC 0x02	/* wait for wake */
#define SEND_MESG 0x08	/* waiting for task that recieve message to wake me */
#define WAIT_MESG 0x10	/* waiting on message */
#define FLAGS	0x20	/* waiting for task user flags */
#define TIME_EXPIRED 0x40	/* flag indicating time expired */
#define SEMAPHORE	0x80		/* waiting for semaphore */

#define IDLE 0x0100	/* the task is idle state */
#define READY 0x0200 	/* the task is ready to run (from beginning) */
#define RESUME 0X0400	/* the task is ready to run (not from beginning) */
#define RUNNING 0x0800	/* the task is running */
#define TIME 0x1000	/* wait on time */

#define ANY_WAIT (WAITCCC|SEND_MESG|WAIT_MESG|FLAGS|RESOURCE|SEMAPHORE)


/* status return values */
#define K_OK 0x00		/* successful call */
#define K_TIMEOUT 0x01	/* warning, time period expired or K_Task_Wake_Force was used */
#define K_NOT_WAITING 0x02	/* error: task not waiting */
#define K_RESOURCE_OWNED 0x05	/* error: task does not own resource */
#define K_RESOURCE_NOT_OWNED 0x06 	/* error: resource already owned */
#define K_QUE_FULL 0x0a	/* error: queue is full */
#define K_QUE_EMPTY 0x0b	/* error: queue is empty */
#define K_SEMAPHORE_NONE 0x0c		/* no semaphore posted, returned (only cxsemget call */	
#define K_ERROR 0xff			/* error: general error value */

#define K_I_Disable_Sched()   (++locked_out) /* disable task switching */
#define K_I_Enable_Sched() {if ((cmx_flag1 & 0x37) && (locked_out == 1)) K_I_Sched(); else --locked_out; } 

