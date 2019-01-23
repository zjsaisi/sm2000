#define CMXMODULE 1

#include <stddef.h>
#include <cxfuncs.h>		/* get cmx include header file */
#include <cxextern.h>	/* get cmx include header file */
#include <winio.h>

#define CMX_SEMAPHORE_ENABLE 1 /* Set to one, for CMX-RTX ver 5.xx */
#define RECORD_XMT_REC 0	/* Set to 1, to record transmit and receive stuff */

/* You should not have to change any of the following. */

byte VERSION[6] = {" 1.00"};

/* What services availble in this version.   */
#define A_TASKS 		0x01
#define A_RESOURCES 	0x02
#define A_CYCLIC_TIMERS 0x04
#define A_MAILBOXES     0x08
#define A_QUEUES        0x10 
#define A_RTC_SCALE     0x20
#define A_TSLICE_SCALE  0x40
#if CMX_SEMAPHORE_ENABLE == 1
#define A_SEMAPHORES    0x80	
#else
#define A_SEMAPHORES    0x00	
static byte MAX_SEMAPHORES = 0;
#endif
#define BYTE1 (A_TASKS|A_RESOURCES|A_CYCLIC_TIMERS|A_MAILBOXES|A_QUEUES|A_RTC_SCALE|A_TSLICE_SCALE|A_SEMAPHORES)
#define BYTE2			0
#define BYTE3			0
#define BYTE4			0

static byte cmx_code tcb_size[] = { 
	sizeof(tcbpointer),	/* fwlink size */
	sizeof(tcbpointer),	/* bwlink size */
	sizeof(tcbpointer),	/* fwlink size */
	sizeof(tcbpointer),	/* bwlink size */
	sizeof(word16),		/* tcbstate size */
	sizeof(byte),		/* trigger size */
	sizeof(byte),		/* priority size */
	sizeof(word16),		/* timer size */
	sizeof(tcbpointer),	/* nxttcb size */
	sizeof(CMX_FP),		/* task_addr size */
	sizeof(word16 cmx_xdata *),	/* stack start size */
	sizeof(word16 cmx_xdata *),	/* stack save size */
	sizeof(tcbpointer),	/* mesg sender size */
	sizeof(word16),		/* event flags size */
	sizeof(word16),		/* event match size */
	(byte)offsetof(struct _tcb,fwlink),
	(byte)offsetof(struct _tcb,bwlink),
	(byte)offsetof(struct _tcb,ftlink),
	(byte)offsetof(struct _tcb,btlink),
	(byte)offsetof(struct _tcb,tcbstate),
	(byte)offsetof(struct _tcb,trig),
	(byte)offsetof(struct _tcb,priority),
	(byte)offsetof(struct _tcb,tcbtimer),
	(byte)offsetof(struct _tcb,nxttcb),
	(byte)offsetof(struct _tcb,task_addr),
	(byte)offsetof(struct _tcb,stk_start),
	(byte)offsetof(struct _tcb,stk_save),
	(byte)offsetof(struct _tcb,mesg_sender),
	(byte)offsetof(struct _tcb,e_flags),
	(byte)offsetof(struct _tcb,e_match)
  };

static byte cmx_code resource_size[] = {
	sizeof(tcbpointer), /* fwlink size */
	sizeof(tcbpointer), /* bwlink size */
	sizeof(tcbpointer), /* owner size */
	(byte)offsetof(struct cmxresource,fwlink),
	(byte)offsetof(struct cmxresource,bwlink),
	(byte)offsetof(struct cmxresource,owner)
	};

static byte cmx_code mailbox_size[] = {
	sizeof (MSG cmx_xdata *),	/* link to message block. */
	sizeof (MSG cmx_xdata *),	/* link to message block. */
	sizeof (tcbpointer),		/* what task is waiting for message. */
	sizeof (byte),				/* task number for setting event bit */
	sizeof (word16),			/* event number */
	(byte)offsetof(struct cmxmbox,first_lnk),
	(byte)offsetof(struct cmxmbox,next_lnk),
	(byte)offsetof(struct cmxmbox,waiter),
	(byte)offsetof(struct cmxmbox,task_num),
	(byte)offsetof(struct cmxmbox,event_num)
	};

