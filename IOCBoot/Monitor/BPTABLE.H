/// bptable.h

/* ported 7/11/96 DS no mods. */
/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL -- No mods
	
#include "mon_type.h"
                       

typedef struct bp_entry {
	SH_INST 	instruction;	/* Instruction replaced by the BP TRAPA instruction */
	SH_REG	address;    	/* Address of the BP */
	WORD     count;   		/* Count that the BP is to stop on */
	WORD		occurences;		/* Number of times the BP has been encountered */
} BP_ENTRY;


/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is not in table
		2) Maximum number of breakpoints has not been reached
*/	
ERCD AddBreakPoint(SH_REG address, SH_INST instruction, WORD count);


/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Removes a break point from the table */
ERCD RemoveBreakPoint(SH_REG address);
		

/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Gets the details of the given BP */
ERCD GetBreakPoint(SH_REG address, SH_INST *instruction, WORD *count, WORD *bp_count);


/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Index 'i' is between 1 and the number of break points 
Gets the details of the given BP index number */
ERCD GetBreakPointByIndex(BYTE i, SH_REG *address, SH_INST *instruction, WORD *count, WORD *occurences);

/* No pre-conditions.  Use to initialise and clear all break points */
void InitBreakTable(void);


/* Unchecked Pre: BP table has been initialised.
	Checked-Pre: 
		1) Address is in table.
Increments the BP execution count or resets it */
ERCD BPincrementCount(SH_REG address);


/* Unchecked pre: Table has been initialised 
Return number of current break points */				
BYTE NumberOfBP(void);

/* No pre-cons.  Return maximum number of BPs */
BYTE MaxNumberOfBP(void);

/* Reset occuence counters of all BP's */                                    
ERCD BPresetCounts(void);

