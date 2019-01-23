// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: Queue.cpp 1.2 2009/05/05 15:32:42PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */

//#include "zmdefine.h"  
#include "DataType.h"
#include "Queue.h"
#ifndef CXFUNCS_H_05_05_2009
#define CXFUNCS_H_05_05_2009
#include "cxfuncs.h"
#endif
#include "cxconfig.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

uint8 CQueue::s_queue_id_dispatch = 0;

CQueue::CQueue(uint8 size,int number)
{
	m_bValid = 0;
	if (s_queue_id_dispatch >= C_MAX_QUEUES) {
		// No more queue allowed.
		return;
	}
	m_queueId = s_queue_id_dispatch;
	m_queueBuffer = new uint8 [size * number];
	uint8 ret = K_Que_Create(number, size, m_queueBuffer, m_queueId);
	if (ret != K_OK) {
		delete []m_queueBuffer;
		return;
	}
	s_queue_id_dispatch++;
	m_bValid = 1;
}

CQueue::~CQueue()
{

}

// 1 for success
// 2 for full
// -1 for failure
int CQueue::Append(const uint8 *pRec)
{
	if (!m_bValid) return -1;
	uint8 ret = K_Que_Add_Bottom(m_queueId, (uint8 *)pRec);
	switch (ret) {
	case K_ERROR:
		return -1;
	case K_OK: 
		return 1;
	case K_QUE_FULL:
		return 2;
	}
}

// 1 for success
// 2 empty
// -1 for failure
int CQueue::GetTop(uint8 *pRec)
{
	if (!m_bValid) return -1;
	uint8 ret = K_Que_Get_Top(m_queueId, pRec);
	switch (ret) {
	case K_OK:
		return 1;
	case K_QUE_EMPTY:
		return 2;
	case K_ERROR:
		return -1;
	}
}
