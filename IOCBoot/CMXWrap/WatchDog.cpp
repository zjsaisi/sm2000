// WatchDog.cpp: implementation of the CWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#include "CodeOpt.h"
#include "DataType.h"
#include "WatchDog.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWatchDog::CWatchDog()
{
}

CWatchDog::~CWatchDog()
{

}

// Start Watch dog
void CWatchDog::StartWatchDog()
{
	// Setup WDT1:  16Mhz main clk. 131072*256/16M = 2.10 second
	volatile uint16 *write_dog = (volatile uint16 *)0xffffa2;

	*write_dog = 0xa54f; // Disable dog
	*write_dog = 0x5a00; // Clear counter
	*write_dog = 0xa56f; // Enable dog
}


void CWatchDog::StopWatchDog()
{
	volatile uint16 *write_dog = (volatile uint16 *)0xffffa2;
	*write_dog = 0x5a00; // Clear counter
	*write_dog = 0xa54f; // Disable dog
	*write_dog = 0x5a00; // Clear counter

}

