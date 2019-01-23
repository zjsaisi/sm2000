/***************************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/14 00:05:06PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_mgr.c_v  $
$Modtime:   06 Feb 1999 12:38:50  $
$Revision: 1.6 $


                            CLIPPER SOFTWARE
                                 
                                BT3_MGR.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module performs managment of external clock control states
                  and internal transistional states.

MODIFICATION HISTORY:

$Log: bt3_mgr.c  $
Revision 1.6 2010/10/14 00:05:06PDT Jining Yang (jyang) 
Merged from 1.5.1.1.
Revision 1.5.1.1 2010/08/04 23:24:18PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.5 2009/07/13 14:10:32PDT Zheng Miao (zmiao) 
GPZ change
Revision 1.4 2009/06/11 10:53:35PDT zmiao 
George's change
Revision 1.3 2009/03/24 09:25:23PDT zmiao 
Don't change cal number during first hour of operation.
Revision 1.2 2008/04/01 08:32:54PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.1 2007/12/06 11:41:06PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.9 2006/08/24 13:19:23PDT gzampet 
added recal using subtending correction frequency
Revision 1.8 2006/05/11 16:08:38PDT gzampet 
tweaked to improve transient protection
Revision 1.7 2006/05/03 13:28:04PDT gzampet 
tweak to make sure we store factory cal in flash
Revision 1.3 2006/04/28 08:05:26PDT gzampet 
added extended phase slew control
Revision 1.2 2006/04/19 17:13:17PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.6 2004/07/07 14:13:58PDT gzampet 

Revision 1.5  2004/07/01 18:37:43Z  gzampet
Revision 1.4  2004/07/01 17:43:18Z  gzampet
modifiy alarm report to include gear
Revision 1.3  2004/06/23 20:48:12Z  gzampet
Revision 1.2  2004/06/23 20:47:10Z  gzampet
merged bt3_mgr.c
Revision 1.1  2003/04/15 19:46:20Z  gzampet
Initial revision
Revision 1.7  2002/09/13 21:17:28Z  gzampet
Clean up on the cckb ecw and pmask operation
Revision 1.6  2002/09/06 00:31:00Z  gzampet
Changed cck_ok routine to use the "use" flag to
instead of the mon flag...
Revision 1.5  2002/09/03 18:56:14Z  zmiao
Function name change.
Revision 1.4  2002/08/22 23:33:20Z  zmiao
Added temp slew event reporting
Revision 1.3  2002/08/16 19:13:00Z  gzampet
Added support for provision of CCKB using both hardware switch and
User messaging
Revision 1.2  2002/05/31 17:59:53Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
 * 
 *    Rev 1.0   06 Feb 1999 15:41:46   S.Scott
 * TS3000 initial release.

***************************************************************************/

/******************** INCLUDE FILES ************************/

/******************** LOCAL DEFINES ************************/

/************ PROTOTYPES EXTERNAL FUNCTIONS ****************/

/************ PROTOTYPES INTERNAL FUNCTIONS ****************/

/******************** EXTERNAL DATA ************************/

/******************** INTERNAL DATA ************************/

/************************ TABLE ****************************/
       
/************************ CODE *****************************/


/*****************************************************************************
*     BT3 Core Clock Engine (BT3)
*     Module: bt3_mgr.c
*     Description: Module performs managment of external clock control states
*                  and internal transistional states.
*     Version 1.0  9-15-97 New Source Module Created. ASMM Routine moved from
*                          bt3_tsk.c source module to this module for ease of
*                          future management. Previous History can be found in
*                          the bt3_tsk.c (formerly mcca_tsk.c) source file.
*
*      Version 2.1 5-13-98
*                         Added Command_Mgr() routine to managed input commands
*                         mailboxes
*****************************************************************************/

#include "bt3_def.h"
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#endif
/******** global data structures *********/
#include "bt3_dat.h"   /*main file for global data declarations*/
#include "bt3_msg.h"   /*main file for message buffer templates*/
#include "bt3_shl.h"   /*main file for message shell templates*/
//#include "types.h"
//#include "almlog.h"

struct Status_Registers SReg;
extern struct Span_Meas_Data *pmda, *pmdb;
extern struct Remote_Oscillator_Meas_Data *prma,*prmb;
extern struct Temperature_Meas_Data *ptmd;
extern struct GPS_Measurement_Data   GM;
extern struct Plevel_Meas_Data      *ppmd;
extern struct Alevel_Meas_Data      *pamd;      
extern struct Dual_Input_FLL_Control_System DloopSA,DloopSB,DloopRA,DloopRB,*ploop;
extern struct Single_Input_FLL_Control_System Lloop,*sloop;
extern struct Phase_Compensation Pcomp;
extern struct Tracking_Stats TrkStats;
extern double Tracking_Success_rec;  /*One second timing task copy of tracking success
										         rate */     
extern double g_freqMax	;									         
#if REMOTE
extern struct Single_Input_FLL_Control_System Rloop;
#endif                                             


extern struct Threshold_Management TM;
extern struct ASM_Management ASMM; /*State Management Structure */
extern struct Shared_Memory_Interface smi;
extern struct Holdover_Management HMS,HMSSU;
extern struct Calibration_Data  Cal_Data;
extern struct LO_TYPE_BUILD_INFO LTBI;
#if PCSIM
extern unsigned long ptime; /* print log time stamp in cumulative minutes*/
#else
unsigned long ptime;  /*make dummy symbol for embedded call*/
#endif
extern unsigned short printctrl;
extern unsigned short  pcount; /*manage rate of print logging*/  
extern unsigned short phase_control; //phase control for PPS set to zero for release!
extern double ken_MPLL_freq ;
extern struct MSG_EST_SRPT MSG_EST_SRPT_A, MSG_EST_SRPT_B;
extern struct MSG_EST_ASM  MSG_EST_ASM_A,  MSG_EST_ASM_B;
extern struct MSG_FLL_CAL MSG_FLL_CAL_A, MSG_FLL_CAL_B;
struct MSG_CAL_EST MSG_CAL_EST_A;
struct MSG_CAL_FLL MSG_CAL_FLL_A;
struct MSG_Alarm_Status_Report MSG_ASR;
struct MSG_ECW_Command  MSG_ECW_C;
struct MSG_ECW_Report   MSG_ECW_R;
struct MSG_RCW_Command  MSG_RCW_C;
struct MSG_RCW_Report   MSG_RCW_R;
struct MSG_PMASK_Command  MSG_PMASK_C;
struct MSG_PMASK_Report   MSG_PMASK_R;
struct MSG_QMASK_Command  MSG_QMASK_C;
struct MSG_QMASK_Report   MSG_QMASK_R;    

unsigned short Holdover_Recovery_Jam;
unsigned short Reinit_Track_Success;
unsigned short Bridge_Count=0;

/******  external functions declared ******/
extern void Recalibrate(void);
extern void Init_Tasks(void);
extern void Init_Shared_Mem_Int(void);
extern void BT3_MSG_Init(void);
//extern void ShellInit(void);
//extern void Put_Prov_Data_RAM(void);
//extern void Data_Flash_Update(void);
//extern void	Get_Flash_Cal_Data(void);       

unsigned short Get_Gear(void);
extern void	init_cal(void);   
void Cal_Data_Reset(void);   
void Cal_Data_Subtend_Reset(void);
                        
/******************************************************************
 Leaky Bucket Management Macro
 *****************************************************************/
