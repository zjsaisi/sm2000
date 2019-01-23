// RmtComm.h: interface for the CRmtComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMTCOMM_H__2D7E7373_46C7_4D71_BA9C_62DF9F60CC6F__INCLUDED_)
#define AFX_RMTCOMM_H__2D7E7373_46C7_4D71_BA9C_62DF9F60CC6F__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "comm.h"
#include "timer.h"
#include "PV.h"
                          
class CMemBlock;
class CPTPLink;

// Must be smaller than INPUT_SIZE
#define MAX_DEBUG_INPUT_LENGTH 1030
#define MAX_DEBUG_OUTPUT_LENGTH 1025

typedef struct {
	uint16 cmd; // PTP_CMD_DEBUG_STRING
	uint32 handle;
	char buffer[MAX_DEBUG_INPUT_LENGTH];
} SRemoteInput;

typedef struct {
	uint16 cmd; // PTP_CMD_DEBUG_STRING
	uint32 handle;
	int16 option; // 0: normal reply, 1: auto message
	char buffer[MAX_DEBUG_OUTPUT_LENGTH];
} SRemoteOutput;

class CRmtComm : public Ccomm  
{
public:
	void PrintRmtDbgStatus(unsigned long *param, Ccomm *pComm);
	void SetAoAllowed(uint32 cnt);
	int PostAO(const char *pBuffer, int len);
	virtual void flush(void);
	void Init(void);
	enum { PTP_IOC_PORT_REMOTE_DEBUG = 0x78, 
		PTP_IMC_PORT_REMOTE_DEBUG = 0x21,
		PTP_CMD_DEBUG_STRING = 0x280 // Changed from 0x180. To make it different from istate transfer.
	};
	int FeedInput(CMemBlock *pBlock);
	virtual void Close(void);
	virtual void Open(void);
	virtual int ForceSend(const char *pBuffer, int len = -1);
	virtual int Send(const char *pBuffer, int len = -1, int toOpen = 1);
	virtual int Receive(char *pBuffer, int max_len, long within_ms = 1000);
	virtual int WaitTxDone(long within_ms = 1000);
	virtual long GetBaudRate(void);
	virtual long SetBaudRate(long rate);
	CRmtComm(CCheapTimer *pTimer, CPVOperation *pPV);
	virtual ~CRmtComm();

private:
	int SendBlock(const char *pBuffer, int len, int16 option = 0);
	int DumpBlock(void);
	long m_rate; // fake one

	// First tier Input buffer
    enum {INPUT_SIZE = 1040 /* must be bigger than MAX_DEBUG_INPUT_LENGTH*/};
	char m_inputBuffer[INPUT_SIZE];
	volatile int16 m_inputHead;
	volatile int16 m_inputTail;

	// Second tier input buffer
	enum {NUM_INPUT_BLOCK = 20};
	CMemBlock *m_inputBlockList[NUM_INPUT_BLOCK];
	volatile int16 m_inputBlockHead;
	volatile int16 m_inputBlockTail;

	CCheapTimer *m_pTimer;
	CPVOperation *m_pPV;
	CPTPLink *m_pLink;

	// tmp solution
	uint32 m_handle;


	// Output buffer
	enum { OUTPUT_SIZE = 1024 };
	char m_outputBuffer[OUTPUT_SIZE];
	int m_outputSize;

	// Is AO allowed by PPC. <0 to shut it off
	int32 m_bAoAllowed;
};


#endif // !defined(AFX_RMTCOMM_H__2D7E7373_46C7_4D71_BA9C_62DF9F60CC6F__INCLUDED_)
