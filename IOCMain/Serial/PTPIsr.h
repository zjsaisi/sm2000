// PTPIsr.h: interface for the CPTPIsr class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: PTPIsr.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: PTPIsr.h 1.1 2007/12/06 11:41:38PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_PTPISR_H__C8E4C00E_74D5_4E46_B0B3_C45DAA6BD8A2__INCLUDED_)
#define AFX_PTPISR_H__C8E4C00E_74D5_4E46_B0B3_C45DAA6BD8A2__INCLUDED_

#ifdef __cplusplus
#include "CodeOpt.h"
#include "ioext.h"
#include "IntQueue.h"
#include "CRC16.h"
#include "ConstDef.h"
#include "comm.h"

class CPTPLink;
class CDtcSci;
class CDtcSciSend;

#ifndef INSERT_ERROR
#define INSERT_ERROR 0
#endif

#ifndef DEBUG_PHYSICAL
#define DEBUG_PHYSICAL 0
#endif
#define DEBUG_SIZE 4096

class CPTPIsr  
{
public: // For initialization
	// Set the link this is connected to.
	void SetLink(CPTPLink *pLink);
	// reg:  is the hardware register address pointing to SCI register
	// rate: is the baudrate
	CPTPIsr(const char *pName, SCI_REG reg, long rate, unsigned int DTCVector = 0, unsigned int sendVector = 0);
	// This is used to change baudrate.
	long SetBaudRate(long rate);
public: // Within PTP module
	void PrintStatus(Ccomm *pComm);
	void CheckInput(void);
	int RtrvFrame(int tail, char *buffer, int max);
	int SendFrame(const char *str, int len, const char *ctrl = NULL);
	long GetBaudRate(void);
	void isr_txd_idle(void);
	void isr_rxd_error(void);
	void isr_txd(void);
	void isr_rxd(void);
	virtual ~CPTPIsr();
	enum { FLAG = 0x7E, ESCAPE = 0x7d, FLAG2= 0x5e, ESCAPE2 = 0x5d };
private:
	int SendFrameDtc(const char *str, int len, const char *ctrl);
	void AppendByteDtc(char ch, int8 datatype = 0);
	CDtcSciSend *m_pSendDtc;

	inline void RecvCh(char ch);
	void StartSend(void);
	int AppendByte(char ch);
	void init(void);
	long m_baudrate;
	long m_realRate;
	volatile SCI_REG m_sci;
	// If DTC is not used. TX_SIZE should be 2K
	enum { TX_SIZE = 2048, RX_SIZE = FRAME_LENGTH + 100 };
	volatile int tx_head, tx_tail;
	volatile int rx_head, rx_tail;
	unsigned char tx_buffer[TX_SIZE];
	unsigned char rx_buffer[RX_SIZE];	

	// Sending CRC, stuffing
	CIntQueue m_frameEnds; // Those pointed by points are ends of frame.
	CCRC16 m_sendCRC;
	
	// Receiving CRC, destuffing
	int8 m_recvEscape;
	CCRC16 m_recvCRC;

	// Statistic info
	unsigned long m_rx_error;       
	unsigned long m_rx_total;
	unsigned long m_rx_overrun;
	unsigned long m_tx_total;     

	// Link handle
	CPTPLink *m_pLink;

	// DTC buffer
	CDtcSci *m_pDTC;

	char m_name[21];

	// debug info
#if DEBUG_PHYSICAL
	int32 m_alignDebugBuffer;
	char m_debugBuffer[DEBUG_SIZE];
	int m_debugIndex;
	uint32 m_crcErrorCnt;
	uint32 m_invalidLengthCnt;
#endif
};

extern CPTPIsr *g_pSci3;
extern CPTPIsr *g_pSci4;

#endif // __cplusplus

// Test
#if FAKE_SCI3
#define SCI3_REG    ((struct st_sci   *)0xFFFF80) /* SCI1 0xFFFF80 Address, SCI3: 0xFFFDD0 */
#else
#define SCI3_REG    ((struct st_sci   *)0xFFFDD0) /* SCI1 0xFFFF80 Address, SCI3: 0xFFFDD0 */
#endif
#define SCI4_REG    ((struct st_sci   *)0xFFFDD8) /* SCI4  Address*/

EXTERN void sci3_rxd_error(void);
EXTERN void sci3_rxd(void);
EXTERN void sci3_txd(void);
EXTERN void sci3_txd_idle(void);

EXTERN void sci4_rxd_error(void);
EXTERN void sci4_rxd(void);
EXTERN void sci4_txd(void);
EXTERN void sci4_txd_idle(void);
EXTERN void init_ptp(void);

#endif // !defined(AFX_PTPISR_H__C8E4C00E_74D5_4E46_B0B3_C45DAA6BD8A2__INCLUDED_)
