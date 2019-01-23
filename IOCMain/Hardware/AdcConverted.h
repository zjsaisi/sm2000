// AdcConverted.h: interface for the CAdcConverted class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADCCONVERTED_H__B21865A2_A01C_455A_AF81_B81D13906BC7__INCLUDED_)
#define AFX_ADCCONVERTED_H__B21865A2_A01C_455A_AF81_B81D13906BC7__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Adc.h"
#include "comm.h"

class CAdcConverted : public CAdc  
{
public:
	void SetCardRevisionId(int id);
	void SetNormalRange(int chan, double from, double to);
	void PrintAdc(Ccomm *pComm, unsigned long * param);
	double ReadConverted(int chan, int8 option = 0);
	CAdcConverted(uint16 enable_mask);
	virtual ~CAdcConverted();
	virtual void Init(void);
	enum { OPTION_CURRENT = 0, OPTION_FILTER, OPTION_MIN, OPTION_MAX,
		OPTION_FROM, OPTION_TO, OPTION_ISTATE_READ};
private:
	enum { NUM_CHAN = 16};
	const char * GetAdcChanName(int chan);
	static char const * const m_chanNameList[NUM_CHAN]; 
	double m_ocxoRatio; // It's dymanic
};

#endif // !defined(AFX_ADCCONVERTED_H__B21865A2_A01C_455A_AF81_B81D13906BC7__INCLUDED_)
