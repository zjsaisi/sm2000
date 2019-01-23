/***************************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/14 00:01:20PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_fll.c_v  $
$Modtime:   25 Feb 1999 11:31:10  $
$Revision: 1.3 $


                            CLIPPER SOFTWARE
                                 
                                BT3_FLL.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: This module contains the core FLL related tasks to 
                  support the BT3 clock engine.

MODIFICATION HISTORY:

$Log: bt3_fll.c  $
Revision 1.3 2010/10/14 00:01:20PDT Jining Yang (jyang) 
Merged from 1.2.1.2.
Revision 1.2.1.2 2010/08/20 09:08:25PDT Jining Yang (jyang) 
Commented a couple of unnecessary debug print.
Revision 1.2 2008/04/01 07:47:40PDT George Zampetti (gzampetti) 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.1 2007/12/06 11:41:06PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.18 2006/07/07 15:05:36PDT gzampet 
add synthesizer control
Revision 1.17 2006/05/10 10:19:53PDT gzampet 
modifications to improve standby to active performance
Revision 1.14 2006/05/04 14:52:08PDT gzampet 
is_efc_ok call added
Revision 1.13 2006/05/04 14:04:38PDT gzampet 
supress phase slew in lower gears
Revision 1.10 2006/05/03 15:47:46PDT gzampet 
fixed efc meaurement
Revision 1.8 2006/04/19 17:12:49PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.7 2006/03/10 08:04:30PST gzampet 

Revision 1.2 2006/03/01 08:46:22PST gzampet 

Revision 1.1 2005/09/01 14:55:10PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.36 2005/02/03 11:23:19PST gzampet 

Revision 1.35  2005/01/12 23:36:44Z  gzampet
Revision 1.34  2005/01/10 23:44:02Z  gzampet
Revision 1.33  2005/01/10 21:08:40Z  gzampet
Revision 1.32  2005/01/10 19:06:17Z  gzampet
Revision 1.31  2005/01/10 16:28:06Z  gzampet
Revision 1.30  2005/01/05 22:34:07Z  gzampet
Revision 1.29  2004/12/20 17:53:56Z  gzampet
Revision 1.28  2004/12/16 16:20:37Z  gzampet
Revision 1.27  2004/12/10 15:39:56Z  gzampet
Revision 1.26  2004/12/09 21:56:51Z  gzampet
Revision 1.25  2004/10/14 21:47:46Z  gzampet
Revision 1.24  2004/09/20 21:03:49Z  gzampet
Revision 1.23  2004/09/17 17:12:15Z  gzampet
Revision 1.22  2004/09/03 18:21:54Z  zmiao
Revision 1.21  2004/09/03 16:51:17Z  gzampet
Revision 1.20  2004/08/25 23:04:44Z  gzampet
Revision 1.19  2004/08/24 21:46:20Z  gzampet
Revision 1.18  2004/07/30 23:59:16Z  gzampet
Revision 1.17  2004/07/30 14:26:08Z  gzampet
Revision 1.16  2004/07/29 23:07:55Z  gzampet
Revision 1.15  2004/07/29 18:22:24Z  gzampet
Revision 1.14  2004/07/29 18:12:00Z  gzampet
Revision 1.13  2004/07/09 18:28:39Z  gzampet
Added engine reporting
Revision 1.12  2004/07/01 15:43:35Z  gzampet
Revision 1.11  2004/07/01 13:11:57Z  gzampet
Revision 1.10  2004/06/24 16:49:33Z  gzampet
inhibit direct NCO update
Revision 1.9  2004/06/23 19:16:49Z  gzampet
fully merged bt3_fll.c
Revision 1.8  2004/06/23 18:39:02Z  gzampet
Merge with TS3X release (Upto Holdover Function)
Revision 1.7  2004/06/23 17:39:31Z  gzampet
 Partial Merged with Released TimeSource 3X
Revision 1.6  2004/06/23 15:14:52Z  gzampet
Baseline GPS BesTime Version
Revision 1.5  2003/08/22 19:43:22Z  gzampet
missing return statement in get smart clk fixed
Revision 1.4  2003/04/25 18:03:24Z  zmiao
Revision 1.3  2003/04/24 14:59:16Z  gzampet
Revision 1.2  2003/04/19 00:10:59Z  gzampet
Revision 1.7  2002/09/13 21:16:17Z  gzampet
Fixed issues related to holdover and smart clock operation
1) Fixed prediction error calculation to start exactly 8 hours back (not 12)
2) Fixed bug in predicition variance weight reset to allow it the average 
to start over if there is a transient
3) Added frequency modulation to the predicted daily frequency to
prevent the predictors from dropping to zero (fm is canceled so that
it does not impact the holdover base and drift)
4) Added logic to prevent a change in the smart clock state while in holdover
Revision 1.6  2002/08/16 18:44:05Z  gzampet
Added get_smartfreq() function to calculate microprocessor based ICW for CSM
When updating the Lloop (RTHC only loop) modify condition to reset hourly holdover averages to always perform the reset if a frequency step is detected
Significant changes to the holdover update function to operate with
 an 8 hour Prediction window, restart if an input step is detected, use 
an exponential filter to calculated prediction error and change the 
smart clock state calculation.
Revision 1.5  2002/08/09 22:39:49Z  gzampet
Added test variables for holdover debug printout (see nema_user_int)
Added GetSmartClk routine to report state of smart clock algorithm
Added in BT3_FLL.c a test to restart the "one hour" holdover measurement sum i
if a transient is detected
Added a test for a frequency step in holdover_update to force restart
Reduce working holdover channels i loop from 3 to one LLOOP
Reduced early aging from 10 days to 2 days and made linear
Holdover base average reduced from 4 to one hour
Modified 24 hour prediction code to produce a stable 8 hour prediction
Revision 1.4  2002/06/07 20:50:28Z  kho
1. Added code that will bound the EFC range between 0 and 0xFFFFF
2.Added code that prevents Recalibrate() functions when OCXO is still cold.
3 Added code that prevents FLL calculations when OCXO is still cold.
Revision 1.3  2002/06/03 22:11:21Z  kho
1. Changed qualifier for bad GPS measurement from a AND to a OR.
2. Jam from gear 2 to gear 3 for GPS loop.
Revision 1.2  2002/05/31 17:59:42Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
 * 
 *    Rev 1.1   25 Feb 1999 14:02:12   S.Scott
 * TS3000 initial release. Check the status of the recal flag before 
 * ensembling an input. If it has never been recal'd, force it.
 * 
 *    Rev 1.0   06 Feb 1999 15:41:42   S.Scott
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


/******************************************************************************
*         MODULE BT3_FLL.C 9-28-97 Created as part of the BT3 Core Clock Engine
*         Project.
*         This module contains the core FLL related tasks to support the BT3
*         clock engine.
*     Version 1.0 Baseline BT3 clock engine
*                      1) Moved Core Frequency Locked Loop (FLL) Related Tasks
*                         to this new source module to improve manageability.
*
*     Version 2.0
*                     1) Add message receive from span estimation freq_est_250
*                        and remove extern struct Span_Meas_Data
*                     2) Add message receive from RO estimation ro_freq_est_250
*                        and remove extern struct Remote_Oscillator_Meas_Data
*
*                     3) Add Msg_Gen_1sec task to generated output messages
*                        to 1 minute task level
*                     4) Add Ensembling Control Data Structure to provide
*                        a private area for NCO_Update
*                     5) Updated holdover estimation to calculate rms
*                        24 hour prediction error and use 7 day drift estimates.
*      Version 2.1
*                     1) Added use of combined mvar estimate (included TCH
*                        decomposition and 24 holdover prediction stability)
*                        during holdover (see NCO_Update). The combined mvar
*                        estimate slowly converts to the 24 prediction as holdover
*                        persists. April 27 1998. Added a new message to transfer
*                        24 hour stability data from the 1 minute holdover task to
*                        the 1 sec NCU_Update task.
*      Version 2.1.b
*                     1) Modify Holdover to use 24 hour average for frequency
*                        calibration instead of last four hours. Also eliminate
*                        use of drift term in span holdover prediction.
*      Version 3.0
*                    1) Readjusted the Recalibration Function (10-29-98) to remove
*                       seams in state transistion. Needed to be more careful in
*                       corrected proportional filter states "backwards in time"
*                       by the correct delay when shifting to a slower tc. The
*                       fix shows improvement under heavy freq drift conditions
*                       as would occur for a cold OCXO.
*****************************************************************************/
#include "bt3_def.h"   
#include "smartclkdef.h"
#define	F2L(x)  (long)((x*1e12)+0.5)
#define	DECMT(x) (x==0)?23:x-1
#define	DECMTD(x) (x==0)?29:x-1
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#include "math.h"  /*Used only for sqrt function rms prediction error*/
#endif 
#include <stdio.h> 
/******** global data structures *********/                     
//#include "proj_inc.h"
//#include "types.h"
#include "bt3_dat.h"   /*main file for global data declarations*/
#include "bt3_msg.h"   /*main file for message buffer templates*/
#include "bt3_shl.h"   /*main file for message shell templates*/
#include "AllProcess.h"
#include "machine.h"
#include "BT3Support.h"

//#include "hardware.h"  
//#include "card_state.h"
/****** macro to convert from 24 predicted time error to freq stability ****/
#define NCOVAR(MVAR,TDEV,INIT)                                               \
{                                                                            \
	if(HMS.Nsum_Day>0)  MVAR =TDEV/SECINDAY;                                  \
	else   MVAR =INIT/SECINDAY;	                                            \
	MVAR *= MVAR;																				  \
}
#if !PCSIM
double sqrt(double);
#endif
/**** enumerate states for ensemble state table***/                               
enum 
{ 
 EPH = 0, // ensemble priority high
 EPM = 1, // ensemble priority medium
 EPL = 2,  // ensemble priority low
 EQ1 = 3, // ensemble quality 1
 EQ2 = 4, // ensemble quality 2
 EQA = 5 // ensemble quality Alarm
};

/********* messaging related data ***********************/
unsigned char msg_count1sec;
extern struct MSG_Span_FLL MSG_Span_FLL_A, MSG_Span_FLL_B;
extern struct MSG_RO_FLL MSG_RO_FLL_A, MSG_RO_FLL_B;
struct MSG_FLL_EST MSG_FLL_EST_A,*pfllestmsg;
struct MSG_NCOUP_EST MSG_NCOUP_EST_A,*pncoupestmsg;
struct MSG_FLL_HOLDOVER MSG_FLL_HOLDOVER_A, MSG_FLL_HOLDOVER_B,*pholdmsg;
struct MSG_HOLDOVER_FLL MSG_HOLDOVER_FLL_A;
struct MSG_HOLDOVER_NCOUP MSG_HOLDOVER_NCOUP_A;
struct MSG_FLL_TCH MSG_FLL_TCH_A, MSG_FLL_TCH_B,*ptchmsg;
struct MSG_FLL_CAL MSG_FLL_CAL_A, MSG_FLL_CAL_B,*pcalmsg;
struct MSG_SR_TCH MSG_SR_TCH_A, MSG_SR_TCH_B, *psrtchmsg;
extern  struct MSG_TCH_NCOUP MSG_TCH_NCOUP_A;

struct MSG_Control_Loop_Section MSG_CLS[CLOOPS];
struct MSG_Control_Loop_Common MSG_CLC;
struct MSG_Daily_Holdover_Report MSG_DHR;
struct MSG_Monthly_Holdover_Report MSG_MHR;   
struct MSG_BesTime_Engine_Report   MSG_BER;
/************ end messaging related data***************/
extern struct GPS_Measurement_Data GM;
extern struct Shared_Memory_Interface smi;
extern struct ASM_Management ASMM;
extern struct Status_Registers SReg;  
extern struct Phase_Compensation Pcomp;
extern struct LO_TYPE_BUILD_INFO LTBI;
extern double weight_correct(int);
// extern void DebugPrint(const char *str);

extern unsigned short trace_source; //controls traceability source  
extern unsigned short GPS_Valid; 
extern unsigned short cts_reg;
extern unsigned short ROB_ASSIGN;  
extern double ken_MPLL_freq ;
unsigned char NCO_Slew; /*counter inhibits nco slew control at startup*/
double NCO_Smooth; /*smoothed nco correction to prevent carrier loss in GPS
							 Costas Loop*/
unsigned short CLS_Count; /*counter controls control loop mailbox update*/
unsigned short DHR_Count; /*counter controls daily holdover report mailbox update*/
double factory_freq_FLL_rec;  /*One second copy of factory frequency calibration*/
double Tracking_Success_rec;  /*One second timing task copy of tracking success     */
unsigned short ensemble_flag=1; //activates or deactivated BesTime ensembling*/     
unsigned short dither_flag=1; //activates or deactivated BesTime ensembling*/     
unsigned short Dither=0;
double NCO_Cor,NCO_Frac;
unsigned short fll_print;

/******* 1 second thread frequency input data *****/
double YspavgA_rec,YspavgB_rec,YroavgA_rec,YroavgB_rec;  /*_rec indicated received data*/
struct Dual_Input_FLL_Control_System DloopSA,DloopSB,DloopRA,DloopRB,*ploop;
struct Single_Input_FLL_Control_System Lloop,*sloop;  
extern struct Single_Input_FLL_Control_System SSUloop;
struct Input_Freq_One_Minute_Data IFOM;
struct Ensembling_Control_Data  ECD;

