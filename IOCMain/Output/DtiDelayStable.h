// DtiDelayStable.h: interface for the CDtiDelayStable class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiDelayStable.h 1.1 2007/12/06 11:41:35PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_DTIDELAYSTABLE_H__6F36B850_612D_48B2_915F_47D73928D325__INCLUDED_)
#define AFX_DTIDELAYSTABLE_H__6F36B850_612D_48B2_915F_47D73928D325__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CDtiDelayStable  
{
public:
	void Reset(void);
	int32 Feed(int32 data);
	static void Init(void);
	static void Create(void);
	static CDtiDelayStable *s_pDtiDelayStable[NUM_OUTPUT_CHAN];
protected:
	CDtiDelayStable(int chan);
	virtual ~CDtiDelayStable();
private:
	enum { NUM_DATA = 6,
		// Data point we will check
		MINI_NUM = 3, MAX_NUM =3 };
	int m_chan;
	int32 m_data[NUM_DATA];
	int32 m_diff[NUM_DATA];
	int m_first;
	int m_last;
};

#endif // !defined(AFX_DTIDELAYSTABLE_H__6F36B850_612D_48B2_915F_47D73928D325__INCLUDED_)
