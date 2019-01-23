// DtcSciSend.cpp: implementation of the CDtcSciSend class.
//
//////////////////////////////////////////////////////////////////////

#include "DtcSciSend.h"
#include "string.h"
#include "AllProcess.h"
#include "machine.h"
#include "comm.h" 
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MRA_CONSTANT 0x80000000
#define PAUSE CAllProcess::g_pAllProcess->Pause

CDtcSciSend::CDtcSciSend(SCI_REG pSci, unsigned int vector)
{
	uint16 *pVector;
	pVector = (uint16 *)(0x400 + (vector << 1));
	m_pReg = (SDTCReg *)(0xff0000 + *pVector);
	m_pSci = pSci;
	m_vector = vector;
	m_hotBlock = -1;

	uint8 *pDst;
	switch (vector) {
	case 82: // TXI0
		pDst = (uint8 *)(&(SCI0.TDR));
		DTC.EE.BIT.B2 = 0;
		m_dtcer = &(DTC.EE.BYTE);
		m_dtcerData = 4;
		break;
	case 86: // TXI1
		pDst = (uint8 *)(&(SCI1.TDR));
		DTC.EE.BIT.B0 = 0;
		m_dtcer = &(DTC.EE.BYTE);
		m_dtcerData = 1;
		break;
	case 122: // TXI3 
		pDst = (uint8 *)(&(SCI3.TDR));
		DTC.EI.BIT.B6 = 0;
		m_dtcer = &(DTC.EI.BYTE);
		m_dtcerData = 0x40;
		break;
	case 126: // TXI4   
		pDst = (uint8 *)(&(SCI4.TDR));
		DTC.EI.BIT.B4 = 0;
		m_dtcer = &(DTC.EI.BYTE);
		m_dtcerData = 0x10;
		break;
	default:
		pDst = NULL;
		break;
	}
	m_pReg->SRC._SAR = ((unsigned long)(&m_block[0][0]) + MRA_CONSTANT);
	m_pReg->DEST._DAR = (unsigned long)pDst;


	// Setup MRB
	// No chain
	m_pReg->DEST.MRB.BIT.CHNE = 0;
	// Enable CPU interrupt after transfer
	m_pReg->DEST.MRB.BIT.DISEL = 0; // 0; disable

	m_pReg->CRA.WORD = 0;

	// Make CRB 0. Not required
	m_pReg->CRB.WORD = 0;

	m_pSci->SCR.BIT.TIE = 0;

	// Init data
	m_sendingBlock = 0;
	m_tailBlock = 0;
	m_index = 0;
	for (int i = 0; i < BLOCK_NUM; i++) {
		m_blockSize[i] = 0;
	}
	memset(&m_block[0][0], 'D', sizeof(m_block));

	m_isrCnt = 0L;
}

void CDtcSciSend::Test()
{
#if 0
	int i;
	memset(&m_block[0][0], 'Z', sizeof(m_block));
	memset(&m_block[0][0], 'A', BLOCK_SIZE); m_blockSize[0] = BLOCK_SIZE - 1;
	memset(&m_block[1][0], 'B', BLOCK_SIZE); m_blockSize[1] = BLOCK_SIZE - 1;
	memset(&m_block[2][0], 'C', BLOCK_SIZE); m_blockSize[2] = BLOCK_SIZE - 1;
	for (i = 0; i < BLOCK_SIZE - 1; i++) {
		m_block[0][i + 1] = '0' + i;
		m_block[2][i + 1] = 'a' + i;
	}
//	for (int i = 0; i < BLOCK_NUM; i++) {
//		m_block[i][BLOCK_SIZE - 2] = '\r';
//		m_block[i][BLOCK_SIZE - 1] = '\n';
//	}
	m_tailBlock = 3; m_sendingBlock = 0;

	//m_pReg->SRC._SAR = ((unsigned long)(&m_block[0][0]) + MRA_CONSTANT);
	//m_pReg->CRA.WORD = BLOCK_SIZE -1; 
	//m_pSci->SCR.BIT.TIE = 0;
	//*m_dtcer |= m_dtcerData;
	//m_pSci->SCR.BIT.TIE = 1;
	StartSend();
#endif

#if 0
	char const *ptr;
	ptr = "This is a test string of this long\r\nAnd this line]";

	Send1(ptr, strlen(ptr));
	flush1();
#endif
}

