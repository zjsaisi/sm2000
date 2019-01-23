/// Ilreslv1.c

/*************************************************************************************
**  H8S monitor																		**
**	11/12/97 1.0		EJ 		Stenkil Software                                    **
**                              added for support for new H8S instructions          **
**																					**
*************************************************************************************/

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL	-- No mods

#include "mon_type.h"  
#include "instrctn.h"  

#pragma section MONITOR 

int
ILResolve_1a (SH_REG address)
{

	/* function to resolve instructions with opcodes
	starting 0100 or 0140. The second word must be 
	read in order to resolve the length of the instruction */
	
	SH_INST instruction;
    int length = 2;
        
	#ifdef FUNCTIONALITY_TEST 
	extern int second_word_read;
	extern unsigned short get_new_instruction ();
	second_word_read = 1;
	instruction = get_new_instruction ();
	#else
	address+=2;	
	instruction = *(SH_INST *)address;
	#endif
		/* instruction now contains second word */
		
	instruction = instruction & 0xfff0;
		/* set bit mask to ignore last four bits */
		
	if (
		instruction == 0x6ba0 ||
		instruction == 0x6b20
		)
		length = 4;
 	else if (
 		instruction == 0x6b00 ||
 		instruction == 0x6b80 
 		)
 		length = 3;                  
	instruction = instruction & 0xff00;
		/* set bit mask to ignore second byte */
	if (instruction == 0x7800)
		length = 5;
	else if (instruction == 0x6f00)
		length = 3;
	else if (
		instruction == 0x6900 ||
		instruction == 0x6d00
		)
		length = 2;
	return length;
}
int
ILResolve_1 (SH_REG address)
{

	/* function to resolve instruction length
	for those instructions with opcodes starting
	with 01 */
	

         
	SH_INST instruction = *(SH_INST *) address;
	
	if (
		instruction == 0x0100 ||
		instruction == 0x0140 ||
		instruction == 0x0141       /* added for H8S support, Stenkil Software */
		)
		return ILResolve_1a (address);
	instruction = instruction & 0xfff0;
		/* set bit mask to ignore last four bits */
	if (
		instruction == 0x0110 ||    /* added for H8S support, Stenkil Software */
		instruction == 0x0120 ||    /* added for H8S support, Stenkil Software */
		instruction == 0x0130 ||    /* added for H8S support, Stenkil Software */
		instruction == 0x0160 ||    /* added for H8S support, Stenkil Software */
		instruction == 0x01c0 ||
		instruction == 0x01d0 ||
		instruction == 0x01e0 ||    /* added for H8S support, Stenkil Software */
		instruction == 0x01f0 
		)
		return 2;
	return 1;
}


		 
	
	
	      
