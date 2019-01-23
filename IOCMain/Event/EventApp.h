// EventApp.h: interface for the CEventApp class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: EventApp.h 1.1 2007/12/06 11:41:22PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_EVENTAPP_H__99BEF319_FC2C_4761_B8B3_A968D4E6530C__INCLUDED_)
#define AFX_EVENTAPP_H__99BEF319_FC2C_4761_B8B3_A968D4E6530C__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "EventKb.h"

class CEventApp  
{
public:
	CEventApp();
	virtual ~CEventApp();

protected:
	virtual int AddEventList(SEventKB const *pList, CEventKb *pKb);
};

#endif // !defined(AFX_EVENTAPP_H__99BEF319_FC2C_4761_B8B3_A968D4E6530C__INCLUDED_)
