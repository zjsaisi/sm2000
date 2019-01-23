// BT3Support.h
//
//////////////////////////////////////////////////////////////////////
/* 
 * BT3 support functions
 * $Header: BT3Support.h 1.9 2010/12/22 16:46:52PST Jining Yang (jyang) Exp  $
 * $Revision: 1.9 $
 */
#ifndef BT3_SUPPORT_H_0701
#define BT3_SUPPORT_H_0701

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "bt3_shell.h"
#include "bt3_shl.h"
#include "string.h"

#ifndef ALLOW_FAKE_BT3
#define ALLOW_FAKE_BT3 1
#endif

typedef struct {
		uint32 offsetTime;  // the time the offset is to be applied (nominally next 1PPS)
		int8  offset;      // predicted offset of GPS 1PPS for timeStamp second,
                            //   0 to 85ns if GPS qualified by PPC,
                            //   255 if GPS not qualified by PPC
        uint8  status;       // status of GPS on PPC:
                            //   0 = GPS disqualified by PPC
                            //   1 = Position Survey
                            //   2 = Position Fixed, TRAIM On
                            //   3 = Position Fixed, TRAIM Off
 		uint32 handle;  	// Identifies Source of TOD
} SReportGPS;

EXTERN int Is_EFC_OK(void);
EXTERN void Get_RGP(SReportGPS *pRGP);

#ifdef __cplusplus
#include "Input.h"

class Ccomm;
class CFpga;
class CCardState;
class CSecondProcess;

EXTERN void InitBT3Support(CFpga *pFPGA, CCardState *pCardState, CSecondProcess *pSecondProcess); 

class CBT3InBound {
	friend void Get_RGP(SReportGPS *pRGP);
	friend int Is_EFC_OK(void);
public:
	static void SetTodSource(int tod);
	static const char * GetSysModeName(int mode);
	enum { BT3_SYS_MODE_SSU=0, BT3_SYS_MODE_SUBTEND = 1, BT3_SYS_MODE_PRR = 2 };
	int GetPhysicalError(void);
	void ResetRgpFifo(void);
	void TickBT3InBound1Second(void);
	void SetGPSReport(SReportGPS *pGPS);
	void SetInputQualityLevel(int chan, int ql);
	void SetInputPriority(int chan, int priority);
	void SetInputAlarm(int chan, int alarm);
	void SetInputEnable(int chan, int enable);
	void SetActiveInput(int chan);
	void PrintInBoundStatus(Ccomm *pComm, unsigned long *param = NULL);
	enum { RESET_FACTORY_COLD_START = 0,
		RESET_NORMAL_COLD = 1, RESET_WARM_START = 2, RESET_HOT_START = 3};
	void ResetBT3(int option = RESET_NORMAL_COLD);
	CBT3InBound();
	~CBT3InBound();
	void SetSysMode(int mode);
	uint8 GetSysMode(void) { return m_sysMode; };
	static void Create(void);
	static CBT3InBound *s_pBT3InBound;
private:
	void SetBt3TraceSource(void);
	int m_activeInput;

	int ReadEfcOk(void);
	void SendInputStatusBT3(void);
	
	enum { NUM_RGP = 13} ;
	SReportGPS m_gps[NUM_RGP];
	volatile int m_gpsPhysicalError;
	volatile int m_rgpIncoming;
	volatile int m_rgpOutcoming;
	volatile uint32 m_rgpInCnt;
	volatile uint32 m_rgpOutCnt;
	volatile uint32 m_rgpInRequestCnt;
	volatile uint32 m_rgpOutRequestCnt;
	uint32 m_rgpDebugCnt; 
	uint32 m_rgpExpectedCnt;

	typedef struct {
		int ql; // Quality level
		int priority; // 
		int alarm; // 1: alarm. 0: OK
		int m_bEnable; // 1: enable 0: disable
	} SInputState;
	SInputState m_inputState[CInput::NUM_INPUT_CHAN];
	int8 m_inputChanged;
	uint8 m_sysMode;
	static int m_todSource;
	static int m_efcOk;

