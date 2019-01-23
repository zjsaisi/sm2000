// Fpga.cpp: implementation of the CFpga class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: Fpga.cpp 1.36 2010/10/14 00:25:28PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.36 $
 */                 

#include "CodeOpt.h"
#include "DataType.h"   
#include "ConstDef.h"
#include "Fpga.h"
#include "stdio.h"
#include "iodefine.h"
#include "comm.h"
#include "AllProcess.h"
#include "machine.h"
#include "DtiOutput.h"
#include "IstateDti.h"
#include "bt3_def.h"
#include "bt3_dat.h"
#include "BT3Api.h"
#include "DtiOutput.h"
#include "Input.h"
#include "SecondProcess.h"
#include "JumpMon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define STAT_GPS_LOS 0x8

#define STAT_TPIU_LOS 0x4
#define STAT_ANT_LOS  0xc0
		
CFpga *CFpga::s_pFpga = NULL;
SFpgaMap volatile * CONST_READ_HARDWARE CFpga::m_pReadHardware = (SFpgaMap *)FPGA_START_ADDRESS;
SFpgaMap volatile * const CFpga::m_pWriteHardware = (SFpgaMap *)FPGA_START_ADDRESS;
#define BIT_IMC_JAM 0x40000

// STpiuMap volatile * const CFpga::m_pTpiu = (STpiuMap *)FPGA_START_ADDRESS;
//SClientMap volatile * const CFpga::m_pClient11 = (SClientMap *)FPGA_START_ADDRESS;
//SClientMap volatile * const CFpga::m_pClient12 = (SClientMap *)(FPGA_START_ADDRESS + 0x20);

//#define OUTPUT_STATUS ((uint32 volatile *)(0xb0 + FPGA_START_ADDRESS))
//#define GPSSEC ((uint32 volatile *)(0x82 + FPGA_START_ADDRESS))
//#define GPSSEC_LSB ((uint8 volatile *)(0x85 + FPGA_START_ADDRESS))
//#define TOD_MESSAGE ((uint8 volatile *)(0x86 + FPGA_START_ADDRESS))
//#define FPGA_VERSION ((uint8 volatile *)(0x2e + FPGA_START_ADDRESS))
//#define INPUT_PAGE ((uint8 volatile *)(0x0e + FPGA_START_ADDRESS))

#define FINE_CLIENT_TUNE01_ADDRESS (uint16 volatile *)(0x200 + FPGA_START_ADDRESS)
#define FINE_CLIENT_TUNE02_ADDRESS (uint16 volatile *)(0x202 + FPGA_START_ADDRESS)
#define FINE_SERVER_TUNE01_ADDRESS (uint16 volatile *)(0x200 + FPGA_START_ADDRESS)
#define FINE_SERVER_TUNE02_ADDRESS (uint16 volatile *)(0x202 + FPGA_START_ADDRESS)
// Those data should be from Bob H.
#define FINE_CLIENT_TUNE01_VALUE   0x010c
#define FINE_CLIENT_TUNE02_VALUE   0x0b20
#define FINE_SERVER_TUNE01_VALUE   0
#define FINE_SERVER_TUNE02_VALUE   0

#define CLEAR_MEMORY(var) memset(var, 0, sizeof(var))
extern "C" unsigned long Get_micro_icw(void);
CFpga::CFpga()
{
#if SIMULATE_FPGA_READ
	memset(&m_readFpgaSimulator, 0, sizeof(m_readFpgaSimulator));
	m_pReadHardware = &m_readFpgaSimulator;
#endif

#if 1
	m_pEfcMonitor = new CJumpMon(8192);
#else
	m_pEfcMonitor = NULL;
#endif

	m_gpsPhaseInvalid = 0;
	m_gpsPhaseInvalidReported = 0;
	m_gpsPhaseMeasureLoss = 0L;

	m_gpsSecond = 0;
	m_leapSecond = 0;

	m_todSource = -1;

	m_spanInputType[0] = -1;
	m_spanInputType[1] = -1;

	m_spanAPresent = 0;
	m_spanBPresent = 0;
	m_gpsPresent = 0;

	m_ipllStatusOr = 0xff;
	m_previousIpllStatus = 0xff;

	memset(&m_fpgaBuff, 0, sizeof(m_fpgaBuff));
	m_statusAnd = m_previousStatusAnd = 0UL;
	m_statusOr = m_previousStatusOr = 0UL;
	m_isrCnt = 0;

	*FINE_CLIENT_TUNE01_ADDRESS = FINE_CLIENT_TUNE01_VALUE;
	*FINE_CLIENT_TUNE02_ADDRESS = FINE_CLIENT_TUNE02_VALUE;
	*FINE_SERVER_TUNE01_ADDRESS = FINE_SERVER_TUNE01_VALUE;
	*FINE_SERVER_TUNE02_ADDRESS = FINE_SERVER_TUNE02_VALUE;

	m_imcJam = 0;

	CLEAR_MEMORY(m_isrProcessStartTimeStamp);
	m_pIsrTimeStampStart = m_isrProcessStartTimeStamp;
	CLEAR_MEMORY(m_isrProcessEndTimeStamp);
	m_pIsrTimeStampEnd = m_isrProcessEndTimeStamp;

	memset(&m_span1E1Status, 0, sizeof(m_span1E1Status));
	memset(&m_span2E1Status, 0, sizeof(m_span2E1Status));
	memset(&m_span1T1Status, 0, sizeof(m_span1T1Status));
	memset(&m_span2T1Status, 0, sizeof(m_span2T1Status));


/*        old stuff */
	m_gpsPhase = 0L;
	m_jamCnt = 0;
	m_jamClearCnt = 0;
	m_outputStatusAnd = 0xffffffff;
	m_outputStatusOr = 0L;
	m_previousOutputStatusAnd = 0L;
	m_previousOutputStatusOr = 0L;

	//m_fpgaBuff.ctrl2 = MPLL_FREERUN;
	SetReady(0);
	UnlockPll();
	m_edgeAdjustCnt = 0;
	m_25msLost = 0;
	m_cfOutputTodLength = 6;

	m_ctsStatus = 0;
	m_ctsStatusOr = 0;
	m_previousCtsStatusOr = 0;

	m_high = 0x8000;
	m_low = 0x8000;
	m_dither = 0;
	m_currentDither = 0;
	m_freq_off=0;
	memset(m_inputServerStatus, 0, sizeof(m_inputServerStatus));
	memset(m_inputServerStatusAnd, 0, sizeof(m_inputServerStatusAnd));
	memset(m_inputServerStatusWorking, 0, sizeof(m_inputServerStatusWorking));
	memset(m_inputServerPath, 0xff, sizeof(m_inputServerPath));
	memset(m_inputTodMessage, 0xff, sizeof(m_inputTodMessage));

	CLEAR_MEMORY(m_outputClientNormalCnt);
	CLEAR_MEMORY(m_preOutputClientNormalCnt);
	CLEAR_MEMORY(m_outputClientHoldoverCnt);
	CLEAR_MEMORY(m_preOutputClientHoldoverCnt);
	CLEAR_MEMORY(m_outputClientStatus);
	CLEAR_MEMORY(m_outputT3);
	CLEAR_MEMORY(m_preOutputT3);
	CLEAR_MEMORY(m_outputT4);
	CLEAR_MEMORY(m_preOutputT4);
	CLEAR_MEMORY(m_outputT6);
	CLEAR_MEMORY(m_preOutputT6);
	CLEAR_MEMORY(m_outputT7);
	CLEAR_MEMORY(m_preOutputT7);


	CLEAR_MEMORY(m_inputQualified);
	CLEAR_MEMORY(m_inputCollision);
	CLEAR_MEMORY(m_inputMessageFailure);


	SetFpgaHodoverMode(1);

	m_tpiuTune = 0x8000;
	m_taTune = 0x8000;
	m_tpiuCrc[0] = 0;
	m_tpiuCrc[1] = 0;
	m_taCrc[0] = 0;
	m_taCrc[1] = 0;
}

CFpga::~CFpga()
{

}

CFpga *CFpga::CreateFpga()
{
	if (s_pFpga == NULL) {
		s_pFpga = new CFpga();
	}
	return s_pFpga;
}

EXTERN void isr_fpga1(void) // From IRQ4
{
	CFpga::s_pFpga->Isr25ms();
}

EXTERN void isr_fpga2(void) // From IRQ6 
{
}

#define SECOND_EDGE 0x80000

