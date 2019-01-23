/// Bptable.c

/* ported 7/11/96  DS no mods. */

/* 16th december 1996
		Modified function BPresetCounts:
			count will only be reset if occurences >= count

	11/12/97 1.0		EJ 		Stenkil Software
    ported to H8S/2655 
    #ifdef H8S2655 added
    static declaration deleted - problem in H8S compiler

*/

/* Ported to H8S2623 - 01/03/99 (DTH) */

// Ported to 2633 1/10/01 DGL

#pragma section MONITOR 

#include "bptable.h" 

BP_ENTRY bp_table[MAX_BP]; 	/* The table of break points */
BYTE bp_count;			 /* Number of break points in the table */

/* Unchecked Pre: BP table has been initialised.
	Checked Pre: None.
Returns true if given address is a valid break point */
char BreakPointExists(SH_REG address)
{  
	BYTE i;
	
	for (i = 0; i < bp_count && bp_table[i].address != address; i++);
	return bp_table[i].address == address && bp_count > 0 && i < bp_count;
}



/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is not in table
		2) Maximum number of breakpoints has not been reached
*/	
ERCD AddBreakPoint(SH_REG address, SH_INST instruction, WORD count)
{              
	if (bp_count < MAX_BP)
		if (! BreakPointExists(address)) {
			bp_table[bp_count].instruction = instruction;
			bp_table[bp_count].address = address;
			bp_table[bp_count].count = count;
			bp_table[bp_count].occurences = 0;
			bp_count ++;
			return ERCD_OK;		             		
		} else
			return ERCD_BPEXISTS;
	else
		return ERCD_BPFULL;
}



/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Removes a break point from the table */
ERCD RemoveBreakPoint(SH_REG address)
{
	BYTE i, j;
	
	for (i = 0; i < bp_count && bp_table[i].address != address; i ++);
	if (bp_table[i].address == address && bp_count > 0) {
		for (j = i; j < bp_count - 1; j ++)
			bp_table[j] = bp_table[j + 1];
		bp_count --;
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}
	
		

/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Gets the details of the given BP */
ERCD GetBreakPoint(SH_REG address, SH_INST *instruction, WORD *count, WORD *occurences)
{                                                                                   
	BYTE i;
	
	for (i = 0; i < bp_count && bp_table[i].address != address; i ++);
	if (bp_table[i].address == address) {
		*count = bp_table[i].count;	
		*occurences = bp_table[i].occurences;	
		*instruction = bp_table[i].instruction;	
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}


/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Index 'i' is between 1 and the number of break points 
Gets the details of the given BP index number */
ERCD GetBreakPointByIndex(BYTE i, SH_REG *address, SH_INST *instruction, WORD *count, WORD *occurences)
{                                                                                  
	if (i > 0 && i <= bp_count) { 
		i --;                  			/* Adjust BP number for array index */
		*count = bp_table[i].count;	
		*occurences = bp_table[i].occurences;	
		*instruction = bp_table[i].instruction;	
		*address = bp_table[i].address;		
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}

/* No pre-conditions.  Use to initialise and clear all break points */
void InitBreakTable(void)
{
	bp_count = 0;
}                   



/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Increments the BP execution count */
ERCD BPincrementCount(SH_REG address)
{                                                   
	BYTE i;
	
	for (i = 0; i < bp_count && bp_table[i].address != address; i ++);
	if (bp_table[i].address == address) {
			bp_table[i].occurences ++;	
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}


ERCD BPsetInstruction(SH_REG address, SH_INST instruction)
{                                                   
	BYTE i;
	
	for (i = 0; i < bp_count && bp_table[i].address != address; i ++);
	if (bp_table[i].address == address) {
			bp_table[i].instruction = instruction;	
		return ERCD_OK;
	} else
		return ERCD_NOTFOUND;
}


/* Reset the occurence count of every BP */
ERCD BPresetCounts(void)
{      
	BYTE i;
	for (i = 0;i < bp_count;i++)
		if (bp_table[i].occurences >=bp_table[i].count)
			bp_table[i].occurences = 0;
		
	return ERCD_OK;
}

/* Unchecked pre: Table has been initialised 
Return number of current break points */				
#pragma inline(NumberOfBP)
BYTE NumberOfBP(void)
{
	return bp_count;
}


/* No pre-cons.  Return maximum number of BPs */
#pragma inline(MaxNumberOfBP)
BYTE MaxNumberOfBP(void)
{
	return MAX_BP;
}
