// DebugProcess.h: interface for the DebugProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DebugProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DebugProcess.h 1.1 2007/12/06 11:41:17PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_)
#define AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_

#include "BaseProcess.h"
#include "flash.h"
#include "comm.h"
#include "DebugDL.h"
#include "loader.h"

class CDebugProcess : public CBaseProcess  
{
public:
	void Init(void);
	CDebugProcess(Ccomm *pPort, CLoader *pLoader, CFlash *pFlash, 
		const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CDebugProcess();
	CDebugDL *GetDebugger(void) { return m_pDebugger; };
private:
	static void Entry(void);
private:
	void entry(void);
	Ccomm *m_pPort;
	CLoader *m_pLoader;
	CFlash *m_pFlash;
	CDebugDL *m_pDebugger;
};

#endif // !defined(AFX_DEBUGPROCESS_H__EAA76919_AAC2_478F_9D52_B3C2BDCD6081__INCLUDED_)