static byte cmx_code queue_size[] = {
	sizeof (byte cmx_xdata *),	/* address of user supplied memory for queue. */
	sizeof (word16),		/* the number of slots within this queue */
	sizeof (word16),		/* the number of slots used */
	sizeof (byte),			/* size of slots */
	sizeof (signed int),	/* must be signed to test for < 0 */
	sizeof (signed int),	/* must be signed to test for < 0 */
	(byte)offsetof(struct cmxqueue,base_ptr),
	(byte)offsetof(struct cmxqueue,num_slots),
	(byte)offsetof(struct cmxqueue,queue_cnt),
	(byte)offsetof(struct cmxqueue,size_slot),
	(byte)offsetof(struct cmxqueue,head),
	(byte)offsetof(struct cmxqueue,tail)
	};

/* the timed procedure structure */
static byte cmx_code cyclic_size[] =  {
	sizeof (struct _tcproc cmx_xdata *),	/* forward time link */
	sizeof (struct _tcproc cmx_xdata *),	/* backward time link */
	sizeof (byte),		/* byte indicating timed procedure stopped / started */
	sizeof (word16),	/* the timer counter */	
	sizeof (word16),	/* the cyclic time that will be reloaded */	
	sizeof (byte),		/* what mode to sent to K_Event_Signal function. */
	sizeof (byte),		/* task slot number or priority (may not be used). */
	sizeof (word16),	/* the event to set. */
	(byte)offsetof(struct _tcproc,ftlink),
	(byte)offsetof(struct _tcproc,btlink),
	(byte)offsetof(struct _tcproc,tproc_start),
	(byte)offsetof(struct _tcproc,tproc_timer),
	(byte)offsetof(struct _tcproc,reload_time),
	(byte)offsetof(struct _tcproc,mode),
	(byte)offsetof(struct _tcproc,tskid_pri),
	(byte)offsetof(struct _tcproc,event_num)
	};

#if CMX_SEMAPHORE_ENABLE == 1
/* the semaphore structure */
static byte cmx_code semaphore_size[] =  {
	sizeof(tcbpointer), /* fwlink size */
	sizeof(tcbpointer), /* bwlink size */
	sizeof (word16),	/* the semaphore counter */	
	sizeof (word16),	/* the semaphore N count (maximum) */	
	(byte)offsetof(struct semaphore,fwlink),
	(byte)offsetof(struct semaphore,bwlink),
	(byte)offsetof(struct semaphore,sem_count),
	(byte)offsetof(struct semaphore,sem_n)
	};
#endif

#define IN_BUFF_LEN 20

#define COMMAND_LOCATION 0x00
#define MAJOR_LOCATION 0x00
#define MINOR_LOCATION 0x01
#define WORKING_NUM_LOCATION 0x02
#define DATA_LOC1 0x02
#define DATA_LOC2 0x03

#define OUT_BUFF_LEN 256+5		/* 5 = START1, start2, COUNT, CRC */

#define MAX_DUMMY 0x00

#define DUMP_TARGET 0x10
#define DUMP_TASK_SIZE 0x11
#define DUMP_RESOURCE_SIZE	0x12
#define DUMP_MAILBOX_SIZE 0x13
#define DUMP_CYCLIC_SIZE 0x14
#define DUMP_QUEUE_SIZE 0x15
#define TICK_COMPUTE 0x16
#define DUMP_SEMAPHORE_SIZE 0x17

#define DUMP_TASK_NAME 0x20
#define FREEZE 0x22
#define UNFREEZE 0x23

