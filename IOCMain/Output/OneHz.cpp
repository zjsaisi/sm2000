/* OneHz.cpp: Interface for COneHzOutput
*/
/* 
 * Author: Zheng Miao
 * All rights reserved. 
 * $Header: OneHz.cpp 1.5 2010/01/19 11:44:29PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.5 $
 * Manage 1PPS output setup
 */

#include "OneHz.h"
#include <string.h>
#include "OutputProcess.h"
#include "Fpga.h"

#define TRACE TRACE_OUTPUT

COneHzOutput *COneHzOutput::s_pOneHzOutput = NULL;

COneHzOutput::COneHzOutput(int chan) : CTeleOutput(chan)
{
	m_valid = 1; // Always valid
}

COneHzOutput::~COneHzOutput()
{
}

int COneHzOutput::CreateOneHzOutput(void)
{
	if (s_pOneHzOutput == NULL) {
		s_pOneHzOutput = new COneHzOutput(ONE_HZ_CHAN);
		if (s_pOneHzOutput != NULL) {
			return 1;
		}
	}
	return 0;
}

void COneHzOutput::SetupOutput(void)
{
	int on;
	on = QuerySwitch();
	if (on == OUTPUT_CHOICE_ON) {
		TRACE("Turn on 1PPS enable=%d Loop=%d\r\n", m_enabled, m_currentLoop);
		CFpga::s_pFpga->Set1PPSOutput(1);
		ReportOutputSwitchState(OUTPUT_CHOICE_ON);
	} else {
		TRACE("Turn off 1pps enable=%d Loop=%d\r\n", m_enabled, m_currentLoop);
		CFpga::s_pFpga->Set1PPSOutput(0);
		ReportOutputSwitchState(OUTPUT_CHOICE_OFF);
	}
}
