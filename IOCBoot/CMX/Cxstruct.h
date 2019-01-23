/*******************************************
 The following are CMX structures
********************************************/
struct _tcb {
	struct _tcb *fwlink;	/* forward wait link */
	struct _tcb *bwlink;	/* backward wait link */
	struct _tcb *ftlink;	/* forward time wait link */
	struct _tcb *btlink;	/* backward time wait link */
	word16 tcbstate;
	byte trig;			/* the number of triggers (start requests) */
	byte priority;		/* the priority of this task */
	word16 tcbtimer;	/* the timer counter */
	struct _tcb *nxttcb;	/* points to next task, according to priority */
	void (*task_addr)(void); 	/* the task's CODE address */
	word16 *stk_start;			/* location in external ram, of where to save  
										task's context	*/
	word16 *stk_save;			/* location in external ram, of where to save  
										task's context	*/
	struct _tcb *mesg_sender;
	word16 e_flags;				/* flags for this task */
	word16 e_match;				/* the flags states for match */
};

typedef struct _tcb *tcbpointer;

typedef struct tsk_time_lnk {
	struct _tcb *dum1;	/* forward wait link */
	struct _tcb *dum2;	/* backward wait link */
	struct _tcb *ftlink;	/* forward time link */
	struct _tcb *btlink;	/* backward time link */
	} TSK_TIMER_LNK;

typedef struct cmxmesg {
	struct cmxmesg *env_link;	/* link to next message block. */
	struct cmxmesg *link;		/* link to mailbox. */
	tcbpointer sender;			/* task who sent message. */
	byte *message;					/* address of message. */
	} MSG;


typedef struct cmxmbox {
	MSG *first_lnk;		/* link to message block. */
	MSG *next_lnk;			/* link to message block. */
	tcbpointer waiter;	/* what task is waiting for message. */
	byte task_num;			/* task number for setting event bit */
	word16 event_num;		/* event number */
	} MAILBOX;


typedef struct cmxqueue {
	byte *base_ptr;	/* address of user supplied memory for queue. */
	sign_word16 num_slots;	/* the number of slots within this queue */
	sign_word16 queue_cnt;	/* the number of slots used */
	byte size_slot;	/* size of slots */
	sign_word16 head;	/* must be signed to test for < 0 */
	sign_word16 tail;	/* must be signed to test for < 0 */
	} QUEHDR;



/* the cyclic timers structure */
typedef struct _tcproc {
	struct _tcproc *ftlink;	/* forward time link */
	struct _tcproc *btlink;	/* backward time link */
	byte tproc_start;		/* byte indicating timed procedure stopped / started */
	word16 tproc_timer;		/* the timer counter */
	word16 reload_time;		/* the cyclic time that will be reloaded */
	word16 event_num;			/* the event to set. */
	byte mode;					/* what mode to sent to K_Event_Signal function. */
	byte tskid_pri;			/* task slot number or priority (may not be used). */
	} CYCLIC_TIMERS;

typedef struct cyclic_tmrs_link {
	struct _tcproc *ftlink;	/* forward time link */
	struct _tcproc *btlink;	/* backward time link */
	} CYCLIC_LNK;

/* the resource structure */
typedef struct cmxresource {
	tcbpointer fwlink;		/* forward wait link. */
	tcbpointer bwlink;		/* backward wait link. */
	tcbpointer owner;			/* The task that owns resource. */
	byte owner_priority;
	} RESHDR;

typedef struct semaphore {
		tcbpointer fwlink;	/* forward wait link. */
		tcbpointer bwlink;	/* backward wait link. */
		word16 sem_count;
		word16 sem_n;
		} SEM;

typedef struct pipe_element {
		byte identifier;
		byte p1;		/* parameter 1 */
		byte p2;		/* parameter 2 */
		union {
		word16 p3;	/* parameter 3 */
		void *p4;	/* address of message. */
			} pipe_u;
		} PIPE_STRUC;
