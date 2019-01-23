/// sci_mon.h

/*
**=================================================================================**
**  H8S monitor																       **
**	11/12/97 1.0		EJ 		Stenkil Software                                   **
**	                            20 MHz used                                        **
**	                            #ifdef H8S2655 added                               **
**=================================================================================**
*/

	/* DS: these clock defs set the baud for the BBR */
	/* DS: These look to be the same for SH and3003 */
	/* EJ: These look to be the same for SH and 3003 and H8S (20 MHz used) */
	/* Ported to H8S2623 - 01/03/99 (DTH) */
// Ported to 2633 1/10/01 DGL
	
/*#define CLOCK20*/
/* #define CLOCK16 */
#define CLOCK16

#ifdef CLOCK10
	#define B9600 32
	#define B19200 15
#endif

#ifdef CLOCK15 
	#define B9600 47
	#define B19200 23
	#define B38400 11
	#define B57600  7
	#define B115200 3
#endif

#ifdef CLOCK18
	#define B38400 14
	#define B115200 4
#endif

#ifdef CLOCK16
	#define B9600 51
	#define B19200 25 
	#define B38400	12
	#define B57600  8
	#define B115200 3
#endif
                      
#ifdef CLOCK20
	/*  DS: clock does not go as fast as 20MHz in 3003 */
	#define B9600 64
	#define B19200 32 
	#define B38400	15
	#define B57600	10      /* Added for H8S/2600 */
#endif

#if defined H8S2623
	/***********************************************************/
	/*  Actually using SCI2 for the prototype board.           */
	/*  For the final EVB we have to implement serial drivers  */
	/*  for an external UART so this is irrelevant.            */
	/***********************************************************/
	#include "2623regs.h"

	#define SCI1_SMR   SCI0_SMR
	#define SCI1_BRR   SCI0_BRR
	#define SCI1_SCR   SCI0_SCR
	#define SCI1_TDR   SCI0_TDR
	#define SCI1_SSR   SCI0_SSR
	#define SCI1_RDR   SCI0_RDR
#endif

/* Serial control register bit values */
#define SCR_TIE  0x80
#define SCR_RIE  0x40
#define SCR_TE   0x20
#define SCR_RE   0x10
#define SCR_MPIE 0x8
#define SCR_TEIE 0x4
#define SCR_CKE1 0x2
#define SCR_CKE0 0x1

/* Serial mode register bit values */
#define SMR_CA   0x80 
#define SMR_CHR  0x40
#define SMR_PE   0x20
#define SMR_OE   0x10
#define SMR_STOP 0x8
#define SMR_MP   0x4
#define SMR_CKS1 0x2
#define SMR_CKS0 0x1

/* Serial stats register bit values */
#define SSR_TDRE 0x80
#define SSR_RDRF 0x40
#define SSR_ORER 0x20
#define SSR_FER  0x10
#define SSR_PER  0x8
#define SSR_TEND 0x4
#define SSR_MPB  0x2
#define SSR_MPBT 0x1


/* Parity values used in port initialisation */
#define P_NONE 0
#define P_EVEN 1
#define P_ODD  2

#define INTERVAL 5000			/* At least interval to transmit 1-bit */


#define ERROR_OFFSET 200	   /* Value added to error return codes */

/* Error codes returned by user routines */
#define SCIE_OK  			0 + ERROR_OFFSET     		/* Used internally to signify success */
#define SCIE_PRM 			1 + ERROR_OFFSET   			/* Parameter error.  e.g. Invalid number of stop bits */
#define SCIE_RX 			2 + ERROR_OFFSET				/* Parity, Frame, & overrun errors */
#define SCIE_FULL 		3 + ERROR_OFFSET
#define SCIE_TMOT 		4 + ERROR_OFFSET				/* Timeout.  No data present at receive port */    
#define SCIE_NODATA 		5 + ERROR_OFFSET				/* Given transmit buffer is empty */
#define SCIE_OVERFLOW 	6 + ERROR_OFFSET   			/* Attempt to over fill  buffer */

#define SCIE_UNEXSOM 	7 + ERROR_OFFSET				/* Unexpected SOM character in message */
#define SCIE_CRC 			8 + ERROR_OFFSET				/* CRC error */
#define SCIE_NOSOM 		9 + ERROR_OFFSET				/* Message started without SOM character */      
#define SCIE_VERIFY		10 + ERROR_OFFSET				/* Verification error between received data and target buffer */


#define RXS_START	1
#define RXS_SOM1	2                            
#define RXS_SOM2	3                            
#define RXS_DATA	4                            
#define RXS_ERROR	5                            
#define RXS_EOM   6
#define RXS_DONE	7
	                            
#define SOM 0xAA
#define EOM 0xCC

#define TIME_OUT 50000            
                     
BYTE RxMessage(BYTE *buffer);

void TxMessage(BYTE *buffer, DWORD length);

int InitSCI(unsigned int baud, char parity, char stop, char length);

void Set_MaxRxBuffer_Size(DWORD MaxRxBuffer);

void Set_MaxTxBuffer_Size(DWORD MaxTxBuffer);
 
void SetVerify(VERIFICATION mode);

DWORD LastAddress(void);
                    
void RxInterruptsOn(void);
                              
void RxInterruptsOff(void);
