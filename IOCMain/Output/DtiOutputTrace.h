// DtiOutputTrace.h: interface for the CDtiOutputTrace class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputTrace.h 1.1 2007/12/06 11:41:36PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 * Handle Output Path Traceability
 */

#if !defined(AFX_DTIOUTPUTTRACE_H__01C8B0F4_26B4_4533_9EA4_666B5D131A00__INCLUDED_)
#define AFX_DTIOUTPUTTRACE_H__01C8B0F4_26B4_4533_9EA4_666B5D131A00__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CDtiOutputTrace  
{
public:
	static void TraceHack(int chan, Ccomm *pComm);
	static void CfRootDtiVersion(int version);
	static void CfRootPortNumber(int port);
	static void CfRootIpSix(char *ptr);
	static void CfRootIpFour(uint32 ip);
	static void CfTodSource(int source);
	static void SendTrace(void);
	static void Create(void);
	static void CfSelfIpSix(char *ptr);
	static void CfSelfIpFour(uint32 ip);
	static void CfSelfPortNumber(int chan, int port);
	CDtiOutputTrace(int chan);
	virtual ~CDtiOutputTrace();
	enum { DTI_VERSION = 0 };
private:
	void DtiOutputTraceHack(Ccomm *pComm);
	static void SendForFun(void);
	static void SendAsSubtend(void);
	static void SendAsRoot(void);
	int m_chan;
	static int m_todSource;
	int m_myPortNumber;
	static uint32 m_myIpFour;
	static uint32 m_myIpSix[4];

	static int m_rootPortNumber;
	static uint32 m_rootIpFour;
	static uint32 m_rootIpSix[4];
	static int m_rootVersion;

	static CDtiOutputTrace * s_pDtiOutputTrace[NUM_OUTPUT_CHAN];
};

#endif // !defined(AFX_DTIOUTPUTTRACE_H__01C8B0F4_26B4_4533_9EA4_666B5D131A00__INCLUDED_)
