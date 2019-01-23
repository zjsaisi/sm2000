/*                   
 * Filename: download.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: BTMONDL.h 1.1 2007/12/06 11:41:18PST Zheng Miao (zmiao) Exp  $
 */             


#ifndef _download_h_
#define _download_h_

#include "CodeOpt.h"     
#include "DataType.h"
#include "ConstDef.h"

typedef struct 
{ 		
  unsigned char Flag;
  unsigned char Data_Size;
  unsigned char Chk_Sum;
  unsigned char Address[3];
  unsigned char Data_Field[256];
} dld_struct; 


class Ccomm;
class CFlash;

class CBtmonDL {
public:
	void Start(Ccomm *pWork, Ccomm *pDebug, CFlash *pFlash, unsigned long initAddress = MAIN_CODE_START);
	CBtmonDL(void);
	~CBtmonDL();
private:
	int ReceiveDLDRecord(void); 
	void ShowData(char *buffer, int len);
	int ReadMessage(void);  
	void RespondPing(void);
	void RespondAck(void); 
	int ReadWordFromCom(uint16 *pRet, long ms = 1000);
	int ReadByteFromCom(uint8 *pRet, long ms = 1000); 
	void DumpDLDRecord(dld_struct *pRec);
	Ccomm *m_pWork;
	Ccomm *m_pDebug; 
	CFlash *m_pFlash;
	unsigned long m_startAddress;
	uint16 m_dataAcc; // Total checksum	
	uint16 m_prevChecksum;
	dld_struct m_prevRecord;
	long m_oldBaudrate;
};

#endif
