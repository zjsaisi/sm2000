// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: Input.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: Input.h 1.13 2011/07/08 15:35:10PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.13 $
 */             

#if !defined(AFX_INPUT_H__AD331769_6196_4E68_B1E3_F2638E9B0350__INCLUDED_)
#define AFX_INPUT_H__AD331769_6196_4E68_B1E3_F2638E9B0350__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"
#include "InputEvent.h"

#define INPUT_COMBINED_PRIORITY_INVALID   200

class CInput  
{
public:
	virtual int GetCurrentQL(void);

protected:
	int m_statNow;
	unsigned int m_statReported;
	int m_statCnt;
	virtual int UpdateInputStatus(int stat);
public:
	// Read reported status for WARMUP code. 
	virtual int GetReportedStatus(void);

public:
	// Only used by CInputSpan
	virtual int CfFrameChanInput(int frame); 
	virtual int CfCrc4ChanInput(int crc4);
	virtual int CfSsmBitChanInput(int ssmBit);
	virtual int CfSsmReadChanInput(int ssmRead);
	virtual int CfAssumedQLChanInput(int assumed);

public:
	static void RefreshInputSetup(void);
	static void CfTodSource(int tod);
	void CfValidChanInput(int valid);
	static void SetActiveInput(int chan);
	enum { LOCK_LEVEL_NULL = 0
		, LOCK_LEVEL_PHASE_ERROR = 1
		, LOCK_LEVEL_CRC_NOISY = 2
		, LOCK_LEVEL_LOCK_FLAG = 0x1000
		, LOCK_LEVEL_TOTAL = 0x1000};

	virtual int IsLocked(void);
	static int AllInputSetup(void);
	static const char * GetInputChanName(int chan);
	static void TouchInput(void);
	static const char * GetStatName(int stat);
	void CfInputEnable(int enable);
	void CfDisqDelay(int delay);
	void CfQualDelay(int delay);
	void CfPriority(int priority);
	virtual int GetPriority(void);
	int GetQualify(void) { return m_inputQualified; };
	static unsigned int AllInputTick(void);
	static void AllInputHack(unsigned long *param, Ccomm *pComm);
	virtual int InputHack(unsigned long *param, Ccomm *pComm);
	static void Init(void);
	static void CreateInput(void);
	static CInput *s_pInput[NUM_INPUT];
	enum {
		INPUT_STAT_OK = 1,
		INPUT_STAT_DISABLED,
		INPUT_STAT_LOS = -100,
		// Span input
		INPUT_STAT_AIS,
		INPUT_STAT_OOF,
		INPUT_STAT_PHASE_ERROR,
		INPUT_STAT_FREQUENCY_ERROR,
		INPUT_STAT_IPLL_UNSETTLED,
		INPUT_STAT_POOR_SSM,

		INPUT_STAT_SERVER_WARMUP, // DTI input Server is in warmup, cannot track this input
		INPUT_STAT_CABLE_ADVANCE_INVALID // DTI input
		, INPUT_STAT_INVALID_PATH_TRACEABILITY // DTI input
		, INPUT_STAT_INVALID_TOD_MESSAGE // DTI input

		// Added on 8/3/2006 by zmiao
		, INPUT_STAT_TA_TUNING_OUT_OF_RANGE  // GPS input
		, INPUT_STAT_TPIU_TUNING_OUT_OF_RANGE // GPS input
		, INPUT_STAT_GPS_PATH_CRC_ERROR     // GPS input path CRC error
		
		, INPUT_STAT_NO_RGP // GPS input
		, INPUT_STAT_GPS_ACQUIRING // Aquiring GPS

		, INPUT_PHYSICAL_OK // virtual line

		, INPUT_STAT_BT3_UNSETTLED // Not qualified by BT3 yet. 
	};
	enum { INPUT_QUALIFY_OK = 1,
		INPUT_QUALIFY_DISQ = 0,
		INPUT_QUALIFY_IGNORE = 2 };
	enum { NUM_INPUT_CHAN = NUM_INPUT,
		INPUT_CHAN_GPS = CHAN_GPS,
		INPUT_CHAN_PRS = CHAN_DTI1, // There is no PRS input. 
		INPUT_CHAN_SPAN1 = CHAN_BITS1,
		INPUT_CHAN_SPAN2 = CHAN_BITS2
	};
protected:
	// To be selected by selection algorithm
	// For GPS it should not be selected. 
	int m_toBeSelected;
public:	virtual int IsToBeSelected(void); 

protected:

	virtual int InputSetup(void);
	virtual unsigned int Qualify(int stat);
	enum { INPUT_CONFIG_DISABLE = 0,
		INPUT_CONFIG_ENABLE = 1};

	virtual unsigned int InputTick(void);
	virtual int ReadStatus(void);
	virtual void InitChan(void);
	CInput(int chan, const char *pName, int noStandbyInput);
	virtual ~CInput();
	static int m_activeInput;

	int m_chan;
	int m_noStandbyInput;
	int m_chan256;
	int m_enable;
	int m_cfEnable;
	int m_cfValid;
	int m_inputStat;
	int m_inputStatDebug; // for debug purpose only
	int m_inputQualified;
	int m_inputDbgFlag;

	int m_priority;
	int m_qualDelay;
	int m_qualDelayCf;
	int m_disqDelay;
	int m_disqDelayCf;
	int volatile m_changed;	
	int m_cnt;
	char m_name[10];
	CInputEvent m_event;
	static int m_cardStatus;
	static int m_isCardActive;
	static int m_todSource;
private:
	void DriveEnable(void);

};

#endif // !defined(AFX_INPUT_H__AD331769_6196_4E68_B1E3_F2638E9B0350__INCLUDED_)
