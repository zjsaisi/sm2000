// BlockStorage.h: interface for the CBlockStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: BlockStorage.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: BlockStorage.h 1.1 2007/12/06 11:41:19PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_BLOCKSTORAGE_H__85F5F72C_40DF_41A8_AA8E_AFB0638441A9__INCLUDED_)
#define AFX_BLOCKSTORAGE_H__85F5F72C_40DF_41A8_AA8E_AFB0638441A9__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "Storage.h"
#include "PV.h"
class Ccomm;

class CBlockStorage : public CStorage  
{
public:
	virtual int EndOfDownload(void);
	virtual int EndOfCollection(void);
	virtual int Seek(int32 offset);
	void PrintContent(Ccomm *pComm, int bASCII = 0);
	virtual int Read(char *pBuffer, int len);
	virtual int CheckIntegrity(uint32 *pTotal = NULL);
	virtual int CloseStorage(uint32 id);
	virtual int OpenStorage(uint32 id);
	virtual int Write(const char *pBuffer, int len);
	virtual void Rewind(void);
	CBlockStorage(const char *pName, int32 maxSize);
	virtual ~CBlockStorage();
protected:
	int32 m_offset;
	int32 m_currentSize;
private:
	int32 m_maxSize;
	uint8 *m_buffer;
	uint32 m_userId;
	static CPVOperation *m_pPV;
};

#endif // !defined(AFX_BLOCKSTORAGE_H__85F5F72C_40DF_41A8_AA8E_AFB0638441A9__INCLUDED_)
