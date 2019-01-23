// Adc.h: interface for the CAdc class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Adc.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Adc.h 1.3 2008/04/18 11:02:45PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#if !defined(AFX_ADC_H__6842A540_194A_4511_8CAC_8BD437FC819B__INCLUDED_)
#define AFX_ADC_H__6842A540_194A_4511_8CAC_8BD437FC819B__INCLUDED_

#ifdef __cplusplus 

#include "DataType.h"
#include "AdcFilter.h"

class CAdc  
{
public:
	uint16 ReadRangeTo(int chan);
	uint16 ReadRangeFrom(int chan);
	int IsWithinRange(int chan);
	uint16 ReadMax(int chan);
	uint16 ReadMin(int chan);
	void ResetMinMax(void);
	enum { ADC_CHAN_OCXO_CURRENT = 6,
		ADC_CHAN_1V8 = 0,
		ADC_CHAN_10M_EFC = 1,
		ADC_CHAN_20M_EFC = 2,
		ADC_CHAN_25M_EFC = 3,
		ADC_CHAN_24M5_EFC = 4,
		ADC_CHAN_24M7_EFC = 5,

		//ADC_CHAN_5V = 4,
		//ADC_CHAN_12V = 1,
		//ADC_CHAN_2V5 = 2,
		//ADC_CHAN_1V2 = 3,

		ADC_CHAN_DUMMY
	};
	uint16 ReadFiltered(int chan);
	uint16 Read(int chan);
	void StartAdc(void);
	void isr_adc(void);
	CAdc(uint16 enable_mask);
	virtual ~CAdc();
	virtual void Init(void);
protected:
	void SetNormalRange(int chan, uint16 from, uint16 to);
	uint16 m_count[16];
	uint16 m_enableMask;
	uint32 m_oorCnt[16];
private:
	void NextGroup(void);
	uint16 m_data[16];
	uint16 m_dataMin[16];
	uint16 m_dataMax[16];
	uint16 m_dataNormalFrom[16];
	uint16 m_dataNormalTo[16];
	uint8 m_bDone;
	int8 m_group;
	CAdcFilter *m_pFilter;
};
#endif

EXTERN void isr_adc(void);

#endif // !defined(AFX_ADC_H__6842A540_194A_4511_8CAC_8BD437FC819B__INCLUDED_)
