// IstateAccess.cpp: implementation of the CIstateAccess class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: IstateAccess.cpp 1.2 2008/01/29 11:29:26PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

#include "IstateAccess.h"
#include "PTPLink.h"
#include "IstateProcess.h"
#include "errorcode.h"
#include "Istate.h"
#include "IstateKb.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateAccess::CIstateAccess(CIstateProcess *pIstateProcess, /* CCardState *pCardState,*/ CIstate *pIocIstate, CPTPLink *pImcLink)
{
	m_pIstateProcess = pIstateProcess;
	m_pIocIstate = pIocIstate;
	m_pImcLink = pImcLink;
	//m_pCardState = pCardState;
	m_pKB = m_pIocIstate->GetKB();
}

CIstateAccess::~CIstateAccess()
{

}

// return 1 OK. 
// This should only be called by IstateProcess.
int CIstateAccess::ExecuteImcAccess(const char *ptr, int len)
{
	SCmdGeneral *pCmd; 
	SRespGeneral resp;
	int ret;
	uint16 flag;

	pCmd = (SCmdGeneral *)ptr;
	resp.cmd = pCmd->cmd;
	resp.handle = pCmd->handle;

	switch (pCmd->cmd) {
	case CMD_SET_ISTATE: {
		SCmdSetIstate *pSet;
		SRespSetIstate setResp;
		pSet = (SCmdSetIstate *)pCmd;
		setResp.cmd = pSet->cmd;
		setResp.handle = pSet->handle;

		// For most Istate writing, it should be sent to active card.
		// For now, there is no easy way to do so.
#if 0
		int cardState = m_pCardState->GetCardState();
		if (cardState != CCardState::CARD_STATE_ACTIVE) {
		}
#endif
		flag = m_pKB->GetIstateFlag(pSet->istate_id); 
		if (flag & FLAG_IMC_READ_ONLY_ISTATE) { // Don't allow it written by PPC.
			setResp.error = ERROR_READ_ONLY_ISTATE;
		} else {
			setResp.error = ret = m_pIstateProcess->WriteOneIOCIstateItem(pSet->istate_id, 
				pSet->index1, pSet->index2, pSet->value, CMD_WRITE_ISTATE_FROM_IMC);
		}
		if (!(flag & FLAG_SILENT_ISTATE)) {
			m_pImcLink->PostFrame((char *)&setResp, sizeof(setResp), LINK_IOCISTATE_ACCESS_IMC); 
		}
						 }
		break;
	case CMD_GET_ISTATE: {
		SCmdGetIstate *pGet;
		SRespGetIstate getResp;
		pGet = (SCmdGetIstate *)pCmd;
		getResp.cmd = pGet->cmd;
		getResp.handle = pGet->handle;
		getResp.istate_id = pGet->istate_id;
		getResp.index1 = pGet->index1;
		getResp.index2 = pGet->index2;
		getResp.error = ret = m_pIocIstate->ReadOneIstateItem(pGet->istate_id, pGet->index1, pGet->index2,
			&(getResp.value));
		m_pImcLink->PostFrame((char *)&getResp, sizeof(getResp), LINK_IOCISTATE_ACCESS_IMC);
						}
		break;
	default:
		// Unknown command;
		resp.error = ERROR_UNKNOWN_COMMAND;
		m_pImcLink->PostFrame((char *)&resp, sizeof(resp), LINK_IOCISTATE_ACCESS_IMC);
		return ERROR_UNKNOWN_COMMAND;
	}
	return ret;
}
