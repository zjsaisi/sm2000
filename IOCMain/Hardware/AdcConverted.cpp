// AdcConverted.cpp: implementation of the CAdcConverted class.
//
//////////////////////////////////////////////////////////////////////
  
#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "AdcConverted.h"
#include "string.h"
#include "stdio.h"
#include "Fpga.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char const * const CAdcConverted::m_chanNameList[NUM_CHAN] = {
	"1V8",		// 0
	"10M EFC",			// 1
	"20M EFC",			// 2
	"25M EFC",			// 3
	"24M5 EFC",			// 4
	"24M7 EFC",		// 5
	"OCXO Current",	// 6
	"Chan7",		// 7
	"Chan8",		// 8
	"Chan9",		// 9
	"Chan10",		// 10
	"Chan11",		// 11
	"Chan12",		// 12
	"Chan13",		// 13
	"Chan14",		// 14
	"Chan15"		// 15
};

/* For Polestar. For Quartz OSC
 * I * 0.1 = v
 * 8 * v = V * R55 / (R55 + R56).
 * So I = V * 10 * R55 / ((R55 + R56) * 8)
 *		= V * 10 / 24
 * i(mA) = 10000 * ADC * 2.5 / (24 * 1023)
 *
 * For Rubidium OSC, Bob take off R55 zmiao: 4/18/2008
 * I * 0.1 = v
 * 8* v = V
 * So I = V * 10 / 8
 * i(mA) = 10000 * ADC * 2.5 / (8 * 1023)
*/
//#define OCXO_CURRENT_CONVERT_0 ((double)25 / (1.023 * 5.32 * 8))
//#define OCXO_CURRENT_CONVERT_1 ((double)25 / (1.023 * 3 * 8))

#define OCXO_CURRENT_CONVERT ((double)25 / (1.023 * 24))
#define OCXO_CURRENT_CONVERT_RUBIDIUM ((double)25 / (1.023 * 8))

#define VOLT_2V5_CONVERT ((double)5 / 1023)
#define VOLT_12V_CONVERT ((double)7.81 * 2.5 / 1023)
#define VOLT_1V2_CONVERT ((double)2.5 / 1023)
#define VOLT_5V_CONVERT ((double)13.32 * 2.5 / (1023 * 3.32))
#define DEFAULT_CONVERT  ((double)2.5 / 1023)


CAdcConverted::CAdcConverted(uint16 enable_mask) : CAdc(enable_mask)
{
	m_ocxoRatio = OCXO_CURRENT_CONVERT;
}

void CAdcConverted::SetCardRevisionId(int id)
{
#if 0
	switch (id) {
	case HARDWARE_REV_ID0:
		m_ocxoRatio = OCXO_CURRENT_CONVERT_0;
		break;
	default: // in case, somebody mess up
	case HARDWARE_REV_ID1:
		m_ocxoRatio = OCXO_CURRENT_CONVERT_1;
		break;
	}
#endif
}

CAdcConverted::~CAdcConverted()
{

}

void CAdcConverted::Init(void)
{
	CAdc::Init();
	if (CFpga::s_pFpga->IsRubidium()) {
		m_ocxoRatio = OCXO_CURRENT_CONVERT_RUBIDIUM;	
	}
}

// option 0: current value. 1: Filtered.  2: min   3: max
double CAdcConverted::ReadConverted(int chan, int8 option /*= 0*/)
{
	uint16 value = 0;
	double ret = 0.0;

	switch (option) {
	case OPTION_CURRENT: value = Read(chan); break;
	case OPTION_ISTATE_READ:
	case OPTION_FILTER: value = ReadFiltered(chan); break;
	case OPTION_MIN: value = ReadMin(chan); break;
	case OPTION_MAX: value = ReadMax(chan); break;
	case OPTION_FROM: value = ReadRangeFrom(chan); break;
	case OPTION_TO: value = ReadRangeTo(chan); break;
	}
	switch (chan) {
	case ADC_CHAN_OCXO_CURRENT: // OCXO current in mA
		ret = m_ocxoRatio * value ; // (2.5 * value / 1023) / (78.1/10) / 0.1 * 1000 / 8
		break;
	default:
		ret = DEFAULT_CONVERT * value;
		if (option == OPTION_ISTATE_READ) {
			ret *= 1000;
		}
		break;
	}
	return ret;
}

