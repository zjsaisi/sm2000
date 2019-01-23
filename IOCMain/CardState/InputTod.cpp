// InputTod.cpp: implementation of the CInputTod class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputTod.cpp 1.5 2009/04/30 15:06:22PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.5 $
 */             

#include "InputTod.h"
#include "CardProcess.h"
#include "string.h"
#include "Fpga.h"
#include "stdio.h"
#include "IstateProcess.h"
#include "machine.h"
#include "IstateDti.h"
#include "SecondProcess.h"

#define TRACE TRACE_CARD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CInputTod::m_activeInputTod = -1;
int8 CInputTod::m_forceChange = 1;

CInputTod::CInputTod(int chan)
{
	m_totalLeap = 0;
	m_gpsSec = 0L;
	m_leapPending = 0;
	m_leapPendingLast = 0;
	m_localOffset = 0L;
	m_localOffsetLast = 0L;
	m_pReportState = NULL;
	m_pReportTimeSettingMode = NULL;

	m_chan = chan;
	m_index = m_chan - CHAN_DTI1;
	m_stableCnt = 0;
	m_totalLeapLast = 1000;
	m_errorCnt = 0L;
	m_validCnt = 0L;
	m_isValid = 0;
	m_todStatus = 0;
}

CInputTod::~CInputTod()
{

}

int CInputTod::ParseTod(const unsigned char *pMsg)
{
	int valid = 1;
	
	int leapPending = 0;
	unsigned char totalLeap;
	uint32 gpsSec;
	int32 localOffset;
	uint8 todStatus;
	uint8 tmp_exr;

	todStatus = pMsg[0];

	if ((todStatus & 0x3) != 1) {
		valid = 0;
	} else if (pMsg[6] != '*') {
		valid = 0;
	} else if (pMsg[12] != '.') {
		valid = 0;
	} else if (pMsg[23] != '.') {
		valid = 0;
	} else if (pMsg[36] != '.') {
		valid = 0;
	} else if (pMsg[38] != '.') {
		valid = 0;
	} else if (pMsg[40] != 0xd) {
		valid = 0;
	}
	
	memcpy(&gpsSec, pMsg + 1, 4);
	gpsSec += 1; // FPGA will increase by itself. //2;
	totalLeap = pMsg[5];

	// Pending leap
	if (valid) {
		switch (pMsg[39]) {
		case '+':
			leapPending = 1;
			break;
		case '-':
			leapPending = -1;
			break;
		case '0':
			leapPending = 0;
			break;
		default:
			valid = 0;
			break;
		}
	}

	// Local offset
	if (valid) {
		int hr;

		hr = (pMsg[34] - '0') * 10 + pMsg[35] - '0';
		hr *= 60;
		if (pMsg[37] == '5') {
			hr += 30;
		} else if (pMsg[37] != '0') {
			valid = 0;
		}
		if ((hr < 0) || (hr > 780)) {
			valid = 0;
		}
		if (pMsg[33] == '-') {
			hr = -hr;
		} else if (pMsg[33] != '+') {
			valid = 0;
		}
		
		localOffset = (int32)hr * 60;
	}

	if (valid) {
		if (gpsSec != m_gpsSec + 1) {
			valid = 0;
			m_gpsSec = gpsSec;
		}
	}

	// Logic invalid
	if (((todStatus & 0xc) == 0) && (valid)) { 
		TodInvalid(0, 0);
		return 0;
	}

	// decode done
	if (!valid) {
		TodInvalid(0);
		return 0;
	}

	m_validCnt++;
	if (++m_stableCnt < 0) {
		m_stableCnt = 0;
	}
	if (m_totalLeapLast != totalLeap) {
		m_stableCnt = 0;
		m_totalLeapLast = totalLeap;
	}
	if (m_leapPendingLast != leapPending) {
		m_stableCnt = 0;
		m_leapPendingLast = leapPending;
	}
	if (m_localOffsetLast != localOffset) {
		m_stableCnt = 0;
		m_localOffsetLast = localOffset;
	}

	if (m_stableCnt >= 1) {
		// can use the info
		int postIt = (m_forceChange) ? (1) : (0);
		uint32 fpgaSec;

		if (m_totalLeap != totalLeap) {
			m_totalLeap = totalLeap;
			postIt = 1;
		}
		if (m_leapPending != leapPending) {
			m_leapPending = leapPending;
			postIt = 1;
		}
		if (m_localOffset != localOffset) {
			m_localOffset = localOffset;
			postIt = 1;
		}
			
		if (m_activeInputTod == m_chan) {
			if (postIt) {
				PostTod();
			}
			fpgaSec = CFpga::s_pFpga->GetGpsSec();
			if (fpgaSec != gpsSec) {
				CFpga::s_pFpga->SetGpsSec(gpsSec);
				CSecondProcess::s_pSecondProcess->SetTodValid(1);
				// CSecondProcess::s_pSecondProcess->UpdateRtc();
				SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_SYNC_RTC_FROM_FPGA);
				SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_TOD_VALID_ON_STANDBY);
				TRACE("GPS sec reset to input[%d] tod\r\n", m_chan);
			} else if (((m_stableCnt % 60) == 59) || (m_forceChange)) {
				CSecondProcess::s_pSecondProcess->SetTodValid(1);
				SET_MY_ISTATE(COMM_RTE_KEY_COMMON_ACTION, 0, 0, CIstateDti::COMMON_ACTION_TOD_VALID_ON_STANDBY);
			}

			if (m_forceChange || (m_todStatus != todStatus)) {
				CSecondProcess::s_pSecondProcess->SetTimeSettingMode(todStatus >> 4);
				SET_TWIN_ISTATE(COMM_RTE_KEY_SINGLE_ACTION, 0, 0, 0x100 + (todStatus >> 4));
				m_todStatus = todStatus;
			}

			if (m_forceChange) {
				m_forceChange--;
			}
		} else if (postIt) {
			m_totalLeap |= 0x1000;
		}

		if (m_stableCnt >= 3000) {
			m_stableCnt = 2000;
			m_forceChange = 5;
		} else if ((m_stableCnt & 3) == 0) {
			if (m_isValid <= 0) m_isValid = 1;
		}
	}

	m_gpsSec = gpsSec;


	tmp_exr = get_exr();
	set_exr(7);
	{	// report time setting
		uint8 mode = todStatus >> 4;
		if (mode > 3) mode = 16;
		*m_pReportTimeSettingMode = (int32)mode;
		*m_pReportState = (int32)1;
	}
	set_exr(tmp_exr);

	return valid;
}

