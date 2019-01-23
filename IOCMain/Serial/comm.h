/*                   
 * Filename: comm.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: comm.h 1.2 2008/05/27 16:14:10PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             


#ifndef _comm_h_03478
#define _comm_h_03478

#include "DataType.h"

class Ccomm {
public:
	Ccomm(void) {};
	virtual ~Ccomm(void) {};
	virtual long SetBaudRate(long rate) = 0;
	virtual long GetBaudRate(void) = 0;   
	virtual int WaitTxDone(long within_ms = 1000) = 0;	
	virtual int Receive(char *pBuffer, int max_len, long within_ms = 1000)  = 0;
	virtual int Send(const char *pBuffer, int len = -1, int toOpen = 1) = 0;
	virtual int ForceSend(const char *pBuffer, int len = -1) = 0;
	virtual int GetRxSize(void) { return -1; };
	// 5/19/2004: This is used by boot code. When RAM failed, we cannot use interupption
	// So this is added to handle the special case. In main code, actually, there is 
	// no such need. Also, CDebug is changed not to use this function.
	// virtual int Recv(char *pBuffer, int max_len, long within_ms, int no_interrupt = 0) = 0;
	
	virtual void Open(void) = 0;
	virtual void Close(void) = 0;
	virtual int GetStopError(void) { return 0; };
	virtual void ResetStopError(void) { };
	virtual void flush(void) { }; // 5/24/2004: Make sure output is sent
};

#endif
