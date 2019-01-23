/* TeleOutput.h: Interface for CTeleOutput
*/
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: TeleOutput.h 1.9 2010/11/23 11:12:50PST Jining Yang (jyang) Exp  $
 * $Revision: 1.9 $
 * Manage T1/E1 outputs
 */
#if !(defined(_TELEOUTPUT_H_01_11_2008))
#define _TELEOUTPUT_H_01_11_2008

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class Ccomm;
class CPTPLink;


class CTeleOutput 
{
public:
	// If this number changed, be sure to touch init value for s_teleOutput
	static CTeleOutput *s_pTeleOutput[NUM_TELE_OUTPUT_CHAN];
	static void DoLoopChange(void);
protected:
	static int m_currentLoop;
	//int m_on; // Current status is on or off if enabled.
	int m_chan;
	int m_enabled;
	int m_valid;
	//int m_generation;

	// Specific to T1/E1 output
	int m_frame;
	int m_t1Pulse; // 0: normal 1: t1 pulse
	int m_crc;
	int m_enableSsmOutput;
	int m_ssmBit;
protected: // related to subclass
	CTeleOutput(int chan);
	virtual ~CTeleOutput(void);
	static void RefreshSetup(void);
	virtual int CfValidateChan(int valid);
	enum { TRACK_LEVEL_WARMUP = 10, TRACK_LEVEL_FREERUN, 
		TRACK_LEVEL_FAST_TRACK, TRACK_LEVEL_NORMAL_TRACK
	};
	int m_bestTrackingLevel;
public:
	enum { OUTPUT_CHOICE_OFF = 0, OUTPUT_CHOICE_ON = 1, OUTPUT_CHOICE_AIS = 2 };
protected:
	int m_warmupChoice;
	int m_freerunChoice;
	int m_holdoverChoice;
	int m_fastTrackChoice;
	virtual int QuerySwitch(void); // Should output be on/off

protected: // Specific to telecom output
	virtual int CfFrameChan(int frame);
	virtual int CfT1Pulse(int t1Pulse);
	virtual int CfSelCrc4Chan(int crc);
	virtual int CfEnableSsmChan(int enableSsm);
	virtual int CfSsmBitChan(int ssmBit);
public:
	static int CreateTeleOutput(void);
	// Called by istate 
	virtual int CfEnableChan(int enable);
	virtual int CfGenerationChan(int gen);
	virtual int CfOutputChoice(int state, int choice);
	static int CfGenerationTeleOutput(int chan, int gen);
	static int CfEnableTeleOutput(int chan, int enable);
	static int CfValidateTeleOutput(int chan, int valid);
	static int CfFrameTeleOutput(int chan, int frame);
	static int CfT1PulseTeleOutput(int chan, int t1Pulse);
	static int CfSelCrc4TeleOutput(int chan, int crc);
	static int CfEnableSsmTeleOutput(int chan, int enableSsm);
	static int CfSsmBitTeleOutput(int chan, int ssmBit);
	// Called by output process
	static void SetupOutput(void);

private:
	uint32 *m_pOutputSwitchState;
protected:
	void ReportOutputSwitchState(int on);
};

#endif /* _TELEOUTPUT_H_01_11_2008 */
