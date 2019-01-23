// UserProcess.h: interface for the CUserProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_)
#define AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_


#include "BaseProcess.h"

class Ccomm;
class CFlash;
class CDebugDL;
class CLoader;

class CUserProcess : public CBaseProcess  
{
public:
	CUserProcess(Ccomm *pUserComm, Ccomm *pDebugComm, CLoader *pLoader, CFlash *pFlash,const char *name, uint8 priority, 
		uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CUserProcess();
	int IsRunning(void) { return m_bRunning; };
private:
	void entry(void);
private:
	static void Entry(void);
	Ccomm *m_pUserComm;
	Ccomm *m_pDebugComm;
	CLoader *m_pLoader;
	CFlash *m_pFlash;
	CDebugDL *m_pDebug;
	int m_bRunning;
};

#endif // !defined(AFX_USERPROCESS_H__638BB63B_9E42_456A_8391_F315BD97EB8B__INCLUDED_)
