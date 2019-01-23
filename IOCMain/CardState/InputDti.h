// InputDti.h: interface for the CInputDti class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputDit.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputDti.h 1.1 2007/12/06 11:41:14PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             


#if !defined(AFX_INPUTDTI_H__6E01FA15_2C8B_414B_81FE_6ADE7428DFE3__INCLUDED_)
#define AFX_INPUTDTI_H__6E01FA15_2C8B_414B_81FE_6ADE7428DFE3__INCLUDED_

#include "Input.h"
#include "Fpga.h"
#include "InputPath.h"
#include "inputTod.h"

class CInputDti : public CInput  
{
public:
	virtual int IsLocked(void);
	virtual int InputHack(unsigned long *param, Ccomm *pComm);
	CInputDti(int chan, const char *pName);
	virtual ~CInputDti();

protected:
	int ParseTodMessage(void);
	int ParsePathInfo(void);
	virtual unsigned int InputTick(void);
	virtual int InputSetup(void);
	virtual int ReadStatus(void);
	virtual void InitChan(void);
private:
	void DoLed(void);
private:
	void ReportDeviceType(int active);
	unsigned char m_serverTrack;
	unsigned char m_serverFlag;
	unsigned char m_path[CFpga::MAX_PATH_SIZE + 5];
	unsigned char m_todMessage[CFpga::MAX_TOD_SIZE + 5];
	int m_inputDegraded;
	CInputPath m_pathReport;
	CInputTod  m_todReport;
	uint32 m_errorCnt;
	uint32 m_errorSecond;
	//int32 *m_pReportServerClockType;
	int m_serverClockType;
	//int32 *m_pReportServerExternalTimingSource;
	int m_serverExternalTimingSource;
	int32 *m_pReportServerLoopStatus;
	int32 *m_pReportCableAdvance;
};

#endif // !defined(AFX_INPUTDTI_H__6E01FA15_2C8B_414B_81FE_6ADE7428DFE3__INCLUDED_)
