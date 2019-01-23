/// extern.h

/* ported 7/11/96 DS no mods. */
/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL -- No mods

#include "mon_type.h"  

extern int Tx_Mesg_Length; 
extern BYTE MsgInBuffer[];   
extern BYTE MsgOutBuffer[];   

extern BYTE *TxBuffer; 
extern BYTE *RxBuffer;        
 
extern BYTE Rx_Error;  
extern char global_char;
extern REGS 	regs;      


