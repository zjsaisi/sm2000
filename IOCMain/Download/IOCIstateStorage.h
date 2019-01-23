// IOCIstateStorage.h: interface for the CIOCIstateStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: IOCIstateStorage.h 1.1 2007/12/06 11:41:19PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#if !defined(AFX_IOCISTATESTORAGE_H__2085104B_4420_44CB_9370_FBDA96EF46F0__INCLUDED_)
#define AFX_IOCISTATESTORAGE_H__2085104B_4420_44CB_9370_FBDA96EF46F0__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "BlockStorage.h"

class CIstateParser;

class CIOCIstateStorage : public CBlockStorage  
{
public:
	virtual int EndOfDownload(void);
	virtual int StartOfCollection(void);
	void SetParser(CIstateParser *pParser);
	CIOCIstateStorage(const char *pName, int32 maxSize);
	virtual ~CIOCIstateStorage();
private:
	CIstateParser *m_pParser;
};

#endif // !defined(AFX_IOCISTATESTORAGE_H__2085104B_4420_44CB_9370_FBDA96EF46F0__INCLUDED_)
