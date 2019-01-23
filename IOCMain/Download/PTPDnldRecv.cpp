// PTPDnldRecv.cpp: implementation of the CPTPDnldRecv class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldRecv.cpp 1.3 2009/04/29 17:27:49PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
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
                 
#define TRACE TRACE_PTP_DNLD
#define HARDWARE_REVISION  CAllProcess::GetHardwareRevision()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// If maxSize is < 1024, Use one packet to hold it.
CPTPDnldRecv::CPTPDnldRecv(CStorage *pStorage, uint32 maxSize, 
		uint16 cmd, CPTPLink *pLink, uint8 outPort, uint8 bCheckHeader /* = 0 */)
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
	m_bCheckHeader = bCheckHeader;
	m_delayedWriting = 0;
	m_delayedError = 0;
}

CPTPDnldRecv::~CPTPDnldRecv()
{

}

// 1 for success;
// 2 for last one received.
int CPTPDnldRecv::ReceivePacket(CMemBlock *pBlock)
{	
	//char buff[100];
	uint16 size;
	SDnldBigCmd *pCmd;
	SDnldResp resp;
	int32 left;
	int len;
	int ret;
	uint16 expectedSize;
	int8 delayedWriting = m_delayedWriting;

	size = pBlock->GetCurSize();
	pCmd = (SDnldBigCmd *)pBlock->GetBuffer();

	resp.cmd = pCmd->cmd;
	resp.handle = pCmd->handle;
	resp.error_code = 1;
	resp.offset = pCmd->offset;

	expectedSize = (m_maxSize >= BIG_DNLD_PACKET_SIZE) ? (sizeof(SDnldBigCmd)) :
		(sizeof(SDnldSmallCmd));
	// Integrity checking
	if (size != expectedSize) {
		resp.error_code = ERROR_DNLD_UNEXPECTED_FRAME_LENGTH;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);	
		TRACE("Unexpected frame length[%d]\r\n", size);
		return -1;
	}
	if (pCmd->cmd != m_cmdId) {
		resp.error_code = ERROR_UNKNOWN_COMMAND;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("Invalid command[0x%X]\r\n", pCmd->cmd);
		return -2;
	}
	if (pCmd->total > m_maxSize) {
		resp.error_code = ERROR_DNLD_TOO_BIG;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("Total size too big[0x%lX]\r\n", pCmd->total);
		return -3;
	}
	if (pCmd->offset >= pCmd->total) {
		resp.error_code = ERROR_DNLD_INVALID_OFFSET;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("Invalid Offset[0x%lX]\r\n", pCmd->offset);
		return -4;
	}

	left = pCmd->total - pCmd->offset;
	len = (left > BIG_DNLD_PACKET_SIZE) ? (BIG_DNLD_PACKET_SIZE) : (left);
	
	if (pCmd->offset == 0L) {	
		// Check the header info
		if (m_bCheckHeader == 1) { 
			char *ptr;
			int failed;

			ptr = (char *)pCmd->content;
			// Check header
#if 0
			failed = strcmp(ptr + 6, "DTI090-93121-01");
			// Check if the software support this hardware
			if (!failed) {
				char hardwareRev[10];
				sprintf(hardwareRev, " %d ", HARDWARE_REVISION);
				if (strstr(ptr + 38, hardwareRev) == NULL) 
					failed = 1;

			}
#else
			failed = CAllProcess::CheckDldHeader(ptr);
#endif
			if ((failed) || (*ptr != 0x25)) {
				resp.error_code = ERROR_DNLD_INVALID_DLD;
				m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
				TRACE("Download: Invalid Header\r\n");
				return -10;
			}
		}

		ret = m_pStorage->OpenStorage((uint32)this);
		if (ret <= 0) {
			const char *ptr;
			resp.error_code = ERROR_DNLD_BUSY;
			m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
			ptr = "Storage busy, access denied\r\n";
			TRACE(ptr);
			// CloseStorage();
			return -5;
		}
		m_pStorage->Rewind();
		m_offset = 0L;
		m_timer = 1;
		m_total = pCmd->total;
		m_delayedError = 0;
	}
	
	if (pCmd->total != m_total) {
		resp.error_code = ERROR_DNLD_INCONSISTENT_SIZE;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("Unexpected size (Expect:0x%lX R:0x%lX)\r\n", 
			m_total, pCmd->total);
		return -6;
	}
	if (pCmd->offset != m_offset) {
		resp.error_code = ERROR_DNLD_INVALID_OFFSET;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("Unexpected offset[Expect=0x%lX R: 0x%lX] in Receiver\r\n",
			m_offset, pCmd->offset);
		return -7;
	}

	m_timer = 1;

	m_offset += len;

	if ((delayedWriting != 1) || (left <= BIG_DNLD_PACKET_SIZE) || (m_delayedError)) {
		if (m_delayedError) {
			ret = -1;
		} else {
			ret = m_pStorage->Write((char *)pCmd->content, len);
		}
		if (ret <= 0) {
			// Write to flash error
			resp.error_code = ERROR_DNLD_WRITE_FLASH;
			m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
			CloseStorage();
			return -8;
		}
		delayedWriting = 0;
	}

	if (left <= BIG_DNLD_PACKET_SIZE) {
		// Last packet. Extra checking
		TRACE("Last packet received [OFFSET=0x%lX, %d], Checking integrity ... ", 
			m_offset - len, len);
		CloseStorage();

		ret = m_pStorage->CheckIntegrity();
		if (ret <= 0) {
			resp.error_code = ERROR_DNLD_INVALID_DLD;
			m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
			
			TRACE("Failed\r\n");

			return -9;
		}		
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		TRACE("OK\r\n");
		m_pStorage->EndOfDownload();  // Might reboot the card if it's code. For RTE Istate, it should be taken into effect.
		m_pStorage->Rewind();
		return 2;
	} else {
		TRACE("Receiveing packet[OFFSET=0x%lX Len=%d]\r\n", 
			m_offset - len,	len);
	}

	m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
 	
	if (delayedWriting == 1) {
		ret = m_pStorage->Write((char *)pCmd->content, len);
		if (ret <= 0) {
			m_delayedError = 1;
		}
	
	}
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
	// m_pStorage->Rewind();
}

// By sending response back first and then to flash writing will speed things up a little bit.
void CPTPDnldRecv::SetDelayedWriting(int8 delayed)
{
	m_delayedWriting = (delayed) ? (1) : (0);
}
