// TwinProxy.h: interface for the CTwinProxy class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: TwinProxy.h 1.1 2007/12/06 11:41:34PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_TWINPROXY_H__07EFF0BA_E842_487D_9446_A5AB04C1DA34__INCLUDED_)
#define AFX_TWINPROXY_H__07EFF0BA_E842_487D_9446_A5AB04C1DA34__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CPTPLink;

class CTwinProxy  
{
public:
	CTwinProxy(CPTPLink *pIocLink, int port);
	virtual ~CTwinProxy();
	enum { CMD_START = 0x50,
		CMD_SHELF_LIFE,
		CMD_OUTPUT_CONTROL,
		CMD_CARD_STATE,
		CMD_END
	};
private:
	int ReceiveShelfLife(const char *ptr, int size);
	void CorrectCnt(void);
	typedef struct {
		uint16 cmd;
		int16 this_in;
		int16 twin_in;
		uint32 this_run;
	} SShelfLife;

public:
	int GetFresh(void);
	int PostBuffer(const char *ptr, int size);
	void ForceShelfLife(int life);
	int GetShelfLife(void);
	void TwinOffline(void);
	void TwinProxyHack(unsigned long *param, Ccomm *pComm);
	int ReceiveProxy(const char *ptr, int size);
	int SendShelfLife(void);
	void TickOneSecondTwinProxy(void);
	void Init(void);
	int m_inShelf; // count up to 20000 ==> 10000
	int m_twinInShelf; // count up to 20000 ==> 10000
	int m_twinLost;
	uint32 m_twinRun;
	int m_fresh; 
	int m_port;
	static CTwinProxy *s_pTwinProxy;
	CPTPLink *m_pIocLink;
};

#endif // !defined(AFX_TWINPROXY_H__07EFF0BA_E842_487D_9446_A5AB04C1DA34__INCLUDED_)
