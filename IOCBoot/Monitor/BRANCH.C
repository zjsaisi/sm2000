/// Branch.c

/********************************************/
/* file created 4th November 1996           */
/* D. Sananikone                            */
/* 18/02/97 Bug fix for JMP @Rn instruction */
/********************************************/

#pragma section MONITOR 
    
#ifdef FUNCTIONALITY_TEST
#include <stdio.h>
#endif    
#include "mon_type.h"                       
#include "instleng.h"

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL   - No mods

DWORD GetRegister (BYTE);

signed long
UStoSL (unsigned short instruction)
{
	/* function to convert from unsigned short to signed long */

	if (instruction < 0x8000)
		return (unsigned long)instruction;
	return (0xffff0000 | instruction);
}                 

signed long
UBtoSL (unsigned short instruction)
{
	/* function to convert unsigned byte to signed long.
		Byte contained in LSB */
		
	if (instruction < 0x80)
		return (unsigned long)instruction;
	return (0xffffff00 | instruction);
}
		                                             

int
IsBranch (unsigned short instruction)
{
	/* function to determine whether instruction is branch.
		return TRUE if branch */
	
	instruction = instruction & 0xff00;
		/* set mask to ignore second byte */
	if (instruction == 0x5800)     
		/* branch with 16 bit displacement */
		return TRUE;
	instruction = instruction & 0xf000;
		/* set mask to ignore latter 12 bits */
	if (instruction == 0x4000)      
		/* branch with 8 bit displacement */
		return TRUE;
	return FALSE;
}

int 
IsJmp (unsigned short instruction)
{
	/* function to determine whether instruction is a jump:
		JMP, BSR, JSR.
		returns true if instruction is one of the above 
		( must look for instructions with first byte equal to 
		  55, 59, 5A, 5B, 5C, 5D, 5E, or 5F )*/
		
	unsigned short inst;

	inst = instruction;	
	instruction = instruction & 0xf000;
		/* set bit mask to ignore latter 12 bits */
	if (instruction != 0x5000)
		return FALSE;
	instruction = inst & 0xff00;
		/* set bit mask to ignore second byte */
	if (
		instruction <= 0x5400 ||
		instruction == 0x5600 ||
		instruction == 0x5700 ||
		instruction == 0x5800
		)
	return FALSE;
	return TRUE;
}
					 
short
get_displacement_of_branch (unsigned long address)
{
	/* function to return displacement of branch instruction */
                                                  
    unsigned short instruction = *(unsigned short *)address;
	unsigned short inst = instruction;
		/* make a copy of instruction */

	instruction = instruction & 0xff00;
		/* set mask to ignore second byte */
	if (instruction == 0x5800)
	{
		/* 16 bit displacement contained in second word */
		#ifdef FUNCTIONALITY_TEST  
		extern int second_word_read;
		extern unsigned short get_new_instruction ();
		second_word_read = 1;
		instruction = get_new_instruction ();
		return instruction;
		#else	
		address+=2;	                               
		return (*(short *)address);
		#endif
	}
	else
	{
		/* 8 bit displacement contained in second byte */
		inst = inst & 0x00ff;
		if ((inst & 0x0080) > 0x0) /* if negative */
			inst = inst | 0xff00;  /* sign extend */
		return ((short)inst);

	}
}

unsigned long
get_address_of_jump (unsigned long address)

