#define CMXMODULE 1

#include <cxfuncs.h>	/* get cmx include header file */
#include <cxextern.h>	/* get cmx include header file */
#include <string.h>	/* get string include header file */
#if __CPU__ == 3 || __CPU__ == 5 || __CPU__ == 7	/* advanced mode */
#define MAX_MEM_DECIMAL 0x08	/* max decimal size */
#define MAX_MEM_HEX 0x06		/* max hex size */
static byte *from_ptr;
static byte *thru_ptr;
#define mem_size_ptr byte *
#else
#define MAX_MEM_DECIMAL 0x05	/* max decimal size */
#define MAX_MEM_HEX 0x04		/* max hex size */
static byte *from_ptr;
static byte *thru_ptr;
#define mem_size_ptr byte *
#endif

#define MAX_FIELD_LENGTH 12	/* the maximum field length of any field */
#define SCROLL_SIZE 18

#define WHAT_FUNCTION 0
#define DUMP_TASK 1
#define DUMP_RESOURCES 2
#define DUMP_TIMERS 3
#define DUMP_QUEUES 4
#define DUMP_MAILBOXES 5
#define DUMP_RAM 6
#define DUMP_STACKS 7
#define CHANGE_RTC_SCALE 8
#define CHANGE_TSLICE_SCALE 9
#define ENABLE_TIME_SLICING 10
#define DELAY_TICK 11
#define QUICK_TICK 12
#define STATS 13
#define STATS_RESET 14
#define TASK_CTRL 15
#define EXIT_BUG 99

#define WHAT_FUNCTION_PROMPT 00 /* 1 */
#define DUMP_TASK_PROMPT WHAT_FUNCTION_PROMPT + 1 /* 1 */
#define DUMP_RESOURCE_PROMPT DUMP_TASK_PROMPT + 1 /* 1 */
#define DUMP_CYCLIC_PROMPT DUMP_RESOURCE_PROMPT + 1 /* 1 */
#define DUMP_QUEUES_PROMPT DUMP_CYCLIC_PROMPT + 1 /* 1 */
#define DUMP_MAILBOX_PROMPT DUMP_QUEUES_PROMPT + 1 /* 1 */
#define DUMP_RAM_PROMPT DUMP_MAILBOX_PROMPT + 1 /* 2 */
#define DUMP_STACK_PROMPT DUMP_RAM_PROMPT + 2 /* 1 */
#define RTC_SCALE_PROMPT DUMP_STACK_PROMPT + 1 /* 2 */
#define TSLICE_SCALE_PROMPT RTC_SCALE_PROMPT + 2 /* 2 */
#define ENABLE_SLICE_PROMPT TSLICE_SCALE_PROMPT + 2 /* 2 */
#define DELAY_TICK_PROMPT ENABLE_SLICE_PROMPT + 2 /* 1 */
#define STATS_RESET_PROMPT DELAY_TICK_PROMPT + 1 /* 1 */
#define TASK_CTRL_PROMPT STATS_RESET_PROMPT + 1 /* 6 */
struct prompt {
	char *word;
	byte in_length;
	/* we could possibly do the following
	struct prompt *next_prompt; point to next message 
	struct prompt *pevious_prompt; point to next message
	*/
	};

/* prompt char., length of field */
const struct prompt prpt_array [] = {
			/* what function prompt */
			{ " Enter Function? \n",0x02},		
			/*  task dump entry */
			{ "\r\nTASK dump",0x03},
			{ "\r\nRESOURCE dump",0x03},
			{ "\r\nCYCLIC TIMERS dump",0x03},
			{ "\r\nQUEUE dump",0x03},
			{ "\r\nMAILBOX dump",0x03},
			{ "\r\nRAM dump\r\nEnter beginning ADDRESS of memory or <return> to exit? ",MAX_MEM_DECIMAL},
			{ "\r\n\r\nEnter ending ADDRESS of memory? ",MAX_MEM_DECIMAL},
			{ "\r\nSTACK dump",0x03},
			{ "\r\nCurrent SYSTEM TICK SCALE = ",0x00},
			{ "\r\nEnter new SYSTEM TICK SCALE or <return> to leave as is ",0x03},
			{ "\r\nCurrent TIME SLICE SCALE = ",0x00},
			{ "\r\nEnter new TIME SLICE SCALE or <return> to leave as is ",0x03},
			{ "\r\nCurrent TIME SLICING = ",0x00},
			{ "\r\nEnter Y/y or N/n to enable/disable TIME SLICING or <return> to leave as is ",0x01},
			{ "\r\nEnter the number of ticks to wait or <return> to leave? ",0x05},
			{ "\r\nEnter Y/y to reset TIME ANALYSIS array or <return> to leave as is ",0x03},
			{ "\r\n***  ADDITIONAL TASK functions ***",0x01},
			{ "\r\nEnter 1 to START TASK",0x01},
			{ "\r\nEnter 2 to STOP TASK",0x01},
			{ "\r\nEnter 3 to WAKE TASK",0x01},
			{ "\r\nEnter 9 to EXIT",0x01},
			{ "\r\nYour choice? ",0x01},
			{ "\r\nTASK START",0x03},
			{ "\r\nTASK STOP",0x03},
			{ "\r\nTASK WAKE",0x03}
		};

/* keyboard flags */
struct keyflag {	 /* bit structure */
	word16 buff_wait:1;
	word16 f_key:1;
	word16 e_key:1;
	word16 max_in_needed:1;
	word16 non_zero:1;	
	word16 cont:1;
	word16 e_jmp:1;
	word16 input_func:1;
	word16 allow_hex:1;
	word16 echo_off:1;
	word16 loop:1;
	word16 scroll:1;
	word16 string:1;
	word16 string_allowed:1;
	};

