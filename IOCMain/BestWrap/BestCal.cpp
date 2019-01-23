// BestCal.cpp: implementation of the CBestCal class.
//
//////////////////////////////////////////////////////////////////////
/*
 * 
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: BestCal.cpp 1.1 2007/12/06 11:41:05PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */

#include "BestCal.h"
#include "bt3_dat.h"
#include "string.h"
#include "BT3Support.h"
#include "MinuteProcess.h"
#include "AllProcess.h"
#include "stdio.h"
#include "comm.h"
#include "BT3Api.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBestCal *CBestCal::s_pBestCal;

CBestCal::CBestCal(CNVRAM *pNVRAM)
{
	s_pBestCal = this;
	m_pNVRAM = pNVRAM;
	m_blockId = CNVRAM::BLOCK_ID_BESTIME_CAL + BESTIME_CAL_VERSION;
	m_size = sizeof(Cal_Data);

	// Init RAM copy
	memset(&Cal_Data, 0, m_size);
}

CBestCal::~CBestCal()
{

}

// Read init data out of NVRAM
void CBestCal::Init()
{
	int ret;

	ret = m_pNVRAM->RtrvBlock(m_blockId, m_size, (uint8 *)&Cal_Data);
	if (ret != 1) {
		// Cannot retrieve from NVRAM
		SaveChange();
	}
}

void CBestCal::SaveChange()
{
	m_pNVRAM->StoreBlock(m_blockId, m_size, (uint8 *)&Cal_Data);
}

void CBestCal::StartFactoryCal()
{
	Cal_Data.cal_state = 0; // CAL_None
	CBT3InBound::s_pBT3InBound->ResetBT3(CBT3InBound::RESET_FACTORY_COLD_START);
}

void CBestCal::StopFactoryCal()
{
	CBT3Api::s_pBT3Api->StoreCalData();
}

void CBestCal::PrintBestCal(Ccomm *pComm)
{
	char buff[200];

	sprintf(buff, "\tcal_state: %d\r\n\tdate: %ld\r\n\tosc_sn: %d\r\n\tinit_age_type: %d\r\n",
		Cal_Data.cal_state, Cal_Data.date, Cal_Data.osc_sn, Cal_Data.init_age_type);
	pComm->Send(buff);
	
	sprintf(buff, 
		"\tLO_Freq_Cal: %e\r\n\tLO_Drift_Cal: %e\r\n"
		"\tLO_Freq_Cal_Factory: %e\r\n\tLO_Drift_Cal_Factory: %e\r\n"
		"\tATC_TC_Cal: %e\r\n",
		Cal_Data.LO_Freq_Cal, Cal_Data.LO_Drift_Cal, 
		Cal_Data.LO_Freq_Cal_Factory, Cal_Data.LO_Drift_Cal_Factory,
		Cal_Data.ATC_TC_Cal);
	pComm->Send(buff);
	
	sprintf(buff, "\tSlew_Comp: %d\r\n"
		"\tefc_slope: %e\r\n"
		"\tchecksum: %X\r\n"
		"\tupdate_cnt: %d\r\n",
		Cal_Data.Slew_Comp, Cal_Data.efc_slope, Cal_Data.checksum, Cal_Data.update_cnt);
	pComm->Send(buff);
}

void CBestCal::RegisterNvram()
{
	m_pNVRAM->SetupBlock(m_blockId, sizeof(struct Calibration_Data), 
		CNVRAM::MAGIC_BESTIME_CAL);

}
