// EventStore.h: interface for the CEventStore class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: EventStore.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: EventStore.h 1.2 2009/05/05 10:14:01PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_EVENTSTORE_H__F9EF3201_C603_44D8_BB2A_EAFDCCAC9726__INCLUDED_)
#define AFX_EVENTSTORE_H__F9EF3201_C603_44D8_BB2A_EAFDCCAC9726__INCLUDED_

#include "pv.h"
#include <string.h>

class CFlashList;
class CFlash;

// The size has to be consistent with EVENT_ITEM_SIZE
typedef struct {
	int16 event_id;
	uint8 active;
	uint8 alarm_level;
	uint8 aid1;
	uint8 aid2;
	uint32 time_stamp;
} SEventItem;

class CEventStore  
{
public:
	int AppendItem(const unsigned char *pItem);
	int RtrvItems(unsigned long index, unsigned long *pNextIndex, 
			unsigned char *pRec, int max);
	CEventStore(CFlash *pDevice);
	virtual ~CEventStore();
	// Size of each event. Payload size
	enum {EVENT_ITEM_SIZE = 10 }; // Size of SEventItem

private:
	void ResetEventStore(void);
	CFlashList * FindBlock(unsigned long index);
	CFlashList * FindEldestBlock(int *pIndex = NULL);
	CFlashList * FindLatestBlock(int *pIndex = NULL);
	// In order to use this in #if, looks I have to "define".
#define MAX_EVENT_BLOCK_NUM 3
	CFlashList *m_blockList[MAX_EVENT_BLOCK_NUM];
	CFlashList *m_pLatest; // The latest block. To speed up writing part.
	CPVOperation m_pv;
};

#endif // !defined(AFX_EVENTSTORE_H__F9EF3201_C603_44D8_BB2A_EAFDCCAC9726__INCLUDED_)
