// InputSelect.cpp: implementation of the CInputSelect class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputSelect.cpp
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputSelect.cpp 1.23 2011/06/10 13:24:08PDT Jining Yang (jyang) Exp  $
 *		$Revision: 1.23 $
 */             

#include "InputSelect.h"
#include "stdio.h"
#include "Input.h"
#include "AllProcess.h"
#include "CardProcess.h"
#include "CardStatus.h"
#include "IstateProcess.h"
#include "Fpga.h"
#include "InputPath.h"
#include "InputTod.h"
#include "IstateDti.h"
#include "BT3Support.h"
#include "CardLoop.h"

#define TRACE TRACE_CARD

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputSelect *CInputSelect::s_pInputSelect = NULL;
int CInputSelect::m_todSource = -1;
int CInputSelect::m_refSelectMethod = -1;

CInputSelect::CInputSelect()
{
	m_inputSelectDebug = 0;
	m_activeInputSelected = -1;
	m_twinSelect = -2;
	if (s_pInputSelect == NULL) {
		s_pInputSelect = this;
	}
	m_manualInputChan = -1;
	m_systemSsm = 15;
	m_ssmBlockCntUnderNormal = 0;
}

CInputSelect::~CInputSelect()
{

}

// 1: Changed
int CInputSelect::ChooseActiveInput()
{
	int select = m_activeInputSelected;
	unsigned int priority = 15;
	int chan;
	CInput *pInput;

	/*added on June 9, 2011, per jingyang's suggestion: 
	only used combined priority (=user specified pql + predefined pql) 
	to select the input when need to switch the input.
	*/
	int combined_select = m_activeInputSelected;
	unsigned int combined_priority = 0xffff;

	if(m_inputSelectDebug) 
	{
		//debug logging:
		CAllProcess::g_pAllProcess->Print("ChooseActiveInput:\r\n");
		CAllProcess::g_pAllProcess->Print("select=%d\r\n", select);

		CAllProcess::g_pAllProcess->Print("m_refSelectMethod=%d\r\n", m_refSelectMethod);
		switch(m_refSelectMethod)
		{
		case CIstateDti::REFERENCE_SELECT_MANUAL:
			CAllProcess::g_pAllProcess->Print("REFERENCE_SELECT_MANUAL\r\n");
			break;

		case CIstateDti::REFERENCE_SELECT_PRIORITY:
			CAllProcess::g_pAllProcess->Print("REFERENCE_SELECT_PRIORITY\r\n");
			break;

		case CIstateDti::REFERENCE_SELECT_SSM:
			CAllProcess::g_pAllProcess->Print("REFERENCE_SELECT_SSM\r\n");
			break;
		}

		CAllProcess::g_pAllProcess->Print("m_todSource=%d\r\n", m_todSource);
		switch(m_todSource)
		{
		case CIstateDti::TOD_SOURCE_GPS:
			CAllProcess::g_pAllProcess->Print("TOD_SOURCE_GPS\r\n");
			break;

		case CIstateDti::TOD_SOURCE_USER:
			CAllProcess::g_pAllProcess->Print("TOD_SOURCE_USER\r\n");
			break;
		}

	}

	if ((select >= 0) && (select < NUM_INPUT)) 
	{
		pInput = CInput::s_pInput[select];
		priority = HI_UINT16(pInput->GetPriority());

		combined_select = select;
	    combined_priority = pInput->GetPriority();
		
		if(m_inputSelectDebug) 
		{
			//debug logging:
			CAllProcess::g_pAllProcess->Print("select=%d (%s)\r\n", select, CInput::GetInputChanName(select));

			CAllProcess::g_pAllProcess->Print("combined_select=%d, combined_priority=%d\r\n", combined_select, combined_priority);
			
			CAllProcess::g_pAllProcess->Print("GetPriority()=0x%x, low=%d, high=%d\r\n", 
				pInput->GetPriority(), LO_UINT16(pInput->GetPriority()), HI_UINT16(pInput->GetPriority()));

			CAllProcess::g_pAllProcess->Print("GetQualify()=%d\r\n", pInput->GetQualify());
			CAllProcess::g_pAllProcess->Print("IsToBeSelected()=%d\r\n", pInput->IsToBeSelected());
		}

		if ((pInput->GetQualify() != CInput::INPUT_QUALIFY_OK) 
			|| (pInput->IsToBeSelected() <= 0)
			)
		{
				select = -1;
				combined_select = -1;

				if(m_inputSelectDebug) 
				{
					CAllProcess::g_pAllProcess->Print("set select to -1:\r\n");
				}
		}
	}

	for (chan = 0; chan < NUM_INPUT; chan++) 
	{
		if (select == chan) 
		{
			continue;
		}

		pInput = CInput::s_pInput[chan];

		if(m_inputSelectDebug) 
		{
			CAllProcess::g_pAllProcess->Print("select=%d, priority=%d\r\n", select, priority);
			CAllProcess::g_pAllProcess->Print("combined_select=%d, combined_priority=%d\r\n", combined_select, combined_priority);
						
			CAllProcess::g_pAllProcess->Print("------chan=%d (%s)-------:\r\n", chan, CInput::GetInputChanName(chan));

			CAllProcess::g_pAllProcess->Print("GetPriority()=0x%x, low=%d, high=%d\r\n", 
				pInput->GetPriority(), LO_UINT16(pInput->GetPriority()), HI_UINT16(pInput->GetPriority()));
									
			CAllProcess::g_pAllProcess->Print("GetQualify()=%d\r\n", pInput->GetQualify());

			CAllProcess::g_pAllProcess->Print("IsToBeSelected()=%d\r\n", pInput->IsToBeSelected());
		}


		if ((pInput->GetQualify() == CInput::INPUT_QUALIFY_OK)
			&& (pInput->IsToBeSelected() > 0)
			)
		{
			if (select < 0) 
			{
				select = chan;
				priority = HI_UINT16(pInput->GetPriority());

				combined_select = chan;
	            combined_priority = pInput->GetPriority();
				continue;
			}

			//lower priority has higher rank, always select the input with lowest priority:
			if (combined_priority > pInput->GetPriority()) 
			{
				combined_select = chan;
				combined_priority = pInput->GetPriority();
			}

			//lower priority has higher rank, always select the input with lowest priority:
			if (priority > HI_UINT16(pInput->GetPriority())) 
			{
				select = chan;
				priority = HI_UINT16(pInput->GetPriority());
				continue;
			}
		}
	}

	if(m_inputSelectDebug) 
	{
		CAllProcess::g_pAllProcess->Print("select=%d, priority=%d\r\n", select, priority);
	    CAllProcess::g_pAllProcess->Print("combined_select=%d, combined_priority=%d\r\n", combined_select, combined_priority);
	}

	// See if it's in maunal mode
	if (m_refSelectMethod == CIstateDti::REFERENCE_SELECT_MANUAL) {
		CInput *pManualInput = NULL;
		if ((m_manualInputChan >= 0) && (m_manualInputChan < NUM_INPUT)) {
			pManualInput = CInput::s_pInput[m_manualInputChan];
			if ((pManualInput->IsToBeSelected() > 0) 
				&& (pManualInput->GetQualify() == CInput::INPUT_QUALIFY_OK)) {
				select = m_manualInputChan;
			}
		}
	}

	if (select == m_activeInputSelected) {
		// TRACE("Keep Active Input: %d\r\n", m_activeInput);
		return 0;
	}

	if(CIstateDti::REFERENCE_SELECT_SSM == m_refSelectMethod)
	{
		if((combined_select >= 0) && (combined_select < NUM_INPUT))
		{
			if(m_inputSelectDebug) 
			{
				CAllProcess::g_pAllProcess->Print("combined_select=%d\r\n", combined_select);
			}
			select = combined_select;
		}
	}	

	if (select == m_activeInputSelected) {
		// TRACE("Keep Active Input: %d\r\n", m_activeInput);
		return 0;
	}


	// Made new selection
	SelectActiveInput(select);
	TRACE("New Active Input %d\r\n", select); // This line has to be here to make it work. It's a mistery.

    return 1;
}

