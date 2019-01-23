// NewCode.h: interface for the CNewCode class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: NewCode.h 1.1 2007/12/06 11:41:19PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */             

#if !defined(AFX_NEWCODE_H__60F99D4B_9E8E_43A1_9DD5_1E51BD583258__INCLUDED_)
#define AFX_NEWCODE_H__60F99D4B_9E8E_43A1_9DD5_1E51BD583258__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CLoader;
class Ccomm;

class CNewCode  
{
public:
	void NewCodeHack(Ccomm *pComm);
	void DetectCodeSize(void);
	int HasValidNewCode(void);
	void Refresh(void);
	void Init(void);
	static void Create(const char *pHeader, CLoader *pLoader);
	static CNewCode *s_pNewCode;
	void CheckCode(void);
protected:
	CNewCode(const char *pHeader, CLoader *pLoader);
	virtual ~CNewCode();
private:
	const char *m_pHeader;
	char m_header[HEADER_SIGNATURE_LENGTH];
	int volatile m_validNewCode;
	CLoader *m_pLoader;
	uint32 m_codeSize;
	uint16 m_checkSum;
	int m_possibleNewCode;
};

#endif // !defined(AFX_NEWCODE_H__60F99D4B_9E8E_43A1_9DD5_1E51BD583258__INCLUDED_)
