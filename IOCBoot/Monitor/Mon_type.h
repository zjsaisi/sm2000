/// mon_type.h

/* this include file defines user program control data structures */

/*
********************************** *
*  300H register info. added by DS *
*  5/11/96                         *
* ******************************** *
*/
/*
**=================================================================================**
**  H8S monitor																       **
**	11/12/97 1.0		EJ 		Stenkil Software                                   **
**                              H8S register info. added                           **
**	                            #define H8S2655 added                              **
**	                            #ifdef H8S2655 added                               **
**=================================================================================**
*/

/* Ported to H8S2623 - 01/03/99 (DTH) */
// Ported to 2633 1/10/01 DGL
	
#define TRUE 1
#define FALSE 0     


#define MSG_INBUF_LENGTH	50
#define MSG_OUTBUF_LENGTH	50  


/* command defines */
enum
{       
	/* original spec */
	MC_INITIALISE,
	MC_CAPABILITY_QUERY,
	MC_EXECUTE,
	MC_BREAK,
	MC_REGISTER_SETTING,
	MC_REGISTER_QUERY,
	MC_MEMORY_SETTING,
	MC_MEMORY_FILL,
	MC_MEMORY_QUERY,
	MC_MEMORY_SCAN,
	MC_MEMORY_VERIFY,
	MC_MEMORY_VERIFY_FILL,
	MC_MEMORY_CALCULATE_SIGNATURE,
	MC_BREAKPOINT_SETTING,
	MC_BREAKPOINT_QUERY,
	MC_DOWNLOAD,
	MC_MEMORY_GENERAL,
	MC_IO_READ,				/* this is a new one */
	MC_IO_WRITE				/* this is a new one */
};    

/* replies defines */
enum
{       
	/* original spec */
	MR_CAPABILITY,
	MR_END_OF_EXECUTION,
	MR_MEMORY_SCAN_RESULT,
	MR_MEMORY_VERIFY_RESULT,
	MR_MEMORY_SIGNATURE,
	MR_BREAKPOINT_SETTING, 
	MR_RECEPTION_ERROR,		/* this is a new one */   
	MR_VERIFY_ERROR,		/* this is a new one */  
	MR_VERIFY_FILL_ERROR,		/* this is a new one */  
	MR_COMPLETE,			/* this is a new one */  
	MR_UNKNOWN_ERROR,		/* thus is a new one */
	MR_MEMORY_GENERAL,		/* thus is a new one */
	MR_IO_RESULT,			/* this is a new one */        
	MR_DOWNLOAD_FAILURE,	/* 19/12/96 */
	MR_RESET
}; 
      

typedef enum {
	WRITE,     
	VERIFY,
	VERIFY_WRITE
} VERIFICATION;

typedef enum {
	BREAK_ENABLE,     
	BREAK_DISABLE,
	BREAK_QUERY
} BREAKPOINTS;   

typedef enum {
	GMC_COPY,     
	GMC_COMPARE
} MEMORY_GENERAL;


typedef unsigned short	WORD;  
typedef unsigned char	BYTE; 
typedef unsigned long 	DWORD; 

typedef DWORD SH_REG;
typedef WORD SH_INST;


#ifdef SHDSP

/* SH-DSP register structure */    
typedef struct 
{ 
/* general registers */ 
	DWORD	r0; 
	DWORD	r1; 
	DWORD	r2;
	DWORD	r3;
	DWORD	r4;
	DWORD	r5;
	DWORD	r6;
	DWORD	r7;
	DWORD	r8;
	DWORD	r9;
	DWORD	r10;
	DWORD	r11;
	DWORD	r12;
	DWORD	r13;
	DWORD	r14;
	DWORD	r15;
/* control register */	
	DWORD	sr;
	DWORD	rs;
	DWORD	re;
	DWORD	gbr;
	DWORD	vbr;
	DWORD	mod; 
/* system registers */	
	DWORD	MACH;
	DWORD	MACL; 
	DWORD	PR;
	DWORD	PC; 
/* dsp registers */
	DWORD 	A0H; 
	DWORD 	A0L; 
	DWORD 	A1H;
	DWORD	A1L;
	DWORD	M0;	
	DWORD	M1;	
	DWORD	X0;	
	DWORD	X1;	
	DWORD	Y0;	
	DWORD	Y1;
}REGS;    

/* the one below must be made for SH-DSP */
enum 
{ 
		REG_R15,
		REG_SR,
		REG_PC, 
		REG_R0, 
	 
		REG_R1, 
		REG_R2,
		REG_R3,
		REG_R4, 
	
		REG_R5,
		REG_R6, 
		REG_R7,
		REG_R8, 
	