#define DUMP_TASK 0x30
#define DUMP_RESOURCES 0x31
#define DUMP_CYCLIC 0x32
#define DUMP_QUEUES 0x33
#define DUMP_MAILBOXES 0x34
#define CHANGE_RTC_SCALE 0x35
#define CHANGE_TSLICE_SCALE 0x36
#define ENABLE_TIME_SLICING 0x37
#define DELAY_TICK 0x38
#define QUICK_TICK 0x39
#define DISABLE_TIME_SLICING 0x3A
#define TIME_SLICING_STATE 0x3B
#define DUMP_SEMAPHORE 0x3C
#define DELAY_NUM_FUNCS 0x3D
#define QUICK_NUM_FUNCS 0x3E

#define FREEZE_ACK 0x90
#define BUG_CONNECT2	0xA0
#define BUG_REFRESH	0xA1

/* NOT IN AS OF YET, IF EVER */
/*
#define STATS 13
#define STATS_RESET 14
#define TASK_CTRL 15
#define DUMP_RAM 6
#define DUMP_STACKS 7
#define EXIT_BUG 99
*/


#define COMM_START1  'z'
#define COMM_START2  'U'
#define COMM_ERR 0x00
#define INSERT_CHAR 0x0F

#define GOOD 0x80
#define RANGE_ERROR 0x81

/* keyboard flags */
typedef struct in_commflag {	 /* bit structure */
	bit_word16 start1:1;
	bit_word16 start2:1;
	bit_word16 count:1;
	bit_word16 csum1:1;
	bit_word16 csum2:1;
	bit_word16 insert:1;
	bit_word16 transmitting:1;
	bit_word16 err:1;
	bit_word16 compute_tick:1;
	} IN_COMMFLAG;

static struct in_commflag cmx_xdata commflag;
static byte cmx_xdata in_com_buff[IN_BUFF_LEN];
static byte cmx_xdata *in_com_ptr;
static byte cmx_xdata in_com_state;
static word16 cmx_xdata in_com_ctr;
static word16 cmx_xdata in_csum;
static word16 cmx_xdata computed_csum;
byte cmx_xdata cmxbug_slot;
static byte cmx_xdata endian;

static byte cmx_xdata *out_com_ptr;
static byte cmx_xdata out_com_buff[OUT_BUFF_LEN];
static byte cmx_xdata out_count;
static byte cmx_xdata in_char;
static word16 cmx_xdata out_csum;
static byte cmx_xdata bug_freeze;
static byte cmx_xdata bug_nodelay;
byte cmx_xdata bug_step_one;
word16 cmx_xdata bug_step_count;
static volatile byte cmx_xdata cmxbug_process;
static byte cmx_xdata working_num;
static RESHDR cmx_xdata res_dummy[1];
static byte cmx_xdata bug_refresh = FALSE;

#if RECORD_XMT_REC > 0
byte cmx_xdata testrec_buff[1000];
byte cmx_xdata *testrec_ptr;

byte cmx_xdata testxmt_buff[1000];
byte cmx_xdata *testxmt_ptr;

#endif

extern long cmx_xdata stat_array[];
extern byte cmx_xdata CMXBUG_ACTIVE;
extern byte cmx_xdata BUG_WAIT_TICKS;
extern char * cmx_xdata task_name[];
static void bug2_getchr1(void) cmx_reentrant;
static void gencopy(void *,byte) cmx_reentrant;
static byte insert_rtn(void) cmx_reentrant;


static void setup_outbuff(void) cmx_reentrant;

void cmxbug(void) cmx_reentrant;
static void parse_packet(void) cmx_reentrant;
static void dump_task(void) cmx_reentrant;
static void dump_target(void) cmx_reentrant;
static void dump_task_name(void) cmx_reentrant;
static void dump_resource(void) cmx_reentrant;
static void dump_cyclic(void) cmx_reentrant;
static void dump_queue(void) cmx_reentrant;
static void dump_mailbox(void) cmx_reentrant;
static void change_rtc(void) cmx_reentrant;
static void change_tslice(void) cmx_reentrant;
static void tslice_state(void) cmx_reentrant;
static void tslice_on_off(byte) cmx_reentrant;
static void quick_tick(byte) cmx_reentrant;
static void function_tick(byte) cmx_reentrant;
static void comm_error(void) cmx_reentrant;
static void compute_csum(void) cmx_reentrant;
static void dump_task_size(void) cmx_reentrant;
static void dump_resource_size(void) cmx_reentrant;
static void dump_mailbox_size(void) cmx_reentrant;
static void dump_queue_size(void) cmx_reentrant;
static void dump_cyclic_size(void) cmx_reentrant;
void cmxbug_activate(void) cmx_reentrant;
static void refresh_request(void) cmx_reentrant;

