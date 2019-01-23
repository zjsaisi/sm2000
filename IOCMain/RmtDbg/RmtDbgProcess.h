// RmtDbgProcess.h: interface for the CRmtDbgProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMTDBGPROCESS_H__9FB27371_EAA0_4DFD_A8DE_417F3CCDEE65__INCLUDED_)
#define AFX_RMTDBGPROCESS_H__9FB27371_EAA0_4DFD_A8DE_417F3CCDEE65__INCLUDED_

#include "CodeOpt.h"
#include "BaseProcess.h"

class CRmtComm;
class CDebugDL;
class CLoader;
class CFlash;
class CPTPLink;

class CRmtDbgProcess : public CBaseProcess  
{
public:
	virtual int Notify(uint32 from, uint32 what);
	CRmtDbgProcess(CFlash *pFlash, CLoader *pLoader, CRmtComm *pRmtComm, const char *name, uint8 priority, uint16 stack_size, FUNC_PTR entry = Entry);
	virtual ~CRmtDbgProcess();
	static CRmtDbgProcess *s_pRmtDbgProcess;
private:
	void entry(void);
	static void Entry(void);
	CRmtComm *m_pRmtComm;
	CDebugDL *m_pDebug;
	CLoader *m_pLoader;
	CFlash *m_pFlash;
	CPTPLink *m_pLink;
protected:
};

#endif // !defined(AFX_RMTDBGPROCESS_H__9FB27371_EAA0_4DFD_A8DE_417F3CCDEE65__INCLUDED_)