#if PCSIM
#define LBM(LBUCKET,LFLAG,LBFLAG,LBDES)                                      \
{                                                                            \
	if(LBUCKET< temp)                                                         \
	{                                                                         \
		LFLAG=FALSE;                                                           \
		if(pcount<PRINTMAX && LBFLAG)                                          \
		{                                                                      \
			LBFLAG=FALSE;                                                       \
			if (printctrl)                                                      \
			{                                                                   \
			pcount++;                                                           \
			printf("%s%5ld:ASM:ALARM-%s\n",PHEAD,ptime,LBDES);                  \
			}                                                                   \
		}                                                                      \
	}                                                                         \
	else                                                                      \
	{                                                                         \
		if(pcount<PRINTMAX && (LBFLAG==FALSE))                                 \
		{                                                                      \
			LBFLAG=TRUE;                                                        \
			if (printctrl)                                                      \
			{                                                                   \
			pcount++;                                                           \
			printf("%s%5ld:ASM:CLEAR-%s\n",PHEAD,ptime,LBDES);                  \
			}                                                                   \
		}                                                                      \
	}                                                                         \
}
#else
#define LBM(LBUCKET,LFLAG,LBFLAG,LBDES)                                      \
{                                                                            \
	if(LBUCKET< temp)                                                         \
	{                                                                         \
		LFLAG=FALSE;                                                           \
		if(pcount<PRINTMAX && LBFLAG)                                          \
		{                                                                      \
			LBFLAG=FALSE;                                                       \
			if (printctrl)                                                      \
			{                                                                   \
			pcount++;                                                           \
			}                                                                   \
		}                                                                      \
	}                                                                         \
	else                                                                      \
	{                                                                         \
		if(pcount<PRINTMAX && (LBFLAG==FALSE))                                 \
		{                                                                      \
			LBFLAG=TRUE;                                                        \
			if (printctrl)                                                      \
			{                                                                   \
			pcount++;                                                           \
			}                                                                   \
		}                                                                      \
	}                                                                         \
}
#endif
/******************************** local functions***************************/
void ASM(void);
void Status_Report(void);
void Shared_Mem_Override(void);
void Command_Mgr(void);
void Cal_Data_Update(void);
void	BT3_reset( void );
void ASM()
{
	short temp,i;
	/******* local input message data *****/
	struct MSG_EST_ASM *pestrec;
	short SPAphase_rec, SPAfreq_rec, SPAdrift_rec;
	short SPBphase_rec, SPBfreq_rec, SPBdrift_rec;
	short ROAphase_rec, ROAfreq_rec, ROAdrift_rec;
	short ROBphase_rec, ROBfreq_rec, ROBdrift_rec;
	short TPstep_rec, TPslew_rec;
	short PLstep_rec, PLrange_rec;
	short ALstep_rec, ALrange_rec;

	/* receive 250 ms input message*/
	if(MSG_EST_ASM_A.Ready) pestrec= &(MSG_EST_ASM_A);
	else pestrec= &(MSG_EST_ASM_B);
	SPAphase_rec=pestrec->SPAphase_bkt;
	SPBphase_rec=pestrec->SPBphase_bkt;
	SPAfreq_rec=pestrec->SPAfreq_bkt;
	SPBfreq_rec=pestrec->SPBfreq_bkt;
	SPAdrift_rec=pestrec->SPAdrift_bkt;
	SPBdrift_rec=pestrec->SPBdrift_bkt;
	ROAphase_rec=pestrec->ROAphase_bkt;
	ROBphase_rec=pestrec->ROBphase_bkt;
	ROAfreq_rec=pestrec->ROAfreq_bkt;
	ROBfreq_rec=pestrec->ROBfreq_bkt;
	ROAdrift_rec=pestrec->ROAdrift_bkt;
	ROBdrift_rec=pestrec->ROBdrift_bkt;
	TPstep_rec=pestrec->TPstep_bkt;
	TPslew_rec=pestrec->TPslew_bkt;
	PLstep_rec=pestrec->PLstep_bkt;
	PLrange_rec=pestrec->PLrange_bkt;
	ALstep_rec=pestrec->ALstep_bkt;
	ALrange_rec=pestrec->ALrange_bkt;
	pestrec->Ready=FALSE;
	/******* complete message reception *****/

	/***** GPS Leaky Bucket Alarm Managment **/
	ASMM.GPS_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(GM.Gfreq_bucket ,ASMM.GPS_Flag,ASMM.GPS_Freq_Bucket_Flag,"GPS_Freq_Bucket_Thres");
	LBM(GM.Gdrift_bucket,ASMM.GPS_Flag,ASMM.GPS_Drift_Bucket_Flag,"GPS_Drift_Bucket_Thres");
	ASMM.GPS_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(GM.Gfreq_bucket,ASMM.GPS_Hflag,ASMM.GPS_Freq_Bucket_Hyst_Flag,"GPS_Freq_Bucket_Thres+Hyst");
	LBM(GM.Gdrift_bucket,ASMM.GPS_Hflag,ASMM.GPS_Drift_Bucket_Hyst_Flag,"GPS_Drift_Bucket_Thres+Hyst");
	/******* END GPS Alarm Management ********/
	ASMM.SPA_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(SPAphase_rec,ASMM.SPA_Hflag,ASMM.SPA_Phase_Bucket_Hyst_Flag,"SPANA_Phase_Bucket_Thres+Hyst");
	LBM(SPAfreq_rec,ASMM.SPA_Hflag,ASMM.SPA_Freq_Bucket_Hyst_Flag,"SPANA_Freq_Bucket_Thres+Hyst");
	LBM(SPAdrift_rec,ASMM.SPA_Hflag,ASMM.SPA_Drift_Bucket_Hyst_Flag,"SPANA_Drift_Bucket_Thres+Hyst");
	ASMM.SPA_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(SPAphase_rec,ASMM.SPA_Flag,ASMM.SPA_Phase_Bucket_Flag,"SPANA_Phase_Bucket_Thres");
	LBM(SPAfreq_rec,ASMM.SPA_Flag,ASMM.SPA_Freq_Bucket_Flag,"SPANA_Freq_Bucket_Thres");
	LBM(SPAdrift_rec,ASMM.SPA_Flag,ASMM.SPA_Drift_Bucket_Flag,"SPANA_Drift_Bucket_Thres");
	ASMM.SPB_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(SPBphase_rec,ASMM.SPB_Hflag,ASMM.SPB_Phase_Bucket_Hyst_Flag,"SPANB_Phase_Bucket_Thres+Hyst");
	LBM(SPBfreq_rec,ASMM.SPB_Hflag,ASMM.SPB_Freq_Bucket_Hyst_Flag,"SPANB_Freq_Bucket_Thres+Hyst");
	LBM(SPBdrift_rec,ASMM.SPB_Hflag,ASMM.SPB_Drift_Bucket_Hyst_Flag,"SPANB_Drift_Bucket_Thres+Hyst");
	ASMM.SPB_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(SPBphase_rec,ASMM.SPB_Flag,ASMM.SPB_Phase_Bucket_Flag,"SPANB_Phase_Bucket_Thres");
	LBM(SPBfreq_rec,ASMM.SPB_Flag,ASMM.SPB_Freq_Bucket_Flag,"SPANB_Freq_Bucket_Thres");
	LBM(SPBdrift_rec,ASMM.SPB_Flag,ASMM.SPB_Drift_Bucket_Flag,"SPANB_Drift_Bucket_Thres");
	ASMM.ROA_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(ROAphase_rec,ASMM.ROA_Hflag,ASMM.ROA_Phase_Bucket_Hyst_Flag,"ROA_Phase_Bucket_Thres+Hyst");
	LBM(ROAfreq_rec,ASMM.ROA_Hflag,ASMM.ROA_Freq_Bucket_Hyst_Flag,"ROA_Freq_Bucket_Thres+Hyst");
	LBM(ROAdrift_rec,ASMM.ROA_Hflag,ASMM.ROA_Drift_Bucket_Hyst_Flag,"ROA_Drift_Bucket_Thres+Hyst");
	ASMM.ROA_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(ROAphase_rec,ASMM.ROA_Flag,ASMM.ROA_Phase_Bucket_Flag,"ROA_Phase_Bucket_Thres");
	LBM(ROAfreq_rec,ASMM.ROA_Flag,ASMM.ROA_Freq_Bucket_Flag,"ROA_Freq_Bucket_Thres");
	LBM(ROAdrift_rec,ASMM.ROA_Flag,ASMM.ROA_Drift_Bucket_Flag,"ROA_Drift_Bucket_Thres");
	ASMM.ROB_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(ROBphase_rec,ASMM.ROB_Hflag,ASMM.ROB_Phase_Bucket_Hyst_Flag,"ROB_Phase_Bucket_Thres+Hyst");
	LBM(ROBfreq_rec,ASMM.ROB_Hflag,ASMM.ROB_Freq_Bucket_Hyst_Flag,"ROB_Freq_Bucket_Thres+Hyst");
	LBM(ROBdrift_rec,ASMM.ROB_Hflag,ASMM.ROB_Drift_Bucket_Hyst_Flag,"ROB_Drift_Bucket_Thres+Hyst");
	ASMM.ROB_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(ROBphase_rec,ASMM.ROB_Flag,ASMM.ROB_Phase_Bucket_Flag,"ROB_Phase_Bucket_Thres");
	LBM(ROBfreq_rec,ASMM.ROB_Flag,ASMM.ROB_Freq_Bucket_Flag,"ROB_Freq_Bucket_Thres");
	LBM(ROBdrift_rec,ASMM.ROB_Flag,ASMM.ROB_Drift_Bucket_Flag,"ROB_Drift_Bucket_Thres");
	ASMM.TP_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(TPstep_rec,ASMM.TP_Hflag,ASMM.TP_Step_Bucket_Hyst_Flag,"TP_Step_Bucket_Thres+Hyst");
	LBM(TPslew_rec,ASMM.TP_Hflag,ASMM.TP_Slew_Bucket_Hyst_Flag,"TP_Slew_Bucket_Thres+Hyst");
	ASMM.TP_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(TPstep_rec,ASMM.TP_Flag,ASMM.TP_Step_Bucket_Flag,"TP_Step_Bucket_Thres");
	LBM(TPslew_rec,ASMM.TP_Flag,ASMM.TP_Slew_Bucket_Flag,"TP_Slew_Bucket_Thres");
	ASMM.PL_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(PLstep_rec,ASMM.PL_Hflag,ASMM.PL_Step_Bucket_Hyst_Flag,"PL_Step_Bucket_Thres+Hyst");
	LBM(PLrange_rec,ASMM.PL_Hflag,ASMM.PL_Range_Bucket_Hyst_Flag,"PL_Range_Bucket_Thres+Hyst");
	ASMM.PL_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(PLstep_rec,ASMM.PL_Flag,ASMM.PL_Step_Bucket_Flag,"PL_Step_Bucket_Thres");
	LBM(PLrange_rec,ASMM.PL_Flag,ASMM.PL_Range_Bucket_Flag,"PL_Range_Bucket_Thres");
	ASMM.AL_Hflag= TRUE;
	temp = LBUCKETTHRES+LBUCKETHYS;
	LBM(ALstep_rec,ASMM.AL_Hflag,ASMM.AL_Step_Bucket_Hyst_Flag,"AL_Step_Bucket_Thres+Hyst");
	LBM(ALrange_rec,ASMM.AL_Hflag,ASMM.AL_Range_Bucket_Hyst_Flag,"AL_Range_Bucket_Thres+Hyst");
	ASMM.PL_Flag= TRUE;
	temp = LBUCKETTHRES;
	LBM(ALstep_rec,ASMM.AL_Flag,ASMM.AL_Step_Bucket_Flag,"AL_Step_Bucket_Thres");
	LBM(ALrange_rec,ASMM.AL_Flag,ASMM.AL_Range_Bucket_Flag,"AL_Range_Bucket_Thres");
	switch(smi.BT3_mode)
	{
		case GPS_Warmup:
		/******************************************************
		In GPS base BT3 application the GPS warmup is completed
		when
		A) 1 PPS is acquired.
		 The following events must occur to achieve 1 PPS acquistion
		 1) 1PPS jam must align timing within threshold
		 2) GPS Single Control Loop Must acheive SMI.LO_State =2 indicating Freq Lock
		 3) LO_State is only advance if there are no GPS ALARMS (on freq no drift Kalman Happy ...)
		B) Oven Current in LO is stable TP_Reg has no standing alarms
		*************************************/
	  /**** complete warmup when One PPS is first acquired and stable oven
	       and gear is past level 2 stop jamming of 1 PPS ****/
//	  if( !(smi.One_PPS_High)&&!(smi.TP_Reg)&&(Lloop.Cal_Mode>2)) simplify for TimeProvider
	  if( (Lloop.Cal_Mode>2))
		{
			smi.BT3_mode=GPS_Normal;
			smi.BT3_mode_dur=0;  
			Bridge_Count=0;   //GPZ June 2010
			#if REMOTE
			Rloop.Freq_Err_Sum =0.0;
			Rloop.Freq_Sum_Cnt=0;
			#endif
			#if PRINTLOG
			  printf("%s%5ld:ASM:State Change GPS_Warmup to GPS_Normal\n",PHEAD,ptime);
			#endif
		}
		break;
		case GPS_Normal:
		/*********************************************************************

		Reasons to switch to GPS Holdover are :
		1) An Alarm condition exists on GPS reference

		********************************************************************/
		  if(ASMM.GPS_Flag==FALSE&&(Lloop.Trace_Skip==0))//GPZ June 2010 no holdover during trace skip
		  {           
		  			Bridge_Count++;  /* added three minute holdoff before entering holdover*/
		  			if(Bridge_Count>180)
		  			{
						smi.BT3_mode= Holdover;
						smi.BT3_mode_dur=0;

						#if PRINTLOG
						printf("%s%5ld:ASM:State Change GPS_Normal to GPS_Holdover\n",PHEAD,ptime);
						#endif
					}
		  }   
		Bridge_Count=0;   //GPZ June 2010
		break;
		case Holdover:
		/*******************************************************************
		Reasons to re-enter normal state:
		1) GPS clears high water mark

		********************************************************************/
		  if(ASMM.GPS_Hflag)
		  {   
				  /************ GPZ new logic to control extended holdver****/
				  /*****************************
					Use mode duration as criteria for extended holdover
					For OCXO use 4 hours
					For Rb use 24 hours
					If extended holdover period
					  1) Set Holdover_Recovery_Jam Flag to allow
					     a one time jam of the PPS if it is out of tolerance
					  2) Set Flag to re-initialize tracking success to
					     75%
					*****************************/
				   if(LTBI.lotype==OCXO||LTBI.lotype==DOCXO)
					{
					  if(smi.BT3_mode_dur > 240)
					  {
							Holdover_Recovery_Jam=120;
							Reinit_Track_Success = TRUE;
					  }
					}
					else 
					{
						if(smi.BT3_mode_dur > 1440)
						{
							Holdover_Recovery_Jam=120;
							Reinit_Track_Success = TRUE;
						}
					}
					if(Reinit_Track_Success ==TRUE)
					{
						 Reinit_Track_Success = FALSE;
						 for(i=0;i<24;i++)
						 {
							TrkStats.Good_Track_Min[i]=42;
						 }
						 TrkStats.Total_Good_Min=1008;
						 TrkStats.Cur_Hour=0;
						 TrkStats.Daily_Track_Success=70.0;
					}   
					Bridge_Count=0;
					smi.BT3_mode=GPS_Normal;
					smi.BT3_mode_dur=0;
					#if PRINTLOG
					  printf("%s%5ld:ASM:State Change GPS_Holdover to GPS_Normal\n",PHEAD,ptime);
					#endif
		  }		  
		break;
		case Temp_Transient:
		break;
		default:;
	}
	ASMM.Sec_Cnt++;
	if(ASMM.Sec_Cnt >59)
	{
		/* update one minute state duration */
		ASMM.Sec_Cnt=0;
		smi.BT3_mode_dur ++;
		if(smi.BT3_mode_dur > MODE_DUR_MAX) smi.BT3_mode_dur=MODE_DUR_MAX;
		#if PRINTLOG
		ptime++;
		#endif
		/* call recalibration function if needed */ 
		temp= DloopSA.Recalibrate || DloopSB.Recalibrate ||DloopRA.Recalibrate
			 ||DloopRB.Recalibrate||Lloop.Recalibrate;
		#if REMOTE
		temp= temp||Rloop.Recalibrate;
		#endif
		if(temp) Recalibrate();
	}
}