#if CMX_SEMAPHORE_ENABLE == 1
static void dump_semaphore(void) cmx_reentrant;
static void dump_semaphore_size(void) cmx_reentrant;
#endif
static void freeze_bug(void) cmx_reentrant;
static void unfreeze_bug(void) cmx_reentrant;
static void compute_tick_func(void) cmx_reentrant;
static byte K_Bug_Getchr1(byte *);
static void freeze_acknowledge(void) cmx_reentrant;

static byte K_Bug_Getchr1(byte *ptr)
{
	TEST_REC();
}


static void bug2_getchr1(void)
cmx_reentrant {    

	if (K_Bug_Getchr1(&in_char))
		{     
#if RECORD_XMT_REC > 0
			*testrec_ptr = in_char;
			if (++testrec_ptr == &testrec_buff[sizeof (testrec_buff)])
				testrec_ptr = testrec_buff;
#endif
		if (!cmxbug_process)
			{
			if (in_com_state < 2 && (in_char != COMM_START1 && in_char != COMM_START2))
				{
				comm_error();
				}
			switch (in_com_state)
				{
				case 0:
					if (in_char == COMM_START1)
						{
						commflag.start1 = 1;
						in_com_state++;
						}
					else
						{
						comm_error();
						}
					break;
				case 1:
					if (in_char == COMM_START2)
						{
						commflag.start2 = 1;
						in_com_state++;
						}
					else
						{
						comm_error();
						}
					break;
				default:
					if (in_com_state == 0x02)
						{
						if (in_char)
							{
							in_com_ctr = in_char;
							commflag.count = 1;
							in_com_state++;
							}
						else
							{
							comm_error();
							}
						break;
						}
					if (insert_rtn())
						break;
					if (commflag.csum2)
						{
						/* check csum and decide, if good continue */
						in_csum |= in_char;
						if (computed_csum == in_csum)
							{
							cmxbug_process = TRUE;
							}
						comm_error();	/* NO error, just way to reset */
						}
					else if (commflag.csum1)
						{
						commflag.csum2 = 1;
						in_csum = in_char << 8;
						}
					else 
						{
						if (in_com_ptr < &in_com_buff[IN_BUFF_LEN])
							*in_com_ptr++ = in_char;
						else
							{
							comm_error();
							return;
							}
						computed_csum += in_char;
						if (--in_com_ctr)
							break;
						else
							commflag.csum1 = 1;
						}
					break;
				}
			}
		}
}

static void compute_csum(void)
cmx_reentrant {  
	--out_com_ptr; /* decrement by 1, for pointing to next free */
	out_com_buff[2] = out_com_ptr - &out_com_buff[2];	/* COUNT */
	out_com_ptr = &out_com_buff[2];
	out_csum = 0;
	out_count = *out_com_ptr++;
	do {
		out_csum += *out_com_ptr++;
		} while(--out_count);
	*out_com_ptr++ = (byte)(out_csum >> 8);
	*out_com_ptr++ = (byte)out_csum;
	out_count = out_com_ptr - out_com_buff;
	out_com_ptr = out_com_buff; 
	commflag.transmitting = TRUE; 
	XMT_PROLOGUE();
	ENABLE_XMT();
	while (out_count--)
		{
#if RECORD_XMT_REC > 0
		*testxmt_ptr = *out_com_ptr;
		if (++testxmt_ptr == &testxmt_buff[sizeof (testxmt_buff)])
			testxmt_ptr = testxmt_buff;
#endif
		TEST_XMT();
		UART_XMT();		
		}
/*	TEST_XMT(); */
	XMT_EPILOGUE();
	commflag.transmitting = FALSE;
}

