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
#include 	"string.h"
#include	"stacksct.h"
#include 	"iodefine.h"				// ZMIAO: Included to support register access.
#include 	"CodeOpt.h" 
#include 	"asm.h"
#include 	"version.h"
#include 	"ConstDef.h"
#include 	"Debug.h"

#define MAGICH 0xa5a51234
#define MAGICL 0x5a5a9876

#pragma section CheckSum
#if 1
const unsigned long boot_sumH = MAGICH;
const unsigned long boot_sumL = MAGICL;
#else 
const unsigned long boot_sumH = 0x8047;
const unsigned long boot_sumL = 0x2d90a923; // af207af9;
#endif


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

//extern void _call_init(void);			// Remove the comment when you use global class object
//extern void _call_end(void);			// Remove the comment when you use global class object

#if CHECK_RAM
EXTERN void VerifyRAM(void);
EXTERN void ClearRam(void);
#endif
	
#pragma section ResetPRG

unsigned long g_nextBlock;
static unsigned long tmp;
unsigned char g_appFailCnt; 
unsigned char g_cmx_started;

EXTERN void Reset(void);
EXTERN void VerifyROM(void);

EXTERN void Quit_Entry(void)
{  
#ifndef _VISUAL_CPP
#ifdef _NDEBUG
#pragma asm   
	MOV.L #H'ffebc0, SP
#pragma endasm
#else
#pragma asm   
	MOV.L #H'27ffc0, SP
#pragma endasm	
#endif
#endif           
	GetNextBlock_Step1();
	GetNextBlock_Step2(&g_nextBlock, &tmp);
	if (g_nextBlock == 0L) {
		g_appFailCnt++;
	}
	Reset();
}

extern unsigned long g_monFlag1;
extern unsigned long g_monFlag2;
extern volatile unsigned long g_systemTicks;

EXTERN void PowerON_Reset(void)
{
	g_nextBlock = 0L;
	if (g_systemTicks > MIN_REBOOT_PERIOD) {
		g_appFailCnt = 0;
	} else {
		g_appFailCnt++;
	}
#if 1	          
	g_monFlag1 = 0L; // This will diable cross debugging with Hitachi tool.
#endif	
	Reset();
}

EXTERN void mon_startup(void);
EXTERN void Fatal(const char *pStr);
                               
static void HolyCompiler(void)
{     
	const char *ptr;
	g_cmx_started = 0;                   
#if 0
	Fatal("\r\nForced to failed mode\r\n");         
#else 
	ptr = "\r\nForced to failed mode\r\n";
	Fatal(ptr);
#endif	  
}
                         
EXTERN void GoFailed(void)
{                       
#ifndef _VISUAL_CPP
#ifdef _NDEBUG
#pragma asm   
	MOV.L #H'ffebc0, SP
#pragma endasm
#else
#pragma asm   
	MOV.L #H'27ffc0, SP
#pragma endasm	
#endif
#endif           
	HardwareSetup(); // Disbale DTC
	HolyCompiler();                     
}                

