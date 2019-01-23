/// Ilreslv3.c

/*************************************************************************************
**  H8S monitor																		**
**	11/12/97 1.0		EJ 		Stenkil Software                                    **
**                              added for support for new H8S instructions          **
**																					**
*************************************************************************************/

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL -- No mods

#include "mon_type.h"

#pragma section MONITOR 

int
ILResolve_3 (SH_REG address)
{

	/* function to resolve instruction length
	for those instructions with opcodes starting
	with 6 */
         
	SH_INST instruction = *(SH_INST *) address;  
	
	instruction = instruction & 0xfff0;
		/* set mask to ignore last four bits */
   if (
   	instruction == 0x6a00 ||
   	instruction == 0x6a80 ||
   	instruction == 0x6b00 ||
   	instruction == 0x6b80
   	)
   	return 2;
   if (
   	instruction == 0x6a10 ||    /* added for H8S support, Stenkil Software */
   	instruction == 0x6a20 ||
   	instruction == 0x6aa0 ||
   	instruction == 0x6b20 ||
	instruction == 0x6ba0
   	)
   	return 3;
   if (                         /* added for H8S support, Stenkil Software */
   	instruction == 0x6a30       
   	)
   	return 4;
       instruction = instruction & 0xff00;
   	/* set mask to ignore second byte */
   if (
   	instruction == 0x6e00 ||
   	instruction == 0x6f00
   	)
   	return 2;
   return 1;
}
    