static byte insert_rtn(void)
cmx_reentrant {
	if (commflag.insert)
		{
		if (in_char == COMM_START2)
			{
			comm_error();
			return(1);
			}
		if (in_char == INSERT_CHAR)
			{
			commflag.insert = FALSE;
			return(1);
			}
		else
			commflag.insert = FALSE;
		}
	if (in_char == COMM_START1)
		commflag.insert = TRUE;
	else
		commflag.insert = FALSE;
	return(0);
}

static void comm_error(void)
cmx_reentrant {
	in_com_state = COMM_ERR;
	in_com_ctr = 0;
	in_com_ptr = in_com_buff;
	commflag.start1 = 0;
	commflag.start2 = 0;
	commflag.count = 0;
	commflag.csum2 = 0;
	commflag.csum1 = 0;
	computed_csum = 0;
	commflag.insert = FALSE;
}

void cmxbug_activate(void)
cmx_reentrant {
	if (!bug_freeze)
		{
		bug_nodelay = 1;
		}
	K_Task_Wake(cmxbug_slot);
}

void cmxbug(void)
cmx_reentrant {

	byte key_pressed;

#if RECORD_XMT_REC > 0
	testrec_ptr = testrec_buff;
	testxmt_ptr = testxmt_buff;
#endif

	/* calculate if big or little endian */
	in_csum = 0xAA55;
	in_com_ptr = (byte cmx_xdata *)&in_csum;
	if (*in_com_ptr == 0xAA)
		endian = 1;		/* big endian */
	else
		endian = 0;
	comm_error();
	commflag.compute_tick = FALSE;
	bug_freeze = 0;
	while(1)
		{
		if (!bug_freeze)
			{
			if (bug_refresh)
				{
				bug_refresh = FALSE;
				refresh_request();
				}
			if(K_Bug_Getchr1(&key_pressed))
				{
#if RECORD_XMT_REC > 0
				*testrec_ptr = key_pressed;
				if (++testrec_ptr == &testrec_buff[sizeof (testrec_buff)])
					testrec_ptr = testrec_buff;
#endif
				if (key_pressed == '+')
					{
					in_com_buff[COMMAND_LOCATION] = FREEZE_ACK;
					cmxbug_process = TRUE;
					bug_freeze = 1;	
					bug_nodelay = 0;
					goto cmxbug_1;
					}
				}
			if (!bug_nodelay)
				{
				K_Task_Wait(BUG_WAIT_TICKS);
				}
			}
		else
			{
			bug2_getchr1();
			}
cmxbug_1:
		if (cmxbug_process)
			{
			parse_packet();
			cmxbug_process = FALSE;
			comm_error();	/* NO error, just way to reset */
			}
		}
}

static void parse_packet(void)
cmx_reentrant {
	setup_outbuff();
	working_num = in_com_buff[WORKING_NUM_LOCATION];
	in_com_ptr = &in_com_buff[COMMAND_LOCATION];
	switch(*in_com_ptr) {
		case DUMP_TARGET:
			dump_target();
			break;
		case DUMP_TASK_SIZE:
			dump_task_size();
			break;
		case DUMP_RESOURCE_SIZE:
			dump_resource_size();
			break;
		case DUMP_MAILBOX_SIZE:
			dump_mailbox_size();
			break;
		case DUMP_CYCLIC_SIZE:
			dump_cyclic_size();
			break;
		case DUMP_QUEUE_SIZE:
			dump_queue_size();
			break;
		case DUMP_SEMAPHORE_SIZE:
#if CMX_SEMAPHORE_ENABLE == 1
			dump_semaphore_size();
#endif
			break;
		case TICK_COMPUTE:
			compute_tick_func();
			break;
		case DUMP_TASK_NAME:
			dump_task_name();
			break;
		case FREEZE:
			freeze_bug();
			break;
		case UNFREEZE:
			unfreeze_bug();
			break;
		case DUMP_TASK:
			dump_task();
			break;
		case DUMP_RESOURCES:
			dump_resource();
			break;
		case DUMP_CYCLIC:
			dump_cyclic();
			break;
		case DUMP_QUEUES:
			dump_queue();
			break;
		case DUMP_SEMAPHORE:
#if CMX_SEMAPHORE_ENABLE == 1
			dump_semaphore();
#endif
			break;
		case DUMP_MAILBOXES:
			dump_mailbox();
			break;
		case CHANGE_RTC_SCALE:
			change_rtc();
			break;
		case CHANGE_TSLICE_SCALE:
			change_tslice();
			break;
		case ENABLE_TIME_SLICING:
			tslice_on_off(1);
			break;
		case DISABLE_TIME_SLICING:
			tslice_on_off(0);
			break;
		case TIME_SLICING_STATE:
			tslice_state();
			break;
		case DELAY_TICK:
			quick_tick(1);
			break;
		case QUICK_TICK:
			quick_tick(0);
			break;
		case DELAY_NUM_FUNCS:
			function_tick(1);
			break;
		case QUICK_NUM_FUNCS:
			function_tick(0);
			break;
		case FREEZE_ACK:
			freeze_acknowledge();
			break;
		case BUG_CONNECT2:
			freeze_acknowledge();
			break;
		default:
			break;
			}
}

