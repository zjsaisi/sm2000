/*                   
 * Filename: ram_vect.c
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: ram_vect.c 1.1 2007/12/06 11:39:19PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */                        
 
// Only for release version
#if RUN_IN_ROM

#include "ConstDef.h"
#include "DataType.h"
#include "vect.h"

void * const RAM_INT_Vectors[] = 
#include "intrlist.h"

EXTERN void LoadRAMVect(void)
{
	int i; 
	const unsigned long *src = (const unsigned long *)RAM_INT_Vectors;
	unsigned long *target = (unsigned long *)0x200000;
	for (i = 2; i < 128; i++) {                      
		target[i] = src[i -2];
	}  
	target[1] = ((unsigned long)Manual_Reset) + JUMP_INSTR;
}

#else
static unsigned int keep_compiler_happy;
#endif
