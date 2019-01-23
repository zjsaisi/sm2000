/***********************************************************************/
/*                                                                     */
/*  FILE        :resetprg.cpp                                          */
/*  DATE        :Wed, Sep 18, 2002                                     */
/*  DESCRIPTION :Reset Program                                         */
/*  CPU TYPE    :H8S/2633F                                             */
/*                                                                     */
/*  This file is generated by Hitachi Project Generator (Ver.1.1).     */
/*                                                                     */
/***********************************************************************/
                  

#include 	"DataType.h"
#include	<machine.h>
//#include	<stddef.h>					// Remove the comment when you use errno
//#include 	<stdlib.h>					// Remove the comment when you use rand()
//#include	<ios>						// Remove the comment when you use ios
//int ios::Init::init_cnt;				// Remove the comment when you use ios
#include	"stacksct.h"
#include 	"iodefine.h"				// ZMIAO: Included to support register access.
#include 	"CodeOpt.h" 
#include 	"asm.h"
#include 	"version.h"

#pragma entry PowerON_Reset        

EXTERN void main(void);

#ifdef __cplusplus
extern "C" {
#endif
extern void _INITSCT(void);
#ifdef __cplusplus
}
#endif

//#ifdef __cplusplus				// Remove the comment when you use SIM I/O
//extern "C" {
//#endif
//extern void _INIT_IOLIB(void);
//extern void _CLOSEALL(void);
//#ifdef __cplusplus
//}
//#endif

//extern void srand(unsigned int);	// Remove the comment when you use rand()
//extern char *_s1ptr;				// Remove the comment when you use strtok()
		
#ifdef __cplusplus				// Use Hardware Setup
extern "C" {
#endif
extern void HardwareSetup(void);
#ifdef __cplusplus
}
#endif

extern void _call_init(void);			// Remove the comment when you use global class object
extern void _call_end(void);			// Remove the comment when you use global class object


#pragma section ResetPRG

//unsigned long g_nextBlock;

EXTERN void PowerON_Reset(void)
{    		
	//set_imask_ccr(1); 
    
	// ZMIAO: Interruption mode to 2 & disable interrupt.
  	set_imask_exr(7); 
  	SYSCR.BIT.INTM = 2;        
	                     
	// ZMIAO: Double make sure internal RAM is enabled.
  	SYSCR.BIT.RAME  = 1;

	PC.DDR = 0xFF;     		/* enable A0-A7 address bits */
	
	// ZMIAO: Move Hareware setup ahead to make memory access
	HardwareSetup();				// Use Hardware Setup

	// Init RAM data
	_INITSCT();

//	_INIT_IOLIB();					// Remove the comment when you use SIM I/O

//	errno=0;						// Remove the comment when you use errno
//	srand(1);						// Remove the comment when you use rand()
//	_s1ptr=NULL;					// Remove the comment when you use strtok()
		
//	HardwareSetup();				// Use Hardware Setup

	_call_init();					// Remove the comment when you use global class object
									// Sections C$INIT and C$END will be generated
	main();

	_call_end();					// Remove the comment when you use global class object

//	_CLOSEALL();					// Remove the comment when you use SIM I/O
	
	sleep();
}

EXTERN void Manual_Reset(void); 
void Manual_Reset(void)
{
}

EXTERN void    INT_TRAP4(void); 

#pragma section BldDate   
unsigned long const version = 1; // Version of this area
unsigned long const release_date = VERSION_DATE;
unsigned long const release_number = VERSION_NUMBER;
char const * const g_sBuildDate = __DATE__;
char const * const g_sBuildTime = __TIME__;
#pragma section
