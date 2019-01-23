// Fpga.h: interface for the CFpga class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: Fpga.h 1.22 2009/02/03 16:44:23PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.22 $
 */                 

#if !defined(AFX_FPGA_H__5A0689AF_4B64_45E0_ACF8_AE663782DBCB__INCLUDED_)
#define AFX_FPGA_H__5A0689AF_4B64_45E0_ACF8_AE663782DBCB__INCLUDED_

// Set 1 to read fpga from a memory block instead of FPGA
#define SIMULATE_FPGA_READ 0

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class Ccomm;

#define BT3_ENABLE_BIT 1
#define BT3_QUALIFY_BIT 0x100

typedef struct {
	union {
		uint32 word32;
		struct {
			uint8 status_one; // 0
			uint8 status_two; // 1
			uint8 pll_reg;  // 2
			uint8 los_status; // 3
		} byte;
	} status32;
	uint8  ipll_status;		// 4
	uint8  tbd01;			// 5
	uint16 span_one_phase; // 6 7
	uint16 span_two_phase; // 8 9
	uint32 gps_phase; // a b c d
	uint32 uti_one_phase; // e f 10 11
	uint32 uti_two_phase; // 12 13 14 15
	uint8  phase_status; // 16
	uint8  intr_cnt;	 // 17
	uint32 accumulate_efc; // 18 19 1a 1b
	uint32 current_efc;		// 1c 1d 1e 1f
	
	// span input
	uint8  span_one_select; // 20
	uint8  span_one_e1_ssm; // 21
	uint8  span_one_e1_rx_status; // 22
	uint8  span_one_t1_rx_status; // 23
	uint8  span_one_t1_ssm; // 24
	uint8  span_two_select; // 25
	uint8  span_two_e1_ssm; // 26
	uint8  span_two_e1_rx_status; // 27
	uint8  span_two_t1_rx_status; // 28
	uint8  span_two_t1_ssm; // 29

	// span output
	uint8 output_one_select; // 2a
	uint8 output_two_select; // 2b
	uint8 output_three_select; // 2c
	uint8 output_four_select; // 2d
	uint8 output_t1_ssm;	// 2e
	uint8 output_e1_ssm;    // 2f
	uint8 output_e1_ssm_slot_AB; // 30
	uint8 output_e1_ssm_slot_CD; // 31
	uint8 output_hz_control; // 32
	uint8 tbd_33; // 33
	int16 uti_leap_correction; // 34 35
	uint32 uti_gps_second; // 36 37 38 39
	uint8  tod_message; // 3a
	uint8  server_device_type; // 3b
	uint8  client_device_type; // 3c
	uint8  uti_port_type_select; // 3d
	uint16 gps_8k_phase; // 3e, 3f

#if 0
	uint8 ctrl1;		// 0
	uint8 ctrl2;		// 1
	uint8 ctrl3;		// 2
	uint8 ctrl4;		// 3
	uint8 span1_select; // 4
	uint8 span2_select; // 5
	uint32 efc;			// 6
	uint16 tbd01; //tcw; // 8 //4/19/2006: No more.
	uint32 status;		// 0xc
	uint16 tbd02; //uint16 gps_phase;	// 0x10
	uint16 dti_one_phase; // 0x12
	uint16 dti_two_phase; // 0x14
	uint16 span_one_phase; // 0x16
	uint16 span_two_phase; // 0x18
	uint16 gps_8k_phase;	// 0x1a
	uint8 intr_cnt;			// 0x1c
	uint8 span1_t1_status;	// 0x1d
	uint8 span1_t1_ssm;		// 0x1e
	uint8 span2_t1_status;	// 0x1f
	uint8 span2_t1_ssm;		// 0x20
	uint8 span_error_cnt;	// 0x21
	uint32 gps_phase;		// 0x22
#endif
} SFpgaMap;

