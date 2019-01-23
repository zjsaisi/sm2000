/// Control.c

/********************************************************/
/* DS: 10/1/97 changed RAM area 2 to take into account  */
/*     on-chip regs and on-chip RAM 			        */
/*======================================================*/
/*  H8S monitor											*/
/*	11/12/97 1.0	EJ 		Stenkil Software            */
/*                          #ifdef H8S2655 added.       */
/*                          functions MakeLong and      */
/*                          MakeShort updated to be     */
/*                          handled correctly by the    */
/*                          IAR H8S compiler            */
/*														*/
/*														*/
/********************************************************/
/* Modification History:

07-Jan-98 FJL - Corrected size of RAM1 area in capabilities structure.
08-Jan-98 FJL - Modified mc_memory_scan() to avoid faulty array reference
08-Jan-98 FJL - Modified mc_memory_fill() to avoid faulty array reference
08-Jan-98 FJL - Incremented the minor version number (from 0 to 1)
03-Mar-99 DTH - Ported to 2623
// Ported to 2633 1/10/01  DGL
*/


#include "extern.h"   
#include "msghndl.h" 
#include "sci_mon.h"
#include "Constdef.h"

#pragma section MONITOR 

char BreakPointExists (SH_REG);
ERCD BPremove (SH_REG);
ERCD BPset (SH_REG, SH_INST);
ERCD ExecuteUserCode (BYTE, WORD);
void OutMsgHndl ();
                               
typedef struct {
	BYTE identifier;      	/* REPLY IDENTIFIER */
	BYTE serial;    	/* RElEASE NUMBER */	
	BYTE version;      	/* DIGITS AFTER RELEASE */
	BYTE clock;   		/* HARDWARE SUPPORTED CLOCK */	
	
	BYTE CPU;  		/* CPU TYPE 0 FOR SH1, 1 FOR SH2, 2 FOR SH3, ? FOR SH-DSP.  See UDI defines for more info */
	BYTE BREAKS;           	/* NR OF BREAKPOINT SUPPORTED */
	BYTE s_rxbuf;           /* sIZE OF THE RECEIVE BUFFER */
	BYTE s_txbuf;          	/* SIZE OF THE TRANSMIT BUFFER */     
	
	DWORD ROM1_S;          	/* START ADDRESS OF ROM AREA 1 */
	DWORD ROM1_E;      	/* END ADDRESS OF ROM AREA 1 */
	DWORD ROM2_S;		/* START ADDRESS OF ROM AREA 2 */
	DWORD ROM2_E;		/* END ADDRESS OF ROM AREA 2 */
	
	DWORD RAM1_S;		/* START ADDRESS OF RAM AREA 1 */
	DWORD RAM1_E; 		/* END ADDRESS OF RAM AREA 1 */
	DWORD RAM2_S;		/* START ADDRESS OF RAM AREA 2 */
	DWORD RAM2_E;		/* END ADDRESS OF RAM AREA 2 */   
	
	BYTE MEM_WIDTHS;	/* physical widths of above memory areas */   
	
}CAPABILITY;

/*************************************************************************
 *                                                                       *
 *       H8S2623 CAPABILITIES STRUCTURE                                  *
 *                                                                       *
 *************************************************************************
 *                                                                       *
 * NOTE : The 2623 PROTOTYPE board is using SCI2 for it's comms, so A17  *
 *        has to be disabled. This means that we lose the second half of *
 *        external RAM! The final EVB uses the external UART so this is  *
 *        not a problem for this build.                                  *
 *                                                                       *
 *************************************************************************/
 
		const CAPABILITY Capability =
		{  
			MR_CAPABILITY, 			/* Reply header            */
			1, 						/* Release number          */
			0,                    	/* Version number          */
			18,                     /* 20MHz clock source      */
			17, 				    /* ID for H8S/2600         */
			MAX_BP, 				/* Max breakpoints         */
			MSG_INBUF_LENGTH, 		/* Input buffer length     */
			MSG_OUTBUF_LENGTH,		/* Output buffer length    */
			0x0, 					/* Start of ROM area 1     */
			0x3ffff, 				/* End of ROM area 1       */
			0x0, 					/* Start of ROM area 2     */
			0x0, 					/* End of ROM area 2       */
			0x200000,                /* Start of RAM area 1     */
			0x3fffff,                /* End of RAM area 1       */
			0xffb000,               /* Start of RAM area 2     */
			0xffefbf,               /* End of RAM area 2       */
			(0<<6)   |  (3 <<4) |  (1 << 2)  |    0
		/*  - ROM1 ----- ROM2 ------ RAM1 ------ RAM2 -------  */
		/*  - 8 bit ---- N/A------- 16 bit----- 8 bit -------  */
		};    