static struct keyflag keyflags;
static byte key_pressed;	/* byte for scan code of keypad 0x00 thru 0x0f */
static byte num_chars_in;
static byte *in_char_ptr;
static byte max_chars_in;
static byte current_func;
static byte keys_in_buff[MAX_FIELD_LENGTH];
static byte prompt_index;
static byte hex_byte;
static word32 hex_long;
static byte func_offset;		
static byte input_index;
static const struct prompt *prpt_ptr;
static word32 total_ticks;
static byte prompt_out(void);
static void dump_task(void);
static void dump_resource(void);
static void dump_cyclic(void);
static void dump_queue(void);
static void dump_mailbox(void);
static void dump_memory_ram(void);
static void dump_stack(void);
static void change_rtc_scale(void);
static void change_tslice_scale(void);
static void enable_tslice(void);
static void delay_tick(void);
static void quick_tick(void);
static void stat_display(void);
static void stat_reset(void);
static void task_ctrl_func(void);
static void display_char(char);
static void zerofill(byte *);
static void clear_chars_in(void);
static void conv_hex(void);
static void putbyte(byte);		/* output a number (decimal) */
static void putpercent(byte,byte);		/* output a number (decimal) */
static void putword(word16);		/* output a number (decimal) */
static void putlong(word32);
static void common_prompt(void);
static void com1_prompt(void);
static void putstg(char *); 
static void putname(char *s);
static void putspaces(byte);
static void putcrlf(void);
static void puthexbyte(byte);
static void puthexword(word16);
static void puthexlong(word32);
static void showstate(word16);
static const char nibs[] = "0123456789ABCDEF";

static RESHDR res_dummy[1];

/* non-statics */
byte cmxbug_slot;
extern long stat_array[];
extern byte CMXBUG_ACTIVE;
extern byte BUG_WAIT_TICKS;
extern char *task_name[];
extern byte K_Bug_Getchr(byte *ptr);
extern void K_Bug_Putchr(char);
void cmxbug(void);


