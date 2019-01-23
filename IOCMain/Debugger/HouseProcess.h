// HouseProcess.h: interface for the CHouseProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEPROCESS_H__2629317D_2151_4F33_8333_77FF23F40487__INCLUDED_)
#define AFX_HOUSEPROCESS_H__2629317D_2151_4F33_8333_77FF23F40487__INCLUDED_


#include "BaseProcess.h"

#ifndef TRACE_HOUSE
#if 1                  
#define TRACE_HOUSE CHouseProcess::s_pHouseProcess->PrintDebugOutput
#else
#define TRACE_HOUSE 1 ? (void)0 : CHouseProcess::s_pHouseProcess->PrintDebugOutput
#endif
#endif

class CAdcConverted;
enum {NUM_REPORTED_ADC_CHAN = 7 };

class CHouseProcess : public CBaseProcess  
{
public:
	static int32 m_reportedAdc[NUM_REPORTED_ADC_CHAN];
	void HackHouseProcess(unsigned long *param, Ccomm *pComm);
	CAdcConverted *GetAdc(void) { return m_pAdc; };
	virtual void EventTimeOut(void);
	virtual void EventDispatch(uint16 event);
	void Init(void);
	static void TickHouseProcessPerSecond(void);
	CHouseProcess(const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CHouseProcess();
	static CHouseProcess *s_pHouseProcess;
private:
	enum { MAX_ADC_CHAN_NUM = 16 };
	int m_voltOOR[MAX_ADC_CHAN_NUM];
	void CheckVoltage(void);
	void CheckOcxo(void);
	int m_rubUnlock;
	void CheckRubLock(void);
	void CheckPllUnlock(void);
	static void Entry(void);
	enum { EVT_ONE_SECOND_ADC = 1 };
	enum { HOUSE_TICK_PER_SECOND = 1 };

	CAdcConverted *m_pAdc;
	int m_timeout;
	int m_maxTimeout;
	int m_skipTimeoutChecking;
	int m_failed;
	int m_pllUnlockCnt;
	int m_pllUnlockChan;
	int m_voltCheckDelay;

	enum { ADC_POST_CYCLE = 3};
	int m_adcPostCycle;
	void PostAdcValue(void);
};

#ifndef TRACE_HOUSE
#define TRACE_HOUSE CHouseProcess::s_pHouseProcess->PrintDebugOutput
#endif

#endif // !defined(AFX_HOUSEPROCESS_H__2629317D_2151_4F33_8333_77FF23F40487__INCLUDED_)
