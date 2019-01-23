// InputDti.cpp: implementation of the CInputDti class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputDti.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputDti.cpp 1.5 2009/04/29 17:50:52PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.5 $
 */             


#include "InputDti.h"
#include "CardLoop.h"  
#include "Fpga.h"
#include "stdio.h"                    
#include "DtiOutput.h"  
#include "InputSelect.h"
#include "machine.h"
#include "IstateDti.h"
#include "IstateProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputDti::CInputDti(int chan, const char *pName) 
: CInput(chan, pName, 1), m_pathReport(chan), m_todReport(chan)
{
	m_pReportServerLoopStatus = NULL;
	m_pReportCableAdvance = NULL;
	m_serverTrack = 0;
	m_serverFlag = 0;

	memset(m_path, 0xff, sizeof(m_path));
	memset(m_todMessage, 0xff, sizeof(m_todMessage));
	m_inputDegraded = 0;
	m_errorCnt = 0L;
	m_errorSecond = 0L;
	m_serverClockType = -1;
	m_serverExternalTimingSource = -1;
	m_cfValid = 0;
}

CInputDti::~CInputDti()
{

}

void CInputDti::InitChan()
{
	CInput::InitChan();
	m_todReport.Init();
	//m_pReportServerClockType = ((int32 *)CIstateDti::de_inputServerClockType.pVariable) + (m_chan - CHAN_DTI1);
	//m_pReportServerExternalTimingSource = 
	//	((int32 *)CIstateDti::de_inputServerExternalTimingSource.pVariable) + (m_chan - CHAN_DTI1);
	m_pReportServerLoopStatus = 
		((int32 *)CIstateDti::de_inputServerLoopStatus.pVariable) + (m_chan - CHAN_DTI1);
	m_pReportCableAdvance = 
		((int32 *)CIstateDti::de_inputCableAdvance.pVariable) + (m_chan - CHAN_DTI1);
}

int CInputDti::InputHack(unsigned long *param, Ccomm *pComm)
{
	int ret;
	char buff[100];

	ret = CInput::InputHack(param, pComm);
	if (ret < 0) return ret;

	// Print extra help info
	// Hack info for DTI input
	sprintf(buff, "DTI level report\r\n\tErrorSecond=%ld CRC error=%ld\r\n"
		, m_errorSecond, m_errorCnt);
	pComm->Send(buff);

	sprintf(buff, "\tCurrent CRC error: %d ServerTrack: 0x%X InputDegrade: %d\r\n"
		, CFpga::s_pFpga->getInputCrcError(m_chan), m_serverTrack, m_inputDegraded
		);
	pComm->Send(buff);

	sprintf(buff, "\tSelect Locked: 0x%X\r\n", CInputSelect::s_pInputSelect->IsSelectLocked()); 
	pComm->Send(buff);

	m_pathReport.InputPathHack(param, pComm);
	m_todReport.InputTodHack(param, pComm);
	return 0;
}

int CInputDti::ReadStatus()
{
	unsigned int error;
	unsigned char serverStat, serverStatAnd;


	error = CFpga::s_pFpga->getInputCrcError(m_chan);
	if (CFpga::s_pFpga->IsFpgaActive() != 1) {
		// not sure at this point.
		return m_inputStat;
	}
	if (error >= 500) {
		m_serverTrack = 0; // don't anything about server
		m_pathReport.PathInvalid(1);
		m_todReport.TodInvalid(1);
		m_errorSecond++;
		m_errorCnt += error;
		return UpdateInputStatus(INPUT_STAT_LOS);
	}
	
	ParsePathInfo();
	ParseTodMessage();

	CFpga::s_pFpga->GetInputServerStatus(m_chan, &serverStat, &serverStatAnd);
	m_serverTrack = serverStat & CDtiOutput::SERVER_STATUS_MASK;
	m_serverFlag = serverStat;

	if (m_serverTrack == CDtiOutput::SERVER_WARMUP) {
		return UpdateInputStatus(INPUT_STAT_SERVER_WARMUP);
	}

	if (!(serverStatAnd & CDtiOutput::CABLE_ADVANCE_STABLE)) {
		return UpdateInputStatus(INPUT_STAT_CABLE_ADVANCE_INVALID);
	}

	if (m_pathReport.IsPathValid() <= 0) {
		return UpdateInputStatus(INPUT_STAT_INVALID_PATH_TRACEABILITY);
	}

	if (m_todReport.IsTodValid() <= 0) {
		return UpdateInputStatus(INPUT_STAT_INVALID_TOD_MESSAGE);
	}

	if (error >= 200) {
		m_inputDegraded = 1;
	} else {
		m_inputDegraded = 0;
	}

	return UpdateInputStatus(INPUT_STAT_OK);
}