void cmxbug(void)
{
	byte status;

		/* process */
	CMXBUG_ACTIVE = 0;
	while(1)
		{
		if (!keyflags.cont) {
			while(1)
				{
				status = K_Bug_Getchr(&key_pressed);
				if (status)
					break;
				if (!CMXBUG_ACTIVE)
					K_Task_Wait(BUG_WAIT_TICKS);
				}

			if (!keyflags.string)
				{
		switch (key_pressed) {
			case '+':
				locked_out = 1;
				CMXBUG_ACTIVE = 1;
				keyflags.f_key = TRUE;
				break;

			case '"':
				if (!keyflags.non_zero && keyflags.string_allowed)
					{
					display_char(key_pressed);
					max_chars_in = 12;
					keyflags.string = TRUE;
					}
				break;

			case 'p':
			case 'P':
				if (keyflags.echo_off)
					keyflags.echo_off = FALSE;
				else
					keyflags.echo_off = TRUE;
				break;

			case '\r':
				if (keyflags.scroll)
					{
					func_offset |= 0x80;
					keyflags.e_key = TRUE;
					keyflags.e_jmp = TRUE;
					break;
					}
				if ((current_func == TASK_CTRL) && !keyflags.non_zero)
					{
					func_offset = 0x00;
					keyflags.e_key = TRUE;
					keyflags.e_jmp = TRUE;
					break;
					}
				if (!keyflags.non_zero)
					{
					keyflags.f_key = TRUE;
					break;
					}

				if (!keyflags.max_in_needed)
					{
					if (num_chars_in > 0 || keyflags.input_func)
						{
						keyflags.e_key = TRUE;
						}
					}
				else
					{
					if (num_chars_in == max_chars_in)
						{
						keyflags.e_key = TRUE;
						keyflags.max_in_needed = FALSE;
						}
					}
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (num_chars_in < max_chars_in && !keyflags.input_func)
					{
					keyflags.non_zero = TRUE;
					display_char(key_pressed);
					*in_char_ptr++ = key_pressed & 0x0f;
					num_chars_in++;
					}
				break;
			case '*':
				if (num_chars_in < max_chars_in &&!keyflags.input_func)
					{
					keyflags.non_zero = TRUE;
					display_char(key_pressed);
					hex_byte = key_pressed;
					keyflags.e_jmp = TRUE;
					func_offset |= 0x80;	/* set done entry flag */
					num_chars_in++;
					}
				break;

			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
				if (!keyflags.allow_hex)
					break;
				if (num_chars_in < max_chars_in &&!keyflags.input_func)
					{
					keyflags.non_zero = TRUE;
					display_char(key_pressed);
					*in_char_ptr++ = ((key_pressed & 0x0f) + 0x09);
					num_chars_in++;
					}
				break;

			case 'X':
			case 'x':
				if (!keyflags.non_zero)
					{
					display_char('x');
					keyflags.allow_hex = TRUE;
					if (max_chars_in == MAX_MEM_DECIMAL)
						max_chars_in = MAX_MEM_HEX;
					}
				break;

			case 'Y':
			case 'y':
				hex_byte = 1;
				keyflags.e_jmp = TRUE;
				keyflags.e_key = TRUE;
				K_Bug_Putchr('Y');
				func_offset |= 0x80;	/* set done entry flag */
				break;

			case 'N':
			case 'n':
				hex_byte = 0;
				keyflags.e_jmp = TRUE;
				keyflags.e_key = TRUE;
				K_Bug_Putchr('N');
				func_offset |= 0x80;	/* set done entry flag */
				break;

			default:
				break; 
				} /* end of switch */
			}
		else
			{
			if (key_pressed != '\r')
				{
				if (num_chars_in < max_chars_in)
					{
					keyflags.non_zero = TRUE;
					display_char(key_pressed);
					*in_char_ptr++ = key_pressed;
					num_chars_in++;
					}
				}
			else
				{
				func_offset |= 0x80;
				keyflags.e_key = TRUE;
				keyflags.e_jmp = TRUE;
				}
			}
			} /* END OF KEYBOARD.CONT IF */

		if (keyflags.cont)
			{
			keyflags.cont = FALSE;
			keyflags.e_key = TRUE;
			keyflags.e_jmp = TRUE;
			func_offset = 0;
			}
			if (keyflags.f_key)
				{
				keyflags.f_key = FALSE;
				keyflags.e_key = FALSE;
				keyflags.e_jmp = FALSE;
				keyflags.string = FALSE;
				keyflags.string_allowed = FALSE;
				keyflags.allow_hex = FALSE;
				keyflags.scroll = FALSE;
				keyflags.cont = FALSE;
				keyflags.loop = FALSE;
				keyflags.input_func = FALSE;
				func_offset = 0;
				current_func = WHAT_FUNCTION;
				prompt_index = WHAT_FUNCTION_PROMPT;
				keyflags.max_in_needed = FALSE;	/* must have 2 characters in */
				/* clear all normal flags, etc. */
				clear_chars_in();
				max_chars_in = 2;
				putstg("\r\nEnter  1 TASKS\r\n");
				putstg("Enter  2 RESOURCES\r\n");
				putstg("Enter  3 CYCLIC_TIMERS\r\n");
				putstg("Enter  4 QUEUES\r\n");
				putstg("Enter  5 MAILBOXES\r\n");
				putstg("Enter  6 RAM display\r\n");
				putstg("Enter  7 STACK information\r\n");
				putstg("Enter  8 SYSTEM TICK SCALE\r\n");
				putstg("Enter  9 TIME SLICE SCALE\r\n");
				putstg("Enter 10 TIME SLICING select\r\n");
				putstg("Enter 11 GO and RESUME CMXBug\r\n");
				putstg("Enter 12 Quick GO and RESUME CMXBug\r\n");
				putstg("Enter 13 TIME ANALYSIS\r\n");
				putstg("Enter 14 TIME ANALYSIS reset\r\n");
				putstg("Enter 15 ADDITIONAL TASK functions\r\n");
				putstg("Enter 99 EXIT CMXBug\r\n");
				putstg("Enter P/p to toggle ECHO mode, currently ");
				if (!keyflags.echo_off)
					{
					putstg("ON");
					}
				else
					{
					putstg("OFF");
					}
				putstg("\r\nYour choice? ");
				}
			if (keyflags.e_key && CMXBUG_ACTIVE)
				{
				keyflags.e_key = FALSE;
				if (!keyflags.e_jmp)
					{
					zerofill(&keys_in_buff[max_chars_in]);	/* go zero fill, for proper entry */
					conv_hex();	/* go put characters into hex form, just 2 lsd */
					func_offset |= 0x80;	/* set done entry flag */
					}
				else
					keyflags.e_jmp = FALSE;
				if (current_func == WHAT_FUNCTION)
					{
					func_offset = 0;
					current_func = hex_byte;
					switch(current_func) {
						case WHAT_FUNCTION:
							break;
						case DUMP_TASK:
							break;
						case DUMP_RESOURCES:
							break;
						case DUMP_TIMERS:
							break;
						case DUMP_QUEUES:
							break;
						case DUMP_MAILBOXES:
							break;
						case DUMP_RAM:
							break;
						case DUMP_STACKS:
							break;
						case CHANGE_RTC_SCALE:
							break;
						case CHANGE_TSLICE_SCALE:
							break;
						case ENABLE_TIME_SLICING:
							break;
						case QUICK_TICK:
							break;
						case DELAY_TICK:
							break;
						case STATS:
							break;
						case STATS_RESET:
							break;
						case TASK_CTRL:
							break;
						case EXIT_BUG:
							break;
						default:
							current_func = WHAT_FUNCTION;
							keyflags.f_key = TRUE;
							putstg("\r\n\r\nInvalid entry, press any key to continue\r\n");
							break;
						}
					}
				switch(current_func) {
					case DUMP_TASK:
						dump_task();
						break;
					case DUMP_RESOURCES:
						dump_resource();
						break;
					case DUMP_TIMERS:
						dump_cyclic();
						break;
					case DUMP_QUEUES:
						dump_queue();
						break;
					case DUMP_MAILBOXES:
						dump_mailbox();
						break;
					case DUMP_RAM:
						dump_memory_ram();
						break;
					case DUMP_STACKS:
						dump_stack();
						break;
					case CHANGE_RTC_SCALE:
						change_rtc_scale();
						break;
					case CHANGE_TSLICE_SCALE:
						change_tslice_scale();
						break;
					case ENABLE_TIME_SLICING:
						enable_tslice();
						break;
					case QUICK_TICK:
						quick_tick();
						break;
					case DELAY_TICK:
						delay_tick();
						break;
					case STATS:
						stat_display();
						break;
					case STATS_RESET:
						stat_reset();
						break;
					case TASK_CTRL:
						task_ctrl_func();
						break;
					case EXIT_BUG:
						locked_out = 0;
						CMXBUG_ACTIVE = 0;
						putstg("\r\n\r\n You have exited CMXBug(TM), press <+> (plus key) to enter CMXBug again!\r\n");
						break;
					default:
						break;
					} /* end of switch */
				} /* end of e key brace */
		}	/* end of endless while loop */
}

static void common_prompt(void)
{
	putstg(", enter number, '*' for all, <return> to exit. Choice? ");
}

static void com1_prompt(void)
{
	putstg(", enter SLOT #, \"TASK name, <return> to exit. Choice? ");
}


static byte prompt_out(void)
{

	prpt_ptr = &prpt_array[prompt_index + func_offset];
	/* chars_in(); */
	keyflags.non_zero = FALSE;
	max_chars_in = prpt_ptr->in_length;
	clear_chars_in();
	putstg(prpt_ptr->word);
}

/* bit 7 of count will dictate whether we should increment/decrement
	the source pointer */

