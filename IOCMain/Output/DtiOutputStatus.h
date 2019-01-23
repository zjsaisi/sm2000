// DtiOutputStatus.h: interface for the CDtiOutputStatus class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputStatus.h 1.1 2007/12/06 11:41:36PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_DTIOUTPUTSTATUS_H__87724C7C_2239_43FA_B490_02A9415044CB__INCLUDED_)
#define AFX_DTIOUTPUTSTATUS_H__87724C7C_2239_43FA_B490_02A9415044CB__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CDtiOutputStatus  
{
public:
	void ReportDtiVersion(int version);
	void ReportClockType(int type);
	static int GotProblem(void);
	void ReportTesting(int test);
	void ReportPerformanceStable(int stable);
	void ReportCableAdvanceValid(int valid);
	void ReportCableDelay(int32 lsb);
	void RefreshLoopStatus(void);
	void ReportLoopStatus(int loop);
	void ReportConnect(int connected);
	CDtiOutputStatus(int chan);
	virtual ~CDtiOutputStatus();
private:
	enum { OUTPUT_PROBLEM_DISC = 1, OUTPUT_PROBLEM_LOOP = 2,
		OUTPUT_PROBLEM_CABLE_ADVANCE = 4, OUTPUT_PROBLEM_PERFORMANCE = 8,
		OUTPUT_NO_PROBLEM = 0
	};
	void ExitLoop(int loop);
	void EnterLoop(int loop);
	int m_chan;
	int m_problem;
	int m_connected;
	int m_loopStatus;
	int m_cableAdvanceValid;
	int m_outputPerformanceStable;
	int m_testing;
	int m_clockType;
	int m_dtiVersion;
	static CDtiOutputStatus *s_pDtiOutputStatus[NUM_OUTPUT_CHAN];
};

#endif // !defined(AFX_DTIOUTPUTSTATUS_H__87724C7C_2239_43FA_B490_02A9415044CB__INCLUDED_)