void CFpga::Isr25ms()
{
	// Beginning of ISR. 
	*m_pIsrTimeStampStart = TMRA.TCNT;

	m_isrCnt++;
	Collect();
	Bt3_25ms();
	Input_25ms();

	if (m_isrCnt >= 40) {
		Bt3_1sec();
		Input_1sec();

		m_isrCnt = 0;
		Summarize();
		CAllProcess::g_pAllProcess->CsmSecondAnnounce();
	} else if (m_status & SECOND_EDGE) {
		m_25msLost += m_isrCnt;
		m_isrCnt = 0;
		m_edgeAdjustCnt++;
	}

	// End of ISR
	if (m_pIsrTimeStampStart >= (m_isrProcessStartTimeStamp + NUM_ISR_TIMESTAMP - 1)) {
		m_pIsrTimeStampStart = m_isrProcessStartTimeStamp;
	} else {
		m_pIsrTimeStampStart++;
	}
	*m_pIsrTimeStampEnd = TMRA.TCNT;
	if (m_pIsrTimeStampEnd >= (m_isrProcessEndTimeStamp + NUM_ISR_TIMESTAMP - 1)) {
		m_pIsrTimeStampEnd = m_isrProcessEndTimeStamp;
	} else {
		m_pIsrTimeStampEnd++;
	}

	Bt3Next();

#if !TO_BE_COMPLETE

	// Caution: The sequence cannot be changed.
	// Beginning of ISR. 
	*m_pIsrTimeStampStart = TMRA.TCNT;

	m_isrCnt++;
	Collect();
	CollectOutput();

	Bt3_25ms();
	Input_25ms();

	if (m_isrCnt >= 40) {
		Bt3_1sec();
		Input_1sec();
		m_isrCnt = 0;
		CAllProcess::g_pAllProcess->CsmSecondAnnounce();
		Summarize();
	} else if (m_status & SECOND_EDGE) {
		m_25msLost += m_isrCnt;
		m_isrCnt = 0;
		m_edgeAdjustCnt++;
	} else if (m_isrCnt == 1) { // balance the load
		SummarizeOutput();
		if (m_status & 0x100) {
			CDtiOutput::ReadRegisterIsr(1);
		} else {
			CDtiOutput::ReadRegisterIsr(0);
		}
	}

	Bt3Next();

	// End of ISR
#endif
}

void CFpga::PrintIsrTimeStamp(unsigned long *param, Ccomm *pComm, char *buff)
{
	volatile int i;
	uint8 tmp_exr;
	uint8 volatile tick;
	uint32 volatile gpsSec;
	int volatile index;
	int volatile start;
	int volatile next;
	uint16 during;
	uint32 time = 0;
	uint16 volatile diff;
	uint16 duration;
	int16 ms_duration;
	int j;
	int32 ms;
	int limit;
	int error = 0;

	tmp_exr = get_exr();
	set_exr(7);
	tick = m_isrCnt;
	gpsSec = GetGpsSec();
	index = (int)((uint32)m_pIsrTimeStampStart - (uint32)m_isrProcessStartTimeStamp)/2;
	set_exr(tmp_exr);
			
	limit = NUM_ISR_WATCH/40;
	if (*param == 2) {
		if (limit > param[2]) {
			limit = param[2];
		}
	}
			
	pComm->Send("ISR time and duration in 0.1ms\r\n");

	start = (index - tick + NUM_ISR_TIMESTAMP - limit * 40) % NUM_ISR_TIMESTAMP;
	for (i = 0; i < limit + 1; i++) {
		sprintf(buff, "GpsSec=%ld (index%d): ", gpsSec - limit + i, start);
		pComm->Send(buff);
		for (j = 0; j < 40; j++) {
			duration = m_isrProcessEndTimeStamp[start] - m_isrProcessStartTimeStamp[start];
			next = (start + 1) % NUM_ISR_TIMESTAMP;
			diff = m_isrProcessStartTimeStamp[next] - m_isrProcessStartTimeStamp[start];
			ms = (int32)((double)time * 10000 / (CHEAP_TMR_CLK));
			time += diff;
			ms_duration = (int16)((double)duration * 10000 / (CHEAP_TMR_CLK));

			sprintf(buff, "@%05ld D=%d ",
					ms, ms_duration);
			pComm->Send(buff);
#if 1					
			if ((diff > 60) || (error)) {
				sprintf(buff, "From0x%Xto0x%X(0x%X Index=%d) "
					, m_isrProcessStartTimeStamp[start], m_isrProcessStartTimeStamp[next], diff, start);
				pComm->Send(buff);
				if (diff > 60) {
					error++;
				} else if (error) {
					error--;
				}
			}
#endif
			if ((i >= limit) && (j >= tick)) break;
					
			if ((j != 39 ) && ((j % 5) == 4)) {
				pComm->Send("\r\n\t");
			}

			start = next;

		}
		pComm->Send("\r\n");
	}
		
	tmp_exr = get_exr();
	set_exr(7);
	index = (int)((uint32)m_pIsrTimeStampStart - (uint32)m_isrProcessStartTimeStamp)/2;
	set_exr(tmp_exr);
	sprintf(buff, "Writing Index=%d\r\n", index);
	pComm->Send(buff);
				
	return;	
}

void CFpga::FpgaHack(unsigned long *param, Ccomm *pComm)
{
	char buff[200]; 
	int i,pnew,pold;
	long int delta;
	if (*param == 2) {
#if SIMULATE_FPGA_READ
		if (param[1] == 10) {
			if (param[2]) {
				m_readFpgaSimulator.status32.byte.status_one |= 0x8;
			} else {
				m_readFpgaSimulator.status32.byte.status_one &= 0xf7;
			}
			return;
		}
#endif
	}

	if (*param == 1) {
		switch (param[1]) {
		case 1:
			sprintf(buff, "ISR Cnt: %d FPGA cnt:%d\r\n", m_isrCnt, m_pReadHardware->intr_cnt);
			pComm->Send(buff);
			sprintf(buff, "This is @0x%lX Read FPGA @0x%lX, Write Fpga @0x%lX Writebuff @0x%lX\r\n"
				, (uint32)this, (uint32)m_pReadHardware, (uint32)m_pWriteHardware, (uint32)&m_fpgaBuff);
			pComm->Send(buff);
			sprintf(buff, "FPGA active=%d\r\n", IsFpgaActive());
			pComm->Send(buff);
			sprintf(buff, "GPS Present=0x%04X SPANA Present=0x%04X SPANB Present=0x%04X\r\n"
				, m_gpsPresent, m_spanAPresent, m_spanBPresent);
			pComm->Send(buff);
			return;
		case 2:
			sprintf(buff, "Edge Adjust=%d Tick(25ms) lost=%d\r\n"
				, m_edgeAdjustCnt, m_25msLost
				);
			pComm->Send(buff);
			sprintf(buff, "StatusAnd: %08lX  StatusOr: %08lX\r\n"
				, m_previousStatusAnd, m_previousStatusOr);
			pComm->Send(buff);
			sprintf(buff, "IpllStatus: %02X %02X\r\n"
				, m_ipllStatusOr, m_previousIpllStatus);
			pComm->Send(buff);
			sprintf(buff, "PPC Jam: %d (m_imcJam=%d) m_jamCnt=%d m_jamClearCnt=%d\r\n"
				, GotImcJam(0), m_imcJam, m_jamCnt, m_jamClearCnt);
			pComm->Send(buff);
			{
				int32 gpsPhase;
				gpsPhase = GetGPSPhase();
				sprintf(buff, "GPS Phase offset: %ld\r\n", gpsPhase);
				pComm->Send(buff);
				sprintf(buff, "GPS raw phase: 0x%08lX Invalid cnt: %d Measurement loss:%ld\r\n"
					, m_gpsPhase, m_gpsPhaseInvalid, m_gpsPhaseMeasureLoss);
				pComm->Send(buff);
			}
			return;
		case 3:
			FakeJam();
			CSecondProcess::s_pSecondProcess->UpdateRtc();
			pComm->Send("Fake PPC TOD jam\r\n");
			return;    
			
		case 4:  //GPZ Span A FIFO debug
		
			i=(SPANA_Fifo.PPS_Nwrite_marker+236)%256;	
			sprintf(buff, "Span A FIFO current write index %04d\r\n"
					, SPANA_Fifo.PPS_Nwrite_marker);
			pComm->Send(buff);
			
			while(((SPANA_Fifo.PPS_Nwrite_marker%256) != i) && (i<256)) /*read index is still behind */
		    { 
		    	pnew= (SPANA_Fifo.accum[i]&0x7FFF); 
		    	if(pnew>pold)
		    	{
		    		delta=(long int)pnew-(long int)pold;
		    	}
		    	else
		    	{
		    	   delta= (long int)(pnew+32768) -(long int)(pold);
		    	}
				sprintf(buff, "%4d,%04X,%06d,%10ld\r\n" 
					, i   
					, SPANA_Fifo.accum[i]
					, pnew
					,delta);
				pComm->Send(buff);   
				i++;       
		    	pold= pnew;  
		    	return;
				
           }
			
		}
	}

	if (*param <= 2) {
		switch (param[1]) {
		case 7:
			if (*param == 2) {
       			  m_freq_off= (int32)((double)(param[2])/1.70298985e-03); 
       			  sprintf(buff, "frequency offset set to %ld ppb\r\n",param[2]);
				  pComm->Send(buff);
       		}
       		else m_freq_off=0;
       		sprintf(buff, "frequency offset set to %ld DDS bits\r\n",m_freq_off); 
			pComm->Send(buff);
  			return;
     		 
		case 8:
			if (*param == 2) {
       			  m_freq_off= (int32)((double)(param[2])/-1.70298985e-03); 
       			  sprintf(buff, "frequency offset set to %ld ppb\r\n",param[2]);
				  pComm->Send(buff);
       		}
       		else m_freq_off=0;
       		sprintf(buff, "frequency offset set to %ld DDS bits\r\n",m_freq_off);
			pComm->Send(buff);
			return;
		}
	}

	if (param[1] == 4) {
		PrintIsrTimeStamp(param, pComm, buff);
		return;
	}
	
	if ((param[0] > 1) && (param[1] == 9)) {
		if (m_pEfcMonitor == NULL) {
			pComm->Send("EFC debug disabled\r\n");
		} else {
			m_pEfcMonitor->JumpMonHack(pComm, param);
		}
		return;
	}

	const char *pHelp = "Syntax: <option>\r\n"
		"\t1: Report info set 1\r\n"
		"\t2: Report info set 2\r\n"
#if SIMULATE_FPGA_READ
		"\t10 1/0: Set this FPGA active/standby\r\n"
#endif
		"\t3: Fake PPC jam\r\n"
		"\t4: Dump last 40 Span A FIFO samples\r\n" 
		"\t7 <ppb>: Enter Positive Frequency Offset  ppb\r\n"
		"\t8 <ppb>: Enter Negative Frequency OffsetMagnitude ppb\r\n"
		"\t9 1 <cnt>: List EFC number of past <cnt> seconds\r\n" 
		"\t9 2 <cnt> <limit>: Find EFC jump over <limit> in past <cnt> seconds\r\n"
		"\t9 3 0: Stop EFC ctrl collect\r\n"
		"\t9 3 1: Start EFC ctrl collect\r\n"
		;
	pComm->Send(pHelp);

#if !TO_BE_COMPLETE
	volatile int i;
	uint8 tmp_exr;

	sprintf(buff, "ISR Cnt: %d\r\n", m_isrCnt);
	pComm->Send(buff);

	if (*param >= 1) {
		switch (param[1]) {
		case 1:
			P1.DR.BIT.B3 = 0;						/* Set /FPGA_PROG = LOW */
			P1.DR.BIT.B5 = 0;	          			/* Set /FPGA_INIT = LOW  */
			i = 1000 * 10; while (i--);	
		
			P1.DR.BIT.B3 = 1; 				/* Set /FPGA_PROG = HIGH */                                                                                                                                                
			P1.DR.BIT.B5 = 1;				/* Set /FPGA_INIT = HIGH  */
			pComm->Send("FPGA killed\r\n");
			return;

		case 2: {
			// uint32 raw;
			int32 gpsPhase;
			sprintf(buff, "Edge Adjust=%d Tick(25ms) lost=%d\r\n"
				"Dithered EFC: HIGH=0x%X(%u) LOW=0x%X(%u) DITHER=%d DCNT=%d\r\n"
				, m_edgeAdjustCnt, m_25msLost
				, m_high,	m_high, m_low, m_low,	m_dither, m_currentDither
				);
			pComm->Send(buff);
			sprintf(buff, "InputServerPath @0x%lX(2X64)  InputTODMsg @0x%lX(2X42)\r\n",
				m_inputServerPath, m_inputTodMessage);
			pComm->Send(buff);

			gpsPhase = GetGPSPhase();
#if 0
			if (raw & 0x80000000) {
				gpsPhase = 0x7fffffff;
			} else if (raw & 0x40000000) {
				gpsPhase |= 0x80000000;
			}
#endif
			sprintf(buff, "GPS Phase: %ld\r\n", gpsPhase);
			pComm->Send(buff);
			sprintf(buff, "GPS SPAN_PRESENT: 0x%X\r\n", GPS_Fifo.SPAN_Present);
			pComm->Send(buff);
			sprintf(buff, "DTIA Message collision: %ld  Failed: %ld\r\n"
				, m_inputCollision[0], m_inputMessageFailure[0]);
			pComm->Send(buff);
			sprintf(buff, "DTIB Message collision: %ld  Failed: %ld\r\n"
				, m_inputCollision[1], m_inputMessageFailure[1]);
			pComm->Send(buff);
			sprintf(buff, "StatusAnd: %08lX  StatusOr: %08lX\r\n"
				, m_previousStatusAnd, m_previousStatusOr);
			pComm->Send(buff);
			sprintf(buff, "PPC Jam: %d (m_imcJam=%d) m_jamCnt=%d m_jamClearCnt=%d\r\n"
				, GotImcJam(0), m_imcJam, m_jamCnt, m_jamClearCnt);
			pComm->Send(buff);
			return;
				}
		case 3:
			FakeJam();
			pComm->Send("Fake PPC TOD jam\r\n");
			return;
		case 5:
			AlignGpsPhase(1);
			pComm->Send("Align phase to GPS\r\n");
			CAllProcess::g_pAllProcess->Pause(TICK_1SEC * 3 / 2);
			AlignGpsPhase(0);
			return;
		case 6:
			m_edgeAdjustCnt = 0;
			pComm->Send("edge adjust cnt cleared\r\n");
			m_25msLost = 0;
			pComm->Send("Ticks(25ms) lost cnt cleared\r\n");
			return;
		}
	}


#endif
}

