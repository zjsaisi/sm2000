// WatchDog.h: interface for the CWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHDOG_H__E09C124B_BFDD_4514_927B_3AB19F3402E7__INCLUDED_)
#define AFX_WATCHDOG_H__E09C124B_BFDD_4514_927B_3AB19F3402E7__INCLUDED_

class CWatchDog  
{
public:
	static void StopWatchDog(void);
	static void ClearWatchDog(void) { *((volatile uint16 *)0xffffa2) = 0x5a00; };
	static void StartWatchDog(void);
	CWatchDog();
	virtual ~CWatchDog();
};

#endif // !defined(AFX_WATCHDOG_H__E09C124B_BFDD_4514_927B_3AB19F3402E7__INCLUDED_)
