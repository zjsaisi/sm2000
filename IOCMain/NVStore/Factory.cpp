// Factory.cpp: implementation of the CFactory class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Factory.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Factory.cpp 1.2 2008/05/05 14:23:08PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "Factory.h"
#include "NVRAM.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFactory *CFactory::s_pFactory = NULL;

CFactory::CFactory(CNVRAM *pNVRAM)
{
	s_pFactory = this;
	m_pNVRAM = pNVRAM;
	memset(&m_factory, 0, sizeof(m_factory));
	strcpy(m_factory.title, "Factory Setting");
	strcpy(m_factory.osc_serial, "NULL");
	strcpy(m_factory.hardware_rev, "A");
	strcpy(m_factory.card_serial, "C12345");
	strcpy(m_factory.clei_code, "CLEI012345");
	m_factory.osc_type = OSC_PART_NUMBER_023_63001_02;
	m_factory.clock_type = CLOCK_TYPE_III;
	m_factory.debugOption = 0x80000000;
	m_factory.debugBaudrate = DEFAULT_BAUD0;
	m_factory.userBaudrate = DEFAULT_BAUD1;
	m_factory.debugAutoRate = 1;
	m_factory.userAutoRate = 1;
	m_factory.enableLog = 1;

	m_factory.for_local_hardware_mask = 0;
	m_factory.for_imc_hardware_mask = 0;
	m_factory.for_twin_hardware_mask = 0;

	memset(m_factory.reservedSet, 0xff, RESERVED_SET_SIZE);
}

CFactory::~CFactory()
{

}

// This function should be called new CNVRAM & CNVRAM::init().
void CFactory::AddToNVRAM()
{
	m_pNVRAM->SetupBlock(CNVRAM::BLOCK_ID_FACTORY_SETTING + FACTORY_SETTING_VERSION,
		sizeof(FactorySetting), CNVRAM::MAGIC_FACTORY_SETTING);
}

// This should be called after CNVRAM::init()
// Read from NVRAM if available
void CFactory::InitFactory()
{
	int ret = m_pNVRAM->RtrvBlock(CNVRAM::BLOCK_ID_FACTORY_SETTING + FACTORY_SETTING_VERSION,
		sizeof(FactorySetting), (uint8 *)(&m_factory));
	if (ret != 1) {
		// Invalid from NVRAM, write it back
		SaveChange();
	}
}

void CFactory::SaveChange()
{
	m_pNVRAM->StoreBlock(CNVRAM::BLOCK_ID_FACTORY_SETTING + FACTORY_SETTING_VERSION,
		sizeof(FactorySetting), (uint8 *)(&m_factory));
}

char const * CFactory::GetCardSerial()
{
	return(m_factory.card_serial);
}

char const * CFactory::GetOscSerial()
{
	return(m_factory.osc_serial);
}

char const *CFactory::GetHardwareRev()
{
	return(m_factory.hardware_rev);
}

void CFactory::SetHardwareRev(const char *ptr)
{
	if (strcmp(m_factory.hardware_rev, ptr) == 0) return;
	strncpy(m_factory.hardware_rev, ptr, MAX_SERIAL_LENGTH);
	m_factory.hardware_rev[MAX_SERIAL_LENGTH] = 0;
	SaveChange();
}

void CFactory::SetOscSerial(const char *ptr)
{
	if (strcmp(m_factory.osc_serial, ptr) == 0) return;
	strncpy(m_factory.osc_serial, ptr, MAX_SERIAL_LENGTH);
	m_factory.osc_serial[MAX_SERIAL_LENGTH] = 0;
	SaveChange();
}

void CFactory::SetCardSerial(const char *ptr)
{
	if (strcmp(m_factory.card_serial, ptr) == 0) return;
	strncpy(m_factory.card_serial, ptr, MAX_SERIAL_LENGTH);
	m_factory.card_serial[MAX_SERIAL_LENGTH] = 0;
	SaveChange();
}

void CFactory::SetCleiCode(const char *ptr)
{
	if (strcmp(m_factory.clei_code, ptr) == 0) return;
	strncpy(m_factory.clei_code, ptr, MAX_SERIAL_LENGTH);
	m_factory.clei_code[MAX_SERIAL_LENGTH] = 0;
	SaveChange();
}

void CFactory::SetDebugOption(uint32 debugOption)
{
	if (m_factory.debugOption == debugOption) return;
	m_factory.debugOption = debugOption;
	SaveChange();
}

char const * CFactory::GetCleiCode()
{
	return(m_factory.clei_code);
}


uint32 CFactory::GetDebugOption()
{
	return(m_factory.debugOption);
}

void CFactory::GetBaudrate(int32 *pDebugRate, int32 *pUserRate, int16 *pDebugAuto, int16 *pUserAuto)
{
	*pDebugRate = m_factory.debugBaudrate;
	
	switch (*pDebugRate) {
	case 9600: case 19200: case 38400: case 57600:
		break;
	default:
		*pDebugRate = DEFAULT_BAUD0;
		break;
	}

	*pUserRate = m_factory.userBaudrate;
	switch (*pUserRate) {
	case 9600: case 19200: case 38400: case 57600:
		break;
	default:
		*pUserRate = DEFAULT_BAUD1;
		break;
	}

	*pDebugAuto = m_factory.debugAutoRate & 1;
	*pUserAuto = m_factory.userAutoRate & 1;
}

int16 CFactory::GetEnableLog()
{
	return (m_factory.enableLog ? (1) : (0));
}