#if 0
typedef struct {
	unsigned char skip[0x40];
	uint8	ant_fpga_version; // 0x40
	uint8   ant_crc_error_cnt; // 0x41
	uint16	ant_pps_phase_error; // 0x42 0x43
	uint8	ant_dac_level;		// 0x44
	uint8	ant_peak_receive_level; // 0x45
	uint16	ant_tcxo_tuning_voltage; // 0x46 0x47
	uint8	tpiu_fpga_version;	// 0x48
	uint8	tpiu_crc_error_cnt;  // 0x49
	uint16  tpiu_receive_phase_error; // 0x4a 0x4b
	uint8	tpiu_adc_reference_value; //0x4c
	uint8	tpiu_receive_peak_level; //0x4d
	uint16	tpiu_vcxo_tuning_voltage; // 0x4e 0x4f
	uint16	ant_cable_length;	// 0x50 0x51
	uint8	ant_cable_fraction; // 0x52
	uint8	cts_status;			// 0x53
} STpiuMap;
#endif

#if 0
typedef struct {
	unsigned char skip[0xc0];
	unsigned char nothing;   // 0xc0
	unsigned char led;		 // 0xc1
	int32 phaseError;		 // 0xc2
	unsigned char control;   // 0xc6
	unsigned char clientStatus; // 0xc7
	unsigned char skip1[4];    // 0xc8
	unsigned char serverDeviceType;  // 0xcc
	unsigned char serverStatusFlag; // 0xcd
	unsigned char skip2[2];		// 0xce
	int32	cableAdvance;	// 0xd0
	uint16	crcErrorCnt;	// 0xd4
	unsigned char skip3;	// 0xd6
	unsigned char portStatus;	// 0xd7
	uint16 jitterPeak;		// 0xd8
} SClientMap;
#endif

#if SIMULATE_FPGA_READ
#define CONST_READ_HARDWARE 
#else
#define CONST_READ_HARDWARE const
#endif

typedef struct {
	uint8 preAnd;
	uint8 preOr;
	uint8 andData;
	uint8 orData;
} SAndOr;

class CJumpMon;

class CFpga  
{
private:
	uint32 volatile m_gpsPhase; 
	int m_gpsPhaseInvalid; 

	CJumpMon *m_pEfcMonitor; // debug only
	int m_gpsPhaseInvalidReported; // debug only
	uint32 m_gpsPhaseMeasureLoss; // debug only

	int volatile m_edgeAdjustCnt; // debug only
	int m_25msLost; // For debug only
private:
	uint32 m_gpsSecond;
	int m_leapSecond;
public:
	uint32 GetAdjustedGpsSecond(void);
	uint32 GetGpsSec(void);
public:
	void SetOutputQL(int level);
	int GetIpllStatus(int chan);
	int IsRubidium(void);
	int IsRubLocked(void);
private: 
	uint8 volatile m_ipllStatusOr;
	uint8 volatile m_previousIpllStatus;

	uint32 volatile m_previousStatusAnd;
	uint32 volatile m_previousStatusOr;
	uint32 volatile m_statusAnd;
	uint32 volatile m_statusOr;
	uint32 volatile m_status;
	int volatile m_isrCnt;     
	int32 volatile m_freq_off;
	SFpgaMap m_fpgaBuff;
	static SFpgaMap volatile * CONST_READ_HARDWARE m_pReadHardware;
	static SFpgaMap volatile * const m_pWriteHardware;
#if SIMULATE_FPGA_READ
	SFpgaMap m_readFpgaSimulator;
#endif

	int volatile m_imcJam;
	int m_jamCnt;
	int m_jamClearCnt;
	uint32 volatile m_efc;    //GPZ make 32 bits 

	// FPGA ISR debug
	enum { NUM_ISR_TIMESTAMP = 520, NUM_ISR_WATCH = 400 };
	uint16 m_isrProcessStartTimeStamp[NUM_ISR_TIMESTAMP];
	uint16 * m_pIsrTimeStampStart;
	uint16 m_isrProcessEndTimeStamp[NUM_ISR_TIMESTAMP];
	uint16 * m_pIsrTimeStampEnd;
	void PrintIsrTimeStamp(unsigned long *param, Ccomm *pComm, char *buff);

public:
	static CFpga *CreateFpga(void);
	static CFpga *s_pFpga;
	int IsFpgaActive(void);
	enum {FPGA_SO_CCS_NCRC4, FPGA_SO_CCS_CRC4, 
		FPGA_SO_CAS_NCRC4, FPGA_SO_CAS_CRC4,
		FPGA_SO_E1_AIS, FPGA_SO_2048MHZ,
		FPGA_SO_D4, FPGA_SO_ESF, FPGA_SO_ESF_NSSM, 
		FPGA_SO_T1_AIS,
		FPGA_SO_T1_PULSE, FPGA_SO_1544MHZ,
		FPGA_SO_DISABLE = 0xff};
	void SetSpanOutput(int chan, int type);
	void Set10MHzOutput(int enable);
	void Set1PPSOutput(int enable);
	
