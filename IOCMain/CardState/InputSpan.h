// InputSpan.h: interface for the CInputSpan class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputSpan.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputSpan.h 1.8 2011/07/08 15:35:09PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.8 $
 */             


#if !defined(AFX_INPUTSPAN_H__9B0A0BE2_DC72_410E_B399_601822550C02__INCLUDED_)
#define AFX_INPUTSPAN_H__9B0A0BE2_DC72_410E_B399_601822550C02__INCLUDED_

#include "Input.h"
#include "SSMVote.h"

class CInputSpan : public CInput  
{
public:
	virtual int InputHack(unsigned long *param, Ccomm *pComm);
	static CInputSpan *s_pInputSpan[NUM_SPAN_INPUT];
	static int CreateSpan();

	virtual int GetPriority(void);
	virtual int GetCurrentQL(void);

	// Called from Istate
	virtual int CfFrameChanInput(int frame); 
	virtual int CfCrc4ChanInput(int crc4);
	virtual int CfSsmBitChanInput(int ssmBit);
	virtual int CfSsmReadChanInput(int ssmRead);
	virtual int CfAssumedQLChanInput(int assumed);

protected:
	int CheckFrame(void);
	virtual int InputSetup(void);
	int CheckSsm(void);
	virtual unsigned int InputTick(void);

	int m_frame;
	int m_crc4;
	int m_ssmBit;
	int m_ssmRead;
	int m_assumedQL;
	int m_index;
	int m_currentQL;
	int m_prevSSM;
	CInputSpan(int chan, const char *pName);
	virtual ~CInputSpan();
	virtual int ReadStatus(void);
	virtual void InitChan(void);

	CSSMVote m_ssmVote;
};

#endif // !defined(AFX_INPUTSPAN_H__9B0A0BE2_DC72_410E_B399_601822550C02__INCLUDED_)
