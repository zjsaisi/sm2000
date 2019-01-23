/// Exe.c

/*************************************************************************************
**	EXE.C - H8/300H Monitor execution routines, 'C' Level code. Also see execute.src**
**	Monitor source ported from SH1 source                                       	**
**																					**
**	================================================================================**
**	Date		Version	Name		Description										**
**	================================================================================**
**  SH1 monitor																		**
**	05/02/96 1.0		NJH		Initial version of execute routines					**
**	06/02/96 1.1		NJH		Bug fix for execution of STC/STS instructions.		**
**	================================================================================**
**	06/11/96 1.0        DS      Initial port										**
**	31/01/97 1.1	    DS      Bug fix for execution of BRA instructions           ** 
**	================================================================================**
**  H8S monitor																		**
**	11/12/97 1.0		EJ 		Stenkil Software                                    **
**                              #include <inh83.h> changed to <inh8.h>              **
**																					**
************************************************************************************/

/* DS: no checking is done with respect to determining whether instructions 
have been correctly replaced with trap instructions and restoring instructions
after execution in function StepCode. To turn it on uncomment proceeding line */

/* Ported to H8S2623 - 01/03/99 (DTH) */
// Ported to 2633 1/10/01  DGL
/*
#define INST_STEP_CHECK 
*/

#include "break.h"    
#include "exe.h"   
#include "sci_mon.h"   
#include "branch.h"
#include "instleng.h" 
#include <machine.h>     

#pragma section MONITOR                                

#define NOP 0x0                /* NOP instruction code value */
 
extern BYTE comms_state;

extern REGS 	regs;

ERCD BPdisable (void);  
ERCD BPenable (void);
void BPinitialise (void);   
ERCD BPrestoreInst (SH_REG);
ERCD BPrestoreTrap (SH_REG);
DWORD GetRegister (BYTE);
void SetRegister (BYTE, DWORD);
ERCD ConditionsOK (SH_REG);

 
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
		


ERCD GetBreakPoint(SH_REG address, SH_INST *instruction, WORD *count, WORD *bp_count);

ERCD GetBreakPointByIndex(BYTE i, SH_REG *address, SH_INST *instruction, WORD *count, WORD *occurences);

void InitBreakTable(void);


ERCD BPincrementCount(SH_REG address);


	
BYTE NumberOfBP(void);


BYTE MaxNumberOfBP(void);

                              
ERCD BPresetCounts(void);




SH_REG stack_pointer; /* Monitor SP is stored here during target code execution */    

void 
InitExecution(void)
{
	BPinitialise ();
}
                           
int
IsBSRJSR (unsigned short instruction)
{                                 
	/* DS:function to determine whether instruction is a (BSR or JSR) 
	assumption: instruction is a jmp instruction 
		i.e. IsJmp(instruction) returns true
	If re-using this function, ie calling it from somewhere other than
	StepCode uncomment following code about IsJmp*/
		
	instruction = instruction & 0xff00;                                            
    
    /* DS: the following is uncommented because the argument instruction IS
    a branch because it has already been tested when called by StepCode */
     
	/*	
	if (!IsJmP(instruction))
		return FALSE;
	*/
		
	if
	(
	instruction == 0x5500 ||
	(instruction & 0x0c00) == 0x0c00 /* matches 5c,5d,5e,5f */  
	)
	return TRUE;
	return FALSE;
}                                                                       

int 
IsTrap (unsigned short instruction)
{
	/* DS:function to determine whether instruction is a TRAPA instruction */
	/* return true if so */
	
	if ((instruction & 0xff00) == 0x5700)
		return TRUE;
	return FALSE;
}
                                    
