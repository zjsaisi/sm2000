/// Init.c

/*
	Start Date:	17-11-1995
	This module defines and controls the Monitor Protocol 
	parameters. Effectivelly defining the Monitor State             
	V1.0		GL	First cut		1/3/96
	V1.1		GL	Add default registers	24/5/96
	Machine.

	ported 7/11/96 

    H8S monitor									
	11/12/97 1.0		EJ 		Stenkil Software
                                #ifdef H8S2655 regs.er7 = 0x23ff00; 
                                #ifdef H8S2655 BAUDRATE is 57600

*/ 
/* Modification History:

12-Jan-98 FJL Moved user stack pointer from 0x23ff00 to 0x23fbfc
05-Mar-99 DTH Ported to 2623
// Ported to 2633 1/10/01  DGL
*/

#include "extern.h"  
#include "sci_mon.h"  
#include "msghndl.h"   
#include "break.h"      

#pragma section MONITOR 

extern void Set_MaxRxBuffer_Size (DWORD); 
ERCD BPremoveAll (void);

void ResetSystemFlags()
{
}

void ResetReceiveBuffer()
{
	SetVerify(WRITE);	/* set to WRITE mode */
	RxBuffer = &MsgInBuffer[0];   	/* initialise to monitor receive buffer */
	Set_MaxRxBuffer_Size(MSG_INBUF_LENGTH);   /* set buffer size */
}

int InitMonitor()
{ 
	extern void initshv ();
	ResetSystemFlags(); 
	
	/* set default registers */  

	regs.er7 = 0xFFC000;      /*--- Mode 6 generic stack pointer    ---*/
	 	
	/* initialise shadow vector table */
	initshv ();

	/* Initialise Serial Comms 1 */
//	InitSCI(B38400, P_NONE, 1, 8);
//	InitSCI(B115200, P_NONE, 2, 8);
	InitSCI(B57600, P_NONE, 1, 8);
	 
	/* Initialise to no transmit messages */    
	Tx_Mesg_Length=0; 
	
	/* Initialise Receive Buffer to Local Buffer */ 
	ResetReceiveBuffer();  
	
	/* Initialise Transmit Buffer to Local Buffer */
	TxBuffer = &MsgOutBuffer[0];
	   	
	/* Remove all active break points */
	BPremoveAll();	   	
}  
