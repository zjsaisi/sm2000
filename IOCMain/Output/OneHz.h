/* OneHz.h: Interface for COneHzOutput
*/
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: OneHz.h 1.2 2008/01/11 17:00:08PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 * Manage 1PPS output setup
 */

#if !(defined(_ONEHZ_H_01_11_2008))
#define _ONEHZ_H_01_11_2008

#include "TeleOutput.h"

class COneHzOutput : public CTeleOutput
{
public:
	enum { ONE_HZ_CHAN = 4 };
	static COneHzOutput *s_pOneHzOutput;
protected:
	COneHzOutput(int chan);
	virtual ~COneHzOutput();
public:
	static int CreateOneHzOutput(void);
	virtual void SetupOutput(void);
};

#endif /* _ONEHZ_H_01_11_2008 */