void CFpga::SetSpanOutput(int chan, int type)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	switch (chan) {
	case 0:
		m_fpgaBuff.output_one_select = type;
		m_pWriteHardware->output_one_select = type;
		break;
	case 1:
		m_fpgaBuff.output_two_select = type;
		m_pWriteHardware->output_two_select = type;
		break;
	case 2:
		m_fpgaBuff.output_three_select = type;
		m_pWriteHardware->output_three_select = type;
		break;
	case 3:
		m_fpgaBuff.output_four_select = type;
		m_pWriteHardware->output_four_select = type;
		break;
	}
	set_exr(tmp_exr);
}

void CFpga::Set10MHzOutput(int enable)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	if (enable) {
		m_fpgaBuff.output_hz_control |= 1;
	} else {
		m_fpgaBuff.output_hz_control &= 0xfe;
	}
	m_pWriteHardware->output_hz_control = m_fpgaBuff.output_hz_control;
	set_exr(tmp_exr);
}

void CFpga::Set1PPSOutput(int enable)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	if (enable) {
		m_fpgaBuff.output_hz_control |= 2;
	} else {
		m_fpgaBuff.output_hz_control &= 0xfd;
	}
	m_pWriteHardware->output_hz_control = m_fpgaBuff.output_hz_control;
	set_exr(tmp_exr);
}

void CFpga::SetE1SsmChoice(int chan, int ssm)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	switch (chan) {
	case 0:
		m_fpgaBuff.output_e1_ssm_slot_AB &= 0xf0;
		m_fpgaBuff.output_e1_ssm_slot_AB |= (ssm & 0xf);
		m_pWriteHardware->output_e1_ssm_slot_AB = m_fpgaBuff.output_e1_ssm_slot_AB;
		break;
	case 1:
		m_fpgaBuff.output_e1_ssm_slot_AB &= 0xf;
		m_fpgaBuff.output_e1_ssm_slot_AB |= ((ssm << 4) & 0xf0);
		m_pWriteHardware->output_e1_ssm_slot_AB = m_fpgaBuff.output_e1_ssm_slot_AB;
		break;
	case 2:
		m_fpgaBuff.output_e1_ssm_slot_CD &= 0xf0;
		m_fpgaBuff.output_e1_ssm_slot_CD |= (ssm & 0xf);
		m_pWriteHardware->output_e1_ssm_slot_CD = m_fpgaBuff.output_e1_ssm_slot_CD;
		break;
	case 3:
		m_fpgaBuff.output_e1_ssm_slot_CD &= 0xf;
		m_fpgaBuff.output_e1_ssm_slot_CD |= ((ssm << 4) & 0xf0);
		m_pWriteHardware->output_e1_ssm_slot_CD = m_fpgaBuff.output_e1_ssm_slot_CD;
		break;
	}

	set_exr(tmp_exr);
}

void CFpga::SetReady(int ready)
{
#if !TO_BE_COMPLETE

	uint8 tmp_exr;

	if (!ready) {
		Transfer();
		CAllProcess::g_pAllProcess->Pause(1);
	}

	tmp_exr = get_exr();
	set_exr(7);

	if (ready == 1) {
		m_fpgaBuff.ctrl1 |= 0x4;
	} else {
		m_fpgaBuff.ctrl1 &= 0xfb;
	}
	m_pHardware->ctrl1 = m_fpgaBuff.ctrl1;
	set_exr(tmp_exr);
#endif
}

void CFpga::SetFpgaHodoverMode(int mode)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	
	if (mode) {
		m_fpgaBuff.ctrl1 |= 0x10;
	} else {
		m_fpgaBuff.ctrl1 &= 0xef;
	}
	m_pHardware->ctrl1 = m_fpgaBuff.ctrl1;
	
	set_exr(tmp_exr);
#endif
}

void CFpga::SetUnlockHardControl(int set)
{
#if !TO_BE_COMPLETE 
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	if (set) {
		m_fpgaBuff.ctrl1 |= 0x80;
	} else {
		m_fpgaBuff.ctrl1 &= 0x7f;
	}
	m_pHardware->ctrl1 = m_fpgaBuff.ctrl1;

	set_exr(tmp_exr);
#endif
}

void CFpga::SetSlotId(int8 slot)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	if (slot) {
		m_fpgaBuff.ctrl1 |= 0x20;
	} else {
		m_fpgaBuff.ctrl1 &= 0xdf;
	}
	m_pHardware->ctrl1 = m_fpgaBuff.ctrl1;

	set_exr(tmp_exr);
#endif
}

void CFpga::SetTodArm(int8 arm)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;
	uint8 changed = 0;
	tmp_exr = get_exr();
	set_exr(7);

	if (arm) {
		if ((m_fpgaBuff.ctrl1 & 0x40) == 0) {
			m_fpgaBuff.ctrl1 |= 0x40;
			changed = 1;
		}
	} else {
		if (m_fpgaBuff.ctrl1 & 0x40) {
			m_fpgaBuff.ctrl1 &= 0xbf;
			changed = 1;
		}
	}
	m_pHardware->ctrl1 = m_fpgaBuff.ctrl1;

	set_exr(tmp_exr);

	if (changed) {
		REPORT_EVENT(COMM_RTE_EVENT_ARM_SYNC_JAM, arm);
	}