/******************************************************************
Module called as part of 250 msec loop to update common "shared"
memory status registers. The following status registers are defined:
SPA_Reg                   GPS_Reg
0 SP_Freq_Event           0 GPS_Freq_Event
1 SP_Drift_Event          1 GPS_Drift_Event
2 SP_Freq_Alarm           2 GPS_Freq_Alarm
3 SP_Drift_Alarm          3 GPS_Drift_Alarm
4 SP_Jitter_Alarm         4 GPS_Time_Inaccuracy_Event
5 SP_Present_Alarm        5 GPS_Bias_Flag_Invalid
6 SP_Phase_Event          6 GPS_Tracking_Success_Alarm
7 SP_Phase_Alarm          7 Spare
8 SP_Qual_Event
SPB_Reg                   RO_Reg
0 SP_Freq_Event           0 RO_Freq_Event
1 SP_Drift_Event          1 RO_Drift_Event
2 SP_Freq_Alarm           2 R0_Freq_Alarm
3 SP_Drift_Alarm          3 RO_Drift_Alarm
4 SP_Jitter_Alarm         4 RO_Jitter_Alarm
5 SP_Present_Alarm        5 RO_Present_Alarm
6 SP_Phase_Event          6 RO_Phase_Event
7 SP_Phase_Alarm          7 RO_Phase_Alarm
8 SP_Qual_Event           8 SP_Qual_Event
TP_Reg                    PL_Reg
0 TP_Step Event           0 PL_Step_Event
1 TP_Slew Event           1 PL_Range Event
2 TP_Step Alarm           2 PL_Step_Alarm
3 TP_Slew Alarm           3 PL_Range_Alarm
4 TP_Jitter_Alarm         4 PL_Jitter_Alarm
5 TP_Present_Alarm        5 PL_Present_Alarm
6 TP_Range_Alarm (kjh)
March 16, 1998 Ver 2.0
		1) Added SR register structure to eventually replace SMI
			interface
      

*******************************************************************/
void Status_Report()
{
	/******* local input message data *****/
	struct MSG_EST_SRPT *pestrec;
	unsigned short SPA_Reg_Events_rec;
	unsigned short SPB_Reg_Events_rec;
	unsigned short ROA_Reg_Events_rec;
	unsigned short ROB_Reg_Events_rec;
	unsigned short TP_Reg_Events_rec;
	unsigned short PL_Reg_Events_rec;
	unsigned short AT_Reg_Events_rec;
	/* receive 250 ms input message*/
	if(MSG_EST_SRPT_A.Ready) pestrec= &(MSG_EST_SRPT_A);
	else pestrec= &(MSG_EST_SRPT_B);
	SPA_Reg_Events_rec=pestrec->SPA_Reg_Events;
	SPB_Reg_Events_rec=pestrec->SPB_Reg_Events;
	ROA_Reg_Events_rec=pestrec->ROA_Reg_Events;
	ROB_Reg_Events_rec=pestrec->ROB_Reg_Events; 
	TP_Reg_Events_rec=pestrec->TP_Reg_Events;
	PL_Reg_Events_rec=pestrec->PL_Reg_Events;
	AT_Reg_Events_rec=pestrec->AT_Reg_Events;
	pestrec->Ready=FALSE;
	/******* complete message reception *****/
	if(smi.CLR_Lreg)
	{
		smi.CLR_Lreg=FALSE;
		smi.SPA_Lreg=0;
		smi.SPB_Lreg=0;
		smi.ROA_Lreg=0;
		smi.ROB_Lreg=0;
		smi.GPS_Lreg=0;
		smi.TP_Lreg=0;
		smi.PL_Lreg=0;
		smi.AT_Lreg=0;
	}
	SReg.SPA_Reg=SPA_Reg_Events_rec;
	if(ASMM.SPA_Freq_Bucket_Flag==FALSE)  SReg.SPA_Reg = SReg.SPA_Reg | 4;
	if(ASMM.SPA_Drift_Bucket_Flag==FALSE) SReg.SPA_Reg = SReg.SPA_Reg | 8;
	if(ASMM.SPA_Phase_Bucket_Flag==FALSE) SReg.SPA_Reg = SReg.SPA_Reg | 128;
	SReg.SPA_Mreg = SReg.SPA_Mreg|SReg.SPA_Reg;
	smi.SPA_Lreg = smi.SPA_Lreg|SReg.SPA_Reg;
	smi.SPA_Reg  = SReg.SPA_Reg;

	SReg.SPB_Reg=SPB_Reg_Events_rec;
	if(ASMM.SPB_Freq_Bucket_Flag==FALSE)  SReg.SPB_Reg = SReg.SPB_Reg | 4;
	if(ASMM.SPB_Drift_Bucket_Flag==FALSE) SReg.SPB_Reg = SReg.SPB_Reg | 8;
	if(ASMM.SPB_Phase_Bucket_Flag==FALSE) SReg.SPB_Reg = SReg.SPB_Reg | 128;
	SReg.SPB_Mreg  = SReg.SPB_Mreg|SReg.SPB_Reg;
	smi.SPB_Lreg = smi.SPB_Lreg|SReg.SPB_Reg;
	smi.SPB_Reg  = SReg.SPB_Reg;

	SReg.GPS_Reg=0;
	if(ASMM.GPS_Freq_Flag==FALSE)         SReg.GPS_Reg = SReg.GPS_Reg | 1;
	if(ASMM.GPS_Drift_Flag==FALSE)        SReg.GPS_Reg = SReg.GPS_Reg | 2;
	if(ASMM.GPS_Freq_Bucket_Flag==FALSE)  SReg.GPS_Reg = SReg.GPS_Reg | 4;
	if(ASMM.GPS_Drift_Bucket_Flag==FALSE) SReg.GPS_Reg = SReg.GPS_Reg | 8;
	if(ASMM.GPS_Acc_Flag==FALSE)          SReg.GPS_Reg = SReg.GPS_Reg | 16;
	if(ASMM.GPS_Bias_Flag==FALSE)         SReg.GPS_Reg = SReg.GPS_Reg | 32;
	// GPS 7/13/2009: Commented out.
	/* if(Tracking_Success_rec<40.0)          SReg.GPS_Reg = SReg.GPS_Reg | 64; */
	SReg.GPS_Mreg  = SReg.GPS_Mreg|SReg.GPS_Reg;
	smi.GPS_Lreg = smi.GPS_Lreg|SReg.GPS_Reg;
	smi.GPS_Reg  = SReg.GPS_Reg;
	SReg.ROA_Reg=ROA_Reg_Events_rec;
	if(ASMM.ROA_Freq_Bucket_Flag==FALSE)   SReg.ROA_Reg = SReg.ROA_Reg | 4;
	if(ASMM.ROA_Drift_Bucket_Flag==FALSE)  SReg.ROA_Reg = SReg.ROA_Reg | 8;
	if(ASMM.ROA_Phase_Bucket_Flag==FALSE)  SReg.ROA_Reg = SReg.ROA_Reg | 128;
	SReg.ROA_Mreg  = SReg.ROA_Mreg|SReg.ROA_Reg;
	smi.ROA_Lreg = smi.ROA_Lreg|SReg.ROA_Reg;
	smi.ROA_Reg  = SReg.ROA_Reg;
	SReg.ROB_Reg=ROB_Reg_Events_rec;
	if(ASMM.ROB_Freq_Bucket_Flag==FALSE)   SReg.ROB_Reg = SReg.ROB_Reg | 4;
	if(ASMM.ROB_Drift_Bucket_Flag==FALSE)  SReg.ROB_Reg = SReg.ROB_Reg | 8;
	if(ASMM.ROB_Phase_Bucket_Flag==FALSE)  SReg.ROB_Reg = SReg.ROB_Reg | 128;
	SReg.ROB_Mreg  = SReg.ROB_Mreg|SReg.ROB_Reg;
	smi.ROB_Lreg = smi.ROB_Lreg|SReg.ROB_Reg;
	smi.ROB_Reg  = SReg.ROB_Reg;
	SReg.TP_Reg=TP_Reg_Events_rec;
	if(ASMM.TP_Step_Bucket_Flag==FALSE)   SReg.TP_Reg = SReg.TP_Reg | 4;
	if(ASMM.TP_Slew_Bucket_Flag==FALSE)   SReg.TP_Reg = SReg.TP_Reg | 8;
	SReg.TP_Mreg  = SReg.TP_Mreg|SReg.TP_Reg;
	smi.TP_Lreg = smi.TP_Lreg|SReg.TP_Reg;
	smi.TP_Reg  = SReg.TP_Reg;
	SReg.PL_Reg=PL_Reg_Events_rec;
	if(ASMM.PL_Step_Bucket_Flag==FALSE)    SReg.PL_Reg = SReg.PL_Reg | 4;
	if(ASMM.PL_Range_Bucket_Flag==FALSE)   SReg.PL_Reg = SReg.PL_Reg | 8;
	SReg.PL_Mreg  = SReg.PL_Mreg|SReg.PL_Reg;
	smi.PL_Lreg = smi.PL_Lreg|SReg.PL_Reg;
	smi.PL_Reg  = SReg.PL_Reg;
	SReg.AT_Reg=AT_Reg_Events_rec;
	if(ASMM.AL_Step_Bucket_Flag==FALSE)    SReg.AT_Reg = SReg.AT_Reg | 4;
	if(ASMM.AL_Range_Bucket_Flag==FALSE)   SReg.AT_Reg = SReg.AT_Reg | 8;
	SReg.AT_Mreg  = SReg.AT_Mreg|SReg.AT_Reg;
	smi.AT_Lreg = smi.AT_Lreg|SReg.AT_Reg;
	smi.AT_Reg  = SReg.AT_Reg;
	/********* Update Alarm and Status  Report MailBox**/
	MSG_ASR.GPS_Reg=SReg.GPS_Reg;
	MSG_ASR.SPA_Reg=SReg.SPA_Reg;
	MSG_ASR.SPB_Reg=SReg.SPB_Reg;
	MSG_ASR.ROA_Reg=SReg.ROA_Reg;
	MSG_ASR.ROB_Reg=SReg.ROB_Reg;
	MSG_ASR.PL_Reg=SReg.PL_Reg;
	MSG_ASR.TP_Reg=SReg.TP_Reg;
	MSG_ASR.BT3_Mode=smi.BT3_mode;
	MSG_ASR.BT3_Mode_Dur=smi.BT3_mode_dur;
	MSG_ASR.Gear=Get_Gear();
	MSG_ASR.Ready=TRUE;
}
/********************************************************************
	This function permits user to override appropriated control loop
	setting by changing the common memory value (smi interface). If the
	new value is not in range it will be rejected and the shared memory returned
	to the current value
**********************************************************************/
void Shared_Mem_Override()
{
  register unsigned int testtc;
  register unsigned int ucurthres;   /*current threshold*/
  register unsigned int propthres; /*proposed threshold*/
 /**** Check for Override Input from Shared Memory Interface	 *****/
 /***** Single Loop Override Check ***/
  testtc = Lloop.Prop_Tau;
  if(testtc != smi.sloop_prop_tc)
		{
			testtc = smi.sloop_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 Lloop.Prop_Tau = (double)testtc;
				/* Lloop.Prop_State=0.0; /*jam to zero to minimize transient*/
			}
			else
			{
				 smi.sloop_prop_tc=(unsigned int)Lloop.Prop_Tau;
			}
  }
  testtc = Lloop.Int_Tau;
  if(testtc != smi.sloop_int_tc)
		{
			testtc = smi.sloop_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 Lloop.Int_Tau = (double)testtc;
			}
			else
			{
				 smi.sloop_int_tc=(unsigned int)Lloop.Int_Tau;
			}
		}
  testtc = Lloop.Dint_Tau;
  if(testtc != smi.sloop_dint_tc)
		{
			testtc = smi.sloop_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 Lloop.Dint_Tau = (double)testtc;
			}
			else
			{
				 smi.sloop_dint_tc=(unsigned int)Lloop.Dint_Tau;
			}
	}
 #if REMOTE
 /***** Remote Loop Override Check ***/
  testtc = Rloop.Prop_Tau;
  if(testtc != smi.rloop_prop_tc)
		{
			testtc = smi.rloop_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 Rloop.Prop_Tau = (double)testtc;
			}
			else
			{
				 smi.rloop_prop_tc=(unsigned int)Rloop.Prop_Tau;
			}
  }
  testtc = Rloop.Int_Tau;
  if(testtc != smi.rloop_int_tc)
		{
			testtc = smi.rloop_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 Rloop.Int_Tau = (double)testtc;
			}
			else
			{
				 smi.rloop_int_tc=(unsigned int)Rloop.Int_Tau;
			}
		}
  testtc = Rloop.Dint_Tau;
  if(testtc != smi.rloop_dint_tc)
		{
			testtc = smi.rloop_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 Rloop.Dint_Tau = (double)testtc;
			}
			else
			{
				 smi.rloop_dint_tc=(unsigned int)Rloop.Dint_Tau;
			}
	}
