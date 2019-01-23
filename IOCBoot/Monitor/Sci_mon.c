/// Sci_mon.c

/**********************************************************************************************************************
** SCI_MON.C - Serial Communications Interface & Low-Level Protocol Driver Module									 **
**																													 **
** Author: HMSE																										 **
**																													 **
**																													 **
**	=================================================================================================================**
**	Version	Date		Description																					 **
**	=================================================================================================================**
**	1.00		27/11/95	First version																			 **  
** 1.10		11/12/95 Modified to handle modes of verification														 **
**	=================================================================================================================**
**	ported 7/11/96 DS no mods.                                                                                       **   
**	=================================================================================================================**
**  H8S monitor																		                                 **
**	11/12/97 1.0		EJ 		Stenkil Software                                                                     **
**                              #include <inh8.h> added                                                              **    
**                              H8S/2655 EVB acually uses SCI channel 0                                              **
**                              #ifdef H8S2655 and #ifndef H8S2655 added. See code.                                  **
**																													 **
**********************************************************************************************************************/

/* Modification History:

07-Jan-98 FJL Re-wrote TxMessage function to use pointers to fix array access
              bug. Further comments in code.
08-Jan-98 FJL Modified StoreChar() to not use array access off a pointer
05-Mar-99 DTH Ported to 2623
// Ported to 2633 1/10/01  DGL
*/
   
#include "extern.h"
#include "sci_mon.h"

#pragma section MONITOR 

#ifdef H8S2623
	int mon_sci_init;
#endif
	
static DWORD tx_buff_len,
				 rx_buff_len;
                           

static VERIFICATION verify;
static BYTE verify_error;
static DWORD i;							/* Index into receive buffer & last written address */

void SetVerify(VERIFICATION mode)
{
	verify = mode;
}


DWORD LastAddress(void)
{
	return i;
}


/* Fetch one byte from SCI or return suitable error code */	  	  
BYTE RxChar(BYTE *data_char)
{
   BYTE ercd;
   WORD timer;
   

	ercd = SCIE_OK;
	timer = 0;	
	do {
		if (SCI1_SSR & (SSR_ORER | SSR_FER | SSR_PER))   	/* Check for errors */
			ercd = SCIE_RX;			
		else if (timer >= TIME_OUT)
			ercd = SCIE_TMOT;
		timer ++;
	} while(! (SCI1_SSR & SSR_RDRF) && ercd == SCIE_OK);		/* Wait for data or error */

	if (ercd == SCIE_OK)
		*data_char = SCI1_RDR;									/* Load received data into given buffer */
	else
		SCI1_SSR &= ~ (SSR_ORER | SSR_FER  | SSR_PER);		/* Clear error bits */                                          

	SCI1_SSR &= ~ SSR_RDRF;	      								/* Clear RDRF flag */
	return ercd;
}


void ClearRx(void)
{   
	while ((SCI1_SSR & (SSR_ORER | SSR_FER  | SSR_PER)) || (SCI1_SSR & SSR_RDRF)) {
		SCI1_SSR &= ~ SSR_RDRF;	      								/* Clear RDRF flag */                                        
		SCI1_SSR &= ~ (SSR_ORER | SSR_FER  | SSR_PER);		/* Clear error bits */                                          
	}
		
	
}


void TxByte(BYTE data_char)
{              	
	while(! (SCI1_SSR & SSR_TDRE));  	/* Wait until TDRE = 1  */
	SCI1_TDR = data_char;   				/* Set transmit data    */
	SCI1_SSR &= ~ SSR_TDRE;               	/* Clear TDRE bit       */
}
     


/* Store character in given buffer and calculate current CRC total */
void StoreChar(BYTE *buffer, DWORD *i, WORD *crc, BYTE *cid, BYTE data_char)
{ 
int r;

    r = 0;
	if (*cid == SCIE_NODATA)
		*cid = data_char;		
	else if (*i < rx_buff_len) 
	{					/* If buffer length not exceeded then write data */
	unsigned char *target = (unsigned char *)buffer + *i;
	
		switch(verify)
		{
		case WRITE:
			*target = (char)data_char; 
			break;

		case VERIFY: 
			verify_error += (*target ^ (char)data_char);
			break;        

		case VERIFY_WRITE:
			buffer[*i] = (char)data_char; 
			verify_error += (*target ^ (char)data_char);
		}
		(*i) ++;
	}
	else 
		(*i) ++;
		
	(*crc) += data_char;
}
                        