#endif
}

void CFpga::Collect()
{
	uint32 stat;
	uint32 gpsPhase;

	stat = m_status = m_pReadHardware->status32.word32 ;
	m_statusAnd &= stat;
	m_statusOr |= stat;
	
#if 0 // gpz code
	gpsPhase = m_pReadHardware->gps_phase;  
	m_gpsPhase = gpsPhase;
	m_gpsPhaseInvalid = 0;
#else // zmiao code
	gpsPhase = m_pReadHardware->gps_phase;  
	if (gpsPhase & 0x80000000) {
		if (!(gpsPhase & 0x40000000)) {
			gpsPhase &= 0x7fffffff;
		}
		m_gpsPhase = gpsPhase;
		m_gpsPhaseInvalid = 0;
		m_gpsPhaseInvalidReported = 0;
	} else if (m_gpsPhaseInvalid < 20000) {
		m_gpsPhaseInvalid++;
		if (m_gpsPhaseInvalid >= 20000) {
			m_gpsPhaseInvalid -= 18000;
			m_gpsPhaseInvalidReported -= 18000;
		}
		if (m_gpsPhaseInvalid > m_gpsPhaseInvalidReported + 40) {
			m_gpsPhaseInvalidReported = m_gpsPhaseInvalid + 40;
			m_gpsPhaseMeasureLoss++;
		}
	}

#endif

#if !TO_BE_COMPLETE
	uint32 stat;

	stat = m_status = m_pHardware->status;
	m_statusAnd &= stat;
	m_statusOr |= stat;

	m_efc = m_pHardware->efc;	

	m_ctsStatus = m_pTpiu->cts_status;
	m_ctsStatusOr |= m_ctsStatus;
#endif
}

void CFpga::Summarize()
{
	m_previousStatusAnd = m_statusAnd;
	m_previousStatusOr = m_statusOr;
	m_statusAnd = 0xffffffffUL;
	m_statusOr = 0UL;

	m_gpsSecond = m_pReadHardware->uti_gps_second;
	m_leapSecond = m_pReadHardware->uti_leap_correction;

#if 0 // changed to software way
	if (m_statusOr & BIT_IMC_JAM) {
		if (m_imcJam != 3) {
			m_jamCnt++;
			m_imcJam = 3; // indicate initial jam.
		}
	} else { // If jammed, the flag clears. Now it's stable. 
		m_imcJam &= 0xfd;
	}
#endif

#if !TO_BE_COMPLETE	

	m_previousCtsStatusOr = m_ctsStatusOr;
	m_ctsStatusOr = 0;

	// Read tpiu/ta tuning 
	// TPIU/TA crc error cnts
	m_tpiuTune = m_pTpiu->tpiu_vcxo_tuning_voltage;
	m_taTune = m_pTpiu->ant_tcxo_tuning_voltage;
	m_tpiuCrc[0] = m_tpiuCrc[1];
	m_tpiuCrc[1] = m_pTpiu->tpiu_crc_error_cnt;
	m_taCrc[0] = m_taCrc[1];
	m_taCrc[1] = m_pTpiu->ant_crc_error_cnt;
#endif
}

int CFpga::IsGpsPathCrcOk(void)
{
#if !TO_BE_COMPLETE
	if (m_taCrc[0] != m_taCrc[1]) return 0;
	if (m_tpiuCrc[0] != m_tpiuCrc[1]) return 0;
	return 1;
#else
	return 1;
#endif
}

int CFpga::IsTaTuneOk(void)
{
#if !TO_BE_COMPLETE
	if ((m_taTune < 6554) || (m_taTune > 58981)) {
		return 0;
	}
	return 1;
#else
	return 1;
#endif
}

int CFpga::IsTpiuTuneOk(void)
{
#if !TO_BE_COMPLETE
	if ((m_tpiuTune < 6554) || (m_tpiuTune > 58981)) {
		return 0;
	}
	return 1;
#else
	return 1;
#endif
}

// return 1 for active. -1 not sure
int CFpga::IsFpgaActive()
{
	if (m_previousStatusAnd & 0x8000000) return 1;
	if (m_previousStatusOr & 0x8000000) return -1;
	return 0;
}

// return 1: Twin in
int CFpga::IsTwinIn()
{
	if (m_previousStatusAnd & 0x80000000) return 1;
	return 0;
}

void CFpga::Transfer()
{
	// No tranfer function is supported from RTE of TP5000
	return;
}

void CFpga::CollectOutput()
{

#if !TO_BE_COMPLETE

	register uint8 chan;
	register uint8 stat;
	register uint8 old;
	uint8 *pStat;
	uint16 *pNormalCnt;
	uint16 *pHoldoverCnt;

	m_outputStatusOr |= *OUTPUT_STATUS;
	m_outputStatusAnd &= *OUTPUT_STATUS;

	pStat = m_outputClientStatus;
	pNormalCnt = m_outputClientNormalCnt;
	pHoldoverCnt = m_outputClientHoldoverCnt;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		*OUTPUT_PORT_SELECT = chan;
		stat = *CLIENT_STATUS_FLAG;
		stat &= 0x3f;
		if (stat != *pStat) {
			old = *pStat;
			if (old == CDtiOutput::CLIENT_FAST_TRACK) {
				if (stat == CDtiOutput::CLIENT_NORMAL_TRACK) {
					m_outputT4[chan]++;
				} else if (stat == CDtiOutput::CLIENT_FREERUN) {
					m_outputT3[chan]++;
				}
			} else if (old == CDtiOutput::CLIENT_BRIDGE) {
				if (stat == CDtiOutput::CLIENT_NORMAL_TRACK) {
					m_outputT6[chan]++;
				} else if (stat == CDtiOutput::CLIENT_HOLDOVER) {
					m_outputT7[chan]++;
				}
			}
			*pStat = stat;
		}
		if (stat == CDtiOutput::CLIENT_NORMAL_TRACK) {
			*pNormalCnt += 250;
		} else if (stat == CDtiOutput::CLIENT_HOLDOVER) {
			*pHoldoverCnt += 250;
		}
		pStat++;
		pNormalCnt++;
		pHoldoverCnt++;
	}
#endif
}

void CFpga::SummarizeOutput()
{
#if !TO_BE_COMPLETE
	m_previousOutputStatusAnd = m_outputStatusAnd;
	m_outputStatusAnd = 0xffffffff;
	m_previousOutputStatusOr = m_outputStatusOr;
	m_outputStatusOr = 0L;

	memcpy(m_preOutputClientNormalCnt, m_outputClientNormalCnt, sizeof(m_preOutputClientNormalCnt));
	memcpy(m_preOutputClientHoldoverCnt, m_outputClientHoldoverCnt, sizeof(m_preOutputClientHoldoverCnt));
	memcpy(m_preOutputT3, m_outputT3, sizeof(m_preOutputT3));
	memcpy(m_preOutputT4, m_outputT4, sizeof(m_preOutputT4));
	memcpy(m_preOutputT6, m_outputT6, sizeof(m_preOutputT6));
	memcpy(m_preOutputT7, m_outputT7, sizeof(m_preOutputT7));

	CLEAR_MEMORY(m_outputClientNormalCnt);
	CLEAR_MEMORY(m_outputClientHoldoverCnt);
	CLEAR_MEMORY(m_outputT3);
	CLEAR_MEMORY(m_outputT4);
	CLEAR_MEMORY(m_outputT6);
	CLEAR_MEMORY(m_outputT7);
#endif
}

void CFpga::GetOutputStatus(uint32 &d_and, uint32 &d_or)
{
#if !TO_BE_COMPLETE

	//uint8 tmp_exr;

	//tmp_exr = get_exr();
	//set_exr(7);
	d_and = m_previousOutputStatusAnd;
	d_or = m_previousOutputStatusOr;
	//set_exr(tmp_exr);
#endif

}


void CFpga::SetGpsSec(uint32 gps)
{
#if !TO_BE_COMPLETE

	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	*GPSSEC = gps;
	set_exr(tmp_exr);
#endif
}

uint8 CFpga::GetGpsLsb()
{
#if !TO_BE_COMPLETE
	return *GPSSEC_LSB;
#else
	return 0;
#endif
}

void CFpga::SendTod(const char *ptr)
{
#if !TO_BE_COMPLETE
	int i;
	uint8 tmp_exr;
	int len;

	len = m_cfOutputTodLength;
	tmp_exr = get_exr();
	set_exr(7);
	for (i = 0; i < len; i++) {
		*TOD_MESSAGE = *ptr++;
	}
	set_exr(tmp_exr);
#endif
}

void CFpga::UnlockPll()
{
	uint8 tmp_exr;
	uint8 volatile ctrl;

	tmp_exr = get_exr();
	set_exr(7);

	ctrl = m_fpgaBuff.status32.byte.pll_reg;
	m_pWriteHardware->status32.byte.pll_reg = 1;
	m_pWriteHardware->status32.byte.pll_reg = ctrl;
	
	set_exr(tmp_exr);
}

void CFpga::SetMpll(int mpll)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;
	uint8 ctrl;

	tmp_exr = get_exr();
	set_exr(7);

	ctrl = m_fpgaBuff.ctrl2 & 0xf8;
	ctrl |= (mpll & 7);
	m_pHardware->ctrl2 = ctrl;
	m_fpgaBuff.ctrl2 = ctrl;

	set_exr(tmp_exr);