void CInputSelect::SelectActiveInput(int select)
{
	int8 done = 0;

	if ((m_activeInputSelected >= 0) && (m_activeInputSelected < NUM_INPUT)) {
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_ACTIVE, 0, m_activeInputSelected);
	}

	if ((select >= 0) && (select < NUM_INPUT)) {
		REPORT_EVENT(COMM_RTE_EVENT_INPUT_ACTIVE, 1, select);
	}

	m_activeInputSelected = select;
	
	if (IS_ACTIVE) {
		CBT3InBound::s_pBT3InBound->SetActiveInput(select);
	} else {
		// 3/5/2009 zmiao: Do the same thing for standby card. That's what George Z wants.
		CBT3InBound::s_pBT3InBound->SetActiveInput(select);
	}

	UpdateSystemSsm();

#if 0 
	// set to MPLL
	switch (select) {
	case CHAN_DTI1:
		CFpga::s_pFpga->SetMpll(CFpga::MPLL_DTI_ONE);
		done = 1;
		break;
	case CHAN_DTI2:
		CFpga::s_pFpga->SetMpll(CFpga::MPLL_DTI_TWO);
		done = 1;
		break;
	case CHAN_GPS:
		if (m_todSource == CIstateDti::TOD_SOURCE_GPS) {
			CFpga::s_pFpga->SetMpll(CFpga::MPLL_GPS);
			done = 1;
			break; // 9/19/2006 Bob H said to keep GPS mode the way it is. That is to set MPLL to GPS whether GPS is qualified or not.
			// But for Subtending mode, if no input is qualified, set MPLL to holdover.
		}
		// break; // Fall thru
	default:
		break;
	}

	// Added on 9/19/2006
	if (!done) {
		if (m_todSource == CIstateDti::TOD_SOURCE_DTI) { // No valid input is found.
			CFpga::s_pFpga->SetMpll(CFpga::MPLL_HOLDOVER);
			done = 1;
		} else if (m_todSource == CIstateDti::TOD_SOURCE_GPS) {
			CFpga::s_pFpga->SetMpll(CFpga::MPLL_GPS);
			done = 1;
		}

	}
