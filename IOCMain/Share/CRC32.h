// CRC32.h: interface for the CCRC32 class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: CRC32.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: CRC32.h 1.1 2007/12/06 11:41:40PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_CRC32_H__5EE78999_D4C2_42BD_A167_8253AF195486__INCLUDED_)
#define AFX_CRC32_H__5EE78999_D4C2_42BD_A167_8253AF195486__INCLUDED_


#ifndef QUICK_CRC32_TABLE
#define QUICK_CRC32_TABLE 1
#endif

class CCRC32  
{
public:
	unsigned long GetCRC32(void);
	void Reset(void);
	void UpdateCRC32(unsigned char data);
	CCRC32();
	virtual ~CCRC32();
private:
	static unsigned long const QUOTIENT;
	unsigned long m_crc32;
#if QUICK_CRC32_TABLE
	static void InitTable(void);
	static unsigned long m_crctab[256];
	static unsigned char m_bInited;
#endif
};

#endif // !defined(AFX_CRC32_H__5EE78999_D4C2_42BD_A167_8253AF195486__INCLUDED_)
