// InputGps.cpp: implementation of the CInputGps class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputGps.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputGps.cpp 1.12 2011/08/26 11:20:43PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.12 $
 */             


#include "InputGps.h"
#include "InputSpan.h"
#include <string.h>
#include <stdio.h>
#include "Fpga.h"
#include "IstateDti.h"
#include "InputSelect.h"
#include "BT3Support.h"
#include "CardProcess.h"
#include "IstateProcess.h"
#include "AllProcess.h"
#include "CardStatus.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputGps *CInputGps::s_pInputGps;

CInputGps::CInputGps(int chan, const char *pName) : 
CInput(chan, pName, 0)
{
	m_ignoreImc = 0;
	s_pInputGps = this;
	m_cfValid = 1;
	m_assumedQL = -1;
#if 0 	
	m_taTuneError = 0;
	m_tpiuTuneError = 0;
	m_gpsPathCrcError = 0;
#endif	
}

CInputGps::~CInputGps()
{

}

void CInputGps::InitChan()
{
	CInput::InitChan();
}

int CInputGps::InputHack(unsigned long *param, Ccomm *pComm)
{
	int ret;
	char buff[256];

	// Hack GPS input info


	ret = CInput::InputHack(param, pComm);
	if (ret < 0) return ret;
	
	// Print GPS input help info
	sprintf(buff, "\tm_assumedQL: %d\r\n", m_assumedQL);
	pComm->Send(buff);
		
	return 0;
}

#if 0 
void CInputGps::CountTuneCrcError(void)
{
	if (CFpga::s_pFpga->IsGpsPathCrcOk() > 0) {
		m_gpsPathCrcError = 0;
	} else {
		m_gpsPathCrcError++;
		return; // CRC error happens, don't look any further
	}

	if (CFpga::s_pFpga->IsTaTuneOk() > 0) {
		m_taTuneError = 0;
	} else {
		m_taTuneError++;
	}

	if (CFpga::s_pFpga->IsTpiuTuneOk() > 0) {
		m_tpiuTuneError = 0;
	} else {
		m_tpiuTuneError++;
	}

}
#endif

int CInputGps::ReadStatus()
{
	int rgpError;

	// CountTuneCrcError(); // Don't apply to TP5000

	if (CFpga::s_pFpga->GetGpsLos()) {
		return UpdateInputStatus(INPUT_STAT_LOS);
	}
    
#if 0 
	if (m_taTuneError >= 3) return UpdateInputStatus(INPUT_STAT_TA_TUNING_OUT_OF_RANGE);
	if (m_tpiuTuneError >= 3) return UpdateInputStatus(INPUT_STAT_TPIU_TUNING_OUT_OF_RANGE);
	if (m_gpsPathCrcError >= 3) return UpdateInputStatus(INPUT_STAT_GPS_PATH_CRC_ERROR);
#endif

	rgpError = CBT3InBound::s_pBT3InBound->GetPhysicalError();

	if (!m_ignoreImc) {
		if (rgpError >= 100) {
			return UpdateInputStatus(INPUT_STAT_GPS_ACQUIRING);
		} else if (rgpError) {
			return UpdateInputStatus(INPUT_STAT_NO_RGP);
		}
	}

	if (IS_ACTIVE) {
		if (CBT3OutBound::s_pBT3OutBound->IsGPSClear() != 1) {
			return UpdateInputStatus(INPUT_STAT_BT3_UNSETTLED);
		}
	}

	return UpdateInputStatus(INPUT_STAT_OK);
	// For debug only
	//return CInput::ReadStatus();
}

#if 0 // Don't do that. zmiao 8/13/2010
//GPZ June 2010 make another public version of above
int CInputGps::ReadStatus_GPS()
{
	int rgpError;

	// CountTuneCrcError(); // Don't apply to TP5000

	if (CFpga::s_pFpga->GetGpsLos()) {
		return UpdateInputStatus(INPUT_STAT_LOS);
	}

	rgpError = CBT3InBound::s_pBT3InBound->GetPhysicalError();

	if (!m_ignoreImc) {
		if (rgpError >= 100) {
			return UpdateInputStatus(INPUT_STAT_GPS_ACQUIRING);
		} else if (rgpError) {
			return UpdateInputStatus(INPUT_STAT_NO_RGP);
		}
	}

	if (IS_ACTIVE) {
		if (CBT3OutBound::s_pBT3OutBound->IsGPSClear() != 1) {
			return UpdateInputStatus(INPUT_STAT_BT3_UNSETTLED);
		}
	}

	return UpdateInputStatus(INPUT_STAT_OK);
	// For debug only
	//return CInput::ReadStatus();
}
#endif 

void CInputGps::SetIgnoreImc(int ignore)
{
	m_ignoreImc = ignore;
}

int CInputGps::GetCurrentQL(void)
{
	return m_assumedQL;
}

