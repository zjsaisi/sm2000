// CRC32.cpp: implementation of the CCRC32 class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: CRC32.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: CRC32.cpp 1.1 2007/12/06 11:41:40PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#include "CRC32.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned long const CCRC32::QUOTIENT = 0x04c11db7;

CCRC32::CCRC32()
{
	Reset();
#if QUICK_CRC32_TABLE
	if (!m_bInited) {
		InitTable();
		m_bInited = 1;
	}
#endif
}

CCRC32::~CCRC32()
{

}

void CCRC32::UpdateCRC32(unsigned char data)
{
#if QUICK_CRC32_TABLE
	m_crc32 = (m_crc32 << 8) ^ m_crctab[(unsigned char)(m_crc32 >> 24) ^ data];
#else
	for (int i = 0; i < 8; i++) {
		if ((((unsigned char )(m_crc32 >> 24)) ^ data ) & 0x80) {
			m_crc32 = (m_crc32 << 1) ^ QUOTIENT;
		} else {
			m_crc32 <<= 1;
		}
		data <<= 1;
	}
#endif
}

void CCRC32::Reset()
{
	m_crc32 = 0xffffffff;
}

unsigned long CCRC32::GetCRC32()
{
	return m_crc32;
}

#if QUICK_CRC32_TABLE
unsigned long CCRC32::m_crctab[256];
unsigned char CCRC32::m_bInited = 0;

void CCRC32::InitTable()
{
	unsigned int i, j;
	unsigned long crc;
	for (i = 0; i < 256; i++) {
		crc = ((unsigned long)i) << 24;
		for (j = 0; j < 8; j++) {
			if (crc & 0x80000000) {
				crc = (crc << 1) ^ QUOTIENT;
			} else {
				crc <<= 1;
			}
		}
		m_crctab[i] = crc;
	}
}
#endif
