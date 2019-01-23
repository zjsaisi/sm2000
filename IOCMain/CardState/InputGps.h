// InputGps.h: interface for the CInputGps class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputGps.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputGps.h 1.5 2011/04/22 10:43:29PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.5 $
 */             


#if !defined(AFX_INPUTGPS_H__7F1CC728_F07D_4B95_AF30_F2809D5EA2F9__INCLUDED_)
#define AFX_INPUTGPS_H__7F1CC728_F07D_4B95_AF30_F2809D5EA2F9__INCLUDED_

#include "Input.h"

class CInputGps : public CInput  
{
public:
	void SetIgnoreImc(int ignore);
	virtual int InputHack(unsigned long *param, Ccomm *pComm);
	CInputGps(int chan, const char *pName);
	virtual ~CInputGps();
	static CInputGps *s_pInputGps;
protected:
	virtual int ReadStatus(void);
	virtual void InitChan(void);
protected:
	virtual int InputSetup(void);
	int m_assumedQL;

private:
	int m_ignoreImc;
public:
	virtual int GetCurrentQL(void);
	virtual int CfAssumedQLChanInput(int assumed);
	virtual int GetPriority(void);
#if 0 
private:
	int m_taTuneError;
	int m_tpiuTuneError;
	int m_gpsPathCrcError;
	void CountTuneCrcError(void);
#endif
};

#endif // !defined(AFX_INPUTGPS_H__7F1CC728_F07D_4B95_AF30_F2809D5EA2F9__INCLUDED_)
