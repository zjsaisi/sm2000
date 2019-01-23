// MailBox.h: interface for the CMailBox class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: MailBox.h 1.1 2007/12/06 11:41:10PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_MAILBOX_H__8D920852_50AF_47FB_8EFC_93AC7F883F47__INCLUDED_)
#define AFX_MAILBOX_H__8D920852_50AF_47FB_8EFC_93AC7F883F47__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"

class CMailBox  
{
public:
	void * ReceiveMail(void);
	int SetMailEvent(uint8 task, uint16 event);
	int SendMail(void * mail);
	CMailBox();
	virtual ~CMailBox();
private:
	unsigned char m_box;
};

#endif // !defined(AFX_MAILBOX_H__8D920852_50AF_47FB_8EFC_93AC7F883F47__INCLUDED_)
