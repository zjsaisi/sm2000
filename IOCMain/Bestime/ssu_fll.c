/***************************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/14 00:07:58PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_fll.c_v  $
$Modtime:   25 Feb 1999 11:31:10  $
$Revision: 1.6 $


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

$Log: ssu_fll.c  $
Revision 1.6 2010/10/14 00:07:58PDT Jining Yang (jyang) 
Merged from 1.5.1.1.
Revision 1.5.1.1 2010/08/04 23:24:20PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.5 2009/06/01 09:12:02PDT Zheng Miao (zmiao) 
Change pullin range according OSC type.
Revision 1.4 2008/05/27 08:40:10PDT gzampetti 
Changed synth LSB from 2.7e-11 to 1.7e-12
Revision 1.3 2008/04/01 07:11:45PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.2 2008/03/25 15:02:55PDT gzampetti 
ff
Revision 1.1 2007/12/06 11:41:07PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.5 2006/07/07 14:17:45PDT gzampet 
add synthesizer control
Revision 1.4 2006/05/09 13:41:19PDT gzampet 
modifications to improve standby to active performance
Revision 1.2 2006/04/19 15:49:09PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.1 2005/09/01 14:55:12PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.11 2005/01/05 14:34:18PST gzampet 

Revision 1.10  2004/12/16 16:20:39Z  gzampet
Revision 1.9  2004/12/09 21:56:52Z  gzampet
Revision 1.8  2004/09/03 16:52:16Z  gzampet
Revision 1.7  2004/08/24 21:47:20Z  gzampet
Revision 1.6  2004/07/09 18:29:42Z  gzampet
Revision 1.5  2004/07/01 17:44:49Z  gzampet
Revision 1.4  2004/07/01 13:12:28Z  gzampet
Revision 1.3  2004/06/24 16:41:02Z  gzampet
Updated Smart Holdover to included BesTime NCO Correction when in PRR
mode
Revision 1.2  2004/06/23 15:16:03Z  gzampet
Baseline GPS BesTime Version
Revision 1.1  2004/06/21 16:22:11Z  gzampet
Initial revision
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
When updating the SSUloop (RTHC only loop) modify condition to reset hourly holdover averages to always perform the reset if a frequency step is detected
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
Reduce working holdover channels i loop from 3 to one SSUloop
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
 
 GPZ June 2004 Created new source file ssu_fll.c to support only
 SSU "smart holdover specific featured" Stripped out
 unused portions of legacy file

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
#include "BT3Support.h"

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


#if !PCSIM
double sqrt(double);
#endif
/************ end messaging related data***************/
extern struct Shared_Memory_Interface smi;
extern struct LO_TYPE_BUILD_INFO LTBI;    
extern double ken_MPLL_freq ;//open loop RTHC test data
// extern void DebugPrint(const char *str);
extern struct Calibration_Data  Cal_Data; 
extern struct MSG_RCW_Command  MSG_RCW_C;
/******* 1 second thread frequency input data *****/


struct Single_Input_FLL_Control_System SSUloop,*ssuloop;

struct Holdover_Management HMSSU,*phmssu;
struct Holdover_LSF  SSU_HLSF;
double ssu_test_freq;
unsigned short ssu_test_cnt;
unsigned short ssu_test_seq;  
int smart_state=SMART_OFF;    
/* local function templates*/   
void init_ssu(void);
void SSUFLL_Update(void);
void Holdover_SSU_Update(void);
void SSU_Recalibrate(void);
void SSU_Holdover_Drift_Fit(double);
int get_smartclk(void);   
long int get_smart_freq(void);  
unsigned long Get_micro_icw(void);
void Holdover_Print(void);
void SSU_Loop_Print(void); 
void refresh_smart_icw(void);    
 
void	BT3_span_est(void);  
void 	BT3_mpll_est(void);
void	Status_Report(void);     /* fills out status data into smi, SReg, and MSG_ASR structures */
void	BT3_1Sec_Tasks(void);    /* Process GPS 1 PPS estimate */
void	BT3FLL_Update(void);     /* span lines, ROs and GPS FLLs */
void	SSUFLL_Update(void);     /* span lines, ROs and GPS FLLs */  
void	NCO_Update(void);       /*GPZ note do update before refresh to ensure we synchonize measurement and control*/
void	PCOMP_Update_1sec(void); 
void	Msg_Gen_1sec(void);      /* generate messages to slower tasks */  
void	ShellMain(void);                              
void	Alarm_Report_Loop_Print(void); //Debug test only    
void	Engine_Report_Loop_Print(void); //Debug test only
void	three_corner_decomp(void);	
void	Holdover_Update(void);
void	Holdover_SSU_Update(void);    
void	Holdover_Print(void);
void	Cal_Data_Update(void);  
void	SSU_Recalibrate(void);
void	Init_Tasks(void);
void	Init_Shared_Mem_Int(void);
void	BT3_MSG_Init(void);
void	ShellInit(void);
void	init_cal(void);
void	init_ssu(void); /*new task to intialize SSU specific smart clock features*/