int download_failure;
char Writable (SH_REG);

// Added by Zheng Miao
extern unsigned long g_monFlag1;
extern unsigned long g_monFlag2;
extern unsigned int  g_quitCnt;

/* Receive a message from the SCI sent in the correct format */	                            	                            	                            	                            
BYTE RxMessage(BYTE *buffer)
{                          


	BYTE 	state,
			err,
			data_char, 
		  	crc_data,
		  	command_id; 
	WORD 	crc_sum;      
        
	state = RXS_START;
	err = SCIE_OK;
  	command_id = SCIE_NODATA; 
	crc_sum = 0;      

	verify_error = 0;
	i = 0;
	while (state != RXS_ERROR && state != RXS_DONE) { 
		err = RxChar(&data_char);
		if ( err != SCIE_OK)
			state = RXS_ERROR;               
		    
		// Added by Zheng Miao (2/13/2003)
		if (state == RXS_START) {
			if ((data_char == 'c') || (data_char == 'C')) {
				if (++g_quitCnt >= 10) {
					g_monFlag1 = 0L;
					g_monFlag2 = 0L;
					g_quitCnt = 0;
				}
				continue;
			} else{
				g_quitCnt = 0;
			} 		                 
		}
		if (state == RXS_START) {
			if (data_char != (BYTE)SOM) {
				state = RXS_ERROR;  
				err = SCIE_NOSOM;
			} else
				state = RXS_SOM1;
				
		} else if (state == RXS_SOM1) {
			if (data_char == (BYTE)SOM)
				state = RXS_SOM2;
 			else if (data_char == (BYTE)EOM) {
				state = RXS_ERROR;
				err = SCIE_NODATA;
			} else {
				state = RXS_DATA;
				StoreChar(buffer, &i, &crc_sum, &command_id, data_char);
			}  
			
		} else if (state == RXS_DATA) {
			if (data_char == (BYTE)SOM)
				state = RXS_SOM2;
			else if (data_char == (BYTE)EOM)
				state = RXS_EOM;
			else
				StoreChar(buffer, &i, &crc_sum, &command_id, data_char);

		} else if (state == RXS_SOM2) {
			if (data_char == (BYTE)~SOM) {
				state = RXS_DATA;
				StoreChar(buffer, &i, &crc_sum, &command_id, SOM);
			} else {
				state = RXS_ERROR;
				err = SCIE_UNEXSOM;				
			}
		} else if (state == RXS_EOM) {
			if (data_char == (BYTE)~EOM) {
				state = RXS_DATA;
				StoreChar(buffer, &i, &crc_sum, &command_id, EOM);
			} else {
				crc_sum |= 0x8000;										/* MSB is always set */
				if (RxChar(&crc_data) == SCIE_OK) {
					if ((data_char << 8) + crc_data == crc_sum)
						state = RXS_DONE;
					else {
						state = RXS_ERROR;
						err = SCIE_CRC;
					}
				} else {
					state = RXS_ERROR;
					err = SCIE_RX;
				}
			}
		}            
		
		if (i > rx_buff_len) {
			state = RXS_ERROR;
			err = SCIE_OVERFLOW;
		} else if (verify_error > 0) {
			state = RXS_ERROR;
			err = SCIE_VERIFY; 	
			i = (DWORD)&buffer[i - 1];
		}
	}  
                           
	download_failure = 0;
/* DS 22/5/98 removed this feature for Wayne Lyons to allow "write" to ROM */
/*
	if (command_id == MC_DOWNLOAD && !Writable(&(SH_REG)buffer) )
		download_failure = 1;	     
*/
	if (state == RXS_DONE)
		return command_id;
	else 
		return err;
}
 
