// Sema.h: interface for the CSema class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEMA_H__7A677F25_8874_4A97_9E04_AB6EAAEE1FAE__INCLUDED_)
#define AFX_SEMA_H__7A677F25_8874_4A97_9E04_AB6EAAEE1FAE__INCLUDED_

#include "DataType.h"

class CSema  
{
public:
	void Post(void);
	int Wait(int max_ticks = 0);
	void Reset(void);
	CSema(int init_count);
	virtual ~CSema();
private:
	uint8 m_sema;
public:
	void PostIntr(void);
	int8 m_bValid;
};

#endif // !defined(AFX_SEMA_H__7A677F25_8874_4A97_9E04_AB6EAAEE1FAE__INCLUDED_)
