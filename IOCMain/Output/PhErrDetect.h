// PhErrDetect.h: interface for the CPhErrDetect class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: PhErrDetect.h 1.1 2007/12/06 11:41:37PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */

#if !defined(AFX_PHERRDETECT_H__05BF3DB3_53C7_42E7_991C_E8EBFE5F52BC__INCLUDED_)
#define AFX_PHERRDETECT_H__05BF3DB3_53C7_42E7_991C_E8EBFE5F52BC__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CPhErrDetect  
{
public:
	void ResetPhaseError(void);
	int FeedPhaseError(int32 phase);
	CPhErrDetect(int chan);
	virtual ~CPhErrDetect();
private:
	int m_chan;
	int m_bucketStable;
	int m_rmsStable;
	int m_bucket;
	int32 m_rms;
};

#endif // !defined(AFX_PHERRDETECT_H__05BF3DB3_53C7_42E7_991C_E8EBFE5F52BC__INCLUDED_)


#if 0 // From siggen
#define PHASE_ERR_THRESHOLD		154 	/* 4ns peak */
#define PHASE_ERR_MS_ALARM_SET	2304	/* 1.25ns rms; 1.25ns=48, ms is 48^2 */
#define PHASE_ERR_MS_ALARM_CLR	1083	/* 1ns rms; 1ns=38, ms is 38^2 */
#define PHASE_ERR_MS_LIMIT		5929	/* 2ns rms; 2ns=77, ms is 77^2 */
#define BUCKET_UP_LIMIT			256
#define BUCKET_UP				32
#define BUCKET_DOWN				16
#define BUCKET_ALARM_SET		128		/* 4 peak error to trigger alarm */
#define BUCKET_ALARM_CLR		64		/* 4 to 12 good data to clear alarm */
	for (port=0; port<2; port++)
	{
		FPGA_REG.PORT_SELECT = port;
		if (((FPGA_REG.SERVER_STATUS_FLAGS&WARMUP_MODE) == 0) &&
			((FPGA_REG.PORT_STATUS.BYTE&0x03) == 0)) /* No LOS and no LOF */
		{
			/* Read phase data */
			phase_err = FPGA_REG.CLIENT_PHASE_ERR_MSB;
			phase_err = (phase_err << 8) + FPGA_REG.CLIENT_PHASE_ERR_1;
			phase_err = (phase_err << 8) + FPGA_REG.CLIENT_PHASE_ERR_LSB;
			if (phase_err & 0x800000)
			{
				// Sign extention if negative number
				phase_err |= 0xFF000000;
			}
			/* Update leacky bucket */
			if (phase_err < -PHASE_ERR_THRESHOLD || phase_err > PHASE_ERR_THRESHOLD)
			{
				if (phase_err_bucket[port] < BUCKET_UP_LIMIT)
				{
					phase_err_bucket[port] += BUCKET_UP;
				}
			}
			else
			{
				if (phase_err_bucket[port] > 0)
				{
					phase_err_bucket[port] -= BUCKET_DOWN;
				}
			}
			/* Check phase peak error alarm */
			if (phase_err_bucket[port] >= BUCKET_ALARM_SET)
			{
				client_phase_peak_alarm[port] = TRUE;
			}
			else if (phase_err_bucket[port] < BUCKET_ALARM_CLR)
			{
				client_phase_peak_alarm[port] = FALSE;
			}
			/* Filter phase square value (noise) */
			phase_err_square = phase_err * phase_err;
			phase_err_ms[port] -= (phase_err_ms[port] >> 4);	/* 93.75% of exsting value */
			phase_err_ms[port] += (phase_err_square >> 4);		/* 6.25% of new value */
			/* Clip at upper limit */
			if (phase_err_ms[port] > PHASE_ERR_MS_LIMIT)
			{
				phase_err_ms[port] = PHASE_ERR_MS_LIMIT;
			}
			/* Check phase error mean square alarm */
			if (phase_err_ms[port] > PHASE_ERR_MS_ALARM_SET)
			{
				client_phase_ms_alarm[port] = TRUE;
			}
			else if (phase_err_ms[port] < PHASE_ERR_MS_ALARM_CLR)
			{
				client_phase_ms_alarm[port] = FALSE;
			}
		}
	}

#endif
