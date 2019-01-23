// YModemRecv.h: interface for the CYModemRecv class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: UModemRecv.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: YModemRecv.h 1.1 2007/12/06 11:41:21PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_)
#define AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_

#include "DataType.h"

#include "crc16.h"

class Ccomm; // Communication port 
class CCodeStorage; // Class to store received data

// Starting of common part on PC & Hitachi firmware
class CYModemRecv  
{
public:
	int Start(Ccomm *pPort, CCodeStorage *pStorage);
	CYModemRecv();
	virtual ~CYModemRecv();
private:
	enum { MODE_CRC16, MODE_CHECKSUM};
	static const uint8 SOH;
	static const uint8 STX;
	static const uint8 EOT;
	static const uint8 ACK;
	static const uint8 NAK;
	static const uint8 CAN;
	static const uint8 C;
	static const uint8 TCPHEADER;
private:
	int Purge(void);
	int RecvBlock(uint8 *blocknum, char *pBlock, int timeout = 5000);
	int RecvBlockZero(char *buff);
	Ccomm *m_pPort;
	CCodeStorage *m_pStorage;
	int m_nTry;
	int8 m_mode;
	// Filelength
	uint32 m_fileLength;

	// Checksum
	CCRC16 m_crcCheck; // Crc16 mode
	uint8 m_checksum; // Checksum mode

};

#endif // !defined(AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_)
