/// Instleng.c

/* **************************************

   file created on 28th October 1996
   Author: Dang Sananikone
           HMSE                           
           
*************************************** */

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01  DGL  -- No mods

#include "mon_type.h"
#include "instleng.h"   

#pragma section MONITOR                      

int
get_instruction_length (SH_REG address)
{
	/* function to resolve the length of 
	a 300H instruction and to return the word 
	length as an integer.
	input: address of start of instruction
	        (type SH_REG typedef'ed as unsigned long)
	output: integer word length of instruction 
	*/
	

	int length = 1; 
	 
	SH_INST instruction = *(SH_INST *) address;
		/* instruction contains instruction opcode, length 2 bytes */
                                                    
	instruction = instruction & 0xff00;
		/* set mask to ignore second byte */
 
	if (instruction == 0x0100)                 
		{	
			/* matches first byte 01 */
			#ifdef DEBUG
			printf ("first byte 01: calling ILResolve_1\n");
			#endif
			length = ILResolve_1 (address);
		}
	else
	{    
		instruction = instruction & 0xf000;
			/* set mask to ignore latter 12 bits */
		if (instruction == 0x5000)		
		{ 
			#ifdef DEBUG 
			printf ("first byte 5X: calling ILResolve_2\n");
			#endif
			length = ILResolve_2 (address);  
				/* match ops starting with 5 */
		}
		else if (instruction == 0x6000)
		{
			#ifdef DEBUG
			printf ("first byte 6X: calling ILResolve_3\n");
			#endif
			length = ILResolve_3 (address);
				/* matches ops starting with 6 */
		}
		else if (instruction == 0x7000) 
		{   
			#ifdef DEBUG                                
			printf ("first byte 7X: calling ILResolve_4\n");
			#endif
			length = ILResolve_4 (address);    
				/* matches ops starting with 7 */
		}
	}	
	return length*2; /* byte length */
} 
