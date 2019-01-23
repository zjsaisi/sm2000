/*                   
 * Filename: sci.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: sci.h 1.2 2009/05/05 15:23:30PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             


#ifndef _SCI_DEFINED 
#define _SCI_DEFINED  

#include "CodeOpt.h"
#include "DataType.h"
#include "iodefine.h"

EXTERN void sci1_init(void);
EXTERN void sci_init(void);

EXTERN void sci0_rxd_error(void);
EXTERN void sci0_rxd(void);
EXTERN void sci0_txd(void);
EXTERN void sci0_txd_idle(void);

EXTERN void sci1_rxd_error(void);
EXTERN void sci1_rxd(void);
EXTERN void sci1_txd(void);
EXTERN void sci1_txd_idle(void);


#ifdef __cplusplus

//#include "zmdefine.h"  
#include "DataType.h"
#include "comm.h"
#include "timer.h"
#include "string.h"
#include "PV.h"
#include "ioext.h"

class CBaseProcess;
class CDtcSci;
 
class CSci: public Ccomm {
	friend void sci1_init(void);
	friend static void sci0_init(void);
public:
	void CheckInput(void);
	virtual void Close(void);
	virtual void Open(void);
	void SetSciNotification(CBaseProcess *pTask, uint32 from);
	virtual int ForceSend(const char *pBuffer, int len = -1);
	CSci(SCI_REG reg, CCheapTimer *pTimer,  CPVOperation *pPV, long rate = 9600,
		unsigned int DTCVector = 0);
	~CSci(); 
	 
	void init(void);
	void isr_rxd_error(void);
	void isr_rxd(void);
	void isr_txd(void);
	void isr_txd_idle(void);
	
	virtual long SetBaudRate(long rate);
	virtual long GetBaudRate(void);   
	virtual int WaitTxDone(long within_ms = 1000);
	virtual int Receive(char *pBuffer, int max_len, long within_ms = 1000);
	virtual int Recv(char *pBuffer, int max_len, long within_ms, int no_interrupt = 0);
	virtual int Send(const char *pBuffer, int len = -1, int toOpen = 1);
	
private:
	enum { TX_SIZE = 500, RX_SIZE =1100 };
	volatile int tx_head, tx_tail;
	volatile int rx_head, rx_tail;
	char tx_buffer[TX_SIZE];
	char rx_buffer[RX_SIZE];	
	volatile SCI_REG m_sci;               
	unsigned long m_rx_error;       
	unsigned long m_rx_total;
	unsigned long m_rx_overrun;
	unsigned long m_tx_total;     
	long m_baudrate;     
	uint8 m_intrLevel;     
	CCheapTimer *m_pTimer; 
	CPVOperation *m_pPV;
	CBaseProcess *m_pTask;
	uint32 m_from;
	int8 volatile m_bOpened;
	CDtcSci *m_pDTC;

};        

#ifndef DONOT_DECLARE_SCI01
extern CSci *g_pSci0;
extern CSci *g_pSci1;           
#endif

#endif // __cplusplus

#if ENABLE_SCI0
#ifndef SCI0_REG
#define SCI0_REG    ((struct st_sci   *)0xFFFF78) /* SCI0  Address*/
#endif
#endif

#if ENABLE_SCI1
#ifndef SCI1_REG
#define SCI1_REG (( struct st_sci   *)0xFFFF80) /* SCI1  Address*/
#endif
#endif
                      
#endif