//#if(TARGET==CLPR)
//unsigned short PutFpgaNcoData(unsigned short,unsigned short,unsigned short);
//#endif
/********************************************************************
  This module implements the update of each of the Frequency Locked Loop
  (FLL)s supported in the BT3 core engine. The loops are structured based
  on the Multiple Frequency Locked Loop Patented approach to optimizing
  frequency lock to multiple input. signals.
*******************************************************************/
void SSUFLL_Update()
{
	int i,j;
	double Drift;

	/******* Receive 1 minute holdover data ****************/
		SSUloop.LO_Freq_Pred= HMSSU.Osc_Freq_State;
	/*complete receive of 1 minute holdover data */
	/**** Update SSU Only Single Loop *****/
	/*****Update open loop freq error estimates****/    
	if(!Is_OCXO_warm())
	{                                              
		SSUloop.Freq_Err_Est= SSUloop.LO_Freq_Pred; /*use holdover estimate*/
		SSUloop.Ygps_Buf[GPSDELAY-1]=SSUloop.LO_Freq_Pred;
		SSUloop.Ygps_Buf[GPSDELAY-2]=SSUloop.LO_Freq_Pred;
		SSUloop.Ygps_Buf[GPSDELAY-3]=SSUloop.LO_Freq_Pred;
		SSUloop.Ygps_Buf[GPSDELAY-4]=SSUloop.LO_Freq_Pred;
		//force to clean restart all the time
        SSUloop.Freq_Sum_Cnt=0;
		HMSSU.FreqSum=0;
		HMSSU.FreqSumCnt=0;
	}
	else /*good local open loop LO vs reference input measurement */
	{
	 SSUloop.Freq_Err_Est= ken_MPLL_freq;
	 SSUloop.Freq_Err_Sum+= SSUloop.Freq_Err_Est;/*used current data*/   
	 SSUloop.Freq_Sum_Cnt++;
	}
	for(j=1;j<GPSDELAY;j++) SSUloop.Ygps_Buf[j-1]= SSUloop.Ygps_Buf[j];
	SSUloop.Ygps_Buf[GPSDELAY-1]= SSUloop.Freq_Err_Est;
	/**** Update Local Loop  Note TAU of update is 1 second*****/
	SSUloop.Sprop1=((SSUloop.Ygps_Buf[GPSDELAY-4])*SSUloop.Propf1)+(SSUloop.Sprop1*SSUloop.Propf2);
	SSUloop.Sprop2=(SSUloop.Sprop1*SSUloop.Propf1)+(SSUloop.Sprop2*SSUloop.Propf2);
	SSUloop.min_cnt++;
	SSUloop.Prop_State=SSUloop.Sprop2;
	if(SSUloop.min_cnt>59)/*update once per minute*/
	{
	  SSUloop.min_cnt=0;
	  Drift=(SSUloop.Sprop2-SSUloop.Sprop_prev)*SSUloop.Prop_Tau/30.0;
	 if (LTBI.lotype!=RB)
	 {
	 	if(SSUloop.Cal_Mode<3)
	 	{
			/*GPZ zero out integral term during initial acq*/
			/*if(SSUloop.minute_cnt < SSUloop.shift_cnt/2)*/
			{
			 SSUloop.Int_State=0.0;
			 Drift=0;
			}
	 	}
	 }	 
	  SSUloop.Int_State= (Drift*SSUloop.Intf1)+(SSUloop.Int_State*SSUloop.Intf2);
	  SSUloop.Sprop_prev=SSUloop.Sprop2;
	}
  /***** Local NCO Update *********/
	SSUloop.NCO_Cor_State = SSUloop.Prop_State +SSUloop.Int_State;

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
void SSU_Recalibrate()
{
	struct Single_Input_FLL_Control_System *psingle;
	double scale;
	/* local oscillator control loop recalibration*/
	psingle = &SSUloop;        
//	DebugPrint("SSU Recalibration Entered\r\n", 2);  
	if(psingle->Recalibrate&& Is_MPLL_Tracking()&&In_MPLL_Phase_Window()&&Is_OCXO_warm())
	{   
//		DebugPrint("SSU Recalibration Update\r\n", 2);  
	 	if(!psingle->Cal_Mode)/*start of gps local loop recalibration*/
		{
			psingle->Prop_Tau=(LTBI.single_prop_tau)/LTBI.single_accel;
			psingle->Int_Tau=(unsigned int)(MAX_INT_TC);
			psingle->Int_State=0.0;
			psingle->Propf1=20.0/(psingle->Prop_Tau);
			psingle->Propf2=1.0-psingle->Propf1;
			psingle->Intf1=60.0/(psingle->Int_Tau);
			psingle->Intf2=1.0-psingle->Intf1;
			psingle->minute_cnt=0;
			psingle->shift_cnt=((psingle->Prop_Tau+60)/20); /*number of minutes to stay at current
											time constant*/
			psingle->Cal_Mode=1;
	 	}
	 	if(psingle->minute_cnt > psingle->shift_cnt) /*time to shift up*/
	 	{
			if(psingle->Cal_Mode==1)
			{
		  	psingle->Int_Tau=(unsigned int)(LTBI.single_int_tau/LTBI.single_recal);
		  	psingle->Prop_Tau=(LTBI.single_prop_tau)/LTBI.single_recal;
		  	psingle->Int_State=0.0;
			}
			else
			{
				psingle->Prop_Tau *=LTBI.gear_space;
				psingle->Int_Tau*=LTBI.gear_space;
			}
			if(psingle->Prop_Tau >=LTBI.single_prop_tau)
			{
				psingle->Prop_Tau= LTBI.single_prop_tau;
			 	psingle->Int_Tau=LTBI.single_int_tau;
			 	psingle->Recalibrate=FALSE;
			}
			psingle->Propf1=1.0/(psingle->Prop_Tau);
			psingle->Propf2=1.0-psingle->Propf1;
			psingle->Intf1=60.0/(psingle->Int_Tau);
			psingle->Intf2=1.0-psingle->Intf1;

			if(LTBI.lotype!=RB)
			{
			 /**** GPZ reworked to be based on Gear_Space and Cal_mode****/
		 		scale= (LTBI.gear_space * (psingle->Cal_Mode+1)/7.0);
		 		psingle->Int_State*=scale;
		 	/****** reuse scale to correct for increased delay in prop filt*/
		 		scale = psingle->Int_State*(1.0-1.0/LTBI.gear_space);
		 		psingle->Prop_State-=scale;
		 		psingle->Sprop2=psingle->Prop_State;
		 		psingle->Sprop1-=(2.0*scale);
			}
			else
			{
/*		 	RB drift is insignificant*/
		 		psingle->Int_State=0.0;
		 		psingle->Sprop2=psingle->Prop_State;
			}
			psingle->min_cnt=0;/*resync drift calculation to new time constants*/
			psingle->Sprop_prev=psingle->Prop_State-30.0*psingle->Int_State/(psingle->Prop_Tau);
			psingle->minute_cnt=0;
			if(LTBI.lotype!=RB)
			{
		 		psingle->shift_cnt=((psingle->Prop_Tau+60)/15);
			}
			else
			{
		 		psingle->shift_cnt=((psingle->Prop_Tau+60)/15);
			}

			psingle->Cal_Mode++;
	 	}
	 	psingle->minute_cnt++; 
	} /*end if recalibrate is TRUE */
}
/*************************************************************************
Module to update holdover state
*************************************************************************/   
static double ssu_pvar_wa=1;
static double ssu_pvar_wb=0; 
static double ssu_pred_var=0;   
static unsigned short SSU_Hold_Min_Cnt=0;
static short ssu_hold_count=0;
void Holdover_SSU_Update()
{
	unsigned short Hfreq_Valid,Hgps_Valid;
	double Delta_Hold,Old_Freq,Old_Base,Old_Var;
	double freq_est,ftemp,pred_err,max_pred;
	double temp,fm_mod,freq_no_mod;
	double SlewWindow;
	short i,j,lag_indx,lag0,lag1,lag2,lag3;
	/******* local input message data *****/
	double SSUloop_Freq_Err_Sum_rec;
	unsigned short SSUloop_Freq_Sum_Cnt_rec;
	SSU_Loop_Print();
	SSUloop_Freq_Err_Sum_rec=SSUloop.Freq_Err_Sum;
	SSUloop_Freq_Sum_Cnt_rec=SSUloop.Freq_Sum_Cnt;   
	SSUloop.Freq_Err_Sum=0;
	SSUloop.Freq_Sum_Cnt=0;
	
	/**** accumulate one minute data */
	HMSSU.FreqSum+=SSUloop_Freq_Err_Sum_rec;
	HMSSU.FreqSumCnt+=SSUloop_Freq_Sum_Cnt_rec;  
  	SSU_Hold_Min_Cnt++;

	/* calculate cumulative holdover stats over 4 hour window*/
	ssu_test_seq=0;  

#if NORMAL_HOUR
	if(SSUloop.Cal_Mode<7) /*restart hourly collection process*/
#else
	if(SSUloop.Cal_Mode<6) /*accelerate holdover mode*/
#endif
	{
	  /* generate initial estimates for Osc Freq State*/
	 	if(HMSSU.FreqSumCnt)
	  	{
			temp=HMSSU.FreqSum/HMSSU.FreqSumCnt;
			if(SSUloop.Cal_Mode<2) HMSSU.Osc_Freq_State=temp;
			else HMSSU.Osc_Freq_State=0.2*HMSSU.Osc_Freq_State+0.8*temp;      
			ssu_test_seq|=1;   
			
	  	}
	  	SSU_Hold_Min_Cnt=0;
	  	HMSSU.FreqSum=0.0;
	  	HMSSU.FreqSumCnt=0;
	}
	else if(SSU_Hold_Min_Cnt>MIN_PER_HOUR) /*perform hourly update*/
	{
	  	SSU_Hold_Min_Cnt=0;
	  /*front end to assign pointers for multiple channels*/ 
	  	for(i=0;i<1;i++) /*single channel RTHC application */	  
	 	{
	 		Hfreq_Valid=TRUE;
			if(HMSSU.FreqSumCnt > HOUR_FREQ_CNT_MIN) 
			{
					freq_est=( HMSSU.FreqSum/HMSSU.FreqSumCnt); 
					ssu_test_freq=freq_est; 
					ssu_test_cnt=HMSSU.FreqSumCnt;
					Hgps_Valid=TRUE;
					Hfreq_Valid=TRUE; 
					ssu_test_seq|=2;
			}
			else
			{
		 			Hfreq_Valid=FALSE;
		 			Hgps_Valid=FALSE;
		 			ssu_test_seq|=4; 
		 			ssu_test_cnt=0;

			}

				HMSSU.FreqSum=0.0;
				HMSSU.FreqSumCnt=0;
				phmssu = &HMSSU;
	  			lag0=phmssu->Cur_Hour;
	  			lag1=(phmssu->Cur_Hour+23)%24;
	  			lag2=(phmssu->Cur_Hour+22)%24;
	  			lag3=(phmssu->Cur_Hour+21)%24;
	  			Old_Freq = phmssu->Daily_Freq_Log[lag0];
	  			Old_Base = phmssu->Holdover_Base[lag0];
	  			Old_Var  = phmssu->Prediction_Var24[lag0];
	  			phmssu->Nsum_Hour++;   /*max out at 24 see code */  
	  			
	  			if(Hfreq_Valid&&(ssu_hold_count&0x01)==0) /*good data */
	  			{  
	  			/*** GPZ add renormalize holdover if large step****/
 		  		Delta_Hold = freq_est - phmssu->Daily_Freq_Log[lag1];
                if(Delta_Hold<0) Delta_Hold *=-1.0;  
                /*open up slew window for 1st day */
 		  		SlewWindow = MAX_HOLD_SLEW*24.0/(phmssu->Nsum_Hour);
		  			if(phmssu->Restart|(Delta_Hold>SlewWindow))
		  			{    
		  				ssu_test_seq|=16;
						phmssu->Restart=FALSE;
						phmssu->Nsum_Hour=1;
						phmssu->Cur_Hour=0;
						lag0=0;
						lag1=23;
						lag2=22;
						lag3=21;
						temp=freq_est;
						SSU_HLSF.finp=freq_est; 
						SSU_HLSF.wa=1.0;
						SSU_HLSF.wb=0.0;   
						SSU_HLSF.dsmooth=phmssu->Long_Term_Drift;

						for (j=24;j>0;j--)
						{
							lag_indx=(lag0+j)%24;
							phmssu->Daily_Freq_Log[lag_indx]= temp;
							phmssu->Holdover_Base[lag_indx] = temp;
							phmssu->Holdover_Drift[lag_indx]=phmssu->Long_Term_Drift;
							phmssu->Prediction_Var24[lag_indx]=0.0;
							temp -=phmssu->Long_Term_Drift;
						}
						Old_Freq = temp;
						Old_Base = temp;
						Old_Var  = 0.0;
						phmssu->Freq_Sum_Daily=0.0;
            			phmssu->Var_Sum_Daily =0.0; 
            			// Special Case if large freq step double pump to clear 
            			if(Delta_Hold>SlewWindow)
            			{
            				phmssu->Restart=TRUE;
            				return;
            			}	
		  			} 

		  			Delta_Hold = freq_est - phmssu->Daily_Freq_Log[lag1];
		  			if(Delta_Hold > SlewWindow)
		  			{
						Delta_Hold = SlewWindow;
		  			}
		  			else if(Delta_Hold< -SlewWindow)
		  			{
						Delta_Hold = -SlewWindow;
		  			}     
		  			freq_no_mod=phmssu->Daily_Freq_Log[lag1]+ Delta_Hold;
					phmssu->Daily_Freq_Log[lag0]= freq_no_mod;
					ssu_test_seq|=32; 
					fm_mod=0.0;

	  			}
	  			else /*not good data*/
	  			{
				  /* forcast next freq word*/ 
			 	 /***** GPZ 12-13-00 prevent forcast until first good data*/
		  			if(phmssu->Restart) return;  
		  			ssu_test_seq|=4096;  
		  			ssu_hold_count++;
//					if(LTBI.lotype!=RB)
//					{     
						freq_no_mod =phmssu->Holdover_Base[lag1]+ phmssu->Holdover_Drift[lag1];
			  			phmssu->Daily_Freq_Log[lag0]= freq_no_mod;
			 			ssu_test_seq|=64;
			 			/*Inserted FM modulation to keep prediction error at
			 			upper limit for current smart clock condition*/
			 			 	if(get_smartclk()==SMART_ON)
			 			 	{   
			 			 		temp=SMART_NARROW_FM;   
								if(HMSSU.Predict_Err24Hr<temp)temp=HMSSU.Predict_Err24Hr;
								if(LTBI.lotype==RB)	temp=temp*0.75; 
        				 	}
			 			 	else
			 			 	{
								temp=SMART_WIDE_FM;			 			 	
			 			 	}
		 			 		if((lag0%2)==0)
			 			 	{  
			 			 		ssu_test_seq|=1024;
		 			 	  		phmssu->Daily_Freq_Log[lag0]+=temp; 
		 			 	  		fm_mod=temp;
		 			 	  	}
		 			 	  	else
		 			 	  	{
			 			 		ssu_test_seq|=2048;
		 			 	  		phmssu->Daily_Freq_Log[lag0]-=temp;
		 			 	  		fm_mod=-temp;
		 			 	  	}
//					}
//					else
//					{
//			 			phmssu->Daily_Freq_Log[lag0]= phmssu->Holdover_Base[lag1]+ phmssu->Holdover_Drift[lag1];
//
//					}

	  			}  
	  			
				/******** update holdover state calculations *******/
	  			if(LTBI.lotype!=RB)
	  			{
	 				if(phmssu->Nsum_Day <2) /*use early aging curve during first 3 days */
	 				{
						SSU_Holdover_Drift_Fit(freq_no_mod);
						ftemp=SSU_HLSF.dsmooth ; // strait line fit
						ssu_test_seq|=128;
						for(j=0;j<24;j++)
						{
		 					if(j<phmssu->Nsum_Hour)
		 					{
		  						lag_indx=(lag0-j+24)%24;
		  						if(ftemp!=0.0)
		  						{ 
		  							phmssu->Holdover_Drift[lag_indx]= ftemp;
			 						//phmssu->Holdover_Drift[lag_indx]= 1.0/ftemp;
			 						/****** GPZ add sanity constraints 12-10-00*****/
		 	 						if(phmssu->Holdover_Drift[lag_indx] > MAX_DRIFT_RATE)
			 						{
										phmssu->Holdover_Drift[lag_indx]= MAX_DRIFT_RATE;
			 						}
		 	 						else if(phmssu->Holdover_Drift[lag_indx] < -MAX_DRIFT_RATE)
			 						{
										phmssu->Holdover_Drift[lag_indx]= -MAX_DRIFT_RATE;
			 						}
		  						} 
//								if(Get_Clock_Mode()<2)  
								if(0)
								{
									  phmssu->Holdover_Drift[lag_indx]=0.0;     
									  phmssu->Long_Term_Drift=0.0;
								}  
		  						
//			  						ftemp-= HLSF.mfit; SKIP FOR STRAIT LINE FIT
		 					}
						}
	 				}
	 				else
		 			{   
		 				ssu_test_seq|=256; 
//		 				if(Get_Clock_Mode()<2) 
						if(0)
						{
								  phmssu->Long_Term_Drift=0.0;
						}  
		 				phmssu->Holdover_Drift[lag0]= phmssu->Long_Term_Drift;
		 			}
	   				phmssu->Holdover_Base[lag0]=freq_no_mod;
     			}
    			else
				{       
		  			ftemp=  (phmssu->Daily_Freq_Log[lag0]+phmssu->Daily_Freq_Log[lag1]);
					ftemp+= (phmssu->Daily_Freq_Log[lag2]+phmssu->Daily_Freq_Log[lag3]); 
					phmssu->Holdover_Drift[lag0]= 0.0;     
	 				phmssu->Holdover_Base[lag0] =freq_no_mod; 

//	   				if(phmssu->Nsum_Day==0)
//	   				{
//		 				phmssu->Holdover_Base[lag0] =freq_no_mod; 
//	   				}
//	   				else
//	   				{
//		 				phmssu->Holdover_Base[lag0]=phmssu->Monthly_Freq_Log[((phmssu->Cur_Day+29)%30)];
//	   				}
    			}
	  			phmssu->Freq_Sum_Daily +=(double)freq_no_mod;
				/******** update 24 hr prediction error******/
	  			max_pred=0.0;
				lag_indx=(17+lag0)%24;
	  			ftemp=phmssu->Holdover_Base[lag_indx];
	  			pred_err= 0;
	  			for(j=1;j<8;j++) //Reduced from 24 hrs to 8 for RTHC
	  			{
		 			lag_indx=(17+lag0+j)%24;
		 			ftemp+=phmssu->Holdover_Drift[lag_indx];
		 			pred_err+=phmssu->Daily_Freq_Log[lag_indx]-ftemp;
	  			}
	  			max_pred = pred_err*3600.0;/*convert to time error estimate*/
	  			max_pred *= max_pred;
	  			phmssu->Prediction_Var24[lag0]= max_pred;
	  			phmssu->Var_Sum_Daily+= max_pred;
			  /******* end prediction error update **********/
	  			if(phmssu->Nsum_Hour>24)
	  			{
					phmssu->Nsum_Hour=24;
					phmssu->Freq_Sum_Daily -= Old_Freq;
					phmssu->Var_Sum_Daily-=   Old_Var;
	  			} 

			 if(phmssu->Nsum_Hour >7)
			 {	
			 	ssu_pred_var=ssu_pvar_wa*max_pred+ssu_pvar_wb*ssu_pred_var; 
			  	temp=ssu_pred_var;
 			 	/*reduce filter bandwidth to 24hr moving average equivalant*/
			  	if(ssu_pvar_wa>.0833)
			  	{
					ssu_pvar_wa*=.96;
				 	ssu_pvar_wb=1.0-ssu_pvar_wa;
			  	}
			 }
			 else temp=max_pred;  
			 phmssu->Predict_Err24Hr=sqrt(temp);
			 if(phmssu->Nsum_Hour >15)
			 {	
			 	if(get_smartclk()==SMART_OFF ) 
			 	{
			    	ssu_pvar_wa=1.0;  
			    	ssu_pvar_wb=1.0-ssu_pvar_wa;
			 	}
			 }

	  			phmssu->Osc_Freq_State= (double)(phmssu->Holdover_Base[lag0]+0.5*phmssu->Holdover_Drift[lag0]);
	  			if(phmssu->Osc_Freq_State > MAX_HOLD_RANGE)
	  			{
		 			phmssu->Osc_Freq_State= MAX_HOLD_RANGE;
	  			}
	  			else if(phmssu->Osc_Freq_State < -MAX_HOLD_RANGE)
	  			{
		 			phmssu->Osc_Freq_State=  -MAX_HOLD_RANGE;

	  			}
	  			phmssu->Cur_Hour++;
	  			if(phmssu->Cur_Hour>23)  /* end of day logic  long term drift evaluation*/
	  			{
					phmssu->Cur_Hour=0;
					/* Update monthly data and drift estimate */
					phmssu->Nsum_Day++;
					phmssu->Monthly_Freq_Log[phmssu->Cur_Day]= (phmssu->Freq_Sum_Daily/(double)(phmssu->Nsum_Hour));
					phmssu->Monthly_Pred_Log[phmssu->Cur_Day]=(float)( phmssu->Predict_Err24Hr);
					if(phmssu->Nsum_Day >1)
					{
		  				temp =  ((double)(phmssu->Monthly_Freq_Log[phmssu->Cur_Day])
					 		-(double)(phmssu->Monthly_Freq_Log[(phmssu->Cur_Day+29)%30]))/24.0;
		  				if(phmssu->Nsum_Day ==3) phmssu->Drift_Sum_Weekly=2.0*temp; /*special case discard first drift*/
		  				else  phmssu->Drift_Sum_Weekly+=temp;
		  				if(phmssu->Nsum_Day>4)  /* for RTHC use 3 day average*/
		  				{
			  				phmssu->Nsum_Day=4;
			  				phmssu->Drift_Sum_Weekly -=
								((double)(phmssu->Monthly_Freq_Log[(phmssu->Cur_Day+27)%30])-
								 (double)(phmssu->Monthly_Freq_Log[(phmssu->Cur_Day+26)%30]))/24.0;
		  				}
						/*  incremental offset per hour */
		  				phmssu->Long_Term_Drift = phmssu->Drift_Sum_Weekly/(phmssu->Nsum_Day-1);
						 /* Update NCO Holdover Drift with Range Limits */
		 				if(phmssu->Long_Term_Drift > MAX_DRIFT_RATE)
		 				{
			  				phmssu->Long_Term_Drift= MAX_DRIFT_RATE;

		 				}
		 				else if(phmssu->Long_Term_Drift < -MAX_DRIFT_RATE)
		 				{
			  				phmssu->Long_Term_Drift = -MAX_DRIFT_RATE;

		 				}  
//		 				if(Get_Clock_Mode()<2)   
						if(0)
						{
							 phmssu->Long_Term_Drift=0.0;
						}  
		 				
					}  /* end if Nsum_Daily >1 */
					if(LTBI.lotype!=RB)
					{
		 				if(phmssu->Nsum_Day ==2) /*special case */
		 				{
		  					for(j=0;j<24;j++)phmssu->Holdover_Drift[j]= phmssu->Long_Term_Drift;
		 				}
					}
					phmssu->Cur_Day++;
					if(phmssu->Cur_Day>29)phmssu->Cur_Day=0;
	  			} /* end of end of day logic */
		} /*end for each channel*/ 
		 Holdover_Print();  //print debug info for smartclock
  	} /*end hourly update*/
}   
/*****************************************************************
Time Provider Holdover Debug Print Function
******************************************************************/
void Holdover_Print()
{    
	char cbuf[200];
 	int i1,i2;
//	snapshot= Read_Cntrl_Reg();
// 	if(snapshot&CNTRL_HOLD_MODE) strcpy((char *)(hmode),"FW ");  
//	else strcpy((char *)(hmode),"HW ");
	i1= (HMSSU.Cur_Hour+23)%24;  
	i2= (HMSSU.Cur_Day +29)%30;
    sprintf((char *)cbuf,"%4d,%4x,%4d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%3d,%3d,%3d,%3d,%3d,%3d,%ld\r\n",
//    (int)(get_ticks()&0xFFF), 
    ssu_test_cnt,
    ssu_test_seq,   
    SSU_Hold_Min_Cnt,
    (long int)(ssu_test_freq*(double)(1e15)),
    (long int)(HMSSU.Daily_Freq_Log[i1]*(double)(1e15)), 
    (long int)(HMSSU.Holdover_Base[i1]*(double)(1e15)), 
    (long int)(HMSSU.Holdover_Drift[i1]*(double)(1e15)), 
    (long int)(HMSSU.Monthly_Freq_Log[i2]*(double)(1e15)),     
    (long int)(HMSSU.Predict_Err24Hr*(double)(1e9)), 
    (long int)(HMSSU.Osc_Freq_State*(double)(1e15)), 
    (long int)(HMSSU.Long_Term_Drift*(double)(1e15)),
    HMSSU.Cur_Hour,
    HMSSU.Cur_Day,
    HMSSU.Nsum_Hour,
    HMSSU.Nsum_Day,
    HMSSU.Restart, 
	get_smartclk(),    
//    (char *)hmode, 
    (unsigned long int)(Get_micro_icw())            
    );   
	DebugPrint(cbuf, 0);  
}
/*****************************************************************
Time Provider SSU Loop Debug Print Function
******************************************************************/
void SSU_Loop_Print()
{  
		char cbuf1[120];
	    sprintf((char *)cbuf1,"%4d,%4d,%4d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
	    Is_MPLL_Tracking(),
	    In_MPLL_Phase_Window(),
	    Is_OCXO_warm(), 
	    (long int)(ken_MPLL_freq*(double)(1.0e15)),
	    (long int) SSUloop.Freq_Sum_Cnt,
	    (long int)(SSUloop.Freq_Err_Est*(double)(1.0e15)),
	    (long int) SSUloop.Prop_Tau,
	    (long int) SSUloop.Int_Tau ,
		(long int)(SSUloop.Prop_State*(double)(1.0e15)),
		(long int) (SSUloop.Int_State*(double)(1.0e15)),  
	    (long int) SSUloop.Recalibrate,
	    (long int) SSUloop.Cal_Mode,
	    (long int) SSUloop.shift_cnt
	    );
		DebugPrint(cbuf1, 2);
}

/*********************************************************************
This function is used during the first week of oscillator aging to determine
the early aging curve an OCXO.
This function calculates a least squares fit to the "drift rate"
of the LO hourly frequency data. 


*************************************************************************/
void SSU_Holdover_Drift_Fit(double fin)
{
  double id,denom; /*derived inverse drift sample*/
  if(SSU_HLSF.wa==0)  /* initialize algorithm if start */
  {
	  SSU_HLSF.wa=1.0;
	  SSU_HLSF.wb=0.0;
	  SSU_HLSF.dsmooth=0.0;
	  SSU_HLSF.mfit=0.0;
	  SSU_HLSF.bfit=0.0;
	  SSU_HLSF.sx=0.0;
	  SSU_HLSF.sy=0.0;
	  SSU_HLSF.sxy=0.0;
	  SSU_HLSF.sx2=0.0;
	  SSU_HLSF.finp=fin;
	  return;
  }
  id= fin-SSU_HLSF.finp;
  SSU_HLSF.finp=fin; 
  /**** progressively increase filtering to counteract high noise gain as
		  drift approaches zero. *******/
  SSU_HLSF.dsmooth=SSU_HLSF.wa*id+SSU_HLSF.wb*SSU_HLSF.dsmooth;
  /*reduce filter bandwidth to 24hr moving average equivalant*/
  if(SSU_HLSF.wa>.0833)
  {
	 SSU_HLSF.wa*=.96;
	 SSU_HLSF.wb=1.0-SSU_HLSF.wa;
  }

}

/********************************************************************* 
 This function returns the state of the smart clock algorithm
 The following states are possible
 SMART_OFF no smart clock currently available
 SMART_DEGRADED smart clock available but holdover limited to less than max
 SMART_ON smart clock available at maximum specified duration
*********************************************************************/     
int get_smartclk()
{        
	double scale;  
//	Card_State card_state = Get_card_state();  
	int card_state;
	card_state=0; //gpz override for now 
//	if(card_state==CARD_HOLDOVER&&(HMSSU.Restart==0))return(smart_state);
	if(card_state==1&&(HMSSU.Restart==0))return(smart_state); //dummy code for now
	if(HMSSU.Nsum_Hour<8|HMSSU.Restart) smart_state=SMART_OFF;
	else if(HMSSU.Nsum_Hour<16)
	{         
		scale=3.0*SMART_WIDE/(18.0-HMSSU.Nsum_Hour);
	 	if(HMSSU.Predict_Err24Hr<scale) smart_state=SMART_DEGRADE;
	 	else smart_state=SMART_OFF;
	}
	else
	{
	 	if(HMSSU.Predict_Err24Hr<SMART_NARROW) smart_state=SMART_ON;
	    else if(HMSSU.Predict_Err24Hr<SMART_WIDE) smart_state=SMART_DEGRADE;
        else smart_state=SMART_OFF;
	}
	return(smart_state); //BUG BUG there was no return statement!!!
}       

/********************************************************************* 
 This function returns the best freq estimate of the smart clock algorithm
2.72478E-11
 
*********************************************************************/ 
static double frac_err=0;
long int get_smart_freq()
{    
	double err,temp;
	long int fword;   
//	if(Get_Clock_Mode()==2)    /*PRR Mode*/
//	{  
//		temp=  smi.freq_state_cor+smi.phase_state_cor+smi.temp_state_cor;
//		fword= (long int)(((temp+frac_err)/1.70298985e-12)+0.5);  
//		err=temp-(fword*1.70298985e-12);  
		temp=  smi.freq_state_cor+smi.phase_state_cor+smi.temp_state_cor;
		fword= (long int)(((temp+frac_err)/1.70298985e-12)+0.5);  //was 2.72478E-11
		err=temp-(fword*1.70298985e-12);
		
//	}
//	else
//	{
//		fword= (long int)(((HMSSU.Osc_Freq_State+smi.temp_state_cor+frac_err)/1.70298985e-12)+0.5);
//		err=HMSSU.Osc_Freq_State-(fword*1.70298985e-12);
//	}
	frac_err+=err;	
	return(fword);
}
void smart_clk_restart()
{
	HMSSU.Restart=TRUE;
}

static unsigned long currentSmartICW = 0L;
void refresh_smart_icw(void)
{
	uint8 tmp;
	long freq;
	freq = get_smart_freq();
	tmp = get_exr();
	set_exr(7);
	currentSmartICW = (unsigned long)(freq);
	set_exr(tmp);
}

/************************************************************************
*TITLE:     
*IN:           NONE
*              
*OUT:          NONE
*Return value: NONE
*Description:  
*************************************************************************/  
unsigned long Get_micro_icw(void)
{       
	//return get_smart_freq();
	// Call from 25ms ISR
	return currentSmartICW;
} 

/***********************************************************************
Moduled called after init tasks to ssu specific smart clock
features (encapsulated small subset of BesTime) GPZ June 2004
*************************************************************************/
void init_ssu()
{
		  short j;  
		  short i;
		  double temp; 
		  if(Cal_Data.cal_state)  /*doesn't work in mode 0 or 1   (kjh)*/
		  { /*if valid calibration data*/
			 ssuloop= &SSUloop; 
			 if(Cal_Data.cal_state==CAL_Dnld||
				((MSG_RCW_C.RCW&0x7)>1))
			 {
			
/*			  #if HOTDLOAD*/
			  if(MSG_RCW_C.RCW&0x7==3)
			  {
				ssuloop->Prop_Tau=LTBI.single_prop_tau;
				ssuloop->Int_Tau= LTBI.single_int_tau;
				ssuloop->Dint_Tau=LTBI.single_dint_tau;
				ssuloop->Propf1=1.0/ssuloop->Prop_Tau;
				ssuloop->Propf2=1.0-ssuloop->Propf1;
				ssuloop->Recalibrate=FALSE;
				if(LTBI.lotype==RB)
				 ssuloop->Cal_Mode=7;
				else
				 ssuloop->Cal_Mode=5;
			  }
			  else
			  {
				ssuloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_recal;
				ssuloop->Int_Tau= LTBI.single_int_tau/LTBI.single_recal;
				ssuloop->Dint_Tau=LTBI.single_dint_tau;
				ssuloop->Prop_Tau *=LTBI.gear_space;
				ssuloop->Prop_Tau *=LTBI.gear_space;
				ssuloop->Prop_Tau *=LTBI.gear_space;
				ssuloop->Int_Tau  *=LTBI.gear_space;
				ssuloop->Int_Tau  *=LTBI.gear_space;
				ssuloop->Int_Tau  *=LTBI.gear_space;
				ssuloop->Propf1=1.0/ssuloop->Prop_Tau;
				ssuloop->Propf2=1.0-ssuloop->Propf1;
				ssuloop->Cal_Mode=5;
				ssuloop->Recalibrate=TRUE;
			  }
			 }
			 else
		 	{
			  ssuloop->Prop_Tau=LTBI.single_prop_tau/LTBI.single_recal;
			  ssuloop->Int_Tau= LTBI.single_int_tau/LTBI.single_recal;
			  ssuloop->Dint_Tau=LTBI.single_dint_tau;
			  ssuloop->Propf1=1.0/ssuloop->Prop_Tau;
			  ssuloop->Propf2=1.0-ssuloop->Propf1;
			  ssuloop->Cal_Mode=2;
			  ssuloop->Recalibrate=TRUE;
		 	}
			 ssuloop->Prop_State=Cal_Data.LO_Freq_Cal;
			 ssuloop->Int_State= Cal_Data.LO_Drift_Cal*ssuloop->Prop_Tau/1800.0;
			 ssuloop->Dint_State=0.0;
			 ssuloop->NCO_Cor_State= ssuloop->Prop_State + ssuloop->Int_State;
			 ssuloop->Freq_Err_Est=0.0;
			 ssuloop->LO_Freq_Pred= Cal_Data.LO_Freq_Cal;
			 ssuloop->Freq_Err_Sum=0.0;
			 ssuloop->Freq_Sum_Cnt=0;
			 ssuloop->Sprop1=Cal_Data.LO_Freq_Cal;
			 ssuloop->Sprop2=  Cal_Data.LO_Freq_Cal;
			 ssuloop->Intf1=60.0/ssuloop->Int_Tau;
			 ssuloop->Intf2=1.0-ssuloop->Intf1;
			 for(j=0;j<GPSDELAY;j++)
			 {
			  ssuloop->Ygps_Buf[j]=Cal_Data.LO_Freq_Cal; /*Buffer of last n gps loop inputs*/
			 }
			 ssuloop->Sprop_prev= Cal_Data.LO_Freq_Cal;
			 ssuloop->min_cnt=0;
			 ssuloop->LO_weight=1.0; /* ensemble weight contribution of Local Oscillator */
			 ssuloop->minute_cnt=0;
			 if(LTBI.lotype==RB)
			 ssuloop->shift_cnt=((ssuloop->Prop_Tau+60)/22);
			 else
			 ssuloop->shift_cnt=((ssuloop->Prop_Tau+60)/20);
			 ssuloop->jamloop=FALSE;
			 /**** jam previous 24 hour holdover data base*/
			 temp=Cal_Data.LO_Freq_Cal;
			 HMSSU.Freq_Sum_Daily=0.0;
			 for (i=24;i>0;i--)
			 {
				j=i%24;
				HMSSU.Daily_Freq_Log[j]=temp;
				HMSSU.Holdover_Base[j]=temp;
				HMSSU.Holdover_Drift[j]=Cal_Data.LO_Drift_Cal;
				temp -=Cal_Data.LO_Drift_Cal;
			 }
			 HMSSU.Cur_Hour=0;
			 HMSSU.Cur_Day=0;
			 HMSSU.Nsum_Hour=0;
			 HMSSU.Nsum_Day=0;
			 HMSSU.Freq_Sum_Daily=0.0;
			 HMSSU.Predict_Err24Hr=0.0;
			 HMSSU.Long_Term_Drift=Cal_Data.LO_Drift_Cal;
			 HMSSU.Drift_Sum_Weekly=0.0;
			 HMSSU.Osc_Freq_State=Cal_Data.LO_Freq_Cal;
			 HMSSU.Restart=TRUE;
		  }/*end if valid cal data*/
        /*update NCO to best initial estimate*/
		  refresh_smart_icw(); //GPZ make sure smart_icw is ready for freerun		  
		  
} 
void BT3_1second()
{
// call BT3 one second tasks here

//     DebugPrint("one second task \r\n");  

    
 		BT3_span_est();  
 		BT3_mpll_est();
// 		TP_Est_250ms();             
		Status_Report();     /* fills out status data into smi, SReg, and MSG_ASR structures */
//    	Check_OCXO();        /* This function will check if OCXO is warm and ready */
		BT3_1Sec_Tasks();    /* Process GPS 1 PPS estimate */
		BT3FLL_Update();     /* span lines, ROs and GPS FLLs */
		SSUFLL_Update();     /* span lines, ROs and GPS FLLs */  
		NCO_Update();        /*GPZ note do update before refresh to ensure we synchonize measurement and control*/
		refresh_smart_icw(); 
//		ATC_Update();        /* Temperature comensation */

//		Stab_Ana_Update();
		PCOMP_Update_1sec(); 
//		PL_Est_250ms();
		Msg_Gen_1sec();      /* generate messages to slower tasks */  
		ShellMain();                              

    
} 
void BT3_1minute()
{
//    DebugPrint("one minute task \r\n"); 
	Alarm_Report_Loop_Print(); //Debug test only    
	Engine_Report_Loop_Print(); //Debug test only
	three_corner_decomp();	
	Holdover_Update();
	Holdover_SSU_Update();    
	Holdover_Print();
	Cal_Data_Update();  
	if(SSUloop.Recalibrate) SSU_Recalibrate();
}
void BesTime_init(int isRubidium)
{

	if (isRubidium > 0) {
		g_freqMax = SFREQMAX_RB;
	} else {
		g_freqMax = SFREQMAX_XO;
	}

	// zmiao 6/1/2009: Old code, may look odd. Leave it alone for now.
	LTBI.lotype=DOCXO;
	Init_Tasks();
	Init_Shared_Mem_Int();
 	BT3_MSG_Init();
	ShellInit();
//	BT3Flash_init();    
//	Get_Flash_Cal_Data(); 
//	Cal_Data.cal_state = 2;
	init_cal();
	init_ssu(); /*new task to intialize SSU specific smart clock features*/
}  
