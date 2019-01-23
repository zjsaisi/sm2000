// Queue.cpp: implementation of the CQueue class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: Queue.cpp 1.1 2007/12/06 11:41:11PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

//#include "zmdefine.h"  
#include "DataType.h"
#include "Queue.h"
#include "cxfuncs.h"
#include "cxconfig.h"
#include "comm.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

uint8 CQueue::s_queue_id_dispatch = 0;

CQueue::CQueue(uint8 size,int number)
{
	m_cnt = 0;
	m_bValid = 0;
	if (s_queue_id_dispatch >= C_MAX_QUEUES) {
		// No more queue allowed.
		return;
	}
	m_queueId = s_queue_id_dispatch;
	m_queueBuffer = (uint8 *)(new uint16 [(size * number + 1)>>1]);
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
// 2 for full. Last one retrieved
// -1 for failure
int CQueue::Append(const uint8 *pRec)
{
	if (!m_bValid) 
		return -1;

	uint8 ret = K_Que_Add_Bottom(m_queueId, (uint8 *)pRec);
	switch (ret) {
	case K_ERROR:
		return -1;
	case K_OK:
		m_cnt++;
		return 1;
	case K_QUE_FULL:
		m_cnt++;
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
		m_cnt--;
		return 1;
	case K_QUE_EMPTY:
		m_cnt = 0;
		return 2;
	case K_ERROR:
		return -1;
	}
}

// Return the number of items in queue
int CQueue::IsFilled()
{
	return m_cnt;
}

void CQueue::PrintStatus(Ccomm *pComm)
{
	char buff[100];

	sprintf(buff, "Queue used %d, total %d %s\r\n",
		s_queue_id_dispatch, C_MAX_QUEUES, (C_MAX_QUEUES >= s_queue_id_dispatch) ? ("OK") : ("FAILED")
		);
	pComm->Send(buff);
		
}