int CInputGps::InputSetup(void)
{
	CBT3InBound::s_pBT3InBound->SetInputPriority(m_chan - CHAN_GPS + INPUT_CHAN_GPS, m_priority);
	return 0;
}


int CInputGps::CfAssumedQLChanInput(int assumed)
{
	if (m_assumedQL == assumed) return 0;
	m_assumedQL = assumed;
	//RefreshInputSetup();
	m_changed = 1;
	return 1;
}



/*
don zhang: April 20, 2011:

GetPriority is called by CInputSelect::ChooseActiveInput() when performing 
an input selection.

GetPriority is multiplexed and the name may be a misnomer:
If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_SSM, GetPriority will 
return a combined priority number=predefined hardcoded priority (low byte) 
+ input pql (if available) or user specified pql COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL 
(high byte) if not available.
This combined priority number is used to achieve the following: if two or more inputs 
have the same SSM (aka PQL, QL, quality level), use the predefined priority 
as a tie breaker.
The predefined priority is hardcoded as follows: 
1=gps, 5=INP1, 6=INP2 (see EFS for TimeProvider 5000 Release 2).

If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_PRIORITY, return the user 
specified priority (COMM_RTE_KEY_SPAN_INPUT_PRIORITY)

If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_MANUAL, return the user 
specified priority (COMM_RTE_KEY_SPAN_INPUT_PRIORITY). The return value does not 
matter in this case.


-------------------------------------------------------------------------------
From TimeProvider 5000 2.0 Release SOFTWARE DESIGN DOCUMENT (by Jining Yang):
When reference selection criteria (Istate COMM_RTE_KEY_REFERENCE_SELECT_METHOD) 
is set to SSM, also uses PQL for GPS input.

When m_refSelectMethod is CIstateDti::REFERENCE_SELECT_SSM, also perform the 
following:
a. When TOD source is GPS (time mode) (Istate COMM_RTE_KEY_TOD_SOURCE), select GPS 
as active reference if GPS PQL is not worse than oscillator QL. If GPS PQL is 
worse than oscillator QL, then select SPAN input (the selection between SPAN inputs 
are same as before).

b. When TOD source is SYS (frequency mode), select active reference among GPS, SPAN-1 
(aka INP1, BITS1) and SPAN-2 (aka INP2, BITS2) based on PQL.
*/
int CInputGps::GetPriority(void)
{
	/*
	priority: lower priority is better (higher rank), legal range is 1~9 from user perspective, 
	but INPUT_COMBINED_PRIORITY_INVALID (200) is also OK internally, which effectively disables the input;
	likewise, 0 is used internally as a hack to select and use an input.
	*/
	int combinedpriority = BUILD_UINT16(0, INPUT_COMBINED_PRIORITY_INVALID); 
	
	switch(CInputSelect::m_refSelectMethod)
	{
	case CIstateDti::REFERENCE_SELECT_SSM:
		if(CIstateDti::TOD_SOURCE_GPS == m_todSource)
		{
			/*time mode:
			When TOD source is GPS (time mode) (Istate COMM_RTE_KEY_TOD_SOURCE), select GPS 
			as active reference if GPS PQL is better than oscillator QL. If GPS PQL is 
			worse than oscillator QL, then select SPAN input (the selection between SPAN inputs 
			are same as before)
			*/
			if(m_assumedQL <= CAllProcess::GetOscQL())
			{
				//0 is used internally as a hack to force 
				//CInputSelect::ChooseActiveInput to always select/use the GPS input.
	            combinedpriority = 0; 
			}
			else
			{
				//GPS PQL is worse than oscillator QL:
				//INPUT_COMBINED_PRIORITY_INVALID is used internally as a hack to disable the GPS input.
	            combinedpriority = BUILD_UINT16(0, INPUT_COMBINED_PRIORITY_INVALID); 
			}
		}
		else
		{
			/*user/sys/frequency mode:
			When TOD source is SYS (frequency mode), select active reference among 
			GPS, SPAN-1 (aka INP1, BITS1) and SPAN-2 (aka INP2, BITS2) based on PQL.
			1=hardcoded gps predefined priority.
			*/
			//(lowbyte, highbyte)
			combinedpriority = BUILD_UINT16(1, m_assumedQL);
		}
		break;

	//when gps is valid and in time mode, always use gps, don zhang, June 17, 2011:
	case CIstateDti::REFERENCE_SELECT_PRIORITY:
		if(CIstateDti::TOD_SOURCE_GPS == m_todSource)
		{
			/*time mode:
			When TOD source is GPS (time mode) (Istate COMM_RTE_KEY_TOD_SOURCE), 
			always select GPS as active reference. Don zhang, June 17, 2011:
			*/
			combinedpriority = 0;
		}
		else
		{
			combinedpriority = BUILD_UINT16(0, CInput::GetPriority());
		}
		break;

	case CIstateDti::REFERENCE_SELECT_MANUAL:
		combinedpriority = BUILD_UINT16(0, CInput::GetPriority());
		break;
	}
	
	return combinedpriority;
}



