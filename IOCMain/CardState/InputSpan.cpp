// InputSpan.cpp: implementation of the CInputSpan class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputSpan.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputSpan.cpp 1.23 2011/07/09 09:50:36PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.23 $
 */             


#include "InputSpan.h"
#include <string.h>
#include <stdio.h>
#include "Fpga.h"
#include "IstateDti.h"
#include "InputSelect.h"
#include "BT3Support.h"
#include "CardProcess.h"
#include "IstateProcess.h"
#include "AllProcess.h"
#include "CardStatus.h"

#ifndef TRACE
#define TRACE TRACE_CARD
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputSpan *CInputSpan::s_pInputSpan[NUM_SPAN_INPUT] = {NULL, NULL};

CInputSpan::CInputSpan(int chan, const char *pName) : CInput(chan, pName, 0)
{ 
	m_frame = -1;
	m_crc4 = -1;
	m_ssmBit = -1;
	m_ssmRead = -1;
	m_assumedQL = -1;
	m_currentQL = 15;
	m_index = chan - CHAN_BITS1;
	m_ssmVote.InitSsm(m_currentQL);
	m_currentQL = 12;
	m_prevSSM = -1;
}

CInputSpan::~CInputSpan()
{

}

void CInputSpan::InitChan()
{
	CInput::InitChan();
}

int CInputSpan::CreateSpan()
{
	int i;
	int ret = 0;
	char name[10];
	for (i = 0; i < NUM_SPAN_INPUT; i++) {
		sprintf(name, "BITS%d", i+1);
		if (s_pInputSpan[i] == NULL) {
			s_pInputSpan[i] = new CInputSpan(CHAN_BITS1 + i, name);
			ret++;
		}
	}
	return ret;
}

int CInputSpan::CfFrameChanInput(int frame)
{
	if (m_frame == frame) return 0;
	m_frame = frame;
	RefreshInputSetup();
	return 1;
}

int CInputSpan::CfCrc4ChanInput(int crc4)
{
	if (m_crc4 == crc4) return 0;
	m_crc4 = crc4;
	RefreshInputSetup();
	return 1;
}

int CInputSpan::CfSsmReadChanInput(int ssmRead)
{
	if (m_ssmRead == ssmRead) return 0;
	m_ssmRead = ssmRead;
	RefreshInputSetup();
	return 1;
}

int CInputSpan::CfAssumedQLChanInput(int assumed)
{
	if (m_assumedQL == assumed) return 0;
	m_assumedQL = assumed;
	RefreshInputSetup();
	return 1;
}

int CInputSpan::InputHack(unsigned long *param, Ccomm *pComm)
{
	int ret;
	char buff[256];

	// Hack Span input specific info

	ret = CInput::InputHack(param, pComm);
	if (ret < 0) return ret;

	// Print extra help info
	sprintf(buff, "\tm_assumedQL: %d\r\n", m_assumedQL);
	pComm->Send(buff);


	return 0;

}

int CInputSpan::CheckFrame(void)
{
	int ret = 0;
	switch (m_frame) {
	case CIstateDti::SPAN_INPUT_FRAME_1544MHZ:
		// Make sure 1.544Mhz signal is not T1 signal
		ret = CFpga::s_pFpga->Check1544MHZ(m_chan);
		break;
	case CIstateDti::SPAN_INPUT_FRAME_2MHZ:
		// Make sure 2Mhz is not 2Mbps signal
		ret = CFpga::s_pFpga->Check2MHZ(m_chan);
		break;
	case CIstateDti::SPAN_INPUT_FRAME_CCS:
		if (m_crc4) {
			ret = CFpga::s_pFpga->CheckE1(m_chan, 0, 1);
		} else {
			ret = CFpga::s_pFpga->CheckE1(m_chan, 0, 0);
		}
		break;
	case CIstateDti::SPAN_INPUT_FRAME_CAS:
		if (m_crc4) {
			ret = CFpga::s_pFpga->CheckE1(m_chan, 1, 1);
		} else {
			ret = CFpga::s_pFpga->CheckE1(m_chan, 1, 0);
		}
		break;
	case CIstateDti::SPAN_INPUT_FRAME_D4:
		ret = CFpga::s_pFpga->CheckT1(m_chan, 0);
		break;
	case CIstateDti::SPAN_INPUT_FRAME_ESF:
		ret = CFpga::s_pFpga->CheckT1(m_chan, 1);
		break;
	}

	return ret;
}