#endif

	// Inform
	CInputPath::SetActiveInput(select);
	CInputTod::SetActiveInput(select);
	CInput::SetActiveInput(select);
	CBT3InBound::s_pBT3InBound->SetActiveInput(select);//GPZ June 2010 add BT3 engine to informed classes
	switch (select) {
	case CHAN_BITS1:
		CIstateDti::PostActiveFrequencyInput(0);
		break;
	case CHAN_BITS2:
		CIstateDti::PostActiveFrequencyInput(1);
		break;
	default:
		CIstateDti::PostActiveFrequencyInput(-1);
		break;
	}

}

void CInputSelect::InputSelectPrint(Ccomm *pComm)
{
	char buff[256];

	sprintf(buff, "Active Input: %d. Twin Select: %d\r\n", 
		m_activeInputSelected, m_twinSelect);
	pComm->Send(buff);

	sprintf(buff, "OSC QL: %d\r\n", CAllProcess::GetOscQL());
	pComm->Send(buff);

	sprintf(buff, "SSM block cnt: %d\r\n", m_ssmBlockCntUnderNormal);
	pComm->Send(buff);
	
	sprintf(buff, "TOD Source: %d\r\n", m_todSource);
	pComm->Send(buff);
}

int CInputSelect::MakeSelect()
{
	int ret;
	int select;

	UpdateSystemSsm(); // In case it's only OSC level changed. 

	// Active RTE 
	if (IS_ACTIVE) {
		ret = ChooseActiveInput();
		SET_MY_ISTATE(COMM_RTE_KEY_ACTIVE_INPUT, 0, 0, m_activeInputSelected);
		EvalReferenceLed(0);
		return ret;
	}

	EvalReferenceLed(1);

	// Failed or offline
	if (!IS_STANDBY) {
		if (m_activeInputSelected != -1) {
			SelectActiveInput(-1);
			return 1;
		}
		return 0;
	}

	// Standby case
	select = m_twinSelect;
	switch (select) {
	default: // 9/20/2006: Bob H want holdover for Subtending mode. 
		break; // 3/6/2009 zmiao: Not true for TP5K RTE. 
	case CHAN_DTI1: // Subtending mode, follow active RTE
	case CHAN_DTI2:
		if (m_activeInputSelected != select) {
			TRACE("Select DTI input\r\n");
			SelectActiveInput(select);
			return 1;
		}
		return 0;
	}

	if ((select < 0) || (select >= NUM_INPUT)) {
		// TRACE("No Valid input from Active RTE\r\n");
		return ChooseActiveInput();
	}
	if (CInput::s_pInput[select]->GetQualify() == CInput::INPUT_QUALIFY_OK) {
		if (m_activeInputSelected != select) {
			TRACE("Follow Active Input change to Chan %d\r\n", select);
			SelectActiveInput(select);
			return 1;
		}
		return 0;
	} else {
		// TRACE("Standby Pick input by itself\r\n");
		return ChooseActiveInput();
	}

}

void CInputSelect::SetTwinSelect(int select)
{
	if (m_twinSelect == select) return;
	m_twinSelect = select;
	CInput::TouchInput();

}

void CInputSelect::RefreshSelect()
{
	CInput::TouchInput();
}

int CInputSelect::IsSelectLocked()
{
	if ((m_activeInputSelected < 0) || (m_activeInputSelected >= NUM_INPUT)) {
		return 0;
	}
	return CInput::s_pInput[m_activeInputSelected]->IsLocked();
}

