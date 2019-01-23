// IstateCheckSum.cpp: implementation of the CIstateCheckSum class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateCheckSum.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateCheckSum.cpp 1.4 2009/04/30 17:52:46PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.4 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "IstateCheckSum.h"
#include "Istate.h"
#include "IstateKb.h"
#include "PTPLink.h"
#include "crc32.h"
#include "AllProcess.h"
#include "IstateProcess.h"
#include "Warmup.h"
#include "comm.h"
#include "stdio.h"
#include "Factory.h"
#include "CardStatus.h"
          
#define TRACE TRACE_ISTATE
#define SLOT_ID   CAllProcess::GetSlotId()
// Hardware option that might affect Istate setup.
#define HARDWARE_OPTION_ISTATE (0L)

// Hardware feature that might affect compatibility between IOCs
// For first release. 1  and the firmware is compatible with 1 only.
#define HARDWARE_COMPATIBILITY 1


// Hardware capbility. Informational only for now.
// For First release. 1.
#define HARDWARE_CAPBILITY  (1L)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIstateCheckSum::CIstateCheckSum(//CCardState *pCardState, 
	CIstate *pIstate, CPTPLink *pLink, uint8 peer)
{
	m_twinIstateCheckSum = 0L;

	m_linkCnt = 0;
	//m_nSupplementReceived = 0;
	//m_extensionSendTimer = -10;
	//m_pCardState = pCardState;
	m_pIstate = pIstate;
	m_pLink = pLink;
	m_peer = peer;
	m_pKB = pIstate->GetKB();
	m_pIstateLocation = m_pKB->GetIstateLocation(&m_istateSize, &m_istateVersion);
	m_sendTimer = 100;
	// m_thisCardState = 0;
	m_bThisIstateInited = 0;
	m_nCheckSumReceived = 0;
	m_nMismatchCnt = 0L;
	m_thisIstateCheckSum = GetCheckSum();
	m_twinIstateVersion = 0;
	m_bTwinIstateInited = 0; // Assume twin not inited.
	m_twinIsActive = 0;
	m_twinSlotId = 0;
	m_twinHardwareOptionIstate = 0;
	m_twinHardwareCompatibility = 0;
	m_twinHardwareCapbility = 0L;
}

void CIstateCheckSum::Init()
{    
	// m_JIOSwitch = g_pAll->GetJIOSwitch();
}

CIstateCheckSum::~CIstateCheckSum()
{

}

// Called every second if Istate process is idle
void CIstateCheckSum::TickPerSecondIstateCheckSum()
{
	int limit;

	limit = (m_linkCnt++ > 100) ? (10) : (4);
	if (m_sendTimer++ >= limit) {
		m_sendTimer = 0;
		SendCheckSum();
		if (m_linkCnt > 20000) {
			m_linkCnt = 10000;
		}
	} else if (m_bThisIstateInited != 1) {
		if (IS_ACTIVE) {
			SetInited();
		}
	}
}

void CIstateCheckSum::ClearTwinInitFlag()
{
	m_sendTimer = 0;
	SendCheckSum(CMD_FORCE_UNINIT);
}

#if 0
uint32 CIstateCheckSum::GetIstateVersionExtension()
{
	uint32 option;
	option = 0L; // include options here. Better use high 16bit only.
	return option | IOC_ISTATE_VERSION_EXTENSION; // 
}
#endif

#if 0
int CIstateCheckSum::SendSupplement()
{
	SIstateSupplement msg;
	int ret;
	memset(&msg, 0, sizeof(msg));
	msg.cmd = CMD_SUPPLEMENT;
	msg.cmd_version = CMD_SUPPLEMENT_VERSION;
	msg.ioc_istate_version_extension = GetIstateVersionExtension();
	msg.ioc_hardware_ability = GetHardwareMask(); // retrieve it from flash
	ret = m_pLink->PostFrame((char *)&msg, sizeof(msg), m_peer);
	return ret;
}
#endif

// return 1: OK
// return -1: unexpected  
#if 0
int CIstateCheckSum::ReceiveSupplement(const uint8 *ptr, uint16 len)
{
	SIstateSupplement *pMsg;
	
	if (len != sizeof(SIstateSupplement)) return -1;
	pMsg = (SIstateSupplement *)ptr;
	if ((pMsg->cmd != CMD_SUPPLEMENT) || (pMsg->cmd_version != CMD_SUPPLEMENT_VERSION)) {
		return -1;
	}
	m_nSupplementReceived++;
	m_twinIstateExtension = pMsg->ioc_istate_version_extension;
	m_twinHardwareMask = pMsg->ioc_hardware_ability;
}
#endif   