#if REMOTE
struct  Single_Input_FLL_Control_System Rloop;
#endif
struct Holdover_Management HMSA,HMSB,HMRA,HMRB,HMS,HMR,*phm;
struct Holdover_LSF  HLSF;
struct Tracking_Stats TrkStats;
#if PRINTLOG
extern unsigned long ptime; /* print log time stamp in cumulative minutes*/
#endif
extern unsigned short  pcount; /*manage rate of print logging*/
double Yro_tc;
double smoothl;
double smooths;   
double gpz_test_freq;
unsigned short gpz_test_cnt,standby_cnt=0;
unsigned short gpz_test_seq[5];    
int smart_prr_state=SMART_OFF;  
/* local function templates*/
void BT3FLL_Update(void);
void Holdover_Update(void);
void Holdover_Test(void);
void Recalibrate(void);
void NCO_Update(void);
void NCO_Test(void);
void NCO_Convert(double);
void NCO_Control(void);
void Msg_Gen_1sec(void);
void Control_Loop_Shell(void);
void Holdover_Report(void);
void PRR_Holdover_Print(void);
void PRR_Daily_Holdover_Print(void);  
void PRR_Monthly_Holdover_Print(void);
void Holdover_Inverse_Drift_Fit(double);   
void PRS_Loop_Print(void);
void SPA_Loop_Print(void);
void bestime_engine_ensemble_control( unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
int get_prr_smartclk(void);               
void set_ensemble_mode(unsigned int );
void set_efc_mode(unsigned int );     
void get_var_control(unsigned short *,unsigned short *,unsigned short *);
/********************************************************************
  This module implements the update of each of the Frequency Locked Loop
  (FLL)s supported in the BT3 core engine. The loops are structured based
  on the Multiple Frequency Locked Loop Patented approach to optimizing
  frequency lock to multiple input. signals.
*******************************************************************/
void BT3FLL_Update()
{
	int i,j;
	double Delta_State;
	double Drift; 
	double Yprim_avg, *pX;
	unsigned short *pXcnt;
	double Cal_Pred; /*calibration prediction to use when GPS is not available*/
	unsigned short Status;
	unsigned short SPA_Reg_Events_rec;
	unsigned short SPB_Reg_Events_rec;
	unsigned short ROA_Reg_Events_rec;
	unsigned short ROB_Reg_Events_rec;
	struct MSG_Span_FLL *psprec;
	struct MSG_RO_FLL *prorec;
	/* receive 250 ms input messages*/
	if(MSG_Span_FLL_A.Ready) psprec= &(MSG_Span_FLL_A);
	else psprec= &(MSG_Span_FLL_B);
	YspavgA_rec=psprec->YspavgA;
	YspavgB_rec=psprec->YspavgB;
	SPA_Reg_Events_rec=psprec->SPA_Reg_Events;
	SPB_Reg_Events_rec=psprec->SPB_Reg_Events;
	psprec->Ready=FALSE;
	if(MSG_RO_FLL_A.Ready) prorec= &(MSG_RO_FLL_A);
	else prorec= &(MSG_RO_FLL_B);
	YroavgA_rec=prorec->YroavgA;
	YroavgB_rec=prorec->YroavgB;
	ROA_Reg_Events_rec=prorec->ROA_Reg_Events;
	ROB_Reg_Events_rec=prorec->ROB_Reg_Events;   


	prorec->Ready=FALSE;
	/******* complete receive of local 250ms input data *****/
	/******* Receive 1 minute holdover data ****************/
	if(MSG_HOLDOVER_FLL_A.Ready)
	{
		Lloop.LO_Freq_Pred=MSG_HOLDOVER_FLL_A.Lloop_Osc_Freq_Pred;
		#if REMOTE
		Rloop.LO_Freq_Pred=MSG_HOLDOVER_FLL_A.Rloop_Osc_Freq_Pred;
		#endif
		DloopSA.Prim_Freq_Pred=MSG_HOLDOVER_FLL_A.SPA_Cal_Freq_Pred;
		DloopSB.Prim_Freq_Pred=MSG_HOLDOVER_FLL_A.SPB_Cal_Freq_Pred;   
		DloopRA.Prim_Freq_Pred=MSG_HOLDOVER_FLL_A.ROA_Cal_Freq_Pred;
		DloopRB.Prim_Freq_Pred=MSG_HOLDOVER_FLL_A.ROB_Cal_Freq_Pred;
		
		Tracking_Success_rec=MSG_HOLDOVER_FLL_A.Tracking_Success;

		MSG_HOLDOVER_FLL_A.Ready=FALSE;
	}
	/*complete receive of 1 minute holdover data */       
	PRS_Loop_Print();
	for(i=1;i<CLOOPS;i++) /*for both SPAN Channels and both remote (external) osc. */
	{
		 if(i==SPA_LOOP)
		 {
			Status   = SReg.SPA_Reg&0x18C|SPA_Reg_Events_rec ;
			Yprim_avg= YspavgA_rec;
			ploop = &DloopSA;
			Cal_Pred= DloopSA.Prim_Freq_Pred;
			pX=&(IFOM.XSPA);
			pXcnt=&(IFOM.XSPA_cnt);
		 }
		 else if(i==SPB_LOOP)
		 {
			Status    =  SReg.SPB_Reg&0x18C|SPB_Reg_Events_rec ;
			Yprim_avg= YspavgB_rec;
			ploop= &DloopSB;
			Cal_Pred= DloopSB.Prim_Freq_Pred;
			pX=&(IFOM.XSPB);
			pXcnt=&(IFOM.XSPB_cnt);
		 } 
		 else if(i==ROA_LOOP)
		 {
			Status=   SReg.ROA_Reg&0x18C|ROA_Reg_Events_rec ;
			Yprim_avg= YroavgA_rec;
			ploop= &DloopRA;
			Cal_Pred= DloopRA.Prim_Freq_Pred;
			pX=&(IFOM.XROA);
			pXcnt=&(IFOM.XROA_cnt);
		 }
		 else if(i==ROB_LOOP)
		 {
			Status= SReg.ROB_Reg&0x18C|ROB_Reg_Events_rec;
			Yprim_avg=  YroavgB_rec;
			ploop= &DloopRB;
			Cal_Pred= DloopRB.Prim_Freq_Pred;
			pX=&(IFOM.XROB);
			pXcnt=&(IFOM.XROB_cnt);
		 }
		 if(!(Status&0x173)&&(ploop->Prim_Skip==0)) /*no Primary events and skipping is off*/
		 {
		  /*delay input to manage transients*/
		  for(j=1;j<PRIMDELAY;j++)
		  {
		  	ploop->Yprim_Buf[j-1]= ploop->Yprim_Buf[j];
		  	ploop->Yfback_Buf[j-1]= ploop->Yfback_Buf[j];
		  }
		  ploop->Yprim_Buf[PRIMDELAY-1]= Yprim_avg;
		  ploop->Yfback_Buf[PRIMDELAY-1]= ploop->Prim_Cor_State;
			/****SPAN Branch Updates *****/
		  ploop->Prim_Prop_State += ( ploop->Yprim_Buf[0] -
					ploop->Yfback_Buf[0])/ploop->Prop_Prim_Tau;
		  ploop->Prim_Int_State += (ploop->Prim_Prop_State)/ploop->Int_Prim_Tau;
		  /*ploop->Prim_Dint_State += (ploop->Prim_Prop_State+ ploop->Prim_Int_State)
											*TAUZERO/ploop->Dint_Prim_Tau;*/
		 } /* end if input valid*/
		 else
		 {
/*      NEED TO LET TRANSIENT SETTLE*/
		  if(ploop->Prim_Skip>0) ploop->Prim_Skip--;
		  else ploop->Prim_Skip=PRIMDELAY*4; /*blank input to let transient settle*/
		  ploop->Prim_Prop_State = 0.0;
		  for(j=0;j<PRIMDELAY;j++)
			  ploop->Yprim_Buf[j]= ploop->Prim_Cor_State; /*Buffer of last n primary loop inputs*/
		 }
		 (*pX)+=ploop->Yprim_Buf[0];
		 (*pXcnt)++;
		 /****** if drift alarm is present when Prim_Skip =1 then assume permanent
		  step in PRIM-LO frequency and perform necessary jam and recal
		  Note Status&0x1F5 checks for all valid,present and qualified but drift
		  The test for Start_Recal is there to handle start up when there has never
		  been a span calibration Start_Recal is initialized TRUE to force
		  a span recalibration as soon as non-drift alarms clear. */
/*		 if((ploop->Prim_Skip==1)&&!(Status&0x1F5)&&(Status&0x0A)) old recal test */
		 if((ploop->Prim_Skip==1)&&!(Status&0x1F5)
		 &&(Status&0x0A)||(ploop->Prim_Skip==1&&ploop->Start_Recal)) /*new recal test */
		 {   
		  Delta_State=Yprim_avg - ploop->Prim_Cor_State;
		  ploop->Prim_Cor_State += Delta_State;
		  ploop->Prim_Int_State += Delta_State;
			  for(j=0;j<PRIMDELAY;j++)
			  {
				  ploop->Yprim_Buf[j]= Yprim_avg; /*Buffer of last n primary loop inputs*/
				  ploop->Yfback_Buf[j]= ploop->Prim_Cor_State;
		  	  }
			  if(!(SReg.GPS_Reg&0x0C)) /*no GPS alarms */
			  {
				 ploop->GPS_Prop_State -=Delta_State;
				 ploop->Ygps_scal1-=Delta_State;
			 	 ploop->Ygps_scal2-=Delta_State;
			 	for(j=0;j<GPSDELAY;j++) ploop->Ygps_Buf[j]= ploop->GPS_Prop_State;
			  }
			  if((smi.Input_Mode&0xFF)==SPAN_ONLY||(smi.Input_Mode&0xFF)==RO_ONLY||(ploop->Start_Recal))
			  {
				ploop->GPS_Prop_State = 0.0;
			 	ploop->Ygps_scal1 = 0.0;
			 	ploop->Ygps_scal2 = 0.0;
			 	for(j=0;j<GPSDELAY;j++) ploop->Ygps_Buf[j]= 0.0;
		  	 }
		  ploop->Recalibrate =TRUE; ploop->Start_Recal=TRUE;
		 }
		 /**** GPS Branch Update*****/
		 if(!(SReg.GPS_Reg&0x33)) /*no GPS events*/
		 {
		  for(j=1;j<GPSDELAY;j++) ploop->Ygps_Buf[j-1]= ploop->Ygps_Buf[j];  
//		GPZ use open loop corrected data from GPS measurement to steer loop so we don't need to compensate here		  
//		  if(Get_Clock_Mode()==2)
//		  {
//			  ploop->Ygps_Buf[GPSDELAY-1]= GM.Ygps+ECD.freq_state_cor+ smi.phase_state_cor
//									   -ploop->Prim_Cor_State;  
//		  }	
//		  else
//		  {
//			  ploop->Ygps_Buf[GPSDELAY-1]= GM.Ygps+ken_MPLL_freq
//									   -ploop->Prim_Cor_State;  
//		  }									   
			  ploop->Ygps_Buf[GPSDELAY-1]= GM.Ygps-ploop->Prim_Cor_State;  
		 }
		 else  /*use calibration prediction as best estimate during GPS outage*/
		 {
		  for(j=0;j<GPSDELAY;j++) ploop->Ygps_Buf[j]= Cal_Pred;
		 }
		 ploop->Ygps_scal1= ploop->Gcalf1*(ploop->Ygps_Buf[0]) + ploop->Gcalf2*ploop->Ygps_scal1;
		 ploop->Ygps_scal2= ploop->Gcalf1*(ploop->Ygps_scal1) + ploop->Gcalf2*ploop->Ygps_scal2;
		 ploop->GPS_Prop_State = ploop->Ygps_scal2;
		 if((smi.Input_Mode&0xFF)==SPAN_ONLY||(smi.Input_Mode&0xFF)==RO_ONLY)
		 {
			 ploop->GPS_Prop_State = 0.0;
		 }
		 /*****NCO Update *********/
		 ploop->Prim_Cor_State=   ploop->Prim_Prop_State  +
					ploop->Prim_Int_State   +
					ploop->Prim_Dint_State;
		 ploop->NCO_Cor_State = ploop->Prim_Cor_State  +
					ploop->GPS_Prop_State;
	} /* end for both SPAN Channels and both RO channels */
	/******* generate 1 second message from FLL task to Estimation ********/
	pfllestmsg=&(MSG_FLL_EST_A);
	pfllestmsg->YprimoutSPA=DloopSA.Prim_Cor_State;
	pfllestmsg->YprimoutSPB=DloopSB.Prim_Cor_State;   
	pfllestmsg->YprimoutROA=DloopRA.Prim_Cor_State;
	pfllestmsg->YprimoutROB=DloopRB.Prim_Cor_State;
	pfllestmsg->Ready=TRUE;
	/*************end message generation *********************/
	/**** Update GPS Only Single Loop *****/
	/*****Update open loop freq error estimates****/  
   if(Lloop.Trace_Skip>0)Lloop.Trace_Skip--;  //GPZ June 2010 Always decrement trace skip
	
//	if((SReg.GPS_Reg&0x3F)||(!GPS_Valid)) /*bad local gps vs LO input measurement use forcast*/ {
/* Don't start the loop if GPS is bad, or if OCXO is not stable (kjh) */
//	if(((SReg.GPS_Reg&0x3F)||(!GPS_Valid)||!Is_OCXO_warm())||Lloop.Trace_Skip)
	if((SReg.GPS_Reg&0x3F)||(!GPS_Valid)||(Lloop.Trace_Skip))    //GPZ June 2010 do not depend on oscillator warm
	{                
//        if(Lloop.Trace_Skip>0)Lloop.Trace_Skip--;   GPZ June 2010 move above
		/****** If GPS not available and EFC is okay use SSU predication running from actual EFC ****/             
		 //GPZ June 2010 Not sure why all this initialization is here I've comment certain pieces out		
		if(Is_EFC_OK())
		{               
//   			DebugPrint("In_EFC_Okay\r\n", 7);

		    if(Get_Gear()==5 && standby_cnt <600)
		    {  
 	  		 DebugPrint("stdby_count<600\r\n", 7);

			 Lloop.Prop_State=SSUloop.LO_Freq_Pred;
			 Lloop.Int_State= 0.0;
			 Lloop.Dint_State=0.0;
			 Lloop.NCO_Cor_State= Lloop.Prop_State + Lloop.Int_State;
			 Lloop.Sprop1= SSUloop.LO_Freq_Pred;
			 Lloop.Sprop2= SSUloop.LO_Freq_Pred;
		    }
		    if(standby_cnt<3600) standby_cnt++;
	 		Lloop.Freq_Err_Est      = SSUloop.LO_Freq_Pred; /*use holdover estimate*/ 
	 		MSG_HOLDOVER_FLL_A.Lloop_Osc_Freq_Pred= SSUloop.LO_Freq_Pred;
            HMS.Osc_Freq_State=SSUloop.LO_Freq_Pred;
			Lloop.Ygps_Buf[GPSDELAY-1]=SSUloop.LO_Freq_Pred;
			Lloop.Ygps_Buf[GPSDELAY-2]=SSUloop.LO_Freq_Pred;
			Lloop.Ygps_Buf[GPSDELAY-3]=SSUloop.LO_Freq_Pred;
			Lloop.Ygps_Buf[GPSDELAY-4]=SSUloop.LO_Freq_Pred;
              
		}  
		else
		{
//  			DebugPrint("In_EFC__NOT_Okay\r\n", 7);
                 
		    standby_cnt=0;
		   	Lloop.Freq_Err_Est  =    Lloop.LO_Freq_Pred;  
		   	Lloop.Ygps_Buf[GPSDELAY-1]=Lloop.LO_Freq_Pred;
		   	Lloop.Ygps_Buf[GPSDELAY-2]=Lloop.LO_Freq_Pred;
		   	Lloop.Ygps_Buf[GPSDELAY-3]=Lloop.LO_Freq_Pred;
		   	Lloop.Ygps_Buf[GPSDELAY-4]=Lloop.LO_Freq_Pred;

		} 
		if((trace_source ==3)&&(Lloop.Trace_Skip>0)&&(Lloop.Trace_Skip<<40)&&!(SReg.GPS_Reg&0x3F)) //GPZ June 2010 force on to potential new freq
		{		
		   	Lloop.Freq_Err_Est  =      GM.Ygps;  
		   	Lloop.Ygps_Buf[GPSDELAY-1]=GM.Ygps;
		   	Lloop.Ygps_Buf[GPSDELAY-2]=GM.Ygps;
		   	Lloop.Ygps_Buf[GPSDELAY-3]=GM.Ygps;
		   	Lloop.Ygps_Buf[GPSDELAY-4]=GM.Ygps;
//		   	if(((GM.Ygps-Lloop.Sprop1)>0.5e-9) ||((GM.Ygps-Lloop.Sprop1)< -0.5e-9)) 
//		   	{ 
				Lloop.Sprop1=GM.Ygps;		   	
				Lloop.Sprop2=GM.Ygps;	
				Lloop.Sprop_prev=GM.Ygps;
//			}					   	
		}
		// Force restart of holdover freq data if step is expected       
		if(1) //force to clean restart all the time
		//if(SReg.GPS_Reg&0x8)
		{ 
            Lloop.Freq_Sum_Cnt=0;
			HMS.FreqSum=0;
			HMS.FreqSumCnt=0;
        }    
	}
	else /*good local gps vs LO input measurement */
	{   
	/*TODO use mpllfreq as better alternative to open loop*/ 
		/* Lloop.Freq_Err_Est= 2.0e-9; jam for loop test*/ 
//		if(Get_Clock_Mode()==2)
//		{ 
//			Lloop.Freq_Err_Est= GM.Ygps+ECD.freq_state_cor+
//							    	smi.phase_state_cor;  
//		}
//		else
//		{					    	
//			Lloop.Freq_Err_Est= GM.Ygps+ ken_MPLL_freq;
//		}  
		Lloop.Freq_Err_Est= GM.Ygps;
//	    if(Pcomp.TFOM<750.0e-9) /*GPZ add 7-7-00 to force poor tracking success
//									 if time figure of merit is bad was 500ns in TimeSource*/
//		{
//	 		#if(ANTENNA_MODE==ROOFTOP)
//	 		Lloop.Freq_Err_Sum+=Lloop.Prop_State;/*used smoothed data to supress SA*/
//	 		#else
//	 		Lloop.Freq_Err_Sum+= Lloop.Freq_Err_Est;/*used current data*/
//	 		#endif  
//			GPZ crossfeed SSU efc data to always populate holdover data in TimeCreator
	 		Lloop.Freq_Err_Sum+= SSUloop.Freq_Err_Est;
	 		Lloop.Freq_Sum_Cnt++;
//	 	}    In release 2.1 PPS control is not operative so TFOM check is skipped

	}
	for(j=1;j<GPSDELAY;j++) Lloop.Ygps_Buf[j-1]= Lloop.Ygps_Buf[j];
	Lloop.Ygps_Buf[GPSDELAY-1]= Lloop.Freq_Err_Est;
	IFOM.XGPS+=Lloop.Ygps_Buf[0];
	IFOM.XGPS_cnt++;
	#if REMOTE
	if(SReg.ROA_Reg||SReg.GPS_Reg) /*bad remote vs local data or GPS use forcast*/
	{
		Rloop.Freq_Err_Est= Rloop_Osc_Freq_Pred_rec; /*use holdover estimate*/
	}
	else /*good local vs remote measurement and good GPS*/
	{
		Rloop.Freq_Err_Est=(-prma->Yroavg - smi.RO_temp_state_cor+ smi.temp_state_cor)
									+Lloop.Freq_Err_Est;
		Rloop.Freq_Err_Sum+=Rloop.Freq_Err_Est;
		Rloop.Freq_Sum_Cnt++;
	}
	#endif
	/**** Update Local Loop  Note TAU of update is 1 second*****/
	/********/
   Lloop.Sprop1=((Lloop.Ygps_Buf[GPSDELAY-4])*Lloop.Propf1)+(Lloop.Sprop1*Lloop.Propf2);
  /*******/
	/****** TEST CODE ***
	Lloop.Sprop1=(-1.75e-9*Lloop.Propf1)+(Lloop.Sprop1*Lloop.Propf2);
   *****/
	Lloop.Sprop2=(Lloop.Sprop1*Lloop.Propf1)+(Lloop.Sprop2*Lloop.Propf2);
	Lloop.min_cnt++;
	Lloop.Prop_State=Lloop.Sprop2;
	if(Lloop.min_cnt>59)/*update once per minute*/
	{
	  Lloop.min_cnt=0;  
	  /* GPZ made slight correction on 30 sec compensation to null drift better*/
	  Drift=(Lloop.Sprop2-Lloop.Sprop_prev)*Lloop.Prop_Tau/29.15027;
	 if (LTBI.lotype!=RB)
	 {
	 	if(Lloop.Cal_Mode<3)
	 	{
			/*GPZ zero out integral term during initial acq*/
			/*if(Lloop.minute_cnt < Lloop.shift_cnt/2)*/
			{
			 Lloop.Int_State=0.0;
			 Drift=0;
			}
	 	}
	 }	 
	  Lloop.Int_State= (Drift*Lloop.Intf1)+(Lloop.Int_State*Lloop.Intf2);
	  Lloop.Sprop_prev=Lloop.Sprop2;
	}
	/*Lloop.Prop_State += (Lloop.Freq_Err_Est-Lloop.NCO_Cor_State)/Lloop.Prop_Tau;
	Lloop.Int_State  +=  (Lloop.Prop_State)/Lloop.Int_Tau; */
  /* Lloop.Dint_State += (Lloop.Prop_State+ Lloop.Int_State)*TAUZERO/Lloop->Dint_Tau;*/
  /***** Local NCO Update *********/  
	Lloop.NCO_Cor_State = Lloop.Prop_State +Lloop.Int_State;   
	#if REMOTE
	/****** Update Remote Loop *****/
	Yro_tc=smoothl*Yro_tc+
			 smooths*(-prma->Yroavg - smi.RO_temp_state_cor+ smi.temp_state_cor);
	/****** ADD Majority Vote to calculate weight *****/
	Rloop.Prop_State += (Rloop.Freq_Err_Est -Rloop.NCO_Cor_State)
											 /Rloop.Prop_Tau;
	Rloop.Int_State  +=  (Rloop.Prop_State)
											 /Rloop.Int_Tau;
	/*****NCO Update *********/
	Rloop.NCO_Cor_State =   Rloop.Prop_State  +
					Rloop.Int_State  +
					Yro_tc*Rloop.LO_weight;
	/*temporarily jam smi RO to NCO state*/
	smi.RO_freq_state_cor= Rloop.NCO_Cor_State;
	#endif
	Control_Loop_Shell(); /** update control shell mailboxes**/
}
/********************************************************
The Recalibrate Module supports the management of the time constants of
each of the control loops during any required recalibration period.
Recalibration is used during warmup for all loops.
It is also used when a frequency step transient is detected at the
input to the dual loops.
Recalibration adjusts the time constants through the shared (common) memory
interface (smi).
*******************************************************/
void Recalibrate()
{
	unsigned short *Cal_tc,*Prop_tc,*Int_tc,Cal_Final;
	short i;    
	double scale;
	struct Dual_Input_FLL_Control_System *pdual;
	struct Single_Input_FLL_Control_System *psingle;


	for(i=1;i<CLOOPS;i++) /*for both SPAN Channels and both remote (external) oscillators */
	{
		 if(i==SPA_LOOP)
		 {
			pdual = &DloopSA;
			Cal_tc=&(smi.dloopsa_cal_tc);
			Prop_tc=&(smi.dloopsa_prop_tc);
			Int_tc=&(smi.dloopsa_int_tc);
			Cal_Final=DUAL_SP_CAL_TAU;
		 }
		 else if(i==SPB_LOOP)
		 {
			pdual= &DloopSB;
			Cal_tc=&(smi.dloopsb_cal_tc);
			Prop_tc=&(smi.dloopsb_prop_tc);
			Int_tc=&(smi.dloopsb_int_tc);
			Cal_Final=DUAL_SP_CAL_TAU;

		 } 
		 else if(i==ROA_LOOP)
		 {
			pdual= &DloopRA;
			Cal_tc=&(smi.dloopra_cal_tc);
			Prop_tc=&(smi.dloopra_prop_tc);
			Int_tc=&(smi.dloopra_int_tc);
			Cal_Final=DUAL_RO_CAL_TAU;
		 }
		 else if(i==ROB_LOOP)
		 {
			pdual= &DloopRB;
			Cal_tc=&(smi.dlooprb_cal_tc);
			Prop_tc=&(smi.dlooprb_prop_tc);
			Int_tc=&(smi.dlooprb_int_tc);
			Cal_Final=DUAL_RO_CAL_TAU;

		 }		 
//		 if(pdual->Recalibrate&&Is_OCXO_warm())
		 if(pdual->Recalibrate) //GPZ June 2010 Do not depend on oscillator warm
		 {
			 if(*Cal_tc >= Cal_Final||(pdual->Start_Recal))
			 {
				pdual->Start_Recal=FALSE;
//				*Cal_tc=  Cal_Final/16; /*jam to 450 sec to start */
				*Cal_tc= 450; /*jam to 450 sec to start */
				*Prop_tc=(unsigned short)(DUAL_SP_PROP_PRIM_TAU);
				*Int_tc =(unsigned short)(DUAL_SP_INT_PRIM_TAU);/* jam integral time
												  constant to max to prevent oscillation */
				smi.Overide_Mode=1;
				pdual->Gcalf1=3.0/(*Cal_tc);
				pdual->Gcalf2=1.0-pdual->Gcalf1;
/*				pdual->Ygps_scal2=pdual->Ygps_Buf[GPSDELAY-1];*/
/*				pdual->Ygps_scal1=pdual->Ygps_scal2;*/
				pdual->minute_cnt=0;
				pdual->shift_cnt=((*Cal_tc+60)/25); /*number of minutes to stay at current
													time constant*/
				pdual->jamloop=TRUE;
				pdual->Cal_Mode=1;
			 }
			 if(pdual->minute_cnt > pdual->shift_cnt) /*time to shift up*/
			 {
				if(pdual->jamloop)
				{
//				  pdual->Prim_Int_State=pdual->Prim_Cor_State;
//				  pdual->Prim_Prop_State=0.0;
				  pdual->jamloop=FALSE;
				  if(i<3) /*span line loops*/
				  {
					 *Prop_tc=DUAL_SP_PROP_PRIM_TAU;
					 *Int_tc=DUAL_SP_INT_PRIM_TAU;
				  }
				  else
				  {
					 *Prop_tc=DUAL_RO_PROP_PRIM_TAU;
					 *Int_tc=DUAL_RO_INT_PRIM_TAU;
				  }
					smi.Overide_Mode=1;
				}
				*Cal_tc *=2;
				pdual->Cal_Mode++;
				if(*Cal_tc >=Cal_Final)
				{
					*Cal_tc=Cal_Final;
					smi.Overide_Mode=1;
					pdual->Recalibrate=FALSE;
				}
				if(pdual->Recalibrate)pdual->Gcalf1=2.0/(*Cal_tc);
				else  pdual->Gcalf1=1.0/(*Cal_tc);
				pdual->Gcalf2=1.0-pdual->Gcalf1;
				pdual->minute_cnt=0;
				pdual->shift_cnt=((*Cal_tc+60)/20);
			}
			 pdual->minute_cnt++;
		 } /*end if recalibrate is TRUE */
	}/*end for both primary rate channel*/

	/* local oscillator control loop recalibration*/
	psingle = &Lloop;
	Prop_tc=&(smi.sloop_prop_tc);
//	if(psingle->Recalibrate&& ASMM.GPS_Flag&&Is_OCXO_warm())
	if(psingle->Recalibrate&& ASMM.GPS_Flag)   //GPZ June 2010 Do not depend on ocxo warm
	{                     
	 	if(!psingle->Cal_Mode)/*start of gps local loop recalibration*/
		{
			*Prop_tc=(LTBI.single_prop_tau)/LTBI.single_accel;
			smi.sloop_int_tc=(unsigned int)(MAX_INT_TC);
			psingle->Int_State=0.0;
			psingle->Propf1=20.0/(*Prop_tc);
			psingle->Propf2=1.0-psingle->Propf1;
			psingle->Intf1=60.0/(smi.sloop_int_tc);
			psingle->Intf2=1.0-psingle->Intf1;
			psingle->minute_cnt=0;
			psingle->shift_cnt=((*Prop_tc+60)/20); /*number of minutes to stay at current
											time constant*/
			smi.Overide_Mode=1;
			psingle->Cal_Mode=1;
	 	}
	 	if(psingle->minute_cnt > psingle->shift_cnt) /*time to shift up*/
	 	{
			if(psingle->Cal_Mode==1)
			{
		  	smi.sloop_int_tc=(unsigned int)(LTBI.single_int_tau/(LTBI.single_recal*LTBI.single_recal));
		  	*Prop_tc=(LTBI.single_prop_tau)/LTBI.single_recal;
		  	psingle->Int_State=0.0;
			}
			else
			{
				*Prop_tc =(unsigned int)(LTBI.gear_space* (double)(*Prop_tc)+.5);
				smi.sloop_int_tc=(unsigned int)(LTBI.gear_space * LTBI.gear_space * (double)(smi.sloop_int_tc)+.5);
			}
			if(*Prop_tc >=LTBI.single_prop_tau)
			{
				*Prop_tc= LTBI.single_prop_tau;
			 	smi.sloop_int_tc=LTBI.single_int_tau;
			 	psingle->Recalibrate=FALSE;
			}
			psingle->Propf1=1.0/(*Prop_tc);
			psingle->Propf2=1.0-psingle->Propf1;
			psingle->Intf1=60.0/(smi.sloop_int_tc);
			psingle->Intf2=1.0-psingle->Intf1;

/*			if(LTBI.lotype!=RB)*/       
#if 1
			{   
			 /* With EFC control phase will interfere with frequency need to scale back*/
			
			 /**** GPZ reworked to be based on Gear_Space and Cal_mode****/ 
		 		scale= (LTBI.gear_space * (psingle->Cal_Mode+1)/7.0);  
		 		if(scale<1.0) scale =1.0;
		 		psingle->Int_State*=scale;
		 	/****** reuse scale to correct for increased delay in prop filt*/
		 		scale = psingle->Int_State*(1.0-1.0/LTBI.gear_space);
		 		psingle->Prop_State-=scale;
		 		psingle->Sprop2=psingle->Prop_State;
		 		psingle->Sprop1-=(2.0*scale);
			}  
#endif                  
#if 0			
			else
			{
/*		 	RB drift is insignificant*/
		 		psingle->Int_State=0.0;
		 		psingle->Sprop2=psingle->Prop_State;
			}
#endif			
			psingle->min_cnt=0;/*resync drift calculation to new time constants*/
			psingle->Sprop_prev=psingle->Prop_State-30.0*psingle->Int_State/(*Prop_tc);
			psingle->minute_cnt=0;
	 		psingle->shift_cnt=((*Prop_tc+60)/15);

			smi.Overide_Mode=1;
			psingle->Cal_Mode++;

	 	}
	 	psingle->minute_cnt++; 
	} /*end if recalibrate is TRUE */
}
/*************************************************************************
Module to update holdover state
*************************************************************************/   
static double pvar_wa=1;
static double pvar_wb=0; 
static double pred_var=0; 
static short hold_count[4]={0,0,0,0};
void Holdover_Update()
{
	unsigned short Hfreq_Valid,Hskip,Hgps_Valid;
	double Delta_Hold,Old_Freq,Old_Base,Old_Var;
	double freq_est,ftemp,pred_err,max_pred;
	double temp,fm_mod,freq_no_mod;
	double SlewWindow;
	short i,j,lag_indx,lag0,lag1,lag2,lag3;
	unsigned short Status;
	/******* local input message data *****/
	double Lloop_Freq_Err_Sum_rec, Rloop_Freq_Err_Sum_rec;
	double SPA_GPS_Prop_State_rec,SPB_GPS_Prop_State_rec;
	double ROA_GPS_Prop_State_rec,ROB_GPS_Prop_State_rec;
	unsigned short Lloop_Freq_Sum_Cnt_rec, Rloop_Freq_Sum_Cnt_rec;
	unsigned short SPA_Recal_rec,SPB_Recal_rec,ROA_Recal_rec,ROB_Recal_rec;
	unsigned short SPA_Mreg_rec,SPB_Mreg_rec,ROA_Mreg_rec,ROB_Mreg_rec;
	struct MSG_FLL_HOLDOVER *pholdrec;        
//	PRS_Loop_Print();
	SPA_Loop_Print();  
	PRR_Holdover_Print(); 
	PRR_Monthly_Holdover_Print();  
	PRR_Daily_Holdover_Print();
	/* receive 1 sec FLL input message*/
	if(MSG_FLL_HOLDOVER_A.Ready) pholdrec= &(MSG_FLL_HOLDOVER_A);
	else pholdrec= &(MSG_FLL_HOLDOVER_B);
	Lloop_Freq_Err_Sum_rec=pholdrec->Lloop_Freq_Err_Sum;
	SPA_GPS_Prop_State_rec=pholdrec->SPA_GPS_Prop_State;
	SPB_GPS_Prop_State_rec=pholdrec->SPB_GPS_Prop_State;
	ROA_GPS_Prop_State_rec=pholdrec->ROA_GPS_Prop_State;
	ROB_GPS_Prop_State_rec=pholdrec->ROB_GPS_Prop_State;
	SPA_Recal_rec=pholdrec->SPA_Recal;
	SPB_Recal_rec=pholdrec->SPB_Recal;
	ROA_Recal_rec=pholdrec->ROA_Recal;
	ROB_Recal_rec=pholdrec->ROB_Recal;
	SPA_Mreg_rec=pholdrec->SPA_Mreg;
	SPB_Mreg_rec=pholdrec->SPB_Mreg;
	ROA_Mreg_rec=pholdrec->ROA_Mreg;
	ROB_Mreg_rec=pholdrec->ROB_Mreg;
	Lloop_Freq_Sum_Cnt_rec=pholdrec->Lloop_Freq_Sum_Cnt;
	#if REMOTE
	Rloop_Freq_Err_Sum_rec=pholdrec->Rloop_Freq_Err_Sum;
	Rloop_Freq_Sum_Cnt_rec=pholdrec->Rloop_Freq_Sum_Cnt;
	#endif
	pholdrec->Ready=FALSE;
	/******* complete receive of local input data *****/
	/**** accumulate one minute data */
	HMS.FreqSum+=Lloop_Freq_Err_Sum_rec;
	HMS.FreqSumCnt+=Lloop_Freq_Sum_Cnt_rec;
	#if REMOTE
	HMR.FreqSum+=Rloop_Freq_Sum_Cnt_rec;
	HMR.FreqSumCnt+=Rloop_Freq_Sum_Cnt_rec;
	#endif
	/* calculate cumulative holdover stats over 4 hour window*/
	if(smi.BT3_mode==Holdover)
	{
		if((TrkStats.Hold_Stats&0xFF)<241) TrkStats.Hold_Stats++;
	  	if(TrkStats.Last_Mode==GPS_Normal)
	  	{
		 	if((TrkStats.Hold_Stats&0xFF00)<0xFF00) TrkStats.Hold_Stats+=0x100;
	  	}
	}
	TrkStats.Last_Mode=smi.BT3_mode;
	TrkStats.Min_Cnt++;
	if(TrkStats.Min_Cnt>239)
	{
	  	TrkStats.Min_Cnt=0;
	  	TrkStats.Hold_Stats_Latch=TrkStats.Hold_Stats;
	  	TrkStats.Hold_Stats=0;
	}
   /******** end holdover stats calculation *********************/
	/******* end one minute accumulate*/
	ASMM.Min_Cnt++;
	/******* generate 1 minute message from Holdover to FLL ********/
	MSG_HOLDOVER_FLL_A.Lloop_Osc_Freq_Pred=HMS.Osc_Freq_State;
#if REMOTE
	MSG_HOLDOVER_FLL_A.Rloop_Osc_Freq_Pred=HMR.Osc_Freq_State;
#endif
	MSG_HOLDOVER_FLL_A.SPA_Cal_Freq_Pred= HMSA.Osc_Freq_State;
	MSG_HOLDOVER_FLL_A.SPB_Cal_Freq_Pred= HMSB.Osc_Freq_State;    
	MSG_HOLDOVER_FLL_A.ROA_Cal_Freq_Pred= HMRA.Osc_Freq_State;
	MSG_HOLDOVER_FLL_A.ROB_Cal_Freq_Pred= HMRB.Osc_Freq_State;
	MSG_HOLDOVER_FLL_A.Tracking_Success=TrkStats.Daily_Track_Success;
	MSG_HOLDOVER_FLL_A.Ready=TRUE;
	/*************end message generation *********************/
	/******* generate 1 minute message from Holdover to NCOUP ********/
	NCOVAR(MSG_HOLDOVER_NCOUP_A.Lloop_Pred24_Var,HMS.Predict_Err24Hr,LO_TDEV_24);
#if REMOTE
	NCOVAR(MSG_HOLDOVER_NCOUP_A.Rloop_Pred24_Var,HMR.Predict_Err24Hr,LO_TDEV_24);
#endif
	NCOVAR(MSG_HOLDOVER_NCOUP_A.SPA_Pred24_Var,HMSA.Predict_Err24Hr,SP_TDEV_24);
	NCOVAR(MSG_HOLDOVER_NCOUP_A.SPB_Pred24_Var,HMSB.Predict_Err24Hr,SP_TDEV_24);
	NCOVAR(MSG_HOLDOVER_NCOUP_A.ROA_Pred24_Var,HMRA.Predict_Err24Hr,RO_TDEV_24);
	NCOVAR(MSG_HOLDOVER_NCOUP_A.ROB_Pred24_Var,HMRB.Predict_Err24Hr,RO_TDEV_24);
	MSG_HOLDOVER_NCOUP_A.Ready=TRUE;
	/*************end message generation *********************/

	/****** Update BT3 Shell  Holdover Report Mailboxes****/
	Holdover_Report();

	
#if NORMAL_HOUR
	if(Lloop.Cal_Mode<4) /*restart hourly collection process*/
#else
	if(Lloop.Cal_Mode<3) /*accelerate holdover mode*/
#endif
	{
	  /* generate initial estimates for Osc Freq State*/
	 	if(HMS.FreqSumCnt)
	  	{
			temp=HMS.FreqSum/HMS.FreqSumCnt;
			if(Lloop.Cal_Mode<2) HMS.Osc_Freq_State=temp;
			else HMS.Osc_Freq_State=0.2*HMS.Osc_Freq_State+0.8*temp;      
			gpz_test_seq[0]==1;   
			
	  	}
#if REMOTE
	  	if(HMR.FreqSumCnt)
	  	{
			temp=HMR.FreqSum/HMR.FreqSumCnt;
			if(Lloop.Cal_Mode<2) HMR.Osc_Freq_State=temp;
			else HMR.Osc_Freq_State=0.95*HMR.Osc_Freq_State+0.05*temp;
	  	}
#endif
	  	ASMM.Min_Cnt=0;
	  	HMS.FreqSum=0.0;
	  	HMS.FreqSumCnt=0;
#if REMOTE
	  	HMR.FreqSum=0.0;
	  	HMR.FreqSumCnt=0;
#endif
	}
	else if(ASMM.Min_Cnt>MIN_PER_HOUR) /*perform hourly update*/
	{
	  	ASMM.Min_Cnt=0;
	  /*front end to assign pointers for multiple channels*/ 
//	  	for(i=0;i<1;i++) /*single channel RTHC application */	  
#if REMOTE
//	  	for(i=0;i<2;i++) /*two channel application LO and RO */
#endif
#if !REMOTE
		for(i=0;i<5;i++) /*multi channel application LO, spans  */
#endif
	 	{    
	 		gpz_test_seq[i]=0;  
	 		Hfreq_Valid=TRUE;
 			Hgps_Valid=TRUE;	 		
	 		Hskip=FALSE;
	 		if(i==GPS_LOOP)
	 		{
				if(HMS.FreqSumCnt > HOUR_FREQ_CNT_MIN) 
				{
					freq_est=( HMS.FreqSum/HMS.FreqSumCnt); 
					gpz_test_freq=freq_est; 
					gpz_test_cnt=HMS.FreqSumCnt;
					Hgps_Valid=TRUE;
					Hfreq_Valid=TRUE; 
					gpz_test_seq[i]|=2;
				}
				else
				{
		 			Hfreq_Valid=FALSE;
		 			Hgps_Valid=FALSE;
		 			gpz_test_seq[i]|=4; 
//		 			gpz_test_cnt=0;
				}
			/***** Only update tracking success when in normal hour update mode******/				  
#if NORMAL_HOUR				
				TrkStats.Total_Good_Min-= TrkStats.Good_Track_Min[HMS.Cur_Hour];
				TrkStats.Good_Track_Min[HMS.Cur_Hour]=(HMS.FreqSumCnt)/60;
				TrkStats.Total_Good_Min+= TrkStats.Good_Track_Min[HMS.Cur_Hour];
				TrkStats.Cur_Hour=HMS.Cur_Hour;
				TrkStats.Daily_Track_Success=(float)(TrkStats.Total_Good_Min)/14.4;
#endif				
				HMS.FreqSum=0.0;
				HMS.FreqSumCnt=0;
				phm = &HMS;
	 		}
#if REMOTE
			else if(i==0)
	 		{
				if(HMR.FreqSumCnt > HOUR_FREQ_CNT_MIN) 
				{
					freq_est=(HMR.FreqSum/HMR.FreqSumCnt);
					Hfreq_Valid=TRUE;
				}
				else Hfreq_Valid=FALSE;
				HMR.FreqSum=0.0;
				HMR.FreqSumCnt=0;
				phm= &HMR;
	 		}
#endif
#if !REMOTE
	 		else if(i==SPA_LOOP)
	 		{
				if((!SPA_Mreg_rec)&&(Hgps_Valid))
				{
					freq_est= SPA_GPS_Prop_State_rec;
					Hfreq_Valid=TRUE;  
					gpz_test_seq[i]|=2;					
				}
				else
				{
					Hfreq_Valid=FALSE;
					gpz_test_seq[i]|=4;					
				}
				if(SPA_Mreg_rec&0x120) Hskip=TRUE;
				phm = &HMSA;
				if(SPA_Recal_rec) phm->Restart=TRUE;
	 		}
	 		else if(i==SPB_LOOP)
	 		{
				if((!SPB_Mreg_rec)&&(Hgps_Valid))
				{
					freq_est=SPB_GPS_Prop_State_rec;
					Hfreq_Valid=TRUE;
				}
				else
				{
		  			Hfreq_Valid=FALSE;
				}
				if(SPB_Mreg_rec&0x120) Hskip=TRUE;
				phm = &HMSB;
				if(SPB_Recal_rec) phm->Restart=TRUE;
	 		} 
	 		
			else if(i==ROA_LOOP)
			{
				if((!ROA_Mreg_rec)&&(Hgps_Valid))
				{
					freq_est=  ROA_GPS_Prop_State_rec;
					Hfreq_Valid=TRUE;
				}
				else
				{
					Hfreq_Valid=FALSE;
				}
				if(ROA_Mreg_rec&0x120) Hskip=TRUE;
				phm = &HMRA;
				if(ROA_Recal_rec) phm->Restart=TRUE;
			}
			else if(i==ROB_LOOP)
			{
				if((!ROB_Mreg_rec)&&(Hgps_Valid))
				{
					freq_est= ROB_GPS_Prop_State_rec;
					Hfreq_Valid=TRUE;
				}
				else
				{
				  Hfreq_Valid=FALSE;
				}
				if(ROB_Mreg_rec&0x120) Hskip=TRUE;
				phm = &HMRB;
				if(ROB_Recal_rec) phm->Restart=TRUE;
			 }	 		
#endif
			if(Hskip)
	 		{ 
	 			gpz_test_seq[i]|=8;
#if PRINTLOG&&NORMAL_HOUR
				printf("%s%5ld:Holdover_Update:Holdover update skip  channel: %d not provisioned\n",PHEAD,ptime,i);
				
#endif
	 		}
	 		else
	 		{
	  			lag0=phm->Cur_Hour;
	  			lag1=(phm->Cur_Hour+23)%24;
	  			lag2=(phm->Cur_Hour+22)%24;
	  			lag3=(phm->Cur_Hour+21)%24;
	  			Old_Freq = phm->Daily_Freq_Log[lag0];
	  			Old_Base = phm->Holdover_Base[lag0];
	  			Old_Var  = phm->Prediction_Var24[lag0];
	  			phm->Nsum_Hour++;   /*max out at 24 see code */  
  				gpz_test_seq[i]|=8192;
	  			
	  			if(Hfreq_Valid&&(hold_count[i]&0x01)==0) /*good data */
	  			{ 
	  			hold_count[i]=0;        
	  			/*** GPZ add renormalize holdover if large step****/
 		  		Delta_Hold = freq_est - phm->Daily_Freq_Log[lag1];
                if(Delta_Hold<0) Delta_Hold *=-1.0;  
                /*open up slew window for 1st day */
 		  		SlewWindow = MAX_HOLD_SLEW*24.0/(phm->Nsum_Hour);
		  			if(phm->Restart|(Delta_Hold>SlewWindow))
		  			{    
		  				gpz_test_seq[i]|=16;      
						if(phm->Restart&&i==0)
						{
							DebugPrint("Restart init HLSF\n\r", 3);  
							HLSF.finp=freq_est;  
							HLSF.wa=1.0; 
							HLSF.wb=0.0;        
							HLSF.dsmooth=phm->Long_Term_Drift;
						}	
						phm->Restart=FALSE;
						phm->Nsum_Hour=1;
						phm->Cur_Hour=0;
						lag0=0;
						lag1=23;
						lag2=22;
						lag3=21;
						temp=freq_est;  
						for (j=24;j>0;j--)
						{
							lag_indx=(lag0+j)%24;
							phm->Daily_Freq_Log[lag_indx]= temp;
							phm->Holdover_Base[lag_indx] = temp;
							phm->Holdover_Drift[lag_indx]=phm->Long_Term_Drift;
							phm->Prediction_Var24[lag_indx]=0.0;
							temp -=phm->Long_Term_Drift;
						}
						Old_Freq = temp;
						Old_Base = temp;
						Old_Var  = 0.0;
						phm->Freq_Sum_Daily=0.0;
            			phm->Var_Sum_Daily =0.0; 
            			// Special Case if large freq step double pump to clear 
            			if(Delta_Hold>SlewWindow)
            			{
            				phm->Restart=TRUE;
            				return;
            			}	
		  			} 

		  			Delta_Hold = freq_est - phm->Daily_Freq_Log[lag1];
		  			if(Delta_Hold > SlewWindow)
		  			{
						Delta_Hold = SlewWindow;
#if PRINTLOG
				  		printf("%s%5ld:Holdover_Update:Hourly update slew limit for channel: %d\n",PHEAD,ptime,i);
#endif
		  			}
		  			else if(Delta_Hold< -SlewWindow)
		  			{
						Delta_Hold = -SlewWindow;
#if PRINTLOG
				  		printf("%s%5ld:Holdover_Update:Hourly update slew limit for channel: %d\n",PHEAD,ptime,i);
#endif
		  			}
		  			freq_no_mod=  phm->Daily_Freq_Log[lag1]+ Delta_Hold;
					phm->Daily_Freq_Log[lag0]= freq_no_mod;
					gpz_test_seq[i]|=32; 
					fm_mod=0.0;

	  			}
	  /***** GPZ 12-13-00 prevent forcast until first good data*/
//	  			else if(phm->Restart) return;
	  			else
	  			{
		  /* forcast next freq word*/ 
	 	 /***** GPZ 12-13-00 prevent forcast until first good data*/
		  			if(phm->Restart) return;  
		  			gpz_test_seq[i]|=4096;  
		  			hold_count[i]++;        
		  			/* always add FM mod in GPS loop*/
//					if(LTBI.lotype!=RB)
//					{   
						freq_no_mod= phm->Holdover_Base[lag1]+ phm->Holdover_Drift[lag1];
			  			phm->Daily_Freq_Log[lag0]= freq_no_mod;
			 			gpz_test_seq[i]|=64;
			 			/*Inserted FM modulation to keep prediction error at
			 			upper limit for current smart clock condition*/
			 			if(i==GPS_LOOP)
			 			{
			 			 	if(get_prr_smartclk()==SMART_ON)
			 			 	{  
			 			 	
			 			 		temp=SMART_NARROW_FM;   
								if(HMS.Predict_Err24Hr<temp)temp=HMS.Predict_Err24Hr;
								if(LTBI.lotype==RB)	temp=temp*0.75; 
			 			 	 
						 	}
			 			 	else
			 			 	{
								temp=SMART_WIDE_FM;			 			 	
			 			 	}  
	//	 			 		if((lag0%2)==0||(lag0%8)==3)
		 			 		if((lag0%2)==0) /* no need for complex modulation scheme*/
			 			 	{  
			 			 		gpz_test_seq[i]|=1024;
		 			 	  		phm->Daily_Freq_Log[lag0]+=temp; 
		 			 	  		fm_mod=temp;
		 			 	  	}
		 			 	  	else
		 			 	  	{
			 			 		gpz_test_seq[i]|=2048;
		 			 	  		phm->Daily_Freq_Log[lag0]-=temp;
		 			 	  		fm_mod=-temp;
		 			 	  	}
			 			} /*end add FM modulation*/
//					}
//					else
//					{
//			 			phm->Daily_Freq_Log[lag0]= phm->Holdover_Base[lag1]+ phm->Holdover_Drift[lag1];
//
//					}
#if (PRINTLOG)
					printf("%s%5ld:Holdover_Update:Hourly Freq Invalid for channel: %d\n",PHEAD,ptime,i);
#endif
	  			}  
	  			                           
	  /******** update holdover state calculations *******/
	  			if(LTBI.lotype!=RB)
	  			{
	   				if(i==GPS_LOOP)
	   				{
		 				if(phm->Nsum_Day <2) /*use early aging curve during first 3 days */
		 				{
							Holdover_Inverse_Drift_Fit(freq_no_mod);
							//ftemp=HLSF.bfit+HLSF.N*HLSF.mfit; /*current inverse fit*/
							ftemp=HLSF.dsmooth ; // strait line fit
							gpz_test_seq[i]|=128;
							for(j=0;j<24;j++)
							{
			 					if(j<phm->Nsum_Hour)
			 					{
			  						lag_indx=(lag0-j+24)%24;
//			  						if(ftemp!=0.0)
//			  						{ 
			  							phm->Holdover_Drift[lag_indx]= ftemp;
				 						//phm->Holdover_Drift[lag_indx]= 1.0/ftemp;
				 						/****** GPZ add sanity constraints 12-10-00*****/
			 	 						if(phm->Holdover_Drift[lag_indx] > MAX_DRIFT_RATE)
				 						{
											phm->Holdover_Drift[lag_indx]= MAX_DRIFT_RATE;
				 						}
			 	 						else if(phm->Holdover_Drift[lag_indx] < -MAX_DRIFT_RATE)
				 						{
											phm->Holdover_Drift[lag_indx]= -MAX_DRIFT_RATE;
				 						}
//			  						}
//			  						ftemp-= HLSF.mfit; SKIP FOR STRAIT LINE FIT   
//									if(Get_Clock_Mode()<2)
									if(0)
									{
									  phm->Holdover_Drift[lag_indx]=0.0;     
									  phm->Long_Term_Drift=0.0;
									}  
									  
			 					}
							}
		 				}
		 				else
		 				{   
		 					gpz_test_seq[i]|=256;         
//		 					if(Get_Clock_Mode()<2)  
							if(0)
							{
									  phm->Long_Term_Drift=0.0;
							}  

                            phm->Holdover_Drift[lag0]= phm->Long_Term_Drift;
		 				}
	   				}
     			}
	  			ftemp=  (phm->Daily_Freq_Log[lag0]+phm->Daily_Freq_Log[lag1]);
				ftemp+= (phm->Daily_Freq_Log[lag2]+phm->Daily_Freq_Log[lag3]); 
	 	 		if(LTBI.lotype!=RB)
	  			{
	   				if(i==SPA_LOOP||i==SPB_LOOP)
	   				{
		  				phm->Holdover_Drift[lag0]= 0.0;  
						phm->Holdover_Base[lag0] =ftemp/4.0; /*use 4 hour average as base*/

//		  				if(phm->Nsum_Day==0)
//		  				{
//							phm->Holdover_Base[lag0] =ftemp/4.0; /*use 4 hour average as base*/
//		  				}
//		  				else
//		  				{
//							phm->Holdover_Base[lag0]=phm->Monthly_Freq_Log[((phm->Cur_Day+29)%30)];
//		  				}
	   				}
	   				else
	   				{   
	   					gpz_test_seq[i]|=512;    
	   					phm->Holdover_Base[lag0]=freq_no_mod;
//		 				phm->Holdover_Base[lag0] =ftemp/4.0 +1.5*phm->Holdover_Drift[lag1]; /*use 4 hour average as base*/
		 				if(i!=GPS_LOOP) phm->Holdover_Drift[lag0]= phm->Long_Term_Drift;
	   				}
    			}
    			else
				{
	   				phm->Holdover_Drift[lag0]= 0.0;      
	 				phm->Holdover_Base[lag0] =freq_no_mod; /*use 4 hour average as base*/
	   				
//	   				if(phm->Nsum_Day==0)
//	   				{
//		 				phm->Holdover_Base[lag0] =ftemp/4.0; /*use 4 hour average as base*/
//	   				}
//	   				else
//	   				{
//		 				phm->Holdover_Base[lag0]=phm->Monthly_Freq_Log[((phm->Cur_Day+29)%30)];
//	   				}
    			}
	  			phm->Freq_Sum_Daily +=freq_no_mod;
	  /******** update 24 hr prediction error******/
	  			max_pred=0.0;
/*	  ftemp= Old_Base;*/
/*	  pred_err = Old_Freq-Old_Base;*/ 
				lag_indx=(17+lag0)%24;
	  			ftemp=phm->Holdover_Base[lag_indx];
	  			pred_err= 0;
	  			for(j=1;j<8;j++) //Reduced from 24 hrs to 8 for RTHC
	  			{
		 			lag_indx=(17+lag0+j)%24;
		 			ftemp+=phm->Holdover_Drift[lag_indx];
		 			pred_err+=phm->Daily_Freq_Log[lag_indx]-ftemp;
//		 			if(pred_err>max_pred) max_pred=pred_err;
//		 			else if(pred_err<-max_pred) max_pred= -pred_err;
	  			}
	  			max_pred = pred_err*3600.0;/*convert to time error estimate*/
	  /* factor in percent of holdover time which biases the prediction error
		  to zero*/
//	  			max_pred = ((100.0-TrkStats.Daily_Track_Success)*phm->Predict_Err24Hr +
//					  TrkStats.Daily_Track_Success*max_pred)/100.0;
#if REMOTE
//	  			if(i) 
//	  			{ /*ensemble prediction error for OCXO side */
//		 			max_pred+= HMS.Predict_Err24Hr;
//		 			max_pred *=0.5;
//	  			}
#endif
	  			max_pred *= max_pred;
	  			phm->Prediction_Var24[lag0]= max_pred;
	  			phm->Var_Sum_Daily+= max_pred;
	  /******* end prediction error update **********/
	  			if(phm->Nsum_Hour>24)
	  			{
					phm->Nsum_Hour=24;
					phm->Freq_Sum_Daily -= Old_Freq;
					phm->Var_Sum_Daily-=   Old_Var;
	  			} 
			if(i!=GPS_LOOP)
			{			
	  			if(phm->Nsum_Hour >8)
	  			{
		 			temp =phm->Var_Sum_Daily/(double)(phm->Nsum_Hour);
	  			}
	  			else
	  			{
		 			temp=max_pred; /*use current prediction during first 48hr*/
	  			}
	  			if(temp>0.0)  phm->Predict_Err24Hr=sqrt(temp);
	  			else
	  			{
#if PRINTLOG
		 			printf("%s%5ld:Holdover_Update:negative or zero variance estimate for channel: %d\n",PHEAD,ptime,i);
#endif
	  			} 
	 		 			
		    /**** progressively increase filtering to ****/   
		    }
		  	else //GPS loop variance estimate update
		    {  
			 if(phm->Nsum_Hour >7)
			 {	
			 	pred_var=pvar_wa*max_pred+pvar_wb*pred_var; 
			  	temp=pred_var;
 			 	/*reduce filter bandwidth to 24hr moving average equivalant*/
			  	if(pvar_wa>.0833)
			  	{
					pvar_wa*=.96;
				 	pvar_wb=1.0-pvar_wa;
			  	}
			 }
			 else temp=max_pred;  
			 phm->Predict_Err24Hr=sqrt(temp);
			 if(phm->Nsum_Hour >15)
			 {	
			 	if(get_prr_smartclk()==SMART_OFF ) 
			 	{
			    	pvar_wa=1.0;  
			    	pvar_wb=1.0-pvar_wa;
			 	}
			 }
		   } 

/*   phm->Osc_Freq_State= (double)(phm->Holdover_Base[lag0]+ 1.5*phm->Holdover_Drift[lag0]);
	  /*This is adjusted to make transition to holdover data seamless*/
	  			phm->Osc_Freq_State= (double)(phm->Holdover_Base[lag0]+0.5*phm->Holdover_Drift[lag0]);
	  			if(phm->Osc_Freq_State > MAX_HOLD_RANGE)
	  			{
		 			phm->Osc_Freq_State= MAX_HOLD_RANGE;
#if PRINTLOG
		 			printf("%s%5ld:Holdover_Update:Osc_Freq_State Maximum for channel: %d\n",PHEAD,ptime,i);
#endif
	  			}
	  			else if(phm->Osc_Freq_State < -MAX_HOLD_RANGE)
	  			{
		 			phm->Osc_Freq_State=  -MAX_HOLD_RANGE;
#if PRINTLOG
		 			printf("%s%5ld:Holdover_Update:Osc_Freq_State Maximum for channel: %d\n",PHEAD,ptime,i);
#endif
	  			}
	  			phm->Cur_Hour++;
	  			if(phm->Cur_Hour>23)  /* end of day logic  long term drift evaluation*/
	  			{
					phm->Cur_Hour=0;
		/* Update monthly data and drift estimate */
					phm->Nsum_Day++;
					phm->Monthly_Freq_Log[phm->Cur_Day]= (phm->Freq_Sum_Daily/(double)(phm->Nsum_Hour));
					phm->Monthly_Pred_Log[phm->Cur_Day]=(float)( phm->Predict_Err24Hr);
					if(phm->Nsum_Day >1)
					{
		  				temp =  ((double)(phm->Monthly_Freq_Log[phm->Cur_Day])
					 		-(double)(phm->Monthly_Freq_Log[(phm->Cur_Day+29)%30]))/24.0;
		  				if(phm->Nsum_Day ==3) phm->Drift_Sum_Weekly=2.0*temp; /*special case discard first drift*/
		  				else  phm->Drift_Sum_Weekly+=temp;
		  				if(phm->Nsum_Day>4)  /* for RTHC use 3 day average*/
		  				{
			  				phm->Nsum_Day=4;
			  				phm->Drift_Sum_Weekly -=
								((double)(phm->Monthly_Freq_Log[(phm->Cur_Day+27)%30])-
								 (double)(phm->Monthly_Freq_Log[(phm->Cur_Day+26)%30]))/24.0;
		  				}
				/*  incremental offset per hour */
		  				phm->Long_Term_Drift = phm->Drift_Sum_Weekly/(phm->Nsum_Day-1);
		 /* Update NCO Holdover Drift with Range Limits */
		 				if(phm->Long_Term_Drift > MAX_DRIFT_RATE)
		 				{
			  				phm->Long_Term_Drift= MAX_DRIFT_RATE;
#if PRINTLOG
			  				printf("%s%5ld:Holdover_Update:Daily update slew limit for channel: %d\n",PHEAD,ptime,i);
#endif
		 				}
		 				else if(phm->Long_Term_Drift < -MAX_DRIFT_RATE)
		 				{
			  				phm->Long_Term_Drift = -MAX_DRIFT_RATE;
#if PRINTLOG
			  				printf("%s%5ld:Holdover_Update:Daily update slew limit for channel: %d\n",PHEAD,ptime,i);
#endif
		 				}  
//		 				if(Get_Clock_Mode()<2) 
						if(0)
						{
								  phm->Long_Term_Drift=0.0;
						}  

					}  /* end if Nsum_Daily >1 */
					if(LTBI.lotype!=RB)
					{
		 				if(phm->Nsum_Day ==2) /*special case */
		 				{
		  					for(j=0;j<24;j++)phm->Holdover_Drift[j]= phm->Long_Term_Drift;
		 				}
					}
					phm->Cur_Day++;
					if(phm->Cur_Day>29)phm->Cur_Day=0;
	  			} /* end of end of day logic */
	 		} /*end else no holdover skip*/
		} /*end for each channel*/ 
//		 PRR_Holdover_Print();  //print debug info for smartclock
  	} /*end hourly update*/
}   
/*****************************************************************
Time Provider Holdover Debug Print Function
******************************************************************/
void PRR_Holdover_Print()
{    
	char cbuf[200];
 	int i1,i2;
	i1= (HMS.Cur_Hour+23)%24;  
	i2= (HMS.Cur_Day +29)%30;
    sprintf((char *)cbuf,"HOLD_GPS:%4d,%4x,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%3d,%3d,%3d,%3d,%3d\r\n",
//    (int)(get_ticks()&0xFFF), 
    gpz_test_cnt,
    gpz_test_seq[0],   

    (long int)(gpz_test_freq*(double)(1e15)),
    (long int)(HMS.Daily_Freq_Log[i1]*(double)(1e15)), 
    (long int)(HMS.Holdover_Base[i1]*(double)(1e15)), 
    (long int)(HMS.Holdover_Drift[i1]*(double)(1e15)), 
    (long int)(HMS.Monthly_Freq_Log[i2]*(double)(1e15)),     
    (long int)(HMS.Predict_Err24Hr*(double)(1e9)), 
    (long int)(HMS.Osc_Freq_State*(double)(1e15)), 
    (long int)(HMS.Long_Term_Drift*(double)(1e15)), 
    (long int)(  HLSF.finp*(double)(1e15)),   
   (long int)(  HLSF.dsmooth*(double)(1e15)),   
    (long int)(  HLSF.wa*(double)(1e2)),   
    (long int)(  HLSF.wb*(double)(1e2)),   
    HMS.Cur_Hour,
    HMS.Cur_Day,
    HMS.Nsum_Hour,
    HMS.Nsum_Day,
    HMS.Restart
//	get_smartclk(),    
//    (char *)hmode, 
//    (long int)(Get_micro_icw())            
    );   
	DebugPrint(cbuf, 3);  
}    
/*****************************************************************
Time Provider Daily Holdover Debug Print Function
******************************************************************/
void PRR_Daily_Holdover_Print()
{                    
	char cbuf[400];
 	int i2,i1;
	i2= (HMS.Cur_Hour +23)%24;     
    sprintf((char *)cbuf,"DAILY_GPS_HOLDOVER:index %4d\r\n",i2);   
	DebugPrint(cbuf, 11); 
    for(i1=0;i1<24;i1=i1+6)
    {
    	sprintf((char *)cbuf,"%15ld,%15ld,%15ld,%15ld,%15ld,%15ld,",
    	(long int)(HMS.Daily_Freq_Log[(i2+24-i1)%24]*(double)(1e15)),
    	(long int)(HMS.Daily_Freq_Log[(i2+23-i1)%24]*(double)(1e15)),
    	(long int)(HMS.Daily_Freq_Log[(i2+22-i1)%24]*(double)(1e15)),
    	(long int)(HMS.Daily_Freq_Log[(i2+21-i1)%24]*(double)(1e15)),
    	(long int)(HMS.Daily_Freq_Log[(i2+20-i1)%24]*(double)(1e15)),
    	(long int)(HMS.Daily_Freq_Log[(i2+19-i1)%24]*(double)(1e15))
    	);      
   		DebugPrint(cbuf, 11); 
   		sprintf((char *)cbuf,"\r\n");
 		DebugPrint(cbuf, 11); 
     }	
}
  
/*****************************************************************
Time Provider Monthly Holdover Debug Print Function
******************************************************************/
void PRR_Monthly_Holdover_Print()
{                    
	char cbuf[400];
 	int i2,i1;
	i2= (HMS.Cur_Day +29)%30;     
    sprintf((char *)cbuf,"MONTHLY_GPS_HOLDOVER:index %4d\r\n",i2);   
	DebugPrint(cbuf, 8); 
    for(i1=0;i1<30;i1=i1+6)
    {
    	sprintf((char *)cbuf,"%15ld,%15ld,%15ld,%15ld,%15ld,%15ld,",
    	(long int)(HMS.Monthly_Freq_Log[(i2+30-i1)%30]*(double)(1e15)),
    	(long int)(HMS.Monthly_Freq_Log[(i2+29-i1)%30]*(double)(1e15)),
    	(long int)(HMS.Monthly_Freq_Log[(i2+28-i1)%30]*(double)(1e15)),
    	(long int)(HMS.Monthly_Freq_Log[(i2+27-i1)%30]*(double)(1e15)),
    	(long int)(HMS.Monthly_Freq_Log[(i2+26-i1)%30]*(double)(1e15)),
    	(long int)(HMS.Monthly_Freq_Log[(i2+25-i1)%30]*(double)(1e15))
    	);      
   		DebugPrint(cbuf, 8); 
   		sprintf((char *)cbuf,"\r\n");
 		DebugPrint(cbuf, 8); 
     }	
}
/***************************************************************************
Module for testing holdover algorithm only.
*****************************************************************************/
void Holdover_Test()
{
	int i;
	/* jam to normal state with long duration */
	smi.BT3_mode=GPS_Normal;
	smi.BT3_mode_dur = (smi.sloop_int_tc *3)/60; /* force 3 time constants of
																	stable operation */
		DloopSA.Prim_Int_State = 1e-7; /*fixed offset test*/
	for(i=0;i<900;i++)
	{
		if(i%10==5&&i>50) smi.BT3_mode=GPS_Warmup;
		else smi.BT3_mode=GPS_Normal;
		DloopSA.Prim_Int_State += 1e-11; /*linear ramp test*/
		Holdover_Update();
	}
}  

/*****************************************************************
Time Provider PRS  Loop Debug Print Function
******************************************************************/
void PRS_Loop_Print()
{  
		char cbuf1[180];     
		fll_print=(fll_print+1)%64; 
		if(fll_print==57)
		{                         
		    sprintf((char *)cbuf1,"PRS-EngA: clk_mode:%4d, gear:%4d, GPSreg: %4x,CTSreg:%4x,GPS_valid:%4x,Trace_Skip:%4d,EFC_OK:%4d\r\n",   
	    	(int)(Get_Clock_Mode()),  
		    (int) Get_Gear(),
		    (int)SReg.GPS_Reg,
			(int)cts_reg, /*fake CTS channel for now*/	     
		    (int)GPS_Valid,
		    (int)(Lloop.Trace_Skip), 
		    (int) Is_EFC_OK()

	    	);
			DebugPrint(cbuf1, 7);
        }
 		else if(fll_print==59)
        {
		    sprintf((char *)cbuf1,"PRS-EngB: ECD_freq ppt: %7ld, Time_Cor ppt: %7ld, GPS_freq ppt: %7ld\r\n",   
		    (long int)(ECD.freq_state_cor*1.0e12),
	    	(long int)(smi.phase_state_cor*1.0e12),
		    (long int)(GM.Ygps*(double)(1.0e12))
		    );
			DebugPrint(cbuf1, 7);
		}   
		
		else if(fll_print==61)
        {
		    sprintf((char *)cbuf1,"PRS-EngC: FPGA_freq ppt: %7ld, Use_FPGA_Freq: %4d, Trace_Freq ppt:%7ld\r\n",   
		    (long int) (ken_MPLL_freq*(double)(1.0e12)),  
	    	(int) Is_EFC_OK(),
		    (long int)(Lloop.Freq_Err_Est*(double)(1.0e12))
		    );
			DebugPrint(cbuf1, 7);
		}
		else if(fll_print==63)
		{
		    sprintf((char *)cbuf1,"PRS-EngD: P_Tau%5ld, I_Tau:%5ld, Prop ppt: %7ld, Int ppt: %7ld, GPS_W:%4ld, PA_W:%4ld, PB_W:%4ld,stby_cnt:%4d\r\n",   
		    (long int) Lloop.Prop_Tau,
	    	(long int) Lloop.Int_Tau ,
			(long int)(Lloop.Prop_State*(double)(1.0e12)),
			(long int) (Lloop.Int_State*(double)(1.0e12)),  
			(long int) (ECD.GPS_weight*100.0),
			(long int) (ECD.PA_weight*100.0),
			(long int) (ECD.PB_weight*100.0),
	    	(int) standby_cnt
	    	);
			DebugPrint(cbuf1, 7);
	    }
#if 0		
	    sprintf((char *)cbuf1,"PRS-Eng:%d,%d,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%d,%ld,%ld,%ld\r\n",   
	    (int)(Get_Clock_Mode()),
	    (int)SReg.GPS_Reg,
		(int)cts_reg, /*fake CTS channel for now*/	     
	    (int)GPS_Valid,
	    (long int)(ECD.freq_state_cor*1.0e15),
	    (long int)(smi.phase_state_cor*1.0e15),
	    (long int)(GM.Ygps*(double)(1.0e15)),
	    (long int) (ken_MPLL_freq*(double)(1.0e15)),
	    (long int)(Lloop.Freq_Err_Est*(double)(1.0e15)),
	    (long int) Lloop.Prop_Tau,
	    (long int) Lloop.Int_Tau ,
		(long int)(Lloop.Prop_State*(double)(1.0e15)),
		(long int) (Lloop.Int_State*(double)(1.0e15)),  
//		(long int)(NCO_Cor*1000.0),
//		(long int) (NCO_Frac*1000.0),  
		(long int) (smi.NCO_Low),
		(long int)(Get_micro_icw()),   
	    (int)(Lloop.Trace_Skip), 
//	    (long int) Lloop.Recalibrate,
	    (long int) Is_EFC_OK(),
	    (long int) Get_Gear(),
	    (long int) standby_cnt
	    );
#endif	    
}            
/*****************************************************************
Time Provider SPAN-A  Loop Debug Print Function
******************************************************************/
void SPA_Loop_Print()
{  
		char cbuf1[120];
	    sprintf((char *)cbuf1,"SPA-Eng:%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",   
	    (long int)(ECD.freq_state_cor*1.0e15),
	    (long int)(DloopSA.Prim_Freq_Pred*1.0e15),
	    (long int) (YspavgA_rec*(double)(1.0e15)),
	    (long int) (smi.dloopsa_cal_tc),
	    (long int) (DloopSA.GPS_Prop_State*(double)(1.0e15)) ,
		(long int)(DloopSA.Prim_Prop_State*(double)(1.0e15)),
		(long int) (DloopSA.Prim_Int_State*(double)(1.0e15)),
	    (long int)(DloopSA.Prim_Cor_State*(double)(1.0e15)),
	    (long int) DloopSA.Recalibrate,
	    (long int) DloopSA.Cal_Mode,  
	    (long int) DloopSA.Prim_Skip,  
	    (long int) DloopSA.shift_cnt
	    );
		DebugPrint(cbuf1, 4);
}
void NCO_Update()
{
	double normvar;
	unsigned short PA_Flag, PB_Flag;
	/******* Receive 1 minute TCH task data  (every 10 minutes)****************/
	if(MSG_TCH_NCOUP_A.Ready)
	{
		ECD.GPS_mvar=MSG_TCH_NCOUP_A.GPS_mvar;
		ECD.PA_mvar= MSG_TCH_NCOUP_A.PA_mvar;
		ECD.PB_mvar= MSG_TCH_NCOUP_A.PB_mvar;
		ECD.LO_mvar= MSG_TCH_NCOUP_A.LO_mvar;
		MSG_TCH_NCOUP_A.Ready=FALSE;
	}
	/*complete receive of 1 TCH task mvar stability data */
	/******* Receive 1 minute Holdover task data ****************/
	if(MSG_HOLDOVER_NCOUP_A.Ready)
	{
		ECD.LO24_mvar=MSG_HOLDOVER_NCOUP_A.Lloop_Pred24_Var;
		ECD.SA24_mvar=MSG_HOLDOVER_NCOUP_A.SPA_Pred24_Var;
		ECD.SB24_mvar=MSG_HOLDOVER_NCOUP_A.SPB_Pred24_Var;
		ECD.RA24_mvar=MSG_HOLDOVER_NCOUP_A.ROA_Pred24_Var;
		ECD.RB24_mvar=MSG_HOLDOVER_NCOUP_A.ROB_Pred24_Var;
		MSG_HOLDOVER_NCOUP_A.Ready=FALSE;
	}
	/*complete receive of 1 Holdover task 24 hr mvar stability data */
	/********** calculate ensembled weighted output ********/
		/*logic is if either RO input is provisioned and not disabled and
		  span_only mode is not active then use RO inputs in ensemble*/
	if(
		(
		 (!(SReg.ROA_Reg&0x120)&&!(smi.Input_Mode&0x800))||
		 (!(SReg.ROB_Reg&0x120)&&!(smi.Input_Mode&0x1000))
		)
		 &&((smi.Input_Mode&0xFF)!=SPAN_ONLY)
	  )
	{
		ECD.Pmode=EXTERNAL_MODE;
		PA_Flag= (!(SReg.ROA_Reg&0x1BC)&&(!DloopRA.jamloop)&&!(smi.Input_Mode&0x800 ));
		PB_Flag= (!(SReg.ROB_Reg&0x1BC)&&(!DloopRB.jamloop)&&!(smi.Input_Mode&0x1000));
		/*calculate composite mvar*/
		if(smi.BT3_mode==Holdover) /*holdover case gently convert to holdover weighting*/
		{
			ECD.PAC_mvar= ECD.PAC_mvar*0.998 + ECD.RA24_mvar*0.002;
			ECD.PBC_mvar= ECD.PBC_mvar*0.998 + ECD.RB24_mvar*0.002;
			ECD.LOC_mvar= ECD.LOC_mvar*0.998 + ECD.LO24_mvar*0.002;
		}
	}
	else
	{
		ECD.Pmode=SPAN_MODE;
		PA_Flag= (!(SReg.SPA_Reg&0x1BC)&&(!DloopSA.jamloop)&&!(smi.Input_Mode&0x200));
		PB_Flag= (!(SReg.SPB_Reg&0x1BC)&&(!DloopSB.jamloop)&&!(smi.Input_Mode&0x400));
		/*calculate composite mvar*/
		if(smi.BT3_mode==Holdover) /*holdover case gently convert to holdover weighting*/
		{
			ECD.PAC_mvar= ECD.PAC_mvar*0.998 + ECD.SA24_mvar*0.002;
			ECD.PBC_mvar= ECD.PBC_mvar*0.998 + ECD.SB24_mvar*0.002; 
			ECD.LOC_mvar= ECD.LOC_mvar*0.998 + ECD.LO24_mvar*0.002;

		}
	}
	if(smi.BT3_mode!=Holdover)  /*use TCH stability if not in holdover*/
	{
		ECD.PAC_mvar=ECD.PA_mvar;
		ECD.PBC_mvar=ECD.PB_mvar;
//		ECD.LOC_mvar=ECD.LO_mvar;      GPZ use GPS noise in tracking as the loop is now shorter
		ECD.LOC_mvar=ECD.GPS_mvar;
	}
	/* code to slowly remove local oscillator weight after extended bridging time*/
	if(smi.BT3_mode==Holdover)  
	{
	   if(LTBI.lotype==OCXO||LTBI.lotype==DOCXO)
		{
		  if(smi.BT3_mode_dur > 240)
		  {
			ECD.LOC_mvar= ECD.LOC_mvar*0.998 + MVARMAX*0.002;
		  }
		}
		else 
		{
			if(smi.BT3_mode_dur > 1440)
			{
				ECD.LOC_mvar= ECD.LOC_mvar*0.998 + MVARMAX*0.002;
			}
		}
	
	}
	
	if((smi.Input_Mode&0xFF)==ALL_INPUTS)
	{
	  if(ASMM.GPS_Flag||PA_Flag||PB_Flag)
	  {
		 /*use LO mvar for GPS loop since LO dominates at 10 minute TAU and
			LO controls 24 hour holdover stability */
		 if(ECD.LOC_mvar >0.0) ECD.GPS_weight = 1.0/ECD.LOC_mvar;
		 if(ECD.PAC_mvar >0.0 ) ECD.PA_weight  = PA_Flag/ECD.PAC_mvar;
		 if(ECD.PBC_mvar >0.0 ) ECD.PB_weight  = PB_Flag/ECD.PBC_mvar; 
		 if(ECD.Pmode==SPAN_MODE)
		 {
//		    ECD.PA_weight=ECD.PA_weight*weight_correct(0);
//		    ECD.PB_weight=ECD.PB_weight*weight_correct(1);	 
//			GPZ June 2010 Override
			ECD.GPS_weight = 1.0;
			ECD.PA_weight =  0.0;
			ECD.PB_weight =  0.0;
		    	 
		 }
		 normvar = ECD.GPS_weight + ECD.PA_weight + ECD.PB_weight;
		 if(normvar>0.0)
		 {
//			ECD.GPS_weight = ECD.GPS_weight/normvar;
//			ECD.PA_weight =  ECD.PA_weight/normvar;
//			ECD.PB_weight =  ECD.PB_weight/normvar;    
//			GPZ June 2010 Override
			ECD.GPS_weight = 1.0;
			ECD.PA_weight =  0.0;
			ECD.PB_weight =  0.0;
			
		 }
		 else
		 {
			ECD.GPS_weight = 1.0;
			ECD.PA_weight =  0.0;
			ECD.PB_weight =  0.0;
		 }
		 if(ECD.Pmode==EXTERNAL_MODE)
		 {
		  ECD.freq_state_cor = Lloop.NCO_Cor_State*(double)ECD.GPS_weight +
									DloopRA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopRB.NCO_Cor_State*(double)ECD.PB_weight;
		 }
		 else
		 {

		  ECD.freq_state_cor = Lloop.NCO_Cor_State*(double)ECD.GPS_weight +
									DloopSA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopSB.NCO_Cor_State*(double)ECD.PB_weight;
		 }
	  }
	  else /*revert to Lloop Operation*/
	  {
		 ECD.freq_state_cor = Lloop.NCO_Cor_State;
		 ECD.GPS_weight = 1.0;
		 ECD.PA_weight =  0.0;
		 ECD.PB_weight =  0.0;
	  }
	} /*end if all inputs are provisioned*/
	else if((smi.Input_Mode&0xFF)==GPS_ONLY)
	{
		ECD.freq_state_cor= Lloop.NCO_Cor_State; /*force GPS ONLY for test*/
		ECD.GPS_weight = 1.0;
		ECD.PA_weight =  0.0;
		ECD.PB_weight =  0.0;
	}
	else if(ECD.Pmode==EXTERNAL_MODE)
	{
		ECD.GPS_weight = 0.0;
		smi.phase_state_cor=0.0;
		if(PA_Flag||PB_Flag)
		{
		 if(ECD.PAC_mvar >0.0 ) ECD.PA_weight  =(PA_Flag)/ECD.PAC_mvar;
		 if(ECD.PBC_mvar >0.0 ) ECD.PB_weight  =(PB_Flag)/ECD.PBC_mvar;
		 normvar = ECD.PA_weight + ECD.PB_weight;
//		 ECD.PA_weight =  ECD.PA_weight/normvar;
//		 ECD.PB_weight =  ECD.PB_weight/normvar;    
//			GPZ June 2010 Override
			ECD.GPS_weight = 1.0;
			ECD.PA_weight =  0.0;
			ECD.PB_weight =  0.0;
		 
		 ECD.freq_state_cor = DloopRA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopRB.NCO_Cor_State*(double)ECD.PB_weight;
		}
		else if(ASMM.ROA_Flag&&!(smi.Input_Mode&0x800)&&!(SReg.ROA_Reg&0x120)) /*ROA Mode*/
		{
		  ECD.freq_state_cor = DloopRA.NCO_Cor_State;
		  ECD.PA_weight =  1.0;
		  ECD.PB_weight =  0.0;
		}
		else if(ASMM.ROB_Flag&&!(smi.Input_Mode&0x1000)&&!(SReg.ROB_Reg&0x120)) /*ROB Mode*/
		{
		  ECD.freq_state_cor = DloopRB.NCO_Cor_State;
		  ECD.PA_weight =  0.0;
		  ECD.PB_weight =  1.0;
		}
		else if(!(smi.Input_Mode&0x800)&&!(SReg.ROA_Reg&0x120)) /*ROA Mode*/
		{
		  ECD.freq_state_cor = DloopRA.NCO_Cor_State;
		  ECD.PA_weight =  1.0;
		  ECD.PB_weight =  0.0;
		}
		else if(!(smi.Input_Mode&0x1000)&&!(SReg.ROB_Reg&0x120)) /*ROB Mode*/
		{
		  ECD.freq_state_cor = DloopRB.NCO_Cor_State;
		  ECD.PA_weight =  0.0;
		  ECD.PB_weight =  1.0;
		}
	} /*end RO only cases*/
	else
	{
		ECD.GPS_weight = 0.0;
		smi.phase_state_cor=0.0;
		if(PA_Flag||PB_Flag)
		{
		 if(ECD.PAC_mvar >0.0 ) ECD.PA_weight  =(PA_Flag)/ECD.PAC_mvar;
		 if(ECD.PBC_mvar >0.0 ) ECD.PB_weight  =(PB_Flag)/ECD.PBC_mvar;
		 normvar = ECD.PA_weight + ECD.PB_weight;
		 ECD.PA_weight =  ECD.PA_weight/normvar;
		 ECD.PB_weight =  ECD.PB_weight/normvar;
		 ECD.freq_state_cor = DloopSA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopSB.NCO_Cor_State*(double)ECD.PB_weight;
		}
		else if(ASMM.SPA_Flag&&!(smi.Input_Mode&0x200)&&!(SReg.SPA_Reg&0x120)) /*SPA Mode*/
		{
		  ECD.freq_state_cor = DloopSA.NCO_Cor_State;
		  ECD.PA_weight =  1.0;
		  ECD.PB_weight =  0.0;
		}
		else if(ASMM.SPB_Flag&&!(smi.Input_Mode&0x400)&&!(SReg.SPB_Reg&0x120)) /*SPB Mode*/
		{
		  ECD.freq_state_cor = DloopSB.NCO_Cor_State;
		  ECD.PA_weight =  0.0;
		  ECD.PB_weight =  1.0;
		}
		else if(!(smi.Input_Mode&0x200)&&!(SReg.SPA_Reg&0x120)) /*SPA Mode*/
		{
		  ECD.freq_state_cor = DloopSA.NCO_Cor_State;
		  ECD.PA_weight =  1.0;
		  ECD.PB_weight =  0.0;
		}
		else if(!(smi.Input_Mode&0x400)&&!(SReg.SPB_Reg&0x120)) /*SPB Mode*/
		{
		  ECD.freq_state_cor = DloopSB.NCO_Cor_State;
		  ECD.PA_weight =  0.0;
		  ECD.PB_weight =  1.0;
		}
	} /*end SPAN ONLY cases*/     
	/******* overide ensembling settings if ensembling is disabled*********/
	if(!ensemble_flag)
	{    
		DloopSA.GPS_Prop_State = 0.0;
		DloopSB.GPS_Prop_State = 0.0;
		DloopRA.GPS_Prop_State = 0.0;
		DloopRB.GPS_Prop_State = 0.0;
	
	     if(smi.BT3_mode!=Holdover) 
	     {
			 ECD.GPS_weight=1.0;
			 ECD.PA_weight =0.0;
			 ECD.PB_weight =0.0;
	     }
	     else
	     {
	     	if(ECD.PA_weight)
			{
			 ECD.PA_weight=1.0; 
			 ECD.PB_weight=0.0; 
			 ECD.GPS_weight=0.0;
			}
			else if(ECD.PB_weight)
			{
			 ECD.PA_weight=0.0; 
			 ECD.PB_weight=1.0; 
			 ECD.GPS_weight=0.0;
			}     
			else 
			{
			 ECD.PA_weight=0.0; 
			 ECD.PB_weight=0.0; 
			 ECD.GPS_weight=1.0;
			}     
		     
	     }
	         
	     if(ECD.Pmode==SPAN_MODE)
	     {
		  ECD.freq_state_cor = Lloop.NCO_Cor_State*(double)ECD.GPS_weight +
									DloopSA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopSB.NCO_Cor_State*(double)ECD.PB_weight;
		 }									
	     else
	     {
		  ECD.freq_state_cor = Lloop.NCO_Cor_State*(double)ECD.GPS_weight +
									DloopRA.NCO_Cor_State*(double)ECD.PA_weight +
									DloopRB.NCO_Cor_State*(double)ECD.PB_weight;
		 }									

	}
	/*update smi copies of weight information and freq state info*/
	smi.GPS_weight = ECD.GPS_weight;
	smi.PA_weight =  ECD.PA_weight;
	smi.PB_weight =  ECD.PB_weight;
	smi.freq_state_cor =ECD.freq_state_cor;
	/******* update local NCO 32 bit control for BT3 application *****/
/*	ECD.freq_state_cor = Lloop.NCO_Cor_State;*/
/*	ECD.freq_state_cor =DloopSA.NCO_Cor_State;*/
	if(!smi.R0B_Select) /*if MASTER_B "0" then diagnostic loop operation*/
	{
/*	NCO_Diagnostic();*/
	}
	NCO_Convert(ECD.freq_state_cor+smi.phase_state_cor+smi.temp_state_cor);
//	NCO_Control();
	/******* generate 1 second message from NCO update task to Estimation ********/
	pncoupestmsg=&(MSG_NCOUP_EST_A);
	pncoupestmsg->freq_state_cor=ECD.freq_state_cor;
	pncoupestmsg->Ready=TRUE;
	/*************end message generation *********************/
}
/******************************************************************************
Function call to set ensemble flag
default is on....
******************************************************************************/
void set_ensemble_mode(unsigned int emode)
{      
	if(emode==0) ensemble_flag=0; 
	else ensemble_flag=1;
}  
/******************************************************************************
Function call to set dither flag
default is on....
******************************************************************************/
void set_efc_mode(unsigned int emode)
{      
	if(emode==0) dither_flag=1; 
	else 
	{
		dither_flag=0; 
	    Dither=emode;
	}
} 

/******************************************************************
* Module for converting NCO fractional frequency correction
* to the NCO 32 bit base word NCO_Low and the dither word
* NCO_Dither. The Dither word is an 8 bit value that is
* accumulated in the Dither_Bit Accumulator. When the accumulator
* overflows NCO_High=NCO_Low+1 is sent to the hardware NCO
* other wise NCO_Low is sent.
* For CS2000 applications the nominal NCO update rate is 8ms
********************************************************************/
void NCO_Convert( double nco_in)
{
	 /* Add cumulative error control to prevent freq bias*/  
     char lcbuf1[120];
	 double fract_error;
//	 double NCO_Cor,NCO_Frac;
	 unsigned long NCO_Cor_Int;
	 if(NCO_Slew)
	 {
		 NCO_Slew--;
		 NCO_Smooth=nco_in;
	 }
	 else
	 {
		 NCO_Smooth=0.8*NCO_Smooth+0.2*nco_in;
	 }      
	 
 	 
//	 NCO_Cor = (NCO_Smooth)*LTBI.nco_convert+LTBI.dither_bias+(double)(LTBI.nco_range); /*ensure a positive
//												 number with enough digits of precision*/
	 NCO_Cor = (NCO_Smooth)*MAX_VAR_CONVERSION+(double)(VAR_RANGE); /*ensure a positive
												 number with enough digits of precision*/
	 NCO_Cor_Int = (unsigned long) NCO_Cor; 
	 

//    sprintf((char *)lcbuf1,"ECD_freq_state:%.3e  ",ECD.freq_state_cor );
//	DebugPrint(lcbuf1, 8);
//   sprintf((char *)lcbuf1,"smi_freq_state:%.3e  ",smi.freq_state_cor );
//	DebugPrint(lcbuf1, 8);
//   sprintf((char *)lcbuf1,"smi_phase_state:%.3e  ",smi.phase_state_cor );
//	DebugPrint(lcbuf1, 8);
//  sprintf((char *)lcbuf1,"smi_temp_state:%.3e  ",smi.temp_state_cor );
//	DebugPrint(lcbuf1, 8);
//   sprintf((char *)lcbuf1,"NCO_Smooth:%.3e  ",NCO_Smooth );
//	DebugPrint(lcbuf1, 8);
//    sprintf((char *)lcbuf1,"NCO_Cor_Int:%ld  ",NCO_Cor_Int );
//	DebugPrint(lcbuf1, 8);
//    sprintf((char *)lcbuf1,"NCO_Cor:%.3e  ",NCO_Cor );
//	DebugPrint(lcbuf1, 8);
 	 
	 smi.NCO_Low     = VAR_CENTER-VAR_RANGE+NCO_Cor_Int;
	 NCO_Frac=( NCO_Cor - (double)(NCO_Cor_Int));  
	    
    sprintf((char *)lcbuf1,"NCO_Frac:%.3e\r\n",NCO_Frac );
	DebugPrint(lcbuf1, 8);
  	 
	 
	 
	 
	 
	 if(NCO_Frac<(double)(0.75))
	 {
	  	NCO_Frac = NCO_Frac + 1.0;
	  	smi.NCO_Low --;
	 
	 }  
 	 smi.NCO_High    = smi.NCO_Low+1;    
	 if(dither_flag)
	 {    
//	 	 NCO_Frac=1.66;        
	 	 fract_error= (double)((40.0/3.0))*NCO_Frac; 
//	 	 if(fract_error>24.0) fract_error=23.9;
//	 	 else if(fract_error<0.0)fract_error=0.1;    
//		 NCO_Frac=fract_error;	
		 Dither  = (unsigned short)((int)(24.0-fract_error));  

//		 smi.NCO_Dither_Error+=(40.0*NCO_Frac -(double)(Dither));
//		 if(smi.NCO_Dither_Error>1.0)
//		 {
//			Dither++;
//			smi.NCO_Dither_Error-=1.0;
//		 }
//		 if(Dither>=40.0)
//		 {
//			smi.NCO_Low++;
//			smi.NCO_High++;
//			Dither=0x00;
//	    }
		 smi.NCO_Dither= Dither;    
	 }
	 else
	 {
	   	 smi.NCO_High = smi.NCO_Low;      
  		 smi.NCO_Dither= Dither;    
     }	 
}  
/********************************************************************* 
 This function updates the three parameters controlling the 40Hz update of the
 varactor control.
*********************************************************************/     
void get_var_control(unsigned short *pHigh,unsigned short *pLow,unsigned short *pDither)
{        
	
	*pHigh= (unsigned short)( smi.NCO_High); 
	*pLow= (unsigned short)( smi.NCO_Low); 
	*pDither= (unsigned short)( smi.NCO_Dither); 	
}	

/*****************************************************************
*   Set the NCO 16-bit high, 16-bit low  and 8-bit dither bytes.
*   Read and verify the values set in the NCO
*   Status Code
*   Lsb: Handshake bit with derive, set to one by this routine must
*        be cleared by driver after nco update is complete
*   Upper nibble status field zero means no error
*   bit 4 Dither mismatch
*   bit 5 Low word mismatch
*   bit 6 high word mismatch
*   bit 7 Failed update
*******************************************************************/
void NCO_Control()
{
	 unsigned char byte[4];
	 unsigned char  msb;
	 unsigned short Status;
	 unsigned char  Retry;
	 Status=0;
	 Retry=3;
//	Status=PutFpgaNcoData(smi.NCO_Low,smi.NCO_High,smi.NCO_Dither);
	 if(smi.NCO_Status&0x1) /*last update failed*/
	 {
		 Status=0x80; /*failed nco_update*/
	 }
//	 while(Retry)
//	 {
//		Status=Status&0x80;
//		byte[0] = (unsigned char)smi.NCO_Low;      /* lsb low   */
//		#if !PCSIM
//		NCO_LOW_ADR0 = byte[0];
//		#endif
//		msb  = (unsigned char)(smi.NCO_Low>>8);
//		byte[1] = msb;
//		#if !PCSIM
//		NCO_LOW_ADR1 = byte[1];                   /* lsb high    */
//		#endif
//		byte[2] = (unsigned char)smi.NCO_High;
//		#if !PCSIM
//		NCO_HIGH_ADR0 = byte[2];                  /* msb low   */
//		#endif
//		msb = (unsigned char)(smi.NCO_High>>8);
//		byte[3] = msb;
//		#if !PCSIM
//		NCO_HIGH_ADR1  = byte[3];                 /* msb high   */
//		#endif
//		#if !PCSIM
//		NCO_DITHER_ADR = (unsigned char)(smi.NCO_Dither&0xFF);    /* dither     */
//		if( (byte[0]!= (unsigned char)NCO_LOW_ADR0)||(byte[1]!= (unsigned char)NCO_LOW_ADR1))Status=Status|0x20;
//		if( (byte[2]!= (unsigned char)NCO_HIGH_ADR0)||(byte[3]!= (unsigned char)NCO_HIGH_ADR1))Status=Status|0x40;
//		if((smi.NCO_Dither&0xFF)!=(NCO_DITHER_ADR&0xFF)) Status=Status|0x10;
//		#endif
//		if(!(Status&0x70))
//		{
//		  Status=Status|0x1;
//		  Retry=0;
//		}
//		else Retry--;
//	 }
	 smi.NCO_Status=Status;

}
/*******************************************************
 Test routine should be called once every second with
 all other BT3_1sec tasks disabled in ttabs.asm
 Takes smi.freq_state_cor and updates actual NCO
*****************************************************/
void NCO_Test()
{
	  NCO_Convert( smi.freq_state_cor);
	  NCO_Control();
}
/****************************************************************************
The following task should be called after all other 1sec level tasks to
generate output messages to slower tier tasks
****************************************************************************/
void Msg_Gen_1sec()
{

	/******* generate 1 minute message for Holdover   ******/
	msg_count1sec++;
	if(msg_count1sec==60) /*generate a 1 minute message at index 60 and 0*/
	{
	 pholdmsg=&(MSG_FLL_HOLDOVER_A);
	 ptchmsg=&(MSG_FLL_TCH_A);
	 psrtchmsg= &(MSG_SR_TCH_A);
	 pcalmsg= &(MSG_FLL_CAL_A);
	}
	else if(msg_count1sec==120)
	{
	  pholdmsg=&(MSG_FLL_HOLDOVER_B);
	  ptchmsg=&(MSG_FLL_TCH_B);
	  psrtchmsg= &(MSG_SR_TCH_B);
	  pcalmsg= &(MSG_FLL_CAL_B);
	  msg_count1sec=0;
	}
	if(msg_count1sec==60||msg_count1sec==0)
	{
	  pholdmsg->Lloop_Freq_Err_Sum=Lloop.Freq_Err_Sum;
	  pholdmsg->SPA_GPS_Prop_State=DloopSA.GPS_Prop_State;
	  pholdmsg->SPB_GPS_Prop_State=DloopSB.GPS_Prop_State;
	  pholdmsg->ROA_GPS_Prop_State=DloopRA.GPS_Prop_State;
	  pholdmsg->ROB_GPS_Prop_State=DloopRB.GPS_Prop_State;
	  pholdmsg->Lloop_Freq_Sum_Cnt=Lloop.Freq_Sum_Cnt;
	  pholdmsg->SPA_Recal= DloopSA.Recalibrate;
	  pholdmsg->SPB_Recal= DloopSB.Recalibrate; 
	  pholdmsg->ROA_Recal= DloopRA.Recalibrate;
	  pholdmsg->ROB_Recal= DloopRB.Recalibrate;
	  pholdmsg->SPA_Mreg=SReg.SPA_Mreg;
	  pholdmsg->SPB_Mreg=SReg.SPB_Mreg;
	  pholdmsg->ROA_Mreg=SReg.ROA_Mreg;
	  pholdmsg->ROB_Mreg=SReg.ROB_Mreg;
	  Lloop.Freq_Err_Sum=0.0;
	  Lloop.Freq_Sum_Cnt=0;
	  #if REMOTE
	  pholdmsg->Rloop_Freq_Err_Sum=Rloop.Freq_Err_Sum;
	  pholdmsg->Rloop_Freq_Sum_Cnt=Rloop.Freq_Sum_Cnt;
	  Rloop.Freq_Err_Sum=0.0;
	  Rloop.Freq_Sum_Cnt=0;
	  #endif
	  pholdmsg->Ready=TRUE;
	  ptchmsg->XGPS_1Min=IFOM.XGPS; ptchmsg->XGPS_cnt=IFOM.XGPS_cnt;
	  ptchmsg->XSPA_1Min=IFOM.XSPA; ptchmsg->XSPA_cnt=IFOM.XSPA_cnt;
	  ptchmsg->XSPB_1Min=IFOM.XSPB; ptchmsg->XSPB_cnt=IFOM.XSPB_cnt;
	  ptchmsg->XROA_1Min=IFOM.XROA; ptchmsg->XROA_cnt=IFOM.XROA_cnt;
	  ptchmsg->XROB_1Min=IFOM.XROB; ptchmsg->XROB_cnt=IFOM.XROB_cnt;
	  ptchmsg->Pmode=ECD.Pmode;
	  ptchmsg->Ready=TRUE;
	  IFOM.XGPS_cnt=0;
	  IFOM.XSPA_cnt=0,IFOM.XSPB_cnt=0,IFOM.XROA_cnt=0,IFOM.XROB_cnt=0;
	  IFOM.XGPS=0.0;
	  IFOM.XSPA=0.0,IFOM.XSPB=0.0,IFOM.XROA=0.0,IFOM.XROB=0.0;
	  psrtchmsg->SPA_Mreg=SReg.SPA_Mreg; SReg.SPA_Mreg=0;
	  psrtchmsg->SPB_Mreg=SReg.SPB_Mreg; SReg.SPB_Mreg=0;
	  psrtchmsg->ROA_Mreg=SReg.ROA_Mreg; SReg.ROA_Mreg=0;
	  psrtchmsg->ROB_Mreg=SReg.ROB_Mreg; SReg.ROB_Mreg=0;
	  psrtchmsg->GPS_Mreg=SReg.GPS_Mreg; SReg.GPS_Mreg=0;
	  psrtchmsg->Ready=TRUE;
	  pcalmsg->Lloop_NCO=Lloop.NCO_Cor_State;
	  pcalmsg->Lloop_Prop_Tau=Lloop.Prop_Tau;
	  pcalmsg->Lloop_Int_State=Lloop.Int_State;
	  pcalmsg->Ready=TRUE;

	}
	/*************end message generation *********************/
}
/*********************************************************************
 This function updated the outgoing mailboxes to report
 the control loop status information
*********************************************************************/
void Control_Loop_Shell()
{
	extern struct MSG_CAL_FLL MSG_CAL_FLL_A;
	if(MSG_CAL_FLL_A.Ready)
	{
	  factory_freq_FLL_rec=MSG_CAL_FLL_A.Factory_Freq_Cal;
	  MSG_CAL_FLL_A.Ready=FALSE;
	}
	switch(CLS_Count){
	 case SPA_LOOP :
		if(SReg.SPA_Reg&0x120)
		{
		  MSG_CLS[SPA_LOOP].IN_Prop_State=0.0;
		  MSG_CLS[SPA_LOOP].IN_Int_State =0.0;
		  MSG_CLS[SPA_LOOP].IN_Dint_State =0.0;
		  MSG_CLS[SPA_LOOP].IN_GPS_Cal_State=0.0;
		  MSG_CLS[SPA_LOOP].IN_Recalibration_Mode= 0;
		  MSG_CLS[SPA_LOOP].IN_Mode_Shift= 0;
		  MSG_CLS[SPA_LOOP].IN_Weight= 0;
		  MSG_CLS[SPA_LOOP].Ready=TRUE;
		}
		else
		{
		  MSG_CLS[SPA_LOOP].IN_Prop_State=(float)DloopSA.Prim_Prop_State;
		  MSG_CLS[SPA_LOOP].IN_Int_State= (float)(DloopSA.Prim_Int_State-
		  factory_freq_FLL_rec);
		  MSG_CLS[SPA_LOOP].IN_Dint_State=0.0;
		  MSG_CLS[SPA_LOOP].IN_GPS_Cal_State=(float)DloopSA.GPS_Prop_State;
		  MSG_CLS[SPA_LOOP].IN_Recalibration_Mode=  DloopSA.Cal_Mode;
		  MSG_CLS[SPA_LOOP].IN_Mode_Shift= DloopSA.shift_cnt-DloopSA.minute_cnt+2;
		  if(ECD.Pmode==SPAN_MODE)
		  {
			 MSG_CLS[SPA_LOOP].IN_Weight= (unsigned short)(ECD.PA_weight*100); 
			 if(MSG_CLS[SPA_LOOP].IN_Weight==0&&ECD.PA_weight>0.0)
			 {
			 	MSG_CLS[SPA_LOOP].IN_Weight=1;
			 }	
		  }	
		  else MSG_CLS[SPA_LOOP].IN_Weight=0;
		  MSG_CLS[SPA_LOOP].Ready=TRUE;
		}
		break;
	 case SPB_LOOP :
		if(SReg.SPB_Reg&0x120)
		{
		  MSG_CLS[SPB_LOOP].IN_Prop_State=0.0;
		  MSG_CLS[SPB_LOOP].IN_Int_State =0.0;
		  MSG_CLS[SPB_LOOP].IN_Dint_State =0.0;
		  MSG_CLS[SPB_LOOP].IN_GPS_Cal_State=0.0;
		  MSG_CLS[SPB_LOOP].IN_Recalibration_Mode= 0;
		  MSG_CLS[SPB_LOOP].IN_Mode_Shift= 0;
		  MSG_CLS[SPB_LOOP].IN_Weight= 0;
		  MSG_CLS[SPB_LOOP].Ready=TRUE;
		}
		else
		{
		  MSG_CLS[SPB_LOOP].IN_Prop_State=(float)DloopSB.Prim_Prop_State;
		  MSG_CLS[SPB_LOOP].IN_Int_State= (float)(DloopSB.Prim_Int_State-
		  factory_freq_FLL_rec);
		  MSG_CLS[SPB_LOOP].IN_Dint_State=0.0;
		  MSG_CLS[SPB_LOOP].IN_GPS_Cal_State=(float)DloopSB.GPS_Prop_State;
		  MSG_CLS[SPB_LOOP].IN_Recalibration_Mode=  DloopSB.Cal_Mode;
		  MSG_CLS[SPB_LOOP].IN_Mode_Shift= DloopSB.shift_cnt-DloopSB.minute_cnt+2;
		  if(ECD.Pmode==SPAN_MODE)
		  {
			 MSG_CLS[SPB_LOOP].IN_Weight= (unsigned short)(ECD.PB_weight*100); 
			 if(MSG_CLS[SPB_LOOP].IN_Weight==0&&ECD.PB_weight>0.0)
			 {
			 	MSG_CLS[SPB_LOOP].IN_Weight=1;
			 }	
			 
		  }
		  else MSG_CLS[SPB_LOOP].IN_Weight=0;
		  MSG_CLS[SPB_LOOP].Ready=TRUE;
		}
		break; 
	 case ROA_LOOP :
		if(SReg.ROA_Reg&0x120)
		{
		  MSG_CLS[ROA_LOOP].IN_Prop_State=0.0;
		  MSG_CLS[ROA_LOOP].IN_Int_State =0.0;
		  MSG_CLS[ROA_LOOP].IN_Dint_State =0.0;
		  MSG_CLS[ROA_LOOP].IN_GPS_Cal_State=0.0;
		  MSG_CLS[ROA_LOOP].IN_Recalibration_Mode= 0;
		  MSG_CLS[ROA_LOOP].IN_Mode_Shift= 0;
		  MSG_CLS[ROA_LOOP].IN_Weight= 0;
		  MSG_CLS[ROA_LOOP].Ready=TRUE;
		}
		else
		{
			MSG_CLS[ROA_LOOP].IN_Prop_State=(float)DloopRA.Prim_Prop_State;
			MSG_CLS[ROA_LOOP].IN_Int_State= (float)(DloopRA.Prim_Int_State-
			factory_freq_FLL_rec);
			MSG_CLS[ROA_LOOP].IN_Dint_State=0.0;
			MSG_CLS[ROA_LOOP].IN_GPS_Cal_State=(float)DloopRA.GPS_Prop_State;
			MSG_CLS[ROA_LOOP].IN_Recalibration_Mode=  DloopRA.Cal_Mode;
			MSG_CLS[ROA_LOOP].IN_Mode_Shift= DloopRA.shift_cnt-DloopRA.minute_cnt+2;
			if(ECD.Pmode!=SPAN_MODE)
			{
			  MSG_CLS[ROA_LOOP].IN_Weight= (unsigned short)(ECD.PA_weight*100);   
			 if(MSG_CLS[ROA_LOOP].IN_Weight==0&&ECD.PA_weight>0.0)
			 {
			 	MSG_CLS[ROA_LOOP].IN_Weight=1;
			 }	
			}
			else MSG_CLS[ROA_LOOP].IN_Weight=0;
			MSG_CLS[ROA_LOOP].Ready=TRUE;
		}
		break;
	 case ROB_LOOP :
		if(SReg.ROB_Reg&0x120)
		{
		  MSG_CLS[ROB_LOOP].IN_Prop_State=0.0;
		  MSG_CLS[ROB_LOOP].IN_Int_State =0.0;
		  MSG_CLS[ROB_LOOP].IN_Dint_State =0.0;
		  MSG_CLS[ROB_LOOP].IN_GPS_Cal_State=0.0;
		  MSG_CLS[ROB_LOOP].IN_Recalibration_Mode= 0;
		  MSG_CLS[ROB_LOOP].IN_Mode_Shift= 0;
		  MSG_CLS[ROB_LOOP].IN_Weight= 0;
		  MSG_CLS[ROB_LOOP].Ready=TRUE;
		}
		else
		{
		  MSG_CLS[ROB_LOOP].IN_Prop_State=(float)DloopRB.Prim_Prop_State;
		  MSG_CLS[ROB_LOOP].IN_Int_State= (float)(DloopRB.Prim_Int_State-
		  factory_freq_FLL_rec);
		  MSG_CLS[ROB_LOOP].IN_Dint_State=0.0;
		  MSG_CLS[ROB_LOOP].IN_GPS_Cal_State=(float)DloopRB.GPS_Prop_State;
		  MSG_CLS[ROB_LOOP].IN_Recalibration_Mode=  DloopRB.Cal_Mode;
		  /**** GPZ changed to correct bug used RB-RA by mistake*/
		  MSG_CLS[ROB_LOOP].IN_Mode_Shift= DloopRB.shift_cnt-DloopRB.minute_cnt+2;
		  if(ECD.Pmode!=SPAN_MODE)
		  {
			 MSG_CLS[ROB_LOOP].IN_Weight= (unsigned short)(ECD.PB_weight*100); 
			 if(MSG_CLS[ROB_LOOP].IN_Weight==0&&ECD.PB_weight>0.0)
			 {
			 	MSG_CLS[ROB_LOOP].IN_Weight=1;
			 }
			 
		  }
		  else MSG_CLS[ROB_LOOP].IN_Weight=0;
		  MSG_CLS[ROB_LOOP].Ready=TRUE;
		}
		break;		
	 case GPS_LOOP :
		MSG_CLS[GPS_LOOP].IN_Prop_State=(float)(Lloop.Prop_State-
		factory_freq_FLL_rec);
		MSG_CLS[GPS_LOOP].IN_Int_State= (float)(Lloop.Int_State);
		MSG_CLS[GPS_LOOP].IN_Dint_State=0.0;
		MSG_CLS[GPS_LOOP].IN_GPS_Cal_State=0.0;
		MSG_CLS[GPS_LOOP].IN_Recalibration_Mode=  Lloop.Cal_Mode;
		MSG_CLS[GPS_LOOP].IN_Mode_Shift= Lloop.shift_cnt-Lloop.minute_cnt+1;
		MSG_CLS[GPS_LOOP].IN_Weight= (unsigned short)(ECD.GPS_weight*100);
		MSG_CLS[GPS_LOOP].Ready=TRUE;
		break;
	 default : break;
	}
	MSG_CLC.Freq_State_Cor=(float)(smi.freq_state_cor-factory_freq_FLL_rec);
	MSG_CLC.Phase_State_Cor=(float)smi.phase_state_cor;
	MSG_CLC.Temp_State_Cor=(float)smi.temp_state_cor;
	MSG_CLC.Input_Mode=smi.Input_Mode;
	MSG_CLC.Ready=TRUE;
	CLS_Count++;
	if(CLS_Count>=CLOOPS)CLS_Count=0;   
	/* populate bestime engine summary report*/       
	MSG_BER.SPA_Weight= MSG_CLS[SPA_LOOP].IN_Weight;  
	MSG_BER.SPB_Weight= MSG_CLS[SPB_LOOP].IN_Weight;  
	MSG_BER.PRR_Weight= MSG_CLS[ROA_LOOP].IN_Weight;   
	MSG_BER.LO_Prediction=(long)(1.0e9*( HMS.Predict_Err24Hr) +.5);  
	MSG_BER.SPA_Prediction=(long)(1.0e9*( HMSA.Predict_Err24Hr)+.5 );  
	MSG_BER.SPB_Prediction=(long)(1.0e9*( HMSB.Predict_Err24Hr)+.5 );  
	MSG_BER.PRR_Prediction=(long)(1.0e9*( HMRA.Predict_Err24Hr)+.5 ); 
	if(ROB_ASSIGN)
	{
	 if( MSG_CLS[SPB_LOOP].IN_Weight==0)
	 {
	 	MSG_CLS[SPB_LOOP].IN_Weight=  MSG_CLS[ROB_LOOP].IN_Weight;    
	 }  
	}
	else
	{
	
	 if( MSG_CLS[SPA_LOOP].IN_Weight==0)
	 {
	 	  MSG_CLS[SPA_LOOP].IN_Weight=  MSG_CLS[ROB_LOOP].IN_Weight;    
	 }
	
	}
//	MSG_BER.SPANAorB=ROB_ASSIGN; 
	MSG_BER.LO_Weight = 100 - (MSG_BER.SPA_Weight+ MSG_BER.SPB_Weight + MSG_BER.PRR_Weight);
	if( MSG_BER.LO_Weight>100)MSG_BER.LO_Weight=100;   
    if(smi.One_PPS_Mag<MAX_PPS_JAM&&smi.One_PPS_Range==0)  
    {
   		MSG_BER.TFOM=250;/* TODO get actual value*/  
   	}
   	else
   	{
   		MSG_BER.TFOM=9999;/* TODO get actual value*/  
   	}	
	MSG_BER.GPS_mdev=(long)(1.0e12*(sqrt(MSG_TCH_NCOUP_A.GPS_mvar)));
	MSG_BER.PA_mdev= (long)(1.0e12*(sqrt(MSG_TCH_NCOUP_A.PA_mvar)));
	MSG_BER.PB_mdev= (long)(1.0e12*(sqrt(MSG_TCH_NCOUP_A.PB_mvar)));
	MSG_BER.LO_mdev= (long)(1.0e12*(sqrt(MSG_TCH_NCOUP_A.LO_mvar)));	          
}
/*********************************************************************
 This function updated the outgoing mailboxes to report
 the  holdover report information
*********************************************************************/
void Holdover_Report()
{
  struct Holdover_Management *hmp;
  struct MSG_Daily_Holdover_Report *hp;
  struct MSG_Monthly_Holdover_Report *hmop;

  unsigned short hr,i,day;
  long af_base, pf_base;
	hp=&(MSG_DHR);
	hmop=&(MSG_MHR);

	switch(DHR_Count){
	 case SPA_LOOP :
		hmp=&HMSA;
		break;
	 case SPB_LOOP :
		hmp=&HMSB;
		break;
	 case GPS_LOOP :
		hmp=&HMS;
		break;
	 case ROA_LOOP :
		hmp=&HMRA;
		break;
	 case ROB_LOOP :
		hmp=&HMRB;
		break;

	 default : break;
	}
	hr = (*hmp).Cur_Hour;
	hr = DECMT(hr); /*GPZ added to start at actual last valid data*/
	for( i = 0; i<4; i++ )
	{
		af_base = F2L((float)( (*hmp).Daily_Freq_Log[ hr ]) );
		if(DHR_Count==GPS_LOOP)
			(*hp).avg_freq[i].Freq_Base = af_base-F2L(factory_freq_FLL_rec);
		else
			(*hp).avg_freq[i].Freq_Base = af_base;

		pf_base = F2L((float)( (*hmp).Holdover_Base[ hr ]) );
		if(DHR_Count==GPS_LOOP)
			(*hp).pred_freq[i].Freq_Base = pf_base-F2L(factory_freq_FLL_rec);
		else
			(*hp).pred_freq[i].Freq_Base = pf_base;
		hr = DECMT(hr);
		(*hp).avg_freq[i].Freq_Off_4  = (short)(af_base-
											  F2L( (*hmp).Daily_Freq_Log[ hr ] ));
		(*hp).pred_freq[i].Freq_Off_4 = (short)(pf_base-
											  F2L( (*hmp).Holdover_Base[ hr ] ));
		hr = DECMT(hr);
		(*hp).avg_freq[i].Freq_Off_3  = (short)(af_base-
											  F2L( (*hmp).Daily_Freq_Log[ hr ] ));
		(*hp).pred_freq[i].Freq_Off_3 = (short)(pf_base-
											  F2L( (*hmp).Holdover_Base[ hr ] ));
		hr = DECMT(hr);
		(*hp).avg_freq[i].Freq_Off_2  = (short)(af_base-
											  F2L( (*hmp).Daily_Freq_Log[ hr ] ));
		(*hp).pred_freq[i].Freq_Off_2 = (short)(pf_base-
											  F2L( (*hmp).Holdover_Base[ hr ] ));
		hr = DECMT(hr);
		(*hp).avg_freq[i].Freq_Off_1  = (short)(af_base-
											  F2L( (*hmp).Daily_Freq_Log[ hr ] ));
		(*hp).pred_freq[i].Freq_Off_1 = (short)(pf_base-
											  F2L( (*hmp).Holdover_Base[ hr ] ));
		hr = DECMT(hr);
		(*hp).avg_freq[i].Freq_Off_0  = (short)(af_base-
											  F2L( (*hmp).Daily_Freq_Log[ hr ] ));
		(*hp).pred_freq[i].Freq_Off_0 = (short)(pf_base-
											  F2L( (*hmp).Holdover_Base[ hr ] ));
		hr = DECMT(hr);
	} 
	hr = (*hmp).Cur_Hour;
	hr = DECMT(hr); /*GPZ added to start at actual last valid data*/
    (*hp).Freq_Drift_Estimate = F2L((float)( (*hmp).Holdover_Drift[hr]*24.0));
	(*hp).Freq_Pred_Err_24 = F2L((float)( (*hmp).Predict_Err24Hr) );
	(*hp).Channel=DHR_Count;
	(*hp).Ready=TRUE;
	/****** Update Monthly Report Mailboxes */
	day= (*hmp).Cur_Day;
	day = DECMTD(day); /*decrement to actual last data entered*/
	for( i = 0; i<5; i++ )
	{
		af_base = F2L((float)( (*hmp).Monthly_Freq_Log[ day ]) );
		if(DHR_Count==GPS_LOOP)
			(*hmop).Daily_Freq[i].Freq_Base = af_base-F2L(factory_freq_FLL_rec);
		else
			(*hmop).Daily_Freq[i].Freq_Base = af_base;

		pf_base = F2L( (*hmp).Monthly_Pred_Log[day]);
		if(DHR_Count==GPS_LOOP)
			(*hmop).Daily_Pred[i].Freq_Base = pf_base;
		else
			(*hmop).Daily_Pred[i].Freq_Base = pf_base;
		day = DECMTD(day);
		(*hmop).Daily_Freq[i].Freq_Off_4  = (short)(af_base-
											  F2L( (*hmp).Monthly_Freq_Log[ day ] ));
		(*hmop).Daily_Pred[i].Freq_Off_4 = (short)(pf_base-
											  F2L( (*hmp).Monthly_Pred_Log[ day ] ));
		day = DECMTD(day);
		(*hmop).Daily_Freq[i].Freq_Off_3  = (short)(af_base-
											  F2L( (*hmp).Monthly_Freq_Log[ day ] ));
		(*hmop).Daily_Pred[i].Freq_Off_3 = (short)(pf_base-
											  F2L( (*hmp).Monthly_Pred_Log[ day ] ));
		day = DECMTD(day);
		(*hmop).Daily_Freq[i].Freq_Off_2  = (short)(af_base-
											  F2L( (*hmp).Monthly_Freq_Log[ day ] ));
		(*hmop).Daily_Pred[i].Freq_Off_2 = (short)(pf_base-
											  F2L( (*hmp).Monthly_Pred_Log[ day ] ));
		day = DECMTD(day);
		(*hmop).Daily_Freq[i].Freq_Off_1  = (short)(af_base-
											  F2L( (*hmp).Monthly_Freq_Log[ day ] ));
		(*hmop).Daily_Pred[i].Freq_Off_1 = (short)(pf_base-
											  F2L( (*hmp).Monthly_Pred_Log[ day ] ));
		day = DECMTD(day);
		(*hmop).Daily_Freq[i].Freq_Off_0  = (short)(af_base-
											  F2L( (*hmp).Monthly_Freq_Log[ day ] ));
		(*hmop).Daily_Pred[i].Freq_Off_0 = (short)(pf_base-
											  F2L( (*hmp).Monthly_Pred_Log[ day ] ));
		day = DECMTD(day);
	}
	(*hmop).Channel=DHR_Count;
	(*hmop).Ready=TRUE;
	DHR_Count++;
	if(DHR_Count>=CLOOPS)DHR_Count=0;
}
/*********************************************************************
This function is used during the first week of oscillator aging to determine
the early aging curve an OCXO.
This function calculates a least squares fit to the "inverse drift rate"
of the LO hourly frequency data. This is equivalant to fitting the standard
log fit y=A+Blog(1+ct) aging curve. By taking the derivative we obtain the
drift curve fit d=B/(c(1+ct)). Thus the inverse drift rate can be modelled
as a linear process id = mt+b (m=c/B and b=c*c/B).


*************************************************************************/
void Holdover_Inverse_Drift_Fit(double fin)
{
  double id,denom; /*derived inverse drift sample*/
//  if(HLSF.wa==0.0)  /* initialize algorithm if start */
//  {
//	  HLSF.wa=1.0;
//	  HLSF.wb=0.0;
//	  HLSF.dsmooth=0.0;
//	  HLSF.mfit=0.0;
//	  HLSF.bfit=0.0;
//	  HLSF.sx=0.0;
//	  HLSF.sy=0.0;
//	  HLSF.sxy=0.0;
//	  HLSF.sx2=0.0;
//	  HLSF.finp=fin;
//	  return;
//  }   
DebugPrint("calculate delta freq\r\n", 3);  
  id= fin-HLSF.finp;
  HLSF.finp=fin; 
	if(id > MAX_DRIFT_RATE)
	{
		id= MAX_DRIFT_RATE;
	}
	else if(id < -MAX_DRIFT_RATE)
	{
		id= -MAX_DRIFT_RATE;
	}
  
  
  /**** progressively increase filtering to counteract high noise gain as
		  drift approaches zero. *******/
  HLSF.dsmooth=HLSF.wa*id+HLSF.wb*HLSF.dsmooth;
  /*reduce filter bandwidth to 24hr moving average equivalant*/
  if(HLSF.wa>.0833)
  {    
	  DebugPrint("update wa,wb\r\n", 3);  

	 HLSF.wa*=.96;
	 HLSF.wb=1.0-HLSF.wa;
  }
  /* clip hyperbolic infinite gain for drift less than 1e-12 per hr */
  if(HLSF.dsmooth< 1e-12)
  { 
	DebugPrint("clip id level\r\n", 3);  
  
	 if (HLSF.dsmooth> 0.0) id = 1e-12;
	 else if(HLSF.dsmooth > -1e-12) id=-1e12;
	 else id = HLSF.dsmooth;
  }
  else id = HLSF.dsmooth;
  id=1.0/id;
  /* update LSF stats */
  HLSF.N++;
  HLSF.sx+=HLSF.N;
  HLSF.sy+=id;
  HLSF.sxy+= HLSF.N*id;
  HLSF.sx2+= HLSF.N*HLSF.N;
  /* update fit coefficients*/
  denom=HLSF.N*HLSF.sx2-HLSF.sx*HLSF.sx;
  if(denom!=0.0)
  {
	 HLSF.mfit= ((HLSF.N*HLSF.sxy)- HLSF.sx*HLSF.sy)/denom;
	 HLSF.bfit= ((HLSF.sy*HLSF.sx2)-(HLSF.sx*HLSF.sxy))/denom;
  }
}
/********************************************************************* 
 This function returns the state of the smart clock algorithm
 The following states are possible
 SMART_OFF no smart clock currently available
 SMART_DEGRADED smart clock available but holdover limited to less than max
 SMART_ON smart clock available at maximum specified duration
*********************************************************************/     
int get_prr_smartclk()
{        
	double scale;  
//	Card_State card_state = Get_card_state();  
	if(HMS.Nsum_Hour<8|HMS.Restart) smart_prr_state=SMART_OFF;
	else if(HMS.Nsum_Hour<16)
	{         
		scale=3.0*SMART_WIDE/(18.0-HMS.Nsum_Hour);
	 	if(HMS.Predict_Err24Hr<scale) smart_prr_state=SMART_DEGRADE;
	 	else smart_prr_state=SMART_OFF;
	}
	else
	{
	 	if(HMS.Predict_Err24Hr<SMART_NARROW) smart_prr_state=SMART_ON;
	    else if(HMS.Predict_Err24Hr<SMART_WIDE) smart_prr_state=SMART_DEGRADE;
        else smart_prr_state=SMART_OFF;
	}
	return(smart_prr_state); //BUG BUG there was no return statement!!!
}       

/**** enumerate states for ensemble state table***/                               
//enum 
//{
// EQ1 = 0, // ensemble quality 1
// EQ2 = 1, // ensemble quality 2
// EQA = 2, // ensemble quality Alarm
// EPH = 3, // ensemble priority high
// EPM = 4, // ensemble priority medium
// EPL = 5  // ensemble priority low
//};  
/*************************************************************
 Call to configure besttime engine ensembling
 as a function of SPAN A, SPANB and PRR current states
 state inputs are Quality Level, Priority, Alarm  and Enable

***************************************************************/
void bestime_engine_ensemble_control( 
unsigned short SPAQ, 
unsigned short SPAP,
unsigned short SPAA,
unsigned short SPAE, 
unsigned short SPBQ, 
unsigned short SPBP,
unsigned short SPBA,
unsigned short SPBE,
unsigned short PRRQ, 
unsigned short PRRP,
unsigned short PRRA,
unsigned short PRRE
)
{  
	unsigned short LECW; /*local ensemble control word*/
	
	/* prepare ensemble state table states*/   
	/*set quality levels*/
	if(SPAA||!SPAE) SPAQ=EQA;
	else if(SPAQ==1) SPAQ=EQ1;
	else if(SPAQ==2) SPAQ=EQ2;
	else SPAQ=EQA;   
	if(SPBA||!SPBE) SPBQ=EQA;
	else if(SPBQ==1) SPBQ=EQ1;
	else if(SPBQ==2) SPBQ=EQ2;
	else SPBQ=EQA;  
	if(PRRA||!PRRE) PRRQ=EQA;
	else if(PRRQ==1) PRRQ=EQ1;
	else if(PRRQ==2) PRRQ=EQ2;
	else PRRQ=EQA; 
	/* set priority levels*/  
	/* all equal priorities*/
	if(SPAP==SPBP&&SPAP==PRRP) SPAP=SPBP=PRRP= EPH;
	/* all different priorities*/
	else if(SPAP!=SPBP&&SPBP!=PRRP&&SPAP!=PRRP)
	{
		if(SPAP<SPBP)   //A>B
		{
		    if(SPBP<PRRP)
		    {
		       SPAP=EPH; SPBP=EPM; PRRP=EPL;
		    }
		    else if(SPAP<PRRP)
		    {  
		       SPAP=EPH; SPBP=EPL; PRRP=EPM;
		    }                 
		    else
		    {
		       SPAP=EPM; SPBP=EPL; PRRP=EPH;
		    }
		}
		else  //B>A
		{
		    if(SPAP<PRRP)
		    {
		       SPAP=EPM; SPBP=EPH; PRRP=EPL;
		    }
		    else if(SPBP<PRRP)
		    {  
		       SPAP=EPL; SPBP=EPH; PRRP=EPM;
		    }                 
		    else
		    {
		       SPAP=EPL; SPBP=EPM; PRRP=EPH;
		    }
		
		}	 
	} // end all different logic
	// A and B equal
	else if(SPAP==SPBP&&SPAP!=PRRP)
	{
	 	if(SPAP<PRRP)
	 	{
		       SPAP=EPH; SPBP=EPH; PRRP=EPM;	 	
	 	} 
	 	else    
	 	{
		       SPAP=EPM; SPBP=EPM; PRRP=EPH;	 	
	 	} 
	}// end A and B equal	
	// B and PRR equal
	else if(SPBP==PRRP&&SPBP!=SPAP)
	{
	 	if(SPBP<SPAP)
	 	{
		       SPAP=EPM; SPBP=EPH; PRRP=EPH;	 	
	 	} 
	 	else    
	 	{
		       SPAP=EPH; SPBP=EPM; PRRP=EPM;	 	
	 	} 
	}// end B and PRR equal	 
	/****** END TABLE STATE PREPARATION*********/
	/****** START SELECTION TABLE LOGIC ********/
	/********* PRR SELECTION CASES *************/
	if(SPAQ!=EQ1&&SPBQ!=EQ1&&PRRQ==EQ1) LECW=0x1600;  
	else if(SPAQ==EQ1&&SPBQ!=EQ1&&PRRQ==EQ1&&SPAP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ==EQ1&&SPBQ==EQ1&&PRRQ==EQ1&&SPAP!=EPH&&SPBP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ!=EQ1&&SPBQ==EQ1&&PRRQ==EQ1&&SPBP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ==EQ1&&SPBQ!=EQ1&&PRRQ==EQ1&&SPAP==EPL&&PRRP==EPM) LECW=0x1600; 
	else if(SPAQ==EQ1&&SPBQ==EQ1&&PRRQ==EQ1&&SPAP==EPL&&SPBP==EPL&&PRRP==EPM) LECW=0x1600; 
	else if(SPAQ!=EQ1&&SPBQ==EQ1&&PRRQ==EQ1&&SPBP==EPL&&PRRP==EPM) LECW=0x1600; 
	else if(SPAQ==EQA&&SPBQ==EQA&&PRRQ==EQ2) LECW=0x1600; 
	else if(SPAQ==EQ2&&SPBQ==EQA&&PRRQ==EQ2&&SPAP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ==EQ2&&SPBQ==EQ2&&PRRQ==EQ2&&SPAP!=EPH&&SPBP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ==EQA&&SPBQ==EQ2&&PRRQ==EQ2&&SPBP!=EPH&&PRRP==EPH) LECW=0x1600; 
	else if(SPAQ==EQ2&&SPBQ==EQA&&PRRQ==EQ2&&SPAP==EPL&&PRRP==EPM) LECW=0x1600; 
	else if(SPAQ==EQ2&&SPBQ==EQ2&&PRRQ==EQ2&&SPAP==EPL&&SPBP==EPL&&PRRP==EPM) LECW=0x1600; 
	else if(SPAQ==EQA&&SPBQ==EQ2&&PRRQ==EQ2&&SPBP==EPL&&PRRP==EPM) LECW=0x1600; 
	/********* SPAN A SELECTION CASES *************/
	else if(PRRQ!=EQ1&&SPBQ!=EQ1&&SPAQ==EQ1) LECW=0x1C00;  
	else if(PRRQ==EQ1&&SPBQ!=EQ1&&SPAQ==EQ1&&PRRP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ==EQ1&&SPBQ==EQ1&&SPAQ==EQ1&&PRRP!=EPH&&SPBP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ!=EQ1&&SPBQ==EQ1&&SPAQ==EQ1&&SPBP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ==EQ1&&SPBQ!=EQ1&&SPAQ==EQ1&&PRRP==EPL&&SPAP==EPM) LECW=0x1C00; 
	else if(PRRQ==EQ1&&SPBQ==EQ1&&SPAQ==EQ1&&PRRP==EPL&&SPBP==EPL&&SPAP==EPM) LECW=0x1C00; 
	else if(PRRQ!=EQ1&&SPBQ==EQ1&&SPAQ==EQ1&&SPBP==EPL&&SPAP==EPM) LECW=0x1C00; 
	else if(PRRQ==EQA&&SPBQ==EQA&&SPAQ==EQ2) LECW=0x1C00; 
	else if(PRRQ==EQ2&&SPBQ==EQA&&SPAQ==EQ2&&PRRP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ==EQ2&&SPBQ==EQ2&&SPAQ==EQ2&&PRRP!=EPH&&SPBP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ==EQA&&SPBQ==EQ2&&SPAQ==EQ2&&PRRP!=EPH&&SPAP==EPH) LECW=0x1C00; 
	else if(PRRQ==EQ2&&SPBQ==EQA&&SPAQ==EQ2&&PRRP==EPL&&SPAP==EPM) LECW=0x1C00; 
	else if(PRRQ==EQ2&&SPBQ==EQ2&&SPAQ==EQ2&&PRRP==EPL&&SPBP==EPL&&SPAP==EPM) LECW=0x1C00; 
	else if(PRRQ==EQA&&SPBQ==EQ2&&SPAQ==EQ2&&SPBP==EPL&&SPAP==EPM) LECW=0x1C00; 
	/********* SPAN B SELECTION CASES *************/
	else if(SPAQ!=EQ1&&PRRQ!=EQ1&&SPBQ==EQ1) LECW=0x1A00;  
	else if(SPAQ==EQ1&&PRRQ!=EQ1&&SPBQ==EQ1&&SPAP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ==EQ1&&PRRQ==EQ1&&SPBQ==EQ1&&SPAP!=EPH&&PRRP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ!=EQ1&&PRRQ==EQ1&&SPBQ==EQ1&&PRRP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ==EQ1&&PRRQ!=EQ1&&SPBQ==EQ1&&SPAP==EPL&&SPBP==EPM) LECW=0x1A00; 
	else if(SPAQ==EQ1&&PRRQ==EQ1&&SPBQ==EQ1&&SPAP==EPL&&PRRP==EPL&&SPBP==EPM) LECW=0x1A00; 
	else if(SPAQ!=EQ1&&PRRQ==EQ1&&SPBQ==EQ1&&PRRP==EPL&&SPBP==EPM) LECW=0x1A00; 
	else if(SPAQ==EQA&&PRRQ==EQA&&SPBQ==EQ2) LECW=0x1600; 
	else if(SPAQ==EQ2&&PRRQ==EQA&&SPBQ==EQ2&&SPAP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ==EQ2&&PRRQ==EQ2&&SPBQ==EQ2&&SPAP!=EPH&&PRRP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ==EQA&&PRRQ==EQ2&&SPBQ==EQ2&&PRRP!=EPH&&SPBP==EPH) LECW=0x1A00; 
	else if(SPAQ==EQ2&&PRRQ==EQA&&SPBQ==EQ2&&SPAP==EPL&&SPBP==EPM) LECW=0x1A00; 
	else if(SPAQ==EQ2&&PRRQ==EQ2&&SPBQ==EQ2&&SPAP==EPL&&PRRP==EPL&&SPBP==EPM) LECW=0x1A00; 
	else if(SPAQ==EQA&&PRRQ==EQ2&&SPBQ==EQ2&&PRRP==EPL&&SPBP==EPM) LECW=0x1A00; 	
	/******* dual ensembling cases*******/    
	/***** select PRR and SPAN A (if SPAN B is higher than we already caught above )****/
	else if(SPAQ==EQ1&&PRRQ==EQ1&&SPAP==EPH&&PRRP==EPH)
	{
		 LECW=0x0600; ROB_ASSIGN=0;
	}	 
	else if(SPAQ==EQ1&&PRRQ==EQ1&&SPAP==EPM&&PRRP==EPM)  
	{
		 LECW=0x0600; ROB_ASSIGN=0;
	}	 
	else if(SPAQ==EQ2&&PRRQ==EQ2&&SPAP==EPH&&PRRP==EPH) 
	{
		 LECW=0x0600; ROB_ASSIGN=0;
	}	 
	else if(SPAQ==EQ2&&PRRQ==EQ2&&SPAP==EPM&&PRRP==EPM)    
	{
		 LECW=0x0600; ROB_ASSIGN=0;
	}	 
	
	/***** select PRR and SPAN B ****/  
	else if(SPBQ==EQ1&&PRRQ==EQ1&&SPBP==EPH&&PRRP==EPH)
	{
		 LECW=0x0600; ROB_ASSIGN=1;
	}	 
	else if(SPBQ==EQ1&&PRRQ==EQ1&&SPBP==EPM&&PRRP==EPM)  
	{
		 LECW=0x0600; ROB_ASSIGN=1;
	}	 
	else if(SPBQ==EQ2&&PRRQ==EQ2&&SPBP==EPH&&PRRP==EPH) 
	{
		 LECW=0x0600; ROB_ASSIGN=1;
	}	 
	else if(SPBQ==EQ2&&PRRQ==EQ2&&SPBP==EPM&&PRRP==EPM)    
	{
		 LECW=0x0600; ROB_ASSIGN=1;
	}	 
	/***** select SPAN A and SPAN B ****/  
	else if(SPAQ==EQ1&&SPBQ==EQ1&&SPAP==EPH&&SPBP==EPH)
	{
		 LECW=0x1800; 
	}	 
	else if(SPAQ==EQ1&&SPBQ==EQ1&&SPAP==EPM&&SPBP==EPM)
	{
		 LECW=0x1800; 
	}	 
	else if(SPAQ==EQ2&&SPBQ==EQ2&&SPAP==EPH&&SPBP==EPH)
	{
		 LECW=0x1800; 
	}	 
	else if(SPAQ==EQ2&&SPBQ==EQ2&&SPAP==EPM&&SPBP==EPM)
	{
		 LECW=0x1800; 
	}
	else LECW=0x1E00; /*gps only case*/	
	/****** report backup configuration ****/   
	if(LECW==0x1600) 
	{
	 	MSG_BER.Backup_Reg=0x04;
	}  
	else if(LECW==0x1C00) 
	{
	 	MSG_BER.Backup_Reg=0x02;
	}  
	else if(LECW==0x1A00) 
	{
	 	MSG_BER.Backup_Reg=0x01;
	}  
	else if(LECW==0x0600) 
	{  
		if(ROB_ASSIGN) MSG_BER.Backup_Reg=0x06;
		else MSG_BER.Backup_Reg=0x05;
	}   
	else if(LECW==0x1800) 
	{  
		MSG_BER.Backup_Reg=0x03;
	} 
	else  MSG_BER.Backup_Reg=0x00; 
	
	
	      	
	PutBt3EnsembleControlWord( LECW );	
}


