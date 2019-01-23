/// Main.c

/* 
	Start Date:	17-11-1995
	This module contains the main monitor module
	ported 7/11/96 DS

	11/12/97 1.0		EJ 		Stenkil Software
    ported to H8S/2655 
	Test EVB added. EVB test is enabled when IRQ0 is pressed.
    The user application may overwrites the IRQ0 test vector, disabling the test functionality.
    The test uses SCI channel 1, 38400 baud, N, 8, 1
    #ifdef H8S2655 added
    #ifndef H8S2655 added

*/       
/* Modification History:

08-Jan-98 FJL Changed date on IRQ0 sign-on for identification purposes

09-Jan-98 FJL Added checksum options and function to IRQ0 test mode.
              New functions: calc_checksum(), PutHexInt_SCI1()
 
05-Mar-99 DTH Ported to 2623
// Ported to 2633 1/10/01  DGL
*/


#include "extern.h" 
#include "msghndl.h"

#include "2623regs.h"
#include "sci_mon.h"    

#pragma section MONITOR 

extern BYTE MsgInBuffer[MSG_INBUF_LENGTH];   
extern BYTE MsgOutBuffer[MSG_OUTBUF_LENGTH]; 
void InitExecution (void); 

/* globals that monitor the state of comunication */ 
BYTE Rx_Error,Tx_Error;	 
	
/* 
	globals for handling reply message 
	special case Tx_Mesg_Length = 0 no transmission message required
*/         
int Tx_Mesg_Length;
BYTE *TxBuffer;  
BYTE *RxBuffer; 

void InMsgHndl()
{
	Rx_Error=RxMessage(RxBuffer);
		
	switch (Rx_Error)
	{
	 	case SCIE_OVERFLOW: 
	 	case SCIE_CRC: 
	 	case SCIE_RX: 
	 	case SCIE_NODATA: 
	 	case SCIE_UNEXSOM: 
	 	case SCIE_NOSOM:  
	 		BuildReplyMessage(MR_RECEPTION_ERROR);
	 		break;

	 	case SCIE_VERIFY:
	 		BuildReplyMessage(MR_VERIFY_ERROR);
	 		break;		

	 	case SCIE_TMOT: 
	 		Tx_Mesg_Length = 0;	/* IDLE Mode defined Here */     
			break;  
		default: 
			MsgHnd_Stage1();
			break;   
	}
} 

void OutMsgHndl()
{
	if (Tx_Mesg_Length)
	{    
		TxMessage(TxBuffer,Tx_Mesg_Length);
		Tx_Mesg_Length = 0; 
		TxBuffer = &MsgOutBuffer[0];  
	}
}	

#ifdef H8S2623
	extern int mon_sci_init; // See sci_mon.c
#endif

void mon_main()
{     
	
	BYTE buffer[2];
	
#ifdef H8S2623
	mon_sci_init = 0;
#endif

    MSTPCRA = 0x00;	/*--- Clear stops for DTC, TPU, PPG & A/D    ---*/
    MSTPCRB = 0x00;	/*--- Clear stops for SCI0, SCI1 & SCI2      ---*/
    MSTPCRC = 0x00;	/*--- Clear stops for PBC. ---*/
	
	InitExecution();     /* initialise BP table */
	InitMonitor(); 

	buffer[0] = MR_RESET;
	buffer[1] = 0;
	TxMessage (buffer,2);

	/* main monitor loop */
	while(1)
	{ 
		InMsgHndl();  	/* incoming message handler */
		OutMsgHndl();   /* outgoing message handler */
    }
}