REGS 	regs;  

char global_char;  

DWORD MakeLong(BYTE *buf)
{
    unsigned long ll, tmp;

#if 0
/*	This line don't work in H8S compiler */
    return ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]));
#else
    
    tmp = (unsigned long)buf[0]  << 8;
	tmp += (unsigned long)buf[1];
    ll = (tmp << 16);

	tmp = (unsigned long)buf[2] << 8;
	tmp += (unsigned long)buf[3];
    ll += tmp;

	return ll;
#endif
}      
void LoadLong(BYTE *buf, DWORD val)
{
	buf[0]= val >> 24;
	buf[1]= val >> 16;
	buf[2]= val >> 8;
	buf[3]= val;
}
void LoadShort(BYTE *buf, WORD val)
{
	buf[0]= val >> 8;
	buf[1]= val;
}
WORD MakeShort(BYTE *buf)
{
    unsigned short ss;

/*	This line don't work in H8S compiler 
    return (buf[0] << 8) | (buf[1]); */
	ss = (unsigned short)buf[0] << 8;
	ss += (unsigned short)buf[1];
	return ss;
}
/* this function reads the message buffer and sets the specified register */
void SetRegister(BYTE reg_id, DWORD value)
{     
	((DWORD *)&regs)[reg_id] = value;	
}          
void LoadRegister(BYTE *Buffer, BYTE reg_id)
{     	
	Buffer[0] = ((DWORD *)&regs)[reg_id] >> 24; 
	Buffer[1] = ((DWORD *)&regs)[reg_id] >> 16;
	Buffer[2] = ((DWORD *)&regs)[reg_id] >> 8;
	Buffer[3] = ((DWORD *)&regs)[reg_id];
}
		
DWORD GetRegister(BYTE reg_id)
{
	return ((DWORD *)&regs)[reg_id];
}

extern void SCIrxError(void);
extern void SCIrx(void); 
// Added by Zheng Miao 2/12/2003 to support SCI0 ISR to RAM
void RecoverIsr(void)
{  
		unsigned long *ptr;    
#ifdef MON_SCI0 		
		ptr = (unsigned long *)(RAM_START + 0x140);
		*ptr = JUMP_INSTR + (unsigned long)SCIrxError;
		ptr = (unsigned long *)(RAM_START + 0x142);
		*ptr = JUMP_INSTR + (unsigned long)SCIrx; 
#endif	
#ifdef MON_SCI2
		ptr = (unsigned long *)(RAM_START + 0x160);
		*ptr = JUMP_INSTR + (unsigned long)SCIrxError;
		ptr = (unsigned long *)(RAM_START + 0x162);
		*ptr = JUMP_INSTR + (unsigned long)SCIrx; 
#endif	

}
   
void mc_execute()  
{               
	// Zheng Miao 2/6/2003: Make sure SCI0 rxError and rx is right
	RecoverIsr();    
	
	/* send acknowledge */    
	TxBuffer[0]=MR_COMPLETE;
	OutMsgHndl(); 
		
    global_char=ExecuteUserCode(RxBuffer[0], MakeShort(&RxBuffer[1]));   
    if (global_char == ERCD_SCI)
	    RxMessage(0); /* Clear any remaining data from Rx buffer.  This should consist of a MC_BREAK message */
	      
	TxBuffer[0] = MR_END_OF_EXECUTION; 
    TxBuffer[1] = global_char;
    Tx_Mesg_Length = 2;
    RecoverIsr();
}     
void mc_capability()
{
	TxBuffer = (BYTE *)&Capability;    
	Tx_Mesg_Length = sizeof (CAPABILITY);      /* set message length to length of data transfer */
}    
	
