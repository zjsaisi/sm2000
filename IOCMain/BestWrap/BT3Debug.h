// BT3Debug.h: interface for the CBT3Debug class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: BT3Debug.h
 * Author: Zheng Miao
 * All rights reserved.
 * RCS: $Header: BT3Debug.h 1.1 2007/12/06 11:41:04PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_BT3DEBUG_H__0CEA391E_125F_4336_9F89_FB958319F7EB__INCLUDED_)
#define AFX_BT3DEBUG_H__0CEA391E_125F_4336_9F89_FB958319F7EB__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class Ccomm;

class CBT3Debug  
{
protected:
	CBT3Debug();
	virtual ~CBT3Debug();
public:
	void PrintBT3Status(unsigned long *param, Ccomm *pComm);
	static CBT3Debug *s_pBT3Debug;
	static void Create(void);

private:
	void PrintMpll(Ccomm *pComm);
	void PrintSpanFifo(Ccomm *pComm, struct Span_Meas_Data *pData);
	void Print(const char *str);
	void PrintBT3Help(void);
	Ccomm *m_port;
};

#endif // !defined(AFX_BT3DEBUG_H__0CEA391E_125F_4336_9F89_FB958319F7EB__INCLUDED_)
