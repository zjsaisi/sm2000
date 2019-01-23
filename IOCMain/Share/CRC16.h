// CRC16.h: interface for the CCRC16 class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: CRC16.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: CRC16.h 1.1 2007/12/06 11:41:40PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_CRC16_H__FC115334_7F34_492A_811A_E5A1739EB637__INCLUDED_)
#define AFX_CRC16_H__FC115334_7F34_492A_811A_E5A1739EB637__INCLUDED_

#include "DataType.h"

// Set 1 to use table ( more space, quick )
// Set 0 to calculate CRC16 at run time
#define QUICK_CRC16_TABLE 1

class CCRC16  
{
public:
	void Reset(void) { m_crc16 = 0; };
	void UpdateCrc(uint8 newData);
	CCRC16();
	virtual ~CCRC16();
	uint16 GetCrc16(void) {return m_crc16; };
private:
	uint16 m_crc16;

#if QUICK_CRC16_TABLE
	static unsigned short crcTable[256];
	static void InitCRC16(void);
	static int8 m_bInited;
#endif

};

#endif // !defined(AFX_CRC16_H__FC115334_7F34_492A_811A_E5A1739EB637__INCLUDED_)
