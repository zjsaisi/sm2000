// CardAlign.h: interface for the CCardAlign class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: CardAlign.h 1.5 2009/05/01 09:17:27PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 */

#if !defined(AFX_CARDALIGN_H__460AFA31_37A3_4CBA_83CE_E25AE599EFD1__INCLUDED_)
#define AFX_CARDALIGN_H__460AFA31_37A3_4CBA_83CE_E25AE599EFD1__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Constdef.h"
#include "comm.h"

class CCardAlign  
{
private:
	static int m_todSource;
public:
	static void CfTodSource(int source);
public:
	void CardAlignHack(Ccomm *pComm);
	int32 GetGpsPhase(void) { return m_gpsPhase; };
	int32 GetSignedGpsPhase(void) { return m_signedGpsPhase; };
	static const char * GetName(int state);
	int GetTwinAlign(void) const;
	void SetTwinAlign(int align);
	void CardAlignOffline(void);
	enum { CARD_ALIGN_UNKNOWN = 1,
		CARD_ALIGN_TIGHT = 0x38, CARD_ALIGN_LOOSE};
	void Init(void);
	void CardAlignFail(void);
	int TickCardAlign(void);
	CCardAlign();
	virtual ~CCardAlign();
	int GetCardAlign(void) const { return m_cardAlign; };
	static CCardAlign *s_pCardAlign;
private:
	int TransitAlign(int state);
	int TickTight(void);
	int TickLoose(void);
	void ReadGpsPhase(void);
	int m_cardAlign;
	int m_stop;
	int m_twinAlign;
	int32 *m_pReportGpsPhase;
	int32 m_preReportGpsPhase;
	int32 volatile m_gpsPhase;
	int32 m_signedGpsPhase;
};

#endif // !defined(AFX_CARDALIGN_H__460AFA31_37A3_4CBA_83CE_E25AE599EFD1__INCLUDED_)
