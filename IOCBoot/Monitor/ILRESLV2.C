/// Ilreslv2.c

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL -- No mods

#include "mon_type.h"   

#pragma section MONITOR 

int
ILResolve_2 (SH_REG address)
{                      
	/* function to resolve instruction length
		for those instructions with opcodes starting
		with 5 */

	SH_INST instruction = *(SH_INST *) address;      
	
	instruction = instruction & 0xff00; 
		/* set mask to ignore second byte */
	
	if (
		instruction == 0x5800 || 
		instruction == 0x5a00 ||
		instruction == 0x5c00 ||
		instruction == 0x5e00
		)
		return 2;
	return 1;
}

	