static void setup_outbuff(void)
cmx_reentrant {
	out_com_ptr = out_com_buff;
	*out_com_ptr++ = COMM_START1;
	*out_com_ptr++ = COMM_START2;
	out_com_ptr++;		/* bypass count */
	*out_com_ptr++ = in_com_buff[MAJOR_LOCATION];
	*out_com_ptr++ = in_com_buff[MINOR_LOCATION];
}

static void gencopy(void *src,byte count)
cmx_reentrant {
	byte *new_src;

	new_src = (byte *)src;
	do {
		*out_com_ptr++ = *new_src++;
		} while (--count);
}

static void dump_target(void)
cmx_reentrant {
	*out_com_ptr++ = CMX_SEMAPHORE_ENABLE;
	*out_com_ptr++ = MAX_TASKS;
	*out_com_ptr++ = MAX_RESOURCES;
	*out_com_ptr++ = MAX_CYCLIC_TIMERS;
	*out_com_ptr++ = MAX_MAILBOXES;
	*out_com_ptr++ = MAX_QUEUES;
	*out_com_ptr++ = RTC_SCALE;
	*out_com_ptr++ = TSLICE_SCALE;
	*out_com_ptr++ = MAX_SEMAPHORES;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = MAX_DUMMY; 
	*out_com_ptr++ = MAX_DUMMY;
	*out_com_ptr++ = endian;
	*out_com_ptr++ = BYTE1;
	*out_com_ptr++ = BYTE2;
	*out_com_ptr++ = BYTE3;
	*out_com_ptr++ = BYTE4;
	gencopy(TARGET_NAME,21);
	gencopy(VERSION,5);
	compute_csum();
}

static void freeze_acknowledge(void)
cmx_reentrant {
	locked_out = 1;
	CMXBUG_ACTIVE = 1;
	out_com_ptr = out_com_buff;
	*out_com_ptr++ = COMM_START1;
	*out_com_ptr++ = COMM_START2;
	out_com_ptr++;		/* bypass count */
	*out_com_ptr++ = FREEZE_ACK;
	*out_com_ptr++ = 0x00;
	compute_csum();
}

static void dump_task_size(void)
cmx_reentrant {
	gencopy((void *)tcb_size, sizeof (tcb_size));
	compute_csum();
}

static void dump_resource_size(void)
cmx_reentrant {
	gencopy((void *)resource_size, sizeof (resource_size));
	compute_csum();
}

static void dump_mailbox_size(void)
cmx_reentrant {
	gencopy((void *)mailbox_size, sizeof (mailbox_size));
	compute_csum();
}

static void dump_queue_size(void)
cmx_reentrant {
	gencopy((void *)queue_size, sizeof (queue_size));
	compute_csum();
}

static void dump_cyclic_size(void)
cmx_reentrant {
	gencopy((void *)cyclic_size, sizeof (cyclic_size));
	compute_csum();
}

