// IntQueue.h: interface for the CIntQueue class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IntQueue.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IntQueue.h 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_INTQUEUE_H__6B03DBFD_A5FE_42A0_946B_231B8C5CA1D7__INCLUDED_)
#define AFX_INTQUEUE_H__6B03DBFD_A5FE_42A0_946B_231B8C5CA1D7__INCLUDED_

#include "DataType.h"

class CIntQueue  
{
public:
	void DeleteTop(void);
	void Append(int16 n);
	int16 Top(void);
	CIntQueue(int16 size);
	virtual ~CIntQueue(); 
	enum { INVALID = -1 };
private:
	int16 *m_pQueue;
	volatile int16 m_head;
	volatile int16 m_tail;
	int16 m_size;
public:
	int8 m_bOverFlow;
	int8 m_bUnderFlow;
};

#endif // !defined(AFX_INTQUEUE_H__6B03DBFD_A5FE_42A0_946B_231B8C5CA1D7__INCLUDED_)
