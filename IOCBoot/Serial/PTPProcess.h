// PTPProcess.h: interface for the CPTPProcess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPProcess.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: PTPProcess.h 1.1 2007/12/06 11:39:17PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_PTPPROCESS_H__E804EA60_D28F_4F87_8591_C112CEF052D4__INCLUDED_)
#define AFX_PTPPROCESS_H__E804EA60_D28F_4F87_8591_C112CEF052D4__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "BaseProcess.h"

class CPTPLink;

class CPTPProcess : public CBaseProcess 
{
public:
	// Entry1 & Entry1 are the same. For IOC, we need communication 
	// to IMC and the other IOC. There are two CPTPProcess object.
	// One should use Entry1. The other should use Etnry2
	static void Entry2(void);
	static void Entry1(void);
	// pLink points to CPTPLink object.
	// pName points to name of the process.
	// priority is priority of the process.
	// stack_size is the size of the stack
	// entry should be either Entry1 or Entry2
	CPTPProcess(CPTPLink *pLink, const char *pName, uint8 priority, 
		uint16 stack_size, FUNC_PTR entry);
	virtual ~CPTPProcess();

private:
	void entry(void);
private:
	CPTPLink *m_pLink;
protected:
	void EventDispatch(uint16 event);
	void EventTimeOut(void);
};

#endif // !defined(AFX_PTPPROCESS_H__E804EA60_D28F_4F87_8591_C112CEF052D4__INCLUDED_)