uint32 CFactory::GetOscType(int8 *pIsRub /*= NULL*/)
{
	if (pIsRub) {
		*pIsRub = 0;
	}
	switch (m_factory.osc_type) {
	case OSC_PART_NUMBER_023_63001_02:
		break;
	case OSC_PART_MTI260:
		break;
#if ENABLE_RUBIDIUM
	case OSC_PART_RUBIDIUM_X72:
		if (pIsRub) {
			*pIsRub = 1;
		}
		break;
#endif
	default:
		m_factory.osc_type = OSC_PART_NUMBER_023_63001_02;
		break;
	}
	return m_factory.osc_type;
}

void CFactory::SetOscQualityLevel(uint32 level)
{
	if (level == m_factory.osc_quality_level) return;
	m_factory.osc_quality_level = level;
	SaveChange();
}

uint32 CFactory::GetOscQualityLevel(void)
{
	uint32 level;
	level = m_factory.osc_quality_level;
	if ((level >=4) && (level <= 7)) {
		return level;
	}
	return 6;
}

uint32 CFactory::GetClockType()
{
	switch (m_factory.clock_type) {
	case CLOCK_TYPE_I:
	case CLOCK_TYPE_II:
	case CLOCK_TYPE_III:
//	case CLOCK_TYPE_STRATUM_II:
	case CLOCK_TYPE_STRATUM_3E:
	case CLOCK_TYPE_MINIMUM:
		break;
	default:
		m_factory.clock_type = CLOCK_TYPE_III;
		break;
	}
	return m_factory.clock_type;
}

void CFactory::SetOscType(uint32 type)
{
	if (m_factory.osc_type == type) return;
	m_factory.osc_type = type;
	SaveChange();
}

void CFactory::SetClockType(uint32 type)
{
	if (m_factory.clock_type == type) return;
	m_factory.clock_type = type;
	SaveChange();
}

void CFactory::SetBaudrate(int32 debugRate, int32 userRate, int16 debugAuto, int16 userAuto)
{
	int changed = 0;
	if (m_factory.debugBaudrate != debugRate) {
		switch (debugRate) {
		case 9600: case 19200: case 38400: case 57600:
			changed++;
			m_factory.debugBaudrate = debugRate;
			break;
		}
	}
	if (m_factory.userBaudrate != userRate) {
		switch (userRate) {
		case 9600: case 19200: case 38400: case 57600:
			changed++;
			m_factory.userBaudrate = userRate;
		break;
		}
	}
	if ((m_factory.debugAutoRate != debugAuto) && (debugAuto >= 0)) {
		changed++;
		m_factory.debugAutoRate = debugAuto;
	}
	if ((m_factory.userAutoRate != userAuto) && (userAuto >= 0)) {
		changed++;
		m_factory.userAutoRate = userAuto;
	}
	if (changed) {
		SaveChange();
	}
}

void CFactory::SetEnableLog(int16 bLog)
{
	if (m_factory.enableLog == bLog) return;
	m_factory.enableLog = bLog;
	SaveChange();
}

char const * CFactory::GetECI()
{
	return (m_factory.eci_code);
}

void CFactory::SetECI(const char *ptr)
{
	if (strcmp(m_factory.eci_code, ptr) == 0) return;
	strncpy(m_factory.eci_code, ptr, MAX_SERIAL_LENGTH);
	m_factory.eci_code[MAX_SERIAL_LENGTH] = 0;
	SaveChange();

}

int32 CFactory::GetUserBridge()
{
	return (m_factory.userBridge);
}

void CFactory::SetUserBridge(int32 timer)
{
	if (timer == m_factory.userBridge) return; // no change
	m_factory.userBridge = timer;
	SaveChange();
}

uint32 CFactory::GetForLocalMask()
{
	return (m_factory.for_local_hardware_mask);
}

void CFactory::SetForLocalMask(uint32 mask)
{
	if (mask == m_factory.for_local_hardware_mask) return; // no change
	m_factory.for_local_hardware_mask = mask;
	SaveChange();
}

uint32 CFactory::GetForImcMask()
{
	return (m_factory.for_imc_hardware_mask);
}

void CFactory::SetForImcMask(uint32 mask)
{
	if (mask == m_factory.for_imc_hardware_mask) return;
	m_factory.for_imc_hardware_mask = mask;
	SaveChange();
}

uint32 CFactory::GetForTwinMask()
{
	return (m_factory.for_twin_hardware_mask);
}

void CFactory::SetForTwinMask(uint32 mask)
{
	if (mask == m_factory.for_twin_hardware_mask) return;
	m_factory.for_twin_hardware_mask = mask;
	SaveChange();
}

char const * CFactory::GetPartNumber()
{      
	return "090-93121-01";
	
	// For OSC 023-63001-02. Part number is 090-58021-01  
#if 0
	uint32 osc_type;
	int8 jio_switch;
	osc_type = GetOscType();
	jio_switch = GetJIOSwitch();

	switch (osc_type) {
	case OSC_PART_NUMBER_023_63001_02:
#if FORCE_JIO_ON
		return "090-58021-01";
#else
		if (jio_switch) {
			return "090-58021-02";
		} else {
			return "090-58021-01";
		}
#endif
		break;
	case OSC_PART_RUBIDIUM_X72:
#if FORCE_JIO_ON
		return "090-58022-01";
#else
		if (jio_switch) {
			return "090-58022-02";
		} else {
			return "090-58022-01";
		}
#endif
		break;
	case OSC_PART_MTI260: // No part number for MTI260. Test only at this point.
		return "090-58021-99";
	// Put some other OSC type here.		
	}

	return "090-58021-??";
#endif
}
