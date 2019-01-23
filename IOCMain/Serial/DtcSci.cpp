// DtcSci.cpp: implementation of the CDtcSci class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DtcSci.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DtcSci.cpp 1.1 2007/12/06 11:41:37PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "string.h"
#include "DtcSci.h"
#include "iodefine.h"
#include "comm.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDtcSci::CDtcSci(unsigned int vector)
{
	uint16 *pVector;
	pVector = (uint16 *)(0x400 + (vector << 1));
	m_pReg = (SDTCReg *)(0xff0000 + *pVector);

	uint8 *pSrc;
	switch (vector) {
	case 81: // RXI0
		pSrc = (uint8 *)(&(SCI0.RDR));
		break;
	case 85: // RXI1
		pSrc = (uint8 *)(&(SCI1.RDR));
		break;
	case 121: // RXI3 
		pSrc = (uint8 *)(&(SCI3.RDR));
		break;
	case 125: // RXI4   
		pSrc = (uint8 *)(&(SCI4.RDR));
		break;
	default:
		pSrc = NULL;
		break;
	}
	m_pReg->SRC._SAR = (unsigned long)pSrc;
	m_pReg->DEST._DAR = (unsigned long)m_repeatBuffer;
	
	// Setup MRA
	// Fixed source
	m_pReg->SRC.MRA.BIT.SM = 0;
	// Incrementing dest
	m_pReg->SRC.MRA.BIT.DM = 2; 
	// Repeat mode
	m_pReg->SRC.MRA.BIT.MD = 1;
	// Dest repeat
	m_pReg->SRC.MRA.BIT.DTS = 0;
	// Size = 1 byte
	m_pReg->SRC.MRA.BIT.Sz = 0;

	// Setup MRB
	// No chain
	m_pReg->DEST.MRB.BIT.CHNE = 0;
	// Disable CPU interrupt after transfer
	m_pReg->DEST.MRB.BIT.DISEL = 0; // 0;

	// Setup repeat size according to BUFF_SIZE == 256
	m_pReg->CRA.BYTE.CRAH = BUFF_SIZE; // 256 -> 0
	m_pReg->CRA.BYTE.CRAL = BUFF_SIZE; // 256 -> 0

	// Make CRB 0. Not required
	m_pReg->CRB.WORD = 0;

	// test
	memset(m_repeatBuffer, 0, sizeof(m_repeatBuffer));
	strcpy((char *)m_repeatBuffer, "Test DtcSci");

	// Enable DTC
	switch (vector) {
	case 81: // RXI0
		DTC.EE.BIT.B3 = 1;
		break;
	case 85: // RXI1
		DTC.EE.BIT.B1 = 1;
		break;
	case 121: // RXI3
		DTC.EI.BIT.B7 = 1;
		break;
	case 125: // RXI4
		DTC.EI.BIT.B5 = 1;
		break;
	}

	m_rxHead = 0;
}

CDtcSci::~CDtcSci()
{

}

// 1: Get one char to pCh
// 0: No more
int CDtcSci::GetDtcCh(char *pCh)
{
	int rx_tail;
	rx_tail = (int)((0xffffff & m_pReg->DEST._DAR) - (uint32)m_repeatBuffer);
	if (m_rxHead == rx_tail) return 0; // nothing show up.
	if ((rx_tail < 0) || (rx_tail >= BUFF_SIZE)) return 0; // Safe guard

	*pCh = (char)(m_repeatBuffer[m_rxHead++]);
	m_rxHead %= BUFF_SIZE;
	return 1;
}

// 1: For valid DTC vector
// 0: ROM don't support
int CDtcSci::CheckVector(unsigned int vector)
{
	uint16 *p16;

	p16 = (uint16 *)(0x400 + (vector << 1));

	if (*p16 & 3) return 0;
	if ((*p16 < 0xEBC0) || (*p16 > 0xEFBF)) return 0;
	return 1;
}

void CDtcSci::DtcSciHack(Ccomm *pComm)
{
	char buff[100];
	int rx_tail;

	rx_tail = (int)((0xffffff & m_pReg->DEST._DAR) - (uint32)m_repeatBuffer);
	sprintf(buff, "RxHead=%d RxTail=%d\r\n"
		, m_rxHead, rx_tail);
	pComm->Send(buff);
}
