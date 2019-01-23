/// Msghndl.c

/*      
	Start Date:	19-November-1995
	handles the reading of messages from host, executes commands and
	prepares reply message packets.
	Stage 1 - Detects message, Accept message or rejects 
		  Choises
		  1. New Message
		  2. Ongoing Message
	Stage 2 - Reads Message Header, Accepts or Rejects,
	          Ignores when  
	Stage 3 - Reads Message Data, 
	Stage 4 - Execute message with data 
	Stage 5 - Prepare Reply Message Packet   
	First Implementation with dummy execution 
	This module is designed to be transparent to protocol

	ported 7/11/96 DS no mods.
*/

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL

#include "extern.h" 
#include "msghndl.h" 
#include "sci_mon.h"

#pragma section MONITOR 
            
extern void RecoverIsr(void);

void SetRegister (BYTE, DWORD);
BYTE MsgInBuffer[MSG_INBUF_LENGTH];     
BYTE MsgOutBuffer[MSG_OUTBUF_LENGTH];   
void LoadLong (BYTE *, DWORD);
DWORD MakeLong (BYTE *);
void MemoryMessage ();
void MemorySetInit ();
void RegisterMessage ();
void mc_breakpoint_setting ();
void mc_capability ();
void mc_execute ();
void mc_memory_calculate_signature ();
int mc_memory_fill (BYTE);
void mc_memory_scan ();
void ResetReceiveBuffer ();
void mc_io_write ();	
void mc_io_read ();

void MsgHnd_Stage1(void)
{      

	/* Note: if case uses BuildReplyMessage() it must use return, when not using return 
	   then by default BuildReplyMessage(MR_COMPLETE) is called to save multiple calls */

   extern int download_failure;

	switch(Rx_Error)
	{  
        	case MC_INITIALISE :      		/* 1 */
        		RecoverIsr(); 
        		InitMonitor();  
        		break;
        		
        	case MC_CAPABILITY_QUERY :  		/* 2 */   
        		mc_capability();		  /* no BuildReplyMessage used here */
        		return;
        	
        	case MC_EXECUTE :        		/* 3 */    
        		BuildReplyMessage(MR_END_OF_EXECUTION);	    
        		return;
        	
        	case MC_BREAK :             		/* 4 */
        		break;
        		
        	case MC_REGISTER_SETTING :   		/* 5 */ 
        		SetRegister(RxBuffer[0],MakeLong(&RxBuffer[1]));	
               		break;
          
        	case MC_REGISTER_QUERY :      		/* 6 */     
        		RegisterMessage();
             		return;
        	
        	case MC_MEMORY_VERIFY :     		/* 7 */ 
        		SetVerify(VERIFY);	/* set to VERIFY mode */
        			
        	case MC_MEMORY_SETTING :    		/* 8 */  
        		MemorySetInit();     			
        		break;            
            
        	case MC_MEMORY_FILL :    		/* 9 */    
        		mc_memory_fill(MC_MEMORY_FILL);
        		break;
        
        	case MC_MEMORY_QUERY :      		/* 10 */  
        		MemoryMessage();
        		return;
      
        	case MC_MEMORY_SCAN :    		/* 11 */ 
        		mc_memory_scan();
        		return;
        
        	case MC_MEMORY_VERIFY_FILL : 		/* 12 */
        		if(!mc_memory_fill(MC_MEMORY_VERIFY_FILL))
        		{
        			TxBuffer[0] = MR_VERIFY_FILL_ERROR; 
        			Tx_Mesg_Length = 5;
        			return;
        		}
        		break;
        
        	case MC_MEMORY_CALCULATE_SIGNATURE :  	/* 13 */  
        		mc_memory_calculate_signature();
        		return;
        
        	case MC_BREAKPOINT_SETTING :          	/* 14 */   
        		mc_breakpoint_setting();
        		return;
        
        	case MC_DOWNLOAD :       		/* 15 */
        		/* reset receive buffers */   
        		ResetReceiveBuffer();  
        		if (download_failure)
        		{
        			TxBuffer[0] = MR_DOWNLOAD_FAILURE;
					Tx_Mesg_Length = 1;
        			return;
        		}
        		break;      
        			
        	case MC_MEMORY_GENERAL :      		/* 16 */  
        		TxBuffer[1]=mc_memory_general();             	/* loads result straight away */
        		return;

		case MC_IO_READ :					/* 17 */
			mc_io_read();
			return;

		case MC_IO_WRITE :					/* 18 */
			mc_io_write();
			return;
             		
               	default :               		/* 19 */  
               		BuildReplyMessage(MR_UNKNOWN_ERROR);
               		return;
        	
       	} /* end of switch */   
       	
       	BuildReplyMessage(MR_COMPLETE);
}   		
  
/*******************************************************************
	This functions implements Stage 5 
	1. Set the Reply Buffer to point to the desired memory location
	2. Load Buffer with reply data
	3. Determine the size of the buffer and returns the value
********************************************************************/

void BuildReplyMessage(int reply)
{  
	Tx_Mesg_Length = 1;  	    /* this is a optimisation mode */
	switch(reply)
	{
        	case MR_COMPLETE :      		/* 1 */  
        		TxBuffer[0] = MR_COMPLETE; 
        		break;
        				
        	case MR_END_OF_EXECUTION :             	/* 2 */   
                        mc_execute();
        		break;
        		
        	case MR_MEMORY_SCAN_RESULT :    	/* 3 */
        		break;
        
        	case MR_MEMORY_VERIFY_RESULT :      	/* 4 */
        		break;
        
        	case MR_RECEPTION_ERROR : 		/* 6 */ 
        		TxBuffer[0] = MR_RECEPTION_ERROR; 
        		ResetReceiveBuffer();
        		break;
        
        	case MR_VERIFY_ERROR : 			/* 7 */ 
        		TxBuffer[0] = MR_VERIFY_ERROR; 
        		LoadLong(&TxBuffer[1],LastAddress());	/* loads the failing address */
        		Tx_Mesg_Length = 5;  
        		ResetReceiveBuffer();
        		break;
        
               	default :               		/* 8 */   
               		TxBuffer[0] = MR_UNKNOWN_ERROR; 
               		break;
        	
       	} /* end of switch */	
}
