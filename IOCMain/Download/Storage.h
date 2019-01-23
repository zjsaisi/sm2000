// Storage.h: interface for the CCodeStorage class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: Storage.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: Storage.h 1.2 2008/01/29 11:29:21PST Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.2 $
 */

#if !defined(AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_)
#define AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_

#include "string.h"
#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CFlash;
class CLoader;

class CStorage {
public:
	virtual int Seek(int32 offset);
	virtual void Rewind(void) = 0;
	virtual const char *GetName(void) { return m_name;};
	CStorage(const char *pName);
	virtual ~CStorage();
	virtual int Write(const char *pBuffer, int len) = 0;
	virtual int CheckIntegrity(uint32 *pTotal = NULL) = 0;
	virtual int Read(char *pBuffer, int len) = 0;
	virtual int CloseStorage(uint32 id) = 0;
	virtual int OpenStorage(uint32 id) = 0;
	virtual int EndOfDownload(void) { return 1; }; // Called at the end of download data from PPC.
	// These two used by CPTPDnldReport
	virtual int StartOfCollection(void) { return 1; }; // Called at the beginning of Istate dump from RTE to PPC
	virtual int EndOfCollection(void) { return 1; }; // Called at the end of Istate ==> Istate mem block
protected:
	char m_name[30];
};

class CCodeStorage : public CStorage
{
public:
	virtual int EndOfDownload(void);
	virtual int CloseStorage(uint32 id);
	virtual int OpenStorage(uint32 id);
	virtual int Read(char *pBuffer, int len);
	virtual int CheckIntegrity(uint32 *pTotal = NULL);
	virtual void Rewind(void);
#if 0
	void SetInit(CFlash *pFlash, unsigned long startAddress = MAIN_CODE_START);
#endif
	virtual int Write(const char *pBuffer, int len);
	CCodeStorage(const char *pName, CLoader *pLoader, uint32 startAddress = MAIN_CODE_START );
	virtual ~CCodeStorage();
private:
	CFlash *m_pFlash;
	CLoader *m_pLoader;
	unsigned long m_startAddress;
	unsigned long m_initStartAddress;
	uint32 m_user;
};

#endif // !defined(AFX_STORAGE_H__6A46FF78_08C6_4458_863E_55A1D20E577A__INCLUDED_)
