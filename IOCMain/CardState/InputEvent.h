// InputEvent.h: interface for the CInputEvent class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputEvent.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputEvent.h 1.1 2007/12/06 11:41:14PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             

#if !defined(AFX_INPUTEVENT_H__688742DA_67ED_4809_B3B3_F2D846C86733__INCLUDED_)
#define AFX_INPUTEVENT_H__688742DA_67ED_4809_B3B3_F2D846C86733__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CInputEvent  
{
public:
	void ReportPerformanceStable(unsigned char flag);
	void ReportServerIpv4(uint32 ip);
	int ReportServerTrack(int track, int32 *pIstateVariable);
	int ReportQualified(int qualified);
	int ReportInputStatus(int stat);
	CInputEvent(int chan);
	virtual ~CInputEvent();
private:
	int m_chan;
	int m_stat;
	int m_qualified;
	int m_serverTrack;
	unsigned char m_serverFlag;
	uint32 m_ipv4;
};

#endif // !defined(AFX_INPUTEVENT_H__688742DA_67ED_4809_B3B3_F2D846C86733__INCLUDED_)
