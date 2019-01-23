// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////
/*
 * $Header: Queue.h 1.1 2007/12/06 11:41:11PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_QUEUE_H__E68AD7F3_713C_4686_91EF_D682A722B03A__INCLUDED_)
#define AFX_QUEUE_H__E68AD7F3_713C_4686_91EF_D682A722B03A__INCLUDED_

class Ccomm;

class CQueue  
{
public:
	static void PrintStatus(Ccomm *pComm);
	int IsFilled(void);
	int GetTop(uint8 *pRec);
	int Append(const uint8 *pRec);
	CQueue(uint8 size,int number);
	virtual ~CQueue();
private:
	static uint8 s_queue_id_dispatch;
	uint8 m_queueId;
	int m_bValid;
	uint8 *m_queueBuffer;
	int m_cnt;
};

#endif // !defined(AFX_QUEUE_H__E68AD7F3_713C_4686_91EF_D682A722B03A__INCLUDED_)
