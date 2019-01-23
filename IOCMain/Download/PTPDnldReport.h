// PTPDnldReport.h: interface for the CPTPDnldReport class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPDnldReport.h 1.1 2007/12/06 11:41:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#if !defined(AFX_PTPDNLDREPORT_H__DC7246F4_3233_4400_A9C3_F3F4B7A26D42__INCLUDED_)
#define AFX_PTPDNLDREPORT_H__DC7246F4_3233_4400_A9C3_F3F4B7A26D42__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"

class CStorage;
class CPTPLink;
class CMemBlock;
class CStorage;

class CPTPDnldReport  
{
public:
	int ReceivePacket(CMemBlock *pBlock);
	CPTPDnldReport(CStorage *pStorage, uint16 cmd, 
		CPTPLink *pLink, uint8 outPort, uint16 frameSize /* 1024 or 64 */);
	virtual ~CPTPDnldReport();
private:
	enum { BIG_FRAME_SIZE = 1024, SMALL_FRAME_SIZE = 64 };
	typedef struct {
		uint16 cmd;
		uint32 handle;
		uint32 offset;
	} SCmd;
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error_code;
		uint32 size;
		uint32 offset;
		uint8 content[BIG_FRAME_SIZE];
	} SBigResp;
	typedef struct {
		uint16 cmd;
		uint32 handle;
		int16 error_code;
		uint32 size;
		uint32 offset;
		uint8 content[SMALL_FRAME_SIZE];
	} SSmallResp;

	CStorage *m_pStorage;
	uint16 m_cmd;
	CPTPLink *m_pLink;
	uint8 m_outPort;
	uint16 m_frameSize;
	uint16 m_totalSize;
};

#endif // !defined(AFX_PTPDNLDREPORT_H__DC7246F4_3233_4400_A9C3_F3F4B7A26D42__INCLUDED_)