static void zerofill(byte *mov_char_ptr)
{
	byte count;
	byte *copy_in_char_ptr;

	if (num_chars_in != max_chars_in)
		{
		copy_in_char_ptr = in_char_ptr;
		for (count = 0; count < num_chars_in; count++)
			{
			*(--mov_char_ptr) = *(--in_char_ptr);
			}
		for (count = num_chars_in;	count < max_chars_in; count++)
			{
			*(--mov_char_ptr) = 0x00;
			}
		in_char_ptr = copy_in_char_ptr;
		}
}

static void conv_hex(void)
{

	byte *hex_char_ptr;
	word32 multiplier;

	multiplier = 1;
	hex_long = 0; 
	if (max_chars_in == 1)
		{
		hex_char_ptr = keys_in_buff;
		hex_byte = *hex_char_ptr;
		hex_long = (word32)hex_byte;
		}
	else
		{
		hex_char_ptr = &keys_in_buff[max_chars_in-2];
		if (keyflags.allow_hex)
			{
			hex_byte = *hex_char_ptr * 16;
			hex_char_ptr++;
			hex_byte += *hex_char_ptr;
			}
		else
			{
			hex_byte = *hex_char_ptr * 10;
			hex_char_ptr++;
			hex_byte += *hex_char_ptr;
			}
		for (hex_char_ptr = &keys_in_buff[max_chars_in - 1];
				hex_char_ptr >= keys_in_buff;	--hex_char_ptr)
			{
			hex_long += *hex_char_ptr * multiplier;
			if (keyflags.allow_hex)
				multiplier *= 16;
			else
				multiplier *= 10;
			} 
		}

}

static void display_char(char x)
{
	if (!keyflags.echo_off)
		K_Bug_Putchr(x);
}

static void clear_chars_in(void)
{
	in_char_ptr = keys_in_buff;
	do {
		*in_char_ptr++ = 0;
		} while (in_char_ptr != &keys_in_buff[sizeof keys_in_buff]);
	in_char_ptr = keys_in_buff;
	num_chars_in = 0;
}

static void putword(word16 num)		/* output a number (decimal) */
{

	byte digit,suppress;
	word16 divisor;
    divisor = 10000;
    suppress = 1;
    while( !( divisor == 1 )){
		digit = num / divisor;
		num = num - (digit * divisor);
		divisor = divisor / 10;
		if( suppress  &&  digit )
	   	suppress = 0;
		if( !suppress )
			digit |= '0';
		else
			digit = ' ';
		K_Bug_Putchr(digit);
    	}
	K_Bug_Putchr(num+'0');
}

static void putbyte(byte num)		/* output a number (decimal) */
{

	byte digit,suppress;
	byte divisor;
    divisor = 100;
    suppress = 1;
    while( !( divisor == 1 )){
		digit = num / divisor;
		num = num - (digit * divisor);
		divisor = divisor / 10;
		if( suppress  &&  digit )
	   	suppress = 0;
		if( !suppress )
			digit |= '0';
		else
			digit = ' ';
		K_Bug_Putchr(digit);
    	}
	K_Bug_Putchr(num+'0');
}

static void putpercent(byte num,byte fill)		/* output a number (decimal) */
{

	byte digit,suppress;
	byte divisor;
    divisor = 100;
    suppress = 1;
    while( !( divisor == 1 )){
		digit = num / divisor;
		num = num - (digit * divisor);
		if (divisor == 10 && fill)
	   	suppress = 0;
		divisor = divisor / 10;
		if( suppress  &&  digit )
	   	suppress = 0;
		if( !suppress )
			digit |= '0';
		else
			continue;
		K_Bug_Putchr(digit);
    	}
	K_Bug_Putchr(num+'0');
}

static void putlong(word32 num)		/* output a number (decimal) */
{
	word16 digit,suppress;
	word32 divisor;
    divisor = 1000000;
    suppress = 1;
    while( !( divisor == 1 )){
		digit = num / divisor;
		num = num - (digit * divisor);
		divisor = divisor / 10;
		if( suppress  &&  digit )
	   	suppress = 0;
		if( !suppress )
			digit |= '0';
		else
			digit = ' ';
		K_Bug_Putchr(digit);
		}
   K_Bug_Putchr((char)('0'+num));
}

static void putstg(char *s) /* display null-terminated ASCII string thru serial port */
{
	while (*s)
		K_Bug_Putchr(*s++);
}

static void putname(char *s) /* display null-terminated ASCII string thru serial port */
{
	byte ctr;

	ctr = 0;
	do {
		if (*s)
			K_Bug_Putchr(*s++);
		else
			K_Bug_Putchr(' ');
		} while (++ctr < 13 );
}	

static void putspaces(byte num)
{
	while (num--)
		K_Bug_Putchr(' ');
}


static void putcrlf(void)
{
	K_Bug_Putchr(0x0d);
	K_Bug_Putchr(0x0a);
}	/* putcrlf */

static void puthexlong(word32 w)	/* puts out long word in hex */
{
	puthexbyte((byte)(w>>24));
	puthexbyte((byte)(w>>16));
	K_Bug_Putchr(':');
	puthexbyte((byte)(w>>8));
	puthexbyte((byte)(w & 0xff));
	putspaces(1);
}	/* puthexlong */

static void puthexbyte(byte b)	/* puts out byte in ASCII hex, to serial port */
{
	K_Bug_Putchr(nibs[ b >> 4 ]);
	K_Bug_Putchr(nibs[ b & 0x0F ]);
}	/* puthexbyte */

static void puthexword(word16 w)
/*	Put out word in hex, high byte first. */
{
	puthexbyte((byte)(w>>8));
	puthexbyte((byte)(w & 0xff));
	putspaces(1);
}	/* puthexword */


