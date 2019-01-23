// MailBox.cpp: implementation of the CMailBox class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MailBox.cpp 1.2 2009/05/05 15:32:10PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "MailBox.h"
#include "Resource.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "cxfuncs.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailBox::CMailBox()
{
	m_box = GetMailBoxAssignment();
}

CMailBox::~CMailBox()
{

}

// Return 1 for success
int CMailBox::SendMail(void * mail)
{
	uint8 ret;
	ret = K_Mesg_Send(m_box, mail);
	if (ret == K_OK) return 1;
	return -1;
}

// Return 1 for success
int CMailBox::SetMailEvent(uint8 task, uint16 event)
{
	uint8 ret;
	ret = K_Mbox_Event_Set(m_box, task, event);
	if (ret == K_OK) return 1;
	return -1;
}

void * CMailBox::ReceiveMail()
{
	return(K_Mesg_Get(m_box));
}