void CAdcConverted::SetNormalRange(int chan, double from, double to)
{
	uint16 intFrom;
	uint16 intTo;

	switch (chan) {
	case ADC_CHAN_OCXO_CURRENT:
		intFrom = from / m_ocxoRatio;
		intTo = to / m_ocxoRatio;
		break;
	default: 
		intFrom = from / DEFAULT_CONVERT;
		intTo = to / DEFAULT_CONVERT;
		break;
	}
	
	if (intTo > 1024) {
		intTo = 1024;
	}

	CAdc::SetNormalRange(chan, intFrom, intTo);
}

const char * CAdcConverted::GetAdcChanName(int chan)
{
	if ((chan >=0) && (chan < NUM_CHAN)) {
		return m_chanNameList[chan];
	}
	return NULL;
}

void CAdcConverted::PrintAdc(Ccomm *pComm, unsigned long *param)
{
	int chan;
	char buff[200];
	double adc;
	double adcMin;
	double adcMax;
	double currentAdc;
	double adcFrom;
	double adcTo;
	int option = -1;
	int ok = 0;
	uint16 value;
	uint16 mask;
	uint16 rawFrom;
	uint16 rawTo;

	if (pComm == NULL) return;
	if (*param == 1) {
		option = param[1];
	}

#if 0
	sprintf(buff, "OCXO ratio: %f\r\n", m_ocxoRatio);
	pComm->Send(buff);
#endif

	if ((option > 0) && (option < 10)) {
		mask = 1;
		for (chan = 0; chan < NUM_CHAN; chan++) {
			value = Read(chan);
			currentAdc = ReadConverted(chan, OPTION_CURRENT);
			adc = ReadConverted(chan, OPTION_FILTER);
			adcMin = ReadConverted(chan, OPTION_MIN);
			adcMax = ReadConverted(chan, OPTION_MAX);
			adcFrom = ReadConverted(chan, OPTION_FROM);
			rawFrom = ReadRangeFrom(chan);
			adcTo = ReadConverted(chan, OPTION_TO);
			rawTo = ReadRangeTo(chan);
			switch (option) {
			case 1:
				sprintf(buff, "Chan[%02d] %8.4f  0x%04X %s\r\n", chan,
					(double)value * 2.5 / 1023, value, GetAdcChanName(chan));
				break;
			case 2:
				if (!(mask & m_enableMask)) {
					buff[0] = 0;
					break;
				}
				sprintf(buff, "Chan[%02d]\t %8.4f\t %s\r\n", chan, 
					currentAdc, GetAdcChanName(chan));
				break;
			case 3: {
				if (!(mask & m_enableMask)) {
					buff[0] = 0;
					break;
				}
				sprintf(buff, "Chan[%02d]\t %8.4f [Current:%8.4f]\t %s\r\n", chan, 
					adc, currentAdc, GetAdcChanName(chan));
					}
				break;
			case 4: {
				sprintf(buff, "Chan[%02d]\t %8.4f [Current:%8.4f Min: %8.4f Max: %8.4f]\t%d\t%s\r\n", chan, 
					adc, currentAdc, adcMin, adcMax, m_count[chan], GetAdcChanName(chan));
					}
				break;
			case 5:
				if (!(mask & m_enableMask)) {
					buff[0] = 0;
					break;
				}
				sprintf(buff, "Chan[%2d]  %8.4f\t %s\t"
					"Range %8.4f(0x%04X) -- %8.4f(0x%04X)\tOORCnt:%ld\r\n"
					, chan, currentAdc, GetAdcChanName(chan)
					, adcFrom, rawFrom, adcTo, rawTo, m_oorCnt[chan]);
				break;
			default:
				ok = -1;
				break;
			}
			ok++;
			if (ok > 0) pComm->Send(buff);
			mask <<= 1;
		}
	} else if (option == 10) {
		sprintf(buff, "Mask: %04X\r\n", m_enableMask);
		pComm->Send(buff);
		ResetMinMax();
		ok = 1;
	} else if ((*param == 4) && (param[1] == 11)) {
		CAdc::SetNormalRange(param[2], param[3], param[4]);
		ok = 1;
	}

	if (ok <= 0) {
		pComm->Send("Option:\r\n" "\t1: List raw data\r\n"
			"\t2: List converted data\r\n" "\t3: List filered data\r\n"
			"\t4: List filter data and min/max\r\n"
			"\t5: List converted data with range\r\n"
			"\t10: Clear min/max\r\n"
			"\t11 <chan> <from> <to>: Set ADC range with raw data\r\n" );
	}
}


