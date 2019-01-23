// Storage.h: interface for the CCodeStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Storage.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Storage.h 1.1 2007/12/06 11:38:52PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.1 $
 */

#if !defined(AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_)
#define AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_

#include "string.h"

class CFlash;
class CLoader;

class CStorage {
public:
	virtual void Rewind(void) = 0;
	CStorage();
	virtual ~CStorage();
	virtual int Write(const char *pBuffer, int len) = 0;
	virtual int CheckIntegrity(uint32 *pTotal = NULL) = 0;
	virtual int Read(char *pBuffer, int len) = 0;
	virtual int CloseStorage(uint32 id) = 0;
	virtual int OpenStorage(uint32 id) = 0;
	virtual int EndOfStorage(void) { return 1; }; 
};

class CCodeStorage : public CStorage
{
public:
	virtual int EndOfStorage(void); 
	virtual int CloseStorage(uint32 id);
	virtual int OpenStorage(uint32 id);
	virtual int Read(char *pBuffer, int len);
	virtual int CheckIntegrity(uint32 *pTotal = NULL);
	virtual void Rewind(void);
#if 0
	void SetInit(CFlash *pFlash, unsigned long startAddress = 0x810000);
#endif
	virtual int Write(const char *pBuffer, int len);
	CCodeStorage(CLoader *pLoader, uint32 startAddress = 0x810000 );
	virtual ~CCodeStorage();
private:
	CFlash *m_pFlash;
	CLoader *m_pLoader;
	unsigned long m_startAddress;
	unsigned long m_initStartAddress;
	uint32 m_user;
};

#endif // !defined(AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_)
