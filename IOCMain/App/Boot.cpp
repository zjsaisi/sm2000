/***********************************************************************/
/*                                                                     */
/*  FILE        :Boot.cpp                                              */
/*  DATE        :Wed, Sep 18, 2002                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :H8S/2633F                                             */
/*                                                                     */
/*  This file is generated by Hitachi Project Generator (Ver.1.1).     */
/*                                                                     */
/***********************************************************************/
                  
#include "machine.h" 
#include "CodeOpt.h"
#include "DataType.h"
#include "g_cmx.h"
#include "CxFuncs.h"
#include "AllProcess.h"
#include "WatchDog.h"

static void StartCMX(void)
{ 
	set_exr(7);     
	g_cmx_started = 1; 
	K_OS_Start();			/* enter CMX RTOS */
	
}

EXTERN void main(void)
{    
	int ret;
	CAllProcess *pAll;
	
	// Kill self
	// while(1){};

	CWatchDog::StartWatchDog();

	// start CMX
	K_OS_Init();	 /* initialize ram and things */  
	{
		extern volatile unsigned long g_systemTicks; 
		g_systemTicks = 0L;
	}

	CWatchDog::ClearWatchDog();
	pAll = new CAllProcess();
	pAll->Init();
	pAll->StartAll();
	CWatchDog::ClearWatchDog();
	StartCMX();
}
                     
