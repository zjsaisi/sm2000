/// Break.c

/* ported 7/11/96 DS no mods. */
/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL - No mods
 
#pragma section MONITOR 

#include "bptable.h"
#include "break.h"

#define NULL 0

char BreakPointExists (SH_REG);

void BPinitialise(void)
{
	InitBreakTable();
}

	

/* Check if the given address is in RAM */                                                                                                                                                                                                                

/* DS: used temporary variable as high optimisation destroyed
	the context */

char Writable(SH_REG address)
{
	SH_INST temp;  
	SH_REG a1 =address;
	
	temp = *(SH_INST *) address;								/* Store the memory word */
	* (SH_INST *) address = ~(*(SH_INST *) address);		/* Invert the memory word */
   
	if (*(SH_INST *) a1 == temp)						/* Is it inverted? */
		return FALSE;  /* No --- address is in ROM */	
	else   
	{
		*(SH_INST *) address = temp;							
		return TRUE;                     
	}    



}



/* Setup a breakpoint at the given address */
ERCD BPset(SH_REG address, WORD count)
{                                                 
	char loc;
	ERCD err;
	
	if (address & 1 == 1)
		return ERCD_ALIGN;
	else if (Writable(address) || *(SH_INST*)address == TRAPA) {      
			err = AddBreakPoint(address, *(SH_INST*)address, count);
			if (err != ERCD_OK)
				return err;
			else {			
				/*^^ *(SH_INST*)address = TRAPA; removed as not needed with enable/disable commands - NJH */
				return ERCD_OK;
			}
	} else
		return ERCD_NOWRITE;
}          



/* Remove a breakpoint from the given address */
ERCD BPremove(SH_REG address)
{                                         
	SH_INST inst;
	WORD count, cycles;
	
	if (GetBreakPoint(address, &inst, &count, &cycles) == ERCD_OK) {
/*^^		*(SH_INST *)address = inst;  NJH, removed as not needed with enable/disable commands */
		return RemoveBreakPoint(address);
	} else
		return ERCD_NOTFOUND;
}
		

ERCD ConditionsOK(SH_REG address)
{                                         
	SH_INST inst;
	WORD count, cycles;
		
	if (GetBreakPoint(address, &inst, &count, &cycles) == ERCD_OK)
		return cycles >= count;
	else
		return FALSE;
}


/* This routine will restore the instruction at the location of a breakpoint */		
ERCD BPrestoreInst(SH_REG address)
{
	SH_INST inst;
	WORD count, cycles;
	
	if (GetBreakPoint(address, &inst, &count, &cycles) == ERCD_OK) {
		*(SH_INST *)address = inst;                        
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}



/* This routine will restore the trap instruction at the location of a breakpoint */
ERCD BPrestoreTrap(SH_REG address)
{
	if (BreakPointExists(address)) {   
		*(SH_INST *)address = TRAPA; 
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}


/* This routine will remove all break points */
ERCD BPremoveAll(void)
{      
	BYTE i, err, BPcount;
	SH_REG address;
	WORD null_data;
	
	BPcount = NumberOfBP();
	
	for (i = 1; i <= BPcount; i ++)
		if((err = GetBreakPointByIndex(1, &address, &null_data, &null_data, &null_data)) == ERCD_OK) {
			if((err = BPremove(address)) != ERCD_OK)
				return err;
		} else
			return err;	
	return ERCD_OK;
}				
                                   

/* These two function are used to enable breakpoints before execution, and to disable them afterwards.  It is important
that enable is called before disable, and that disable is called before enable (except initially).
*/
ERCD BPenable(void)
{                    
	WORD null_data;
	SH_REG address;
	BYTE bp, err;
   extern ERCD BPsetInstruction(SH_REG, SH_INST);
		
	for (bp = 1; bp <= NumberOfBP(); bp ++)
		if((err = GetBreakPointByIndex(bp, &address, &null_data, &null_data, &null_data)) == ERCD_OK) {
			if((err = BPsetInstruction(address, *(SH_INST*)address)) != ERCD_OK)
				return err;
			*(SH_INST*)address = TRAPA;
		} else
			return err;	
}
 

ERCD BPdisable(void)
{
	WORD null_data;
	SH_REG address;
	BYTE bp, err;
	
	/* The robustness of this can be further increased by checking if TRAPA's have been overwritten during execution.  If
	they have, the new data is used to replace the instruction stored in the BP table */
		
	for (bp = 1; bp <= NumberOfBP(); bp ++)
		if((err = GetBreakPointByIndex(bp, &address, &null_data, &null_data, &null_data)) == ERCD_OK) {
			if((err = BPrestoreInst(address)) != ERCD_OK)
				return err;
		} else
			return err;	
}
