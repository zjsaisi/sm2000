// DtcSci.h: interface for the CDtcSci class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: DtcSci.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: DtcSci.h 1.1 2007/12/06 11:39:16PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_)
#define AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"

class CDtcSci  
{
private:
	typedef struct {

		union {
			unsigned long _SAR; 
			union {
				unsigned char BYTE;
				struct {
					unsigned char SM : 2;
					unsigned char DM : 2;
					unsigned char MD : 2;
					unsigned char DTS: 1;
					unsigned char Sz : 1;
				} BIT;
			} MRA;
		} SRC;

		union {
			unsigned long volatile _DAR;
			union {
				unsigned char BYTE;
				struct {
					unsigned char CHNE : 1;
					unsigned char DISEL : 1;
					unsigned char : 6;
				} BIT;
			} MRB;
		} DEST;

		union {
			unsigned short WORD;
			struct {
				unsigned char CRAH;
				unsigned char CRAL;
			} BYTE;
		} CRA;

		union {
			unsigned short WORD;
			struct {
				unsigned char CRBH;
				unsigned char CRBL;
			} BYTE;
		} CRB;
	} SDTCReg;

public:
	static int CheckVector(unsigned int vector);
	int GetCh(char *pCh);
	CDtcSci(unsigned int vector);
	virtual ~CDtcSci();
private:
	enum { BUFF_SIZE = 256 };
	uint8 m_repeatBuffer[BUFF_SIZE+2];
	SDTCReg *m_pReg;
	int m_rxHead;
};

#endif // !defined(AFX_DTCSCI_H__0A257778_052E_4F87_836E_026288309F09__INCLUDED_)
