/*                   
 * Filename: loader.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: loader.h 1.1 2007/12/06 11:41:22PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             

#ifndef _Loader_H_10_07
#define _Loader_H_10_07   

#include "DataType.h"
#include "Btmondl.h"
#include "string.h"

class CFlash;

class CLoader {
public:
	CLoader(CFlash *pFlash);
	~CLoader();
	int CheckDLDBlock(unsigned long address, unsigned long *pTotal = NULL);  
	int ReadDLDRecord(uint8 *ptr, dld_struct *pRec);
	int Start(unsigned long startAddress = MAIN_CODE_START, int block_number = 15, 
		uint8 bCheckIntegrity = 1);
	CFlash *GetFlash(void) { return m_pFlash; };
	uint16 GetCheckSum(void) { return m_checkSum; };
private:	
	int Load(unsigned long address, unsigned long *pStartAddress, 
		unsigned long *pNextBlock);	
	CFlash *m_pFlash;
	uint16 m_checkSum;
}; 

// extern CLoader *g_loader;
#endif // _Loader_H_10_07
