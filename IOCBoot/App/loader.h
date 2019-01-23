/*                   
 * Filename: loader.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: loader.h 1.1 2007/12/06 11:38:53PST Zheng Miao (zmiao) Exp  $
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
	int Start(unsigned long startAddress = 0x810000, int block_number = 15, 
		uint8 bCheckIntegrity = 1);
	CFlash *GetFlash(void) { return m_pFlash; };
private:	
	int Load(unsigned long address, unsigned long *pStartAddress, 
		unsigned long *pNextBlock);	
	CFlash *m_pFlash;	
}; 

// extern CLoader *g_loader;
#endif // _Loader_H_10_07