{
	/* function to return jump address of jump instructions:
		JMP, BSR, JSR, RTS, RTE
		JMP 59, 5A, 5B
		BSR 55, 5C
		JSR 5D, 5E, 5F
		RTS 54
		RTE 56 */
		     
	#ifdef FUNCTIONALITY_TEST
	extern FILE *output_file;
	#endif
	unsigned long pc = address;
	unsigned short instruction = *(unsigned short *) address;                          		
	unsigned short inst = instruction;
	unsigned long return_address;


		                     
	instruction = instruction & 0xff00;
	
		/* set bit mask to ignore second byte */	
	if (
		instruction == 0x5900 ||
		instruction == 0x5d00
		)
	{
		/* selected 59 or 5D which has a register indirect specified
			in bits 2,3, and 4 in the second byte */

		#ifdef FUNCTIONALITY_TEST
		instruction = ((inst >> 4) &  0x000f);
		fprintf (output_file, "Register indirect:R%d selected, contents = %x\n", instruction,
			GetRegister((unsigned char) instruction)); 
		/*
		fprintf (stdout, "Register indirect:R%d selected, contents = %x\n", instruction,
			GetRegister((unsigned char) instruction));*/
		return 0;           
		#else                                 
		instruction = ((inst >> 4) &  0x000f);   
		if(instruction >2)
			instruction -= 2;	
		else
			instruction +=8;
      return (GetRegister((unsigned char) instruction)) ;
		#endif
	}
	
	if (
	 	instruction == 0x5a00 ||
	 	instruction == 0x5e00
	 	)
	{
		/* selected 5A or 5E which contains an absolute address
			contained in bytes 2, 3 and 4 */         
	#ifdef FUNCTIONALITY_TEST                 
	extern second_word_read;
	second_word_read = 1;
	instruction = get_new_instruction ();
	return_address = inst;
	return_address = ((return_address & 0x00ff) << 16);
	fprintf (output_file, "absolute address: %lx\n", return_address | instruction);
	return 0;
	#else
	address+=2;
	instruction = *(unsigned short *) address;         
	return_address = inst;
	return_address = ((return_address & 0x00ff) << 16);
	return (return_address | instruction);
	#endif 
 			  
	}
	
	if (
		instruction == 0x5b00 ||
		instruction == 0x5f00
		)
	{
		/* selected 5B or 5F which contains a memory indirect contained
			in the second byte */ 
	instruction = (inst & 0x00ff);
	#ifdef FUNCTIONALITY_TEST
	fprintf (output_file, "memory indirect: %x\n", instruction);
	return 0;
	#else
	return (* (unsigned long *)instruction);
	#endif
	}                                       
	                        
	if (
		instruction == 0x5500 ||
		instruction == 0x5c00
		)
	{
	 	/* selected 55 or 5C which contains displacement:
	 	if 55 disp. contained in second byte
	 	if 5C disp contained in third and fourth bytes */
	 	                       
		pc = address + get_instruction_length (pc);	 	                       
		/* we must add the length of the current instruction onto the pc 
		because the pc points to the current instruction and not the next instruction */
			 	                       
	 	if (instruction == 0x5500)
	 	{ 
	 		#ifdef FUNCTIONALITY_TEST	
	 		fprintf (output_file, "8-bit disp.:%x\n", (inst & 0xff)); 
	 		return 0;
	 		#else
	 		unsigned long result, Pc, Inst;
	 		Inst = UBtoSL (inst & 0xff);
	 		Pc = pc;
	 		result = Pc + Inst;
	 		return result;
	 		#endif
	 	}
	 	else
	 	{   
	 		#ifdef FUNCTIONALITY_TEST            
	 		extern int second_word_read;
	 		second_word_read = 1;
	 		instruction = get_new_instruction ();
	 		displacement = (inst & 0xff); 
	 		displacement = (displacement << 16);
	 		fprintf (output_file,"24-bit disp.: %lx\n", (displacement | (unsigned long)instruction));	 		
	 		return 0;
	 		#else         
	 			/* the following is a bit strange as I'm trying to convert from unsigned shorts
	 				to signed longs and add them */              
			signed long Inst, Pc= pc, result;
	 		address+=2;
	 		instruction = *(unsigned short *) address;      	 		      
			Inst = UStoSL(instruction);     
			result = Pc + Inst;
	 		return result;
	 		#endif
	 	}
	 	
	}		                        
	if (instruction == 0x5400)
	{
		/* selected RTS instruction
			The address is on the stack */ 
		#ifdef FUNCTIONALITY_TEST
		fprintf (output_file, "RTS instruction\n");
		return 0;
		#else 
		return ((* (unsigned long *)GetRegister(REG_SP)) & 0x00ffffff);
			/* do we need to mask the top 4 bits? */
		#endif
	}
	if (instruction == 0x5600)
	{
		/* selected RTE instruction 
			The address (24-bit) is on the stack */
		#ifdef FUNCTIONALITY_TEST
		fprintf (output_file, "RTE instruction\n");
		return 0;
		#else
		return ((* (unsigned long *)GetRegister(REG_SP)) & 0x00ffffff);
			/* the top byte is actually the CCR, thus we remove that
				to get the PC address */		                      
		#endif       
	}
}

                                                            

                                                             
		
	
		
			

		
	
	
