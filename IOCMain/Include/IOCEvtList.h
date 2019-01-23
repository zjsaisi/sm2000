/*                   
 * Filename: IOCEvtList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: IOCEvtList.h 1.3 2008/01/30 09:16:13PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */             
// This file is shared between RTE/PPC for event ids of RTE
#include "CodeOpt.h"

#if (!defined(BASIC_EVENT_ONLY)) || (BASIC_EVENT_ONLY == 0)
#include "iocEvt.h"
#undef	_IOCEVTLIST_H_08_16_2005
#define _IOCEVTLIST_H_08_16_2005
#endif

#ifndef _IOCEVTLIST_H_08_16_2005
#define _IOCEVTLIST_H_08_16_2005

// This is a list of minimum event

typedef enum {
	COMM_RTE_EVENT_NONE = 0, // Invalid Event id

	// Name: RTERST
	// AID1, AID2: N/A
	// RTE reboot in a controlled way. e.g. Triggerred by boot command.
	// Transient event
	COMM_RTE_EVENT_RTE_RESET,

	// Name: PPCNCOM
	// AID1, AID2: N/A
	// Communication failed with PPC.
	// This event should for RTE only because it cannot talk with PPC.
	COMM_RTE_EVENT_PPC_COM_ERROR,

	// Name: RTENCOM
	// AID1, AID2: N/A
	// Communication failed with twin RTE
	COMM_RTE_EVENT_RTE_COM_ERROR,

	// Name: FIRMCOPY
	// AID1, AID2: N/A
	// Event is on when RTE is sending out RTE code to twin.
	// off when it's done.
	COMM_RTE_EVENT_FIRMWARE_COPY,

	// Name: COPYFAIL
	// AID1, AID2: N/A
	// Transient event to indicate copying firmware between RTE failed.
	COMM_RTE_EVENT_FIRMWARE_COPY_FAIL,

	// Add new one here
	// Also be sure to change Istate version, LOG, ALM istate
	// Add default log @ vt_eventLogDefault
	// Add default alarm level @ vt_eventAlarmLevelDefault
	// Add default SA/NSA @ vt_eventServiceAffectDefault
	// Because alarm level for events are configrable and part of Istate 
	// If it's an alarm, make sure it's squelched when card is Out-Of-Service.
	COMM_RTE_EVENT_MAX,

	COMM_RTE_EVENT_MAKE_IT_16BIT = 1000
} CommRTEEventEnum;

#endif // _IOCEVTLIST_H_08_16_2005
