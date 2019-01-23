// BT3Debug.cpp: implementation of the CBT3Debug class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: BT3Debug.cpp
 * Author: Zheng Miao
 * All rights reserved.
 * RCS: $Header: BT3Debug.cpp 1.3 2008/05/12 14:05:28PDT Zheng Miao (zmiao) Exp  $
 *	$Revision: 1.3 $
 */

#include "CodeOpt.h"
#include "DataType.h"
#include "BT3Debug.h"       
#include "stdio.h"
#include "string.h"
#include "comm.h"
#include "BT3Support.h"
#include "BestCal.h"
#include "AllProcess.h"
#include "InputGps.h"
#include "BT3Api.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBT3Debug::CBT3Debug()
{
	m_port = NULL;
}

CBT3Debug::~CBT3Debug()
{

}

CBT3Debug *CBT3Debug::s_pBT3Debug = NULL;

void CBT3Debug::Create()
{
	if (s_pBT3Debug == NULL) {
		s_pBT3Debug = new CBT3Debug();
	}
}

void CBT3Debug::Print(const char *str)
{
	if (m_port != NULL) 
		m_port->Send(str, -1);
}

void CBT3Debug::PrintBT3Help()
{
	Print("Invalid option\r\nSyntax: <option>\r\n"
		"1   \t: Start factory calibration\t"
		"2   \t: Stop factory calibration\r\n"
		"3   \t: Print calibration data\r\n"
		"10 <src>: Set source for BT3. \r\n\t<src>=0: SPA\t1: SPB\t2: PRS\t3: GPS\t4: GPS without comm\r\n"
		"11 <on/off>: Set BT3 ensemble on(1)/off(0)\r\n"
		"12 <efcmode>: Set efc test mode.\r\n\t<emode>=100 freeze efc update:\t emode=99 no dither\t <emode>=0 normal\r\n");
	Print("51 <ecw>: Put Ensemble Control Word\r\n"
		"52 \t: Get Ensemble Control Word\r\n"
		"53 <pcw>: Put Provision Control Word\r\n"
		"54 \t: Get Provision Control Word\r\n"
		"55 <rcw>: Put Reset Control Word\r\n"
		"56 \t: Get Reset Control Word\r\n"
		"57 \t: Get BT3 mode and gear (OutBound)\r\n"
		"58 \t: Get BT3 inbound info\t"
		"58 1\t: Print GPS fifo\t"
		);
	Print("58 2\t: Print MPLL fifo\r\n\t58 3: Print SPANA fifo\t58 4: Print SPANB fifo\r\n"
		"59 \t: Reset RGP FIFO\r\n"
		"60 <mode>: Force BT3 mode\t"
		"61 <gear>: Force BT3 gear\r\n"
		"62 <reset>: reset BT3 engine. <reset> 0=FACTORY 1=COLD 2=WARM 3=HOT\r\n" 
		"63 \t: reinitialize calibration to zero offset\r\n"
		"64 <pctrl>: Force phase control: -1: Auto  0: Freq  1: Normal  2: aggressive\r\n"
		"65 \t: reinitialize calibration to subtending frequency\r\n"

		);
}


