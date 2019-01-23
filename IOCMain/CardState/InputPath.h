// InputPath.h: interface for the CInputPath class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputPath.h 1.2 2008/03/11 10:11:28PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             

#if !defined(AFX_INPUTPATH_H__0B1C1A4D_D08E_4608_A4C9_DC7BCC01784A__INCLUDED_)
#define AFX_INPUTPATH_H__0B1C1A4D_D08E_4608_A4C9_DC7BCC01784A__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CInputPath  
{
public:
	void InputPathHack(unsigned long *param, Ccomm *pComm);
	void CardInactive(void);
	static void SetActiveInput(int chan);
	void PathValid(void);
	int IsPathValid(void);
	void PathInvalid(int isLos);
	void ReportRootPort(int port);
	void ReportRootDtiVersion(int version);
	void ReportRootIpv6(char *ptr);
	void ReportRootIpv4(uint32 ip);
	CInputPath(int chan);
	virtual ~CInputPath();
	static uint32 const m_invalidIpv6[4];
private:
	void ReassignIstate(void);
	void PostPath(void);
	void ResetPathInfo(void);
	int m_chan;
	uint32 m_errorCnt;
	uint32 m_totalValidCnt;
	int m_index;
	int m_change;
	uint32 m_ipv4;
	uint32 m_ipv6[4];
	int m_version;
	int m_portNumber;
	int m_validCnt;
	int m_isValid;
	static int m_activeInputPath;
};

#endif // !defined(AFX_INPUTPATH_H__0B1C1A4D_D08E_4608_A4C9_DC7BCC01784A__INCLUDED_)