#endif
}

int CFpga::GetTick()
{
#if !TO_BE_COMPLETE
	return m_isrCnt;    
#else
	return 0;	
#endif 
	
}

int CFpga::GotImcJam(int clear)
{
	int volatile ret;
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	ret = (m_imcJam == 1) ? (1) : (0);

	if ((clear) && (m_imcJam == 1)) {
		m_jamClearCnt++;
		m_imcJam = 0;
	}

	set_exr(tmp_exr);

	return ret;

}

void CFpga::FakeJam()
{
	m_imcJam = 1;

}


void CFpga::CfOutputTodMode(int mode)
{
#if !TO_BE_COMPLETE

	int len;
	switch (mode) {
	case CIstateDti::OUTPUT_TOD_SHORT:
		len = 6;
		break;
	case CIstateDti::OUTPUT_TOD_VERBOSE:
		len = 41;
		break;
	default:
		len = 6;
		break;
	}
	m_cfOutputTodLength = len;
#endif
}

void CFpga::SetPortMode(int chan, int out)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;
	uint8 dor, dand;

	switch (chan) {
	case 10:
		dor = 0x40;
		dand = 0xbf;
		break;
	case 11:
		dor = 0x80;
		dand = 0x7f;
		break;
	default:
		return;
	}

	tmp_exr = get_exr();
	set_exr(7);

	if (out) {
		m_fpgaBuff.ctrl2 |= dor;
	} else {
		m_fpgaBuff.ctrl2 &= dand;
	}
	m_pHardware->ctrl2 = m_fpgaBuff.ctrl2;
	set_exr(tmp_exr);
#endif
}

uint32 CFpga::GetGPSPhase()
{
	uint8 tmp_exr;
	uint32 ret;
	tmp_exr = get_exr();
	set_exr(7);
	if (m_gpsPhaseInvalid >= 120) {
		ret = 0xF0000000;
	} else {
		ret = m_gpsPhase;
	}
	set_exr(tmp_exr);
	return ret; 
}

#define INCREMENT_BT3(x) x.Nwrite = (x.Nwrite + 1) % MBSIZE
void CFpga::Bt3Next()
{
	INCREMENT_BT3(mpll);
	INCREMENT_BT3(GPS_Fifo);
	INCREMENT_BT3(SPANA_Fifo);
	INCREMENT_BT3(SPANB_Fifo);
}

void CFpga::Bt3_25ms()
{
	int32 efc;
	mpll.sum[mpll.Nwrite] = m_efc = m_pReadHardware->accumulate_efc;
	GPS_Fifo.accum[GPS_Fifo.Nwrite] = m_pReadHardware->gps_8k_phase;
	SPANA_Fifo.accum[SPANA_Fifo.Nwrite] = m_pReadHardware->span_one_phase;
	SPANB_Fifo.accum[SPANB_Fifo.Nwrite] = m_pReadHardware->span_two_phase;

	m_pWriteHardware->current_efc = efc = Get_micro_icw() + m_freq_off;
	if (m_pEfcMonitor) {
		m_pEfcMonitor->Update(efc);
	}

}

#if !TO_BE_COMPLETE
static unsigned int pace;
void CFpga::Bt3_25ms()
{
	uint16 icw,dcor; //EFC
	uint32 expandedEfc;
#if 0
	// control EFC
	if(m_dither<100)  
	{            
//		if(m_dither<99)
//		{
//			m_currentDither += m_dither;
//		}
//		if (m_currentDither >= 40) {
//			m_currentDither %= 40;
//			m_pHardware->efc = m_high;
//		} else if (m_low != 0) {
//			m_pHardware->efc = m_low;
//		}     
		if(m_dither<99)		
		{
			m_currentDither += 19;		
			m_currentDither %= 40;	
			dcor=(m_currentDither>>3);	 
			if(dcor>2)
			{
			 if(m_dither)
			 {
			  dcor=0;
			  m_dither--;
			 }  
			 else
			 {
			   dcor=3;
			 }
			} 
		} 
		else
		{
		 	dcor=0;
		}
		m_pHardware->efc = m_low+dcor;
   }
   else if (m_dither==101)
   {
   		m_pHardware->efc = 0xB000; 
   }
   else if (m_dither==102)
   {
   		m_pHardware->efc = 0x3000; 
   }
   else if (m_dither==103)
   {
   		m_pHardware->efc = (m_pHardware->efc+0x0001)&0xFFFF; 
   }
   else if (m_dither==104)
   {          
        if(++pace >39)
        {
   		  m_pHardware->efc = (m_pHardware->efc+0x0001)&0xFFFF;    
   		  pace=0;
   		}  
   }
#endif //GPZ replace with synthesizer control      		
	expandedEfc = m_efc;
//	if (m_status & 0x20000) {
//		expandedEfc |= 0x80000000;
//	}
	mpll.sum[mpll.Nwrite] = expandedEfc;

#if 1
//	m_pHardware->efc = Get_micro_icw();    
	m_pHardware->efc = (Get_micro_icw()+m_freq_off);
	
#endif

	GPS_Fifo.accum[GPS_Fifo.Nwrite] = m_pHardware->gps_8k_phase;
}
#endif

void CFpga::Bt3_1sec()
{
	mpll.PPS_Nwrite_marker = mpll.Nwrite;
	GPS_Fifo.PPS_Nwrite_marker = GPS_Fifo.Nwrite;
	SPANA_Fifo.PPS_Nwrite_marker = SPANA_Fifo.Nwrite;
	SPANB_Fifo.PPS_Nwrite_marker = SPANB_Fifo.Nwrite;

	// Tmp code to make all valid
//	GPS_Fifo.SPAN_Present &= ~0x100;
//	SPANA_Fifo.SPAN_Present &= ~0x100;
//	SPANB_Fifo.SPAN_Present &= ~0x100;
	if (m_statusAnd & m_previousStatusAnd & 0x8000000) {
		GPS_Fifo.SPAN_Present = m_gpsPresent;
		SPANA_Fifo.SPAN_Present = m_spanAPresent;
		SPANB_Fifo.SPAN_Present = m_spanBPresent;
	} else {
		GPS_Fifo.SPAN_Present = 0;
		SPANA_Fifo.SPAN_Present = 0;
		SPANB_Fifo.SPAN_Present = 0;
	}

	if (m_pEfcMonitor) {
		m_pEfcMonitor->GoNext();
	}
	
#if !TO_BE_COMPLETE

	mpll.PPS_Nwrite_marker = mpll.Nwrite;
	GPS_Fifo.PPS_Nwrite_marker = GPS_Fifo.Nwrite;

	if ((m_statusOr & STAT_TPIU_LOS) || (m_ctsStatusOr & STAT_ANT_LOS)) {
		GPS_Fifo.SPAN_Present &= ~0x100;
	} else if (m_inputQualified[CHAN_GPS] != CInput::INPUT_QUALIFY_OK) {
		GPS_Fifo.SPAN_Present &= ~0x100;
	}else {
		GPS_Fifo.SPAN_Present |= 0x100;
	}

#endif
}

// 1: LOS
// 0: OK
int CFpga::GetGpsLos()
{
	if (m_previousStatusOr & STAT_GPS_LOS) {
		return 1;
	}
	return 0;
}


void CFpga::SetHighLowDither(unsigned int high, unsigned int low, unsigned int dither)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	m_high = high;
	m_low = low;
	m_dither = dither;

	set_exr(tmp_exr);
#endif
}

void CFpga::SetTodSource(int tod)
{
	m_todSource = tod;
	if (m_todSource == CIstateDti::TOD_SOURCE_GPS) {
		m_gpsPresent |= BT3_ENABLE_BIT; 
	}	
	//GPZ June 2010 allow GPS present in user mode
	else if (m_todSource == CIstateDti::TOD_SOURCE_USER) {
		m_gpsPresent |= BT3_ENABLE_BIT;    
	//GPZ June 2010 allow GPS present in hybrid mode
	}else if (m_todSource == CIstateDti::TOD_SOURCE_HYBRID) {
		m_gpsPresent |= BT3_ENABLE_BIT;
	} else {
		m_gpsPresent &= ~(uint16)BT3_ENABLE_BIT;
	}

#if !TO_BE_COMPLETE
	switch (tod) {
	case CIstateDti::TOD_SOURCE_RTC:
	case CIstateDti::TOD_SOURCE_USER:
	case CIstateDti::TOD_SOURCE_NTP:
		SetMpll(CFpga::MPLL_HOLDOVER);
		break;
	case CIstateDti::TOD_SOURCE_GPS:
		SetMpll(CFpga::MPLL_GPS);
		break;
	case CIstateDti::TOD_SOURCE_DTI:
		break;
	}
#endif
}

void CFpga::SetInput11ClientStatus(unsigned char stat)
{
#if !TO_BE_COMPLETE
	m_pClient11->clientStatus = stat;
#endif
}

void CFpga::SetInput12ClientStatus(unsigned char stat)
{
#if !TO_BE_COMPLETE
	m_pClient12->clientStatus = stat;
#endif
}

