/*                   
 * Filename: asm.c
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: asm.c 1.1 2007/12/06 11:39:07PST Zheng Miao (zmiao) Exp  $
 */ 

#include "asm.h"
#include "machine.h"
#include "string.h"         
#include "iodefine.h"

                                
#pragma section BootAPI

static unsigned long NEXT_BLOCK; 
static unsigned long CURR_BLOCK;

#pragma section

// For Boot
void SetNextBlock_Step1(unsigned long next, unsigned long current)
{                  
	NEXT_BLOCK = next;
	CURR_BLOCK = current;
}

void SetNextBlock_Step2(void)
{
#pragma asm   
	MOV.L @_$NEXT_BLOCK:32, ER4
	MOV.L @_$CURR_BLOCK:32, ER3
#pragma endasm	             
}

// For application        
void GetNextBlock_Step1(void)
{
#pragma asm   
	MOV.L ER4, @_$NEXT_BLOCK:32
	MOV.L ER3, @_$CURR_BLOCK:32	
#pragma endasm                        
	
}
void GetNextBlock_Step2(unsigned long *pNext, unsigned long *pCurrent)   
{   
	if (pNext != NULL) {              
		*pNext = NEXT_BLOCK;
	}   
	if (pCurrent != NULL) {    
		*pCurrent = CURR_BLOCK;
	}
}

void Quit(void)
{                    
	set_imask_ccr(1); 
	set_imask_exr(7);
#pragma asm   
	MOV.L @_$NEXT_BLOCK:32, ER4
#pragma endasm
	trapa(3);
}                          

void Repeat(void) 
{                       
	//PORT1.DR.BIT.B1 = 0;  
	set_imask_ccr(1); 
	set_imask_exr(7);
#pragma asm   
	MOV.L @_$CURR_BLOCK:32, ER4                                       
#pragma endasm	               
	trapa(3);
}  

void Reboot(void)
{  
	//PORT1.DR.BIT.B1 = 0;          
	set_imask_ccr(1);
	set_imask_exr(7);
#pragma asm   
	MOV.L #0:32, ER4
#pragma endasm	    
	trapa(3);
}
