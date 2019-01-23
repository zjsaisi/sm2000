/*                   
 * Filename: resource.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: resource.cpp 1.1 2007/12/06 11:39:05PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */                 

#include "resource.h"
 
static unsigned char s_resource = 0;
unsigned char GetResourceAssignment(void)
{                         
	return(s_resource++);
}

unsigned char GetResourceUsed(void)
{
	return s_resource;
}

static unsigned char s_semaphore = 0;
unsigned char GetSemaAssignment(void)
{
	// max: C_MAX_SEMAPHORES
	return (s_semaphore++);
}                          

unsigned char GetSemaUsed(void)
{
	return s_semaphore;
} 

static unsigned char s_mailbox = 0;
unsigned char GetMailBoxAssignment(void)
{
	// C_MAX_MAILBOXES
	return (s_mailbox++);
}                        

unsigned char GetMailBoxUsed(void)
{
	return (s_mailbox);
}
