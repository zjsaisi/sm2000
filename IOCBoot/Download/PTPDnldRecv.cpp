// PTPDnldRecv.cpp: implementation of the CPTPDnldRecv class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldRecv.cpp 1.2 2009/05/05 16:15:22PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */

#include "PTPDnldRecv.h"
#include "PTPLink.h"
#include "MemBlock.h"
#include "Storage.h"
#include "errorcode.h"
#include "Loader.h"
#include "stdio.h"
#include "PTPDnldProcess.h"
#include "AllProcess.h"

#define HARDWARE_REVISION CAllProcess::GetHardwareRevision()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPTPDnldRecv::CPTPDnldRecv(CStorage *pStorage, uint32 maxSize, 
		uint16 cmd, CPTPLink *pLink, uint8 outPort)
{
	m_offset = 0L;
	m_pLink = pLink;
	m_outPort = outPort;
	m_pStorage = pStorage;
	// m_pLoader = pLoader;
	m_cmdId = cmd;
	m_maxSize = maxSize;
	m_timer = 0;
	m_total = 0L;
}

CPTPDnldRecv::~CPTPDnldRecv()
{

}

// 1 for success;
int CPTPDnldRecv::ReceivePacket(CMemBlock *pBlock)
{	
	char buff[100];
	uint16 size;
	SDnldCmd *pCmd;
	SDnldResp resp;
	int32 left;
	int len;
	int ret;

	size = pBlock->GetCurSize();
	pCmd = (SDnldCmd *)pBlock->GetBuffer();

	resp.cmd = pCmd->cmd;
	resp.handle = pCmd->handle;
	resp.error_code = 1;
	resp.offset = pCmd->offset;

	// Integrity checking
	if (size != sizeof(SDnldCmd)) {
		resp.error_code = ERROR_DNLD_UNEXPECTED_FRAME_LENGTH;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);	
		sprintf(buff, "Unexpected frame length[%d]\r\n", size);
		CPTPDnldProcess::DebugOutput(buff);
		return -1;
	}
	if (pCmd->cmd != m_cmdId) {
		resp.error_code = ERROR_UNKNOWN_COMMAND;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		sprintf(buff, "Invalid command[0x%X]\r\n", pCmd->cmd);
		CPTPDnldProcess::DebugOutput(buff);
		return -2;
	}
	if (pCmd->total > m_maxSize) {
		resp.error_code = ERROR_DNLD_TOO_BIG;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		sprintf(buff, "Total size too big[0x%lX]\r\n", pCmd->total);
		CPTPDnldProcess::DebugOutput(buff);
		return -3;
	}
	if (pCmd->offset >= pCmd->total) {
		resp.error_code = ERROR_DNLD_INVALID_OFFSET;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		sprintf(buff, "Invalid Offset[0x%lX]\r\n", pCmd->offset);
		CPTPDnldProcess::DebugOutput(buff);
		return -4;
	}

	left = pCmd->total - pCmd->offset;
	len = (left > DNLD_PACKET_SIZE) ? (DNLD_PACKET_SIZE) : (left);
	
	if (pCmd->offset == 0L) {		
		// Check the header info
		{
			char *ptr;
			int failed;
			ptr = (char *)pCmd->content;

#if 0
			failed = strcmp(ptr + 6, "DTI090-93121-01");
			if (!failed) {
				char hardwareRev[10];
				sprintf(hardwareRev, " %d ", HARDWARE_REVISION);
				if (strstr(ptr + 38, hardwareRev) == NULL) 
					failed = 1;
			}
#else
			failed = CAllProcess::CheckDldHeader((char *)pCmd->content);
#endif
			if ((failed) || (*ptr != 0x25)) {
				resp.error_code = ERROR_DNLD_INVALID_DLD;
				m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
				CPTPDnldProcess::DebugOutput("Download: Invalid Header\r\n");
				return -10;
			}
		}

		ret = m_pStorage->OpenStorage((uint32)this);
		if (ret <= 0) {
			const char *ptr;
			resp.error_code = ERROR_DNLD_BUSY;
			m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
			ptr = "Storage busy, access denied\r\n";
			CPTPDnldProcess::DebugOutput(ptr);
			// CloseStorage();
			return -5;
		}
		m_pStorage->Rewind();
		m_offset = 0L;
		m_timer = 1;
		m_total = pCmd->total;
	}
	
	if (pCmd->total != m_total) {
		resp.error_code = ERROR_DNLD_INCONSISTENT_SIZE;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		sprintf(buff, "Unexpected size (Expect:0x%lX R:0x%lX)\r\n", 
			m_total, pCmd->total);
		CPTPDnldProcess::DebugOutput(buff);
		return -6;
	}
	if (pCmd->offset != m_offset) {
		resp.error_code = ERROR_DNLD_INVALID_OFFSET;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		sprintf(buff, "Unexpected offset[Expect=0x%lX R: 0x%lX] in Receiver\r\n",
			m_offset, pCmd->offset);
		CPTPDnldProcess::DebugOutput(buff);
		return -7;
	}

	m_timer = 1;

	m_offset += len;
	ret = m_pStorage->Write((char *)pCmd->content, len);
	if (ret <= 0) {
		// Write to flash error
		resp.error_code = ERROR_DNLD_WRITE_FLASH;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		CloseStorage();
		return -8;
	}

	if (left <= DNLD_PACKET_SIZE) {
		// Last packet. Extra checking

		sprintf(buff, "Last packet received [OFFSET=0x%lX, %d], Checking integrity ... ", 
			m_offset - len, len);
		CPTPDnldProcess::DebugOutput(buff);

		CloseStorage();

		ret = m_pStorage->CheckIntegrity();
		if (ret <= 0) {
			resp.error_code = ERROR_DNLD_INVALID_DLD;
			m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
			
			CPTPDnldProcess::DebugOutput("Failed\r\n");

			return -9;
		}		
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		CPTPDnldProcess::DebugOutput("OK\r\n");
		m_pStorage->EndOfStorage();  // Might reboot the card if it's code.
		return 1;
	} else {
		sprintf(buff, "Receiveing packet[OFFSET=0x%lX Len=%d]\r\n", m_offset - len, 
			len);
		CPTPDnldProcess::DebugOutput(buff);
	}

	m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
 	return 1;
}

void CPTPDnldRecv::TickTime(int seconds)
{
	int ret;
	if (m_timer != 0) { 
		m_timer += seconds;
		if (m_timer > 32) {
			ret = m_pStorage->CloseStorage((uint32)this);
			if (ret <= 0) return;
			m_pStorage->Rewind();
			m_offset = 0L;
			m_timer = 0;
		}
	}
}

void CPTPDnldRecv::CloseStorage()
{
	int ret;
	ret = m_pStorage->CloseStorage((uint32)this);
	if (ret <= 0) return;

	m_offset = 0L;
	m_timer = 0;
	m_pStorage->Rewind();
}
