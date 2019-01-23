/*                   
 * Filename: timer.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: timer.cpp 1.2 2009/05/05 16:44:23PDT Zheng Miao (zmiao) Exp  $
 */             

#include "timer.h"
#include "iodefine.h"
#include "ConstDef.h"

CCheapTimer::CCheapTimer(void)
{
	m_preTime = 0;
	m_unit_to_go = 0L;
}

CCheapTimer::~CCheapTimer(void)
{
}
              
void CCheapTimer::Start(long ms)
{
 	long freq = CHEAP_TMR_CLK;
 	m_unit_to_go = ms * freq / 1000;
 	m_preTime = TMR1.TCNT; 		
}
    
long CCheapTimer::TimeLeft(void)
{
	if (TimeOut()) return 0L;
	return (m_unit_to_go * 1000 / (CHEAP_TMR_CLK));	
}    

int8 CCheapTimer::TimeOut(void)
{   
	uint8 cur = TMR1.TCNT;
	int16 diff;          

	if (cur >= m_preTime) {
	 	diff = (int16)(cur - m_preTime);
	} else {          
		diff = 256 + (int16)cur - (int16)m_preTime;
	}  
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
