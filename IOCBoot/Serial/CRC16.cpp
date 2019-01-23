// CRC16.cpp: implementation of the CCRC16 class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: CRC16.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: CRC16.cpp 1.1 2007/12/06 11:39:15PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "CRC16.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRC16::CCRC16()
{
	m_crc16 = 0;
#if QUICK_CRC16_TABLE
	if (!m_bInited) {
		InitCRC16();
		m_bInited = 1;
	}
#endif
}

CCRC16::~CCRC16()
{

}

#if QUICK_CRC16_TABLE
int8	CCRC16::m_bInited = 0;
unsigned short CCRC16::crcTable[256];

void CCRC16::InitCRC16(void)
{
    unsigned short i, j, crc;

    for(i=0; i <256; i++) {
        crc = (i << 8);
        for(j=0; j < 8; j++)
            crc = (crc << 1) ^ ((crc & 0x8000) ? 0x1021 : 0);
        crcTable[i] = crc & 0xffff;
    }
}
#endif

// generator polynomial is X^16 + X^12 + X^5 + 1
void CCRC16::UpdateCrc(uint8 newData)
{
#if QUICK_CRC16_TABLE
    m_crc16 = crcTable[((m_crc16 >> 8) ^ newData) & 0xFF] ^ (m_crc16 << 8);
#else
	register uint16 crc = m_crc16;
	register uint16 data = ((uint16)newData) << 8;
	int i;

	for (i = 0; i < 8; i++) {		
		if ((crc ^ data) & 0x8000) {
			crc <<= 1;
			data <<= 1;
			crc ^= 0x1021;
		} else {
			crc <<= 1;
			data <<= 1;
		}
	}
	m_crc16 = crc;
#endif
}