void CInputTod::TodInvalid(int isLos, int todState)
{
	if (m_stableCnt > 0) {
		m_stableCnt = 0;
	} else if (m_stableCnt < -20000) {
		m_stableCnt = -10000;
	} else {
		m_stableCnt--;
		if ((m_stableCnt & 7) == 0) {
			if (isLos) {
				m_isValid = 0;
			} else if (m_isValid >= 0) {
				m_isValid = -1;
			}
		}
	}
	m_totalLeap |= 0x1000;
	m_totalLeapLast |= 0x2000;
	m_errorCnt++;

	NoTod(todState);
}

void CInputTod::PostTod()
{
	SET_MY_ISTATE(COMM_RTE_KEY_TOTAL_LEAP_SECOND, 0, 0, (int32)(m_totalLeap & 0xff));
	SET_MY_ISTATE(COMM_RTE_KEY_LEAP_PENDING, 0, 0, (int32)m_leapPending);
	SET_MY_ISTATE(COMM_RTE_KEY_TIME_ZONE, 0, 0, m_localOffset);
	TRACE("Set LEAP from Input[%d], Leap=%d Pend=%d Offset=%ld\r\n",
		m_chan, m_totalLeap, m_leapPending, m_localOffset);
}

void CInputTod::SetActiveInput(int chan)
{
	if (m_activeInputTod != chan) {
		m_activeInputTod = chan;
		m_forceChange = 5;
	}
}

void CInputTod::InputTodHack(unsigned long *param, Ccomm *pComm)
{
	char buff[150];

	sprintf(buff, "TOD message: GPSSEC=%ld(0x%lX) Stable=%d isValid=%d\r\n"
		"\tLeap=%d Pend=%d Local=%ld\r\n"
		, m_gpsSec, m_gpsSec, m_stableCnt, m_isValid
		, m_totalLeap, m_leapPending, m_localOffset
		);
	pComm->Send(buff);
	sprintf(buff, "\tErrorCnt=%ld ValidCnt=%ld\r\n"
		"ForceChange=%d\r\n"
		, m_errorCnt, m_validCnt
		, m_forceChange
		);
	pComm->Send(buff);
}

void CInputTod::CardInactive()
{
	m_stableCnt = 0;
	m_totalLeap |= 0x1000;
	m_errorCnt = 0L;
	m_validCnt = 0L;
	m_isValid = 0;
	m_forceChange = 5;

	NoTod();
}

int CInputTod::IsTodValid()
{
	if (m_isValid) return m_isValid;
	if (m_stableCnt >= -1) return 1;
	return 0;
}

void CInputTod::Init()
{
	m_pReportTimeSettingMode = ((int32 *)CIstateDti::de_inputTodSettingMode.pVariable) + m_index;
	m_pReportState = ((int32 *)CIstateDti::de_inputTodState.pVariable) + m_index;
}

void CInputTod::NoTod(int todState)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	*m_pReportTimeSettingMode = (int32)16;
	*m_pReportState = (int32)todState;
	set_exr(tmp_exr);

}

void CInputTod::ForceChange()
{
	m_forceChange = 5;
}