	enum {FPGA_SPAN_OUTPUT_NO_SSM, FPGA_SPAN_OUTPUT_SA4,
		FPGA_SPAN_OUTPUT_SA5, FPGA_SPAN_OUTPUT_SA6,
		FPGA_SPAN_OUTPUT_SA7, FPGA_SPAN_OUTPUT_SA8,
		FPGA_SPAN_OUTPUT_SA_ALL = 15 }; 
	void SetE1SsmChoice(int chan, int ssm);

private:
	uint16 m_spanAPresent;
	uint16 m_spanBPresent;
	uint16 m_gpsPresent;
private:
	int m_spanInputType[2];
	static uint8 const m_t1SSMMaping[256];
	static uint8 const m_e1SSMMaping[16];
	static uint8 const m_t1QLToSSMCode[17];
	static uint8 const m_e1QLToSSMCode[17];
	int m_spanInputSsm[2][17]; // @25ms level
	int m_spanInputSsmSec[2]; // @second level
	void CFpga::CollectSsm25ms(void);
	void CFpga::CollectSsm1S(void);

public:
	enum {FPGA_SPAN_INPUT_DISABLE, FPGA_SPAN_INPUT_2MHZ, 
		FPGA_SPAN_INPUT_E1, 
		FPGA_SPAN_INPUT_T1 };
	void SetSpanInputType(int chan, int type);

	// SAx is supposed to be continuous
	enum {FPGA_SPAN_INPUT_SSM_SA4, FPGA_SPAN_INPUT_SSM_SA5,
		FPGA_SPAN_INPUT_SSM_SA6, FPGA_SPAN_INPUT_SSM_SA7,
		FPGA_SPAN_INPUT_SSM_SA8};
	void SetSpanInputSsmBit(int chan, int type);

	int GetSpanInputSsmQL(int chan);

private:
	SAndOr m_span1E1Status;
	SAndOr m_span2E1Status;
	SAndOr m_span1T1Status;
	SAndOr m_span2T1Status;
public:
	int CheckE1(int chan, int cas, int crc4);
	int Check2MHZ(int chan);
	int CheckT1(int chan, int esf);
	int Check1544MHZ(int chan);
protected:
	CFpga();
	virtual ~CFpga();
public:
	int GetGpsLos(void);
private:
	int m_todSource;
public:
	void SetTodSource(int tod);





	//////////////////// Old stuff
public:
	void SetUnlockHardControl(int set);
	int GetDcmUnlock(void);
	void SetTodArm(int8 arm);
	void SetSlotId(int8 slot);
	int GetPllUnlock(void);
	void GetCurrentEfcControl(uint16 *pHigh, uint16 *pLow, int *pDither, int *pCnt);
	uint16 GetVersion(void);
	void SetInputQualified(int chan, int ql);
	void FillNHDuration(uint32 *pDuration);
	void FillTCounter(int32 *pCounter);
	int32 GetInputCableAdvance(int chan);
	unsigned char GetInputServerDeviceType(int chan);
	void SetGpsSec(uint32 gps);
	enum { MAX_PATH_SIZE = 63, MAX_TOD_SIZE = 41};
	void GetInputTodMessage(int chan, unsigned char *pTodMessage);
	void GetInputPath(int chan, unsigned char *pPath);
	void SetInputLed(int chan, int yellow, int green);
	uint16 getInputCrcError(int chan);
	void GetInputServerStatus(int chan, unsigned char *pCurr,  unsigned char *pAnd);
	void SetInput12ClientStatus(unsigned char stat);
	void SetInput11ClientStatus(unsigned char stat);
	void SetFpgaHodoverMode(int mode);
	void SetHighLowDither(unsigned int high, unsigned int low, unsigned int dither);
	uint32 GetGPSPhase(void);
	void SetPortMode(int chan, int out);
	void CfOutputTodMode(int mode);
	uint8 GetGpsLsb(void);
	void FakeJam(void);
	int GotImcJam(int clear);
	int GetTick(void);
	void SetMpll(int mpll);
	void UnlockPll(void);
	void SendTod(const char *ptr);
	void GetOutputStatus(uint32 &d_and, uint32 &d_or);
	void Transfer(void);
	int IsTwinIn(void);
	void SetReady(int ready = 1);
	void FpgaHack(unsigned long *param, Ccomm *pComm);
	void Isr25ms(void);
	enum { MPLL_SPAN_ONE, MPLL_SPAN_TWO, 
		MPLL_DTI_ONE, MPLL_DTI_TWO,
		MPLL_GPS, MPLL_NA1,
		MPLL_HOLDOVER, MPLL_FREERUN };
	enum { CLIENT_WARMUP = 1, CLIENT_FREERUN = 2, CLIENT_FAST_TRACK = 4,
		CLIENT_NORMAL_TRACK = 8, CLIENT_HOLDOVER = 16, CLIENT_BRIDGE = 32 };

private:
	void AlignGpsPhase(int set);
	inline void Input_1sec(void);
	inline void Input_25ms(void);
	inline void Bt3Next(void);
	inline void Bt3_1sec(void);
	inline void Bt3_25ms(void);
	void SummarizeOutput(void);
	void CollectOutput(void);
	void Summarize(void);
	void Collect(void);


