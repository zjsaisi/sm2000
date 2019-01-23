// SSMVote.h: interface for the CSSMVote class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: SSMVote.h 1.1 2008/05/02 16:36:01PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#if !defined(AFX_SSMVOTE_H__71C43307_77D0_49B7_BB2D_D49F47AA00B4__INCLUDED_)
#define AFX_SSMVOTE_H__71C43307_77D0_49B7_BB2D_D49F47AA00B4__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "pv.h"

class CSSMVote  
{
public:
	void InitSsm(int init);
	int Update(int ql);
	CSSMVote();
	virtual ~CSSMVote();
private:
	int m_init; // SSM QL if valid QL is never seen
	int m_currentQL;
	int32 m_age;
	enum { FILTER_SIZE = 3 };
	int m_qlList[FILTER_SIZE];
	int m_index;
	static CPVOperation *m_pPV;
};

#endif // !defined(AFX_SSMVOTE_H__71C43307_77D0_49B7_BB2D_D49F47AA00B4__INCLUDED_)
