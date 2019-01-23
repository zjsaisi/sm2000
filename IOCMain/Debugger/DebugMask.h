// DebugMask.h: interface for the CDebugMask class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * RCS: $Header: DebugMask.h 1.2 2008/05/15 15:47:15PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_DEBUGMASK_H__163EE0C3_24CE_47C7_8DB0_06C813D5B7B4__INCLUDED_)
#define AFX_DEBUGMASK_H__163EE0C3_24CE_47C7_8DB0_06C813D5B7B4__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"
#include "string.h"

class CDebugMask  
{
public:
	void DebugHack(Ccomm *pComm, unsigned long *param);
	int CheckMask(uint32 mask);
	void Init(void);
	CDebugMask();
	virtual ~CDebugMask();
	enum { 
		RUBIDIUM_GROUP = 17,
		ISTATE_GROUP = 18, 
		CARD_GROUP = 19,
		OUTPUT_GROUP = 20,
		EVENT_GROUP =21, 
		PTP_GROUP = 22, // Goes to CPTPLink
		PTP_DOWNLOAD_GROUP = 23,
		HOUSE_GROUP = 25,
		SECOND_GROUP = 26,
		MINUTE_GROUP = 27,
		LAST_GROUP_NOT_USED
		};
private:
	void CountRequest(uint32 mask);
	void SetDebugOption(uint32 d_and, uint32 d_or, Ccomm *pComm = NULL);
	uint32 m_mask;
	uint32 m_count[32];
};

#endif // !defined(AFX_DEBUGMASK_H__163EE0C3_24CE_47C7_8DB0_06C813D5B7B4__INCLUDED_)
