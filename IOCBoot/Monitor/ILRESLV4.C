/// Ilreslv4.c

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL -- No mods

#include "mon_type.h"   

#pragma section MONITOR 

int
ILResolve_4 (SH_REG address)
{

	/* function to resolve instruction length
	for those instructions with opcodes starting
	with 7 */
         
	SH_INST instruction = *(SH_INST *) address;  
	      
	instruction = instruction & 0xff00;
		/* set mask to ignore second byte */
	
	if (instruction == 0x7800)
		return 4;
	if (instruction == 0x7a00)
		return 3;	
	if (
		instruction == 0x7b00 ||
		instruction == 0x7900 ||
		instruction == 0x7c00 ||
		instruction == 0x7d00 ||
		instruction == 0x7e00 ||
		instruction == 0x7f00
		)
		return 2;
	return 1;
}	
