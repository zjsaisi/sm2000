// Factory.h: interface for the CFactory class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Factory.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Factory.h 1.3 2008/05/05 14:23:08PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#if !defined(AFX_FACTORY_H__6C637A5A_896F_4A6F_BCA8_5C5AA846A521__INCLUDED_)
#define AFX_FACTORY_H__6C637A5A_896F_4A6F_BCA8_5C5AA846A521__INCLUDED_

#include "CodeOpt.h"
#include "ConstDef.h"
#include "DataType.h"
#include "string.h"

class CNVRAM;


#define FACTORY_SETTING_VERSION 0x0001

// 4 X 32 bit of firm switch
#define FIRM_SWITCH_NUM 4

#define RESERVED_CLEAR_SIZE (512 - 4)

#define RESERVED_SET_SIZE (512-4)
// 4/17/2006. Moved clock_type
typedef struct {
	char title[16]; // Easy to find out where the data is.
	int32 debugBaudrate;
	int32 userBaudrate;
	int16 debugAutoRate;
	int16 userAutoRate;

	uint32 firmSwitch[FIRM_SWITCH_NUM]; // Init is 0
	char hardware_rev[(MAX_SERIAL_LENGTH + 3) & 0xfffe]; 
	char osc_serial[(MAX_SERIAL_LENGTH + 3) & 0xfffe];
	char card_serial[(MAX_SERIAL_LENGTH + 3) & 0xfffe];
	char clei_code[(MAX_SERIAL_LENGTH + 3) & 0xfffe];
	uint32 osc_type;
	uint32 obsolete001; //uint32 clock_type;
	uint32 debugOption;

	char eci_code[MAX_SERIAL_LENGTH + 2];
	int32 userBridge; // This might be used. It's not released. It was changed to system level.
	uint32 for_local_hardware_mask; // for local. IOCHW_
	uint32 for_imc_hardware_mask;  // For PPC 
	uint32 for_twin_hardware_mask; // For RTE twin
	uint32 osc_quality_level; // osc quality level
	uint8 reservedClear[RESERVED_CLEAR_SIZE]; // reserve 512 bytes. init is cleared
	
	int16 enableLog;
	uint32 clock_type;
	uint8 reservedSet[RESERVED_SET_SIZE]; // reserve 512 bytes. init is set.
} FactorySetting;

class CFactory  
{
public:
	void SetOscQualityLevel(uint32 level);
	uint32 GetOscQualityLevel(void);

public:
	void SetUserBridge(int32 timer);
	int32 GetUserBridge(void);
	void SetECI(const char *ptr);
	char const * GetECI(void);
	int16 GetEnableLog(void);
	void SetEnableLog(int16 bLog);
	void SetBaudrate(int32 debugRate, int32 userRate, int16 debugAuto, int16 userAuto);
	void GetBaudrate(int32 *pDebugRate, int32 *pUserRate, int16 *pDebugAuto, int16 *pUserAuto);
	void SetClockType(uint32 type);
	uint32 GetClockType(void);
	uint32 GetDebugOption(void);
	void SetDebugOption(uint32 debugOption);
	char const * GetCleiCode(void);
	char const * GetHardwareRev(void);
	void SetHardwareRev(char const *ptr);
	void SetCleiCode(char const *ptr);
	void SetCardSerial(char const *ptr);
	void SetOscSerial(char const *ptr);
	char const * GetOscSerial(void);
	char const * GetCardSerial(void);
	uint32 GetOscType(int8 *pIsRub = NULL);
	void SetOscType(uint32 type);
	void InitFactory(void);
	void AddToNVRAM(void);
	CFactory(CNVRAM *pNVRAM);
	virtual ~CFactory();
private:
	void SaveChange(void);
	CNVRAM *m_pNVRAM;
	FactorySetting m_factory;
public:
	char const * GetPartNumber(void);
	void SetForTwinMask(uint32 mask);
	uint32 GetForTwinMask(void);
	void SetForImcMask(uint32 mask);
	uint32 GetForImcMask(void);
	void SetForLocalMask(uint32 mask);
	uint32 GetForLocalMask(void);
	static CFactory *s_pFactory;
};

#endif // !defined(AFX_FACTORY_H__6C637A5A_896F_4A6F_BCA8_5C5AA846A521__INCLUDED_)
