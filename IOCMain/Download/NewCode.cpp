// NewCode.cpp: implementation of the CNewCode class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: NewCode.cpp 1.2 2009/04/30 16:59:45PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */             

#include "NewCode.h"
#include "string.h"
#include "Loader.h"
#include "comm.h"               
#include "stdio.h"
#include "MinuteProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNewCode *CNewCode::s_pNewCode = NULL;

CNewCode::CNewCode(const char *pHeader, CLoader *pLoader)
{
	m_header[0] = 0;
	m_pHeader = pHeader;
	m_pLoader = pLoader;
	m_validNewCode = 0;
	m_codeSize = 0L;
	m_possibleNewCode = 0;
	m_checkSum = 0;
}

CNewCode::~CNewCode()
{

}

void CNewCode::Create(const char *pHeader, CLoader *pLoader)
{
	if (s_pNewCode == NULL) {
		s_pNewCode = new CNewCode(pHeader, pLoader);
	}
}

void CNewCode::Init()
{
	memcpy(m_header, m_pHeader, HEADER_SIGNATURE_LENGTH);
}

void CNewCode::Refresh()
{
	m_possibleNewCode = 1;
	CMinuteProcess::s_pMinuteProcess->TickHalfMinute();
}

// return 1: if different and valid code downloaded
// otherwise: 0
int CNewCode::HasValidNewCode()
{
	return (m_validNewCode == 1) ? (1) : (0);
}

void CNewCode::DetectCodeSize(void)
{
	int ret;
	unsigned long total;

	ret = m_pLoader->CheckDLDBlock((unsigned long)m_pHeader, &total);
	if (ret == 1) {
		m_codeSize = total;
		m_checkSum = m_pLoader->GetCheckSum();
	}
}

void CNewCode::NewCodeHack(Ccomm *pComm)
{
	char buff[100];

	HasValidNewCode();

	sprintf(buff, "NewCode: %s(%d) "
		"MySize=%ld CheckSum=0x%X\r\n"
		, (m_validNewCode == 1) ? ("New") : ("Invalid/Same"), m_validNewCode
		, m_codeSize, m_checkSum
		);
	pComm->Send(buff);
}

void CNewCode::CheckCode()
{
	int ret;
	unsigned long total;
	uint16 checkSum;

	if (m_possibleNewCode == 0) {
		return;
	}

	m_possibleNewCode = 0;

	// Check if valid
	ret = m_pLoader->CheckDLDBlock((unsigned long)m_pHeader, &total);
	if (ret != 1) {
		m_validNewCode = -1;
		return;
	}

	checkSum = m_pLoader->GetCheckSum();

	// Check if length different
	if ((total != m_codeSize) || (checkSum != m_checkSum)) {
		m_validNewCode = 1;
		return;
	}

	// Check if header changed
	if (memcmp(m_header, m_pHeader, HEADER_SIGNATURE_LENGTH) != 0) {
		m_validNewCode = 1;
		return;
	}

	m_validNewCode = 0;
}
