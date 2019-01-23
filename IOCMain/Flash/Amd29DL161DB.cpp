// Amd29DL161DB.cpp: implementation of the CAmd29DL161DB class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: Amd29DL161DB.cpp 1.1 2007/12/06 11:41:25PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */             

#include "Amd29DL161DB.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const unsigned long CAmd29DL161DB::m_amd29DL161dbSectorList[(SECT_NUM + 1)*2] = {
		0, 		0x1fff,
	0x2000,		0x3fff,
	0x4000,		0x5fff,
	0x6000,		0x7fff,
	0x8000,		0x9fff,
	0xa000,		0xbfff,
	0xc000,		0xdfff,
	0xe000,		0xffff,
	0x10000,	0x1ffff,
	0x20000,	0x2ffff,
	0x30000,	0x3ffff,
	0x40000,	0x4ffff,
	0x50000,	0x5ffff,
	0x60000,	0x6ffff,
	0x70000,	0x7ffff,
	0x80000,	0x8ffff,
	0x90000,	0x9ffff,
	0xa0000,	0xaffff,
	0xb0000,	0xbffff,
	0xc0000,	0xcffff,
	0xd0000,	0xdffff,
	0xe0000,	0xeffff,
	0xf0000,	0xfffff,
	0x100000,	0x10ffff,
	0x110000,	0x11ffff,
	0x120000,	0x12ffff,
	0x130000,	0x13ffff,
	0x140000,	0x14ffff,
	0x150000,	0x15ffff,
	0x160000,	0x16ffff,
	0x170000,	0x17ffff,
	0x180000,	0x18ffff,
	0x190000,	0x19ffff,
	0x1a0000,	0x1affff,
	0x1b0000,	0x1bffff,
	0x1c0000,	0x1cffff,
	0x1d0000,	0x1dffff,
	0x1e0000,	0x1effff,
	0x1f0000,	0x1fffff,
	0, 			0,
};
CAmd29DL161DB::CAmd29DL161DB(unsigned long start, CCheapTimer *pTimer, CPVOperation *pPV)
: CAmd29B16(DEVICE_ID, m_eraseDelay, m_amd29DL161dbSectorList, start, pTimer, pPV)
{
	m_length = 0x200000; // 2M bytes
	m_sectors = SECT_NUM;
}

CAmd29DL161DB::~CAmd29DL161DB()
{

}