#if CMX_SEMAPHORE_ENABLE == 1

static void dump_semaphore_size(void)
cmx_reentrant {
	gencopy((void *)semaphore_size, sizeof (semaphore_size));
	compute_csum();
}

#endif


static void dump_task_name(void)
cmx_reentrant {
	char *src;

	if (working_num <= MAX_TASKS)
		{
		if ((src = task_name[working_num]))
			{
			do {
				*out_com_ptr++ = *((byte *)(src));
				} while (*src++);
			}
		else
			*out_com_ptr++ = 0;
		compute_csum();
		}
	else
		{
		*out_com_ptr++ = RANGE_ERROR;
		compute_csum();
		}
}

static void dump_task(void)
cmx_reentrant {
	tcbpointer tcbptr;

	if (working_num <= MAX_TASKS)
		{
		tcbptr = &cmx_tcb[working_num];
		gencopy(tcbptr,sizeof (struct _tcb));
		compute_csum();
		}
	else
	{
	*out_com_ptr++ = RANGE_ERROR;
	compute_csum();
	}
}

static void dump_resource(void)
cmx_reentrant {
	RESHDR *res_ptr;
	RESHDR *link;		/* forward wait link. */
    
	if (working_num < MAX_RESOURCES)
		{
		res_ptr = &res_que[working_num];	/* get address of resource */
		link = res_dummy;	/* get address of dummy resource */
		/* now copy over, so we do NOT trash true resources que items. */
		link->fwlink = res_ptr->fwlink;
		link->bwlink = (tcbpointer)res_ptr;
		link->owner = res_ptr->owner;                    
		if (link->owner)
			{
			*out_com_ptr++ = (link->owner - cmx_tcb);				
			while(1)
				{
				if (link->fwlink == (tcbpointer)link->bwlink)
					break;
				*out_com_ptr++ = (link->fwlink - cmx_tcb);
				link->fwlink = link->fwlink->fwlink;				
				}
			}
		*out_com_ptr++ = 0xff;
		compute_csum();
		}
	else
		{
		*out_com_ptr++ = RANGE_ERROR;
		compute_csum();
		}
}

static void dump_cyclic(void)
cmx_reentrant {
	struct _tcproc *tpptr;

	if (working_num < MAX_CYCLIC_TIMERS)
		{
		tpptr = &tcproc[working_num];
		gencopy(tpptr,sizeof (struct _tcproc));
		compute_csum();
		}
	else
		{
		*out_com_ptr++ = RANGE_ERROR;
		compute_csum();
		}
}

static void dump_queue(void)
cmx_reentrant {
	QUEHDR *queue_ptr;

	if (working_num < MAX_QUEUES)
		{
		queue_ptr = &queue[working_num];	/* get address of proper queue handler. */
		gencopy(queue_ptr,sizeof (struct cmxqueue));
		compute_csum();
		}
	else
		{
		*out_com_ptr++ = RANGE_ERROR;
		compute_csum();
		}
}

#if CMX_SEMAPHORE_ENABLE == 1

static void dump_semaphore(void)
cmx_reentrant {
	SEM *sem_ptr;

	if (working_num <= MAX_SEMAPHORES)
		{
		sem_ptr = &sem_array[working_num];	/* CMX semaphore handler. */
		gencopy(sem_ptr,sizeof (struct semaphore));
		compute_csum();
		}
	else
	{
	*out_com_ptr++ = RANGE_ERROR;
	compute_csum();
	}
}

