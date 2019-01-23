/*                   
 * Filename: asm.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: asm.h 1.1 2007/12/06 11:39:07PST Zheng Miao (zmiao) Exp  $
 */ 
                    
#ifndef _ASM_H_ZHENG_MIAO
#define _ASM_H_ZHENG_MIAO 

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif        
#endif

EXTERN void SetNextBlock_Step1(unsigned long next, unsigned long current);  
EXTERN void SetNextBlock_Step2(void);                                       
EXTERN void GetNextBlock_Step1(void);
EXTERN void GetNextBlock_Step2(unsigned long *pNext, unsigned long *pCurrent);
EXTERN void Quit(void);              
EXTERN void Repeat(void);  
EXTERN void Reboot(void);
#endif