		REG_R9,
		REG_R10,  
		REG_R11,
		REG_R12,  
	
		REG_R13,
		REG_R14,
		REG_GBR,
		REG_VBR,    
	
		REG_PR,
		REG_MACH,
		REG_MACL,
		REG_ALL		/* this used to get all registers */
};

#endif

#ifdef SH
	      	      	      	      
typedef struct 
{    
/* general registers order somehow differently */
	  
	DWORD	r2;
	DWORD	r3;
	DWORD	r4; 
	
	DWORD	r5;
	DWORD	r6; 
	DWORD	r7;
	DWORD	r8; 
	
	DWORD	r9;
	DWORD	r10;  
	DWORD	r11;
	DWORD	r12;  
	
	DWORD	r13;
	DWORD	r14; 
	DWORD	r15;
	DWORD	gbr;
	
	DWORD	vbr;    
	DWORD	PR;
	DWORD	MACH;
	DWORD	MACL;  
	
	DWORD	sr;
	DWORD	PC; 
	DWORD	r0;	/* used as working register */
	DWORD	r1; 	/* used as workin register */

}REGS;      


enum 
{ 	  
		REG_R2,
		REG_R3,
		REG_R4, 
		REG_R5,
		
		REG_R6, 
		REG_R7,
		REG_R8, 
		REG_R9,
		
		REG_R10,  
		REG_R11,
		REG_R12,  
		REG_R13,   
		
		REG_R14,
		REG_R15,
		REG_GBR,
		REG_VBR,    
	
		REG_PR,
		REG_MACH,
		REG_MACL,	
		REG_SR,
		
		REG_PC,   
		REG_R0,
		REG_R1,   
		REG_ALL
};

#endif
          

#ifdef H8300H

typedef struct {

	DWORD er2;
	DWORD er3;
	DWORD er4;
	DWORD er5;
	DWORD er6;
	DWORD er7;
	DWORD pc;  
	DWORD ccr; 
	DWORD er0;
	DWORD er1;
} REGS;

enum {    

	REG_ER2,
	REG_ER3,
	REG_ER4,
	REG_ER5,
	REG_ER6,
	REG_ER7,
	REG_PC,
	REG_CCR,
	REG_ER0,
	REG_ER1,
	REG_ALL
};

#define REG_SP REG_ER7

#endif


#if defined H8S2655 || defined H8S2000 || defined H8S2623

typedef struct {

	DWORD er2;
	DWORD er3;
	DWORD er4;
	DWORD er5;
	DWORD er6;
	DWORD er7;
	DWORD pc;  
	DWORD ccr; 
	DWORD er0;
	DWORD er1;
	DWORD exr;  
#if defined H8S2655 || defined H8S2623
	DWORD mach;
	DWORD macl; 
#endif
} REGS;

enum {    

	REG_ER2,
	REG_ER3,
	REG_ER4,
	REG_ER5,
	REG_ER6,
	REG_ER7,
	REG_PC,
	REG_CCR,
	REG_ER0,
	REG_ER1,
	REG_EXR,
#if defined H8S2655 || defined H8S2623
	REG_MACH,
  	REG_MACL,   
#endif  
	REG_ALL
};

#define REG_SP REG_ER7

#endif
		
/*========================================================================================================*/
/*== 													Monitor Error Codes															 */
/*========================================================================================================*/
	
/* Execution error codes */

typedef unsigned short ERCD;

/* These error codes must much those in the assembler file 'execute.src' */
#define ERCD_OK 		0
#define ERCD_ABORT 		1
#define ERCD_GENINST 		2
#define ERCD_SLOTINST 		3
#define ERCD_CPUADDR 		4
#define ERCD_DMAADDR 		5
#define ERCD_BP			6
#define ERCD_UBC		7     
#define ERCD_NMI		8    
#define ERCD_SCIERR		9
#define ERCD_SCI		10


/* The value of these error codes must follow on from the above codes */
#define ERCD_NOWRITE 	11
#define ERCD_BPFULL 	12
#define ERCD_BPEXISTS 	13  

#define ERCD_NOTFOUND 	14
#define ERCD_MEMORY 	15    
#define ERCD_FATAL 	16
#define ERCD_PARM 	17 /* Parameter error */
#define ERCD_GENERAL 18
#define ERCD_BPSET   19    
#define ERCD_UBCISR	20	/* UBC vector entry does not point to UBC trap routine */
#define ERCD_UBCMSK	21	/* Can not carry out operation as UBC interrupt is masked */
#define ERCD_ALIGN	22 /* PC, R15 or VBR does not align correctly */                              

#ifndef NULL
	#define NULL 0
#endif

#define MAX_BP 10



        
