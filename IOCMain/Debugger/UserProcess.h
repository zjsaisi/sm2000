// UserProcess.h: interface for the CUserProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: UserProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: UserProcess.h 1.1 2007/12/06 11:41:18PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_)
#define AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_


#include "BaseProcess.h"

class Ccomm;
class CFlash;
class CDebugDL;
class CLoader;

class CUserProcess : public CBaseProcess  
{
public:
	void Init(void);
	CUserProcess(Ccomm *pUserComm, Ccomm *pDebugComm, CLoader *pLoader, CFlash *pFlash,
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CUserProcess();
	CDebugDL *GetDebugger(void) { return m_pDebug; };
private:
	void entry(void);
private:
	static void Entry(void);
	Ccomm *m_pUserComm;
	Ccomm *m_pDebugComm;
	CLoader *m_pLoader;
	CFlash *m_pFlash;
	CDebugDL *m_pDebug;
};

#endif // !defined(AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_)