EXTERN void Reset(void)
{    
	// ZMIAO: Release version: Stack is expected at 0xFFEBC0. Set stack size to 0, stacksct.h 
	// Debug: Stack starts @0x280000.
	g_cmx_started = 0;	
	 set_imask_ccr(1); 
    
	// ZMIAO: Interruption mode to 2 & disable interrupt.
  	set_imask_exr(7); 
  	SYSCR.BIT.INTM = 2;        
	                     
	// ZMIAO: Double make sure internal RAM is enabled.
  	SYSCR.BIT.RAME  = 1;

	PC.DDR = 0xFF;     		/* enable A0-A7 address bits */
	
	// ZMIAO: Move Hareware setup ahead to make memory access
	HardwareSetup();				// Use Hardware Setup

	// Check RAM and start necessary
	_INITSCT();
    
    if (!g_nextBlock)   
		VerifyROM();

#if CHECK_RAM
	// Only check ram when it starts the first time.
	// And not means to be monitor.
	// If monitor, don't check RAM. Otherwise RAM will be wipe out.
	if ( (g_monFlag1 == DBUG_FLAG1) && (g_monFlag2 == DBUG_FLAG2) ) {
		// Don't do anything
	} else if (g_nextBlock) {
	 	ClearRam();
	} else { 
		VerifyRAM();
	}
#endif	   
	
	_INITSCT();

//	_INIT_IOLIB();					// Remove the comment when you use SIM I/O

//	errno=0;						// Remove the comment when you use errno
//	srand(1);						// Remove the comment when you use rand()
//	_s1ptr=NULL;					// Remove the comment when you use strtok()
		
//	HardwareSetup();				// Use Hardware Setup

//	_call_init();					// Remove the comment when you use global class object
									// Sections C$INIT and C$END will be generated

	// See if it should go to monitor code
	if ((g_monFlag1 == DBUG_FLAG1) && (g_monFlag2 == DBUG_FLAG2)) {
		CDebug::CmdMonitor(NULL);
		// Should never get here		
	}
									
	main();

//	_call_end();					// Remove the comment when you use global class object

//	_CLOSEALL();					// Remove the comment when you use SIM I/O
	
	sleep();
}

EXTERN void Manual_Reset(void); 
void Manual_Reset(void)
{
}


// 1 for success 
// 2 for magic word found
// -1 for failure
// -2, -3 checksum error   
// -100 DTC table error
// other vector table error       
// Defined here because checksum number is in special section (CCheckSum) 
EXTERN void    INT_TRAP4(void);
EXTERN int 	CompDTCVector(void);

// From Monitor 
EXTERN void NMIcapture(void);
EXTERN void BreakPoint(void);
 
EXTERN int PassCheckSum(unsigned long *sumH, unsigned long *sumL)
{  
#if _DEBUG
	return 1;
#endif

#if NDEBUG        
	unsigned long *lPtr;
	unsigned long i;
	unsigned long chksumH = 0L;    
	unsigned long chksumL = 0L;
	unsigned long lTmp;  
	int ret = 2;

	// return ret;

	lPtr = (unsigned long *)0x10000;
	for (i = 0; i < 0x4000 - 2 - 62; i++) {
		lTmp = chksumL;
		chksumL += *lPtr;    
		if (lTmp > chksumL) {
			chksumH++;
		}
		lPtr++;  
	}                        
	lPtr = (unsigned long *)0x20000;
	for (i = 0L; i < 0x8000L; i++) {
		lTmp = chksumL;
		chksumL += *lPtr;
		if (lTmp > chksumL) {
			chksumH++;
		}
		lPtr++;
	}
	
	*sumH = chksumH;
	*sumL = chksumL;
	lPtr = (unsigned long *)0x1fff8;
	if (*lPtr != MAGICH) {
		ret = 1;
		if (*lPtr != chksumH) return -3;
	}       
	lPtr = (unsigned long *)0x1fffc;
	if (*lPtr != MAGICL) {
		ret = 1;
		if (*lPtr != chksumL) return -1;
	}                         
	
	lPtr = (unsigned long *)0;
	if (*lPtr != ((unsigned long)PowerON_Reset) )  {
		return -2;
	}              
	for (i = 1; i < 128; i++) { 
		switch (i) {
		case 11:
			if (lPtr[i] != ((unsigned long) INT_TRAP4)) return (-i - 1000);
			continue;
		case 7:
			if (lPtr[i] != ((unsigned long)NMIcapture)) return (-i - 1000);
			continue;
		case 8:
			if (lPtr[i] != ((unsigned long)BreakPoint)) return (-i - 1000);
			continue;
		default:
			break;	
		}
		if (lPtr[i] != 0x200000 + i * 4) 
			return (-i-1000);
	}
	
	int temp = CompDTCVector();
	if (temp != 1) return -100;
	return ret;
#endif	
}    

#pragma section BldDate   
unsigned long const version = 1; // Version of this area
unsigned long const release_date = VERSION_DATE;
unsigned long const release_number = VERSION_NUMBER;
char const * const g_sBuildDate = __DATE__;
char const * const g_sBuildTime = __TIME__;
#pragma section