int CInputSpan::ReadStatus()
{
	int stat;

	if ((stat = CFpga::s_pFpga->GetIpllStatus(m_chan - CHAN_BITS1)) < 0) {
		return UpdateInputStatus(stat);
	}
	stat = CheckFrame();
	if (stat != INPUT_STAT_OK) {
		return UpdateInputStatus(stat);
	}

	if (m_currentQL > CAllProcess::GetOscQL()) {
		return UpdateInputStatus(INPUT_STAT_POOR_SSM);
	}

	if (IS_ACTIVE) {
		if (CBT3OutBound::s_pBT3OutBound->IsInputOk(m_chan) != 1) {
			return UpdateInputStatus(INPUT_STAT_BT3_UNSETTLED);
		}
	}
	return UpdateInputStatus(INPUT_STAT_OK);
}


/*
don zhang: April 20, 2011:

GetPriority is called by CInputSelect::ChooseActiveInput() when performing 
an input selection.

GetPriority is multiplexed and the name may be a misnomer:
If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_SSM, GetPriority will 
return a combined priority number=predefined hardcoded priority (low byte) 
+ input pql (if available) or user specified pql COMM_RTE_KEY_INPUT_ASSUMED_QUALITY_LEVEL 
(high byte) if not available.
This combined priority number is to achieve the following: if two or more inputs 
have the same SSM (aka PQL, QL, quality level), use the predefined priority 
as a tie breaker.
The predefined priority is hardcoded as follows: 
1=gps, 5=INP1, 6=INP2 (see EFS for TimeProvider 5000 Release 2).

If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_PRIORITY, return the user 
specified priority (COMM_RTE_KEY_SPAN_INPUT_PRIORITY)

If m_refSelectMethod is CIstateDti::REFERENCE_SELECT_MANUAL, return the user 
specified priority (COMM_RTE_KEY_SPAN_INPUT_PRIORITY). The return value does not 
matter in this case.
*/
int CInputSpan::GetPriority(void)
{
	/*
	priority: lower priority is better (higher rank), legal range is 1~9 from user perspective, 
	but INPUT_COMBINED_PRIORITY_INVALID is also OK internally, which effectively disables this input;
	likewise, 0 is used internally as a hack to select an input.
	*/
	int combinedpriority = BUILD_UINT16(0, INPUT_COMBINED_PRIORITY_INVALID); 

	switch(CInputSelect::m_refSelectMethod)
	{
	case CIstateDti::REFERENCE_SELECT_SSM:
		//5~6=hardcoded INP predefined priority, 5: BITS1, 6: BITS2
		//(lowbyte, highbyte)
		combinedpriority = BUILD_UINT16((CHAN_BITS1 == m_chan)?5:6, m_currentQL);
		break;

	case CIstateDti::REFERENCE_SELECT_PRIORITY:
	case CIstateDti::REFERENCE_SELECT_MANUAL:
		combinedpriority = BUILD_UINT16(0, CInput::GetPriority());
		break;
	}

	return combinedpriority;
}



int CInputSpan::CfSsmBitChanInput(int ssmBit)
{
	if (m_ssmBit == ssmBit) return 0;
	m_ssmBit = ssmBit;
	RefreshInputSetup();
	return 1;
}

int CInputSpan::InputSetup(void)
{
	CBT3InBound::s_pBT3InBound->SetInputPriority(m_chan - CHAN_BITS1 + INPUT_CHAN_SPAN1, m_priority);
	if (m_enable == INPUT_CONFIG_DISABLE) {
		CFpga::s_pFpga->SetSpanInputType(m_index, CFpga::FPGA_SPAN_INPUT_DISABLE);
		return 1;
	}
	switch (m_frame) {
	case CIstateDti::SPAN_INPUT_FRAME_1544MHZ:
		break;
	case CIstateDti::SPAN_INPUT_FRAME_2MHZ:
		CFpga::s_pFpga->SetSpanInputType(m_index, CFpga::FPGA_SPAN_INPUT_2MHZ);
		return 1;
	case CIstateDti::SPAN_INPUT_FRAME_CCS:
	case CIstateDti::SPAN_INPUT_FRAME_CAS:
		CFpga::s_pFpga->SetSpanInputType(m_index, CFpga::FPGA_SPAN_INPUT_E1);
		CFpga::s_pFpga->SetSpanInputSsmBit(m_index, 
			m_ssmBit - CIstateDti::SPAN_INPUT_SSM_SA4 + CFpga::FPGA_SPAN_INPUT_SSM_SA4);
		return 1;
	case CIstateDti::SPAN_INPUT_FRAME_D4:
	case CIstateDti::SPAN_INPUT_FRAME_ESF:
		CFpga::s_pFpga->SetSpanInputType(m_index, CFpga::FPGA_SPAN_INPUT_T1);
		return 1;
	}
	CFpga::s_pFpga->SetSpanInputType(m_index, CFpga::FPGA_SPAN_INPUT_DISABLE);
	return 0;
}

