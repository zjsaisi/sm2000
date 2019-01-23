/* TenMHz.cpp: Implementation for CTenMHzOutput
 */
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: TenMHz.cpp 1.5 2010/01/19 11:44:29PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 * Manage 10Mhz output setup
 */

#include "TenMHz.h"
#include <string.h>
#include "OutputProcess.h"
#include "Fpga.h"

#define TRACE TRACE_OUTPUT

CTenMHzOutput *CTenMHzOutput::s_pTenMHzOutput = NULL;

CTenMHzOutput::CTenMHzOutput(int chan) : CTeleOutput(chan)
{
	m_valid = 1; // Always valid
}

CTenMHzOutput::~CTenMHzOutput()
{
}

int CTenMHzOutput::CreateTenMHzOutput(void)
{
	if (s_pTenMHzOutput != NULL) return 0;
	s_pTenMHzOutput = new CTenMHzOutput(TEN_MHZ_CHAN);
	if (s_pTenMHzOutput == NULL) return 0;
	return 1;
}

void CTenMHzOutput::SetupOutput(void)
{
	int on;
	on = QuerySwitch();
	if (on == OUTPUT_CHOICE_ON) {
		TRACE("Turn on 10Mhz enable=%d Loop=%d\r\n", m_enabled, m_currentLoop);
		CFpga::s_pFpga->Set10MHzOutput(1);
		ReportOutputSwitchState(OUTPUT_CHOICE_ON);
	} else {
		TRACE("Turn off 10Mhz enable=%d Loop=%d\r\n", m_enabled, m_currentLoop);
		CFpga::s_pFpga->Set10MHzOutput(0);
		ReportOutputSwitchState(OUTPUT_CHOICE_OFF);
	}
}
