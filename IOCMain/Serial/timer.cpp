/*                   
 * Filename: timer.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: timer.cpp 1.1 2007/12/06 11:41:39PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             

#include "timer.h"
#include "iodefine.h"
#include "ConstDef.h"

CCheapTimer::CCheapTimer(void)
{
	m_unit_to_go = 0L;
	m_preTime = 0;
}

CCheapTimer::~CCheapTimer(void)
{
}
              
void CCheapTimer::Start(long ms)
{
 	long freq = CHEAP_TMR_CLK;
 	m_unit_to_go = ms * freq / 1000;
 	m_preTime = TMRA.TCNT; 		
}
    
long CCheapTimer::TimeLeft(void)
{
	if (TimeOut()) return 0L;
	return (m_unit_to_go * 1000 / (CHEAP_TMR_CLK));	
}    

int8 CCheapTimer::TimeOut(void)
{   
	uint16 cur = TMRA.TCNT;
	uint16 diff;          

  	if (m_unit_to_go <= 0L) return 1;
 	diff = (cur - m_preTime);
	m_unit_to_go -= (long)diff;
	m_preTime = cur;
  	if (m_unit_to_go <= 0L) return 1;
  	return 0;
}

void CCheapTimer::Delay(long ms)
{
	Start(ms);
	while (!TimeOut()) ;
}
