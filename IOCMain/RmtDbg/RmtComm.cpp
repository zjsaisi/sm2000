// RmtComm.cpp: implementation of the CRmtComm class.
//
//////////////////////////////////////////////////////////////////////

#include "RmtComm.h"
#include "MemBlock.h"
#include "PTPLink.h"
#include "string.h"
#include "AllProcess.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRmtComm::CRmtComm(CCheapTimer *pTimer, CPVOperation *pPV)
{
	m_rate = 9600;
	m_outputBuffer[0] = 0;
	m_inputBuffer[0] = 0;
	m_inputBlockList[0] = NULL;
	m_inputHead = 0;
	m_inputTail = 0;
	m_pTimer = pTimer;
	m_pPV = pPV;
	m_pLink = NULL;
	m_handle = 0L;
	m_inputBlockHead = 0;
	m_inputBlockTail = 0;
	m_outputSize = 0;
	m_bAoAllowed = 1000L;
}

CRmtComm::~CRmtComm()
{

}

long CRmtComm::SetBaudRate(long rate)
{
	long tmp = m_rate;
	m_rate = rate;
	return tmp;
}

long CRmtComm::GetBaudRate()
{
	return m_rate;
}

int CRmtComm::WaitTxDone(long within_ms)
{
	return 1;
}

int CRmtComm::SendBlock(const char *pBuffer, int len, int16 option)
{
	SRemoteOutput output;
	uint32 handle;
	int total = len;       
	int block;
	int ret;
	int failCnt;

	handle = m_handle;

	if (handle == 0) {
		return 0;
	}

	if (m_pLink == NULL) return 0; // Safe guard

	output.cmd = PTP_CMD_DEBUG_STRING;
	output.handle = handle;
	output.option = option;
	while (len > 0) {
		// Leave a space at the end for PPC to add null. It's not sent from RTE.
		if (len > MAX_DEBUG_OUTPUT_LENGTH - 1) {
			block = MAX_DEBUG_OUTPUT_LENGTH -1; 
		} else {
			block = len;
		}
		memcpy(output.buffer, pBuffer, block);
		failCnt = 0;
		do {
			ret = m_pLink->PostFrame((char *)&output, block + 8, PTP_IMC_PORT_REMOTE_DEBUG, 3);
			if (ret <= 0) {
				if (ret <= -4) { // busy. Then wait.
					CAllProcess::g_pAllProcess->Pause(10);
				} else {
					break;
				}
			}
		} while ((ret <= 0) && (failCnt++ < 10));
		len -= block;	
		pBuffer += block;
	}
	return total;
}

void CRmtComm::flush()
{
	if (m_outputSize != 0) {
		SendBlock(m_outputBuffer, m_outputSize);
		m_outputSize = 0;
	}
}

int CRmtComm::Send(const char *pBuffer, int len, int toOpen)
{
	int vacancy;    
	int total;
	if (len < 0) {
		len = strlen(pBuffer);
	}
    
    total = len;
	while (len > 0) {
		vacancy = OUTPUT_SIZE - m_outputSize;
		if (vacancy > len) {
			vacancy = len;
		}
		memcpy(m_outputBuffer + m_outputSize, pBuffer, vacancy);
		m_outputSize += vacancy;
		pBuffer += vacancy;
		len -= vacancy;
		if (len > 0) {
			// still more to do. first send out the buffer.
			SendBlock(m_outputBuffer, m_outputSize);
			m_outputSize = 0;
		}
	}
    return total;
    
}

int CRmtComm::ForceSend(const char *pBuffer, int len)
{
	return 0;
}

void CRmtComm::Open()
{

}

void CRmtComm::Close()
{

}

int CRmtComm::Receive(char *pBuffer, int max_len, long within_ms)
{
	int16 next;
	int len = 0;
	if (m_pTimer) {
		m_pTimer->Start(within_ms);
	}
	while (max_len) {
		if (m_inputHead != m_inputTail) {
			// There is data
			*pBuffer = m_inputBuffer[m_inputHead];
			next = (m_inputHead >= INPUT_SIZE - 1) ? (0) : (m_inputHead + 1);
			m_inputHead = next;
			max_len--;
			len++;
			continue;
		} else {
			DumpBlock(); // See if we can get input from second tier
			if (m_pTimer) {
				// no data
				if (m_pTimer->TimeOut()) break;
				m_pPV->Pause(1); // pause a tick
			}
		}
	}
	return len;
}