int CInputDti::InputSetup()
{
	int ret = 0;
	int loop;

	loop = CCardLoop::s_pCardLoop->GetCardLoop();
	switch (loop) {
	case CCardLoop::CARD_FREQ_WARMUP:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_WARMUP);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_WARMUP);
		}
		break;
	case CCardLoop::CARD_FREQ_FREERUN:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_FREERUN);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_FREERUN);
		}
		break;
	case CCardLoop::CARD_FREQ_FAST_TRACK:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_FAST_TRACK);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_FAST_TRACK);
		}
		break;
	case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_NORMAL_TRACK);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_NORMAL_TRACK);
		}
		break;
	case CCardLoop::CARD_FREQ_BRIDGE:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_BRIDGE);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_BRIDGE);
		}
		break;
	case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
	case CCardLoop::CARD_FREQ_HOLDOVER:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(CFpga::CLIENT_HOLDOVER);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(CFpga::CLIENT_HOLDOVER);
		}
		break;
	case CCardLoop::CARD_FREQ_FAIL:
	case CCardLoop::CARD_FREQ_OFFLINE:
	default:
		if (m_chan == 1) {
			CFpga::s_pFpga->SetInput11ClientStatus(0);
		} else {
			CFpga::s_pFpga->SetInput12ClientStatus(0);
		}
		break;
	}

	ret |= CInput::InputSetup();

	DoLed();
	return ret;
}

void CInputDti::DoLed()
{
	switch (m_inputQualified) {
	case INPUT_QUALIFY_OK:
		CFpga::s_pFpga->SetInputLed(m_chan, 0, 1);
		break;
	case INPUT_QUALIFY_DISQ:
		if (m_inputStat == INPUT_STAT_LOS) {
			CFpga::s_pFpga->SetInputLed(m_chan, 1, 0);
		} else {
			CFpga::s_pFpga->SetInputLed(m_chan, 1, 1);
		}
		break;
	case INPUT_QUALIFY_IGNORE:
		CFpga::s_pFpga->SetInputLed(m_chan, 0, 0);
		break;
	}
}

unsigned int CInputDti::InputTick()
{
	unsigned int change;


	change = CInput::InputTick();
	if (m_inputStat == INPUT_STAT_DISABLED) {
		m_serverTrack = 0; // don't know anything about server
	}
	if (change) {
		DoLed();
	}

	m_event.ReportServerTrack(m_serverTrack, m_pReportServerLoopStatus);

	// Report performance stable
	if (m_inputStat == INPUT_STAT_OK) {
		m_event.ReportPerformanceStable(m_serverFlag);
	} else {
		m_event.ReportPerformanceStable(CDtiOutput::CLIENT_PERFORMANCE_STABLE);
	}

	if (!m_isCardActive) {
		m_pathReport.CardInactive();
		m_todReport.CardInactive();
		ReportDeviceType(0);
		*m_pReportCableAdvance = 0L;
		m_serverTrack = 0;
		m_serverFlag = CDtiOutput::CLIENT_PERFORMANCE_STABLE | CDtiOutput::CABLE_ADVANCE_STABLE;
		
	} else {
		if (m_inputStat == INPUT_STAT_OK) {
			ReportDeviceType(1);
			*m_pReportCableAdvance = CFpga::s_pFpga->GetInputCableAdvance(m_chan);
		} else {
			ReportDeviceType(0);
			*m_pReportCableAdvance = 0L;
		}
	}
	return change;
}

