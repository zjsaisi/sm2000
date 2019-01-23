/***********************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/14 00:02:39PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_int.c_v  $
$Modtime:   06 Feb 1999 12:37:52  $
$Revision: 1.6 $


                            CLIPPER SOFTWARE
                                 
                                BT3_INT.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module call at start at initialization to initialize BT3 tasks.

MODIFICATION HISTORY:

$Log: bt3_int.c  $
Revision 1.6 2010/10/14 00:02:39PDT Jining Yang (jyang) 
Merged from 1.5.1.1.
Revision 1.5.1.1 2010/08/04 23:24:16PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.5 2009/06/11 10:53:35PDT Zheng Miao (zmiao) 
George's change
Revision 1.4 2009/06/01 09:10:28PDT zmiao 
Change pullin range according OSC type.
Revision 1.3 2008/04/01 07:40:30PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.2 2008/01/29 11:16:18PST zmiao 
dev
Revision 1.1 2007/12/06 11:41:06PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.3 2006/05/03 12:45:11PDT gzampet 
Updated to support the BT63 command and general startup issues in the lower gears
Revision 1.2 2006/04/28 07:24:51PDT gzampet 
added extended phase slew control
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.12 2004/09/03 09:51:39PDT gzampet 

Revision 1.11  2004/08/24 21:46:31Z  gzampet
Revision 1.10  2004/07/29 23:08:19Z  gzampet
Revision 1.9  2004/07/09 18:28:47Z  gzampet
Added separate CTS Measurement channel
Revision 1.8  2004/07/01 13:12:08Z  gzampet
Revision 1.7  2004/06/23 20:17:59Z  gzampet
full merge
Revision 1.6  2004/06/23 15:15:25Z  gzampet
Baseline GPS BesTime Version
Revision 1.5  2003/06/05 17:03:20Z  gzampet
Revision 1.4  2003/05/28 18:43:24Z  gzampet
Add initial call to smart_icw refresh in cal init
Revision 1.3  2003/04/19 00:11:01Z  gzampet
Revision 1.2  2003/04/15 21:16:01Z  gzampet
Revision 1.5  2002/09/06 00:29:57Z  gzampet
Change default provisioning of the span B input to be
unprovisioned to prevent monitoring until activated through
the craft interface
Revision 1.4  2002/08/16 19:10:19Z  gzampet
Revision 1.3  2002/06/19 20:49:23Z  kho
Added RTHC
Revision 1.2  2002/05/31 17:59:48Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
 * 
 *    Rev 1.0   06 Feb 1999 15:41:42   S.Scott
 * TS3000 initial release.

***************************************************************************/

/******************** INCLUDE FILES ************************/
//#include "proj_inc.h"
/******************** LOCAL DEFINES ************************/

/************ PROTOTYPES EXTERNAL FUNCTIONS ****************/

/************ PROTOTYPES INTERNAL FUNCTIONS ****************/

/******************** EXTERNAL DATA ************************/

/******************** INTERNAL DATA ************************/

/************************ TABLE ****************************/
       
/************************ CODE *****************************/


/********************************************************************
	Module call at start at initialization
	to initialize tasks
*******************************************************************/
#include "bt3_def.h"
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#endif      
/******** global data structures *********/
#include "bt3_dat.h"   /*main file for global data declarations*/
#include "bt3_msg.h"   /*main file for message buffer templates*/
#include "bt3_shl.h"   /*main file for message shell templates*/  
#include "BT3Support.h" 
#if(TARGET==CLPR)
struct Shared_Memory_Interface smi;
extern struct Span_Meas_Data  SPANA_Fifo, SPANB_Fifo,PRS_Fifo,GPS_Fifo;
struct Remote_Oscillator_Meas_Data ROA_Fifo,ROB_Fifo;
struct Temperature_Meas_Data TP_Fifo;
struct Plevel_Meas_Data  PL_Fifo;
struct Alevel_Meas_Data  AL_Fifo;
struct Calibration_Data  Cal_Data;
#else
extern struct Shared_Memory_Interface smi;
extern struct Span_Meas_Data SPANA_Fifo, SPANB_Fifo;
extern struct Remote_Oscillator_Meas_Data ROA_Fifo,ROB_Fifo;
extern struct Temperature_Meas_Data TP_Fifo;
extern struct Plevel_Meas_Data  PL_Fifo;
extern struct Alevel_Meas_Data  AL_Fifo;
extern struct Calibration_Data  Cal_Data;
#endif
extern struct Span_Meas_Data *pmda, *pmdb, *pmdc,*pmdd, *pmdg;
extern struct Remote_Oscillator_Meas_Data *prma,*prmb;
extern struct Temperature_Meas_Data *ptmd;
extern struct Plevel_Meas_Data      *ppmd;
extern struct Alevel_Meas_Data      *pamd;

extern struct Temperature_Calibration_Data_Record TCDR[MAXTEMPRECORDS];
extern struct Temperature_Compensation Tcomp;
extern struct Phase_Compensation Pcomp;
extern struct GPS_Measurement_Data GM;
extern struct Threshold_Management TM;
extern struct Dual_Input_FLL_Control_System DloopSA,DloopSB,DloopRA,DloopRB,*ploop;
extern struct Single_Input_FLL_Control_System Lloop,*sloop;
#if REMOTE
extern struct Single_Input_FLL_Control_System Rloop;
#endif
extern struct Input_Freq_One_Minute_Data IFOM;
extern struct Adaptive_Temperature_Compensation ATC;
extern struct Holdover_Management  HMSA,HMSB,HMRA,HMRB,HMS,HMR,*phm; /* Holdover management for both E1 inputs */
extern struct Holdover_LSF  HLSF;
extern struct Tracking_Stats TrkStats;
extern struct ASM_Management ASMM; /* Adaptive state machine for both E1 inputs */
extern struct Status_Registers SReg;
extern struct Ensembling_Control_Data  ECD;
extern struct Factory_Measurement_Data  FMD;
extern unsigned short GPS_Valid;
extern unsigned short GPS_Drift_Count;
extern unsigned char NCO_Slew; /*counter inhibits nco slew control at startup*/
extern double NCO_Smooth; /*smoothed nco correction to prevent carrier loss in GPS
								  Costas Loop*/
#if PRINTLOG
unsigned long ptime; /* print log time stamp in cumulative minutes*/
#endif
unsigned short  pcount; /*manage rate of print logging*/
unsigned short printctrl;
/* add global flag to control oscillator type*/
struct LO_TYPE_BUILD_INFO LTBI;
/* add PPS control flags ***/
extern unsigned short OnePPSJamFlag;
extern	double Yro_tc;
extern	double smoothl;
extern	double smooths;
extern double Temp_Est_tsk; /*task file copy of Temp_Est variable*/
extern double YroavgA_tsk, YroavgB_tsk;
/***** messaging related variables **************************************/
extern unsigned char msg_count1sec;  /*controls one minute messaging from 1 second tasks*/
extern unsigned char msg_count250ms; /*control one second message process from estimation*/
extern unsigned short spa_reg_local, spb_reg_local; /*local span event registers*/
extern struct MSG_Span_FLL MSG_Span_FLL_A, MSG_Span_FLL_B;
extern unsigned short roa_reg_local, rob_reg_local; /*local span event registers*/
extern struct MSG_EST_ATC MSG_EST_ATC_A, MSG_EST_ATC_B;
extern unsigned short tp_reg_local; /*local temperature event registers*/
extern unsigned short pl_reg_local; /*local 60 MHz PLL event registers*/
extern unsigned short at_reg_local; /*local antenna current event registers*/
extern struct MSG_EST_SRPT MSG_EST_SRPT_A,MSG_EST_SRPT_B;
extern struct MSG_RO_FLL MSG_RO_FLL_A, MSG_RO_FLL_B;
extern struct MSG_FLL_EST MSG_FLL_EST_A;
extern struct MSG_NCOUP_EST MSG_NCOUP_EST_A;
extern struct MSG_PCOMP_EST MSG_PCOMP_EST_A;
extern struct MSG_EST_ASM MSG_EST_ASM_A, MSG_EST_ASM_B;
extern struct MSG_FLL_HOLDOVER MSG_FLL_HOLDOVER_A, MSG_FLL_HOLDOVER_B;
extern struct MSG_HOLDOVER_FLL MSG_HOLDOVER_FLL_A;
extern struct MSG_HOLDOVER_NCOUP MSG_HOLDOVER_NCOUP_A;
extern struct MSG_FLL_TCH  MSG_FLL_TCH_A, MSG_FLL_TCH_B;
extern struct MSG_SR_TCH MSG_SR_TCH_A, MSG_SR_TCH_B;
extern struct MSG_TCH_NCOUP MSG_TCH_NCOUP_A;
extern struct MSG_FLL_CAL MSG_FLL_CAL_A, MSG_FLL_CAL_B;
extern struct MSG_CAL_EST MSG_CAL_EST_A;
extern struct MSG_CAL_FLL MSG_CAL_FLL_A;

/******************** External BT3 Shell Messages ***************/
extern struct MSG_Control_Loop_Section MSG_CLS[CLOOPS];
extern struct MSG_Control_Loop_Common MSG_CLC;
extern struct MSG_Alarm_Status_Report MSG_ASR;
extern struct MSG_Daily_Holdover_Report MSG_DHR;
extern struct MSG_Monthly_Holdover_Report MSG_MHR;
extern struct MSG_PSM_Report MSG_PSM;
extern struct MSG_Timing_Status_Report MSG_TSR;
extern struct MSG_ECW_Command  MSG_ECW_C;
extern struct MSG_ECW_Report   MSG_ECW_R;
extern struct MSG_RCW_Command  MSG_RCW_C;
extern struct MSG_RCW_Report   MSG_RCW_R;
extern struct MSG_PMASK_Command  MSG_PMASK_C;
extern struct MSG_PMASK_Report   MSG_PMASK_R;
extern struct MSG_QMASK_Command  MSG_QMASK_C;
extern struct MSG_QMASK_Report   MSG_QMASK_R;

/***** function templates *********/
extern void init_stat(double);
extern void NCO_Convert(double);
extern void NCO_Control(void); 
extern int  Read_CCKB(void);
void init_cal(void);
void init_build(void);
void BT3_MSG_Init(void);
void Init_Shared_Mem_Int(void); 
void BesTime_init(int isRubidium);
void refresh_smart_icw(void);

extern struct MPLL_Meas_Data mpll;

double g_freqMax = SFREQMAX_XO;