int CInputSpan::CheckSsm(void)
{
	int ssm = m_assumedQL;
	int changed = 0;

	/*
	don zhang, July 8, 2011: PTR 66142
	always read ssm regardless of whether m_ssmRead is 0 or 1,
	use currentssmread to remeber the current ssm reading, and resotre 
	ssm to m_assumedQL when m_ssmRead is 0 so as not to break the 
	existing code logic.
	*/
	int currentssmread = m_assumedQL;
		
	if(m_inputDbgFlag & 0x01)
	{
		CAllProcess::g_pAllProcess->Print("m_chan=%d\r\n", m_chan);
		CAllProcess::g_pAllProcess->Print("m_enable=%d\r\n", m_enable);
		CAllProcess::g_pAllProcess->Print("ssm=%d\r\n", ssm);
		CAllProcess::g_pAllProcess->Print("m_ssmRead=%d\r\n", m_ssmRead);
		CAllProcess::g_pAllProcess->Print("m_frame=%d(ccs:2,cas:3,ESF:5)\r\n", m_frame);
		CAllProcess::g_pAllProcess->Print("m_crc4=%d\r\n", m_crc4);
	}

	if (m_enable == INPUT_CONFIG_DISABLE) 
	{
		ssm = 15; // 9

		currentssmread = ssm;
	} 
	else
	{
		ssm = 15; // 2/5/2010 zmiao: to be consitent for signal without SSM.
		switch (m_frame) 
		{
		case CIstateDti::SPAN_INPUT_FRAME_CCS:
		case CIstateDti::SPAN_INPUT_FRAME_CAS:
			if (m_crc4) 
			{
				ssm = CFpga::s_pFpga->GetSpanInputSsmQL(m_chan - CHAN_BITS1);
			}
			break;

		case CIstateDti::SPAN_INPUT_FRAME_ESF:
			ssm = CFpga::s_pFpga->GetSpanInputSsmQL(m_chan - CHAN_BITS1);
			break;

		//default:
			//	ssm = 15; // 11 // Impossible to read SSM
			//	break;
		}

		if ((ssm <= 0) || (ssm >= 10)) ssm = 15;
		ssm = m_ssmVote.Update(ssm);
		
		//currentssmread now holds the current ssm reading for future use:
		currentssmread = ssm;

		//hacking, resotre to assumedQL if m_ssmRead=0: don zhang, July 8, 2011:
		if (0 == m_ssmRead) ssm = m_assumedQL;
	}


	if(-1 == m_prevSSM)
	{
		CAllProcess::g_pAllProcess->Print("Input[%d]: -1=m_prevSSM\r\n", m_chan);
	}
		
	//existing logic:
	if(m_currentQL != ssm)
	{ 
		// Input SSM changed.
		TRACE("Input[%d] SSM changed %d-->%d\r\n", m_chan, m_currentQL, ssm);

		if(m_inputDbgFlag & 0x04)
		{
			CAllProcess::g_pAllProcess->Print("(m_ssmRead=%d\r\n", m_ssmRead);
			CAllProcess::g_pAllProcess->Print("1: Input[%d] m_currentQL changed %d-->%d\r\n", m_chan, m_currentQL, ssm);
	    }

		REPORT_EVENT(COMM_RTE_EVENT_INPUT_SSM_CHANGE, 1, m_chan);

		m_currentQL = ssm;
		changed = 1;
	}


	if(m_prevSSM != currentssmread)
	{
		//new logic, added by don zhang, July 8, 2011:
		if(m_inputDbgFlag & 0x08)
		{
			CAllProcess::g_pAllProcess->Print("ssm=%d\r\n", ssm);
			CAllProcess::g_pAllProcess->Print("(m_enable=%d\r\n", m_enable);
			CAllProcess::g_pAllProcess->Print("2: Input[%d] SSM changed %d-->%d\r\n", 
				m_chan, m_prevSSM, currentssmread);
		}
		CIstateDti::PostCurrentQualityLevel(m_chan - CHAN_BITS1, currentssmread);
				
		m_prevSSM = currentssmread;
	}
		
	return changed;
}

unsigned int CInputSpan::InputTick(void)
{
	unsigned int ret;
	ret = CInput::InputTick();
	ret |= CheckSsm();

	if (CInputSelect::m_refSelectMethod == CIstateDti::REFERENCE_SELECT_SSM) {
		CBT3InBound::s_pBT3InBound->SetInputQualityLevel(m_chan - CHAN_BITS1 + INPUT_CHAN_SPAN1, m_currentQL);
	} else {
		CBT3InBound::s_pBT3InBound->SetInputQualityLevel(m_chan - CHAN_BITS1 + INPUT_CHAN_SPAN1, 1);
	}

	return ret;
}

int CInputSpan::GetCurrentQL(void)
{
	return m_currentQL;
}