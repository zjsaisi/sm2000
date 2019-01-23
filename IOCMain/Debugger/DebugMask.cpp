// DebugMask.cpp: implementation of the CDebugMask class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * RCS: $Header: DebugMask.cpp 1.2 2008/05/15 15:54:11PDT Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#include "DebugMask.h"
#include "Factory.h"
#include "stdio.h"
#include "IstateProcess.h"
#include "EventProcess.h"
#include "PTPDnldProcess.h"
#include "HouseProcess.h"
#include "CardProcess.h"
#include "OutputProcess.h"
#include "SecondProcess.h"
#include "MinuteProcess.h"
#include "RubProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define READ_MASK CFactory::s_pFactory->GetDebugOption()
#define WRITE_MASK CFactory::s_pFactory->SetDebugOption

CDebugMask::CDebugMask()
{
	m_mask = 0xffffffffUL;
	memset(m_count, 0, sizeof(m_count));
}

CDebugMask::~CDebugMask()
{

}

void CDebugMask::Init()
{
	m_mask = READ_MASK;
	CRubProcess::s_pRubProcess->SetDebugGroup(RUBIDIUM_GROUP);
	CIstateProcess::s_pIstateProcess->SetDebugGroup(ISTATE_GROUP);
	CEventProcess::s_pEventProcess->SetDebugGroup(EVENT_GROUP);
	CPTPDnldProcess::s_pPTPDnldProcess->SetDebugGroup(PTP_DOWNLOAD_GROUP);
	CHouseProcess::s_pHouseProcess->SetDebugGroup(HOUSE_GROUP);
	CCardProcess::s_pCardProcess->SetDebugGroup(CARD_GROUP);
	COutputProcess::s_pOutputProcess->SetDebugGroup(OUTPUT_GROUP);
	CSecondProcess::s_pSecondProcess->SetDebugGroup(SECOND_GROUP);
	CMinuteProcess::s_pMinuteProcess->SetDebugGroup(MINUTE_GROUP);
}

int CDebugMask::CheckMask(uint32 mask)
{
	if (m_mask & mask) {
		CountRequest(mask);
		return 1;
	}

	return 0;
}

void CDebugMask::DebugHack(Ccomm *pComm, unsigned long *param)
{
	char buff[100];
	if (*param == 0) {
		char const *ptr;
		ptr = "Debug mask setup\r\nSyntax 1: DM <group> <enable>\r\n\
\t<group>: Debug group. \r\n\t\tFor BT3 code, it is 0..15.\r\n\t\tFor the rest: 16 .. 30.\r\n\
\t<enable>: Enable or Disable\r\n\t\t0 = disable the debug print of <group>.\r\n\t\t1 = enable debug print from <group>\r\n";
		pComm->Send(ptr);
		ptr = "\r\nSyntax 2: DM <bitmap> <enable>\r\n\
\t<bitmap>: Bitmap of debug group.\r\n\
\t<enable>: 100 = enable, 200 = disable\r\n";
		pComm->Send(ptr);
		ptr = "\r\nSyntax 3: DM <option>\r\n"
			"\t100: Enable all debug group\r\n\t200: Disable all debug group\r\n"
			"\t300: List group assignment\r\n"
			"\t400: List debug counting\r\n";
		pComm->Send(ptr);
		SetDebugOption(0xffffffff, 0L, pComm);
		return;
	}
	if (*param == 2) {
		switch (param[2]) {
		case 1:
			// Enable a group
			sprintf(buff, "Group %d enabled\r\n", (int)param[1]);
			pComm->Send(buff);
			SetDebugOption(0xffffffff, (uint32)1 << param[1], pComm);
			return;
		case 0:
			// Disable a group
			sprintf(buff, "Group %d disabled\r\n", (int)param[1]);
			pComm->Send(buff);
			SetDebugOption(~((uint32)1 << param[1]), 0L, pComm);
			return;
		case 100:
			// Enable a bitmap
			SetDebugOption(0xffffffff, param[1], pComm);
			return;
		case 200:
			// Disable a bitmap
			SetDebugOption(~param[1], 0, pComm);
			return;
		}
	}
	if (*param == 1) {
		switch (param[1]) {
		case 100: 
			// Enable all
			pComm->Send("Enable all print group\r\n");
			SetDebugOption(0xffffffff, 0xffffffff);
			return;
		case 200:
			// Disable all
			pComm->Send("Disable all print group\r\n");
			SetDebugOption(0L, 0L);
			return;
		case 300: {
			const char *ptr;
			ptr = "Debug group assignment: \r\n\t0 .. 15 for BT3\r\n"
				"\t00: SSU Holdover\r\n\t01: Not specified\r\n"
				"\t02: SSU Holdover Filter Loop\r\n\t03: PRR Holdover\r\n"
				"\t04: SPA Loop Filter\r\n\t05: BesTime Internal Alarm Report\r\n"
				"\t06: BesTime Engine Report\r\n"
				"\t07: PRR Loop Filter\r\n\t08: PRR Monthly Holdover Report\r\n"
				"\t09: GPS RGP and CTS measurements\r\n\t10: Trace Source Phase Measurement\r\n"
				"\t11: PRR Daily Holdover Report\r\n"
				"\t12: EFC data monitor\r\n";
			pComm->Send(ptr);
			ptr = "\t16: Not specified\r\n"
				"\t17: Rubidium Process\r\n"
				"\t18: Istate status\r\n"
				"\t19: Card State\r\n"
				"\t20: Output status\r\n"
				"\t21: Event reporting\r\n"
				"\t22: PTP message\r\n"
				"\t23: Download over PTP message\r\n"
			//	"\t24: Warmup message\r\n"
				"\t25: House Keeping function\r\n"
				"\t26: Second Process\r\n"
				"\t27: Minute Process\r\n"
			//	"\t26: Phase report\r\n"
			//	"\t30: Card state reporting from twin\r\n"
			;
			pComm->Send(ptr);
				  }
			return;
		case 400: {
			for (int i = 0; i < 32; i++) {
				sprintf(buff, "G%02d=%ld\t", i, m_count[i]);
				pComm->Send(buff);
				if ((i % 4) == 3) {
					pComm->Send("\r\n");
				}
			}
			return;
				 }
		}   // switch
	}
	pComm->Send("Invalid option\r\n");

}

void CDebugMask::SetDebugOption(uint32 d_and, uint32 d_or, Ccomm *pComm)
{
	m_mask &= d_and;
	m_mask |= d_or;
	m_mask |= 0x80000000;
	WRITE_MASK(m_mask);

	if (pComm) {
		char buff[10];
		uint8 bit;

		pComm->Send("\r\nCurrent debug print group mask: \r\nGroup\t31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16\r\nEnable\t");
		for (int i = 31; i >= 0; i--) {
			bit = m_mask >> i;
			bit &= 1;
			sprintf(buff, "%2d ", bit);
			pComm->Send(buff);
			if (i == 16) {
				pComm->Send("\r\nGroup\t15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00\r\nEnable\t");
			}
		}
	}
}

void CDebugMask::CountRequest(uint32 mask)
{
	uint32 tmp;

	tmp = 1;
	for (int i = 0; i < 32; i++) {
		if (mask & tmp) {
			if (mask & ~tmp) {
				// multiple bits. Skip it.
				return;
			}
			m_count[i]++;
			return;
		}
		tmp <<= 1;
	}
}
