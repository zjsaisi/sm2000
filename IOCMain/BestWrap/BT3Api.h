// BT3Api.h: interface for the CBT3Api class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BT3API_H__0D85B188_AC6B_42C2_82E6_FF534C5713EB__INCLUDED_)
#define AFX_BT3API_H__0D85B188_AC6B_42C2_82E6_FF534C5713EB__INCLUDED_

#ifdef __cplusplus
#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CNVRAM;
class CBestCal;

class CBT3Api  
{
public:
	void ForcePhaseControl(int force);
	void PrintBT3Api(Ccomm *pComm);
	void SetPhaseControl(int phaseMode);   
	void StoreCalData(void);
	void Init(int isRubidium);
	static void BT3TickOneMinute(void);
	static void BT3TickOneSecond(void);
	CBT3Api(CNVRAM *pNVRAM);
	virtual ~CBT3Api();
	static CBT3Api *s_pBT3Api;
private:
	void DoPhaseControl(void);
	void CheckSmartClock(void);
	void CheckCalData(void);
	void InitFifo(void);
	void InitBt3Data(void);
	CBestCal *m_pBestCal;
	int m_smartClk;
	int m_toStoreCal;

	// phase control
	int m_bt3PhaseMode;
	int m_forceBt3PhaseMode;
	int m_realBt3PhaseMode;

};

extern struct MPLL_Meas_Data mpll;
extern struct Span_Meas_Data GPS_Fifo;
extern struct Span_Meas_Data PRS_Fifo;
extern struct Span_Meas_Data SPANA_Fifo;
extern struct Span_Meas_Data SPANB_Fifo;

#endif // end of CPP

#endif // !defined(AFX_BT3API_H__0D85B188_AC6B_42C2_82E6_FF534C5713EB__INCLUDED_)