// 1: more data
// 0: no more data
int CDtcSciSend::StartSend(int8 fromIsr)
{
	int8 nextBlock;

	if (m_sendingBlock == m_tailBlock) return 0; // No data. This is safeguard only. Shouldn't happen

	if (m_pReg->CRA.WORD != 0) {
		// If already running. Make sure it's enabled.
		StartEngine();
		return 1;
	}

	nextBlock = (m_sendingBlock + 1) % BLOCK_NUM;
	m_pReg->SRC._SAR = ((unsigned long)(&m_block[m_sendingBlock][0]) | MRA_CONSTANT);
	m_pReg->CRA.WORD = m_blockSize[m_sendingBlock];
	m_hotBlock = m_sendingBlock;
	m_sendingBlock = nextBlock;
	StartEngine();
	return 1;
}

void CDtcSciSend::SendIsr()
{
	int ret;

	m_isrCnt++;
	m_pSci->SSR.BIT.TDRE = 0; // Key to ship out the last byte.
	ret = StartSend(1);
	if (ret == 1) {
		return;
	}
	m_pSci->SCR.BIT.TIE = 0;
	m_hotBlock = -1;
	*m_dtcer &= (~m_dtcerData);
}

CDtcSciSend::~CDtcSciSend()
{

}

int CDtcSciSend::Send(const char *pStr, int len)
{
	return Send1(pStr, len);
}


void CDtcSciSend::flush()
{
	flush1();
}

void CDtcSciSend::PrintStatus(Ccomm *pComm)
{
	char buff[100];

	sprintf(buff, "DTC TX%d  ISR_CNT=%ld\r\n",
		m_vector, m_isrCnt);
	pComm->Send(buff);
	sprintf(buff, "SendBlk: %d  TailBlk: %d  Index: %d\r\n"
		, m_sendingBlock, m_tailBlock, m_index);
	pComm->Send(buff);
}

int CDtcSciSend::Send1(const char *pStr, int len)
{
	int8 nextBlock;
	int cut;
	int8 full = 0;
	uint8 tmp_exr = get_exr();
	int sent = len;
	
	while (len > 0) {
		set_exr(7);
		cut = BLOCK_SIZE - m_index;
		cut = (cut > len) ? (len) : (cut);
		memcpy(&m_block[m_tailBlock][m_index], pStr, cut);
		m_index += cut;
		len -= cut;
		pStr += cut;
		if (m_index >= BLOCK_SIZE) {
			// Go to next block
			full = 0;
			nextBlock = (m_tailBlock + 1) % BLOCK_NUM;
			while ((nextBlock == m_sendingBlock) || (nextBlock == m_hotBlock)) {
				// Full.
				if (!full) {
					// Make sure it's started.
					if (m_pSci->SCR.BIT.TIE == 0) {
						StartSend();
					}
					full = 1;
				}
				set_exr(tmp_exr);
				PAUSE(1);
			}
			set_exr(7);
			m_blockSize[m_tailBlock] = BLOCK_SIZE;
			m_tailBlock = nextBlock;
			m_index = 0;
			if (m_pSci->SCR.BIT.TIE == 0) {
				StartSend();
			}
			continue;
		}
	}
	set_exr(tmp_exr);
	return sent;
}

void CDtcSciSend::flush1()
{
	int8 nextBlock;
	int8 full = 0;
	uint8 tmp_exr;

	tmp_exr = get_exr();

	set_exr(7);
	if (m_index == 0) {
		if (m_sendingBlock == m_tailBlock) {
			set_exr(tmp_exr);
			return;
		}
		if (m_pSci->SCR.BIT.TIE == 0) {
			StartSend();
		}
		set_exr(tmp_exr);
		return;
	}


	while (1) {
		set_exr(7);
		full = 0;
		nextBlock = m_tailBlock + 1;
		nextBlock %= BLOCK_NUM;
		while ((nextBlock == m_sendingBlock) || (nextBlock == m_hotBlock)) {
			if (!full) {
				if (m_pSci->SCR.BIT.TIE == 0) {
					StartSend();
				}
				full = 1;
			}
			set_exr(tmp_exr);
			PAUSE(1);
		}
		set_exr(7);
		if (full) continue;
		if (m_index == 0) break;
		m_blockSize[m_tailBlock] = m_index;
		m_tailBlock = nextBlock;
		m_index = 0;
		if (m_pSci->SCR.BIT.TIE == 0) {
			StartSend();
		}
		break;
	}
	
	set_exr(tmp_exr);
}

void CDtcSciSend::StartEngine()
{
	*m_dtcer |= m_dtcerData;
	m_pSci->SCR.BIT.TIE = 1;

}

// 1 idle 
// 0 busy
int CDtcSciSend::IsIdle()
{
	if (m_sendingBlock != m_tailBlock) return 0;
	if (m_hotBlock >= 0) return 0;
	return 1;
}