int CIstateCheckSum::SendCheckSum(uint16 cmd /* = CMD_CHECKSUM */ )
{
	SIstateCheckSum msg;
	memset(&msg, 0, sizeof(msg));

	msg.checksum = m_thisIstateCheckSum = GetCheckSum();
	msg.version = m_istateVersion;
	msg.cmd = cmd;
	msg.is_active = IS_ACTIVE; 
	msg.slot_id = SLOT_ID;
	msg.hardware_option_istate = HARDWARE_OPTION_ISTATE; 
	msg.bInited = m_bThisIstateInited;
	msg.hardware_compatibility = HARDWARE_COMPATIBILITY;
	msg.hardware_capbility = HARDWARE_CAPBILITY;

	return m_pLink->PostFrame((char *)&msg, sizeof(msg), m_peer);

}

uint32 CIstateCheckSum::GetCheckSum()
{
	CCRC32 crc32;
	const uint8 *ptr;

	m_pIstate->OpenIstate();
	ptr = m_pIstateLocation;
	for (int i = 0; i < m_istateSize; i++) {
		crc32.UpdateCRC32(*ptr++);
	}
	m_pIstate->CloseIstate();
	return crc32.GetCRC32();
}

// Receive the checksum package from peer
// 1: for OK
// -1: Unexpected size;
int CIstateCheckSum::ReceiveCheckSum(const uint8 *ptr, uint16 len)
{  
	int result = 1;
	int oldTwinInited;
	const SIstateCheckSum *pMsg;
	SIstateCheckSum message;

	pMsg = (SIstateCheckSum *)ptr;

	if (len != sizeof(SIstateCheckSum)) {
		int size;

		size = (len > sizeof(SIstateCheckSum)) ? (sizeof(SIstateCheckSum)) : (len);
		memset(&message, 0, sizeof(message));
		memcpy(&message, ptr, size);
		pMsg = &message;

		m_nMismatchCnt = 0L;
		// m_nCheckSumReceived++; // 5/27/2004: When change format, we are receiving.
		result = -1;
	}

	oldTwinInited = m_bTwinIstateInited;
	m_twinIstateCheckSum = pMsg->checksum;
	m_twinIstateVersion = pMsg->version;
	m_twinIsActive = pMsg->is_active; 
	m_twinSlotId = pMsg->slot_id;
	m_twinHardwareOptionIstate = pMsg->hardware_option_istate;
	m_twinHardwareCompatibility = pMsg->hardware_compatibility;
	m_twinHardwareCapbility = pMsg->hardware_capbility;

	m_bTwinIstateInited = pMsg->bInited;

	if ((oldTwinInited != 1) && (m_bTwinIstateInited == 1)) {
		//CWarmup *pWarmup = g_pAll->GetWarmup();
		// pWarmup->ShortenLeastDelay(); // LEast delay set for Istate transfer. Since it's done, make it short.

		//CCardState::s_pCardState->ConfigReady();
	}

	m_nCheckSumReceived++;
	
	if (IsTwinSameVersion() != 1) {
		m_nMismatchCnt = 0L;
		return result;
	} else {
		// Same version
		if (m_twinIstateCheckSum != m_thisIstateCheckSum) {
			m_nMismatchCnt++;
		} else {
			m_nMismatchCnt = 0L;
		}
	}
	return result;
}

int CIstateCheckSum::DispatchMessage(const uint8 *ptr, uint16 len)
{
	const uint16 *pCmd;
	pCmd = (const uint16 *)ptr;
	switch (*pCmd) {
	case CMD_CHECKSUM:
		return ReceiveCheckSum(ptr, len);
	case CMD_FORCE_UNINIT:
		// Tell me istate is not updated with active RTE.
		ReceiveCheckSum(ptr, len); // Update info
		m_bThisIstateInited = 0;
		m_sendTimer = 100;
		TRACE("Istate init flag cleared\r\n");
		return 1;
//	case CMD_SUPPLEMENT:
//		return ReceiveSupplement(ptr, len);
	default:
		return -1;
	}
}

// Return 1: If Istate in twin inited
// 0: twin is not inited.
// -1: Don't know
int CIstateCheckSum::IsTwinInited()
{
	if (m_nCheckSumReceived <= 0) return -1;
	if (m_bTwinIstateInited) return 1;
	return 0;
}

// Return 1: same version
// 0: Not same version
// -1: Don't know
int CIstateCheckSum::IsTwinSameVersion()
{
	if (m_nCheckSumReceived == 0) return -1;
	if (m_twinIstateVersion != m_istateVersion) return 0;
	if (m_twinHardwareOptionIstate != HARDWARE_OPTION_ISTATE) return 0;
	return 1;
}

// return 1: the istate
// 0: different
// -1: don't know
int CIstateCheckSum::IsMatch(uint32 *pTimes)
{
	int ret;
	
	if (pTimes) {
		*pTimes = m_nMismatchCnt;
	}
	ret = IsTwinSameVersion();
	if (ret != 1) return ret;
	if (m_twinIstateCheckSum != m_thisIstateCheckSum) {
		return 0;
	}
	return 1;

#if 0
	if (pTimes) *pTimes = 0L;
	if (m_nCheckSumReceived == 0) return -1;
	if (m_twinIstateCheckSum != m_thisIstateCheckSum) {
		if (pTimes) *pTimes = m_nMismatchCnt;
		return 0;
	}
	if (m_twinIstateVersion != m_istateVersion) return 0;
	if (m_JIOSwitch != m_twinJIOSwitch) return 0;
	return 1;
#endif
}

