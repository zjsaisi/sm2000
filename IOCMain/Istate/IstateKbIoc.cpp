// IstateKbIoc.cpp: implementation of the CIstateKbIoc class.
//
//////////////////////////////////////////////////////////////////////
#include "CodeOpt.h"
#include "DataType.h"
#include "IstateKbIoc.h"
#include "string.h"  

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma section ISTATE
SIstateInRam CIstateKbIoc::m_istateInRam;
#pragma section

CIstateKbIoc::CIstateKbIoc(uint16 version, int16 start, int16 end) :
CIstateKb(version, start, end)
{
	memset(&m_istateInRam, 0, sizeof(m_istateInRam));
}

CIstateKbIoc::~CIstateKbIoc()
{

}

uint8 * CIstateKbIoc::GetNonVolatileIstateLocation(uint16 *pSize, uint16 *pVersion)
{
	if (pVersion)
		*pVersion = m_istateVersion;
	if (pSize) 
		*pSize = (uint8 *)(&m_istateInRam.nvram_end) - (uint8 *)(&m_istateInRam); 
	return (uint8 *)(&m_istateInRam);

}

uint8 * CIstateKbIoc::GetIstateLocation(uint16 *pSize, uint16 *pVersion)
{
	if (pVersion) 
		*pVersion = m_istateVersion;
	if (pSize)
		*pSize = (uint8 *)(&m_istateInRam.share_end) - (uint8 *)(&m_istateInRam);
	return (uint8 *)(&m_istateInRam);

}
