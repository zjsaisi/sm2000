// AllProcess.h: interface for the CAllProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_)
#define AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_

class CDebugProcess;
// class CAmd29lv800bb;
class CAmd29DL161DB;
class CUserProcess;
class CBaseProcess;
class CLoader;
class Ccomm;
class CPTPProcess;
class CPTPLink;
class CPTPDnldProcess;

class CAllProcess  
{
public:
	static int CheckDldHeader(const char *pHead);
	static uint8 GetHardwareRevision(void);
	static int8 GetFpgaDone();
	CLoader *GetLoader(void) { return m_pLoader; };
	void Reboot(void);
	void WaitForChoice(void);
	void MakeBootChoice(Ccomm *pComm, int bBlinking = 0);
	CBaseProcess * GetUserProcess(void);
	CBaseProcess * GetDebugProcess(void);
	void StartAll(unsigned long nextBlock);
	CAllProcess();
	virtual ~CAllProcess();
	void ReportVersion(Ccomm *pSci);
	void SecondTick(void);	
	void DebugOutput(const char *str, int len=-1, uint32 option = 0x10000);
	CPTPDnldProcess *GetPTPDnldProcess(void) { return m_pPTPDnldProcess; };
	enum { 
		FIRST_PRIORITY = 1, FIRST_PROC_PRIORITY = 1,
		IOC_IOC_PRIORITY,
		IMC_IOC_PRIORITY,
		DEBUG_PRIORITY,
		USER_PRIORITY,
		PTP_DNLD_PRIORITY,
		LAST_PRIORITY
	};
private:
	void BlinkChoice(void);
	void ForceDebugOutput(const char *str);
	int CheckBoot(void);
	void SendStr(const char *str);
	
	//CAmd29lv800bb *m_pFlash;  
	CAmd29DL161DB *m_pFlash;
	
	CDebugProcess *m_pDebug;
	CUserProcess *m_pUser;
	CLoader *m_pLoader;
	int8 m_mainAvailable;
	CPTPProcess *m_pIMCIOC;
	CPTPProcess *m_pIOCIOC;
	CPTPLink *m_pLinkIMCIOC;
	CPTPLink *m_pLinkIOCIOC;
	CPTPDnldProcess *m_pPTPDnldProcess;

	// For Blink when waiting for choice
	enum { BLINK_WAIT_NUM = 3 };
	int m_blinkIndex;
	static uint16 m_pattern[BLINK_WAIT_NUM];
public:
	static int m_skipHeader;
};
     
extern CAllProcess *g_pAll;

#endif // !defined(AFX_ALLPROCESS_H__A8506052_E138_483B_9190_16B5E16A6A2C__INCLUDED_)
