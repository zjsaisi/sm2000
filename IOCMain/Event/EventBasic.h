// BasicEvent.h: interface for the CEventBasic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICEVENT_H__279A19FC_1028_46C6_9638_90B559593373__INCLUDED_)
#define AFX_BASICEVENT_H__279A19FC_1028_46C6_9638_90B559593373__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "EventKb.h"
#include "EventApp.h"

class CEventBasic : public CEventApp
{
public:
	CEventBasic(CEventKb *pKb);
	virtual ~CEventBasic();
private:
	static const SEventKB m_eventList[];

};

#endif // !defined(AFX_BASICEVENT_H__279A19FC_1028_46C6_9638_90B559593373__INCLUDED_)
