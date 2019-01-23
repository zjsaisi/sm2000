/* TenMHz.h: Interface for CTenMHzOutput
 */
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: TenMHz.h 1.2 2008/01/11 17:00:21PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 * Manage 10Mhz output setup
 */
#if !(defined(_TEN_MHZ_01_11_2008))
#define _TEN_MHZ_01_11_2008

#include "TeleOutput.h"

class CTenMHzOutput : public CTeleOutput 
{
public:
	enum {TEN_MHZ_CHAN = 5};
	static CTenMHzOutput *s_pTenMHzOutput;
protected:
	CTenMHzOutput(int chan);
	virtual ~CTenMHzOutput();
public:
	static int CreateTenMHzOutput(void);
	// Called by output process
	virtual void SetupOutput(void);
};

#endif /* _TEN_MHZ_01_11_2008 */