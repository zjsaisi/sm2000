// PTPFactory.cpp: implementation of the CPTPFactory class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPFactory.cpp 1.1 2007/12/06 11:41:20PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#include "string.h"
#include "PTPFactory.h"
#include "MemBlock.h"
#include "PTPLink.h"
#include "Factory.h"
#include "errorcode.h"
#include "version.h" 
#include "AllProcess.h"
// #include "RetimerFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPTPFactory::CPTPFactory(CFactory *pFactory, CPTPLink *pLink, uint8 outPort)
{
	m_pFactory = pFactory;
	m_outPort = outPort;
	m_pLink = pLink;
}

CPTPFactory::~CPTPFactory()
{

}

// 1 for success
int CPTPFactory::ReceiveReadPacket(CMemBlock *pBlock)
{
	SCmdReadFactory *pCmd;
	SRespReadFactory resp;
	uint16 size;

	pCmd = (SCmdReadFactory *)(pBlock->GetBuffer());
	memset(&resp, 0, sizeof(resp));
	resp.cmd = pCmd->cmd;
	resp.error_code = 1;
	resp.handle = pCmd->handle;
	size = pBlock->GetCurSize();
	if (size != sizeof(SCmdReadFactory)) {
		resp.error_code = ERROR_FACTORY_UNEXPECTED_FRAME_LENGTH;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		return 1;
	}
	
	if ((pCmd->id & 0xff00) == 0x100) {
#if COMPLETE	
		int slot;
		int item;
		CRetimerFactory *pRetimer;

		slot = (pCmd->id >> 4) & 0xf;
		item = pCmd->id & 0xf;
		if (slot >= 4) {
			resp.error_code = ERROR_FACTORY_UNKNOWN_ID;
		} else {
			pRetimer = CRetimerFactory::s_pRetimerFactory[slot];
			switch (item) {
			case FACTORY_ID_PART_NUMBER: // partnum
				strncpy(resp.value, pRetimer->GetPartNumber(), PTP_STRING_SIZE -1);
				break;
			case FACTORY_ID_CARD_SERIAL_NO: // serial number
				strncpy(resp.value, pRetimer->GetSerialNumber(), PTP_STRING_SIZE -1);
				break;
			case FACTORY_ID_CLEI_CODE: // clei
				strncpy(resp.value, pRetimer->GetCleiCode(), PTP_STRING_SIZE -1);
				break;
			case FACTORY_ID_HARDWARE_REV: // hardware rev
				strncpy(resp.value, pRetimer->GetHardwareRevision(), PTP_STRING_SIZE -1);
				break;
			default:
				resp.error_code = ERROR_FACTORY_UNKNOWN_ID;
				break;
			}
		}
#endif		
	} else {
		switch (pCmd->id) {
		case FACTORY_ID_CARD_SERIAL_NO:
			strncpy(resp.value, m_pFactory->GetCardSerial(), PTP_STRING_SIZE - 1);
			break;
		case FACTORY_ID_HARDWARE_REV:
			strncpy(resp.value, m_pFactory->GetHardwareRev(), PTP_STRING_SIZE - 1);
			break;
		case FACTORY_ID_CLEI_CODE:
			strncpy(resp.value, m_pFactory->GetCleiCode(), PTP_STRING_SIZE - 1);
			break;
		case FACTORY_ID_PART_NUMBER:
			strncpy(resp.value, m_pFactory->GetPartNumber(), PTP_STRING_SIZE - 1);
			break;
		case FACTORY_ID_OSC_SERIAL_NO:
			strncpy(resp.value, m_pFactory->GetOscSerial(), PTP_STRING_SIZE - 1);
			break;
#if 0
		case FACTORY_ID_ECI_CODE:
			strncpy(resp.value, m_pFactory->GetECI(), PTP_STRING_SIZE - 1);
			break;
#endif
		case FACTORY_ID_VERSION_STRING:
			strncpy(resp.value, VERSION_STRING,	PTP_STRING_SIZE - 1);
			break;
		default:
			resp.error_code = ERROR_FACTORY_UNKNOWN_ID;
			break;
		}
	}
	resp.value[PTP_STRING_SIZE - 1] = 0;
	m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
	return 1;
}

int CPTPFactory::ReceiveWritePacket(CMemBlock *pBlock)
{
	SCmdWriteFactory *pCmd;
	SRespWriteFactory resp;
	char buff[PTP_STRING_SIZE + 1];
    uint16 size;
    
	pCmd = (SCmdWriteFactory *)(pBlock->GetBuffer());
	memset(&resp, 0, sizeof(resp));
	resp.cmd = pCmd->cmd;
	resp.error_code = 1;
	resp.handle = pCmd->handle;
	size = pBlock->GetCurSize();
	if (size != sizeof(SCmdWriteFactory)) {
		resp.error_code = ERROR_FACTORY_UNEXPECTED_FRAME_LENGTH;
		m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
		return 1;
	}
	strncpy(buff, pCmd->value, PTP_STRING_SIZE);
	buff[PTP_STRING_SIZE - 1] = 0;
	switch (pCmd->id) {
	case FACTORY_ID_CARD_SERIAL_NO:
		m_pFactory->SetCardSerial(buff);
		break;
	case FACTORY_ID_HARDWARE_REV:
		m_pFactory->SetHardwareRev(buff);
		break;
	case FACTORY_ID_CLEI_CODE:
		m_pFactory->SetCleiCode(buff);
		break;
	case FACTORY_ID_OSC_SERIAL_NO:
		m_pFactory->SetOscSerial(buff);
		break;
#if 0
	case FACTORY_ID_ECI_CODE:
		m_pFactory->SetECI(buff);
		break;
#endif
	default:
		resp.error_code = ERROR_FACTORY_UNKNOWN_ID;
		break;
	}
	m_pLink->PostFrame((char *)&resp, sizeof(resp), m_outPort);
	return 1;
}
