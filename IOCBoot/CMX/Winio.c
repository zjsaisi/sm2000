/* IO functions for winbug.c and wintrack.c */

#define CMXMODULE

#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include <cxfuncs.h>
#endif
#include <winio.h>

void setbaud(void);

#if __CPU__ > 3
/* these values are for 20 Mhz clock */
#define BAUD_384	15
#define BAUD_192	32
#define BAUD_96	64
#else
/* these values are for 13 Mhz clock */
#define BAUD_384	10
#define BAUD_192	20
#define BAUD_96	41
#endif

void setbaud(void)
{
#if __CPU__ > 3
		/* The following is for serial channel 1, H8S/2357 processor */
		MSTPCRL &= 0xBF;		/* clear module stop for SCI 1 */
		SCI1_SMR = 0x00;		/* set BAUD rate to 9600, @ 20MHZ */
		SCI1_BRR = BAUD_384;	/* set bit rate register */
		SCI1_SCR = 0x30;	/* Enable reciever and transmitter, NO interrupts */
								/* Note that TDRE bit should now be set to 1 */
#else
		/* The following is for serial channel 0, H83048 processor */
		SCI0_SMR = 0x00;		/* set BAUD rate to 9600, @ 13MHZ */
		SCI0_BRR = BAUD_384;	/* set bit rate register */
		SCI0_SCR = 0x30;	/* Enable receiver and transmitter, NO interrupts */
								/* Note that TDRE bit should now be set to 1 */
#endif
}

