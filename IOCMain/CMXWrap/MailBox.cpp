// MailBox.cpp: implementation of the CMailBox class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MailBox.cpp 1.1 2007/12/06 11:41:10PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#include "MailBox.h"
#include "Resource.h"
#include "cxfuncs.h"

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