void CIstateCheckSum::SetInited()
{
	if (m_bThisIstateInited == 0) {
		// CWarmup *pWarmup;

		m_bThisIstateInited = 1;
		TRACE("Istate inited\r\n");

		CAllProcess::g_pAllProcess->IstateInited();

		//pWarmup = g_pAll->GetWarmup();
		//pWarmup->ShortenLeastDelay(); // LEast delay set for Istate transfer. Since it's done, make it short.
		//CCardState::s_pCardState->ConfigReady();

		// Tell twin That I am done receiving ASAP
		m_sendTimer = 100;
	}
}

void CIstateCheckSum::ResetMismatchCounter()
{
	m_nMismatchCnt = 0L;
}

void CIstateCheckSum::PrintIstateCheckSum(Ccomm *pComm)
{
	char buff[400];
	uint32 self_option;
	self_option = HARDWARE_OPTION_ISTATE; //GetHardwareMask();
	
	sprintf(buff, "CheckSum Packet Count: %ld\r\n", 
		m_nCheckSumReceived);
	pComm->Send(buff);

	sprintf(buff, "Istate Main version: 0x%04X Hardware Option(for twin): 0x%08lX(%ld)\r\n"
		"Active=%d Inited=%d SlotId=%d IsMaster=%d\r\n" 
		"CheckSum = %lx Mismatch Cnt = %ld\r\n"
		"Compatibility=%d Capbility=0x%lX\r\n"
		,m_istateVersion, self_option, self_option
		,IS_ACTIVE, m_bThisIstateInited, SLOT_ID, IsMaster()
		, m_thisIstateCheckSum, m_nMismatchCnt
		, HARDWARE_COMPATIBILITY, HARDWARE_CAPBILITY); 
	pComm->Send(buff);

	sprintf(buff, "\r\nTwin Main version: 0x%04X Hardware Option(for twin): %08lX(%ld)\r\n"
		"TwinActive=%d Inited=%d TwinSlotId=%d\r\n"
		"TwinCheckSum = %lx\r\n"
		"TwinHardwareCompatibility=%d TwinCapbility=0x%lX\r\n"
		, m_twinIstateVersion, m_twinHardwareOptionIstate, m_twinHardwareOptionIstate 
		, m_twinIsActive, m_bTwinIstateInited, m_twinSlotId
		, m_twinIstateCheckSum
		, m_twinHardwareCompatibility, m_twinHardwareCapbility);  
	pComm->Send(buff);
	
}

uint32 CIstateCheckSum::GetHardwareMask()
{
	return CFactory::s_pFactory->GetForTwinMask();
}

// return -1: Don't know
// return 1: Compatible
// return 0: Incompatible 
#if 0
int CIstateCheckSum::IsTwinHardwareCompatible()
{
	uint32 self_mask;
	int link;

	link = m_pLink->GetLinkStatus();
	if (link <= 0) return -1; // not communicating.
	if (m_nSupplementReceived == 0L) {
		if (link < 20) return -1; // Allow 10 second for twin to send info
		// Otherwise, assume twin is 0.
	}
	self_mask = GetHardwareMask();
	if (self_mask != m_twinHardwareMask) return 0;
	return 1;
}
#endif

// 1: Twin is active.
// -1: don't know
// 0: twin inactive
int CIstateCheckSum::IsTwinActive()
{
	if (m_nCheckSumReceived <= 0) return -1;
	if (m_twinIsActive) return 1;
	return 0;
}

int CIstateCheckSum::IsMaster()
{
	if (IS_ACTIVE) return 1;
	if (IsTwinActive() == 1) return 0;
	if (m_nCheckSumReceived <= 0) return -1;
	if ((m_bTwinIstateInited > 0) && (m_bThisIstateInited <= 0)) {
		return 0;
	} else if ((m_bTwinIstateInited <= 0) && (m_bThisIstateInited > 0)) {
		return 1; // I was inited and the other guy is new.
	}
	if (SLOT_ID < m_twinSlotId) return 1;
	return -1;
}

void CIstateCheckSum::LinkDown()
{
	m_nCheckSumReceived = 0;
	m_bTwinIstateInited = 0;
	m_linkCnt = 0;
}

// return -1: Don't know
// 1: Compatible
// 0: Not compatible
int CIstateCheckSum::IsTwinCompatible()
{
	if (m_nCheckSumReceived == 0) return -1;
	if (m_twinIstateVersion != m_istateVersion) return 0;
	if (m_twinHardwareOptionIstate != HARDWARE_OPTION_ISTATE) return 0;
	if (m_twinHardwareCompatibility != HARDWARE_COMPATIBILITY) return 0;
	return 1;

}