int CFpga::GetIpllStatus(int chan)
{
	uint8 stat;
	stat = m_previousIpllStatus;
	switch (chan) {
	case 1:
		stat >>= 4;
		// fall thru
	case 0:
		if (stat & 1) return CInput::INPUT_STAT_LOS;
		if (stat & 2) return CInput::INPUT_STAT_PHASE_ERROR;
		if (stat & 8) return CInput::INPUT_STAT_FREQUENCY_ERROR;
		if (stat & 4) return CInput::INPUT_STAT_IPLL_UNSETTLED;
		break;
	default:
		return CInput::INPUT_STAT_LOS;
	}
	return CInput::INPUT_STAT_OK;
}

// return 1: for rub card
// return 0: for Oven
int CFpga::IsRubidium(void)
{
	uint8 stat;
	stat = m_pReadHardware->status32.byte.status_two;
	// According to Bob H 3/31/2008, 
	// Bit 7 is 1 for rubidium osc
	if (stat & 0x80) return 1;
	return 0;
}

int CFpga::IsRubLocked(void)
{
	if (m_previousStatusOr & 0x2000) {
		return 0;
	}
	return 1;
}

int CFpga::Check2MHZ(int chan)
{
	uint8 And, Or;
	switch (chan) {
	case CHAN_BITS1:
		And = m_span1E1Status.preAnd;
		Or = m_span1E1Status.preOr;
		break;
	case CHAN_BITS2:
		And = m_span2E1Status.preAnd;
		Or = m_span2E1Status.preOr;
		break;
	default:
		return CInput::INPUT_STAT_LOS;
	}
	if (And & 0x20) {
		// It's E1 signal. 
		return CInput::INPUT_STAT_LOS;
	}
	return CInput::INPUT_STAT_OK;

}

int CFpga::CheckE1(int chan, int cas, int crc4) 
{
	uint8 And, Or;
	switch (chan) {
	case CHAN_BITS1:
		And = m_span1E1Status.preAnd;
		Or = m_span1E1Status.preOr;
		break;
	case CHAN_BITS2:
		And = m_span2E1Status.preAnd;
		Or = m_span2E1Status.preOr;
		break;
	default:
		return CInput::INPUT_STAT_OOF;
	}
	if (Or & 0x80) {
		// AIS detected		
		return CInput::INPUT_STAT_AIS;
	}
	if ((And & 0x20) == 0) {
		// no FAS
		return CInput::INPUT_STAT_OOF;
	}
	if (cas && ((And & 0x10) == 0)) {
		// no CAS
		return CInput::INPUT_STAT_OOF;
	}
	if (crc4 && ((And & 0x40) == 0)) {
		return CInput::INPUT_STAT_OOF;
	}
	return CInput::INPUT_STAT_OK;

}

int CFpga::Check1544MHZ(int chan)
{
	uint8 And, Or;
	switch (chan) {
	case CHAN_BITS1:
		And = m_span1T1Status.preAnd;
		Or = m_span1T1Status.preOr;
		break;
	case CHAN_BITS2:
		And = m_span2T1Status.preAnd;
		Or = m_span2T1Status.preOr;
		break;
	default:
		return CInput::INPUT_STAT_LOS;
	}
	
	if (And & 0x8) { // ESF. Not good
		return CInput::INPUT_STAT_LOS;
	}
	if (And & 0x10) { // D4. Not Good
		return CInput::INPUT_STAT_LOS;
	}
	return CInput::INPUT_STAT_OK;
}

int CFpga::CheckT1(int chan, int esf)
{  
	uint8 And, Or;
	switch (chan) {
	case CHAN_BITS1:
		And = m_span1T1Status.preAnd;
		Or = m_span1T1Status.preOr;
		break;
	case CHAN_BITS2:
		And = m_span2T1Status.preAnd;
		Or = m_span2T1Status.preOr;
		break;
	default:
		return CInput::INPUT_STAT_OOF;
	}

	if (Or & 0x2) return CInput::INPUT_STAT_AIS;
	if (esf) {
		if (And & 0x8) return CInput::INPUT_STAT_OK;
		return CInput::INPUT_STAT_OOF;
	}
	if (And & 0x10) return CInput::INPUT_STAT_OK;
	return CInput::INPUT_STAT_OOF;
}

void CFpga::Input_25ms()
{
	uint8 tmp;
	// zmiao 9/25/2008: Bob H/Gary J said to ignore those IPLL bits.
	m_ipllStatusOr |= (m_pReadHardware->ipll_status & 0xbb);
	
	// Look at framing
	m_span1E1Status.andData &= tmp = m_pReadHardware->span_one_e1_rx_status;
	m_span1E1Status.orData |= tmp;
	m_span2E1Status.andData &= tmp = m_pReadHardware->span_two_e1_rx_status;
	m_span2E1Status.orData |= tmp;
	m_span1T1Status.andData &= tmp = m_pReadHardware->span_one_t1_rx_status;
	m_span1T1Status.orData |= tmp;
	m_span2T1Status.andData &= tmp = m_pReadHardware->span_two_t1_rx_status;
	m_span2T1Status.orData |= tmp;
	
	CollectSsm25ms();

#if !TO_BE_COMPLETE
	// server status
	m_inputServerStatusWorking[0] &= m_pClient11->serverStatusFlag;
	m_inputServerStatusWorking[1] &= m_pClient12->serverStatusFlag;
#endif

}

//#define FPGA_INPUT_PATH			(FPGA_START_ADDRESS + 0x100)
//#define FPGA_INPUT_TOD_MESSAGE  (FPGA_START_ADDRESS + 0x140)  // should be 140
//#define FPGA_SELECT_INPUT_MESSAGE		(unsigned char volatile *)(FPGA_START_ADDRESS + 0xff)
#define COPY_STATUS(x) x.preAnd = x.andData; \
	x.preOr = x.orData; \
	x.andData = 0xff; x.orData = 0;

void CFpga::Input_1sec()
{
	m_previousIpllStatus = m_ipllStatusOr;
	m_ipllStatusOr = 0;

	// look at framing
	COPY_STATUS(m_span1E1Status);
	COPY_STATUS(m_span2E1Status);
	COPY_STATUS(m_span1T1Status);
	COPY_STATUS(m_span2T1Status);

	CollectSsm1S();

#if !TO_BE_COMPLETE
	// Server device type
	m_inputServerDeviceType[0] = m_pClient11->serverDeviceType;
	m_inputServerDeviceType[1] = m_pClient12->serverDeviceType;

	// server status
	m_inputServerStatusAnd[0] = m_inputServerStatusWorking[0];
	m_inputServerStatusWorking[0] = 0xff;
	m_inputServerStatusAnd[1] = m_inputServerStatusWorking[1];
	m_inputServerStatusWorking[1] = 0xff;

	m_inputServerStatus[0] = m_pClient11->serverStatusFlag;
	m_inputServerStatus[1] = m_pClient12->serverStatusFlag;

	// CRC error
	m_inputErrorCnt[0] = m_pClient11->crcErrorCnt;
	m_inputErrorCnt[1] = m_pClient12->crcErrorCnt;


	// Cable advance
	m_inputCableAdvance[0] = m_pClient11->cableAdvance;
	m_inputCableAdvance[1] = m_pClient12->cableAdvance;

	// Clear crc error and jitter
	m_pClient11->control = m_pClient11->control | 0x6;
	m_pClient11->control = m_pClient11->control & 0xf9;
	m_pClient12->control = m_pClient12->control | 0x6;
	m_pClient12->control = m_pClient12->control & 0xf9;

	{
		uint8 volatile page;
		uint8 volatile next;
		int8 i;

		// read input message
		*FPGA_SELECT_INPUT_MESSAGE = 0;
		for (i = 0; i < 2; i++) {  // read port 11
			page = (*INPUT_PAGE) & 4;
			memcpy(m_inputServerPath[0], (char *)FPGA_INPUT_PATH, MAX_PATH_SIZE); 
			memcpy(m_inputTodMessage[0], (char *)FPGA_INPUT_TOD_MESSAGE, MAX_TOD_SIZE);
			next = (*INPUT_PAGE) & 4;
			if (page == next) break;
			if (i) {
				m_inputMessageFailure[0]++;
			} else {
				m_inputCollision[0]++;
			}
		}

		*FPGA_SELECT_INPUT_MESSAGE = 3;
		for (i = 0; i < 2; i++) {
			page = (*INPUT_PAGE) & 8;
			memcpy(m_inputServerPath[1], (char *)FPGA_INPUT_PATH, MAX_PATH_SIZE); 
			memcpy(m_inputTodMessage[1], (char *)FPGA_INPUT_TOD_MESSAGE, MAX_TOD_SIZE);
			next = (*INPUT_PAGE) & 8;
			if (page == next) break;
			if (i) {
				m_inputMessageFailure[1]++;
			} else {
				m_inputCollision[1]++;
			}
		}
		*FPGA_SELECT_INPUT_MESSAGE = 0; // Not necessary. 
	}
#endif
}

void CFpga::GetInputServerStatus(int chan, unsigned char *pCurr, unsigned char *pAnd)
{
#if !TO_BE_COMPLETE
	unsigned char tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	*pCurr = m_inputServerStatus[chan - CHAN_DTI1];
	*pAnd = m_inputServerStatusAnd[chan - CHAN_DTI1];
	set_exr(tmp_exr);
#endif
}

uint16 CFpga::getInputCrcError(int chan)
{
#if !TO_BE_COMPLETE
	return m_inputErrorCnt[chan-CHAN_DTI1];  
#else
	return 0;	
#endif
}

void CFpga::SetInputLed(int chan, int yellow, int green)
{
#if !TO_BE_COMPLETE
	uint8 led;

	led = (yellow << 1) | green;
	switch (chan) {
	case CHAN_DTI1:
		m_pClient11->led = led;
		break;
	case CHAN_DTI2:
		m_pClient12->led = led;
		break;

	}
#endif
}

