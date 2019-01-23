/*                   
 * Filename: timer.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: timer.h 1.1 2007/12/06 11:41:39PST Zheng Miao (zmiao) Exp  $
 *	`	$Revision: 1.1 $
 */             

#ifndef _timer_h
#define _timer_h
//#include "zmdefine.h"
#include "DataType.h"

class CCheapTimer {  
public:
	void Delay(long ms);
	CCheapTimer(void);
	~CCheapTimer(void);
	void Start(long ms);
	int8 TimeOut(void); 
	long TimeLeft(void);
private:       
	uint16 m_preTime;
	long m_unit_to_go;
};
#endif