static void showstate(word16 ts)
{

	if (ts & RESOURCE)
		putstg("resource ");
	else if (ts & WAIT)
		putstg("wait     ");
	else if (ts & SEND_MESG)
		putstg("mesg_send");
	else if (ts & WAIT_MESG)
		putstg("mesg_wait");
	else if (ts & FLAGS)
		putstg("events   ");
	else if (ts & IDLE)
		putstg("idle     ");
	else if (ts & READY)
		putstg("ready    ");
	else if (ts & RESUME)
		putstg("ready++  ");
	else if (ts & RUNNING)
		putstg("running  ");
	else
		putstg("no create");
}	/* showstate */

static void dump_task(void)
{
	tcbpointer tcbptr;

	prompt_index = DUMP_TASK_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			/* if good */
			/* if all, set hex_byte to 2, because of bug task */
			if (!keyflags.scroll)
				{
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 1;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			}
			putcrlf();
			putstg("SLOT,   NAME     , State    (time), Trig, Pri, TSK Addr, E-Flags, E-Match\r\n");
			while (1)
				{
				if (hex_byte <= MAX_TASKS)
					{
					/* MUST change to relect slot number and name */
					tcbptr = &cmx_tcb[hex_byte];
					/* maybe change to decimal, later */
					putspaces(1);
					putbyte(hex_byte);	/* output task number */
					putspaces(1);
					if (task_name[hex_byte])
						putname(task_name[hex_byte]);
					else
						putspaces(12);
					putspaces(1);
					showstate(tcbptr->tcbstate);
					if (tcbptr->tcbstate & TIME)
						{
						putstg("(");
						putword(tcbptr->tcbtimer);
						putstg(")");
						}
					else if ((tcbptr->tcbstate & TIME_EXPIRED) && ((activetcb - cmx_tcb) != hex_byte))
						{
						putstg("( TIME)");
						}
					else
						putspaces(7);
					putspaces(1);
					putbyte(tcbptr->trig);
					putspaces(3);
					putbyte(tcbptr->priority);
					putstg("   0x");
					if ((sizeof(void (*)())) == 4)
						puthexlong((word32)(tcbptr->task_addr));	/* TCB address */
					else
						puthexword((word16)(tcbptr->task_addr));	/* TCB address */
					putstg("   0x");
					puthexword(tcbptr->e_flags);
					putstg("  0x");
					puthexword(tcbptr->e_match);
					putcrlf();	
					}
				else
					{
					putstg("TASK Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte > MAX_TASKS)
						{
						keyflags.scroll = FALSE;
						break;
						}
					else
						{
						if (hex_byte % SCROLL_SIZE == 0)
							{
							keyflags.scroll = TRUE;
							putstg("Enter <return> to continue");
							return;
							}
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void dump_stack(void)
{
	tcbpointer tcbptr;

	prompt_index = DUMP_STACK_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			/* if good */
			/* if all, set hex_byte to 2, because of bug task */
			if (!keyflags.scroll)
				{
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 1;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			}
			putcrlf();
			putstg("SLOT,   NAME     , STACK Addr, MAX. bytes used SO FAR \r\n");
			while (1)
				{
				if (hex_byte <= MAX_TASKS)
					{
					/* MUST change to relect slot number and name */
					tcbptr = &cmx_tcb[hex_byte];
					/* maybe change to decimal, later */
					putspaces(1);
					putbyte(hex_byte);	/* output task number */
					putspaces(1);
					if (task_name[hex_byte])
						putname(task_name[hex_byte]);
					else
						putspaces(12);
					putspaces(1);
					putstg("   0x");
					if ((sizeof(char *)) == 4)
						puthexlong((word32)(tcbptr->stk_start));	/* TCB address */
					else
						puthexword((word16)(tcbptr->stk_start));	/* TCB address */
					putspaces(5);
					putstg("Not Implemented");
/*					putword(stack_usage[hex_byte]); */
					putcrlf();	
					}
				else
					{
					putstg("TASK Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte > MAX_TASKS)
						{
						keyflags.scroll = FALSE;
						break;
						}
					else
						{
						if (hex_byte % SCROLL_SIZE == 0)
							{
							keyflags.scroll = TRUE;
							putstg("Enter <return> to continue");
							return;
							}
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}


static void dump_resource(void)
{
	RESHDR *res_ptr;
	RESHDR *link;		/* forward wait link. */
	word16 ctr;

	prompt_index = DUMP_RESOURCE_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 0;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			putcrlf();
			putstg("RES #,    Owner   , Task(s) Waiting, MAX. 3 shown\r\n");
			while (1)
				{
				if (hex_byte < MAX_RESOURCES)
					{
					res_ptr = &res_que[hex_byte];	/* get address of resource */
					link = res_dummy;	/* get address of dummy resource */
					/* now copy over, so we do NOT trash true resources que items. */
					link->fwlink = res_ptr->fwlink;
					link->bwlink = res_ptr->bwlink;
					link->owner = res_ptr->owner;
					/* maybe change to decimal, later */
					putspaces(1);
					putbyte(hex_byte);	/* output task number */
					putspaces(1);
					if (res_ptr->owner)
						{
						if (task_name[res_ptr->owner - cmx_tcb])
							putname(task_name[res_ptr->owner - cmx_tcb]);
						else
							putspaces(12);
						putspaces(1);
						ctr = 0;
						while(1)
							{
							if (link->fwlink == (tcbpointer)res_ptr)
								{
								break;
								}
							if (ctr == 3)
								{
								putstg(", More...");
								break;
								}
							if (ctr)
								putstg(", ");
							if (task_name[link->fwlink - cmx_tcb])
								putname(task_name[link->fwlink - cmx_tcb]);
							else
								putspaces(12);
							link->fwlink = link->fwlink->fwlink;
							ctr++;
							}
						if (!ctr)
							{
							putstg("NONE");
							}
						}
					else
						{
						putstg("NONE          NONE");
						}
					putcrlf();	
					}
				else
					{
					putstg("RESOURCE Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte >= MAX_RESOURCES)
						{
						break;
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void dump_cyclic(void)
{
	struct _tcproc *tpptr;

	prompt_index = DUMP_CYCLIC_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 0;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			putcrlf();
			putstg("TIMER #, Started?, Time left, CYCLIC Time,    Mode    ,  task/pri. , EVENT \r\n");
			while (1)
				{
				if (hex_byte < MAX_CYCLIC_TIMERS)
					{
					tpptr = &tcproc[hex_byte];
					/* maybe change to decimal, later */
					putspaces(3);
					putbyte(hex_byte);	/* output task number */
					putspaces(4);
					if (tpptr->tproc_start)
						putstg("YES  ");
					else
						putstg("NO   ");
					putstg("     0x");
					puthexword(tpptr->tproc_timer);
					putstg("     0x");
					puthexword(tpptr->reload_time);
					putspaces(2);
					switch(tpptr->mode) {
						case 0:
							putstg("Task number  ");
							if (task_name[tpptr->tskid_pri])
								putname(task_name[tpptr->tskid_pri]);
							else
								putspaces(12);
							break;
						case 1:
							putstg("Top Pri task ");
							putspaces(12);
							break;
						case 2:
							putstg("Top Pri task waiting");
							putspaces(5);
							break;
						case 3:
							putstg("ALL tasks    ");
							putspaces(12);
							break;
						case 4:
							putstg("ALL tasks waiting");
							putspaces(8);
							break;
						case 5:
							putstg("Same priority");
							putspaces(4);
							putbyte(tpptr->tskid_pri);
							putspaces(4);
							break;
						case 6:
							putstg("Same Pri waiting");
							putspaces(1);
							putbyte(tpptr->tskid_pri);
							putspaces(4);
							break;
						default:
							putstg("Illegal mode ");
							putspaces(12);
							break;
						}
					putstg(" 0x");
					puthexword(tpptr->event_num);
					putcrlf();	
					}
				else
					{
					putstg("CYCLIC Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte >= MAX_CYCLIC_TIMERS)
						{
						break;
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void dump_queue(void)
{
	QUEHDR *queue_ptr;

	prompt_index = DUMP_QUEUES_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 0;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			putcrlf();
			putstg("QUEUE #,   Status    , Max Slots , Slots Used, Slots free, Slot size, QUE. Addr \r\n");
			while (1)
				{
				if (hex_byte < MAX_QUEUES)
					{
					queue_ptr = &queue[hex_byte];	/* get address of proper queue handler. */
					/* maybe change to decimal, later */
					putspaces(3);
					putbyte(hex_byte);	/* output task number */
					putspaces(4);
					if (!queue_ptr->num_slots)
						putstg("NOT CREATED ");
					else if (!queue_ptr->queue_cnt)
						putstg("EMPTY       ");
					else if (queue_ptr->queue_cnt == queue_ptr->num_slots)
						putstg("FULL        ");
					else
						putstg("PARTIAL     ");
					putspaces(3);
					putword(queue_ptr->num_slots);
					putspaces(6);
					putword(queue_ptr->queue_cnt);
					putspaces(7);
					putword(queue_ptr->num_slots - queue_ptr->queue_cnt);
					putspaces(7);
					putbyte(queue_ptr->size_slot);
					putstg("      0x");
					if ((sizeof(char *)) == 4)
						puthexlong((word32)(queue_ptr->base_ptr));	/* TCB address */
					else
						puthexword((word16)(queue_ptr->base_ptr));	/* TCB address */
					putcrlf();	
					}
				else
					{
					putstg("QUEUE Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte >= MAX_QUEUES)
						{
						break;
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void dump_mailbox(void)
{
	MAILBOX *mail_ptr;
	MSG *link;	/* scratch pointer */
	word16 ctr;

	prompt_index = DUMP_MAILBOX_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			common_prompt();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte == '*')
				{
				keyflags.loop = TRUE;
				hex_byte = 0;
				}
			else
				{
				keyflags.loop = FALSE;
				}
			putcrlf();
			putstg("MAILBOX #, # of MESG, TASK waiting , TASK to Sig  , EVENT\r\n");
			while (1)
				{
				if (hex_byte < MAX_MAILBOXES)
					{
					mail_ptr = &mail_box[hex_byte];	/* get address of proper queue handler. */
					/* maybe change to decimal, later */
					putspaces(3);
					putbyte(hex_byte);	/* output task number */
					putspaces(6);
					ctr = 0;
					link = mail_ptr->first_lnk;	/* get first link. */
					while(1)
						{
						if (!link)
							break;
						link = link->link;
						ctr++;
						}
					putword(ctr);
					putspaces(4);
					if (mail_ptr->waiter)	/* task waiting on this mailbox */
						{
						if (task_name[mail_ptr->waiter - cmx_tcb])
							putname(task_name[mail_ptr->waiter - cmx_tcb]);
						else
							putspaces(12);
						}
		 			else
						putstg(" NONE       ");
					if (mail_ptr->task_num)
						{
						putspaces(1);
						if (task_name[mail_ptr->task_num])
							putname(task_name[mail_ptr->task_num]);
						else
							putspaces(12);
						putstg(" 0x");
						puthexword(mail_ptr->event_num);
						}
					putcrlf();	
					}
				else
					{
					putstg("MAILBOX Number out of range\r\n");
					keyflags.loop = FALSE;
					}
				if (keyflags.loop)
					{
					if (++hex_byte >= MAX_MAILBOXES)
						{
						break;
						}
					}
				else
					{
					break;
					}
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void dump_memory_ram(void)
{
	byte row;
	byte ctr;

	prompt_index = DUMP_RAM_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			keyflags.allow_hex = FALSE;
			putcrlf();
			prompt_out();
			break;
		case 0x80:	/* process card number prompt */
			keyflags.allow_hex = FALSE;
			func_offset &= 0x7f;
			from_ptr = (mem_size_ptr)hex_long;
			func_offset = 0x01;
			prompt_out();
			break;
		case 0x81:	/* process card number prompt */
			keyflags.allow_hex = FALSE;
			func_offset &= 0x7f;
			if (!keyflags.scroll)
				{
				thru_ptr = (mem_size_ptr)hex_long;
				if (thru_ptr < from_ptr)
					{
					putstg("\r\nEnding address MUST be equal or greater from address");
					prompt_out();
					break;
					}
				}
			putcrlf();
			putstg("\r\n             00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
			for (ctr = 0; ctr < 16; ctr++)
				{
				putcrlf();
				putspaces(1);
				puthexlong((word32)from_ptr & 0xfffff0);
				putspaces(2);
				for (row	= 0; row < 16; row++)
					{
					if (((word32)from_ptr & 0x00000f) == row)
						break;
					putspaces(3);
					}
				for (; row < 16; row++)
					{
					puthexbyte(*from_ptr);
					if (from_ptr++ == thru_ptr)
						{
						keyflags.scroll = FALSE;
						row = 0xff;
						break;
						}
					putspaces(1);
					}
				if (row == 0xff)
					break;
				}
			if (row < 0xff)
				{
				keyflags.scroll = TRUE;
				putstg("\r\n\r\nEnter <return> to continue");
				return;
				}
			keyflags.cont = TRUE;
			break;
			}
}

static void change_rtc_scale(void)
{
	prompt_index = RTC_SCALE_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			putbyte(RTC_SCALE);
			func_offset++;
			prompt_out();
			break;
		case 0x81:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte)
				RTC_SCALE = hex_byte;
			keyflags.f_key = TRUE;
			keyflags.cont = TRUE;
			break;
			}
}

static void change_tslice_scale(void)
{
	prompt_index = TSLICE_SCALE_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			putbyte(TSLICE_SCALE);
			func_offset++;
			prompt_out();
			break;
		case 0x81:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte)
				TSLICE_SCALE = hex_byte;
			keyflags.f_key = TRUE;
			keyflags.cont = TRUE;
			break;
			}
}

static void enable_tslice(void)
{
	prompt_index = ENABLE_SLICE_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			if (SLICE_ON)
				putstg("Enabled");
			else
				putstg("Disabled");
			func_offset++;
			prompt_out();
			break;
		case 0x81:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte)
				SLICE_ON = 1;
			else
				SLICE_ON = 0;
			keyflags.f_key = TRUE;
			keyflags.cont = TRUE;
			break;
			}
}

static void delay_tick(void)
{
	prompt_index = DELAY_TICK_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			/* maybe change to decimal, later */
			if (hex_long < 0xffff && hex_long)
				{
				putstg("\r\nBUG task, waiting for the following number of TICKS ");
				putword(hex_long);
				CMXBUG_ACTIVE = 0;
				locked_out = 0;
				K_Task_Wait(hex_long);
				locked_out = 1;
				CMXBUG_ACTIVE = 1;
				keyflags.f_key = TRUE;
				keyflags.cont = TRUE;
				break;
				}
			else
				{
				putstg("\r\nMust be non zero number and less 65536!");
				func_offset = 0;
				keyflags.cont = TRUE;
				break;
				}
			}
}

static void quick_tick(void)
{
		CMXBUG_ACTIVE = 0;
		locked_out = 0;
		K_Task_Wait(1);
		locked_out = 1;
		CMXBUG_ACTIVE = 1;
		keyflags.f_key = TRUE;
		keyflags.cont = TRUE;
}


static void stat_display(void)
{
	word32 tick_out;

		if (!keyflags.loop)
			{
			keyflags.loop = TRUE;
			total_ticks = 0;
			for (hex_byte = 0; hex_byte <= MAX_TASKS; hex_byte++)
				total_ticks  += stat_array[hex_byte];
			hex_byte = 1;
			}
		if (!total_ticks)
			{
			if (!keyflags.scroll)
				{
				keyflags.scroll = TRUE;
				hex_byte = 0xFF;
				putstg("\r\n\r\nThe TIME ANALYSIS array is empty\r\n");
				putstg("Enter <return> to EXIT");
				return;
				}
			}
		putcrlf();
		if (hex_byte != 0xFF)
			putstg("SLOT #,    NAME    , Number of Ticks, % of TICKS\r\n");

		tick_out = stat_array[activetcb - cmx_tcb];	/* get number of bug ticks */
		stat_array[activetcb - cmx_tcb] = 0;
		stat_array[0] += tick_out;
		while (1)
			{
			if (hex_byte <= MAX_TASKS)
				{
				/* MUST change to relect slot number and name */
				putspaces(1);
				putbyte(hex_byte);	/* output task number */
				putspaces(2);
				if (task_name[hex_byte])
					putname(task_name[hex_byte]);
				else
					putspaces(12);
				putspaces(3);
				if (stat_array[hex_byte])
					{
					putlong(stat_array[hex_byte]);
					}
				else
					{
					if (cmx_tcb[hex_byte].tcbstate && cmx_tcb[hex_byte].tcbstate & ~IDLE)
						{
						putstg("   < 1");
						}
					else
						{
						putstg("     0");
						}
					}
				putstg("        %");
				tick_out = ((byte)((stat_array[hex_byte] * 100) / total_ticks));
				putpercent((byte)(tick_out),0);
				putstg(".");
				putpercent((byte)((((stat_array[hex_byte] * 100) - (tick_out * total_ticks)) * 100) / total_ticks),1);
				putcrlf();	
				}
			else
				{
				if (hex_byte == 0xFF)
					{
					keyflags.f_key = TRUE;
					keyflags.cont = TRUE;
					keyflags.loop = FALSE;
					keyflags.scroll = FALSE;
					return;
					}
				}
			if (++hex_byte > MAX_TASKS)
				{
				keyflags.scroll = TRUE;
				hex_byte = 0xFF;
				putstg("       IDLE          ");	/* output task number */
				putlong(stat_array[0]);
				putstg("        %");
				tick_out = ((byte)((stat_array[0] * 100) / total_ticks));
				putpercent((byte)(tick_out),0);
				putstg(".");
				putpercent((byte)((((stat_array[0] * 100) - (tick_out * total_ticks)) * 100) / total_ticks),1);
				putcrlf();	
				putstg("Enter <return> to EXIT");
				return;
				}
			else
				{
				if (hex_byte % SCROLL_SIZE == 0)
					{
					keyflags.scroll = TRUE;
					putstg("Enter <return> to continue");
					return;
					}
				}
			}
}

static void stat_reset(void)
{
	prompt_index = STATS_RESET_PROMPT;

	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			putcrlf();
			prompt_out();
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			if (hex_byte)
				{
				for (hex_byte = 0; hex_byte <= MAX_TASKS; hex_byte++)
					stat_array[hex_byte] = 0;
				}
			keyflags.f_key = TRUE;
			keyflags.cont = TRUE;
			break;
			}
}

static void task_ctrl_func(void)
{
	switch (func_offset) {
		case 0x00:	/* display 1'st prompt */
			keyflags.string_allowed = TRUE;
			keyflags.string = FALSE;
			prompt_index = TASK_CTRL_PROMPT;
			putcrlf();
			do {
				prompt_out();
				} while (++func_offset < 6);
			func_offset = 0;
			prompt_index = TASK_CTRL_PROMPT + 5;
			break;
		case 0x80:	/* process card number prompt */
			func_offset &= 0x7f;
			switch (hex_byte) {
				case 1:
					func_offset = 1;
					prompt_out(); 
					com1_prompt();
					break;
				case 2:
					func_offset = 2;
					prompt_out();
					com1_prompt();
					break;
				case 3:
					func_offset = 3;
					prompt_out();
					com1_prompt();
					break;
				case 9:
					current_func = WHAT_FUNCTION;
					keyflags.f_key = TRUE;
					keyflags.cont = TRUE;
					return;
				default:
					keyflags.cont = TRUE;
					break;
				}
			break;
		case 0x81:	/* start task */
			func_offset = 0x00;
			if (keyflags.string)
				{
				for (hex_byte = 1; hex_byte <= MAX_TASKS;hex_byte++)
					{
					if (!strncmp((char *)keys_in_buff,task_name[hex_byte],num_chars_in))
						break;
					}
				if (hex_byte > MAX_TASKS)
					{
					putstg("\r\nTask name NOT found");
					keyflags.cont = TRUE;
					break;
					}
				}
			if ((activetcb - cmx_tcb) == hex_byte)
				putstg("\r\nManipulating CMXBug task is Illegal");
			else if (hex_byte > MAX_TASKS)
				putstg("\r\nTask number out of range");
			else if (cmx_tcb[hex_byte].nxttcb)
				{
				putstg("\r\nTask -- ");
				if (task_name[hex_byte])
					putname(task_name[hex_byte]);
				else
					putspaces(12);
				putstg(" --, has been STARTED");
				K_Task_Start(hex_byte);
				}
			else
				putstg("\r\nTask is either not created or has been deleted");
			keyflags.cont = TRUE;
			break;				
		case 0x82:	/* stop task */
			func_offset = 0x00;
			if (keyflags.string)
				{
				for (hex_byte = 1; hex_byte <= MAX_TASKS;hex_byte++)
					{
					if (!strncmp((char *)keys_in_buff,task_name[hex_byte],num_chars_in))
						break;
					}
				if (hex_byte > MAX_TASKS)
					{
					putstg("\r\nTask name NOT found");
					keyflags.cont = TRUE;
					break;
					}
				}
			if ((activetcb - cmx_tcb) == hex_byte)
				putstg("\r\nManipulating CMXBug task is Illegal");
			else if (hex_byte > MAX_TASKS)
				putstg("\r\nTask number out of range");
			else if (!cmx_tcb[hex_byte].nxttcb)
				putstg("\r\nTask is either not created or has been deleted");
			else if (cmx_tcb[hex_byte].tcbstate == IDLE || \
						cmx_tcb[hex_byte].tcbstate == READY)
				putstg("\r\nTask is NOT started");
			else if (cmx_tcb[hex_byte].tcbstate & ANY_WAIT)
				putstg("\r\nTask is already WAITING on some entity");
			else
				{
				putstg("\r\nTask -- ");
				if (task_name[hex_byte])
					putname(task_name[hex_byte]);
				else
					putspaces(12);
				putstg(" --, has been STOPPED");
				cmx_tcb[hex_byte].tcbstate	= WAIT;
				}
			keyflags.cont = TRUE;
			break;				
		case 0x83:	/* process card number prompt */
			func_offset = 0x00;
			if (keyflags.string)
				{
				for (hex_byte = 1; hex_byte <= MAX_TASKS;hex_byte++)
					{
					if (!strncmp((char *)keys_in_buff,task_name[hex_byte],num_chars_in))
						break;
					}
				if (hex_byte > MAX_TASKS)
					{
					putstg("\r\nTask name NOT found");
					keyflags.cont = TRUE;
					break;
					}
				}
			if ((activetcb - cmx_tcb) == hex_byte)
				putstg("\r\nManipulating CMXBug task is Illegal");
			else if (hex_byte > MAX_TASKS)
				putstg("\r\nTask number out of range");
			else if (!cmx_tcb[hex_byte].nxttcb)
				putstg("\r\nTask is either not created or has been deleted");
			else if (!(cmx_tcb[hex_byte].tcbstate & ANY_WAIT))
				putstg("\r\nTask is NOT waiting on some entity");
			else
				{
				putstg("\r\nTask -- ");
				if (task_name[hex_byte])
					putname(task_name[hex_byte]);
				else
					putspaces(12);
				putstg(" --, has been WOKEN");
				K_Task_Wake_Force(hex_byte);
				}
			keyflags.cont = TRUE;
			break;				
			}
}