void CFpga::GetInputPath(int chan, unsigned char *pPath)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	memcpy(pPath, m_inputServerPath[chan - CHAN_DTI1], MAX_PATH_SIZE);
	set_exr(tmp_exr);
#endif
}

void CFpga::GetInputTodMessage(int chan, unsigned char *pTodMessage)
{
#if !TO_BE_COMPLETE

	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	memcpy(pTodMessage, m_inputTodMessage[chan - CHAN_DTI1], MAX_TOD_SIZE);
	set_exr(tmp_exr);
#endif
}

unsigned char CFpga::GetInputServerDeviceType(int chan)
{
#if !TO_BE_COMPLETE
	return m_inputServerDeviceType[chan - CHAN_DTI1];  
#else
	return 0;	
#endif
}

int32 CFpga::GetInputCableAdvance(int chan)
{
#if !TO_BE_COMPLETE
	return m_inputCableAdvance[chan - CHAN_DTI1];   
#else
	return 0;	
#endif
}

void CFpga::FillTCounter(int32 *pCounter)
{
#if !TO_BE_COMPLETE
	uint8 chan;
	uint8 *pT;
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);

	pT = m_preOutputT3;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pCounter++)) += *(pT++);
	}

	pT = m_preOutputT4;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pCounter++)) += *(pT++);
	}

	pT = m_preOutputT6;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pCounter++)) += *(pT++);
	}

	pT = m_preOutputT7;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pCounter++)) += *(pT++);
	}

	set_exr(tmp_exr);
#endif
}

void CFpga::FillNHDuration(uint32 *pDuration)
{
#if !TO_BE_COMPLETE

	uint8 tmp_exr;
	int chan;
	uint16 *pCount;

	tmp_exr = get_exr();
	set_exr(7);

	pCount = m_preOutputClientNormalCnt;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pDuration++)) += *(pCount++);
	}
	pCount = m_preOutputClientHoldoverCnt;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		(*(pDuration++)) += *(pCount++);
	}

	set_exr(tmp_exr);
#endif
}

void CFpga::SetInputQualified(int chan, int ql)
{
	if ((chan < 0) || (chan >=NUM_INPUT)) return;
	m_inputQualified[chan] = ql;
	switch (chan) {
	case CHAN_BITS1:
		if (ql) {
			m_spanAPresent |= BT3_QUALIFY_BIT;
		} else {
			m_spanAPresent &= ~(uint16)BT3_QUALIFY_BIT;
		}
		break;
	case CHAN_BITS2:
		if (ql) {
			m_spanBPresent |= BT3_QUALIFY_BIT;
		} else {
			m_spanBPresent &= ~(uint16)BT3_QUALIFY_BIT;
		}
		break;
	case CHAN_GPS:
		if (ql) {
			m_gpsPresent |= BT3_QUALIFY_BIT;
		} else {
			m_gpsPresent &= ~(uint16)BT3_QUALIFY_BIT;
		}
	}
}

uint16 CFpga::GetVersion()
{
#if !TO_BE_COMPLETE
	uint8 ret;

	ret = *FPGA_VERSION;
	return (uint16)ret;
#else
	return 0;
#endif
}

void CFpga::AlignGpsPhase(int set)
{
#if !TO_BE_COMPLETE
	uint8 tmp_exr;
	uint8 ctrl;

	tmp_exr = get_exr();
	set_exr(7);
	if (set) {
		ctrl = m_fpgaBuff.ctrl3 | 2;
	} else {
		ctrl = m_fpgaBuff.ctrl3 & 0xfd;
	}
	m_pHardware->ctrl3 = ctrl;
	m_fpgaBuff.ctrl3 = ctrl;
	set_exr(tmp_exr);
#endif
}

void CFpga::GetCurrentEfcControl(uint16 *pHigh, uint16 *pLow, int *pDither, int *pCnt)
{
#if !TO_BE_COMPLETE
	*pHigh = m_high;
	*pLow = m_low;
	*pDither = m_dither;
	*pCnt = m_currentDither;
#else
#endif
}

int CFpga::GetPllUnlock()
{

	if ((m_previousStatusOr & 0x00001f00L) == 0) return -1;
	if (m_previousStatusOr & 0x100L) {
		return 0;
	}

	if (m_previousStatusOr & 0x200L) {
		return 1;
	}

	if (m_previousStatusOr & 0x400L) {
		return 2;
	}
	if (m_previousStatusOr & 0x800L) {
		return 3;
	}
	if (m_previousStatusOr & 0x1000L) {
		return 4;
	}
	return -1; // should never get here.
}

int CFpga::GetDcmUnlock()
{
#if !TO_BE_COMPLETE
	if (m_previousStatusOr & 0x800000) return 1;
	return 0;
#else
	return 0;
#endif
}

int CFpga::IsXsyncOk(void)
{
	if (m_previousStatusOr & 1) return 0;
	return 1;
}

void CFpga::SetSpanInputType(int chan, int type)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	
	switch (chan) {
	case 0:
		m_fpgaBuff.span_one_select = type;
		m_pWriteHardware->span_one_select = type;
		if (type == FPGA_SPAN_INPUT_DISABLE) {
			m_spanAPresent &= ~(uint16)BT3_ENABLE_BIT;
		} else {
			m_spanAPresent |= BT3_ENABLE_BIT;
		}
		m_spanInputType[0] = type;
		break;
	case 1:
		m_fpgaBuff.span_two_select = type;
		m_pWriteHardware->span_two_select = type;
		if (type == FPGA_SPAN_INPUT_DISABLE) {
			m_spanBPresent &= ~(uint16)BT3_ENABLE_BIT;
		} else {
			m_spanBPresent |= BT3_ENABLE_BIT;
		}
		m_spanInputType[1] = type;
		break;
	}

	set_exr(tmp_exr);
}

void CFpga::SetSpanInputSsmBit(int chan, int type)
{
	uint8 tmp_exr;

	tmp_exr = get_exr();
	set_exr(7);
	switch (chan) {
	case 0:
		m_fpgaBuff.span_one_e1_ssm &= 0xf8;
		m_fpgaBuff.span_one_e1_ssm |= (type & 0x7);
		m_pWriteHardware->span_one_e1_ssm = m_fpgaBuff.span_one_e1_ssm;
		break;
	case 1:
		m_fpgaBuff.span_two_e1_ssm &= 0xf8;
		m_fpgaBuff.span_two_e1_ssm |= (type & 0x7);
		m_pWriteHardware->span_two_e1_ssm = m_fpgaBuff.span_two_e1_ssm;
		break;
	}
	set_exr(tmp_exr);
}

// SSM table from Tom F on 7/7/2003
/*
    DS1                   E1
 QL SSM  Text Desc        SSM  Text Desc
--- ---- ---------------- ---- ----------------
  1 0x04 PRS              0x02 PRC/G.811
  2 0x08 STU              0x00 STU
  3 0x0C ST2              0x04 Type II
  4 0x0C Type I           0x04 Type I
  5 0x78 TNC              0x04 G.812 Transit
  6 0x7C ST3E             0x08 Type III
  7 0x10 ST3              0x0B Type IV
  8 0x22 SMC              0x0F G.813 Opt2
  9 0x30 DUS              0x0F DUS
*/

// From E1 SSM coding to QL
uint8 const CFpga::m_e1SSMMaping[16] = {
	2,	// 0000 QL_UNK
	10,  // 0001 QL_INV1
	1,	// 0010 QL_PRC
	10,   // 0011 QL_INV3
	3,    // 0100 QL_SSUT
	10,	// 0101 QL_INV5
	10,	// 0110 QL_INV6
	10,	// 0111 QL_INV7
	6,	// 1000 QL_SSUL // Changed from 5 to 6 according to Tom F
	10,	// 1001 QL_INV9
	10,	// 1010 QL_INV10
	7,	// 1011 QL_SEC
	10,	// 1100 QL_INV12
	10,	// 1101 QL_INV13
	10,	// 1110 QL_INV14
	9	// 1111 QL_DNU // Changed from 9 to 8 according to Tom F 
		// 7/9/2004 Changed it back from 8 to 9 according to Tom F's new table.
};