#endif
static void dump_mailbox(void)
cmx_reentrant {
	MAILBOX *mail_ptr;
	MSG *link;	/* scratch pointer */
	word16 ctr;

	if (working_num < MAX_MAILBOXES)
		{
		ctr = 0;
		mail_ptr = &mail_box[working_num];	/* get address of proper queue handler. */
		link = mail_ptr->first_lnk;	/* get first link. */
		while(1)
			{
			if (!link)
				break;
			link = link->link;
			ctr++;
			}
		*out_com_ptr++ = (byte)ctr;
		if (mail_ptr->waiter)	/* task waiting on this mailbox */
			{
			*out_com_ptr++ = mail_ptr->waiter - cmx_tcb;
			}
		else
			*out_com_ptr++ = 0xff;
		if (mail_ptr->task_num)
			{
			*out_com_ptr++ = mail_ptr->task_num;
			}
		else
			*out_com_ptr++ = 0xff;
		*out_com_ptr++ = (byte)(mail_ptr->event_num);
		compute_csum();
		}
	else
		{
		*out_com_ptr++ = RANGE_ERROR;
		compute_csum();
		}
}

static void quick_tick(byte tick_identifier)
cmx_reentrant {
	word16 num_ticks;

	if (tick_identifier)
		{
		num_ticks = in_com_buff[DATA_LOC1] << 8;
		num_ticks += in_com_buff[DATA_LOC2];
		}
	else
		num_ticks = 1;
	CMXBUG_ACTIVE = 0;
	locked_out = 0;
	K_Task_Wait(num_ticks);
	locked_out = 1;
	CMXBUG_ACTIVE = 1;
	setup_outbuff();
	*out_com_ptr++ = GOOD;
	compute_csum();
}

static void function_tick(byte func_identifier)
cmx_reentrant {

	if (func_identifier)
		{
		bug_step_count = in_com_buff[DATA_LOC1] << 8;
		bug_step_count += in_com_buff[DATA_LOC2];
		}
	else
		bug_step_count = 1;
	CMXBUG_ACTIVE = 0;
	locked_out = 0;
	bug_step_one = 1;
	K_Task_Wait(0);
	bug_step_one = 0;
	locked_out = 1;
	CMXBUG_ACTIVE = 1;
	setup_outbuff();
	*out_com_ptr++ = GOOD;
	compute_csum();
}


static void change_rtc(void)
cmx_reentrant {
	RTC_SCALE = working_num;
	*out_com_ptr++ = GOOD;
	compute_csum();
}

static void change_tslice(void)
cmx_reentrant {
	TSLICE_SCALE = working_num;
	*out_com_ptr++ = GOOD;
	compute_csum();
}

static void tslice_state()
cmx_reentrant {
	*out_com_ptr++ = SLICE_ON;
	compute_csum();
}

static void tslice_on_off(byte on)
cmx_reentrant {
	if (on)
		{
		if (!SLICE_ON)
			SLICE_ON = 1;
		}
	else
		{
		if (SLICE_ON)
			SLICE_ON = 0;
		}
	*out_com_ptr++ = GOOD;
	compute_csum();
}

static void compute_tick_func(void)
cmx_reentrant {
	if (bug_freeze)
		{
		if (!commflag.compute_tick)
			{
			commflag.compute_tick = TRUE;
			compute_csum();
			stat_array[cmxbug_slot] = 0;
			CMXBUG_ACTIVE = 0;
			}
		else
			{
			CMXBUG_ACTIVE = 1;
			commflag.compute_tick = FALSE;
			gencopy(&stat_array[cmxbug_slot],sizeof (long) ); 
			compute_csum();
			}
		}
}

static void freeze_bug(void)
cmx_reentrant {
	locked_out = 1;
	CMXBUG_ACTIVE = 1;
	bug_freeze = TRUE;
	*out_com_ptr++ = GOOD;
	compute_csum();
}

static void unfreeze_bug(void)
cmx_reentrant {
	CMXBUG_ACTIVE = 0;
	locked_out = 0;
	bug_freeze = FALSE;
/*	*out_com_ptr++ = GOOD; */
	compute_csum();
}

static void refresh_request(void)
cmx_reentrant {
	locked_out = 1;
	CMXBUG_ACTIVE = 1;
	out_com_ptr = out_com_buff;
	*out_com_ptr++ = COMM_START1;
	*out_com_ptr++ = COMM_START2;
	out_com_ptr++;		/* bypass count */
	*out_com_ptr++ = BUG_REFRESH;
	*out_com_ptr++ = 0x00;
	compute_csum();
}