// return 1: if there is a block to dump from second tier to first tier
// otherwise return 0.
int CRmtComm::DumpBlock()
{
	CMemBlock *pBlock = NULL;
	SRemoteInput *pInput;
	int cnt = 0;
	int buff_len;
	int i;
	char *ptr;

	while (m_inputBlockTail != m_inputBlockHead) {
		if (pBlock != NULL) 
			pBlock->Release();
		pBlock = m_inputBlockList[m_inputBlockHead];
		m_inputBlockHead = (m_inputBlockHead >= NUM_INPUT_BLOCK - 1) ? (0) : (m_inputBlockHead + 1);
		pInput = (SRemoteInput *)(pBlock->GetBuffer());
		if (pInput->cmd != PTP_CMD_DEBUG_STRING) {
			continue; // invalid cmd. Sorry to find it late.
		}
		buff_len = pBlock->GetCurSize() - 6;
		if (buff_len >= MAX_DEBUG_INPUT_LENGTH) {
			continue; // input buffer too big. shouldn't happen. Too bad.
		}
		// Feed the stuff into the first tier.
		i = m_inputTail;
		ptr = pInput->buffer;
		cnt = 1;
		while (buff_len--) {
			m_inputBuffer[i++] = *ptr++;
			if (i >= INPUT_SIZE) i = 0;
			if (i == m_inputHead) {
				// too much input. almost impossible. Trash input
				i = m_inputTail;
				cnt = 0;
				break;
			}
		}
		m_inputTail = i;
		if (cnt) {
			m_handle = pInput->handle;
			break; // Each time only dump one command
		}
	}
	if (pBlock != NULL) {
		pBlock->Release();
	}
	return cnt;
}

// Input string fed from PTP
int CRmtComm::FeedInput(CMemBlock *pBlock)
{
	int next;

	// Make this short to relieve the burden on PTP process.
	next = (m_inputBlockTail >= NUM_INPUT_BLOCK - 1) ? (0) : (m_inputBlockTail+1);
	if (next == m_inputBlockHead) {
		// Full. Tooo many command
		return -1; // This will cause caller to release the block.
	}
	m_inputBlockList[m_inputBlockTail] = pBlock;
	m_inputBlockTail = next;

	if (m_bAoAllowed == 0) {
		int len;
		len = pBlock->GetCurSize();
		if ((len < 20) && (m_bAoAllowed < 10000000L)) {
			m_bAoAllowed += 1000000L;
		}
	}
	return 1;

}

void CRmtComm::Init()
{
	m_pLink = CAllProcess::g_pAllProcess->GetImcLink();
	m_handle = 0xffffffff;
}

// return -1: deny
int CRmtComm::PostAO(const char *pBuffer, int len)
{
	if (m_bAoAllowed <= 0) {
		return -1;
	}
	m_bAoAllowed--;
	return SendBlock(pBuffer, len, 1);
}

void CRmtComm::SetAoAllowed(uint32 cnt)
{
	m_bAoAllowed = cnt;
}

void CRmtComm::PrintRmtDbgStatus(unsigned long *param, Ccomm *pComm)
{
	int success = 0;
	char buff[200];

	if (*param == 1) {
		switch (param[1]) {
		case 0:
			success = 1;
			sprintf(buff, "RmtDbg Status:\r\nAO allowed: %ld\r\n", 	m_bAoAllowed);
			pComm->Send(buff);
			break;
		default:
			break;
		}
	}
	if (*param == 2) {
		if (param[1] == 1) {
			uint32 cnt;
			success = 1;
			cnt = param[2];
			//if (cnt >= 10000000) {
			//	cnt = 10000000;
			//}
			SetAoAllowed(cnt);
		}
	}
	if (!success) {
		// Print help
		const char *ptr;
		ptr = "Syntax: <option>\r\n0\t: Display status\r\n1 <cnt>: Set AO lines allowed. <cnt> < 0 to stop AO and prohibit PPC to auto enable it\r\n";
		pComm->Send(ptr);
	}
}