	// static STpiuMap volatile * const m_pTpiu;
	//static SClientMap volatile * const m_pClient11;
	//static SClientMap volatile * const m_pClient12;
	uint8 m_previousCtsStatusOr;
	uint8 m_ctsStatusOr;
	uint8 m_ctsStatus;
	int volatile m_cfOutputTodLength;

	// EFC control parameter
	unsigned int volatile m_high;
	unsigned int volatile m_low;
	unsigned int volatile m_dither;
	unsigned int volatile m_currentDither;


	// Port11 and 12 as input
	unsigned char m_inputServerStatus[2]; // input server status
	unsigned char m_inputServerStatusAnd[2];
	unsigned char m_inputServerStatusWorking[2];
	unsigned char m_inputServerDeviceType[2];
	unsigned int  m_inputErrorCnt[2]; // crc error cnt
	int32	volatile m_inputCableAdvance[2]; // Cable advance

	char m_inputServerPath[2][64];
	char m_inputTodMessage[2][42];
	uint32 m_inputCollision[2]; // debug
	uint32 m_inputMessageFailure[2]; // debug

	// Output
	uint32 volatile m_previousOutputStatusAnd;
	uint32 volatile m_previousOutputStatusOr;
	uint32 volatile m_outputStatusAnd;
	uint32 volatile m_outputStatusOr;

	uint16 m_outputClientNormalCnt[NUM_OUTPUT_CHAN];
	uint16 m_preOutputClientNormalCnt[NUM_OUTPUT_CHAN];
	uint16 m_outputClientHoldoverCnt[NUM_OUTPUT_CHAN];
	uint16 m_preOutputClientHoldoverCnt[NUM_OUTPUT_CHAN];
	uint8 m_outputClientStatus[NUM_OUTPUT_CHAN];

	uint8 m_outputT3[NUM_OUTPUT_CHAN];
	uint8 m_preOutputT3[NUM_OUTPUT_CHAN];
	uint8 m_outputT4[NUM_OUTPUT_CHAN];
	uint8 m_preOutputT4[NUM_OUTPUT_CHAN];
	uint8 m_outputT6[NUM_OUTPUT_CHAN];
	uint8 m_preOutputT6[NUM_OUTPUT_CHAN];
	uint8 m_outputT7[NUM_OUTPUT_CHAN];
	uint8 m_preOutputT7[NUM_OUTPUT_CHAN];



	// Input
	int m_inputQualified[NUM_INPUT];

private:
	// TPIU tuning
	uint16 m_tpiuTune;
	// TA tuning
	uint16 m_taTune;
	// TPIU crc
	uint8 m_tpiuCrc[2];
	// TA crc
	uint8 m_taCrc[2];
public:
	int IsTpiuTuneOk(void);
	int IsTaTuneOk(void);
	int IsGpsPathCrcOk(void);
	int IsXsyncOk(void);
};

#endif // !defined(AFX_FPGA_H__5A0689AF_4B64_45E0_ACF8_AE663782DBCB__INCLUDED_)
