// IstateApp.h: interface for the CIstateBasic class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateApp.h 1.1 2007/12/06 11:41:31PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(ISTATE_APP_H_08_25_2005)
#define ISTATE_APP_H_08_25_2005

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CIstateKb;

typedef struct {
	int16 key;
	void *pIstateDesc;
} SIstateItem;

// Base class for Istate application
class CIstateApp  
{
public:
	CIstateApp();
	virtual ~CIstateApp();
protected:
	int AddIstateList(CIstateKb *pKb, SIstateItem const *pList);
	
};

#endif // ISTATE_APP_H_08_25_2005