ERCD StepCode(char mode)
{                                    
 
 	/* DS: 	function to single step or step-over depending on the mode
 		 	mode = STEP_SINGLE: single step through function
 		 	mode = STEP_OVER  : step over function calls 
 	*/

	ERCD err;
    unsigned long trap_address = 0, branch_address = 0;
	unsigned long pc = GetRegister(REG_PC), stored_next_address = 0;
	unsigned short instruction = *(unsigned short *)pc;	
	unsigned short stored_branch_target_instruction = 0;
	unsigned short stored_next_instruction = 0;
   int default_action = 0;
    	/* DS: flag to specify whether to carry out default action 
    	which is add trap after current instruction 
    	if set to default */
                                              
                                              
				/* ------------------------ */
				/* find addresses for traps */
				/* ------------------------ */

	if (IsTrap(instruction))
	{
						 /* ------------------------------------- */
						 /* handle trapa instructions					*/
						 /* if mode==STEP_OVER, default action		*/
						 /* else breakpoint at start of exception */
						 /* processing										*/
						 /* ------------------------------------- */

		if (mode == STEP_OVER)
			default_action = 1;
		else       /* SINGLE STEP */
		{						                                              
			/* the address of the respective interrupt service routines are 
				contained in the *shadow* vector table, which starts at 0x40000.
				Also, the shadow vector table does not contain addresses but jump
				instructions; however, the addresses are contained in the latter 24 
				bits of the instruction , so all is needed is to mask off MSB of 
				long word */
				
			switch(instruction)                                          
			{
			case 0x5710:       		/* TRAPA #1 */
				#ifdef H8S2000
				branch_address = (*(unsigned long *)0x20024) & 0x00ffffff;
				#elif defined H8S2623
					#ifndef MODE7
					branch_address = (*(unsigned long *)0x40024) & 0x00ffffff;
					#else
					branch_address = (*(unsigned long *)0xffc024) & 0x00ffffff;
					#endif	
				#else
				branch_address = (*(unsigned long *)0x40024) & 0x00ffffff;
				#endif
				break;
			case 0x5720:            /* TRAPA #2 */
				#ifdef H8S2000
				branch_address = (*(unsigned long *)0x20028) & 0x00ffffff;
				#elif defined H8S2623
					#ifndef MODE7
					branch_address = (*(unsigned long *)0x40028) & 0x00ffffff;
					#else
					branch_address = (*(unsigned long *)0xffc028) & 0x00ffffff;
					#endif	
				#else
				branch_address = (*(unsigned long *)0x40028) & 0x00ffffff;
				#endif
				break;
			case 0x5730:            /* TRAPA #3 */
				#ifdef H8S2000
				branch_address = (*(unsigned long *)0x2002C) & 0x00ffffff;
				#elif defined H8S2623
					#ifndef MODE7
					branch_address = (*(unsigned long *)0x4002C) & 0x00ffffff;
					#else
					branch_address = (*(unsigned long *)0xffc02C) & 0x00ffffff;
					#endif	
				#else
				branch_address = (*(unsigned long *)0x4002C) & 0x00ffffff;
				#endif
				break;
			}

			/* check if the branch address is 0. If so, return error. This 
				occurs when the address of the respective service routine has 
				not been entered into the shadow vector */
							
			if (branch_address == 0)
				return ERCD_FATAL;
		}
	}

	else if (instruction == 0x5470 || instruction == 0x5670)
	{
					/* ----------------------------  */
					/* instruction is an RTS or RTE  */
					/* place break at return PC      */
					/* ----------------------------  */
			
		branch_address = (*(unsigned long *)GetRegister(REG_ER7)) & 0x00ffffff;
	}
		
	else 	if (IsBranch(instruction))
	{                                                                
						/* ----------------------------------- */                                                                
						/*    handle branch instructions 		*/
						/* doesn't matter about mode; action  	*/
						/* the same: find the 2 addresses where*/ 
						/* traps are to be inserted				*/
						/*													*/
						/* 	1. trapa at branch target			*/
						/* 	2. trapa at next instruction		*/						
						/*													*/
                  /* 31/01/97 DS bug fix BRA instructions*/
						/* only insert trap at branch target   */						
						/* ----------------------------------- */


		signed short Short_Disp = get_displacement_of_branch (pc);
		signed long Pc = pc, Long_Disp = (signed long)Short_Disp, result;	
		unsigned short instruction_length;
		
		result = Pc + Long_Disp;		
		
		branch_address = result; 
		
		/* DS: since pc points to the current instruction and not the next 
		instruction (because it was decremented when the exception was handled)
		we need to add to the branch address the length of the current instruction. 
		Either 2 or 4 will be added
		*/                                                            
		if ((instruction & 0x1000) == 0x1000)	/* matches branches with ops 5xxx */
		{
			branch_address += 4;
			instruction_length = 4;
		}
		else
		{     								/* matches branches with ops 4xxx */
			branch_address += 2; 
			instruction_length = 2;
		}
		/* 31/01/97 DS bug fix for BRA instruction*/
		if ( 
			(instruction & 0xff00) == 0x4000 || /*BRA*/
			(instruction & 0xff00) == 0x4100 || /*BRN*/
			instruction == 0x5800 ||            /*BRA*/
			instruction == 5810                 /*BRN*/
			)
			default_action = 0;
		else if (regs.pc + instruction_length == branch_address) /*if two destinations are same */
			default_action = 0;
		else		
			default_action = 1; /* turns on flag to find address of next instruction */
	}


	else if (
			mode == STEP_SINGLE && IsJmp(instruction) ||
			mode == STEP_OVER && IsJmp(instruction) && !IsBSRJSR(instruction)
			/* 	if mode is STEP_OVER and instruction *is* BSR,JSR then
				place trap at next instruction,ie default action
			*/
			)
	{		                           
						/* ------------------------------------ */                                                                
						/*    handle jump instructions   		*/
						/*    action depends on mode            */
						/*			and jmp type			  	*/
						/* find the address where trap is to be */
						/* inserted 							*/
						/*										*/
						/*	 1. trapa at jump address			*/
						/* ------------------------------------ */		                          
     
		branch_address = get_address_of_jump(pc);
			/* does not perform default action */
	}

	else	
						/* ------------------------------------ */                                                                
						/*           default action		   		*/
						/* 1. trapa at next instruction			*/
						/* ------------------------------------ */		        	

		default_action = 1;	/* turns on flag to find address of next instruction */
                                                                    

				/* ------------------------ */
				/* end of :-				*/
				/* find addresses for traps */
				/* ------------------------ */                                                                   
                                                                    
				/* ------------------------------------ */
				/* insert trapas according to modes and */
				/* instruction type						*/
				/* ------------------------------------ */	

	if (branch_address)     /* does this if branch_address is set */
	{					
		/* DS: add trap for branch */

		stored_branch_target_instruction = *(unsigned short *)branch_address;		
		*(unsigned short *)branch_address = TRAPA;
		#ifdef INST_STEP_CHECK
		if (*(unsigned short *)branch_address != TRAPA)
			return ERCD_FATAL;                                      
		#endif
	}

	if (default_action)   	/* does this if flag is set */
	{
		/* DS: add trap for default action:
		default action: set trap to occur at next instruction */
		
		stored_next_address = pc + get_instruction_length(pc);
		stored_next_instruction = *(unsigned short *)stored_next_address;
		*(unsigned short *)stored_next_address = TRAPA;
		#ifdef INST_STEP_CHECK
		if (*(unsigned short *)stored_next_address != TRAPA)
			return ERCD_FATAL;
		#endif
	}
	            /* ------- */
				/* execute */
				/* ------- */
	
	/*err = Execute ();*/
	Execute ();
	err = Execute_return_code; 
	                                             
					
				/* ------------------------------------ */
				/*       restore instructions			*/
				/* ------------------------------------ */		
						
									                                                    
  
	if (branch_address)
	{
		*(unsigned short *)branch_address = stored_branch_target_instruction;
		#ifdef INST_STEP_CHECK
		if (*(unsigned short *)branch_address != stored_branch_target_instruction)
			return ERCD_FATAL;
		#endif
	}	
	if (default_action)
	{
		*(unsigned short *)stored_next_address = stored_next_instruction;
		#ifdef INST_STEP_CHECK
		if (*(unsigned short *)stored_next_address != stored_next_instruction)
			return ERCD_FATAL;
		#endif 	  
	}

	/* now do a check to see if the current instruction is TRAPA. 
		If it is return ERCD_BP
		if not return ERCD_UBC */

	if (*(SH_INST *)GetRegister(REG_PC) != TRAPA)
		err = ERCD_UBC;
	
	
	return err;
}	           /* end of int StepCode(int mode)	  	*/		
					


