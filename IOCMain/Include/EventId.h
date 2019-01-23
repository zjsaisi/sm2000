/*                   
 * Filename: EventId.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: EventId.h 1.1 2007/12/06 11:41:29PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             
// The header file list all event id and alarm level

#ifndef _EVENT_ID_11_21
#define _EVENT_ID_11_21

typedef enum {
	COMM_ALARM_LEVEL_NULL,
	COMM_ALARM_LEVEL_JUST_REPORT,
	COMM_ALARM_LEVEL_MINOR,
	COMM_ALARM_LEVEL_MAJOR,
	COMM_ALARM_LEVEL_CRITICAL,
	COMM_ALARM_LEVEL_MAX
} CommAlarmLevel;

#define MESSAGE_TYPE_BROADCAST 0x4000
typedef enum {
	MESSAGE_CARD_FAIL = MESSAGE_TYPE_BROADCAST+1,
	MESSAGE_CARD_OFFLINE,
	MESSAGE_END
} SystemMessageType;

#include "IOCEvtList.h"

#endif // _EVENT_ID_11_21
