// DtiOutputPm.h: interface for the CDtiOutputPm class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputPm.h 1.2 2008/01/29 11:16:29PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 * Collection of routines to send out PM data about output. Phase, Jitter and FER
 */

#if !defined(AFX_DTIOUTPUTPM_H__251A2C32_D8CE_4799_A8BC_3B3661A3E393__INCLUDED_)
#define AFX_DTIOUTPUTPM_H__251A2C32_D8CE_4799_A8BC_3B3661A3E393__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CPTPLink;

class CDtiOutputPm  
{
public:
	void DtiOutputPmHack(unsigned long *param, Ccomm *pComm);
	void SendJitter(int chan, uint16 jitter);
	void Init(void);
	void SendFer(int chan, uint16 cnt);
	int TickPerSecond(void);
	void SendPhase(int chan, int16 phase);
	void SetEnable(int chan, int enable);
	CDtiOutputPm();
	virtual ~CDtiOutputPm();
private:
	void ResetBuffer(void);

	enum { CMD_PHASE_FER = 0x190, CMD_JITTER = 0x191 };
	enum { NUM_PHASE_POINT = 10 };
	enum { PTP_PORT_PM_REPORT = 0x70 };

	// Structure to hold Output Phase data 
	// Reported every 10 second to port PTP_PORT_PM_REPORT
	typedef struct {
		uint16	cmd; // Always CMD_PHASE_FER
		uint32	timeStamp; // TimeStamp, GPS seconds

		// Phase data of output. 0x7fff=invalid phase data
		// LSB is defined per FPGA. Version 1C says 26.077ps 
		int16	phase[NUM_OUTPUT_CHAN][NUM_PHASE_POINT]; 
		uint16  iocActive; // bit[i]==1 if RTE active in second i.  i=0..9  
	} SPhase; 
	
	// Jitter & FER report structure. 
	// Reported every second to port PTP_PORT_PM_REPORT
	typedef struct {
		uint16	cmd; // Always CMD_JITTER
		uint32	timeStamp;  // TimeStamp, GPS seconds

		// Jitter for each output channel. LSB defined per FPGA. 
		// Invalid when 0xffff
		uint16	jitter[NUM_OUTPUT_CHAN];
		uint16  fer[NUM_OUTPUT_CHAN]; // Frame Error Counter of the second. Moved from SPhase.
		int8	enabled[NUM_OUTPUT_CHAN]; // 0: Channel disabled.  1: Channel enabled.
		uint8	iocActive;		// bit0==1 if RTE active
		uint8	unused;
	} SJitter; 

	SPhase m_phase;
	SJitter	  m_jitter;
	CPTPLink *m_pLink;
	int m_cnt;

	int m_debugPhaseFer;
	int m_debugJitter;
	int32 m_jitterSentCnt;
	int32 m_jitterRejectCnt;
	int32 m_phaseSentCnt;
	int32 m_phaseRejectCnt;
public:
	void ResetPhase(int phase);
	void SetActive(int active);
	static CDtiOutputPm *s_pDtiOutputPm;
};

#endif // !defined(AFX_DTIOUTPUTPM_H__251A2C32_D8CE_4799_A8BC_3B3661A3E393__INCLUDED_)