/* This routine will execute the instruction that is replaced by a TRAPA at a 
break point.If no BP is at the PC location, the instruction will be handled
with a single step command. */
 
ERCD RunOverBreakPoint(char mode)
{
	register SH_REG the_pc;                 
	ERCD err;                      
   	
   	if (*(SH_INST *)GetRegister(REG_PC) != TRAPA)
   		return StepCode(mode);
	
	else if (BPrestoreInst(GetRegister(REG_PC)) == ERCD_OK) 
	{
		the_pc = GetRegister(REG_PC);
		err = StepCode(mode);
		if (BPrestoreTrap(the_pc) == ERCD_OK)
			return err;
		else
			return ERCD_FATAL;
	} 
	else 
	{   
	    /* Not a valid breakpoint, so skip over it */                              
	    
		/* ???? */	
		
		SetRegister(REG_PC, GetRegister(REG_PC) + 2); /* the trap is 2 bytes long */
		return ERCD_OK;
	}
}    		/* end of ERCD RunOverBreakPoint(char mode) */
		

/* This routine is used to control execution of the user code.  The first parameter 
is the mode of execution and can be single step, step over, or run (go).  The second 
parameter is the number of steps for the step modes only. */		

ERCD ExecuteUserCode(BYTE mode, WORD no_times)
{
	ERCD err = ERCD_UBC;
	
	register WORD n;

	/* Perform register alignment tests before beggining. */	

	if ((GetRegister(REG_PC) & 1) != 0) /* Must align to 2 */
		return ERCD_ALIGN;
	else if ((GetRegister(REG_ER7) & 1) != 0) /* Must align to 2 */
		return ERCD_ALIGN;

	BPresetCounts();	/* Reset occurence counters on all BP's */		
	RxInterruptsOn();   /* Enable serial Rx interrupts to be generated 
							(Depending on user SR) */
						
	BPenable(); 		/* Activate software breakpoints */

	if ((mode == STEP_SINGLE || mode == STEP_OVER) && no_times > 0)
		for (n = 1; n <= no_times & err == ERCD_UBC; n ++)
			err = RunOverBreakPoint(mode); 
			
	else if (mode == RUN) 
	{
		do 
		{ 
		/* This loop is used to check the occurence of 
			break points with their count value */

			err = RunOverBreakPoint(STEP_SINGLE);			
			if (err == ERCD_UBC|| err == ERCD_BP)
									/* ^^^^^^^^^^^^^^^^^ bug fix V0.5*/
			{
				Execute ();
				err = Execute_return_code;
				if (err == ERCD_BP)
					BPincrementCount(GetRegister(REG_PC));	
					/* Increment count of BP occurence */  
			}
		}
		while (err == ERCD_BP && ! ConditionsOK(GetRegister(REG_PC))); 
			/* Stop when BP conditions met */
	}   
	
	else
		err = ERCD_PARM;		/* Error in given parameters */

	BPdisable(); 			/* Deactivate software breakpoints */
	
	RxInterruptsOff(); 	/* Disable serial Rx interrupts from being generated */
//	set_interrupt_mask(3);		/* Disable all interrupts from being accepted */
    set_imask_exr(7);
	return err;
}

                                                                      