void TxMessage(BYTE *buffer, DWORD length)
{                                 
	WORD crc_sum;
	DWORD i;
	char currentChar;

	crc_sum = 0;
	ClearRx();	
								/* OK. Go ahead */
	TxByte(SOM);  

	while (length--)
	{
	unsigned char c;
		TxByte (c = *buffer++);
		if (c == SOM || c == EOM)
			TxByte (~c);	
		crc_sum += c;
	}
				                    
	TxByte(EOM);		                  		/* Send EOM */
	crc_sum |= 0x8000; 					/* Set MSB so CRC cannot equal the EOM marker */
	TxByte(crc_sum>>8);			/* Send CRC bytes */
	TxByte(crc_sum & 0x00FF);      
	while(! (SCI1_SSR & SSR_TEND))
		;   			/* Wait until TEND = 1  */
}	


int InitSCI(unsigned int baud, char parity, char stop, char length)
{
  	int 	i, 
     		fun_error;

  	i = INTERVAL; 
    fun_error = SCIE_OK;
#ifdef H8S2623
     /*--- Temporary fix! When this function is called a second time i.e. ---
       --- when link up is made, it seems to spew out a NULL into the Tx  ---
       --- register or something. This screws up the connection and HDI   ---
       --- crashes, so this should only be initialised once.              ---*/
     if (mon_sci_init < 1)       // Just left it on the 2633
     {
     	mon_sci_init = 1;
#endif
     			
  	/* DS: the initialisation procedure seems to be the same for
  	SH and 3003 */

  	/* EJ: H8S/2655 EVB acually uses SCI channel 0 */
	
  	/* Clear TE & RE bits to 0 in SCR */
  	SCI1_SCR = 0;
	SCI1_SSR &= ~ (SSR_PER + SSR_FER + SSR_ORER);          /* Clear Rx error bits */
	                                                              
  	/* Set communications format */
   SCI1_SMR = 0;									/* Clear all flags to zero.  Following code will set required bits */
   	
   if (parity == P_EVEN)						/* Set parity */
   	SCI1_SMR |= SMR_PE;
   if (parity == P_ODD)
     	SCI1_SMR |= SMR_PE + SMR_OE;
   else if (parity != P_NONE)
   	fun_error = SCIE_PRM;

   if (stop == 2)           				 	/* Set stop bits */
     	SCI1_SMR |= SMR_STOP;
   else if (stop != 1)
   	fun_error = SCIE_PRM;
  	
   if (length == 7)					 			/* Set character length */
   	SCI1_SMR |= SMR_CHR;
   else if (length != 8)
   	fun_error = SCIE_PRM;

   SCI1_BRR = baud;								/* Select baud rate value */
   
   while(i --);               				/* Wait at least interval of sending one bit */

   SCI1_SCR |= SCR_TE;                  	/* Enable transmission  */       
   SCI1_SCR |= SCR_RE;							/* Enable reception */       

#ifdef H8S2623
    }
#endif
     
	SYSCR &= 0x1;

	/*--- Set the interrupt priorities in case the user code ---*/
	/*--- changes the SYSCR to mode 2. We still want SCI     ---*/
	/*--- interrupts to be recieved over all else.           ---*/
	/*--- Set SCI2 interrupts as the highest priority (7)    ---*/
#ifdef	MON_SCI0
	IPRJ = 0x17;
	IPRK = 0x11;
#else
	IPRJ = 0x11;
	IPRK = 0x17;
#endif

	/*--- Set all other interrupts to low priority (1)       ---*/
	/*--- This is done in case the user code selects the     ---*/
	/*--- interrupt control mode 2 (using EXR and priorities ---*/
	IPRA = 0x11;
	IPRB = 0x11;	
	IPRC = 0x11;
	IPRE = 0x11;
	IPRF = 0x11;
	IPRG = 0x11;
	IPRH = 0x11;
	IPRI = 0x11;

   return (fun_error);
}
        
#pragma inline(Set_MaxRxBuffer_Size)
void Set_MaxRxBuffer_Size(DWORD MaxRxBuffer)
{
   rx_buff_len = MaxRxBuffer;
}

#pragma inline(Set_MaxTxBuffer_Size)
void Set_MaxTxBuffer_Size(DWORD MaxTxBuffer)
{
   tx_buff_len = MaxTxBuffer;
}

 
#pragma inline(RxInterruptsOn)
void RxInterruptsOn(void)
{                             
	SCI1_SCR |= SCR_RIE;
}
 
                              
#pragma inline(RxInterruptsOff)
void RxInterruptsOff(void)
{                      
	SCI1_SCR &= ~SCR_RIE;
} 
