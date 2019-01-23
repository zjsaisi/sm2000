// IstateAccess.h: interface for the CIstateAccess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateAccess.h 1.1 2007/12/06 11:41:31PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_ISTATEACCESS_H__DCEB8055_DFE1_4435_8C90_9B15EC3F879D__INCLUDED_)
#define AFX_ISTATEACCESS_H__DCEB8055_DFE1_4435_8C90_9B15EC3F879D__INCLUDED_

#include "DataType.h"
#include "CodeOpt.h"

class CPTPLink;
class CIstateProcess;
class CIstate;
class CIstateKb;

class CIstateAccess  
{
public:
	int ExecuteImcAccess(const char *ptr, int len);
	CIstateAccess(CIstateProcess *pIstateProcess, /* CCardState *pCardState, */
		CIstate *pIocIstate, CPTPLink *pImcLink);
	virtual ~CIstateAccess();
private:
	CIstate *m_pIocIstate;
	CIstateProcess *m_pIstateProcess;
	// CCardState *m_pCardState;
	CPTPLink *m_pImcLink;
	CIstateKb *m_pKB;
	enum { LINK_IOCISTATE_ACCESS_IMC = 0x30 };
	enum { CMD_SET_ISTATE= 0x131, CMD_GET_ISTATE = 0x130}; 
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 istate_id;
		int16 index1;
		int16 index2;
		uint32 value;
	} SCmdSetIstate;

	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 istate_id;
		int16 index1;
		int16 index2;
	} SCmdGetIstate;

	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error;
	} SRespSetIstate;

	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error;
		int16 istate_id;
		int16 index1;
		int16 index2;
		uint32 value;
	} SRespGetIstate;
};

#endif // !defined(AFX_ISTATEACCESS_H__DCEB8055_DFE1_4435_8C90_9B15EC3F879D__INCLUDED_)