	// For debug only
	int m_traceSource;
};

class CBT3OutBound {
public:
	void ForceBT3Gear(int gear);
	void ForceBT3Mode(int mode);
	void SetTodSource(int mode);
	uint16 GetInputWeight(int chan);
	void TickBT3OutBound1Minute(void);
	void ClearBackupAlarm(void);
	void CheckBackupInput(int chan, uint8 bEnable);
	int8 IsGPSClear(void);
	uint16 GetGPSReg(void);
	int IsInputOk(int chan);
	void PrintOutBoundStatus(Ccomm *pComm);
	static void Create(void);
	static CBT3OutBound *s_pBT3OutBound;
	void TickBT3OutBound1Second(void);
	int GetGear(void);
	enum { BT3_MODE_WARMUP = 0, BT3_MODE_NORMAL = 1, BT3_MODE_HOLDOVER = 2};
	int GetMode(void); 
protected:
	CBT3OutBound();
	~CBT3OutBound();
private:
	void PrintBT3Reg(const char *pName, unsigned short value, char *pBuffer);
	struct ALM_STAT_STRUCT m_BT3AlmStatus;
	int m_bt3Mode;
	int m_bt3Gear;
	int m_bt3ModeForced; // for debug
	int m_bt3GearForced; // for debug
	uint16 m_prsReg;
	uint16 m_spanAReg;
	uint16 m_spanBReg;
	int8 m_prsLoss; // prs not available
	int8 m_spanALoss; // Span A not available
	int8 m_spanBLoss; // Span B not available
	int8 m_backAnyLoss; // any one of them is not available.
	int8 m_backAnyLossReported; 
	uint32 *m_pBT3EngineReport; // place to store BT3 engine report
	uint16 m_weightPrs;
	uint16 m_weightSpan1;
	uint16 m_weightSpan2;
	uint16 m_backupReg;
	int m_todMode;
};
#endif

EXTERN uint32 Get_GPS_Phase(void);
EXTERN void Align_GPS_Phase(void);
EXTERN uint8 Is_MPLL_Tracking(void);
EXTERN uint8 Is_OCXO_warm(void);
EXTERN uint8 In_MPLL_Phase_Window(void);
EXTERN uint8 Get_Clock_Mode(void);
EXTERN void Reset_RGP_Fifo(void);
EXTERN int GetOscillatorType(void);

// From bt3_message.c
EXTERN void	PutBt3EnsembleControlWord( unsigned short EnsembleControlWord );
EXTERN void	GetBt3EnsembleControlWord( unsigned short *EnsembleControlWord, unsigned short *Override );
EXTERN void	PutBt3ProvisionControlWord( unsigned short ProvisionControlWord );
EXTERN void	GetBt3ProvisionControlWord( unsigned short *ProvisionControlWord );
EXTERN void	GetBt3ResetControlWord( unsigned short *ResetControlWord, unsigned short *Override );
EXTERN void	PutBt3ResetControlWord( unsigned short ResetControlWord );
EXTERN void GetBt3AlarmStatus( Bt3AlarmReportType *AlarmReport );
EXTERN void Get_BesTime_Engine_Report( struct MSG_BesTime_Engine_Report *Engine_Report);
EXTERN void bestime_engine_ensemble_control( unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);

// From bt3_tsk.c
EXTERN void set_trace_source(unsigned int tsource);  
EXTERN void set_TOD_mode(unsigned int tsource);
// from bt3_est.c
EXTERN void Set_GPS_Drift_Low(void);

// From bt3_fll.c
EXTERN void set_ensemble_mode(unsigned int emode);    

// From bt3_fll.c
EXTERN void set_efc_mode(unsigned int emode);
// From bt3_mgr.c
EXTERN void Cal_Data_Reset(void);
EXTERN void Cal_Data_Subtend_Reset(void);

#endif // BT3_SUPPORT_H_0701