#endif
 /***** Double Loop A Override Check ***/
  testtc = DloopSA.Prop_Prim_Tau;
  if(testtc != smi.dloopsa_prop_tc)
		{
			testtc = smi.dloopsa_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 DloopSA.Prop_Prim_Tau = (double)testtc;
				 DloopSA.Prim_Prop_State=0.0; /*jam to zero to minimize transient*/
			}
			else
			{
				 smi.dloopsa_prop_tc=(unsigned int)DloopSA.Prop_Prim_Tau;
			}
  }
  testtc = DloopSA.Int_Prim_Tau;
  if(testtc != smi.dloopsa_int_tc)
		{
			testtc = smi.dloopsa_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 DloopSA.Int_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsa_int_tc=(unsigned int)DloopSA.Int_Prim_Tau;
			}
		}
  testtc = DloopSA.Dint_Prim_Tau;
  if(testtc != smi.dloopsa_dint_tc)
		{
			testtc = smi.dloopsa_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopSA.Dint_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsa_dint_tc=(unsigned int)DloopSA.Dint_Prim_Tau;
			}
	}
  testtc = DloopSA.Prop_Gps_Tau;
  if(testtc != smi.dloopsa_cal_tc)
		{
			testtc = smi.dloopsa_cal_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopSA.Prop_Gps_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsa_cal_tc=(unsigned int)DloopSA.Prop_Gps_Tau;
			}
	}

 /***** Double Loop B Override Check ***/
  testtc = DloopSB.Prop_Prim_Tau;
  if(testtc != smi.dloopsb_prop_tc)
		{
			testtc = smi.dloopsb_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 DloopSB.Prop_Prim_Tau = (double)testtc;
				 DloopSB.Prim_Prop_State=0.0; /*jam to zero to minimize transient*/
			}
			else
			{
				 smi.dloopsb_prop_tc=(unsigned int)DloopSB.Prop_Prim_Tau;
			}
  }
  testtc = DloopSB.Int_Prim_Tau;
  if(testtc != smi.dloopsb_int_tc)
		{
			testtc = smi.dloopsb_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 DloopSB.Int_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsb_int_tc=(unsigned int)DloopSB.Int_Prim_Tau;
			}
		}
  testtc = DloopSB.Dint_Prim_Tau;
  if(testtc != smi.dloopsb_dint_tc)
		{
			testtc = smi.dloopsb_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopSB.Dint_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsb_dint_tc=(unsigned int)DloopSB.Dint_Prim_Tau;
			}
	}
  testtc = DloopSB.Prop_Gps_Tau;
  if(testtc != smi.dloopsb_cal_tc)
		{
			testtc = smi.dloopsb_cal_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopSB.Prop_Gps_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopsb_cal_tc=(unsigned int)DloopSB.Prop_Gps_Tau;
			}
	}

 /***** Double Loop RA Override Check ***/
  testtc = DloopRA.Prop_Prim_Tau;
  if(testtc != smi.dloopra_prop_tc)
		{
			testtc = smi.dloopra_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 DloopRA.Prop_Prim_Tau = (double)testtc;
				 DloopRA.Prim_Prop_State=0.0; /*jam to zero to minimize transient*/
			}
			else
			{
				 smi.dloopra_prop_tc=(unsigned int)DloopRA.Prop_Prim_Tau;
			}
  }
  testtc = DloopRA.Int_Prim_Tau;
  if(testtc != smi.dloopra_int_tc)
		{
			testtc = smi.dloopra_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 DloopRA.Int_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopra_int_tc=(unsigned int)DloopRA.Int_Prim_Tau;
			}
		}
  testtc = DloopRA.Dint_Prim_Tau;
  if(testtc != smi.dloopra_dint_tc)
		{
			testtc = smi.dloopra_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopRA.Dint_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopra_dint_tc=(unsigned int)DloopRA.Dint_Prim_Tau;
			}
	}
  testtc = DloopRA.Prop_Gps_Tau;
  if(testtc != smi.dloopra_cal_tc)
		{
			testtc = smi.dloopra_cal_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopRA.Prop_Gps_Tau = (double)testtc;
			}
			else
			{
				 smi.dloopra_cal_tc=(unsigned int)DloopRA.Prop_Gps_Tau;
			}
	}

 /***** Double Loop B Override Check ***/
  testtc = DloopRB.Prop_Prim_Tau;
  if(testtc != smi.dlooprb_prop_tc)
		{
			testtc = smi.dlooprb_prop_tc;
			if(testtc > MIN_TC && testtc < MAX_PROP_TC)
			{
				 DloopRB.Prop_Prim_Tau = (double)testtc;
				 DloopRB.Prim_Prop_State=0.0; /*jam to zero to minimize transient*/
			}
			else
			{
				 smi.dlooprb_prop_tc=(unsigned int)DloopRB.Prop_Prim_Tau;
			}
  }
  testtc = DloopRB.Int_Prim_Tau;
  if(testtc != smi.dlooprb_int_tc)
		{
			testtc = smi.dlooprb_int_tc;
			if(testtc > MIN_TC && testtc < MAX_INT_TC)
			{
				 DloopRB.Int_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dlooprb_int_tc=(unsigned int)DloopRB.Int_Prim_Tau;
			}
		}
  testtc = DloopRB.Dint_Prim_Tau;
  if(testtc != smi.dlooprb_dint_tc)
		{
			testtc = smi.dlooprb_dint_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopRB.Dint_Prim_Tau = (double)testtc;
			}
			else
			{
				 smi.dlooprb_dint_tc=(unsigned int)DloopRB.Dint_Prim_Tau;
			}
	}
  testtc = DloopRB.Prop_Gps_Tau;
  if(testtc != smi.dlooprb_cal_tc)
		{
			testtc = smi.dlooprb_cal_tc;
			if(testtc > MIN_TC && testtc < MAX_DINT_TC)
			{
				 DloopRB.Prop_Gps_Tau = (double)testtc;
			}
			else
			{
				 smi.dlooprb_cal_tc=(unsigned int)DloopRB.Prop_Gps_Tau;
			}
	}


	/******* Threshold Override Cases*********/
	/* Remote Oscillator Cases*/
  ucurthres = TM.ROdelmax;
  if(ucurthres*10/26!= smi.ROdelmax)
		{
			propthres = smi.ROdelmax;
			if(propthres >9  && propthres < 2000)
			{
				 TM.ROdelmax = propthres*26/10;
			}
			else if(propthres > 0)
			{
				 smi.ROdelmax= ucurthres*10/26;
			}
			else
			{
				 TM.ROdelmax=RODELMAX;
				 smi.ROdelmax= RODELMAX*10/26;
			}
	}


  ucurthres = TM.ROdriftcheck;
  if(ucurthres*TAUZERO != smi.ROdriftcheck)
		{
			propthres = smi.ROdriftcheck;
			if(propthres >0  && propthres < 61)
			{
				 TM.ROdriftcheck = propthres/TAUZERO;
			}
			else if(propthres>0)
			{
				 smi.ROdriftcheck= ucurthres*TAUZERO;
			}
			else
			{
				 TM.ROdriftcheck=RODRIFTCHECK;
				 smi.ROdriftcheck= RODRIFTCHECK*TAUZERO;
			}
	}
  ucurthres = (unsigned int)(TM.ROdriftmax*1e9);
  if(ucurthres != smi.ROdriftmax)
		{
			propthres = smi.ROdriftmax;
			if(propthres >0  && propthres < 201)
			{
				 TM.ROdriftmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.ROdriftmax= ucurthres;
			}
			else
			{
				 TM.ROdriftmax = RODRIFTMAX;
				 smi.ROdriftmax= (unsigned int)(RODRIFTMAX*1.0e9);
			}
	}
  ucurthres = (unsigned int)(TM.ROfreqmax*1e9);
  if(ucurthres != smi.ROfreqmax)
		{
			propthres = smi.ROfreqmax;
			if(propthres >1999L  && propthres < 20000L)
			{
				 TM.ROfreqmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.ROfreqmax= ucurthres;
			}
			else
			{
				 TM.ROfreqmax = ROFREQMAX;
				 smi.ROfreqmax= (unsigned int)(ROFREQMAX*1.0e9);
			}
	}
  ucurthres = (unsigned int)(TM.ROphasemax*1e9*TAUZERO);
  if(ucurthres != smi.ROphasemax)
		{
			propthres = smi.ROphasemax;
			if(propthres >9L  && propthres < 2001L)
			{
				 TM.ROphasemax = (double)(propthres*1.0e-9/TAUZERO);
			}
			else if(propthres>0)
			{
				 smi.ROphasemax= ucurthres;
			}
			else
			{
				 TM.ROphasemax = ROPHASEMAX;
				 smi.ROphasemax= (unsigned int)(ROPHASEMAX*1.0e9*TAUZERO);
			}
	}
  ucurthres = TM.TPslewcheck;
  if(ucurthres*TAUZERO != smi.TPslewcheck)
		{
			propthres = smi.TPslewcheck;
			if(propthres >0  && propthres < 601)
			{
				 TM.TPslewcheck = propthres/TAUZERO;
			}
			else if(propthres>0)
			{
				 smi.TPslewcheck= ucurthres*TAUZERO;
			}
			else
			{
				 TM.TPslewcheck=TPSLEWCHECK;
				 smi.TPslewcheck= TPSLEWCHECK*TAUZERO;
			}
	}

 ucurthres = TM.TPdelmax;
  if(ucurthres != smi.TPdelmax)
		{
			propthres = smi.TPdelmax;
			if(propthres >0  && propthres < 11)
			{
				 TM.TPdelmax = propthres;
			}
			else if(propthres > 0)
			{
				 smi.TPdelmax= ucurthres;
			}
			else
			{
				 TM.TPdelmax=TPDELMAX;
				 smi.TPdelmax= TPDELMAX;
			}
	}
 ucurthres = TM.TPstepmax;
  if(ucurthres != smi.TPstepmax)
		{
			propthres = smi.TPstepmax;
			if(propthres >0  && propthres < 11)
			{
				 TM.TPstepmax = propthres;
			}
			else if(propthres > 0)
			{
				 smi.TPstepmax= ucurthres;
			}
			else
			{
				 TM.TPstepmax=TPSTEPMAX;
				 smi.TPstepmax= TPSTEPMAX;
			}
	}
 ucurthres = TM.TPslewmax;
  if(ucurthres != smi.TPslewmax)
		{
			propthres = smi.TPslewmax;
			if(propthres >0  && propthres < 11)
			{
				 TM.TPslewmax = propthres;
			}
			else if(propthres > 0)
			{
				 smi.TPslewmax= ucurthres;
			}
			else
			{
				 TM.TPslewmax=TPSLEWMAX;
				 smi.TPslewmax= TPSLEWMAX;
			}
	}

  ucurthres = TM.Sdelmax;
  if(ucurthres*50 != smi.Sdelmax)
		{
			propthres = smi.Sdelmax;
			if(propthres >99  && propthres < 2000)
			{
				 TM.Sdelmax = propthres/50;
			}
			else if(propthres > 0)
			{
				 smi.Sdelmax= ucurthres*50;
			}
			else
			{
				 TM.Sdelmax=SDELMAX;
				 smi.Sdelmax= SDELMAX*50;
			}
	}
  ucurthres = TM.Sfreqtau;
  if(ucurthres != smi.Sfreqtau)
		{
			propthres = smi.Sfreqtau;
			if(propthres >9  && propthres < 61)
			{
				 TM.Sfreqtau = propthres;
				 pmda->Sfilt1=(SDRIFTCHECK*TAUZERO)/(propthres);
				 pmda->Sfilt2=1.0-pmda->Sfilt1;
				 pmdb->Sfilt1=pmda->Sfilt1;
				 pmdb->Sfilt2=pmda->Sfilt2;
			}
			else if(propthres>0)
			{
				 smi.Sfreqtau= ucurthres;
			}
			else
			{
				 TM.Sfreqtau=SFREQTAU;
				 smi.Sfreqtau=SFREQTAU;
			}
	}
  ucurthres = (unsigned int)(TM.Sdriftmax*1e9);
  if(ucurthres != smi.Sdriftmax)
		{
			propthres = smi.Sdriftmax;
			if(propthres >0  && propthres < 201)
			{
				 TM.Sdriftmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.Sdriftmax= ucurthres;
			}
			else
			{
				 TM.Sdriftmax = SDRIFTMAX;
				 smi.Sdriftmax= (unsigned int)(SDRIFTMAX*1.0e9);
			}
	}
  ucurthres = (unsigned int)(TM.Sfreqmax*1e9);
  if(ucurthres != smi.Sfreqmax)
		{
			propthres = smi.Sfreqmax;
			if(propthres >5999L  && propthres < 20000L)
			{
				 TM.Sfreqmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.Sfreqmax= ucurthres;
			}
			else
			{
				 TM.Sfreqmax = g_freqMax;
				 smi.Sfreqmax= (unsigned int)(g_freqMax*1.0e9);
			}
	}
  ucurthres = (unsigned int)(TM.Sphasemax*1e9*TAUZERO);
  if(ucurthres != smi.Sphasemax)
		{
			propthres = smi.Sphasemax;
			if(propthres >9L  && propthres < 2001L)
			{
				 TM.Sphasemax = (double)(propthres*1.0e-9/TAUZERO);
			}
			else if(propthres>0)
			{
				 smi.Sphasemax= ucurthres;
			}
			else
			{
				 TM.Sphasemax = SPHASEMAX;
				 smi.Sphasemax= (unsigned int)(SPHASEMAX*1.0e9*TAUZERO);
			}
	}
  ucurthres = (unsigned int)(TM.Gfreqmax*1e9);
  if(ucurthres != smi.Gfreqmax)
		{
			propthres = smi.Gfreqmax;
			if(propthres >5999L  && propthres < 20000L)
			{
				 TM.Gfreqmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.Gfreqmax= ucurthres;
			}
			else
			{
				 TM.Gfreqmax = LTBI.gfreqmax;
				 smi.Gfreqmax= (unsigned int)(LTBI.gfreqmax*1.0e9);
			}
	}
  ucurthres = (unsigned int) (TM.Gdriftmax*1e9);
  if(ucurthres != smi.Gdriftmax)
		{
			propthres = smi.Gdriftmax;
			if(propthres >0  && propthres < 201)
			{
				 TM.Gdriftmax = (double)(propthres*1.0e-9);
			}
			else if(propthres>0)
			{
				 smi.Gdriftmax= ucurthres;
			}
			else
			{
				 TM.Gdriftmax = LTBI.gdriftmax;
				 smi.Gdriftmax= (unsigned int)(LTBI.gdriftmax*1.0e9);
			}
	}
}
/********************************************************************
This function provides the BT Shell to communicate commands to the
BT3 application through input mailboxes.
The command manager is called once per second.
**********************************************************************/ 