// From T1 SSM coding to QL
uint8 const CFpga::m_t1SSMMaping[256] = {
	0, // 0x0
	0, // 0x1
	0, // 0x2
	0, // 0x3
	1, // 0x4  // PRS ------------------------------------
	0, // 0x5
	0, // 0x6
	0, // 0x7
	2, // 0x8
	0, // 0x9
	0, // 0xa
	0, // 0xb
	3, // 0xc // ------------------------------------------
	0, // 0xd
	0, // 0xe
	0, // 0xf

	7, // 0x10 // Change from 6 to 7 according to Tom F
	0, // 0x11
	0, // 0x12
	0, // 0x13
	0, // 0x14
	0, // 0x15
	0, // 0x16
	0, // 0x17
	0, // 0x18
	0, // 0x19
	0, // 0x1a
	0, // 0x1b
	0, // 0x1c
	0, // 0x1d
	0, // 0x1e
	0, // 0x1f

	0, // 0x20 
	0, // 0x21
	8, // 0x22 // Changed from7 to 8 according to Tom F
	0, // 0x23
	0, // 0x24
	0, // 0x25
	0, // 0x26
	0, // 0x27
	8, // 0x28 // No show up in Tom F's table ------------------------------------
	0, // 0x29
	0, // 0x2a
	0, // 0x2b
	0, // 0x2c
	0, // 0x2d
	0, // 0x2e
	0, // 0x2f

	9, // 0x30 // ------------------------------------------- 
	0, // 0x31
	0, // 0x32
	0, // 0x33
	0, // 0x34
	0, // 0x35
	0, // 0x36
	0, // 0x37
	0, // 0x38
	0, // 0x39
	0, // 0x3a
	0, // 0x3b
	0, // 0x3c
	0, // 0x3d
	0, // 0x3e
	0, // 0x3f

	10, // 0x40  // Leave it alone. Not specified by Tom F
	0, // 0x41
	0, // 0x42
	0, // 0x43
	0, // 0x44
	0, // 0x45
	0, // 0x46
	0, // 0x47
	0, // 0x48
	0, // 0x49
	0, // 0x4a
	0, // 0x4b
	0, // 0x4c
	0, // 0x4d
	0, // 0x4e
	0, // 0x4f

	0, // 0x50
	0, // 0x51
	0, // 0x52
	0, // 0x53
	0, // 0x54
	0, // 0x55
	0, // 0x56
	0, // 0x57
	0, // 0x58
	0, // 0x59
	0, // 0x5a
	0, // 0x5b
	0, // 0x5c
	0, // 0x5d
	0, // 0x5e
	0, // 0x5f

	0, // 0x60
	0, // 0x61
	0, // 0x62
	0, // 0x63
	0, // 0x64
	0, // 0x65
	0, // 0x66
	0, // 0x67
	0, // 0x68
	0, // 0x69
	0, // 0x6a
	0, // 0x6b
	0, // 0x6c
	0, // 0x6d
	0, // 0x6e
	0, // 0x6f

	0, // 0x70
	0, // 0x71
	0, // 0x72
	0, // 0x73
	0, // 0x74
	0, // 0x75
	0, // 0x76
	0, // 0x77
	5, // 0x78  // Changed from 4 to 5 according to Tom F.
	0, // 0x79
	0, // 0x7a
	0, // 0x7b
	6, // 0x7c  // Changed from 5 to 6
	0, // 0x7d
	0, // 0x7e
	0, // 0x7f

	0, // 0x80
	0, // 0x81
	0, // 0x82
	0, // 0x83
	0, // 0x84
	0, // 0x85
	0, // 0x86
	0, // 0x87
	0, // 0x88
	0, // 0x89
	0, // 0x8a
	0, // 0x8b
	0, // 0x8c
	0, // 0x8d
	0, // 0x8e
	0, // 0x8f

	0, // 0x90
	0, // 0x91
	0, // 0x92
	0, // 0x93
	0, // 0x94
	0, // 0x95
	0, // 0x96
	0, // 0x97
	0, // 0x98
	0, // 0x99
	0, // 0x9a
	0, // 0x9b
	0, // 0x9c
	0, // 0x9d
	0, // 0x9e
	0, // 0x9f

	0, // 0xa0
	0, // 0xa1
	0, // 0xa2
	0, // 0xa3
	0, // 0xa4
	0, // 0xa5
	0, // 0xa6
	0, // 0xa7
	0, // 0xa8
	0, // 0xa9
	0, // 0xaa
	0, // 0xab
	0, // 0xac
	0, // 0xad
	0, // 0xae
	0, // 0xaf

	0, // 0xb0
	0, // 0xb1
	0, // 0xb2
	0, // 0xb3
	0, // 0xb4
	0, // 0xb5
	0, // 0xb6
	0, // 0xb7
	0, // 0xb8
	0, // 0xb9
	0, // 0xba
	0, // 0xbb
	0, // 0xbc
	0, // 0xbd
	0, // 0xbe
	0, // 0xbf

	0, // 0xc0
	0, // 0xc1
	0, // 0xc2
	0, // 0xc3
	0, // 0xc4
	0, // 0xc5
	0, // 0xc6
	0, // 0xc7
	0, // 0xc8
	0, // 0xc9
	0, // 0xca
	0, // 0xcb
	0, // 0xcc
	0, // 0xcd
	0, // 0xce
	0, // 0xcf

	0, // 0xd0
	0, // 0xd1
	0, // 0xd2
	0, // 0xd3
	0, // 0xd4
	0, // 0xd5
	0, // 0xd6
	0, // 0xd7
	0, // 0xd8
	0, // 0xd9
	0, // 0xda
	0, // 0xdb
	0, // 0xdc
	0, // 0xdd
	0, // 0xde
	0, // 0xdf

	0, // 0xe0
	0, // 0xe1
	0, // 0xe2
	0, // 0xe3
	0, // 0xe4
	0, // 0xe5
	0, // 0xe6
	0, // 0xe7
	0, // 0xe8
	0, // 0xe9
	0, // 0xea
	0, // 0xeb
	0, // 0xec
	0, // 0xed
	0, // 0xee
	0, // 0xef

	0, // 0xf0
	0, // 0xf1
	0, // 0xf2
	0, // 0xf3
	0, // 0xf4
	0, // 0xf5
	0, // 0xf6
	0, // 0xf7
	0, // 0xf8
	0, // 0xf9
	0, // 0xfa
	0, // 0xfb
	0, // 0xfc
	0, // 0xfd
	0, // 0xfe
	0 // 0xff

};


uint8 const CFpga::m_e1QLToSSMCode[17] = {
	0xe, // Invalid
	2, // QL = 1
	0, // QL = 2
	4,  // QL = 3
	4,	// QL = 4
	4, // QL = 5	// Changed from 8 to 4 according to Tom F
	8, // QL = 6  
	0xb, // QL = 7
	0xf, // QL = 8
	0xf, // QL = 9
	0xf, // QL = 10
	0xf, // QL = 11
	0xf, // QL = 12
	0xf, // QL = 13 
	0xf, // QL = 14
	0xf, // QL = 15
	0xf, // QL = 16
};

uint8 const CFpga::m_t1QLToSSMCode[17] = {
	0x30, // Invalid
	0x4, // QL = 1
	0x8, // QL = 2
	0xc, // QL = 3
	0xc, // QL = 4  // Changed from 0x78 to 0xC accroding to Tom F.
	0x78, // QL = 5 // Changed from 0x7c to 0x78 according to Tom F.
	0x7c, // QL = 6 // Changed from 0x10 to 0x7C according to Tom F.
	0x10, // QL = 7 // Changed from 0x22 to 0x10 according to Tom F.
	0x22, // QL = 8 // Changed from 0x28 to 0x22 according to Tom F.
	0x30, // QL = 9
	0x40, // QL = 10
	0x30,// QL = 11
	0x30,// QL = 12
	0x30,// QL = 13
	0x30,// QL = 14
	0x30,// QL = 15
	0x30 // QL = 16
};

void CFpga::CollectSsm25ms(void)
{
	int chan;
	uint8 stat;
	uint8 level;

	for (chan = 0; chan < 2; chan++) {
		if (m_spanInputType[chan] == FPGA_SPAN_INPUT_E1) {
			switch (chan) {
			case 0:
				stat = m_pReadHardware->span_one_e1_rx_status;
				break;
			case 1:
				stat = m_pReadHardware->span_two_e1_rx_status;
				break;
			}
			if ((stat & 0xe0) != 0x60) {
				m_spanInputSsm[chan][0]++;			
				continue; // no ssm
			}
			level = m_e1SSMMaping[stat & 0xf] & 0xf;
			m_spanInputSsm[chan][level]++;
		} else if (m_spanInputType[chan] == FPGA_SPAN_INPUT_T1) {
			switch (chan) {
			case 0:
				stat = m_pReadHardware->span_one_t1_rx_status;
				level = m_pReadHardware->span_one_t1_ssm;
				break;
			case 1:
				stat = m_pReadHardware->span_two_t1_rx_status;
				level = m_pReadHardware->span_two_t1_ssm;
				break;
			}
			if ((stat & 0x1e) != 0xc) {
				m_spanInputSsm[chan][0]++;			
				continue;
			}
			level = m_t1SSMMaping[level] & 0xf;
			m_spanInputSsm[chan][level]++;
		} else {
			m_spanInputSsm[chan][0] = 40;
		}
	}
}

void CFpga::CollectSsm1S(void)
{
	int chan;
	int ssm;
	int level;
	for (chan = 0; chan < 2; chan++) {
		ssm = 10;
		for (level = 0; level <= 10; level++) {
			if (m_spanInputSsm[chan][level] >= 28) {
				ssm = level;
				break;
			}
		}
		m_spanInputSsmSec[chan] = ssm;
	}
	memset(m_spanInputSsm, 0, sizeof(m_spanInputSsm));
}

int CFpga::GetSpanInputSsmQL(int chan)
{    
	return m_spanInputSsmSec[chan];
}

void CFpga::SetOutputQL(int level)
{
	uint8 tmp_exr;

	level &= 0xf;

	tmp_exr = get_exr();
	set_exr(7);

	m_pWriteHardware->output_t1_ssm = m_t1QLToSSMCode[level];
	m_pWriteHardware->output_e1_ssm = m_e1QLToSSMCode[level];

	set_exr(tmp_exr);
}

uint32 CFpga::GetAdjustedGpsSecond(void)
{
	return m_gpsSecond - (int32)m_leapSecond;
}

uint32 CFpga::GetGpsSec()
{
	return m_gpsSecond;
}
