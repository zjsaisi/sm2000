// IstateKbIoc.h: interface for the CIstateKbIoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISTATEKBIOC_H__33D68A1F_133C_4B64_9ECB_AF24333BCA71__INCLUDED_)
#define AFX_ISTATEKBIOC_H__33D68A1F_133C_4B64_9ECB_AF24333BCA71__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "IstateKb.h"
#include "IOCKeyList.h"

#define DEFINE_ISTATE_VARIABLE
typedef struct {

#define ISTATE_SHARE_FLASH // Istate share & flash
#include "IstateBasic.h"
#if DTI_ISTATE
#include "IstateDti.h"
#endif
#undef  ISTATE_SHARE_FLASH

	uint16 nvram_end;

#define ISTATE_SHARE_VOLATILE // Istate share & RAM
#include "IstateBasic.h"
#if DTI_ISTATE
#include "IstateDti.h"
#endif
#undef  ISTATE_SHARE_VOLATILE

	uint16 share_end;         // End of share

#include "IstateBasic.h"
#include "IstateDti.h"

} SIstateInRam;
#undef DEFINE_ISTATE_VARIABLE

class CIstateKbIoc : public CIstateKb  
{
	friend class CIstateBasic;
	friend class CIstateDti;
public:
	virtual uint8 * GetIstateLocation(uint16 *pSize, uint16 *pVersion);
	virtual uint8 * GetNonVolatileIstateLocation(uint16 *pSize, uint16 *pVersion);
	CIstateKbIoc(uint16 version, int16 start, int16 end);
	virtual ~CIstateKbIoc();
private:
	static SIstateInRam m_istateInRam;

};

#endif // !defined(AFX_ISTATEKBIOC_H__33D68A1F_133C_4B64_9ECB_AF24333BCA71__INCLUDED_)
