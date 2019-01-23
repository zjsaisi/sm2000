/*                   
 * Filename: comm.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: comm.h 1.1 2007/12/06 11:39:18PST Zheng Miao (zmiao) Exp  $
 */             


#ifndef _comm_h_03478
#define _comm_h_03478

#include "DataType.h"

class Ccomm {
public:
	Ccomm(void) {};
	~Ccomm(void) {};
	virtual long SetBaudRate(long rate) = 0;
	virtual long GetBaudRate(void) = 0;   
	virtual int WaitTxDone(long within_ms = 1000) = 0;	
	virtual int Receive(char *pBuffer, int max_len, long within_ms = 1000)  = 0;
	virtual int Send(const char *pBuffer, int len = -1, int toOpen = 1) = 0;
	virtual int ForceSend(const char *pBuffer, int len = -1) = 0;
	virtual int Recv(char *pBuffer, int max_len, long within_ms, int no_interrupt = 0) = 0;
		 
};

#endif