void RegisterMessage()
{       
	/* send acknowledge */  
	Tx_Mesg_Length = 1;  
	TxBuffer[0]=MR_COMPLETE;
	OutMsgHndl(); 	
	
        if (RxBuffer[0] != REG_ALL)
        {    	
     		LoadRegister((BYTE *)&TxBuffer[0], RxBuffer[0]);
        	Tx_Mesg_Length = 4;   
        } 
        else
        { 	
        	/* set transmit buffer to point to registers */ 
        	TxBuffer = (BYTE *)&regs;
        	Tx_Mesg_Length = sizeof (REGS);
        }   
}
void MemoryMessage()
{ 
	/* send acknowledge */  
	Tx_Mesg_Length = 1;  
	TxBuffer[0]=MR_COMPLETE;
	OutMsgHndl(); 	

    /* now build the memory to be read */
    TxBuffer = (BYTE *)MakeLong((BYTE *)&RxBuffer[0]);    /* set transmit buffer to memory location */
    Tx_Mesg_Length = MakeLong((BYTE *)&RxBuffer[4]);      /* set message length to length of data transfer */
} 

void MemorySetInit()
{   
	/* Rx[0-3] address */
	/* Rx[4-8] length  */ 
	
	Set_MaxRxBuffer_Size(MakeLong(&RxBuffer[4]));
	RxBuffer = (BYTE *)MakeLong(&RxBuffer[0]); 
} 

char Writable (SH_REG);

int mc_memory_fill(BYTE option)
{
	BYTE *address = (BYTE *)MakeLong(&RxBuffer[0]); 	/* specifies the starting memory location */ 
	BYTE patern_length = RxBuffer[4];                       /* get pattern length */
	DWORD Length = MakeLong(&RxBuffer[5]);  		/* get total length of target memory */
	BYTE *patern_address = &RxBuffer[9];  			/* get pattern address  note this rx buffer address */
	long i;
	
	while(1)
	{
		for ( i=0;i< patern_length;i++)
		{
			if(!Length) return TRUE;                      /* if Length = 0 return, function complete */
			if (option == MC_MEMORY_FILL) 
			{ 
				*address++ = *((BYTE *)patern_address+i);
			} 
			else
			{
				if(*address != *((BYTE *)patern_address+i))
				{  	
					LoadLong(&TxBuffer[1],(DWORD)address);	/* loads the failing address */
					return FALSE;   
				}
				address++;
			}
			Length--;
		}
	} 
}  
/* this function return TRUE if buffer match or
   FALSE if their is a mismatch */
int memory_scan(BYTE *buf1,BYTE *buf2,DWORD len)  
{
	while(len--)
		if (*buf1++ != *buf2++) return FALSE; 
	return TRUE;
}
void mc_memory_scan()
{
	BYTE *address = (BYTE *)MakeLong(&RxBuffer[0]); 	/* specifies the starting memory location */ 
	BYTE patern_length = RxBuffer[4];                       /* get pattern length */
	DWORD Length = MakeLong(&RxBuffer[5]);  		/* get total length of target memory */
	BYTE *patern_address = &RxBuffer[9];  			/* get pattern address  note this rx buffer address */
	DWORD i;
	
	
	for (i=0;i<Length;i++)   
	  	if (memory_scan(address++,patern_address,patern_length) == TRUE) break; 
	  	
	/* build reply */
	TxBuffer[0]=MR_MEMORY_SCAN_RESULT;  
	LoadLong(&TxBuffer[1],i);
	Tx_Mesg_Length = 5;  
}

