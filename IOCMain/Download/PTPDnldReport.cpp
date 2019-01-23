// PTPDnldReport.cpp: implementation of the CPTPDnldReport class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldReport.cpp 1.1 2007/12/06 11:41:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#include "PTPDnldReport.h"
#include "PTPLink.h"
#include "MEMBlock.h"
#include "Storage.h"
#include "errorcode.h"
#include "PTPDnldProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACE TRACE_PTP_DNLD

CPTPDnldReport::CPTPDnldReport(CStorage *pStorage, uint16 cmd, CPTPLink *pLink, 
	uint8 outPort, uint16 frameSize)
{
	m_pStorage = pStorage;
	m_cmd = cmd;
	m_pLink = pLink;
	m_outPort = outPort;
	frameSize = (frameSize < BIG_FRAME_SIZE) ? (SMALL_FRAME_SIZE) : (BIG_FRAME_SIZE);
	m_frameSize = frameSize;
	m_totalSize = sizeof(SBigResp) - BIG_FRAME_SIZE + m_frameSize;
}

CPTPDnldReport::~CPTPDnldReport()
{

}

// return 1: OK
// -1: resource busy
// -2: Invalid offset
// -3: Invalid command frame size. Ignore the command.
int CPTPDnldReport::ReceivePacket(CMemBlock *pBlock)
{
	SCmd *pCmd;
	SBigResp respBuff;
	SBigResp *pBigResp;
	SSmallResp *pSmallResp;
	uint32 offset;
	int ret;
	uint32 dataSize = 0L;

	ret = pBlock->GetCurSize();
	if (ret != sizeof(SCmd)) {
		TRACE("Invalid Size for %s report\r\n", m_pStorage->GetName());  
		return -3;
	}

	pCmd = (SCmd *)(pBlock->GetBuffer());
	pBigResp = &respBuff;
	pSmallResp = (SSmallResp *)&respBuff;

	memset(&respBuff, 0, sizeof(respBuff));
	pBigResp->cmd = pCmd->cmd;
	pBigResp->handle = pCmd->handle;
	pBigResp->offset = offset = pCmd->offset;
	pBigResp->error_code = 1;
	pBigResp->size = 0;

	ret = m_pStorage->OpenStorage((uint32)this);
	if (ret != 1) {
		pBigResp->error_code = ERROR_DNLD_BUSY; // Resource busy
		m_pLink->PostFrame((char *)&respBuff, m_totalSize, m_outPort);
		return -1;
	}
	if (offset == 0L) {
		m_pStorage->StartOfCollection();
		TRACE("Start of %s report\r\n", m_pStorage->GetName());
	}
	m_pStorage->CheckIntegrity(&dataSize);
	pBigResp->size = dataSize;
	m_pStorage->Rewind();
	ret = m_pStorage->Seek(offset);
	if ((ret != 1) || (offset >= dataSize)) {
		pBigResp->error_code = ERROR_DNLD_INVALID_OFFSET;
		m_pStorage->CloseStorage((uint32)this);
		m_pLink->PostFrame((char *)&respBuff, m_totalSize, m_outPort);
		return -2; // Invalid offset
	}

	ret = m_pStorage->Read((char *)pBigResp->content, m_frameSize);
	m_pStorage->CloseStorage((uint32)this);
	m_pLink->PostFrame((char *)&respBuff, m_totalSize, m_outPort);
	return 1;
}