void CInputSelect::EvalReferenceLed(int clear)
{
	int chan;
	CInput *pInput;
	int qCnt = 0;
	int enableCnt = 0;
	int q;

	if (clear) {
		return;
	}

	for (chan = 0; chan < NUM_INPUT; chan++) {
		pInput = CInput::s_pInput[chan];
		q = pInput->GetQualify();
		switch (q) {
		case CInput::INPUT_QUALIFY_OK:
			qCnt++;
			enableCnt++;
			break;
		case CInput::INPUT_QUALIFY_DISQ:
			enableCnt++;
			break;
		case CInput::INPUT_QUALIFY_IGNORE:
			break;
		}
		
	}

	if (enableCnt == 0) {
		return;		
	}
	if (qCnt == enableCnt) {
		return;			
	}


}

void CInputSelect::CfTodSource(int tod)
{
	if (m_todSource != tod) {
		m_todSource = tod;
		RefreshSelect();
	}
}

void CInputSelect::CfRefSelectMethod(int method)
{
	if (m_refSelectMethod != method) {
		m_refSelectMethod = method;
		RefreshSelect();
	}
}

void CInputSelect::CfManualInput(int chan)
{
	if (m_manualInputChan != chan) {
		m_manualInputChan = chan;
		RefreshSelect();
	}
}

void CInputSelect::InputSelectTick(void)
{
	if ((m_ssmBlockCntUnderNormal > 0) && (m_ssmBlockCntUnderNormal <= MAX_SSM_BLOCK_PERIOD_NORMAL)) {
	    m_ssmBlockCntUnderNormal++;
	}
}

void CInputSelect::UpdateSystemSsm(void)
{
	// 5/11/2009 zmiao: When losing input, it could be no input and still tracking for a brief moment.
	int level = m_systemSsm /* 10 */;
	int min = CAllProcess::GetOscQL();
	int cardState;
	int cardLoop;

	cardState = CCardStatus::GetCardStatus();
	cardLoop = CCardLoop::s_pCardLoop->GetCardLoop();
	if ((m_activeInputSelected >= 0) && (m_activeInputSelected < NUM_INPUT)) {
		level = CInput::s_pInput[m_activeInputSelected]->GetCurrentQL();
	} 

	switch (cardLoop) {
	case CCardLoop::CARD_FREQ_BRIDGE:
		//level = 2;
		// 2/5/2010 zmiao: For bridge keep previous SSM
		level = m_systemSsm;
		break;
	case CCardLoop::CARD_FREQ_FAST_TRACK: // zmiao 12/4/2010: According to new spec. Make fast track use OSC level.
	case CCardLoop::CARD_FREQ_FREERUN:
	case CCardLoop::CARD_FREQ_HOLDOVER:
	case CCardLoop::CARD_FREQ_EXTENDED_HOLDOVER:
		level = min;
		break;
	case CCardLoop::CARD_FREQ_NORMAL_TRACK:
		if (level > min) {
			// zmiao 12/21/2010: Still in tracking while input is already gone. Keep it as previous one.
			if (m_ssmBlockCntUnderNormal < MAX_SSM_BLOCK_PERIOD_NORMAL) {
				level = m_systemSsm;
				if (m_ssmBlockCntUnderNormal <= 0) {
					m_ssmBlockCntUnderNormal = 1; // Start the timer
				}
			}
		} else {
			m_ssmBlockCntUnderNormal = 0;
		}
		break;
	}
	if (level > min) {
		level = min;
	}

	switch (cardState) {
	case CCardStatus::CARD_HOT_ACTIVE:
	case CCardStatus::CARD_HOT_STANDBY:
		break;
	case CCardStatus::CARD_COLD_ACTIVE:
	case CCardStatus::CARD_COLD_STANDBY:
	case CCardStatus::CARD_WARMUP:
	case CCardStatus::CARD_FAIL:
	case CCardStatus::CARD_OFFLINE:
		level = 9;
		break;
	default:
		level = min;
		break;
	}

	if (m_systemSsm != level) {
		// Set to output
		CFpga::s_pFpga->SetOutputQL(level);

		// Set to Istate
		SET_MY_ISTATE(COMM_RTE_KEY_SYSTEM_SSM, 0, 0, level);

		// Report event
		REPORT_EVENT(COMM_RTE_EVENT_SYSTEM_SSM_CHANGE, 1);

		TRACE("System SSM changed from %d to %d\r\n", m_systemSsm, level);
		m_systemSsm = level;
		m_ssmBlockCntUnderNormal = 0;
	}
	
}

void CInputSelect::InitInputSelect(void)
{
	UpdateSystemSsm();
}


void CInputSelect::InputSetDebug(int debugflag)
{
	m_inputSelectDebug = debugflag;
}
