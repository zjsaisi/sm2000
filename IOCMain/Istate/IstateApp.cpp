// IstateApp.cpp: 
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateApp.cpp 1.2 2008/01/09 15:31:45PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */


#include "CodeOpt.h"
#include "DataType.h"
#include "IstateApp.h"
#include "IstateKb.h"
#include "string.h"

CIstateApp::CIstateApp()
{
}

CIstateApp::~CIstateApp()
{
}

int CIstateApp::AddIstateList(CIstateKb *pKb, SIstateItem const *pList)
{
	while (pList != NULL) {
		if (pList->pIstateDesc == NULL) return 1;
		if (pList->key <= 0) return 1;
		pKb->AddIstateItemDesc(pList->key, pList->pIstateDesc);
		pList++;		
	}
	return 1;
}
