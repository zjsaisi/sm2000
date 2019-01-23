// InputTod.h: interface for the CInputTod class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputTod.h 1.2 2008/03/11 10:12:54PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             

#if !defined(AFX_INPUTTOD_H__233AEC2F_FD84_420C_A314_4622EBEE4F79__INCLUDED_)
#define AFX_INPUTTOD_H__233AEC2F_FD84_420C_A314_4622EBEE4F79__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CInputTod  
{
public:
	static void ForceChange(void);
	void Init(void);
	int IsTodValid(void);
	void CardInactive(void);
	void InputTodHack(unsigned long *param, Ccomm *pComm);
	static void SetActiveInput(int chan);
	void TodInvalid(int isLos, int todState = 16);
	int ParseTod(const unsigned char *pMsg);
	CInputTod(int chan);
	virtual ~CInputTod();
private:
	void NoTod(int todState = 16);
	void PostTod(void);
	int m_chan;
	int m_index;
	static int m_activeInputTod;
	uint32 m_errorCnt;
	uint32 m_validCnt;
	uint8 m_todStatus;
	static int8 m_forceChange;
	uint32 m_gpsSec;

	int m_stableCnt;
	int m_isValid;

	int m_totalLeap;
	int m_leapPending;
	int32 m_localOffset;

	int m_totalLeapLast;
	int m_leapPendingLast;
	int32 m_localOffsetLast;

	int32 *m_pReportTimeSettingMode;
	int32 *m_pReportState;
};

#endif // !defined(AFX_INPUTTOD_H__233AEC2F_FD84_420C_A314_4622EBEE4F79__INCLUDED_)
