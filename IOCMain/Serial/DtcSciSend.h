// DtcSciSend.h: interface for the CDtcSciSend class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DTCSCISEND_H__9340BE9F_4233_4441_8866_0B3F3487A95A__INCLUDED_)
#define AFX_DTCSCISEND_H__9340BE9F_4233_4441_8866_0B3F3487A95A__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "iodefine.h"
#include "ioext.h"

class Ccomm;

class CDtcSciSend  
{
public:
	int IsIdle(void);
	void PrintStatus(Ccomm *pComm);
	void SendIsr(void);
	void Test(void);
	void flush(void);
	int Send(const char *pStr, int len);
	CDtcSciSend(SCI_REG pSci, unsigned int vector);
	virtual ~CDtcSciSend();
private:
	inline void StartEngine(void);
	void flush1(void);
	int Send1(const char *pStr, int len);
	int StartSend(int8 fromIsr = 0);
	enum { BLOCK_NUM = 40, BLOCK_SIZE = 100 };
	SCI_REG volatile m_pSci;
	SDTCReg volatile *m_pReg;
	uint8 volatile *m_dtcer;
	uint8 m_dtcerData;
	char m_block[BLOCK_NUM][BLOCK_SIZE];
	int m_blockSize[BLOCK_NUM];
	int8 volatile m_sendingBlock;
	int8 volatile m_hotBlock;
	int8 volatile m_tailBlock;
	int volatile m_index;
	int32 m_isrCnt;
	unsigned int m_vector;
};

#endif // !defined(AFX_DTCSCISEND_H__9340BE9F_4233_4441_8866_0B3F3487A95A__INCLUDED_)