int CInputDti::ParsePathInfo()
{
	uint32 ipv4 = 0L;
	uint32 ipv6[4];
	uint8  portNumber = 0;
	uint8  version = 0;
	int valid = 1;
	unsigned char *ptr;
	int len = CFpga::MAX_PATH_SIZE;
	uint16 typeLen;
	int unit;

	memset(ipv6, 0, sizeof(ipv6));
	CFpga::s_pFpga->GetInputPath(m_chan, m_path);
	ptr = m_path;
	while ((len > 2) && (valid == 1)) {
		unit = *(ptr+1);
		typeLen = ((uint16)(*ptr) << 8) + unit;
		ptr += 2;
		switch (typeLen) {
		case 0x104: 
			memcpy(&ipv4, ptr, 4);
			break;
		case 0x201:
			portNumber = *ptr;
			break;
		case 0x510:
			memcpy(ipv6, ptr, 16);
			break;
		case 0x701:
			version = *ptr;
			break;
		case 0x901:
			if (*ptr != 0) {
				valid = 0;
			} else {
				valid = 2;
			}
			break;
		default:
			valid = 0;
			break;
		}
		ptr += unit;
		len -= unit + 2;
	}
	if (valid == 2) {
		m_pathReport.ReportRootDtiVersion(version);
		m_pathReport.ReportRootIpv4(ipv4);
		m_pathReport.ReportRootIpv6((char *)ipv6);
		m_pathReport.ReportRootPort(portNumber);
		m_pathReport.PathValid();
		return 1;
	} else {
		m_pathReport.PathInvalid(0);
	}
	return 0;
}

int CInputDti::ParseTodMessage()
{
    
	CFpga::s_pFpga->GetInputTodMessage(m_chan, m_todMessage);


	return m_todReport.ParseTod(m_todMessage);
}

int CInputDti::IsLocked()
{
	int ret = LOCK_LEVEL_LOCK_FLAG;

	if (m_inputStat != INPUT_STAT_OK) return LOCK_LEVEL_NULL;
	if (m_inputQualified != INPUT_QUALIFY_OK) return LOCK_LEVEL_NULL;

	if (!(m_serverFlag & CDtiOutput::CLIENT_PERFORMANCE_STABLE)) {
		ret |= LOCK_LEVEL_PHASE_ERROR;
	}
	if (m_inputDegraded) {
		ret |= LOCK_LEVEL_CRC_NOISY;
	}
	return ret;
}

void CInputDti::ReportDeviceType(int active)
{
	uint8 tmp_exr;
	uint8 device;
	uint8 clock;
	uint8 timing;

	if (active) {
		clock = device = CFpga::s_pFpga->GetInputServerDeviceType(m_chan);
		clock &= 0x7;
		timing = device >> 5;
		if (timing > 2) {
			timing = 16;
		} else if (m_activeInput == m_chan) {
			// CDtiOutput::SetExternalTimeSource(timing);
			//CDtiOutput::SetServerHop(1);
		}
		if (clock > 3) clock = 16;

		if ((m_serverClockType != clock) || (m_changed)) {
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_SERVER_CLOCK_TYPE, m_chan - CHAN_DTI1, 0, clock);
			m_serverClockType = clock;
		}
		if ((m_serverExternalTimingSource != timing) || (m_changed)) {
			SET_MY_ISTATE(COMM_RTE_KEY_INPUT_SERVER_EXTERNAL_TIMING_SOURCE, m_chan - CHAN_DTI1, 0, timing);
			m_serverExternalTimingSource = timing;
		}

	} else {
		clock = 16;
		timing = 16;
	}

	tmp_exr = get_exr();
	set_exr(7);
	//*m_pReportServerClockType = (int32)clock;
	//*m_pReportServerExternalTimingSource = (int32)timing;
	set_exr(tmp_exr);
}
