// Amd29DL161DB.h: interface for the CAmd29DL161DB class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Amd29DL161DB.h 1.1 2007/12/06 11:38:50PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */             

#if !defined(AFX_AMD29DL161DB_H__00ADAC10_089A_44BC_AC19_BCA99CA77F64__INCLUDED_)
#define AFX_AMD29DL161DB_H__00ADAC10_089A_44BC_AC19_BCA99CA77F64__INCLUDED_

#include "codeopt.h"
#include "Amd29B16.h"

class CAmd29DL161DB : public CAmd29B16  
{
public:
	enum {SECT_NUM = 39 };
	CAmd29DL161DB(unsigned long start, CCheapTimer *pTimer, CPVOperation *pPV);
	virtual ~CAmd29DL161DB();
	static const unsigned long m_amd29DL161dbSectorList[(SECT_NUM + 1)*2];                     
private:
	enum {DEVICE_ID = 0x2239 };
	long m_eraseDelay[SECT_NUM + 1];
};

#endif // !defined(AFX_AMD29DL161DB_H__00ADAC10_089A_44BC_AC19_BCA99CA77F64__INCLUDED_)
