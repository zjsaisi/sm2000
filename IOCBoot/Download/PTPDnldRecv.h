// PTPDnldRecv.h: interface for the CPTPDnldRecv class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldRecv.h 1.1 2007/12/06 11:39:08PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#if !defined(AFX_PTPDNLDRECV_H__53524D0A_6137_4603_A2B9_4C0CAFE378EC__INCLUDED_)
#define AFX_PTPDNLDRECV_H__53524D0A_6137_4603_A2B9_4C0CAFE378EC__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"

class CPTPLink;
class CMemBlock;
class CStorage;
class CLoader;

class CPTPDnldRecv  
{
public:
	void TickTime(int seconds);
	enum { DNLD_PACKET_SIZE = 1024 };
	int ReceivePacket(CMemBlock *pBlock);
	CPTPDnldRecv(CStorage *pStorage, uint32 maxSize, uint16 cmd,  
		CPTPLink *pLink, uint8 outPort);
	virtual ~CPTPDnldRecv();
	typedef struct {
		uint16 cmd; // Always CMD_DOWNLOAD
		uint32 handle;
		uint32 total;
		uint32 offset;
		uint8 content[DNLD_PACKET_SIZE];
	} SDnldCmd;
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error_code;
		uint32 offset;
	} SDnldResp;
private:
	void CloseStorage(void);
	// CLoader *m_pLoader;
	CStorage *m_pStorage;
	CPTPLink *m_pLink;
	uint32 m_offset;
	uint32 m_total;
	uint32 m_maxSize;
	uint16 m_cmdId;
	uint8 m_outPort;
	int m_timer;
};

#endif // !defined(AFX_PTPDNLDRECV_H__53524D0A_6137_4603_A2B9_4C0CAFE378EC__INCLUDED_)
