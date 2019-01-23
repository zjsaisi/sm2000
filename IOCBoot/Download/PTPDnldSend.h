// PTPDnldSend.h: interface for the CPTPDnldSend class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldSend.h 1.1 2007/12/06 11:39:08PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */
#if !defined(AFX_PTPDNLDSEND_H__200040AF_E654_4C01_BB05_4BDE357FB6B9__INCLUDED_)
#define AFX_PTPDNLDSEND_H__200040AF_E654_4C01_BB05_4BDE357FB6B9__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"

class CPTPLink;
class CLoader;
class CMemBlock;
class CStorage;

class CPTPDnldSend  
{
public:
	void TickTime(int seconds);
	int ReceivePacket(CMemBlock *pBlock);
	void StartSendCode(void);
	CPTPDnldSend(CStorage *pStorage, uint16 cmd, CPTPLink *pLink, uint8 outPort);
	virtual ~CPTPDnldSend();
private:
	void CloseStorage(void);
	int SendPacket(void);
	uint8 m_outPort;
	CPTPLink *m_pLink;
	CStorage *m_pStorage;
	uint32 m_total;
	uint32 m_offset;
	uint16 m_cmdId;
	int m_timer;

};

#endif // !defined(AFX_PTPDNLDSEND_H__200040AF_E654_4C01_BB05_4BDE357FB6B9__INCLUDED_)
