// DtiOutput.h: interface for the CDtiOutput class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutput.h 1.3 2008/07/22 15:53:01PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.3 $
 */

#if !defined(AFX_DTIOUTPUT_H__D6F63F72_B969_471A_80C4_A6059FF7AF64__INCLUDED_)
#define AFX_DTIOUTPUT_H__D6F63F72_B969_471A_80C4_A6059FF7AF64__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "DtiOutputStatus.h"
#include "PhErrDetect.h"

class Ccomm;
class CDtiOutputPm;
class CDtiDelayStable;
class CPTPLink;

#define OUTPUT_FPGA_BASE  FPGA_START_ADDRESS

// 7/22/2008 zmiao: Send dti output to dummy for now.
extern uint8 outputDummy[512];
#undef OUTPUT_FPGA_BASE 
#define OUTPUT_FPGA_BASE outputDummy

#define OUTPUT_PORT_SELECT ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x8d))
#define OUTPUT_PATH_TRACE ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x90))
#define CLIENT_STATUS_FLAG ((uint8 volatile *)(OUTPUT_FPGA_BASE + 0x9d))

class CDtiOutput  
{
private:
	typedef struct {
		uint16 cmd; // CTwinProxy::CMD_OUTPUT_CONTROL
		uint16 version; // 1 for first release.
		uint32 cableAdvance[NUM_OUTPUT_CHAN];
		uint8 outputLed[NUM_OUTPUT_CHAN];
		uint8 serverStatus[NUM_OUTPUT_CHAN];
		uint32 pmEdge;

		// Only add at the end
	} SOutputControl;
	static SOutputControl m_twinControl;
	static int m_cardStatus;
	static CPTPLink *m_pIOCLink;
public:
	static void SetMyClockType(int type);
	static void CfTestMode(int chan, int test);
	static void DoStatusChange(void);
	static void SetupStandby(void);
	static int ReceiveOutputControl(const char *ptr, int size);
	static void CfCableAdvanceValue(int chan, uint32 advance);
	static void CfCableAdvanceMode(int chan, int mode);
	static void CfValidate(int chan, int valid);
	static void DtiOutputHack(unsigned long *param, Ccomm *pComm);
	static void SetupOutput(void);
	static void CfEnable(int chan, int enable);
	static void ServiceOutput(void);
	static void ReadRegisterIsr(int active);
	static void PowerOn(void);
	static void DoLoopChange(void);
	static void SetExternalTimeSource(int source);
	static void SetServerHop(int hop);
	static void Init(void);
	static void Create(void);
	static void SetClockType(int type);

	enum { CLOCK_TYPE_TYPE_I = 0, CLOCK_TYPE_TYPE_II = 1,
		CLOCK_TYPE_TYPE_III = 2, CLOCK_TYPE_ST3 = 3 };
	enum { CLIENT_WARMUP = 1, CLIENT_FREERUN = 2, CLIENT_FAST_TRACK = 4,
		CLIENT_NORMAL_TRACK = 8, CLIENT_HOLDOVER = 0x10, 
		CLIENT_BRIDGE = 0x20, CLIENT_STATUS_MASK = 0x3f };
	enum { SERVER_WARMUP = 1, SERVER_FREERUN = 2, SERVER_FAST_TRACK = 4, 
		SERVER_NORMAL_TRACK = 8, SERVER_HOLDOVER = 0x10, SERVER_STATUS_MASK = 0x1f
		, CABLE_ADVANCE_STABLE = 0x20
		, CLIENT_PERFORMANCE_STABLE = 0x40
	};

protected:
	CDtiOutput(int chan);
	virtual ~CDtiOutput();

private:
	static int WaitForIstateTrasnfer(void);
	static int SendNHDuration(int nh);
	static void InactiveClearance(int cnt);
	static int SendTwinTCounters(int index2);
	int IsLos(void);
	int IsDisabled(void);
	static void ClearEvents(void);
	void ServiceChanWarmup(void);
	static int SendOutputControl(void);
	int AutoMode(uint8 &status, int32 &cableAdvance);
	void ReportStatus(int clearIt = 0);
	void SetupOutputChan(void);
	void ServiceChanActive(void);
	void SetNServerStatusFlag(void);
	static void GetServerMode(void);
	void SetNLed(int8 yellow = -1, int8 green = -1);
	// List of object
	static CDtiOutput *m_dtiOutput[NUM_OUTPUT_CHAN];

	static uint16 m_serviceCnt;

	CDtiDelayStable *m_pDelayStable;
	CDtiOutputStatus m_eventStatus;
	CPhErrDetect	m_phaseErrorDetect;
	static CDtiOutputPm  *m_pOutputPm;

	//// configuration
	int m_enable;
	int m_testMode;

	////////////////////////////These are mirror data for FPGA registers.
	// Singular setup
	static uint8 m_deviceType;
	static uint8 m_thisDeviceType;
	static uint8 m_serverMode; // Port N Server status flag @0x95 (bit4 .. 0)
	static uint32 m_outputPortStatusAnd; // Combined port status AND
	static uint32 m_outputPortStatusOr; // Combined port status OR
	static int m_activeCnt;
	static int m_inActiveCnt;
	uint32 m_losBit;
	uint32 m_lofBit;

	// setup per output channel
	int m_chan;
	int m_valid; // 1 for valid. 0: this is a channel used as DTI client.
	int m_cableAdvanceMode; // AUTO_MODE MANUAL_MODE
	uint32 m_cableAdvanceManualSet;
	uint8 m_led;
	uint8 m_control;
	uint8 m_serverStatusFlag; // only bit 6 and 5 of Port N Server status flags.
	static uint8 m_controlUpdated;
	int32 m_cableAdvance;
	int32 m_cableAdvancePrinted; // debug only

	int m_init;
	int32 *m_pIstatePhaseError; // The place to store phase error for Istate report.

	// Status read back per channel
	uint32 m_oldCableDelay;
	uint32 m_cableDelay;
	uint8 m_clientDeviceType;
	uint8 m_clientStatusFlag;
	uint8 m_clientVersion;
	uint32 m_clientPhaseError;
	uint16 m_crcErrorCount;
	// uint8 m_portStatus; // Moved to combined one.
	uint16 m_jitter;
	int m_clientLockCnt;

	// ioc status
	static int m_fpgaActive;
	static uint32 m_pmEdge;
	static int m_isActive;

	// Report counters
	static int32 *m_pReportTCounters;
	// report normal holdover duration
	static uint32 *m_pReportNHDuration;
};

#endif // !defined(AFX_DTIOUTPUT_H__D6F63F72_B969_471A_80C4_A6059FF7AF64__INCLUDED_)
