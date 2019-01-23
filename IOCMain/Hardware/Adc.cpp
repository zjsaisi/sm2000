// Adc.cpp: implementation of the CAdc class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Adc.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Adc.cpp 1.2 2008/04/18 11:03:47PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "CodeOpt.h"
#include "Adc.h"
#include "machine.h"
#include "iodefine.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CAdc *s_pAdc = NULL;

CAdc::CAdc(uint16 enable_mask)
{
	int i;
	m_enableMask = enable_mask;
	m_bDone = 1;

	// Enable ADC
	MSTP.CRA.BIT.B1 = 0;
	AD.CSR.BIT.ADIE = 0; // Disbale intr now
	AD.CSR.BIT.ADST = 0; // Stop ADC if any
	// AD.DRA AD.DRB AD.DRC AD.DRD
	if (s_pAdc == NULL) {
		s_pAdc = this;
	}
  	INTC.IPRE.BIT.LOW  = INT_LEVEL_ADC;  // Set A/D,WDT1- monitor ananlog levels
	
	memset(m_data, 0, sizeof(m_data));
	m_pFilter = new CAdcFilter[16];
	ResetMinMax();
	m_group = 0;

	memset(m_dataNormalFrom, 0, sizeof(m_dataNormalFrom));
	memset(m_dataNormalTo, 0xff, sizeof(m_dataNormalTo));
	for (i = 0; i < 16; i++) {
		m_dataNormalTo[i] = 1024;
		m_oorCnt[i] = 0L;
	}
}

CAdc::~CAdc()
{

}

void CAdc::Init(void)
{
}

void CAdc::isr_adc()
{
	// Collect data from previous conversion
	uint16 *ptr;
	uint16 tmp;
	uint16 *ptrMin;
	uint16 *ptrMax;
	int offset;

	// Stop it from scan mode
	AD.CSR.BIT.ADST = 0;
	// Make sure it's legal data. Should never be illegal at this point
	if ((m_group < 0) || (m_group > 3)) {
		AD.CSR.BIT.ADIE = 0;
		return;
	}
	// m_group &= 3;
	if (AD.CSR.BIT.ADF) {
		offset = m_group << 2;
		ptr = m_data + offset;
		ptrMin = m_dataMin + offset;
		ptrMax = m_dataMax + offset;
		
		m_count[offset]++;
		m_count[offset+1]++;
		m_count[offset+2]++;
		m_count[offset+3]++;

		*ptr = tmp = AD.DRA >> 6; 
		if (tmp > *ptrMax) {
			*ptrMax = tmp;
		}
		if (tmp < *ptrMin) {
			*ptrMin = tmp;
		}
		ptr++; ptrMin++; ptrMax++;


		*ptr = tmp = AD.DRB >> 6; 
		if (tmp > *ptrMax) {
			*ptrMax = tmp;
		}
		if (tmp < *ptrMin) {
			*ptrMin = tmp;
		}
		ptr++; ptrMin++; ptrMax++;
		
		*ptr = tmp = AD.DRC >> 6;
		if (tmp > *ptrMax) {
			*ptrMax = tmp;
		}
		if (tmp < *ptrMin) {
			*ptrMin = tmp;
		}
		ptr++; ptrMin++; ptrMax++;
		
		*ptr = tmp = AD.DRD >> 6; 
		if (tmp > *ptrMax) {
			*ptrMax = tmp;
		}
		if (tmp < *ptrMin) {
			*ptrMin = tmp;
		}

		AD.CSR.BIT.ADF = 0;
	}
    
	// Setup next converion if necessary
	NextGroup();
	if (m_group != -1) {
		AD.CSR.BIT.ADST = 1;
	} else {
		// Last one
		AD.CSR.BIT.ADIE = 0;
	}
}

void CAdc::StartAdc(void)
{
	// Put existing data away in filter
	for (int i = 0; i < 16; i++) {
		m_pFilter[i].PutData(m_data[i]);

		// check range
		if (m_data[i] < m_dataNormalFrom[i]) {
			m_oorCnt[i]++;
		} else if (m_data[i] > m_dataNormalTo[i]) {
			m_oorCnt[i]++;
		}
	}

	m_group = -1;
	NextGroup();
	if ((m_group >= 0) && (m_group <= 3)) {
		// Valid group
		AD.CSR.BIT.SCAN = 1; // Use scan mode  
		AD.CR.BIT.TRGS = 0; // Software control
		AD.CR.BIT.CKS = 0; // Choose the slowest one. Hopefully it's the most precise one.
		AD.CSR.BIT.ADIE = 1;
		AD.CSR.BIT.ADST = 1; // Start adc conversion
	} else {
		AD.CSR.BIT.ADIE = 0;
		AD.CSR.BIT.ADST = 0;
	}


}

EXTERN void isr_adc(void)
{
	if (s_pAdc) {
		s_pAdc->isr_adc();
	}
}


void CAdc::NextGroup()
{
	const uint8 channels[4] = {3, 7, 11, 15}; 
	int i;
	uint16 mask;
	m_group++;
	mask = ((uint16)0xf) << (m_group * 4);
	for (; m_group < 4; m_group++) {
		if (m_enableMask & mask) break;
		mask <<= 4;
	}
	if ((m_group >= 4) || (m_group < 0)) {
		m_group = -1;
	} else {
		// Group desirable
		AD.CSR.BIT.CH = channels[m_group];
	}
}

uint16 CAdc::Read(int chan)
{
	// If chan is out of range and is 0..16. it will return 0.
	return (m_data[chan]);
}

uint16 CAdc::ReadFiltered(int chan)
{
	return (m_pFilter[chan].GetAverge());
}

void CAdc::ResetMinMax()
{
	for (int i = 0; i < 16; i++) {
		m_dataMin[i] = 1024;
		m_dataMax[i] = 0;
		m_count[i] = 0;
	}
}

uint16 CAdc::ReadMin(int chan)
{
	return (m_dataMin[chan]);
}

uint16 CAdc::ReadMax(int chan)
{
	return (m_dataMax[chan]);
}

void CAdc::SetNormalRange(int chan, uint16 from, uint16 to)
{
	if ((chan < 0) || (chan >= 16)) return;
	m_dataNormalFrom[chan] = from;
	m_dataNormalTo[chan] = to;
	// to start within range
	if (m_data[chan] < from) {
		m_data[chan] = from;
	}
	if (m_data[chan] > to) {
		m_data[chan] = to;
	}
}

//1: OK
// -1: out of range
int CAdc::IsWithinRange(int chan)
{
	uint16 adc;
	if ((chan < 0) || (chan >= 16)) return 0;
	adc = ReadFiltered(chan);
	if (adc < m_dataNormalFrom[chan]) return -1;
	if (adc > m_dataNormalTo[chan]) return -1;
	return 1;
}

uint16 CAdc::ReadRangeFrom(int chan)
{
	return m_dataNormalFrom[chan];
}

uint16 CAdc::ReadRangeTo(int chan)
{
	return m_dataNormalTo[chan];
}
