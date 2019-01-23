// Istate.h: interface for the CIstate class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Istate.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Istate.h 1.1 2007/12/06 11:41:30PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_ISTATE_H__D36682E4_C178_4830_970E_8B9A786E5C59__INCLUDED_)
#define AFX_ISTATE_H__D36682E4_C178_4830_970E_8B9A786E5C59__INCLUDED_

#include "PV.h"

class CIstateKb;
class CNVRAM;
class Ccomm;
class CIstateProcess;

class CIstate  
{
public:
	void AddToNvram(void);
	void ReportIdle(void);
	int CheckWritingIstateItem(int16 item, int16 index1, int16 index2, uint32 value, int16 cmd);
	// Called by debug process
	int PrintStatus(Ccomm *pComm);
	// Called by debug process
	int PrintIstateItem(Ccomm *pComm, int16 item, int bPrintHelp = 0);
	int ReadOneIstateItem(int16 item, 
		int16 index1, int16 index2, uint32 *pResult);
	// This function can only be called by Istate process.
	int WriteOneIstateItemByIstateProcess(int16 cmd, int16 item, 
		int16 index1, int16 index2, uint32 value);
	void TickPerSecond(void);
	void SetModified(void);
	void CloseIstate(void);
	void OpenIstate(void);
	void InitIstate(void);
	CIstate(CIstateProcess *pProcess, CIstateKb *pKB, CNVRAM *pNVRAM, char const *pName);
	virtual ~CIstate();
	CIstateKb *GetKB(void) { return m_pKB; };
	char const *GetName(void) { return m_name; };
	enum { STATE_IDLE_ISTATE, STATE_SENDING_ISTATE };
private:
	const char * GetNextHelp(const char *pHelp);
	// To support reporting idle from Block/Item transceiver
	CIstateProcess *m_pProcess;

	char m_name[12];
	CIstateKb *m_pKB;
	CNVRAM *m_pNVRAM;
	const uint8 *m_pNonVolatileIstateLocation;
	uint16 m_nonVolatileIstateSize;
	uint16 m_istateVersion;
	uint32 m_NVRAMBlockId;
	CPVOperation m_pv;
	uint8 m_bModified;
	uint32 m_modificationCnt;
};

#endif // !defined(AFX_ISTATE_H__D36682E4_C178_4830_970E_8B9A786E5C59__INCLUDED_)