void mc_memory_calculate_signature()
{		
	BYTE *address = (BYTE *)MakeLong(&RxBuffer[0]); 	/* specifies the starting memory location */
	DWORD Length = MakeLong(&RxBuffer[4]);  		/* get total length of target memory */ 
	DWORD Seed = MakeLong(&RxBuffer[8]);  			/* get start Seed value */ 
	
	/* calculate Seed value */
	while(Length--)
		Seed += *address++;
	/* build reply message */	
	TxBuffer[0]=MR_MEMORY_SIGNATURE;
	LoadLong(&TxBuffer[1],Seed);
	Tx_Mesg_Length = 5;  
} 
BYTE mc_memory_general()
{		
	BYTE *s = (BYTE *)MakeLong(&RxBuffer[1]); 		/* specifies the source memory location */
	BYTE *d = (BYTE *)MakeLong(&RxBuffer[5]); 		/* specifies the destination memory location */
	DWORD len = MakeLong(&RxBuffer[9]);  			/* get total length of memory to process */ 

	Tx_Mesg_Length = 2;
	TxBuffer[0]=MR_MEMORY_GENERAL;
	
	if(RxBuffer[0] == GMC_COPY)
	{
		if(s >= d)
		{  
			while(len--)
				*d++ = *s++;
		}
		else
		{
			d += len-1;
			s += len-1;
			while (len--)
				*d-- = *s--;     
		}   
	}
	else if (RxBuffer[0] == GMC_COMPARE)
	{
		while(len--)
			if(*d++ != *s++) return ERCD_NOTFOUND;
	}   
	else
		return ERCD_PARM;
		
	return ERCD_OK;	  
}

void mc_breakpoint_setting()
{
	TxBuffer[0]=MR_BREAKPOINT_SETTING;
	Tx_Mesg_Length = 2;
	switch (RxBuffer[0])
	{
		case BREAK_ENABLE:
			TxBuffer[1]=BPset(MakeLong(&RxBuffer[1]), MakeShort(&RxBuffer[5]));
			break;
		case BREAK_DISABLE: 
			TxBuffer[1]=BPremove(MakeLong(&RxBuffer[1]));
			break;    
		case BREAK_QUERY:  
/*			TxBuffer[1]=BreakPointExists(MakeLong(&RxBuffer[1]));*/	
			if (BreakPointExists(MakeLong(&RxBuffer[1])))
				TxBuffer[1] = ERCD_OK;
			else
				TxBuffer[1] = ERCD_NOTFOUND;
			break;
		default:
			TxBuffer[1]=ERCD_PARM;
			break;
	}
}

/* Followings are new function for I/O read/write */
void mc_io_read()
{
	BYTE size = RxBuffer[4];
	DWORD address = MakeLong((BYTE *)&RxBuffer[0]);
	BYTE bTemp;
	WORD wTemp;
	DWORD dwTemp;

	/* send acknowledge */

	switch(size){
		case 1:
				bTemp = *(BYTE *)address;
				TxBuffer[2] = bTemp;
				Tx_Mesg_Length = 3;
				break;
		case 2:
				wTemp = *(WORD *)address;
				LoadShort(&TxBuffer[2], wTemp);
				Tx_Mesg_Length = 4;
				break;
		case 4:
				dwTemp = *(DWORD *)address;
				LoadLong(&TxBuffer[2], dwTemp);
				Tx_Mesg_Length = 6;
				break;
		default:
				break;
	}
	TxBuffer[0] = MR_IO_RESULT;
	TxBuffer[1] = size;
	OutMsgHndl();
}

void mc_io_write()
{
	DWORD data = MakeLong((BYTE *)&RxBuffer[5]);
	BYTE size = RxBuffer[4];
	DWORD * pdwTemp;
	WORD * pwTemp;
	BYTE * pbTemp;

	switch(size){
		case 1:
				pbTemp = (BYTE *)MakeLong((BYTE *)&RxBuffer[0]);
				(*pbTemp) = (BYTE)(data>>24);
				break;
		case 2:
				pwTemp = (WORD *)MakeLong((BYTE *)&RxBuffer[0]);
				(*pwTemp) = (WORD)(data>>16);
				break;
		case 4:
				pdwTemp = (DWORD *)MakeLong((BYTE *)&RxBuffer[0]);
				*pdwTemp = data;
				break;
		default:
				break;
	}

	/* send acknowledge */
	Tx_Mesg_Length = 1;
	TxBuffer[0]=MR_COMPLETE;
	OutMsgHndl();
}
	
					                                                                                       	                                                                                             	                                                                    






                   	                                                                                              
