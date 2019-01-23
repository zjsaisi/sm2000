// PTPDnldSend.cpp: implementation of the CPTPDnldSend class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldSend.cpp 1.1 2007/12/06 11:39:08PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */
#include "PTPDnldSend.h"
#include "PTPLink.h"
#include "Loader.h"
#include "PTPDnldRecv.h"
#include "MemBlock.h"
#include "stdio.h"
#include "PTPDnldProcess.h"
#include "Flash.h"
#include "Storage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPTPDnldSend::CPTPDnldSend(CStorage *pStorage, uint16 cmd, CPTPLink *pLink, uint8 outPort)
{
	m_pLink = pLink;
	m_outPort = outPort;
	m_total = 0L;
	m_offset = 0L;
	m_cmdId = cmd;
	m_pStorage = pStorage;
	m_timer = 0;
}

CPTPDnldSend::~CPTPDnldSend()
{

}

void CPTPDnldSend::StartSendCode()
{
	int ret;
	int32 len;
	char buff[80];
	ret = m_pStorage->CheckIntegrity(&m_total);
	if (ret <= 0) {
		sprintf(buff, "No Main code available[%d]\r\n", ret);
		CPTPDnldProcess::DebugOutput(buff);
		return;
	}

	// Occupy storage
	ret = m_pStorage->OpenStorage((uint32)this);
	if (ret <= 0) {
		sprintf(buff, "Cannot open storage[%d]\r\n", ret);
		CPTPDnldProcess::DebugOutput(buff);
		return;
	}
	// Start the first packet.
	m_offset = 0L;
	m_timer = 1;
	m_pStorage->Rewind();
	SendPacket();

	len = (m_total > 1024) ? (1024) : (m_total); 
	sprintf(buff, "Sending Packet [OFFSET=0x%lX LEN=%ld]\r\n", m_offset, len);
	CPTPDnldProcess::DebugOutput(buff);
}

// 1: Packet OK, continue next block
// 2: Done
int CPTPDnldSend::ReceivePacket(CMemBlock *pBlock)
{
	int size;
	int32 len;
	char buff[100];
	CPTPDnldRecv::SDnldResp *pResp;

	
	size = pBlock->GetCurSize();
	pResp = (CPTPDnldRecv::SDnldResp *)(pBlock->GetBuffer());

	if (size != sizeof(CPTPDnldRecv::SDnldResp)) {
		sprintf(buff, "Unexpected response size %d\r\n", 
			size);
		CPTPDnldProcess::DebugOutput(buff);
		CloseStorage();
		return -1;
	}
	if (m_cmdId != pResp->cmd) {
		sprintf(buff, "Unexpected cmd[0x%X] receive in Sender\t\n", pResp->cmd);
		CPTPDnldProcess::DebugOutput(buff);
		CloseStorage();
		return -2;
	}
	if (pResp->error_code <= 0) {
		// Receiver report error
		sprintf(buff, "Error code received(%d)\r\n", pResp->error_code);
		CPTPDnldProcess::DebugOutput(buff);
		CloseStorage();
		return -3;
	}
	if (m_offset != pResp->offset) {
		// unexpected Offset. Download failed
		sprintf(buff, "Unexpected offset S:0x%lX R:0x%lX, Sending aborted\r\n", 
			m_offset, pResp->offset);
		CPTPDnldProcess::DebugOutput(buff);
		CloseStorage();
		return -4;
	}

	m_offset += CPTPDnldRecv::DNLD_PACKET_SIZE;
	if (m_offset >= m_total) {
		// End of sending
		sprintf(buff, "Sending %ld 0x%lX bytes done\r\n", m_total, m_total);
		CPTPDnldProcess::DebugOutput(buff);
		CloseStorage();
		return 2;
	}
	
	// Read packet and send it out
	SendPacket();

	len = m_total - m_offset;
	len = (len > 1024) ? (1024) : (len);
	sprintf(buff, "Sending Packet [OFFSET=0x%lX LEN=%ld]\r\n", m_offset, 
		len);
	CPTPDnldProcess::DebugOutput(buff);

	return 1;
}

int CPTPDnldSend::SendPacket()
{
	CPTPDnldRecv::SDnldCmd cmd;
	//char const *pSrc = (char *)0x810000;

	m_timer = 1;
	// Init cmd
	cmd.cmd = m_cmdId;
	cmd.handle = 0x0; // If IOC send file out, it doesn't use handle;
	cmd.total = m_total;

	cmd.offset = m_offset;
	//pSrc += m_offset;

	//m_pFlash->Open();
	//memcpy(cmd.content, pSrc, CPTPDnldRecv::DNLD_PACKET_SIZE);
	//m_pFlash->Close();
	m_pStorage->Read((char *)cmd.content, CPTPDnldRecv::DNLD_PACKET_SIZE);

	m_pLink->PostFrame((char *)&cmd, sizeof(cmd), m_outPort);

	return 1;
}

void CPTPDnldSend::TickTime(int seconds)
{
	if (m_timer != 0) {
		m_timer += seconds;
		if (m_timer > 32) {
			m_pStorage->CloseStorage((uint32)this);
			m_pStorage->Rewind();
			m_offset = 0L;
			m_timer = 0;
		}
	}
}

void CPTPDnldSend::CloseStorage()
{
	int ret;
	ret = m_pStorage->CloseStorage((uint32)this);
	if (ret <= 0) return;

	m_offset = 0L;
	m_timer = 0;
	m_pStorage->Rewind();
}