void Command_Mgr()
{
	unsigned short mask;
	/**** Check ensemble provisioning input mailbox*****/
	if(MSG_ECW_C.Ready)
	{
		if((MSG_ECW_C.ECW&0xFF)==DEFAULT_SET)
		{
		  smi.Input_Mode=ALL_INPUTS;
		  MSG_ECW_R.Override=OVERRIDE_CLEAR;
		}
		else if((MSG_ECW_C.ECW&0xFF)==ALL_INPUTS||(MSG_ECW_C.ECW&0xFF)==GPS_ONLY)
		{
		  smi.Input_Mode=MSG_ECW_C.ECW;
		  MSG_ECW_R.Override=OVERRIDE_ACCEPT;
		}
		else if((MSG_ECW_C.ECW&0xFF)==SPAN_ONLY)
		{
		  if(MSG_ECW_C.ECW&0x200&&MSG_ECW_C.ECW&0x400)  MSG_ECW_R.Override=OVERRIDE_REJECT;
		  else
		  {
			 smi.Input_Mode=MSG_ECW_C.ECW;
			 MSG_ECW_R.Override=OVERRIDE_ACCEPT;
		  }
		}
		else if((MSG_ECW_C.ECW&0xFF)==RO_ONLY)
		{
		  if(MSG_ECW_C.ECW&0x1000&&MSG_ECW_C.ECW&0x800) MSG_ECW_R.Override=OVERRIDE_REJECT;
		  else
		  {
			 smi.Input_Mode=MSG_ECW_C.ECW;
			 MSG_ECW_R.Override=OVERRIDE_ACCEPT;
		  }
		}
		MSG_ECW_R.ECW=smi.Input_Mode; 
		MSG_ECW_R.Ready=TRUE;
		MSG_ECW_C.Ready=FALSE;
	}   
	
	/**** Check reset control input mailbox*****/
	if(MSG_RCW_C.Ready)
	{
	   mask=MSG_RCW_C.RCW;
	   if((mask&0xFF)==DEFAULT_SET)
	   {
		  /*default reset condition*/
		  MSG_RCW_C.RCW=0x0001; /*DOCXO coldstart default*/
		  LTBI.lotype=DOCXO; /*assume DOCXO*/
	      Init_Tasks();
		  MSG_RCW_C.RCW=0x0001; /*OCXO coldstart default*/
		  MSG_RCW_R.Override=OVERRIDE_CLEAR;
		  BT3_reset();
	   }
	   else if((mask&0xFF)<4&&(mask&0xFF00)<0x300)
	   {
		  LTBI.lotype= mask >>8;  
		  if( (mask&0xFF)>1 )
		  {
		     init_cal(); 
		  } 
		  else
		  {
	      	Init_Tasks();
		  	MSG_RCW_C.RCW=mask; /*restore after global struct init*/
		  	BT3_reset();
		  }
		  MSG_RCW_R.Override=OVERRIDE_ACCEPT;
	   }
	   else 
	   {
		  MSG_RCW_R.Override=OVERRIDE_REJECT;
	   }
	   MSG_RCW_R.RCW=MSG_RCW_C.RCW;
	   MSG_RCW_R.Ready=TRUE;
	   MSG_RCW_C.Ready=FALSE;
	}	
	/**** Check  provisioning mask input mailbox*****/
	if(MSG_PMASK_C.Ready)
	{
		if((MSG_PMASK_C.PMASK)==DEFAULT_SET)
		{
			pmda->SPAN_Present= (pmda->SPAN_Present&0xFF00)|TRUE;
			pmdb->SPAN_Present= (pmdb->SPAN_Present&0xFF00)|TRUE;
			prma->RO_Present=   (prma->RO_Present&0xFF00)|TRUE;
			prmb->RO_Present=   (prmb->RO_Present&0xFF00)|TRUE;
			pmda->Span_Type=E1;
			pmdb->Span_Type=E1;  
			/***** NOTE ADD PROVISIONING SUPPORT FOR LOTYPE GPZ */
			MSG_PMASK_R.Override=OVERRIDE_CLEAR;
			MSG_PMASK_C.PMASK=0;
		}
		else
		{
			mask=~MSG_PMASK_C.PMASK;
			pmda->SPAN_Present=(pmda->SPAN_Present&0xFF00)|((mask&0x02)>>1);
			pmdb->SPAN_Present=(pmdb->SPAN_Present&0xFF00)|((mask&0x04)>>2);
			prma->RO_Present=  (prma->RO_Present&0xFF00)|((mask&0x08)>>3);
			prmb->RO_Present=  (prmb->RO_Present&0xFF00)|((mask&0x10)>>4); 
			/** check in span type*/
			pmda->Span_Type=(mask&0x20)>>5;
			pmdb->Span_Type=(mask&0x40)>>6;
			MSG_PMASK_R.Override=OVERRIDE_ACCEPT;
		}
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
		MSG_PMASK_R.PMASK=MSG_PMASK_C.PMASK;
		MSG_PMASK_R.Ready=TRUE;
		MSG_PMASK_C.Ready=FALSE;
	}
	/**** Check  qualification mask input mailbox*****/
	if(MSG_QMASK_C.Ready)
	{
		if((MSG_QMASK_C.QMASK)==DEFAULT_SET)
		{
			pmda->SPAN_Present= (pmda->SPAN_Present&0x00FF)|0x100;
			pmdb->SPAN_Present= (pmdb->SPAN_Present&0x00FF)|0x100;
			prma->RO_Present=   (prma->RO_Present&0x00FF)|0x100;
			prmb->RO_Present=   (prmb->RO_Present&0x00FF)|0x100;
			MSG_QMASK_R.Override=OVERRIDE_CLEAR;
			MSG_QMASK_C.QMASK=0;
		}
		else
		{
			mask=~MSG_QMASK_C.QMASK;
			pmda->SPAN_Present=(pmda->SPAN_Present&0x00FF)|((mask&0x02)<<7);
			pmdb->SPAN_Present=(pmdb->SPAN_Present&0x00FF)|((mask&0x04)<<6);
			prma->RO_Present=  (prma->RO_Present&0x00FF)|((mask&0x08)<<5);
			prmb->RO_Present=  (prmb->RO_Present&0x00FF)|((mask&0x10)<<4);
			MSG_QMASK_R.Override=OVERRIDE_ACCEPT;
		}
		MSG_QMASK_R.QMASK=MSG_QMASK_C.QMASK;
		MSG_QMASK_R.Ready=TRUE;
		MSG_QMASK_C.Ready=FALSE;
	}
}
/************************************************************************
Updates calibration NVM database with best estimates


**************************************************************************/
void Cal_Data_Update()
{
	/******* local input message data *****/
	struct MSG_FLL_CAL *pfllrec;
//	double Lloop_NCO_rec;
//	double	Lloop_Prop_Tau_rec;
//	double Lloop_Int_State_rec;
	/* receive 1 sec FLL input message One per minute*/
//	if(MSG_FLL_CAL_A.Ready) pfllrec= &(MSG_FLL_CAL_A);
//	else pfllrec= &(MSG_FLL_CAL_B);
//	Lloop_NCO_rec=pfllrec->Lloop_NCO;
//	Lloop_Prop_Tau_rec=pfllrec->Lloop_Prop_Tau;
//	Lloop_Int_State_rec=pfllrec->Lloop_Int_State;
//	pfllrec->Ready=FALSE;
	/******* complete message reception *****/
	/**** send factory calibration data to estimation routine****/
	MSG_CAL_EST_A.Factory_Freq_Cal=(double)Cal_Data.LO_Freq_Cal_Factory;
	MSG_CAL_EST_A.Ready=TRUE;
	/**** send factory calibration data to FLL routine****/
	MSG_CAL_FLL_A.Factory_Freq_Cal=(double)Cal_Data.LO_Freq_Cal_Factory;
	MSG_CAL_FLL_A.Ready=TRUE;
	/***********************************************************/    
	if((Get_Gear()>1) && phase_control<2)
	{           
	
		if(HMS.Nsum_Hour)
		{
				Cal_Data.LO_Freq_Cal=HMS.Osc_Freq_State;
		}
#if 0 // zmiao 3/24/2009: George says to take off Cal change during first hour of operation
	  // unless it's manufacture cal process. 
		else
		{
				Cal_Data.LO_Freq_Cal= Lloop.NCO_Cor_State;
		}
#endif
		if(HMS.Nsum_Day)
		{
				Cal_Data.LO_Drift_Cal=HMS.Long_Term_Drift;
		}

		if(HMS.Nsum_Hour) Cal_Data.update_cnt++;
		if(!Cal_Data.cal_state) /*no valid calibration data*/
		{
				// zmiao 3/24/2009: If no cal, update it.
				Cal_Data.LO_Freq_Cal = Lloop.NCO_Cor_State;

				Cal_Data.LO_Freq_Cal_Factory=Cal_Data.LO_Freq_Cal; 
				Cal_Data.cal_state=CAL_Update;
				  
		}
		if(Cal_Data.update_cnt>2&& Cal_Data.cal_state && Get_Gear()>1)
		{
				Cal_Data.update_cnt=0;
				Cal_Data.cal_state=CAL_Update;
		}
	}
 		
}
void	BT3_reset( void )
{
   Init_Shared_Mem_Int();
 	BT3_MSG_Init();
//   ShellInit();
//	Get_Flash_Cal_Data();
	init_cal();
}  
/******************************
Routine to return current
gear [0-7] that local single loop
is operating
******************************/
unsigned short Get_Gear()
{
return (Lloop.Cal_Mode);
}        

/************************************************************************
Reset calibration NVM database with manual estimate of center frequency


**************************************************************************/
void Cal_Data_Reset()
{               
 	Cal_Data.LO_Freq_Cal=0.0;
    Cal_Data.LO_Drift_Cal=0.0;  
    Cal_Data.LO_Freq_Cal_Factory=0.0;  
    Cal_Data.cal_state=CAL_None;
    init_cal(); 
}   

void Cal_Data_Subtend_Reset(void)    
{               
 	Cal_Data.LO_Freq_Cal=ken_MPLL_freq;
    Cal_Data.LO_Drift_Cal=0.0;  
    Cal_Data.LO_Freq_Cal_Factory=ken_MPLL_freq;  
    Cal_Data.cal_state=CAL_None;
    init_cal(); 
} 