/**************************************************************************
  Note all variables that are zero by default do not need to
  be explicitly set to zero in the XR5 environment. These lines
  are not compiled in this case to save space.
************************************************************************/
void Init_Tasks()
{
		int i,j;
		printctrl=TRUE;
		#if (!PCSIM)
		 printctrl=FALSE;
		#endif
		/* set build related parameters*/
      init_build();
		init_stat(1.0); /*note 1 second update used in standard BT3 engine*/
		pmda=&(SPANA_Fifo);
		pmdb=&(SPANB_Fifo);
		pmdc=&(PRS_Fifo);  
		pmdd=&(GPS_Fifo);
		prma=&(ROA_Fifo);
		prmb=&(ROB_Fifo);
		ptmd=&(TP_Fifo);
		ppmd=&(PL_Fifo);
		pamd=&(AL_Fifo);
		/**************************Start all zero FIFO data *******************/
#if PCSIM
		pmda->Nwrite=0;
		pmdb->Nwrite=0;
		pmdc->Nwrite=0;
		pmdd->Nwrite=0;
		prma->Nwrite=0;
		prmb->Nwrite=0;
		ptmd->Nwrite=0;
		ppmd->Nwrite=0;
		pamd->Nwrite=0;
		pmda->Nread=0;
		pmdb->Nread=0;
		pmdc->Nread=0;
		pmdd->Nread=0;
		prma->Nread=0;
		prmb->Nread=0;
		ptmd->Nread=0;
		ppmd->Nread=0;
		pamd->Nread=0;
		pmda->Ndata=0;
		pmdb->Ndata=0;
		pmdc->Ndata=0;
		pmdd->Ndata=0;
		prma->Ndata=0;
		prmb->Ndata=0;
		ptmd->Ndata=0;
		ppmd->Ndata=0;
		pamd->Ndata=0;
		pmda->Xsp=0;
		pmdb->Xsp=0;
		pmdc->Xsp=0;
		pmdd->Xsp=0;
		prma->Xro=0;
		prmb->Xro=0;
		ptmd->Xtp=0;
		ppmd->Xpl=0;
		pamd->Xal=0;
		pmda->Ysp=0;
		pmdb->Ysp=0;
		pmdc->Ysp=0;
		pmdd->Ysp=0;
		prma->Yro=0;
		prmb->Yro=0;
		ptmd->Ytp=0;
		ppmd->Ypl=0;
		pamd->Yal=0;
		prma->Yroold=0;
		prmb->Yroold=0;
		pmda->Yspavg=0;
		pmdb->Yspavg=0;
		pmdc->Yspavg=0;
		pmdd->Yspavg=0;
		prma->Yroavg=0;
		prmb->Yroavg=0;
		ptmd->Ytpavg=0;
		ptmd->Temp_Est=0.0;
		ppmd->Plev_Est=0.0;
		pamd->Alev_Est=0.0;
		pmda->Delta_F1=0;
		pmdb->Delta_F1=0;
		pmdc->Delta_F1=0;
		pmdd->Delta_F1=0;
		prma->Delta_F1=0;
		prmb->Delta_F1=0;
		ptmd->Delta_F1=0;
		ppmd->Delta_F1=0;
		pamd->Delta_F1=0;
		prma->Delta_F2=0;
		prmb->Delta_F2=0;
		ptmd->Delta_F2=0;
		ppmd->Delta_F2=0;
		pamd->Delta_F2=0;
		pmda->Delta_F1_Sum=0;
		pmdb->Delta_F1_Sum=0;
		pmdc->Delta_F1_Sum=0;
		pmdd->Delta_F1_Sum=0;
		prma->Delta_F1_Sum=0;
		prmb->Delta_F1_Sum=0;
		ptmd->Delta_F1_Sum=0;
		ppmd->Delta_F1_Sum=0;
		pamd->Delta_F1_Sum=0;
		pmda->M1write=0;
		for(i=0;i<MASIZE1;i++) pmda->MAbuf1[i]=0;
		pmdb->M1write=0;
		for(i=0;i<MASIZE1;i++) pmdb->MAbuf1[i]=0;
		pmdc->M1write=0;
		for(i=0;i<MASIZE1;i++) pmdc->MAbuf1[i]=0;     
		pmdd->M1write=0;
		for(i=0;i<MASIZE1;i++) pmdd->MAbuf1[i]=0;
		
		pmda->M2write=0;
		pmdb->M2write=0;
		pmdc->M2write=0;
		pmdd->M2write=0;
		pmda->MT1write=0;
		pmdb->MT1write=0;
		pmdc->MT1write=0;
		pmdd->MT1write=0;
		prma->MTwrite=0;
		for(i=0;i<MATSIZE;i++) prma->MATbuf[i]=0;
		prmb->MTwrite=0;
		for(i=0;i<MATSIZE;i++) prmb->MATbuf[i]=0;
		pmda->Delta_F1_Cnt=0;
		pmdb->Delta_F1_Cnt=0;
		pmdc->Delta_F1_Cnt=0;
		pmdd->Delta_F1_Cnt=0;
		prma->Delta_F1_Cnt=0;
		prmb->Delta_F1_Cnt=0;
		ptmd->Delta_F1_Cnt=0;
		ppmd->Delta_F1_Cnt=0;
		pamd->Delta_F1_Cnt=0;
		ppmd->Plevel_Set=0; /*assume low power level for default*/
#endif
		/*********************** END ALL ZERO FIFO DATA ****************/

		pmda->Sfilt1=(SDRIFTCHECK*TAUZERO)/(SFREQTAU); pmda->Sfilt2=1.0-pmda->Sfilt1;
		pmdb->Sfilt1=(SDRIFTCHECK*TAUZERO)/(SFREQTAU); pmdb->Sfilt2=1.0-pmdb->Sfilt1;
		pmdc->Sfilt1=(SDRIFTCHECK*TAUZERO)/(SFREQTAU); pmdc->Sfilt2=1.0-pmdc->Sfilt1;
		pmdd->Sfilt1=(SDRIFTCHECK*TAUZERO)/(SFREQTAU); pmdd->Sfilt2=1.0-pmdd->Sfilt1;
		pmda->Delta_F2=128*MASIZE2;
		pmdb->Delta_F2=128*MASIZE2;
		pmdc->Delta_F2=128*MASIZE2;
		pmdd->Delta_F2=128*MASIZE2;
		for(i=0;i<MASIZE2;i++)
		{
			 pmda->MAbuf2[i]=128L;
			 pmdb->MAbuf2[i]=128L;
			 pmdc->MAbuf2[i]=128L;
			 pmdd->MAbuf2[i]=128L;
		}
		for(i=0;i<MATSIZE1;i++)
		{
			pmda->MATbuf1[i]=4;
			pmdb->MATbuf1[i]=4;
			pmdc->MATbuf1[i]=4;
			pmdd->MATbuf1[i]=4;
		}
		pmda->Tdelta_F1=MATSIZE1*4;
		pmdb->Tdelta_F1=MATSIZE1*4;
		pmdc->Tdelta_F1=MATSIZE1*4;
		pmdd->Tdelta_F1=MATSIZE1*4;
		pmda->Span_Type=SPANTYPEA;
		pmdb->Span_Type=SPANTYPEA;
		pmdc->Span_Type=SPANTYPEA;
		pmdd->Span_Type=SPANTYPEA;
		if(pmda->Span_Type==T1)
		{
		  pmda->DeltaBias= (T1DELTABIAS);  pmda->DeltaFix = (T1DELTAFIX);
		}
		else
		{
		  pmda->DeltaBias= (E1DELTABIAS);  pmda->DeltaFix = (E1DELTAFIX);
		}
		if(pmdb->Span_Type==T1)
		{
		  pmdb->DeltaBias= (T1DELTABIAS);  pmdb->DeltaFix = (T1DELTAFIX);
		}
		else
		{
		  pmdb->DeltaBias= (E1DELTABIAS);  pmdb->DeltaFix = (E1DELTAFIX);
		}
		if(pmdc->Span_Type==T1)
		{
		  pmdc->DeltaBias= (T1DELTABIAS);  pmdc->DeltaFix = (T1DELTAFIX);
		}
		else
		{
		  pmdc->DeltaBias= (E1DELTABIAS);  pmdc->DeltaFix = (E1DELTAFIX);
		} 
		if(pmdd->Span_Type==T1)
		{
		  pmdd->DeltaBias= (T1DELTABIAS);  pmdd->DeltaFix = (T1DELTAFIX);
		}
		else
		{
		  pmdd->DeltaBias= (E1DELTABIAS);  pmdd->DeltaFix = (E1DELTAFIX);
		}
		

		prma->RO_Jitter=FALSE;
		prmb->RO_Jitter=FALSE;
		ptmd->TP_Jitter=FALSE;
		ppmd->PL_Jitter=FALSE;
		pamd->AL_Jitter=FALSE;
		pmda->SPAN_Present=FALSE;  /*Must be set true as needed in simulator (both bytes) */
		pmdb->SPAN_Present=FALSE;
		pmdc->SPAN_Present=FALSE;
		pmdd->SPAN_Present=FALSE;
		prma->RO_Present=  FALSE;   prmb->RO_Present=FALSE;

#if (!PCSIM)
		pmda->SPAN_Present=0x101;  /*Must be validated by embedded system */
		pmdb->SPAN_Present=0x101;
		pmdc->SPAN_Present=0x101;
		pmdd->SPAN_Present=0x101;
//		prma->RO_Present=  0x101;   prmb->RO_Present=0x101; GPZ Disable for CSM
#endif

		pmda->SdriftCheck=0;	pmda->SdriftCount=0;
		pmdb->SdriftCheck=0;	pmdb->SdriftCount=0;
		pmdc->SdriftCheck=0;	pmdc->SdriftCount=0;
		pmdd->SdriftCheck=0;	pmdd->SdriftCount=15;
		prma->ROdriftCheck=0;prmb->ROdriftCheck=0;
		prma->ROdriftCount=3;prmb->ROdriftCount=3;
		ptmd->TPslewCount=0; ptmd->TPslewCheck=0; ptmd->TPSlewUpdate=FALSE;

		/***** Note FALSE for flags means event asserted *****/
		pmda->SPANJitterFlag=FALSE;pmdb->SPANJitterFlag=FALSE;pmdc->SPANJitterFlag=FALSE;pmdd->SPANJitterFlag=FALSE;
		pmda->SPANDriftFlag= FALSE;pmdb->SPANDriftFlag =FALSE;pmdc->SPANDriftFlag =FALSE;pmdd->SPANDriftFlag =FALSE;
		pmda->SPANFreqFlag  =FALSE;pmdb->SPANFreqFlag  =FALSE;pmdc->SPANFreqFlag  =FALSE;pmdd->SPANFreqFlag  =FALSE;
		pmda->SPANPhaseFlag =FALSE;pmdb->SPANPhaseFlag =FALSE;pmdc->SPANPhaseFlag =FALSE;pmdd->SPANPhaseFlag =FALSE;
		prma->ROJitterFlag=FALSE;   prmb->ROJitterFlag=FALSE;
		prma->RODriftFlag=FALSE;	 prmb->RODriftFlag =FALSE;
		prma->ROFreqFlag =FALSE;	 prmb->ROFreqFlag  =FALSE;
		prma->ROPhaseFlag=FALSE;	 prmb->ROPhaseFlag =FALSE;
		ptmd->TPJitterFlag=FALSE; ptmd->TPStepFlag=FALSE; ptmd->TPSlewFlag=FALSE;
		ppmd->PLJitterFlag=TRUE; ppmd->PLStepFlag=TRUE; ppmd->PLRangeFlag=TRUE;
		pamd->ALJitterFlag=FALSE; pamd->ALStepFlag=FALSE; pamd->ALRangeFlag=FALSE;

		ptmd->TP_Present=TRUE;
		ppmd->PL_Present=TRUE;  /*normally assume true for all BT3 applications*/
		pamd->AL_Present=FALSE; /*normally assume false embedded system should
										 manage based on if output power is on*/
		pmda->Sphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		pmda->Sfreq_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		pmda->Sdrift_bucket=0;                         /* leaky bucket for excessive drift drift */
		pmdb->Sphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		pmdb->Sfreq_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		pmdb->Sdrift_bucket=0;                        /* leaky bucket for excessive drift drift */
		pmdc->Sphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		pmdc->Sfreq_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		pmdc->Sdrift_bucket=0;                        /* leaky bucket for excessive drift drift */
		pmdd->Sphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		pmdd->Sfreq_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		pmdd->Sdrift_bucket=0;                       /* leaky bucket for excessive drift drift */
		
		prma->Rphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		prmb->Rphase_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive phase offset */
		ptmd->TPstep_bucket=LBUCKETTHRES + LBUCKETHYS;  /*leaky bucket for excessive Temp Step */
		ppmd->PLstep_bucket=LBUCKETTHRES + LBUCKETHYS;  /*leaky bucket for excessive plev Step */
		pamd->ALstep_bucket=LBUCKETTHRES + LBUCKETHYS;  /*leaky bucket for excessive plev Step */
		prma->Rfreq_bucket=LBUCKETTHRES + LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		prmb->Rfreq_bucket=LBUCKETTHRES + LBUCKETHYS;  /*leaky bucket for excessive freq offset */
		ptmd->TPslew_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for excessive Temp Slew */
//		ptmd->TPslew_bucket=LBUCKETMAX;  /*disable TP (kjh) */
		ppmd->PLrange_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for power level range alm*/
		pamd->ALrange_bucket=LBUCKETTHRES - LBUCKETHYS;  /*leaky bucket for power level range alm*/
		prma->Rdrift_bucket= 200; /* leaky bucket for excessive freq drift */
		prmb->Rdrift_bucket= 200; /* leaky bucket for excessive freq drift */
		prma->Jit_Bucket=LBUCKETTHRES - LBUCKETHYS;
		prmb->Jit_Bucket=LBUCKETTHRES - LBUCKETHYS;
		ptmd->Jit_Bucket=LBUCKETTHRES - LBUCKETHYS;
		ppmd->Jit_Bucket=LBUCKETTHRES - LBUCKETHYS;
		pamd->Jit_Bucket=LBUCKETTHRES - LBUCKETHYS;

		pmda->SPAN_Valid=FALSE;
		pmdb->SPAN_Valid=FALSE;
		pmdc->SPAN_Valid=FALSE;
		pmdd->SPAN_Valid=FALSE;
		prma->RO_Valid=FALSE;
		prmb->RO_Valid=FALSE;
		ptmd->TP_Valid=FALSE;
		ppmd->PL_Valid=FALSE;
		pamd->AL_Valid=FALSE;
#if PCSIM
		GM.Ygps=0.0;
		GM.GPS_freq_prev=0.0;
      GM.GPS_clock_bias=0.0;
		GM.GPS_prev=0.0;
		GM.Gfreq_bucket=0;
		GM.Gdrift_bucket=0;
		GM.inter_cbias=0.0;
		GM.inter_inc=0.0;
		GM.gpssec=0L;
		GM.uptime=0;
#endif
		for(i=1;i<CLOOPS;i++)
		{
		  if(i==SPA_LOOP)
		  {
			  ploop= &DloopSA;
			  phm= &HMSA;
		  }
		  else if(i==SPB_LOOP)
		  {
			  ploop= &DloopSB;
			  phm= &HMSB;
		  }    
		  else if(i==ROA_LOOP)
		  {
			  ploop= &DloopRA;
			  phm= &HMRA;
		  }
		  else if(i==ROB_LOOP)
		  {
			  ploop= &DloopRB;
			  phm= &HMRA;
		  }		  
		  /************* All Zero Control Loop  and Holdover Data ****************/
		  #if PCSIM
		  ploop->Prim_Prop_State=0.0;
		  ploop->Prim_Int_State=0.0;
		  ploop->Prim_Cor_State=0.0;
		  ploop->GPS_Prop_State=0.0;
		  ploop->NCO_Cor_State=0.0;
		  ploop->Prim_Freq_Pred=0.0;
		  for(j=0;j<PRIMDELAY;j++) {
			  ploop->Yprim_Buf[j]=0.0; /*Buffer of last n primary loop inputs*/
			  ploop->Yfback_Buf[j]=0.0;
		  }
		  for(j=0;j<GPSDELAY;j++) {
			  ploop->Ygps_Buf[j]=0.0; /*Buffer of last n primary loop inputs*/
		  }
		  ploop->minute_cnt=0;
		  ploop->shift_cnt=0;
		  ploop->Ygps_scal1=0.0;
		  ploop->Ygps_scal2=0.0;
		  ploop->Cal_Mode=0;
		  phm->Cur_Hour=0;
		  phm->Cur_Day=0;
		  phm->Nsum_Hour=0;
		  phm->Nsum_Day=0;
		  phm->Predict_Err24Hr=0.0;
		  phm->Freq_Sum_Daily=0.0;
		  phm->Var_Sum_Daily=0.0;
		  phm->Long_Term_Drift=0.0;
		  phm->Drift_Sum_Weekly=0.0;
		  phm->Osc_Freq_State=0.0;

		  #endif
		  /***************END ALL ZERO DATA ************************************/
		  phm->Restart=TRUE;
		  if(i<2)
		  {
			 ploop->Prop_Prim_Tau=DUAL_SP_PROP_PRIM_TAU;
			 ploop->Int_Prim_Tau=DUAL_SP_INT_PRIM_TAU;
			 ploop->Dint_Prim_Tau=DUAL_SP_DINT_PRIM_TAU;
			 ploop->Prop_Gps_Tau=DUAL_SP_CAL_TAU;
			 ploop->Gcalf1=1.0/(DUAL_SP_CAL_TAU);
			 ploop->Gcalf2=1.0-ploop->Gcalf1;
		  }
		  else
		  {
			 ploop->Prop_Prim_Tau=DUAL_RO_PROP_PRIM_TAU;
			 ploop->Int_Prim_Tau=DUAL_RO_INT_PRIM_TAU;
			 ploop->Dint_Prim_Tau=DUAL_RO_DINT_PRIM_TAU;
			 ploop->Prop_Gps_Tau=DUAL_RO_CAL_TAU;
			 ploop->Gcalf1=1.0/(DUAL_RO_CAL_TAU);
			 ploop->Gcalf2=1.0-ploop->Gcalf1;
		  }
		  ploop->Prim_Skip=180;  /*allow two minutes of warmup*/
		  ploop->Recalibrate=FALSE;
		  ploop->Start_Recal=TRUE; /* used to indicate unique start up state*/
		  ploop->jamloop=FALSE;
	 }
		  sloop= &Lloop;
		  /**************** All Zero Loop Data *************/
		  #if PCSIM
		  sloop->Prop_State=0.0;
		  sloop->Int_State=0.0;
		  sloop->Dint_State=0.0;
		  sloop->NCO_Cor_State=0.0;
		  sloop->Freq_Err_Est=0.0;
		  sloop->LO_Freq_Pred=0.0;
		  sloop->Freq_Err_Sum=0.0;
		  sloop->Freq_Sum_Cnt=0;
		  sloop->NCO_Cor_State=0.0;
		  sloop->Sprop1=0.0;
		  sloop->Sprop2=0.0;
		  for(j=0;j<GPSDELAY;j++) {
			  sloop->Ygps_Buf[j]=0.0; /*Buffer of last n gps loop inputs*/
		  }
		  sloop->Sprop_prev=0.0;
		  sloop->min_cnt=0;
		  sloop->minute_cnt=0;
		  sloop->shift_cnt=0;
		  #endif
		  /********** End All Zero Local Loop Data ***********/
		  sloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_accel;
		  sloop->Int_Tau=MAX_INT_TC;
		  sloop->Dint_Tau=LTBI.single_dint_tau;
		  sloop->Propf1=LTBI.single_accel/LTBI.single_prop_tau;
		  sloop->Propf2=1.0-sloop->Propf1;
		  sloop->Intf1=60.0/LTBI.single_int_tau;
		  sloop->Intf2=1.0-sloop->Intf1;
		  sloop->LO_weight=1.0; /* ensemble weight contribution of Local Oscillator */
		  sloop->Recalibrate=TRUE;
		  sloop->jamloop=FALSE;
		  sloop->Cal_Mode=0;  
		  sloop->Trace_Skip=0;
		  #if REMOTE
		  sloop= &Rloop;
		  sloop->Prop_Tau=LTBI.single_prop_tau;
		  sloop->Int_Tau=LTBI.single_int_tau;
		  sloop->Dint_Tau=LTBI.single_dint_tau;
		  sloop->Prop_State=0.0;
		  sloop->Int_State=0.0;
		  sloop->Dint_State=0.0;
		  sloop->NCO_Cor_State=0.0;
		  sloop->Freq_Err_Est=0.0;
		  sloop->LO_Freq_Pred=0.0;
		  sloop->Freq_Err_Sum=0.0;
		  sloop->Freq_Sum_Cnt=0;
		  sloop->NCO_Cor_State=0.0;
		  sloop->LO_weight=0.5; /* ensemble weight contribution of Local Oscillator */
		  sloop->Recalibrate=TRUE;
		  sloop->minute_cnt=0;
		  sloop->shift_cnt=0;
		  sloop->jamloop=FALSE;
		  sloop->Cal_Mode=0;  
		  sloop->Trace_Skip=0;
		  #endif
		  #if PCSIM
		  /******** All Data for Input Frequency One Minute Structure is zero***/
		  IFOM.XGPS=0.0; IFOM.XSPA=0.0; IFOM.XSPB=0.0; IFOM.XROA=0.0; IFOM.XROB=0.0;
		  IFOM.XGPS_cnt=0; IFOM.XSPA_cnt=0; IFOM.XSPB_cnt=0;
		  IFOM.XROA_cnt=0; IFOM.XROB_cnt=0;
		  /******** All Data for Factory Measurement Data Structure is zero***/
		  FMD.YROA_Smooth=0.0;
		  #endif
	  /*************** All Zero  Holdover Data****/
	  #if PCSIM
		  phm= &HMS;
		  phm->Cur_Hour=0;
		  phm->Cur_Day=0;
		  phm->Nsum_Hour=0;
		  phm->Nsum_Day=0;
		  phm->FreqSumCnt=0;
		  phm->Predict_Err24Hr=0.0;
		  phm->Freq_Sum_Daily=0.0;
		  phm->Var_Sum_Daily=0.0;
		  phm->Long_Term_Drift=0.0;
		  phm->Drift_Sum_Weekly=0.0;
		  phm->Osc_Freq_State=0.0;
		  phm->FreqSum=0.0;

		  phm= &HMR;
		  phm->Cur_Hour=0;
		  phm->Cur_Day=0;
		  phm->Nsum_Hour=0;
		  phm->Nsum_Day=0;
		  phm->FreqSumCnt=0;
		  phm->Predict_Err24Hr=0.0;
		  phm->Freq_Sum_Daily=0.0;
		  phm->Var_Sum_Daily=0.0;
		  phm->Long_Term_Drift=0.0;
		  phm->Drift_Sum_Weekly=0.0;
		  phm->Osc_Freq_State=0.0;
		  phm->FreqSum=0.0;
	 for (i=0;i<24;i++)
	 {
		  HMS.Daily_Freq_Log[i]=0.0;
		  HMS.Holdover_Base[i]=0.0;
		  HMS.Holdover_Drift[i]=0.0;
		  HMS.Prediction_Var24[i]=0.0;
		  HMR.Daily_Freq_Log[i]=0.0;
		  HMR.Holdover_Base[i]=0.0;
		  HMR.Holdover_Drift[i]=0.0;
		  HMR.Prediction_Var24[i]=0.0;

		  HMSA.Daily_Freq_Log[i]=0.0;
		  HMSA.Holdover_Base[i] =0.0;
		  HMSA.Holdover_Drift[i]=0.0;
		  HMSA.Prediction_Var24[i]=0.0;

		  HMSB.Daily_Freq_Log[i]=0.0;
		  HMSB.Holdover_Base[i]=0.0;
		  HMSB.Holdover_Drift[i]=0.0;
		  HMSB.Prediction_Var24[i]=0.0;    
		  
		  HMRA.Daily_Freq_Log[i]=0.0;
		  HMRA.Holdover_Base[i] =0.0;
		  HMRA.Holdover_Drift[i]=0.0;
		  HMRA.Prediction_Var24[i]=0.0;

		  HMRB.Daily_Freq_Log[i]=0.0;
		  HMRB.Holdover_Base[i]=0.0;
		  HMRB.Holdover_Drift[i]=0.0;
		  HMRB.Prediction_Var24[i]=0.0;

	 }
	 for (i=0;i<30;i++)
	 {
		  HMS.Monthly_Freq_Log[i]=0.0;
		  HMR.Monthly_Freq_Log[i]=0.0;
		  HMSA.Monthly_Freq_Log[i]=0.0;
		  HMSB.Monthly_Freq_Log[i]=0.0;
		  HMRA.Monthly_Freq_Log[i]=0.0;
		  HMRB.Monthly_Freq_Log[i]=0.0;
		  
		  HMS.Monthly_Pred_Log[i]=0.0;
		  HMR.Monthly_Pred_Log[i]=0.0;
		  HMSA.Monthly_Pred_Log[i]=0.0;
		  HMSB.Monthly_Pred_Log[i]=0.0;     
		  HMRA.Monthly_Pred_Log[i]=0.0;
		  HMRB.Monthly_Pred_Log[i]=0.0;
		  

	 }
	 #endif
	 /*********************End All Zero Data *************************/
	 HMS.Restart=TRUE;
	 HMR.Restart=TRUE;
	  #if PCSIM
	  /******* all zero Holdover LSF data ******/
	  HLSF.mfit=0.0;
	  HLSF.bfit=0.0;
	  HLSF.sx=0.0;
	  HLSF.sy=0.0;
	  HLSF.sxy=0.0;
	  HLSF.sx2=0.0;
	  HLSF.finp=0.0;
	  HLSF.dsmooth=0.0;
	  HLSF.wa=0.0;
     HLSF.wb=0.0;
	  HLSF.N=0;
	 #endif
	 for(i=0;i<24;i++)
	 {
		TrkStats.Good_Track_Min[i]=60;
	 }
	 TrkStats.Total_Good_Min=1440;
	 TrkStats.Cur_Hour=0;
	 TrkStats.Daily_Track_Success=100.0;
	 TrkStats.Hold_Stats=0;
	 TrkStats.Last_Mode=GPS_Warmup;
	 TrkStats.Min_Cnt=0;
	 TrkStats.Hold_Stats_Latch=0;
	 ASMM.Sec_Cnt=0;
	 ASMM.Min_Cnt=0;
	 ASMM.SPA_Flag= TRUE;
	 ASMM.SPB_Flag= TRUE;
	 ASMM.GPS_Flag= FALSE;
	 ASMM.ROA_Flag=TRUE;
	 ASMM.ROB_Flag=TRUE;
	 ASMM.TP_Flag=TRUE;
	 ASMM.PL_Flag=TRUE;
	 ASMM.AL_Flag=TRUE;
	 GPS_Drift_Count=3;
#if PRINTLOG
 ptime=0L; /* print log time stamp in cumulative minutes*/
#endif
 pcount=0; /*manage rate of print logging*/
 ASMM.GPS_Acc_Flag = FALSE;
 ASMM.GPS_Bias_Flag = FALSE;
 ASMM.GPS_Freq_Flag = FALSE;
 ASMM.GPS_Drift_Flag = FALSE;
 ASMM.GPS_Freq_Bucket_Flag =FALSE;
 ASMM.GPS_Drift_Bucket_Flag =FALSE;
 ASMM.GPS_Freq_Bucket_Hyst_Flag =FALSE;
 ASMM.GPS_Drift_Bucket_Hyst_Flag =FALSE;
 ASMM.SPA_Phase_Bucket_Flag =FALSE;
 ASMM.SPA_Freq_Bucket_Flag =FALSE;
 ASMM.SPA_Drift_Bucket_Flag =FALSE;
 ASMM.SPA_Phase_Bucket_Hyst_Flag =FALSE;
 ASMM.SPA_Freq_Bucket_Hyst_Flag =FALSE;
 ASMM.SPA_Drift_Bucket_Hyst_Flag =FALSE;
 ASMM.SPB_Phase_Bucket_Flag =FALSE;
 ASMM.SPB_Freq_Bucket_Flag =FALSE;
 ASMM.SPB_Drift_Bucket_Flag =FALSE;
 ASMM.SPB_Phase_Bucket_Hyst_Flag =FALSE;
 ASMM.SPB_Freq_Bucket_Hyst_Flag =FALSE;
 ASMM.SPB_Drift_Bucket_Hyst_Flag =FALSE;
 ASMM.ROA_Phase_Bucket_Flag =FALSE;
 ASMM.ROA_Freq_Bucket_Flag =FALSE;
 ASMM.ROA_Drift_Bucket_Flag =FALSE;
 ASMM.ROA_Phase_Bucket_Hyst_Flag =FALSE;
 ASMM.ROA_Freq_Bucket_Hyst_Flag =FALSE;
 ASMM.ROA_Drift_Bucket_Hyst_Flag =FALSE;
 ASMM.ROB_Phase_Bucket_Flag =FALSE;
 ASMM.ROB_Freq_Bucket_Flag =FALSE;
 ASMM.ROB_Drift_Bucket_Flag =FALSE;
 ASMM.ROB_Phase_Bucket_Hyst_Flag =FALSE;
 ASMM.ROB_Freq_Bucket_Hyst_Flag =FALSE;
 ASMM.ROB_Drift_Bucket_Hyst_Flag =FALSE;
 ASMM.TP_Step_Bucket_Flag =FALSE;
 ASMM.TP_Slew_Bucket_Flag =FALSE;
 ASMM.TP_Step_Bucket_Hyst_Flag =FALSE;
 ASMM.TP_Slew_Bucket_Hyst_Flag =FALSE;
 ASMM.PL_Step_Bucket_Flag =TRUE;
 ASMM.PL_Range_Bucket_Flag =TRUE;
 ASMM.PL_Step_Bucket_Hyst_Flag =TRUE;
 ASMM.PL_Range_Bucket_Hyst_Flag =TRUE;
 ASMM.AL_Step_Bucket_Flag =TRUE;
 ASMM.AL_Range_Bucket_Flag =TRUE;
 ASMM.AL_Step_Bucket_Hyst_Flag =TRUE;
 ASMM.AL_Range_Bucket_Hyst_Flag =TRUE;

 SReg.SPA_Reg=0xFF;          /*Status Reporting Registers (see Report  */
 SReg.SPB_Reg=0xFF;          /*_Status in Task.c for definitions )     */
 SReg.GPS_Reg=0xFF;
 SReg.ROA_Reg=0xFF;
 SReg.ROB_Reg=0xFF;
 SReg.TP_Reg =0xFF;
 SReg.PL_Reg =0xFF;
 SReg.AT_Reg= 0xFF;
 /****** One Minute Latched Registers for one minute tasks ********/
 SReg.SPA_Mreg=0x00;
 SReg.SPB_Mreg=0x00;
 SReg.GPS_Mreg=0x00;
 SReg.ROA_Mreg=0x00;
 SReg.ROB_Mreg=0x00;
 SReg.TP_Mreg =0x00;
 SReg.PL_Mreg =0x00;
 SReg.AT_Mreg= 0x00;
 OnePPSJamFlag=FALSE;
 /***** Ensembling Control Data Initialization *******/
 ECD.GPS_mvar=GPS_MDEV_10*GPS_MDEV_10;
 ECD.PA_mvar=SP_MDEV_10* SP_MDEV_10;
 ECD.PB_mvar=SP_MDEV_10* SP_MDEV_10;
 ECD.LO_mvar=LO_MDEV_10* LO_MDEV_10;
 ECD.PAC_mvar=ECD.PA_mvar;
 ECD.PBC_mvar=ECD.PB_mvar;
 ECD.LOC_mvar=ECD.PB_mvar;
 ECD.SA24_mvar=SP_TDEV_24/SECINDAY;
 ECD.SA24_mvar*=ECD.SA24_mvar;
 ECD.SB24_mvar=SP_TDEV_24/SECINDAY;
 ECD.SB24_mvar*=ECD.SB24_mvar;
 ECD.RA24_mvar=RO_TDEV_24/SECINDAY;
 ECD.RA24_mvar*=ECD.RA24_mvar;
 ECD.RB24_mvar=RO_TDEV_24/SECINDAY;
 ECD.RB24_mvar*=ECD.RB24_mvar;
 ECD.LO24_mvar=LO_TDEV_24/SECINDAY;
 ECD.LO24_mvar*=ECD.LO24_mvar;
 ECD.freq_state_cor=0.0;
 ECD.GPS_weight=1.0;
 ECD.PA_weight=0.0;
 ECD.PB_weight=0.0;
 ECD.Pmode=SPAN_ONLY;
 ECD.Input_Mode=GPS_ONLY;    
PutBt3EnsembleControlWord(0X1E02); //GPZ June 2010 limited engine to GPS only mode
  /* Threshold Management Intialization */
 TM.Gfreqmax=LTBI.gfreqmax;
 TM.Gdriftmax=LTBI.gdriftmax;
 TM.Sphasemax=SPHASEMAX;
 TM.Sfreqmax=g_freqMax;
 TM.Sdriftmax=SDRIFTMAX; 
 for(i=0;i<4;i++)
 {
	 TM.Sdriftvar[i]=1.6e-17; /*based on a 5 sigma threshold using 2e-8 as the max*/
	 TM.Sdriftthres[i]=SDRIFTMAX;
 }		 
 TM.Sdelmax=SDELMAX;
 TM.Sfreqtau=SFREQTAU;
 TM.ROphasemax=ROPHASEMAX;
 TM.ROfreqmax=ROFREQMAX;
 TM.ROdriftmax=RODRIFTMAX;
 TM.ROdelmax=RODELMAX;
 TM.ROdriftcheck=RODRIFTCHECK;
 TM.TPdelmax=TPDELMAX;
 TM.TPstepmax=TPSTEPMAX;
 TM.TPslewmax=TPSLEWMAX;
 TM.TPslewcheck=TPSLEWCHECK;
 TM.TPrangecheck=TPRANGECHECK;
 for(i=0;i<MAXTEMPRECORDS;i++)
 {
	  for(j=0;j<MAXCALRAMPS;j++)
	  {
		  TCDR[i].Freq_Est[j]= MAXFREQSLEW;
		  TCDR[i].Slew_Est[j]=0;
	  }
		  Tcomp.Freq_Comp[i]=0; /*jam for simple test case*/
		  /*Tcomp.Freq_Comp[i]=-80; /*jam for simple test case*/
 }
	/************************* All Zero Tcomp and Pcomp Data **********/
	#if PCSIM
	Tcomp.Slew_Comp=0;
	Tcomp.Part_Base=0;
	Tcomp.part_indx=0;
	Tcomp.prev_part_indx=0;
	for(i=0;i<TEMPERATURE_CACHE_LENGTH;i++) Tcomp.Partition[i]=0;
	Tcomp.Static_Temp_Cor=0;
	Tcomp.Dynamic_Temp_Cor=0;
	Pcomp.no_crossing_cnt=0;
	Pcomp.prev_meas=0;
	Pcomp.cur_error_est=0;
	Pcomp.prev_error_est=0;
	Pcomp.time_err_est=0.0;
   Pcomp.tdev_crossover=0.0;
	#endif
	Tcomp.Part_Init=TRUE;
	/********************** End All Zero Data Area ************************/
	Pcomp.attack_rate = LTBI.maxphaseslew;
	Pcomp.tgain1 = 1.0;
	Pcomp.tgain2 = 1.0-Pcomp.attack_rate*20.0e6;
	Pcomp.acquire_flag=TRUE;
	Pcomp.PPS_Range_Bucket= LBUCKETTHRES + LBUCKETHYS;
	Yro_tc=0.0;
	smooths=0.01;
	smoothl=.99;
	ATC.ATAU=ATCUPDATETAU;
	ATC.IFA = 1.0/ATCUPDATETAU; ATC.IFB= 1.0-ATC.IFA;
	ATC.Stemp1=0.0;
	ATC.Sfreq1=0.0;
	ATC.Ptemp=0.0; ATC.Pfreq=0.0;
	ATC.Stslew1_pos=(ATCMAXSLEW*2.0);ATC.Stslew1_neg= (-ATCMAXSLEW*2.0) ;
	ATC.Stslew2_pos=(ATCMAXSLEW*2.0);ATC.Stslew2_neg= (-ATCMAXSLEW*2.0);
	ATC.Sfslew1_pos=0.0;	ATC.Sfslew1_neg= 0.0 ;
	ATC.Sfslew2_pos=0.0;	ATC.Sfslew2_neg= 0.0 ;
	ATC.TCFA=ATCUPDATETAU;
	ATC.TCFA*= 1.0/ATCSMOOTHTAU;
	ATC.TCFB=1.0-ATC.TCFA;
	ATC.TC_pos= 0.0;    ATC.TC_neg= 0.0;
	ATC.TC_avg= 0.0;
	ATC.Acnt=0;
   ATC.Min_Cnt=0;
	ATC.Settle_Cnt=4; /*skip first four updates to settle filter*/
	ATC.Tslew_Thres=ATCMINSLEW;
	ATC.Slew_Bucket=LBUCKETTHRES+LBUCKETHYS;
	ATC.Mode=ATCGPSMODE;
	NCO_Slew=5; /*skip slew control on first 5 updates*/
	NCO_Smooth=0.0;
	for(i=0;i<MBSIZE;i++)
	{
		mpll.sum[i] = 0;
  	}
  	mpll.PPS_Nwrite_marker = 0;
	mpll.Nwrite = 0;
  	mpll.Nread = 0; 
  	mpll.Ndata = 0;     
  	mpll.PPS_Nwrite_marker = 0; 
  	mpll.status = 0;    
}
 /********************************************************************
	Module call at start at initialization
	to initialize shared memory interface
*******************************************************************/
void Init_Shared_Mem_Int()
{
	unsigned char i;
	smi.clock_bias = 0.0;   /*Estimate of time error of local time base with respect
								to the GPS timescale (sec)*/
	smi.time_acc_est= TIME_ACC_THRES;       /*95% Confidence Estimate of clock bias (ns) */
	smi.clock_bias_flag =FALSE;	   /*Summary flag of current critical gps receiver
												,sattelite or antenna alarms */
	smi.kalman_mode=POSITION;       /*Current Kalman Operating Mode */
	smi.oven_current=0;     /* current 8 bit estimate of OCXO oven current */
	smi.excessive_temp=FALSE;       /*Oven current low threshold exceeded*/
	smi.freq_state_cor=0.0; /*current feedback frequency correction from DCCA */
	smi.RO_freq_state_cor=0.0;/*remote oscillator frequency state correction */
	smi.temp_state_cor=0.0;  /*local oscillator temperature state correction*/
	smi.phase_state_cor=0.0; /*local oscillator phase state correction*/
	smi.RO_temp_state_cor=0.0;/*remote oscillator temperature state correction */
	smi.NCO_Low=LTBI.nco_center; /*Base NCO 32 bit control word
										 for BT3 applications*/
	smi.NCO_High=LTBI.nco_center + 1L;
	smi.NCO_Dither= 0xAA; /*Dither setting for controlling duty cycle of NCO lsb*/
	smi.NCO_Status= 0x00;
	smi.NCO_Dither_Error=0.0;

	smi.BT3_mode= GPS_Warmup;      /*current mode of operation of dfll */
	smi.BT3_mode_dur=0;    /*duration in minutes of current dfll mode */
	smi.GPS_mdev= GPS_MDEV_10; /*Stability estimate of  GPS */
	smi.PA_mdev=  SP_MDEV_10; /*Stability estimate of  GPS */
	smi.PB_mdev=  SP_MDEV_10; /*Stability estimate of  GPS */
	smi.LO_mdev=  LO_MDEV_10; /*Stability estimate of  GPS */
	smi.GPS_mvar= GPS_MDEV_10*GPS_MDEV_10; /*Stability estimate of  GPS */
	smi.PA_mvar=  SP_MDEV_10* SP_MDEV_10; /*Stability estimate of  GPS */
	smi.PB_mvar=  SP_MDEV_10* SP_MDEV_10; /*Stability estimate of  GPS */
	smi.LO_mvar=  LO_MDEV_10* LO_MDEV_10; /*Stability estimate of  GPS */
	smi.GPS_weight=1.0;    /*Weight controls medium term stability influence*/
	smi.PA_weight =0.0;    /*Weight controls medium term stability influence*/
	smi.PB_weight =0.0;    /*Weight controls medium term stability influence*/

	smi.dloopsa_prop_tc=DUAL_SP_PROP_PRIM_TAU; /*Dual GPS Span proportional time constant */
	smi.dloopsa_int_tc =DUAL_SP_INT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopsa_dint_tc =DUAL_SP_DINT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopsa_cal_tc =DUAL_SP_CAL_TAU; /*Dual GPS Span calibration time constant */
	smi.dloopsb_prop_tc=DUAL_SP_PROP_PRIM_TAU; /*Dual GPS Span proportional time constant */
	smi.dloopsb_int_tc =DUAL_SP_INT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopsb_dint_tc =DUAL_SP_DINT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopsb_cal_tc =DUAL_SP_CAL_TAU; /*Dual GPS Span calibration time constant */

	smi.dloopra_prop_tc =DUAL_RO_PROP_PRIM_TAU; /*Dual GPS Span proportional time constant */
	smi.dloopra_int_tc  =DUAL_RO_INT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopra_dint_tc =DUAL_RO_DINT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dloopra_cal_tc  =DUAL_RO_CAL_TAU; /*Dual GPS Span calibration time constant */
	smi.dlooprb_prop_tc =DUAL_RO_PROP_PRIM_TAU; /*Dual GPS Span proportional time constant */
	smi.dlooprb_int_tc  =DUAL_RO_INT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dlooprb_dint_tc =DUAL_RO_DINT_PRIM_TAU; /*Dual GPS Span integral time constant */
	smi.dlooprb_cal_tc  =DUAL_RO_CAL_TAU; /*Dual GPS Span calibration time constant */

	smi.sloop_prop_tc =LTBI.single_prop_tau/LTBI.single_accel; /*Single GPS Only  proportional time constant */
	smi.sloop_int_tc  =MAX_INT_TC; /*Single GPS Only  integral time constant */
	smi.sloop_dint_tc =LTBI.single_dint_tau; /*Dual GPS E1 integral time constant */
	smi.rloop_prop_tc =LTBI.single_prop_tau; /*Single GPS Only  proportional time constant */
	smi.rloop_int_tc  =LTBI.single_int_tau; /*Single GPS Only  integral time constant */
	smi.rloop_dint_tc =LTBI.single_dint_tau; /*Dual GPS E1 integral time constant */
	smi.SPA_Reg=0xFF;          /*Status Reporting Registers (see Report  */
	smi.SPB_Reg=0xFF;          /*_Status in Task.c for definitions )     */
	smi.GPS_Reg=0xFF;
	smi.ROA_Reg=0xFF;
	smi.ROB_Reg=0xFF;
	smi.TP_Reg =0xFF;
	smi.PL_Reg =0xFF;
	smi.AT_Reg= 0xFF;
	smi.SPA_Lreg=0x00;          /*Status Reporting Registers (see Report  */
	smi.SPB_Lreg=0x00;          /*_Status in Task.c for definitions )     */
	smi.GPS_Lreg=0x00;
	smi.ROA_Lreg=0x00;
	smi.ROB_Lreg=0x00;
	smi.TP_Lreg =0x00;
	smi.PL_Lreg =0x00;
	smi.AT_Lreg= 0x00;
	smi.CLR_Lreg=FALSE;

	for(i=0;i<TOTAL_TAUCAT;i++)
	{
	  smi.timing_stats[i]=0.0;
	}
	smi.Timing_Chan=GPS_LO;
	smi.Timing_Stat=AFREQ;
	smi.Mtie_Reset =NONE;
	/* Threshold Management Initialization */
	smi.Gfreqmax=LTBI.gfreqmax*1e9;
	smi.Gdriftmax=LTBI.gdriftmax*1e9;
	smi.Sphasemax=SPHASEMAX*TAUZERO*1e9;
	smi.Sfreqmax=g_freqMax*1e9;
	smi.Sdriftmax=SDRIFTMAX*1e9;
	smi.Sdelmax=SDELMAX*50;
	smi.Sfreqtau=SFREQTAU;
	smi.ROdelmax=RODELMAX*10/26;
	smi.ROphasemax=ROPHASEMAX*1e9;
	smi.ROfreqmax=ROFREQMAX*1e9;
	smi.ROdriftmax=RODRIFTMAX*1e9;
	smi.TPdelmax=TPDELMAX;
	smi.TPslewcheck=TPSLEWCHECK*TAUZERO;
	smi.TPstepmax=TPSTEPMAX;
	smi.TPslewmax=TPSLEWMAX;
	smi.TCcycle= TRUE;
	smi.One_PPS_Lead=0;
	smi.One_PPS_Range=1;
	smi.One_PPS_Mag=0;
	smi.One_PPS_High=TRUE; /*assume not calibrated to start*/
	smi.One_PPS_Jam=0;
	smi.R0B_Select=EXTERNAL_B;
	smi.One_PPS_Est_RO=0;
	smi.LO_PPS_Data_Valid=FALSE; /*assume no remote oscillator to start*/
	smi.RO_PPS_Data_Valid=FALSE; /*assume no remote oscillator to start*/
	smi.Overide_Mode=0;
	smi.Input_Mode= GPS_ONLY; /* was all_inputs*/
	smi.GPS_Locked= FALSE;
}
/***********************************************************************
Module called to initialize all BT3 intertask and external
shell  message structures
Note: default initialization is all zero.
In embedded application where ram is set to zero this
code is "stubbed out" to save code space
************************************************************************/
void BT3_MSG_Init(void)
{
	unsigned short i,j;                     
#if (PCSIM)
	msg_count1sec=0;
	msg_count250ms=0; /*control one second span message process*/
	spa_reg_local=0; spb_reg_local=0; /*local span event registers*/
	Temp_Est_tsk=0.0;           /*task file copy of Temp_Est variable*/
	YroavgA_tsk=0.0;
	YroavgB_tsk=0.0;
	MSG_Span_FLL_A.YspavgA=0.0; /*SPAN A frequency estimate */
	MSG_Span_FLL_A.YspavgB=0.0; /*SPAN B frequency estimate */
	MSG_Span_FLL_A.SPA_Reg_Events=0; /*current event level register*/
	MSG_Span_FLL_A.SPB_Reg_Events=0; /*current event level register*/
	MSG_Span_FLL_A.Ready=FALSE;
	MSG_Span_FLL_B.YspavgA=0.0; /*SPAN A frequency estimate */
	MSG_Span_FLL_B.YspavgB=0.0; /*SPAN B frequency estimate */
	MSG_Span_FLL_B.SPA_Reg_Events=0; /*current event level register*/
	MSG_Span_FLL_B.SPB_Reg_Events=0; /*current event level register*/
	MSG_Span_FLL_B.Ready=FALSE;
	roa_reg_local=0; rob_reg_local=0; /*local RO event registers*/
	MSG_RO_FLL_A.YroavgA=0.0; /*RO A frequency estimate */
	MSG_RO_FLL_A.YroavgB=0.0; /*RO B frequency estimate */
	MSG_RO_FLL_A.ROA_Reg_Events=0; /*current event level register*/
	MSG_RO_FLL_A.ROB_Reg_Events=0; /*current event level register*/
	MSG_RO_FLL_A.Ready=FALSE;
	MSG_RO_FLL_B.YroavgA=0.0; /*RO A frequency estimate */
	MSG_RO_FLL_B.YroavgB=0.0; /*RO B frequency estimate */
	MSG_RO_FLL_B.ROA_Reg_Events=0; /*current event level register*/
	MSG_RO_FLL_B.ROB_Reg_Events=0; /*current event level register*/
	MSG_RO_FLL_B.Ready=FALSE;

	MSG_FLL_EST_A.YprimoutSPA=0.0;
	MSG_FLL_EST_A.YprimoutSPB=0.0;
	MSG_FLL_EST_A.YprimoutROA=0.0;
	MSG_FLL_EST_A.YprimoutROB=0.0;
	MSG_FLL_EST_A.Ready=FALSE;

	MSG_NCOUP_EST_A.freq_state_cor=0.0;
	MSG_NCOUP_EST_A.Ready=FALSE;

	MSG_PCOMP_EST_A.phase_state_cor=0.0;
	MSG_PCOMP_EST_A.Ready=FALSE;
	tp_reg_local=0;
	pl_reg_local=0;
	at_reg_local=0;
	MSG_EST_ATC_A.YroavgA=0.0; /*RO A frequency estimate */
	MSG_EST_ATC_A.YroavgB=0.0; /*RO B frequency estimate */
	MSG_EST_ATC_A.Ytpavg=0.0;
	MSG_EST_ATC_A.Temp_Est=0.0; /*Current oven temperature estimate */
	MSG_EST_ATC_A.ROA_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_A.ROB_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_A.TP_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_A.TP_Slew_Update=FALSE;
	MSG_EST_ATC_A.Ready=FALSE;
	MSG_EST_ATC_B.YroavgA=0.0; /*RO A frequency estimate */
	MSG_EST_ATC_B.YroavgB=0.0; /*RO B frequency estimate */
	MSG_EST_ATC_B.Ytpavg=0.0;
	MSG_EST_ATC_B.Temp_Est=0.0; /*Current oven temperature estimate */
	MSG_EST_ATC_B.ROA_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_B.ROB_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_B.TP_Reg_Events=0; /*current event level register*/
	MSG_EST_ATC_B.TP_Slew_Update=FALSE;
	MSG_EST_ATC_B.Ready=FALSE;
	MSG_EST_SRPT_A.SPA_Reg_Events=0;
	MSG_EST_SRPT_A.SPB_Reg_Events=0;
	MSG_EST_SRPT_A.ROA_Reg_Events=0;
	MSG_EST_SRPT_A.ROB_Reg_Events=0;
	MSG_EST_SRPT_A.TP_Reg_Events=0;
	MSG_EST_SRPT_A.PL_Reg_Events=0;
	MSG_EST_SRPT_A.AT_Reg_Events=0;
	MSG_EST_SRPT_A.Ready=FALSE;
	MSG_EST_SRPT_B.SPA_Reg_Events=0;
	MSG_EST_SRPT_B.SPB_Reg_Events=0;
	MSG_EST_SRPT_B.ROA_Reg_Events=0;
	MSG_EST_SRPT_B.ROB_Reg_Events=0;
	MSG_EST_SRPT_B.TP_Reg_Events=0;
	MSG_EST_SRPT_B.PL_Reg_Events=0;
	MSG_EST_SRPT_B.AT_Reg_Events=0;
	MSG_EST_SRPT_B.Ready=FALSE;
	MSG_EST_ASM_A.SPAphase_bkt=0; MSG_EST_ASM_A.SPAfreq_bkt=0; MSG_EST_ASM_A.SPAdrift_bkt=0;
	MSG_EST_ASM_A.SPBphase_bkt=0; MSG_EST_ASM_A.SPBfreq_bkt=0; MSG_EST_ASM_A.SPBdrift_bkt=0;
	MSG_EST_ASM_A.ROAphase_bkt=0; MSG_EST_ASM_A.ROAfreq_bkt=0; MSG_EST_ASM_A.ROAdrift_bkt=0;
	MSG_EST_ASM_A.ROBphase_bkt=0; MSG_EST_ASM_A.ROBfreq_bkt=0; MSG_EST_ASM_A.ROBdrift_bkt=0;
	MSG_EST_ASM_A.TPstep_bkt=0;   MSG_EST_ASM_A.TPslew_bkt=0;
	MSG_EST_ASM_A.PLstep_bkt=0;   MSG_EST_ASM_A.PLrange_bkt=0;
	MSG_EST_ASM_A.ALstep_bkt=0;   MSG_EST_ASM_A.ALrange_bkt=0;
	MSG_EST_ASM_A.Ready=FALSE;
	MSG_EST_ASM_B.SPAphase_bkt=0; MSG_EST_ASM_B.SPAfreq_bkt=0; MSG_EST_ASM_B.SPAdrift_bkt=0;
	MSG_EST_ASM_B.SPBphase_bkt=0; MSG_EST_ASM_B.SPBfreq_bkt=0; MSG_EST_ASM_B.SPBdrift_bkt=0;
	MSG_EST_ASM_B.ROAphase_bkt=0; MSG_EST_ASM_B.ROAfreq_bkt=0; MSG_EST_ASM_B.ROAdrift_bkt=0;
	MSG_EST_ASM_B.ROBphase_bkt=0; MSG_EST_ASM_B.ROBfreq_bkt=0; MSG_EST_ASM_B.ROBdrift_bkt=0;
	MSG_EST_ASM_B.TPstep_bkt=0;   MSG_EST_ASM_B.TPslew_bkt=0;
	MSG_EST_ASM_B.PLstep_bkt=0;   MSG_EST_ASM_B.PLrange_bkt=0;
	MSG_EST_ASM_B.ALstep_bkt=0;   MSG_EST_ASM_B.ALrange_bkt=0;
	MSG_EST_ASM_B.Ready=FALSE;

	MSG_FLL_HOLDOVER_A.Lloop_Freq_Err_Sum=0.0;
	MSG_FLL_HOLDOVER_A.Rloop_Freq_Err_Sum=0.0;
	MSG_FLL_HOLDOVER_A.SPA_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_A.SPB_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_A.ROA_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_A.ROB_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_A.Lloop_Freq_Sum_Cnt=0;
	MSG_FLL_HOLDOVER_A.Rloop_Freq_Sum_Cnt=0;
	MSG_FLL_HOLDOVER_A.SPA_Recal=FALSE;
	MSG_FLL_HOLDOVER_A.SPB_Recal=FALSE;
	MSG_FLL_HOLDOVER_A.ROA_Recal=FALSE;
	MSG_FLL_HOLDOVER_A.ROB_Recal=FALSE;
	MSG_FLL_HOLDOVER_A.SPA_Mreg=0;
	MSG_FLL_HOLDOVER_A.SPB_Mreg=0;
	MSG_FLL_HOLDOVER_A.ROA_Mreg=0;
	MSG_FLL_HOLDOVER_A.ROB_Mreg=0;
	MSG_FLL_HOLDOVER_A.Ready=FALSE;
	MSG_FLL_HOLDOVER_B.Lloop_Freq_Err_Sum=0.0;
	MSG_FLL_HOLDOVER_B.Rloop_Freq_Err_Sum=0.0;
	MSG_FLL_HOLDOVER_B.SPA_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_B.SPB_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_B.ROA_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_B.ROB_GPS_Prop_State=0.0;
	MSG_FLL_HOLDOVER_B.Lloop_Freq_Sum_Cnt=0;
	MSG_FLL_HOLDOVER_B.Rloop_Freq_Sum_Cnt=0;
	MSG_FLL_HOLDOVER_B.SPA_Recal=FALSE;
	MSG_FLL_HOLDOVER_B.SPB_Recal=FALSE;
	MSG_FLL_HOLDOVER_B.ROA_Recal=FALSE;
	MSG_FLL_HOLDOVER_B.ROB_Recal=FALSE;
	MSG_FLL_HOLDOVER_B.SPA_Mreg=0;
	MSG_FLL_HOLDOVER_B.SPB_Mreg=0;
	MSG_FLL_HOLDOVER_B.ROA_Mreg=0;
	MSG_FLL_HOLDOVER_B.ROB_Mreg=0;
	MSG_FLL_HOLDOVER_B.Ready=FALSE;
	MSG_HOLDOVER_FLL_A.Lloop_Osc_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.Rloop_Osc_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.SPA_Cal_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.SPB_Cal_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.ROA_Cal_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.ROB_Cal_Freq_Pred=0.0;
	MSG_HOLDOVER_FLL_A.Tracking_Success=0.0;
	MSG_HOLDOVER_FLL_A.Ready=FALSE;
	MSG_HOLDOVER_NCOUP_A.Lloop_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.Rloop_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.SPA_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.SPB_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.ROA_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.ROB_Pred24_Var=0.0;
	MSG_HOLDOVER_NCOUP_A.Ready=FALSE;
	MSG_FLL_TCH_A.XGPS_1Min=0.0;
	MSG_FLL_TCH_A.XSPA_1Min=0.0;MSG_FLL_TCH_A.XSPB_1Min=0.0;
	MSG_FLL_TCH_A.XROA_1Min=0.0;MSG_FLL_TCH_A.XROB_1Min=0.0;
	MSG_FLL_TCH_A.XGPS_cnt=0;
	MSG_FLL_TCH_A.XSPA_cnt=0,MSG_FLL_TCH_A.XSPB_cnt=0;
	MSG_FLL_TCH_A.XROA_cnt=0,MSG_FLL_TCH_A.XROB_cnt=0;
	MSG_FLL_TCH_B.Pmode=0;
	MSG_FLL_TCH_A.Ready=FALSE;
	MSG_FLL_TCH_B.XGPS_1Min=0.0;
	MSG_FLL_TCH_B.XSPA_1Min=0.0;MSG_FLL_TCH_B.XSPB_1Min=0.0;
	MSG_FLL_TCH_B.XROA_1Min=0.0;MSG_FLL_TCH_B.XROB_1Min=0.0;
	MSG_FLL_TCH_B.XGPS_cnt=0;
	MSG_FLL_TCH_B.XSPA_cnt=0,MSG_FLL_TCH_B.XSPB_cnt=0;
	MSG_FLL_TCH_B.XROA_cnt=0,MSG_FLL_TCH_B.XROB_cnt=0;
	MSG_FLL_TCH_B.Pmode=0;
	MSG_FLL_TCH_B.Ready=FALSE;
	MSG_SR_TCH_A.SPA_Mreg=0;
	MSG_SR_TCH_A.SPB_Mreg=0;
	MSG_SR_TCH_A.GPS_Mreg=0;
	MSG_SR_TCH_A.ROA_Mreg=0;
	MSG_SR_TCH_A.ROB_Mreg=0;
	MSG_SR_TCH_A.Ready=FALSE;
	MSG_SR_TCH_B.SPA_Mreg=0;
	MSG_SR_TCH_B.SPB_Mreg=0;
	MSG_SR_TCH_B.GPS_Mreg=0;
	MSG_SR_TCH_B.ROA_Mreg=0;
	MSG_SR_TCH_B.ROB_Mreg=0;
	MSG_SR_TCH_B.Ready=FALSE;
	MSG_TCH_NCOUP_A.GPS_mvar=0.0;
	MSG_TCH_NCOUP_A.PA_mvar=0.0;
	MSG_TCH_NCOUP_A.PB_mvar=0.0;
	MSG_TCH_NCOUP_A.LO_mvar=0.0;
	MSG_TCH_NCOUP_A.Ready=FALSE;
	MSG_FLL_CAL_A.Lloop_NCO=0.0;
	MSG_FLL_CAL_A.Lloop_Prop_Tau=0.0;
	MSG_FLL_CAL_A.Lloop_Int_State=0.0;
	MSG_FLL_CAL_A.Ready=FALSE;
	MSG_FLL_CAL_B.Lloop_NCO=0.0;
	MSG_FLL_CAL_B.Lloop_Prop_Tau=0.0;
	MSG_FLL_CAL_B.Lloop_Int_State=0.0;
	MSG_FLL_CAL_B.Ready=FALSE;
	MSG_CAL_EST_A.Factory_Freq_Cal=0.0;
	MSG_CAL_EST_A.Ready=FALSE;
	MSG_CAL_FLL_A.Factory_Freq_Cal=0.0;
	MSG_CAL_FLL_A.Ready=FALSE;

	/********************** EXTERNAL SHELL MESSAGES ***********************/
	for(i=0;i<CLOOPS;i++)
	{
		MSG_CLS[i].IN_Prop_State=0.0;
		MSG_CLS[i].IN_Int_State=0.0;
		MSG_CLS[i].IN_Dint_State=0.0;
		MSG_CLS[i].IN_GPS_Cal_State=0;
		MSG_CLS[i].IN_Recalibration_Mode=0;
		MSG_CLS[i].IN_Mode_Shift=0;
		MSG_CLS[i].IN_Weight=0;
		MSG_CLS[i].Ready=FALSE;
	}
	MSG_CLC.Freq_State_Cor=0.0;
	MSG_CLC.Phase_State_Cor=0.0;
	MSG_CLC.Temp_State_Cor=0.0;
	MSG_CLC.Input_Mode=0;
	MSG_CLC.Ready=FALSE;
	MSG_ASR.GPS_Reg=0;
	MSG_ASR.SPA_Reg=0;
	MSG_ASR.SPB_Reg=0;
	MSG_ASR.ROA_Reg=0;
	MSG_ASR.ROB_Reg=0;
	MSG_ASR.PL_Reg=0;
	MSG_ASR.TP_Reg=0;
	MSG_ASR.BT3_Mode=0;
	MSG_ASR.BT3_Mode_Dur=0;
	MSG_ASR.Ready=FALSE;
	for(j=0;j<4;j++)
	{
	  MSG_DHR.avg_freq[j].Freq_Base=0L;
	  MSG_DHR.avg_freq[j].Freq_Off_0=0;
	  MSG_DHR.avg_freq[j].Freq_Off_1=0;
	  MSG_DHR.avg_freq[j].Freq_Off_2=0;
	  MSG_DHR.avg_freq[j].Freq_Off_3=0;
	  MSG_DHR.avg_freq[j].Freq_Off_4=0;
	  MSG_DHR.pred_freq[j].Freq_Base=0L;
	  MSG_DHR.pred_freq[j].Freq_Off_0=0;
	  MSG_DHR.pred_freq[j].Freq_Off_1=0;
	  MSG_DHR.pred_freq[j].Freq_Off_2=0;
	  MSG_DHR.pred_freq[j].Freq_Off_3=0;
	  MSG_DHR.pred_freq[j].Freq_Off_4=0;
	}
	  MSG_DHR.Freq_Drift_Estimate=0L; /*Current estimate of drift (ps/s) per day*/
	  MSG_DHR.Freq_Pred_Err_24=0L;     /*RMS estimate of 24 hour holdover time error (ns)*/
	  MSG_DHR.Channel=0;
	  MSG_DHR.Ready=FALSE;
	for(j=0;j<5;j++)
	{
	  MSG_MHR.Daily_Freq[j].Freq_Base=0L;
	  MSG_MHR.Daily_Freq[j].Freq_Off_0=0;
	  MSG_MHR.Daily_Freq[j].Freq_Off_1=0;
	  MSG_MHR.Daily_Freq[j].Freq_Off_2=0;
	  MSG_MHR.Daily_Freq[j].Freq_Off_3=0;
	  MSG_MHR.Daily_Freq[j].Freq_Off_4=0;
	  MSG_MHR.Daily_Pred[j].Freq_Base=0L;
	  MSG_MHR.Daily_Pred[j].Freq_Off_0=0;
	  MSG_MHR.Daily_Pred[j].Freq_Off_1=0;
	  MSG_MHR.Daily_Pred[j].Freq_Off_2=0;
	  MSG_MHR.Daily_Pred[j].Freq_Off_3=0;
	  MSG_MHR.Daily_Pred[j].Freq_Off_4=0;
	}
	MSG_MHR.Channel=0;
	MSG_MHR.Ready=FALSE;
	MSG_PSM.Phase=0;
	MSG_PSM.MTIE1=0;
	MSG_PSM.MTIE2=0;
	MSG_PSM.MTIE4=0;
	MSG_PSM.MTIE8=0;
	MSG_PSM.MTIE16=0;
	MSG_PSM.MTIE32=0;
	MSG_PSM.MTIE64=0;
	MSG_PSM.MTIE128=0;
	MSG_PSM.MTIE256=0;
	MSG_PSM.MTIE512=0;
	MSG_PSM.MTIE1K=0;
	MSG_PSM.MTIE2K=0;
	MSG_PSM.MTIE4K=0;
	MSG_PSM.MTIE8K=0;
	MSG_PSM.MTIE16K=0;
	MSG_PSM.AFREQ1=0;
	MSG_PSM.AFREQ2=0;
	MSG_PSM.AFREQ4=0;
	MSG_PSM.AFREQ8=0;
	MSG_PSM.AFREQ16=0;
	MSG_PSM.AFREQ32=0;
	MSG_PSM.AFREQ64=0;
	MSG_PSM.AFREQ128=0;
	MSG_PSM.AFREQ256=0;
	MSG_PSM.AFREQ512=0;
	MSG_PSM.AFREQ1K=0;
	MSG_PSM.AFREQ2K=0;
	MSG_PSM.AFREQ4K=0;
	MSG_PSM.AFREQ8K=0;
	MSG_PSM.AFREQ16K=0;
	MSG_PSM.MVAR1=0;
	MSG_PSM.MVAR2=0;
	MSG_PSM.MVAR4=0;
	MSG_PSM.MVAR8=0;
	MSG_PSM.MVAR16=0;
	MSG_PSM.MVAR32=0;
	MSG_PSM.MVAR64=0;
	MSG_PSM.MVAR128=0;
	MSG_PSM.MVAR256=0;
	MSG_PSM.MVAR512=0;
	MSG_PSM.MVAR1K=0;
	MSG_PSM.MVAR2K=0;
	MSG_PSM.MVAR4K=0;
	MSG_PSM.MVAR8K=0;
	MSG_PSM.MVAR16K=0;
	MSG_PSM.Channel=0;
	MSG_PSM.Ready=FALSE;
	MSG_TSR.One_PPS_High=0;
	MSG_TSR.PPS_Range_Alarm=0;
	MSG_TSR.Phase_State_Correction=0.0;
	MSG_TSR.Time_Err_Est=0.0;
	MSG_TSR.TDEV_Crossover=0.0;
	MSG_TSR.TFOM=0.0;
   MSG_TSR.Time_Err_Meas=0.0;
	MSG_TSR.Ready=FALSE;
	MSG_ECW_C.ECW=0;
	MSG_ECW_C.Ready=FALSE;
	MSG_ECW_R.ECW=0;
	MSG_ECW_R.Override=0;
	MSG_RCW_R.Ready=FALSE;
//	if(Read_CCKB()) 
//	{
//		MSG_ECW_C.ECW|=0x400; 
//		MSG_RCW_R.ECW= MSG_ECW_C.ECW;
//	}
	MSG_PMASK_C.PMASK=0;
	MSG_PMASK_C.Ready=FALSE;  
	MSG_PMASK_R.PMASK=0;
	MSG_PMASK_R.Override=0;
	MSG_PMASK_R.Ready=FALSE; 
//	if(Read_CCKB()) 
//	{
//		MSG_PMASK_C.PMASK|=0x4;   
//		MSG_PMASK_R.PMASK=MSG_PMASK_C.PMASK;
//	}
	MSG_QMASK_C.QMASK=0;
	MSG_QMASK_C.Ready=FALSE;
	MSG_QMASK_R.QMASK=0;
	MSG_QMASK_R.Override=0;
	MSG_QMASK_R.Ready=FALSE;
 #endif
}
/***********************************************************************
Moduled called after init tasks to set-up control loop and holdover
estimation in best known state
*************************************************************************/
void init_cal()
{
		  short j;  
		  short i;
		  double temp; 
		  if(Cal_Data.cal_state)  /*doesn't work in mode 0 or 1   (kjh)*/
		  { /*if valid calibration data*/
		  	 sloop= &Lloop;
			 if(Cal_Data.cal_state==CAL_Dnld||
				((MSG_RCW_C.RCW&0x7)>1))
			 {
				Cal_Data.cal_state=CAL_Update; /*clear one time download state*/
				smi.BT3_mode =GPS_Normal;
/*			  #if HOTDLOAD*/
			  if(MSG_RCW_C.RCW&0x7==3)
			  {
				sloop->Prop_Tau=LTBI.single_prop_tau;
				sloop->Int_Tau= LTBI.single_int_tau;
				sloop->Dint_Tau=LTBI.single_dint_tau;
				sloop->Propf1=1.0/sloop->Prop_Tau;
				sloop->Propf2=1.0-sloop->Propf1;
				sloop->Recalibrate=FALSE;
				if(LTBI.lotype==RB)
				 sloop->Cal_Mode=7;
				else
				 sloop->Cal_Mode=5;
			  }
			  else
			  {
				sloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_recal;
				sloop->Int_Tau= LTBI.single_int_tau/(LTBI.single_recal*LTBI.single_recal);
				sloop->Dint_Tau=LTBI.single_dint_tau;
				sloop->Prop_Tau *=LTBI.gear_space;
				sloop->Prop_Tau *=LTBI.gear_space;
				sloop->Prop_Tau *=LTBI.gear_space;
				sloop->Int_Tau  *=(LTBI.gear_space*LTBI.gear_space);
				sloop->Int_Tau  *=(LTBI.gear_space*LTBI.gear_space);
				sloop->Int_Tau  *=(LTBI.gear_space*LTBI.gear_space);
				sloop->Propf1=1.0/sloop->Prop_Tau;
				sloop->Propf2=1.0-sloop->Propf1;
				sloop->Cal_Mode=5;
				sloop->Recalibrate=TRUE;
			  }
			 }
			 else
			 {
			  sloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_recal;
			  sloop->Int_Tau= LTBI.single_int_tau/(LTBI.single_recal*LTBI.single_recal);
			  sloop->Dint_Tau=LTBI.single_dint_tau;
			  sloop->Propf1=1.0/sloop->Prop_Tau;
			  sloop->Propf2=1.0-sloop->Propf1;
			  sloop->Cal_Mode=2;  
			  sloop->Trace_Skip=0;
			  sloop->Recalibrate=TRUE;
			 }
			 smi.sloop_prop_tc =sloop->Prop_Tau;
			 smi.sloop_int_tc  =sloop->Int_Tau;
			 smi.sloop_dint_tc =sloop->Dint_Tau;
			 sloop->Prop_State=Cal_Data.LO_Freq_Cal;
			 sloop->Int_State= Cal_Data.LO_Drift_Cal*sloop->Prop_Tau/1800.0;
			 sloop->Dint_State=0.0;
			 sloop->NCO_Cor_State= sloop->Prop_State + sloop->Int_State;
			 sloop->Freq_Err_Est=0.0;
			 sloop->LO_Freq_Pred= Cal_Data.LO_Freq_Cal;
			 sloop->Freq_Err_Sum=0.0;
			 sloop->Freq_Sum_Cnt=0;
			 sloop->Sprop1=Cal_Data.LO_Freq_Cal;
			 sloop->Sprop2=  Cal_Data.LO_Freq_Cal;
			 sloop->Intf1=60.0/sloop->Int_Tau;
			 sloop->Intf2=1.0-sloop->Intf1;
			 for(j=0;j<GPSDELAY;j++)
			 {
			  sloop->Ygps_Buf[j]=Cal_Data.LO_Freq_Cal; /*Buffer of last n gps loop inputs*/
			 }
			 sloop->Sprop_prev= Cal_Data.LO_Freq_Cal;
			 sloop->min_cnt=0;
			 sloop->LO_weight=1.0; /* ensemble weight contribution of Local Oscillator */
			 sloop->minute_cnt=0;
			 if(LTBI.lotype==RB)
			 sloop->shift_cnt=((sloop->Prop_Tau+60)/22);
			 else
		/*	 sloop->shift_cnt=((sloop->Prop_Tau+60)/15);*/
			 sloop->shift_cnt=((sloop->Prop_Tau+60)/20);
			 sloop->jamloop=FALSE;
			 smi.freq_state_cor=Cal_Data.LO_Freq_Cal;
			 ECD.freq_state_cor=Cal_Data.LO_Freq_Cal;
			 /**** jam previous 24 hour holdover data base*/
			 temp=Cal_Data.LO_Freq_Cal;
			 HMS.Freq_Sum_Daily=0.0;
			 for (i=24;i>0;i--)
			 {
				j=i%24;
				HMS.Daily_Freq_Log[j]=temp;
				HMS.Holdover_Base[j]=temp;
				HMS.Holdover_Drift[j]=Cal_Data.LO_Drift_Cal;
				temp -=Cal_Data.LO_Drift_Cal;
			 }
			 HMS.Cur_Hour=0;
			 HMS.Cur_Day=0;
			 HMS.Nsum_Hour=0;
			 HMS.Nsum_Day=0;
			 HMS.Freq_Sum_Daily=0.0;
			 HMS.Predict_Err24Hr=0.0;
			 HMS.Long_Term_Drift=Cal_Data.LO_Drift_Cal;
			 HMS.Drift_Sum_Weekly=0.0;
			 HMS.Osc_Freq_State=Cal_Data.LO_Freq_Cal;
			 HMS.Restart=TRUE;
			 /**** update ATC calibration data structure*****/
			 if (Cal_Data.ATC_TC_Cal!=0.0)
			 {
			  ATC.TC_avg=(double)(Cal_Data.ATC_TC_Cal);
			  ATC.Stslew1_pos=(ATCMINSLEW*2.0);ATC.Stslew1_neg= (-ATCMINSLEW*2.0) ;
			  ATC.Stslew2_pos=(ATCMINSLEW*2.0);ATC.Stslew2_neg= (-ATCMINSLEW*2.0);
			  ATC.Sfslew1_pos=ATC.Stslew1_pos*ATC.TC_avg;
			  ATC.Sfslew1_neg=ATC.Stslew1_neg*ATC.TC_avg;
			  ATC.Sfslew2_pos=ATC.Sfslew1_pos; ATC.Sfslew2_neg=ATC.Sfslew1_neg;
			  ATC.TC_pos= ATC.TC_avg;    ATC.TC_neg= ATC.TC_avg;
			 }
			 for(i=0;i<MAXTEMPRECORDS;i++)
			 {
            if((Cal_Data.Freq_Comp[i]<100)||(Cal_Data.Freq_Comp[i]>-100))
				Tcomp.Freq_Comp[i]=(short)Cal_Data.Freq_Comp[i];
			 }
			 #if(TARGET==CLPR)
			 Tcomp.Slew_Comp= (short)Cal_Data.Freq_Comp[255]; /*use last record for
																				 dynamic temperature
                                                             correction*/
			 #endif
		  }/*end if valid cal data*/ 
		  else
		  {
			 sloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_recal;
			 sloop->Int_Tau= LTBI.single_int_tau/LTBI.single_recal;
			 sloop->Dint_Tau=LTBI.single_dint_tau;
			 sloop->Propf1=1.0/sloop->Prop_Tau;
			 sloop->Propf2=1.0-sloop->Propf1;
			 sloop->Cal_Mode=0;    
			 sloop->Trace_Skip=0;
			 sloop->Recalibrate=TRUE;
			 smi.sloop_prop_tc =sloop->Prop_Tau;
			 smi.sloop_int_tc  =sloop->Int_Tau;
			 smi.sloop_dint_tc =sloop->Dint_Tau;
			 sloop->Prop_State=Cal_Data.LO_Freq_Cal;
			 sloop->Int_State= Cal_Data.LO_Drift_Cal*sloop->Prop_Tau/1800.0;
			 sloop->Dint_State=0.0;
			 sloop->NCO_Cor_State= sloop->Prop_State + sloop->Int_State;
			 sloop->Freq_Err_Est=0.0;
			 sloop->LO_Freq_Pred= Cal_Data.LO_Freq_Cal;
			 sloop->Freq_Err_Sum=0.0;
			 sloop->Freq_Sum_Cnt=0;
			 sloop->Sprop1=Cal_Data.LO_Freq_Cal;
			 sloop->Sprop2=  Cal_Data.LO_Freq_Cal;
			 sloop->Intf1=60.0/sloop->Int_Tau;
			 sloop->Intf2=1.0-sloop->Intf1;
			 for(j=0;j<GPSDELAY;j++)
			 {
			  sloop->Ygps_Buf[j]=Cal_Data.LO_Freq_Cal; /*Buffer of last n gps loop inputs*/
			 }
			 sloop->Sprop_prev= Cal_Data.LO_Freq_Cal;
			 sloop->min_cnt=0;
			 sloop->LO_weight=1.0; /* ensemble weight contribution of Local Oscillator */
			 sloop->minute_cnt=0;
			 if(LTBI.lotype==RB)
			 sloop->shift_cnt=((sloop->Prop_Tau+60)/22);
			 else
			 sloop->shift_cnt=((sloop->Prop_Tau+60)/20);
			 sloop->jamloop=FALSE;
			 smi.freq_state_cor=Cal_Data.LO_Freq_Cal;
			 ECD.freq_state_cor=Cal_Data.LO_Freq_Cal;
			 /**** jam previous 24 hour holdover data base*/
			 temp=Cal_Data.LO_Freq_Cal;
			 HMS.Freq_Sum_Daily=0.0;
			 for (i=24;i>0;i--)
			 {
				j=i%24;
				HMS.Daily_Freq_Log[j]=temp;
				HMS.Holdover_Base[j]=temp;
				HMS.Holdover_Drift[j]=Cal_Data.LO_Drift_Cal;
				temp -=Cal_Data.LO_Drift_Cal;
			 }
			 HMS.Cur_Hour=0;
			 HMS.Cur_Day=0;
			 HMS.Nsum_Hour=0;
			 HMS.Nsum_Day=0;
			 HMS.Freq_Sum_Daily=0.0;
			 HMS.Predict_Err24Hr=0.0;
			 HMS.Long_Term_Drift=Cal_Data.LO_Drift_Cal;
			 HMS.Drift_Sum_Weekly=0.0;
			 HMS.Osc_Freq_State=Cal_Data.LO_Freq_Cal;
			 HMS.Restart=TRUE;
			 /**** update ATC calibration data structure*****/
			 if (Cal_Data.ATC_TC_Cal!=0.0)
			 {
			  ATC.TC_avg=(double)(Cal_Data.ATC_TC_Cal);
			  ATC.Stslew1_pos=(ATCMINSLEW*2.0);ATC.Stslew1_neg= (-ATCMINSLEW*2.0) ;
			  ATC.Stslew2_pos=(ATCMINSLEW*2.0);ATC.Stslew2_neg= (-ATCMINSLEW*2.0);
			  ATC.Sfslew1_pos=ATC.Stslew1_pos*ATC.TC_avg;
			  ATC.Sfslew1_neg=ATC.Stslew1_neg*ATC.TC_avg;
			  ATC.Sfslew2_pos=ATC.Sfslew1_pos; ATC.Sfslew2_neg=ATC.Sfslew1_neg;
			  ATC.TC_pos= ATC.TC_avg;    ATC.TC_neg= ATC.TC_avg;
			 }
			 for(i=0;i<MAXTEMPRECORDS;i++)
			 {
            if((Cal_Data.Freq_Comp[i]<100)||(Cal_Data.Freq_Comp[i]>-100))
				Tcomp.Freq_Comp[i]=(short)Cal_Data.Freq_Comp[i];
			 }
			 #if(TARGET==CLPR)
			 Tcomp.Slew_Comp= (short)Cal_Data.Freq_Comp[255]; /*use last record for
																				 dynamic temperature
                                                             correction*/
			 #endif		    
	
		  
		  }
        /*update NCO to best initial estimate*/
//          Cal_Data.LO_Freq_Cal=0.0; 
//		  Cal_Data.LO_Freq_Cal_Factory=0.0;
		  NCO_Convert(smi.freq_state_cor);
		  NCO_Control();
		  refresh_smart_icw(); //GPZ make sure smart_icw is ready for freerun		  
		  
}
/***********************************************************************
Module to set oscillator build related parameters

*************************************************************************/
void init_build()
{
		/***** take care of build info first ********/       
		LTBI.lotype= GetOscillatorType();
	   if(LTBI.lotype==RB) /* 1 is 10 MHz Rb build*/
		{
	     /* PPS control */

        LTBI.maxphaseslew=MAXPHASESLEW_RB; 
        LTBI.minphaseslew=MINPHASESLEW_RB; 
        LTBI.nocrossingcnt=NOCROSSINGCNT_RB; 

	     /*GPS firewall*/

        LTBI.gfreqmax= GFREQMAX_RB; 
        LTBI.gdriftmax= GDRIFTMAX_RB; 

	     /* LO control loop */

	     LTBI.single_prop_tau= SINGLE_PROP_TAU_RB;
	     LTBI.single_int_tau= SINGLE_INT_TAU_RB;
	     LTBI.single_dint_tau= SINGLE_DINT_TAU_RB;
	     LTBI.single_accel= SINGLE_ACCEL_RB;
	     LTBI.single_recal= SINGLE_RECAL_RB;
	     LTBI.gear_space= GEAR_SPACE_RB;

	     /* NCO control */
	     LTBI.nco_center=NCO_CENTER_10;
	     LTBI.nco_max=NCO_MAX_10;
	     LTBI.nco_min=NCO_MIN_10;
	     LTBI.nco_range=NCO_RANGE_10;
	     LTBI.nco_convert=NCO_CONVERT_10;
	     LTBI.dither_bias=DITHER_BIAS_10;
		}
		else if(LTBI.lotype==DOCXO)
		{
	     /* PPS control */

        LTBI.maxphaseslew=MAXPHASESLEW_XO; 
        LTBI.minphaseslew=MINPHASESLEW_XO; 
        LTBI.nocrossingcnt=NOCROSSINGCNT_XO; 

	     /*GPS firewall*/

        LTBI.gfreqmax= GFREQMAX_XO; 
        LTBI.gdriftmax= GDRIFTMAX_XO; 

	     /* LO control loop */

	     LTBI.single_prop_tau= SINGLE_PROP_TAU_DO;
	     LTBI.single_int_tau= SINGLE_INT_TAU_DO;
	     LTBI.single_dint_tau= SINGLE_DINT_TAU_DO;
	     LTBI.single_accel= SINGLE_ACCEL_DO;
	     LTBI.single_recal= SINGLE_RECAL_DO;
	     LTBI.gear_space= GEAR_SPACE_DO;

	     /* NCO control */
	     LTBI.nco_center=NCO_CENTER_51;
	     LTBI.nco_max=NCO_MAX_51;
	     LTBI.nco_min=NCO_MIN_51;
	     LTBI.nco_range=NCO_RANGE_51;
	     LTBI.nco_convert=NCO_CONVERT_51;
	     LTBI.dither_bias=DITHER_BIAS_51;
		} /*end setting lo type configuration info*/
		else
		{
	     /* PPS control */

        LTBI.maxphaseslew=MAXPHASESLEW_XO; 
        LTBI.minphaseslew=MINPHASESLEW_XO; 
        LTBI.nocrossingcnt=NOCROSSINGCNT_XO; 

	     /*GPS firewall*/

        LTBI.gfreqmax= GFREQMAX_XO; 
        LTBI.gdriftmax= GDRIFTMAX_XO; 

	     /* LO control loop */

	     LTBI.single_prop_tau= SINGLE_PROP_TAU_XO;
	     LTBI.single_int_tau= SINGLE_INT_TAU_XO;
	     LTBI.single_dint_tau= SINGLE_DINT_TAU_XO;
	     LTBI.single_accel= SINGLE_ACCEL_XO;
	     LTBI.single_recal= SINGLE_RECAL_XO;
	     LTBI.gear_space= GEAR_SPACE_XO;

	     /* NCO control */
	     LTBI.nco_center=NCO_CENTER_51;
	     LTBI.nco_max=NCO_MAX_51;
	     LTBI.nco_min=NCO_MIN_51;
	     LTBI.nco_range=NCO_RANGE_51;
	     LTBI.nco_convert=NCO_CONVERT_51;
	     LTBI.dither_bias=DITHER_BIAS_51;
		} /*end setting lo type configuration info*/
}
/*****************************************
This routine forces the local control
into an accelerated gear 4 operation
It should be called when the temperature
oven current is at either a hot or cold
extreme
 
 Should call out of a 1 second task to
 ensure no interference with one second BT3_fll
 task. NOTE NEED TO REWRITE TO BE BUILD DEPENDENT
******************************************/
void Temp_Accel()
{
	double scale;
	if(sloop->Cal_Mode==4)
	{
			 /****** force recalibration to not advance*****/
			 sloop->minute_cnt=0;

	}
	else if (sloop->Cal_Mode>4)
	{
		sloop->Prop_Tau=SINGLE_PROP_TAU_DO/SINGLE_RECAL_DO;
		sloop->Int_Tau= SINGLE_INT_TAU_DO /SINGLE_RECAL_DO;
		sloop->Dint_Tau=SINGLE_DINT_TAU_DO;
		sloop->Prop_Tau *=GEAR_SPACE_DO;
		sloop->Prop_Tau *=GEAR_SPACE_DO;
		sloop->Int_Tau  *=GEAR_SPACE_DO;
		sloop->Int_Tau  *=GEAR_SPACE_DO;
		sloop->Propf1=1.0/sloop->Prop_Tau;
		sloop->Propf2=1.0-sloop->Propf1;

		while(sloop->Cal_Mode>4)
		{

			sloop->Int_State/=GEAR_SPACE_DO;
			/****** reuse scale to correct for decreased delay in prop filt*/
			scale = sloop->Int_State*(GEAR_SPACE_DO-1.0);
			sloop->Prop_State+=scale;
			sloop->Sprop2=sloop->Prop_State;
			sloop->Sprop1+=(2.0*scale);
			sloop->Cal_Mode--;
		}

		sloop->Cal_Mode=4;
		sloop->Recalibrate=TRUE;

		smi.sloop_prop_tc=sloop->Prop_Tau;
		smi.sloop_int_tc =sloop->Int_Tau;
		smi.Overide_Mode=1;

	}

}
// GPZ single point of entry to init Bestime from
// main RTE application	