void CBT3Debug::PrintBT3Status(unsigned long *param, Ccomm *pComm)
{
	char buff[100];
	int done = 1;
	m_port = pComm;
	
	if (param[0] == 0) {
		PrintBT3Help();
		return;
	}
	
	if (param[0] == 2) {
		switch (param[1]) {
		case 10L:
			if ((param[2] >= 0) && (param[2] <= 4)) {
				set_trace_source((unsigned int)param[2]);
				if (param[2] == 4) {
					// Ignore info from PPC 
					CInputGps::s_pInputGps->SetIgnoreImc(1);
				} else if (param[2] == 3) {
					// Use info from PPC 
					CInputGps::s_pInputGps->SetIgnoreImc(0);
				}
				return;
			}
			break;
		case 11L:
			set_ensemble_mode((unsigned int)param[2]);
			return; 
			break; 
		case 12L:
			set_efc_mode((unsigned int)param[2]);
			return;  
		case 60L:
			CBT3OutBound::s_pBT3OutBound->ForceBT3Mode(param[2]);
			return;
		case 61L:
			CBT3OutBound::s_pBT3OutBound->ForceBT3Gear(param[2]);
			return;
		}

		if ((param[1] == 58) && (param[2] == 1)) {
			PrintSpanFifo(pComm, &GPS_Fifo);
			return;
		}

		if ((param[1] == 58) && (param[2] == 2)) {
			PrintMpll(pComm);
			return;
		}

		if ((param[1] == 58) && (param[2] == 3)) {
			PrintSpanFifo(pComm, &SPANA_Fifo);
			return;
		}

		if ((param[1] == 58) && (param[2] == 4)) {
			PrintSpanFifo(pComm, &SPANB_Fifo);
			return;
		}

		if (param[1] == 64) {
			CBT3Api::s_pBT3Api->ForcePhaseControl((int)param[2]);
			return;
		}
		if (param[1] == 62) {
			switch (param[2]) {
			case 0:
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_FACTORY_COLD_START);
				pComm->Send("Reset BT3 factory\r\n");
				return;
			case 1:
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_NORMAL_COLD);
				pComm->Send("Reset BT3 cold\r\n");
				return;
			case 2:
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_WARM_START);
				pComm->Send("Reset BT3 warm\r\n");
				return;
			case 3:
				CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_HOT_START);
				pComm->Send("Reset BT3 hot\r\n");
				return;
			}
		}
	}
	switch (param[1]) {
	case 1: // Start factory calibration
		{
			CBestCal *pCal;
			pCal = CBestCal::s_pBestCal; //g_pAll->GetBestCal();
			pCal->StartFactoryCal();
			Print("Factory Calibration started\r\n");
		}
		break;
	case 2: // end factory calibration
		{
			CBestCal *pCal;
			pCal = CBestCal::s_pBestCal; //g_pAll->GetBestCal();
			pCal->StopFactoryCal();
			Print("Factory Calibration stopped\r\n");
		}
		break;
	case 3: // print cal data
		{
			CBestCal *pCal;
			pCal = CBestCal::s_pBestCal; //g_pAll->GetBestCal();
			pCal->PrintBestCal(pComm);
		}
		break;
	case 57:
		{
			CBT3OutBound::s_pBT3OutBound->PrintOutBoundStatus(pComm);
			CBT3Api::s_pBT3Api->PrintBT3Api(pComm);
		}
		break;
	case 58:
		{
			CBT3InBound::s_pBT3InBound->PrintInBoundStatus(pComm);
			CBT3Api::s_pBT3Api->PrintBT3Api(pComm);
		}
		break;
	case 59:
		{
			CBT3InBound::s_pBT3InBound->PrintInBoundStatus(pComm, param);
		}
		break; 
	case 51: // Set ECW
		if (param[0] != 2) {
			done = 0;
		} else {
			PutBt3EnsembleControlWord((unsigned short)(param[2]));
		}
		break;
	case 52: // Get ECW
		{
			unsigned short ecw;
			unsigned short override;
			GetBt3EnsembleControlWord(&ecw, &override);
			sprintf(buff, "ECW: 0x%04X  Override: 0x%04X\r\n", ecw, override);
			Print(buff);
		}
		break;
	case 53: // Put PCW
		if (param[0] != 2) {
			done = 0;
		} else {
			PutBt3ProvisionControlWord((unsigned short)(param[2]));
		}
		break;
	case 54: // Get PCW
		{
			unsigned short pcw;
			GetBt3ProvisionControlWord(&pcw);
			sprintf(buff, "PCW: 0x%04X\r\n", pcw);
			Print(buff);
		}
		break;
	case 55: // put RCW
		if (param[0] != 2) {
			done = 0;
		} else {
			PutBt3ResetControlWord((unsigned short)(param[2]));
		}
		break;
	case 56: // Get RCW
		{
			unsigned short rcw;
			unsigned short override;
			GetBt3ResetControlWord(&rcw, &override);
			sprintf(buff, "RCW: 0x%04X  Override: 0x%04X\r\n", rcw, override);
			Print(buff);
		}
		break;  
	case 63: // reset cal data
		{ 
			Cal_Data_Reset();

			// zmiao: Store the cal data now
			Cal_Data.cal_state = CAL_Stable;
			CBestCal::s_pBestCal->SaveChange();
		}
		break;
	case 65: // reset cal data
		{ 
			Cal_Data_Subtend_Reset();

			// zmiao: Store the cal data now
			Cal_Data.cal_state = CAL_Stable;
			CBestCal::s_pBestCal->SaveChange();
		}
		break;		
	default:
		done = 0;
		break;
	}
	if (!done) {
		PrintBT3Help();
	}
}

EXTERN void DebugPrint(const char *str, int group)
{
	CAllProcess::g_pAllProcess->MaskedPrint((uint32)1 << group, str);
}

void CBT3Debug::PrintSpanFifo(Ccomm *pComm, struct Span_Meas_Data *pData)
{
	char buff[200];
	int cnt;
	sprintf(buff, "Memory Locate @0x%lX\r\n", pData);
	pComm->Send(buff);

	cnt = 0;
	do {
		sprintf(buff, "0x%X ", pData->accum[cnt]);
		pComm->Send(buff);
		if ((cnt & 15) == 15) {
			pComm->Send("\r\n");
		}
	} while (++cnt < MBSIZE);
	sprintf(buff, "Nwrite=%d Nread=%d Ndata=%d\r\n"
		"SPAN_Present=0x%X\r\n"
		, pData->Nwrite, pData->Nread, pData->Ndata
		, pData->SPAN_Present
		);
	pComm->Send(buff);

}

void CBT3Debug::PrintMpll(Ccomm *pComm)
{
	char buff[200];
	int i;

	for (i = 0; i < MBSIZE; i++) {
		sprintf(buff, "0x%lX ", mpll.sum[i]);
		pComm->Send(buff);
		if ((i & 7) == 7){
			pComm->Send("\r\n");
		}
	}

	sprintf(buff, "Marker: %d  Write: %d  Read: %d Ndata: %d\r\n"
		"status: 0x%lX"
		, mpll.PPS_Nwrite_marker, mpll.Nwrite, mpll.Nread
		, mpll.Ndata, mpll.status);
	pComm->Send(buff);
	
}
