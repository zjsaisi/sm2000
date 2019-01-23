// CardAlign.cpp: implementation of the CCardAlign class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardAlign.cpp 1.9 2010/10/14 00:11:26PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.9 $
 */

#include "CardAlign.h"
#include "string.h"
#include "Fpga.h"
#include "stdio.h"
#include "IstateDti.h"
#include "machine.h"
#include "CardProcess.h"
#include "RTC.h"

#define TRACE TRACE_CARD

// TC1000 Accordind to George Z: Set it to 250ns for now 4/19/2006.
// TC1000 (10.24M*256/35) * (250*10^-9) = 18.7
// TP5000 20.48M * 250 * 10^-9 = 5  350ns= 7
#define GPS_PHASE_TIGHT_LIMIT 5 /* 19 */
// about 350ns
#define GPS_PHASE_TIGHT_HOLD_LIMIT 7 /* 26 */
// Actual number is 253.68 ns

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCardAlign *CCardAlign::s_pCardAlign = NULL;
int CCardAlign::m_todSource = -1;

CCardAlign::CCardAlign()
{
	m_signedGpsPhase = 0;
	m_cardAlign = CARD_ALIGN_LOOSE;
	m_stop = 0;
	m_twinAlign = CARD_ALIGN_LOOSE;
	s_pCardAlign = this;
	m_gpsPhase = 0L;
	m_pReportGpsPhase = (int32 *)CIstateDti::de_gpsPhaseError.pVariable;
	m_preReportGpsPhase = 0L;
}

CCardAlign::~CCardAlign()
{

}

// return 1 if changed.
int CCardAlign::TickCardAlign()
{
	ReadGpsPhase();

	if (m_stop) {
		return 0;
	}

	switch (m_cardAlign) {
	case CARD_ALIGN_LOOSE:
		return TickLoose();
	case CARD_ALIGN_TIGHT:
		return TickTight();
	default:
		TransitAlign(CARD_ALIGN_LOOSE);
		return 1;
	}
}

void CCardAlign::CardAlignFail()
{
	m_stop = 2;
	m_cardAlign = CARD_ALIGN_LOOSE;
}

void CCardAlign::CardAlignOffline()
{
	m_stop = 1;
	m_cardAlign = CARD_ALIGN_LOOSE;

}

void CCardAlign::Init()
{

}

void CCardAlign::SetTwinAlign(int align)
{
	m_twinAlign = align;
}

int CCardAlign::GetTwinAlign() const
{
	return m_twinAlign;
}

const char * CCardAlign::GetName(int state)
{
	switch (state) {
	case CARD_ALIGN_TIGHT:
		return "TIGHT";
	case CARD_ALIGN_LOOSE:
		return "LOOSE";
	default: 
		return "UNKNOWN";
	}

}

void CCardAlign::CfTodSource(int source)
{
	if (m_todSource == source) return;
	m_todSource = source;
}

void CCardAlign::ReadGpsPhase()
{
	// int32 ret;
	int32 phase;
	int32 diff;
	//GPZ get phase in either hybrid or GPS case
	if((m_todSource == CIstateDti::TOD_SOURCE_GPS) || (m_todSource == CIstateDti::TOD_SOURCE_HYBRID))
	{
		phase = CFpga::s_pFpga->GetGPSPhase(); 

	}
	else
	{
		phase = 0;	 
	}
//	phase = (m_todSource == CIstateDti::TOD_SOURCE_GPS) ? (CFpga::s_pFpga->GetGPSPhase()) : (0); 
#if 0 // zmiao 5/22/2008 it is not needed. It's not about BT3
	phase= phase&0x7FFFFFFF;
	if(phase&0x40000000)
	{
	 phase=phase|0x80000000;
	}
#endif
	if (phase < 0) {
		m_gpsPhase = -phase;
	} else {
		m_gpsPhase = phase;
	}

#if 0 
	if (ret & 0x80000000) {
		m_gpsPhase = 0x7fffffffL;
		phase = 0x7fffffffL;
	} else {
		if (ret & 0x40000000) {
			ret |= 0x80000000;
			phase = ret;
			ret = -ret; // don't care the sign here. Only the absolute is of my concern
		} else {
			phase = ret;
		}
		m_gpsPhase = ret;
	}
#endif

	if (m_preReportGpsPhase != phase) {
		uint8 tmp_exr;

		m_preReportGpsPhase = phase;
		tmp_exr = get_exr();
		set_exr(7);
		*m_pReportGpsPhase = phase;
		set_exr(tmp_exr);

	}
	
	diff = m_signedGpsPhase - phase;
	if ((diff >= 5) || (diff <= -5)) {
		int year, month, day, hr, minute, second;
		uint16 high, low;
		int  dither, cnt;

		g_pRTC->GetDateTime(&year, &month, &day, &hr, &minute, &second);
		TRACE("GPS phase jump: %ld --> %ld @%04d-%02d-%02d %02d-%02d-%02d\r\n"
			, m_signedGpsPhase, phase, year, month, day, hr, minute, second);
		CFpga::s_pFpga->GetCurrentEfcControl(&high, &low, &dither, &cnt);
		TRACE("EFC Ctrl: high=%04X low=%04X dither=%d Dcnt=%d\r\n",
			high, low, dither, cnt);
	}
	m_signedGpsPhase = phase;
}

int CCardAlign::TickLoose()
{
	if (m_gpsPhase > GPS_PHASE_TIGHT_LIMIT) {
		return 0;
	}
	TransitAlign(CARD_ALIGN_TIGHT);
	return 1;
}

int CCardAlign::TickTight()
{
	if (m_gpsPhase > GPS_PHASE_TIGHT_HOLD_LIMIT) {
		TransitAlign(CARD_ALIGN_LOOSE);
		return 1;
	}
	return 0;
}

int CCardAlign::TransitAlign(int state)
{
	if (m_cardAlign == state) return 0;
	
	m_cardAlign = state;
}

void CCardAlign::CardAlignHack(Ccomm *pComm)
{
	char buff[200];
	sprintf(buff, "twinAlign: %d\r\nGPS phase:%ld\r\n"
		"Stop: %d\r\n"
		, m_twinAlign, m_gpsPhase
		, m_stop);
	pComm->Send(buff);
}
