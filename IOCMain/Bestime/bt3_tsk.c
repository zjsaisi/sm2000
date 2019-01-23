/***************************************************************************
$Header: bt3_tsk.c 1.10 2010/12/22 16:43:37PST Jining Yang (jyang) Exp  $
$Author: Jining Yang (jyang) $
$Date: 2010/12/22 16:43:37PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_tsk.c_v  $
$Modtime:   06 Feb 1999 12:41:42  $
$Revision: 1.10 $


                            CLIPPER SOFTWARE
                                 
                                BT3_TSK.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module performs suite of 250 msec tasks to manage the
                  Frequency/Time hardware control system.

MODIFICATION HISTORY:

$Log: bt3_tsk.c  $
Revision 1.10 2010/12/22 16:43:37PST Jining Yang (jyang) 
Use RGP handle which contains TOD source Id to determine if dither is needed, instead of using offset value.
Revision 1.9 2010/11/17 13:57:13PST Jining Yang (jyang) 
George added dither for VCom receiver.
Revision 1.8 2010/10/14 00:06:17PDT Jining Yang (jyang) 
Merged from 1.7.1.3.
Revision 1.7.1.3 2010/10/07 17:01:51PDT Jining Yang (jyang) 
George added protection during transient.
Revision 1.7.1.2 2010/08/20 09:09:19PDT Jining Yang (jyang) 
George fixed overshoot problem.
Revision 1.7 2009/07/02 17:48:36PDT Zheng Miao (zmiao) 
GPZ change
Revision 1.6 2009/04/23 15:24:47PDT zmiao 
Correct the else position
Revision 1.5 2009/04/17 11:39:29PDT zmiao 
George's change
Revision 1.4 2008/05/21 12:32:33PDT gzampetti 
Minor GPS related edits
Revision 1.3 2008/04/01 07:43:54PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.2 2008/01/29 11:29:22PST zmiao 
dev
Revision 1.1 2007/12/06 11:41:07PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.21 2006/07/05 10:26:54PDT gzampet 
adjusted phase control to limit change in frequency slew to maintain DTI limits
Revision 1.20 2006/06/02 16:58:11PDT gzampet 
fixed 15e-9 constant for extended phase control to 15.0 ns/s to get scaling right for large offset
Revision 1.19 2006/05/04 19:57:54PDT gzampet 
more max_slew tweaking
Revision 1.16 2006/05/04 14:08:40PDT gzampet 
supress phase slew in lower gears
Revision 1.15 2006/05/02 12:56:46PDT gzampet 
Updated to support the BT63 command and general startup issues in the lower gears
Revision 1.14 2006/05/01 08:31:00PDT gzampet 
tweaked drift threshold for furino engine
Revision 1.13 2006/04/28 13:08:03PDT gzampet 
fixed ext_slew calc
Revision 1.11 2006/04/28 09:34:23PDT gzampet 
added extended phase slew control
Revision 1.10 2006/04/27 11:50:52PDT gzampet 
fixed pps range flag bug
Revision 1.9 2006/04/26 13:53:47PDT gzampet 
32 bit phase control
Revision 1.8 2006/04/19 11:33:39PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.7 2006/04/19 07:56:07PDT gzampet 
fix pps mag calculation
Revision 1.6 2006/04/18 15:23:00PDT gzampet 
Added phase control
Revision 1.5 2006/04/13 09:37:35PDT gzampet 
sawtooth fixed
Revision 1.4 2006/03/06 14:25:15PST gzampet 

Revision 1.3 2006/03/02 16:46:34PST gzampet 

Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.20 2005/01/10 13:26:55PST gzampet 

Revision 1.19  2004/09/22 22:04:38Z  gzampet
Revision 1.18  2004/09/17 16:36:37Z  gzampet
Revision 1.17  2004/09/03 16:51:56Z  gzampet
Revision 1.16  2004/08/27 15:12:11Z  gzampet
Revision 1.15  2004/08/25 23:04:49Z  gzampet
Revision 1.14  2004/08/24 21:47:01Z  gzampet
Revision 1.13  2004/08/17 17:22:00Z  gzampet
Revision 1.12  2004/07/30 23:59:27Z  gzampet
Revision 1.11  2004/07/09 18:29:01Z  gzampet
Revision 1.10  2004/07/01 15:43:43Z  gzampet
Revision 1.9  2004/07/01 13:12:17Z  gzampet
Revision 1.8  2004/06/24 14:37:41Z  gzampet
Full Merge with TS3X released version
Revision 1.7  2004/06/23 15:15:38Z  gzampet
Baseline GPS BesTIme Version
Revision 1.6  2003/05/13 15:38:23Z  gzampet
Revision 1.5  2003/04/25 18:03:25Z  zmiao
Revision 1.4  2003/04/24 14:59:18Z  gzampet
Revision 1.3  2003/04/19 00:11:03Z  gzampet
Revision 1.2  2003/04/15 21:16:03Z  gzampet
Revision 1.9  2002/08/16 19:16:46Z  gzampet
Change criteria to accept RTHC input frequency into "GPS" loop 
Also add temperature compensation into Cdata calculation 
Revision 1.8  2002/08/09 22:42:25Z  gzampet
Added MPLL_Phase_Window as additional criteria to validate 
pseudo GPS input data for LLOOP operation in RTHC application
Revision 1.7  2002/07/05 22:15:59Z  zmiao
GZ's second wave change
Revision 1.6  2002/07/03 21:32:41Z  zmiao
Merge with GZ
Revision 1.5  2002/06/07 18:01:05Z  kho
Jam only once a minute instead of every 3 seconds.  Every 3 seconds was a
problem because the loop did not have time to stablize before a new jam occured.
Revision 1.4  2002/06/04 17:09:01Z  kho
Added GPS_Reg qualifier to GPS_Valid if statement.  This will make sure
that that the lock doesn't start until all the alarm are gone.
Revision 1.3  2002/06/03 22:15:15Z  kho
Deleted if statement that zeroed GPS_Delta when its magnitude was 
greater than 100e-9. 
Revision 1.2  2002/05/31 17:59:58Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
 * 
 *    Rev 1.0   06 Feb 1999 15:41:52   S.Scott
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
*     Multi tracking Core Control Algorithm  (MCCA) renamed to BT3 see history
*     Module: task.c
*     Description: Module performs suite of 250 msec tasks to manage the
*                  Frequency/Time hardware control system.
*     Version 1.0  2/22/96 Baseline Version for Cellnet Application
*		Version 1.1  3/4/96  Includes changes required to integrate to Cell
*						 Net platform
*		Version 1.2  3/5/96
*						 1) Create an E1_Jitter Event if NDATA is too small.
*                  2) Changed bias compensation minus instead of plus
*                     Tcxo_bias
*                  3) Added Phase Step Detection (New E1 Fifo Structure)
*                  4) Added Revertive Cnt to hold off reversion back to
*                     E1 after a transient is detected
*     Version 1.3 3/7/96
*                  1) Changed Freq_Estimation Algorithm to faster all
*                     integer based  Old 4kHz Task: 80ms New: 35ms!!!
*     Version 1.4 3/11/96  Add user ajdustable thresholds.
*                  1) Added declaration of new Threshold Management Structure.
*                  2) Add initialization of threshold managment structure and
*                     new commom memory threshold data elements.
*                  3) Added threshold management code on shared memory overrride
*                  4) Changed comparisons in task.c to use TM thresholds instead
*                     of using defines directly
*                  5) Intialized E1B input as present for dual input operation.
*                  6) Added E1B Leaky bucket managment to ASM and intialize
*                     associated flags.
*                  7) Added all E1B report bits to status update
*     Version 1.5 5/20/96
*                  1) Added new  double Prim_Cor_State initialization
*						 2) Change DiFLL Control Update Logic to use the Primary
*                     Cor_State as the feedback input to the GPS calibration
*							 loop and to alway do a calibration update of GPS if valid
*                  3) Change binary filter first segment to "8" and
*                     increase gain from 2^22 to 2^24
*                  4) Add reset of E1 control loop proportional and integral
*                     states of E1 event occurs.
*                  5) Added  Yprim_Buf[PRIMDELAY] initialization and use as a
*                     delay line for the dual control loop.(new j index)
*                  6) Added  Prim_Skip count initialization and use as a
*                     blanking control for the dual control loop.
*                  7) Use delayed primary loop data for E1 vs LO stats.
*                  8) Fixed bug that allowed corrupt primary data to be
*                     stamped as valid for the EA-TCXO and EB-TCXO channels
*                     when a E1 transient is detected (added else condition
*                     to expicitely set Data_250ms.Data_250ms.Data_Valid to
*                     FALSE.
*
*     5/23/96 Module Rename mccatask.c as part of cellsync 2000 project
*             revision history of "new" module starts with 1.0
*     Version 1.0 5/23/96
*                  1) Add separate routine to estimate the remote oscillator
*							 fractional frequency with respect to the local oscillator
*							 R0_Freq_Est250ms()
*                  2) Added Remote Oscillator Delta Max in Threshold Overide
*                     Function.
*
*                  3) Add Remote Oscillator Jitter Flag
*
*                  4) Add initialization of remote oscillator "FIFO" (Circular
*                     Queue)
*                  5) Add RO_NDATAMIN buffer check
*
*                  6) Reorganize FREQ_Est_250ms now includes all E1 processing.
*
*                  7) Add RO_FCONV normalization constant
*
*                  8) Add static ROdriftCheck and ROdriftCount
*
*                  9)Improve ability to detect slow drift. Previous drift
*                    statistic (Ye1avg-Ye1avgold) only observed over 20 * .25
*                    5 decond integration time. New stat Ye1Avg-Prim_Cor_Stat
*                    observes drift over longer time constant of primary loop.
*                    See change in Drift Check code. ploop pointer added to for
*                    loop (for both E1 channels) in Dual_Pll_250ms.
*                  10) Added logic in DPLL_update to determine if a permanent
*                     shift in input frequency has occured to manage a
*                     recalibration. see new if statement starting with comment
* 		  if drift alarm is present when Prim_Skip =1 then assume permanent
*	     step in E1-TCXO frequency and perform necessary jam and recal.
*                  11) Added Recalibration Flag Initialization
*                  12) Add Recalibration function and declaration
*                  13) Add call to recalibration every minute if Recal flags are
*                     TRUE
*                  14) Added Remote Oscillator PLL Initialization
*                  15) Fixed bug 7/8/96 to set DloopA.mode to GPS_NORMAL when
*                      returning from holdover
*                  16) Add Temperate Measurement Structure initialization 7/16/96
*                  17) Add TP_Freq_Est250ms function to update temperature estimate
*                  18) Add TP_FCONV normalization constant.
*                  19)  Add static TPslewCheck and TPslew Count to manage slew
*                      rate calculation and alarms
*                  20) Added Remote Oscillator and Temperature Status to
*                      Status_Report()
*                  21) Add Remote Oscillator and Temperature Alarm logic in
*                      ASMM
*                  22) Initialize Remote Oscillator and Temperature Alarm Flags
*                  23) Add Temperature Calibration Overhead and Record Structure
*                      Initialization
*                  24) Add TC_Update_250ms task to update calibration data base
*                      during calibration operation.
*                  25) Add Initialization of Temperature Compensation data base
*                  26) Add TCOMP_Update_1sec(void) function to update local
*                      oscillator temperature correction state
*     Version 2.0 8/8/96
*                    1)Added PCOMP_Update_1sec(void) and change to only update
*                      if GPS is valid.
*     Version 2.1 8/15/96
*                    1)In general changes in this version are purely to make
*                      the greenhills enviroment happy with this C code.
*                    2)Renamed the Temperature_Calibration Data Structure Tag
*                      to be Tmperature_Calibration to prevent any confusion
*                      with the Temperature_Calibration_Data_Record
*                    3)Change Temperature Calibration variable TC symbol
*                      to TmpC because Greenhills assembler got confused with
*                      this symbol maybe has a problem with all capitals looking
*                      like an assembler symbol?
*                    4)Changed PC label to Pcomp for the phase compensation data
*                      structure for the same reasons as (3)
*      Version 2.2 9/15/96
*                    1) In Temperature Calibration 250ms routine changed logic
*                       to make sure that database is not updated until TC.start
*                       blanking count is decremented to zero. This ensures that
*                       the system is warm-up before starting calibration database
*                       update.
*                     2) In Temperature Calibration add smoothing filter and
*                        slew rate limiting to ensure consistent detection
*                        of transistions from bin to bin during temperature
*                        ramps
*                     3) Add NCO_convert routine to update smi NCO_Low word
*                        and NCO_dither byte in smi
*                     4) Add NCO_High smi to make 8ms update routine simplier
*                        (either use this low 32 bit or high 32 bit NCO
*                        depending on accumulating dither count and if there
*                        is a carry use high word else use low word)
*      Version 2.4 Source Code Reorganized
*                     1) Move Freq_Est Temp_Est and RO_Est to new MCCA_est
*                        module
*                     2) Moved One second tasks to separate funtional module
*                        MCCA_1Sec_Tasks(void)
*                     3) Added Power Level and Antenna Current Registers in
*                        smi and support code in Status_Report
*
*         MODULE RENAME BT3_TSK.C 9-15-97  as part of the BT3 Core Clock Engine
*         Project.
*     Version 1.0 Baseline BT3 clock engine
*                     1) ASMM Module moved to new bt3_mgr.c source file to
*                        improve source file management.
*                     2) Rename Dpll_Update to BT3FLL_Update to reflect the
*                        generalize application. Change the update rate to once
*                        per second and the module is to be called from the
*                        external task scheduler (ttab.asm).
*                     3) Moved BT3_FLL_Update and Holdover_Update to new source
*                        module BT3_FLL.c to improve manageability.
*                     4) Removed void TC_Update_250ms(void) from source code for
*                        BT3. This routine was to be used during temperature school
*                        to create a calibration database. The current approach
*                        is to use an external data analysis to extract the
*                        static and dynamic temperature coefficients when
*                        temperature school is required.
*                      5) Change pcomp attach rate to decrease by 0.125 instead
*                         off 0.5 to improve time lock during warm-up and
*                         thermals Feb 9, 1998
*      Version 2.0 Add RTOS compatible messaging
*                      1) Added MSG_EST_ATC to provide both oven temperature
*                         and RO data to adaptive temperature compensation
*                      2) removed external link to temperature FIFO and RO Fifos
*                         use messaging only
*      Version 3.1
*                     1)Add RB and WINDOW  control of Pcomp task.
*                     2) Use Lloop.Cal_Mode "Gear level" to manage phase control
*                        task. Note Gear Shift is set up for a final state of
*                        "6" for Rb and DOCXO operation
*
*****************************************************************************/
#include "bt3_def.h"
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#include <math.h>
#endif    
#include <stdio.h>  
/******** global data structures *********/
#include "bt3_dat.h"   /*main file for global data declarations*/
#include "bt3_msg.h"   /*main file for message buffer templates*/
#include "bt3_shl.h"   /*main file for message shell templates*/
#include "AllProcess.h"
#include "BT3Support.h" 
//#include "proj_inc.h"   
// state names to control traceability source normally GPS
enum
{
TRACE_SPA =0,
TRACE_SPB =1,
TRACE_PRR =2,
TRACE_GPS =3,
TRACE_GPS_NO_IMC =4
}; 
//TODO set to TRACE_GPS for release build!!!!!
unsigned short trace_source=TRACE_GPS; //controls traceability source
//unsigned short trace_source=TRACE_GPS_NO_IMC; //controls traceability source  
unsigned short TOD_Mode=0; //GPZ June 2010 added for hybrid mode support
unsigned short phase_control=1; //phase control for PPS set to zero for release!
unsigned int phase_skip=0; //GPZ June 2010 added to squelch phase control after source switch
    
unsigned short IMC_flag=1;
unsigned short phase_print;
struct Temperature_Calibration_Data_Record TCDR[MAXTEMPRECORDS];
struct Temperature_Compensation Tcomp;
struct Phase_Compensation Pcomp;
struct GPS_Measurement_Data GM;
struct Threshold_Management TM;
struct Adaptive_Temperature_Compensation ATC;
struct ASM_Management ASMM; /* Adaptive state machine for both SPAN inputs */
extern struct Shared_Memory_Interface smi;
extern struct Calibration_Data  Cal_Data;
extern struct Channel_Data Cdata[MAXCHAN];
extern struct Dual_Input_FLL_Control_System DloopSA,DloopSB,DloopRA,DloopRB;
extern struct Single_Input_FLL_Control_System Lloop;
extern struct Ensembling_Control_Data  ECD;
extern struct Status_Registers SReg;  
extern unsigned short cts_reg;
unsigned short cts_reg_latch; 
extern struct LO_TYPE_BUILD_INFO LTBI;
struct MSG_PCOMP_EST MSG_PCOMP_EST_A,*ppcompestmsg;
extern struct MSG_EST_ATC MSG_EST_ATC_A, MSG_EST_ATC_B;
/******* 1 second task input freq message data *****/
extern double ken_MPLL_freq ;//open loop RTHC test data
extern char Is_Tracking_Valid(void);  
extern double YspavgA_rec,YspavgB_rec,YroavgA_rec,YroavgB_rec;  /*_rec indicated received data*/
extern double OneSecDelta[4],OneSecAvg[4];
double Temp_Est_tsk; /*task file copy of Temp_Est variable*/ 
extern unsigned short Holdover_Recovery_Jam;
double YroavgA_tsk,YroavgB_tsk; /*task file static copy of message variables*/
struct MSG_Timing_Status_Report MSG_TSR;    
SReportGPS RGPD,RGPP,RGPH[8];    
int RGP_Freeze,RGP_Indx;    
double stcomp,stcompsmooth;/* Furino sawtooth compensation terms*/
//extern void stab_ana(int);
extern void timing_stat_report(float statout[TOTAL_TAUCAT],
										 unsigned char Chan,unsigned char Stat);

unsigned short GPS_Valid;
unsigned short GPS_Drift_Count;
unsigned short JamDloopA, JamDloopB;  
double max_slew=0.0;  
#if PRINTLOG
extern unsigned long ptime; /* print log time stamp in cumulative minutes*/
#endif
extern unsigned short  pcount; /*manage rate of print logging*/
extern unsigned long gpssec; /*updated every 5 sec nominally when gps state data is received*/
/* add PPS control flags ***/
unsigned short OnePPSJamFlag;
/***** function templates *********/
void Stab_Ana_Update(void);
void TCOMP_Update_1sec(void);
void ATC_Update(void);
void PCOMP_Update_1sec(void);
void PCOMP_Validate_1sec(void);
void BT3_1Sec_Tasks(void);
void ATC_Mode(unsigned short);
void GPS_interp(void);   
void BT3_1second(void);
void BT3_1minute(void);
extern void ASM(void);
extern void Shared_Mem_Override(void);
extern void Command_Mgr(void);
extern void Holdover_Update(void); 
extern void	Cal_Data_Update(void);    
extern unsigned short Get_Gear(void);
void Phase_Loop_Print(void);
// extern void DebugPrint(const char *str);  
extern void Holdover_Print(void);
#if(TARGET==CLPR)
extern void GetOCXOControl(unsigned short *);
#endif
#if !PCSIM
double sqrt(double);
#endif

/*******************************************************************
This module updates the stability analyzer data channels every
1 second update.
*********************************************************************/
void Stab_Ana_Update()
{ 
// dummy function for timeprovide (stability analysis on PPC)
}
#if 0
void Stab_Ana_Update()
{

	double close_loop_cor;
	close_loop_cor=-(smi.freq_state_cor+smi.phase_state_cor);
	 /********** update channel input data **************/
	if(!(SReg.SPA_Reg&0x130))
	{
			 /*use raw 1 second data to see all transients*/
			 Cdata[SA_LO].X1000ms+= YspavgA_rec+close_loop_cor;
			 Cdata[SA_LO].Data_Valid=TRUE; /* per channel validity flag */
	}
	else
	{
			 Cdata[SA_LO].Data_Valid=FALSE; /* per channel validity flag */
	}
	if(!(SReg.SPB_Reg&0x130))
	{
			 /*use raw 1 second data to see all transients*/
			 Cdata[SB_LO].X1000ms+= YspavgB_rec+close_loop_cor;
			 Cdata[SB_LO].Data_Valid=TRUE; /* per channel validity flag */
	}
	else
	{
			 Cdata[SB_LO].Data_Valid=FALSE; /* per channel validity flag */
	}
//	if(!(SReg.GPS_Reg&0x3F)) 
	if(1)
	{
	 //RTHC open loop DOCXO data 
	     Cdata[GPS_LO].X1000ms+=ken_MPLL_freq-Cal_Data.LO_Freq_Cal_Factory;
	     Cdata[GPS_LO].Data_Valid=TRUE;
//		Cdata[GPS_LO].X1000ms+=Lloop.Ygps_Buf[0]+close_loop_cor;
//		Cdata[GPS_LO].Data_Valid=TRUE; /* per channel validity flag */
	}
	else
	{
			 Cdata[GPS_LO].Data_Valid=FALSE; /* per channel validity flag */
	}
		/***** Update "derived measurement states" ******/
/*		if(pmda->SPAN_Valid && DloopA.Prim_Skip==0)
		{
		  Cdata[SA_GPS].X1000ms+=(DloopA.Yprim_Buf[0] - GM.DeltaGPS)*TAUZERO;
		  Cdata[SA_GPS].Data_Valid=TRUE; /* per channel validity flag */
/*		}
		else  Cdata[SA_GPS].Data_Valid=FALSE; /* per channel validity flag */
/*		if(pmdb->SPAN_Valid&& DloopB.Prim_Skip==0)
		{
		  Cdata[SB_GPS].X1000ms+=(DloopB.Yprim_Buf[0] - GM.DeltaGPS)*TAUZERO;
		  Cdata[SB_GPS].Data_Valid=TRUE; /* per channel validity flag */
/*		}
		else  Cdata[SB_GPS].Data_Valid=FALSE; /* per channel validity flag */
/*	 }
/*	 else    Cdata[GPS_LO].Data_Valid=FALSE; /* per channel validity flag */
	/* Call stability analysis with channel data if required */
		stab_ana(MAXCHAN);
 }
#endif
/***************************************************************************
Module for updating Temperature Compensation. Algorithm handles both dynamic
and static temperature compensation
***************************************************************************/
void TCOMP_Update_1sec(void)
{
	 short i;
	 short delta_temp;
	 short temp_indx;
	 short region,sub_region;
	 short region_comp,inc_comp;
	 short old_base;
	 unsigned short Oven_Drive; /*measure of heater power to inner or outer oven 0-1024*/
//	 #if(TARGET==CLPR)
	 #if 0 /* (kjh) */
	 /** use current FET control level divided by 4 to drive lookup table**/
	 GetOCXOControl(&Oven_Drive);
	 Oven_Drive=Oven_Drive/4;
	 #else
	 Oven_Drive=Temp_Est_tsk;
	 #endif
	 if(ASMM.TP_Flag)
	 {
		Tcomp.part_indx=((short)(Oven_Drive)-Tcomp.Part_Base);
		delta_temp=(Tcomp.part_indx-Tcomp.prev_part_indx);
		/*** test for initial starting point ****/
		if(delta_temp>20)
		{
		  Tcomp.prev_part_indx=Tcomp.part_indx;
		}
		if(delta_temp>0)       delta_temp=1;
		else if(delta_temp<0) delta_temp=-1;
		Tcomp.part_indx=Tcomp.prev_part_indx+delta_temp;
		Tcomp.prev_part_indx = Tcomp.part_indx;
		/*Check if current partition is invalid*/
		if(Tcomp.part_indx < 0 ||
		 Tcomp.part_indx > (TEMPERATURE_CACHE_LENGTH-2) ||
		 Tcomp.Part_Init==TRUE)
		{
		  Tcomp.Part_Init=FALSE;
		 /*determine new base*/
		 old_base=Tcomp.Part_Base;
		 Tcomp.Part_Base  = Tcomp.Part_Base + Tcomp.part_indx
								-(TEMPERATURE_CACHE_LENGTH/2);
		 if(Tcomp.Part_Base<0) Tcomp.Part_Base=0;
		 Tcomp.Part_Base -= (Tcomp.Part_Base%4); /*start on region boundary*/
		 /* reinitialize partition index */
		 Tcomp.part_indx += old_base - Tcomp.Part_Base;
		 if(Tcomp.part_indx<0)
		 {
			 Tcomp.part_indx=0;
			 delta_temp=0;
		 }
		 Tcomp.prev_part_indx=Tcomp.part_indx;
		 /* fill partition with incremental correction data */
		 for(i=0;i< TEMPERATURE_CACHE_LENGTH;i++)
		 {
			 temp_indx=Tcomp.Part_Base+i;
			 region=temp_indx/4;
			 if(region>=MAXTEMPRECORDS)
			 {
				region=(MAXTEMPRECORDS-1);
			 }
			 sub_region=temp_indx%4;
			 if(sub_region==0) /* enter new region */
			 {
				 region_comp=Tcomp.Freq_Comp[region];
				 inc_comp=region_comp/4;
			 }
			 region_comp -= inc_comp;
			 if(sub_region==3) inc_comp+=region_comp;
			 Tcomp.Partition[i]=inc_comp;
		 } /* end for loop to fill partition*/
		}/*end if partition is invalid*/
		/******* update static and dynamic temperature compensation*/
		if(delta_temp > 0)
		{
		 Tcomp.Static_Temp_Cor+=Tcomp.Partition[Tcomp.part_indx-1];
		}/*end update static temperature compensation*/
		else if (delta_temp < 0)
		{
		 Tcomp.Static_Temp_Cor-=Tcomp.Partition[Tcomp.part_indx];
		}/*end update static temperature compensation*/
		/* update dynamic to be done */
		#if(TARGET==CLPR)
		Tcomp.Dynamic_Temp_Cor=Tcomp.Slew_Comp*(Temp_Est_tsk-TPNOMINAL);
		#endif
		/*update shared memory temp compensation state*/
		smi.temp_state_cor=( Tcomp.Static_Temp_Cor + Tcomp.Dynamic_Temp_Cor)*1e-12;
	 } /*end if Temperature is Stable*/
	 else
	 {
		 Tcomp.prev_part_indx=(Oven_Drive-Tcomp.Part_Base);
	 } /*end else temperature is not stable*/
}
/***************************************************************************
Module for testing temperature compensation algorithm only.
*****************************************************************************/
void Temp_Comp_Test()
{
	int i;
	for(i=1024;i>=0;i--)
	{
	  if(i%2==1)
	  Temp_Est_tsk=i+4;
	  else
	  Temp_Est_tsk=i;
	  TCOMP_Update_1sec();
	}
	for(i=0;i<512;i++)
	{
	  Temp_Est_tsk=0;
	  TCOMP_Update_1sec();
	}
	for(i=0;i<=1024;i++)
	{
	  Temp_Est_tsk=i;
	  TCOMP_Update_1sec();
	}
}
/****************************************************************************
Module for adapting the temperature compensation coefficients to provide
dynamic temperature correction. The goal is to achieve a "small signal"
sensitivity coefficient for both positive an negative slew oven current.
This correction is well suited to the normal small thermal cycles in an office
applications. The coefficient estimates are permitted to slowly evolve with
time to follow longer term shifts in the operation temperature of the office.

*****************************************************************************/
void ATC_Update(void)
{
	double Tslew,Fslew,Fin;
	unsigned short Vfreq , mode;
	/******* local input message data *****/
	double    YroavgA_rec,YroavgB_rec,Ytpavg_rec,Temp_Est_rec;  /*_rec indicated received data*/
	unsigned short TP_Reg_Events_rec;
	unsigned short ROA_Reg_Events_rec;
	unsigned short ROB_Reg_Events_rec;
	unsigned short TP_Slew_Update_rec;
	struct MSG_EST_ATC *ptprec;
	/**** receive 250 ms input message *****/
	if(MSG_EST_ATC_A.Ready) ptprec= &(MSG_EST_ATC_A);
	else ptprec= &(MSG_EST_ATC_B);
	YroavgA_rec=ptprec->YroavgA;
	YroavgB_rec=ptprec->YroavgB;
	YroavgA_tsk=YroavgA_rec;
	YroavgB_tsk=YroavgB_rec;
	Ytpavg_rec=ptprec->Ytpavg;
	Temp_Est_rec=ptprec->Temp_Est;
	Temp_Est_tsk=Temp_Est_rec;
	ROA_Reg_Events_rec=ptprec->ROA_Reg_Events;
	ROB_Reg_Events_rec=ptprec->ROB_Reg_Events;
	TP_Reg_Events_rec=ptprec->TP_Reg_Events;
	TP_Slew_Update_rec= ptprec->TP_Slew_Update;
	ptprec->Ready=FALSE;
	/* receive complete */
	/***** call ATC_Mode once per minute to what input to
			 correlate temperature against External or GPS *****/
	ATC.Min_Cnt++;
	if(ATC.Min_Cnt>59)
	{
	  ATC.Min_Cnt=0;
	  mode=ATC.Mode;
	  if(!(SReg.ROA_Reg&&0xBC)) ATC.Mode=ATCEXTAMODE;
	  else if((!(SReg.ROB_Reg&&0xBC))&&(smi.R0B_Select==EXTERNAL_B))ATC.Mode=ATCEXTBMODE;
	  else ATC.Mode=ATCGPSMODE;
	  if(mode!=ATC.Mode)
	  {
		 ATC_Mode(mode);
	  }
	}
#if(STATIC_TEMP)
	TCOMP_Update_1sec(); /*use static table based update*/
#else
	/****** decay temperature correction over long term****/
	smi.temp_state_cor*=0.999995;
	/***** apply adaptive temperature correction******/
	if(TP_Slew_Update_rec&&!(TP_Reg_Events_rec))
	{
		smi.temp_state_cor+=Ytpavg_rec*ATC.TC_avg;
	}
#endif
	/*update input smoothing filters */
	if(ATC.Mode==ATCGPSMODE)
	{
	  Vfreq=!(SReg.GPS_Reg&0x3F);
	  Fin=(GM.Ygps+smi.freq_state_cor+smi.phase_state_cor+ smi.temp_state_cor);
	}
	else if(ATC.Mode==ATCEXTAMODE)
	{
	  Vfreq=!(SReg.ROA_Reg&0x8C|ROA_Reg_Events_rec);
	  Fin=(YroavgA_rec+ smi.temp_state_cor);
	}
	else
	{
	  Vfreq=!(SReg.ROB_Reg&0x8C|ROB_Reg_Events_rec);
	  Fin=(YroavgB_rec+ smi.temp_state_cor);
	}
	if(!(SReg.TP_Reg&0x0C|TP_Reg_Events_rec)&&Vfreq&&(smi.BT3_mode!=GPS_Warmup))
	{
	 ATC.Stemp1= ATC.IFA*(double)(Temp_Est_rec) + ATC.IFB*ATC.Stemp1;
	 ATC.Sfreq1= ATC.IFA*(Fin)+ ATC.IFB*ATC.Sfreq1;
	 ATC.Acnt++;
	}
	else if(smi.BT3_mode==GPS_Warmup)
	{
	  ATC.Stemp1= Temp_Est_rec;
	  ATC.Sfreq1= Fin;
	  ATC.Ptemp=ATC.Stemp1;
	  ATC.Pfreq=ATC.Sfreq1;
	}
	if(ATC.Settle_Cnt)
	{
	  if(ATC.Acnt>ATC.ATAU)
	  {
		 ATC.Acnt=0;
		 ATC.Ptemp=ATC.Stemp1;
		 ATC.Pfreq=ATC.Sfreq1;
		 ATC.Settle_Cnt--;
	  }
	}
	if(ATC.Acnt>=ATC.ATAU&&(ATC.Settle_Cnt==0)&&(ATC.Mode!=ATCGPSMODE))
	{
		ATC.Acnt=0;
		Tslew= ATC.Stemp1-ATC.Ptemp;
		ATC.Ptemp=ATC.Stemp1;
		Fslew= ATC.Sfreq1-ATC.Pfreq;
		ATC.Pfreq=ATC.Sfreq1;
		if(Tslew>ATC.Tslew_Thres)
		{
			ATC.Stslew1_pos=ATC.TCFA*Tslew +    ATC.TCFB*ATC.Stslew1_pos;
			ATC.Stslew2_pos=ATC.TCFA*ATC.Stslew1_pos + ATC.TCFB*ATC.Stslew2_pos;
			ATC.Sfslew1_pos=ATC.TCFA*Fslew +    ATC.TCFB*ATC.Sfslew1_pos;
			ATC.Sfslew2_pos=ATC.TCFA*ATC.Sfslew1_pos + ATC.TCFB*ATC.Sfslew2_pos;
			ATC.TC_pos= ATC.Sfslew2_pos/ATC.Stslew2_pos;
			if(ATC.Slew_Bucket<LBUCKETMAX) ATC.Slew_Bucket += ATCSLEWUP;
		}
		else if(Tslew<-ATC.Tslew_Thres)
		{
			ATC.Stslew1_neg=ATC.TCFA*Tslew +    ATC.TCFB*ATC.Stslew1_neg;
			ATC.Stslew2_neg=ATC.TCFA*ATC.Stslew1_neg + ATC.TCFB*ATC.Stslew2_neg;
			ATC.Sfslew1_neg=ATC.TCFA*Fslew +    ATC.TCFB*ATC.Sfslew1_neg;
			ATC.Sfslew2_neg=ATC.TCFA*ATC.Sfslew1_neg + ATC.TCFB*ATC.Sfslew2_neg;
			ATC.TC_neg= ATC.Sfslew2_neg/ATC.Stslew2_neg;
			if(ATC.Slew_Bucket<LBUCKETMAX) ATC.Slew_Bucket += ATCSLEWUP;
		}
		else
		{
		  if(ATC.Slew_Bucket>LBUCKETMIN) ATC.Slew_Bucket -= ATCSLEWDOWN;
		  if(ATC.Slew_Bucket<LBUCKETTHRES)
		  {
			  if(ATC.Tslew_Thres>ATCMINSLEW) ATC.Tslew_Thres *=0.8;
			  ATC.Slew_Bucket=LBUCKETMIN+LBUCKETTHRES;
		  }
		}
		ATC.TC_avg=(ATC.TC_neg+ATC.TC_pos)/2.0;
		/*** update ram calibration data ***/
		Cal_Data.ATC_TC_Cal= ATC.TC_avg;
		if(ATC.Slew_Bucket>=LBUCKETMAX)
		{
			  if(ATC.Tslew_Thres<ATCMAXSLEW) ATC.Tslew_Thres *=1.25;
			  ATC.Slew_Bucket=LBUCKETMIN+LBUCKETTHRES;
		}
/******
		#if PRINTLOG
		  if(Tslew >ATC.Tslew_Thres)
		  {
			if(pcount<PRINTMAX)
			{
				 pcount++;
				 printf("%s%5ld:ATC_Update (POS):AFslew:%10.3le :: ATslew:%10.3lf\n",PHEAD,ptime,ATC.Sfslew2_pos,ATC.Stslew2_pos);
			}
		  }
		  else if (Tslew<-ATC.Tslew_Thres)
		  {
			if(pcount<PRINTMAX)
			{
				 pcount++;
				 printf("%s%5ld:ATC_Update (NEG):AFslew:%10.3le :: ATslew:%10.3lf\n",PHEAD,ptime,ATC.Sfslew2_neg,ATC.Stslew2_neg);
			}
		  }
		  if(pcount<PRINTMAX)
		  {
				 pcount++;
				 printf("%s%5ld:ATC_Update::SlewBucket:%5d::Thres:%5.2f\n",PHEAD,ptime,ATC.Slew_Bucket,ATC.Tslew_Thres);
		  }
		#endif
	**********/
	}
}
/*****************************************************************************
This module is called every minute from ATC_update directly.
If a remote oscillator input is present and valid then
it sets up the ATC algorithm for external calibration
operation.
****************************************************************************/
void ATC_Mode(unsigned short mode)
{
	if(mode!=ATC.Mode) /*shift input to ATC calibration*/
	{
	  #if PRINTLOG
	  if(pcount<PRINTMAX)
	  {
			pcount++;
			printf("%s%5ld:ATC_Mode MODE CHANGE FROM:%5d to:%5d\n",PHEAD,ptime,mode,ATC.Mode);
	  }
	  #endif
	  ATC.Acnt=0;
	  ATC.Settle_Cnt=3; /*skip first 3 updates to settle freq filter*/
	  ATC.Pfreq=0.0;
	  if(ATC.Mode==ATCGPSMODE)
	  {
		ATC.Sfreq1=(GM.Ygps+smi.freq_state_cor+smi.phase_state_cor+ smi.temp_state_cor);
		ATC.ATAU=ATCUPDATETAU;
		ATC.IFA = 1.0/ATC.ATAU; ATC.IFB= 1.0-ATC.IFA;
		ATC.TCFA=ATC.ATAU;
		ATC.TCFA*= 1.0/ATCSMOOTHTAU;
		ATC.TCFB=1.0-ATC.TCFA;
	  }
	  else
	  {
		if(ATC.Mode==ATCEXTAMODE)ATC.Sfreq1=(YroavgA_tsk+ smi.temp_state_cor);
		else ATC.Sfreq1=(YroavgB_tsk+ smi.temp_state_cor);
		ATC.ATAU=ATCUPDATETAU/2; /*accelerate by 2 for external calibration*/
		ATC.IFA = 1.0/ATC.ATAU; ATC.IFB= 1.0-ATC.IFA;
		ATC.TCFA= ATC.ATAU;
		ATC.TCFA*= 10.0/ATCSMOOTHTAU; /*accelerate by 10 for noise smoothing*/
		ATC.TCFB=1.0-ATC.TCFA;

	  }
	}/*end if mode change*/
}



/***************************************************************************
Module for updating Phase Compensation. Algorithm handles lead/lag info
from FPGA to slowing steer output 1PPS onto noisy GPS 1PPS.
***************************************************************************/   
static long int ppsreg;
void PCOMP_Update_1sec(void)
{
	 short cur_meas;
	 unsigned short no_cross_thres;
	 double phasecor;
//	 double max_slew;  
	 double ext_slew;
     double slew_delta;
	 /******Update current phase measurement data *********/   
 
	 ppsreg = Get_GPS_Phase();    
	 if((RGPD.handle != 1)&&(RGPP.handle != 1))  //GPZ NOV 2010 added some dither when there is no sawtooth
	 {
	  if(smi.One_PPS_Lead==1) // previous update was positive
	  {
	    ppsreg--;
	    if (ppsreg==0)
	    	ppsreg--;
	  }
	  else
	  {
	    ppsreg++;
	    if (ppsreg==0)
	    	ppsreg++;
	  }
	 }
	 if(ppsreg&0x80000000) smi.One_PPS_Range=1;
	 else smi.One_PPS_Range =0;  
	 ppsreg= ppsreg&0x7FFFFFFF;
	 if(ppsreg&0x40000000)
	 {                
	 /**** take complement***/  
	  ppsreg |= 0x80000000;
	  smi.One_PPS_Mag=(unsigned int)((-ppsreg));//eliminate divide by 2 GPZ May 2008 for TP5000 
	  smi.One_PPS_Lead=0;
//	  smi.One_PPS_Lead=1;//try the other way
	 }
	 else
	 {
	  smi.One_PPS_Mag= (unsigned int)((ppsreg));  //eliminate divide by 2 GPZ May 2008 for TP5000 
	  smi.One_PPS_Lead=1;
//	  smi.One_PPS_Lead=0;   //try the other way
	 }        
    Phase_Loop_Print(); /*for debug TODO comment out for release*/                               

	/****Only update if 1PPS error is within range  and ****/
	/*perform one time jam of ONE PPS output when freq calibration
	  reaches transition from fast time constant to medium time c*/
#if 0	 
 	 {
		if(OnePPSJamFlag)
		{
		  smi.One_PPS_High=FALSE;
		  Pcomp.acquire_flag=TRUE;                                            
		  GM.GPS_clock_bias=smi.clock_bias; /*jam initial phase estimate*/
		}
	 }
	 /*GPZ enforce only one jam for varactor based control system Nov 2001*/
//	 else if(Lloop.Cal_Mode==2&&(!OnePPSJamFlag))
	 else if(Lloop.Cal_Mode<8&&(!OnePPSJamFlag)) /* force use of jam mode all the time*/
	 {
		 smi.One_PPS_Jam=TRUE;   
		 Align_GPS_Phase();/*call system function to request a jam*/
		 OnePPSJamFlag=60;                      /* only jam once a minute */
		 Pcomp.acquire_flag=FALSE;
	 }
	}                     
	
	 PCOMP_Validate_1sec();
#endif	

	if(phase_control&&smi.One_PPS_Range==0&&
	(!(SReg.GPS_Reg&0x3F))&&GPS_Valid&&
	((TOD_Mode==6)||(TOD_Mode==0))&&
	(RGPD.offset<255)&&(RGPD.status>0)&&(RGPD.status<3))  //GPZ June 2010 allow time control in GPS and hybrid mode only

//	if(phase_control&&smi.One_PPS_Range==0&&(!(SReg.GPS_Reg&0x3F))&&GPS_Valid&&(trace_source==3))
	{
	   if(LTBI.lotype==RB)
	   {
		 max_slew= (8.0-Lloop.Cal_Mode);
		 /* Added adjustable threshold to assist phase control during
			 Rb warmup*/
         no_cross_thres=LTBI.nocrossingcnt/max_slew;
		 max_slew *= max_slew/2.0;
		 max_slew *= LTBI.maxphaseslew;
		}
	   else
	   {       
	   	if(Lloop.Cal_Mode<4) 
	   	{
	   				Pcomp.no_crossing_cnt=0;
  					max_slew= LTBI.minphaseslew; 
  		}			
	   	else 
	   	{
	   	    if(max_slew<LTBI.maxphaseslew) max_slew = .999* max_slew + 0.001 * LTBI.maxphaseslew;
		}	
        no_cross_thres=LTBI.nocrossingcnt;
	   }
	   
		if(smi.One_PPS_Lead) cur_meas=-1;
		else cur_meas=1;
		
		if(Pcomp.acquire_flag)
		{
		  Pcomp.attack_rate=max_slew;
		  Pcomp.prev_meas=cur_meas;
		  Pcomp.acquire_flag=FALSE;
		}
		if(cur_meas!=Pcomp.prev_meas)
		{
			Pcomp.no_crossing_cnt=0;
			if(Pcomp.attack_rate>LTBI.minphaseslew)
			{
			  Pcomp.attack_rate = Pcomp.attack_rate*0.8;
			  if(Pcomp.attack_rate<LTBI.minphaseslew) Pcomp.attack_rate=LTBI.minphaseslew;
			  Pcomp.tgain1 = 1.0;
			  Pcomp.tgain2 = 1.0-Pcomp.attack_rate*20.0e6;
			}
		} /*end zero crossing detected*/
		if((Pcomp.no_crossing_cnt++)>no_cross_thres)
		{
			Pcomp.no_crossing_cnt=0;
			if(Pcomp.attack_rate<max_slew&&(Lloop.Cal_Mode<8))
			{
			  Pcomp.attack_rate = Pcomp.attack_rate*2.0;
			  if(Pcomp.attack_rate>max_slew) Pcomp.attack_rate=max_slew;
			  Pcomp.tgain1 = 1.0;
			  Pcomp.tgain2 = 1.0-Pcomp.attack_rate*20.0e6;
			  #if PRINTLOG
			  if(pcount<PRINTMAX)
			  {
				 pcount++;
				 printf("%s%5ld:Pcomp_Update:No Crossing Detected new slew:%20.8le\n",PHEAD,ptime,Pcomp.attack_rate);
			  }
			  #endif
			}
		} /*end period of no zero crossing detected*/   
		phasecor= Pcomp.attack_rate*smi.One_PPS_Mag;
		if(phasecor==0.0) phasecor=Pcomp.attack_rate;
		else if(phasecor>max_slew) phasecor=max_slew;
		/**** determine extended slew rate if any ****/
		ext_slew=0.0;
		if(phase_control==2)
		{
		    if( smi.One_PPS_Mag >38)
		    {
				if(	smi.One_PPS_Mag >375)
				{   
					ext_slew=15.0;
				}	    
		        else
		        { 
		        	ext_slew=(0.0115*(double)(smi.One_PPS_Mag-38));  
		        	ext_slew*=ext_slew;
		        }
		    }
		    else ext_slew=0.0;
			phasecor += (ext_slew*1.0e-9);
		} 
		
		phasecor*=(double)(cur_meas);
		Pcomp.time_err_est = Pcomp.tgain2*Pcomp.time_err_est
								  +Pcomp.tgain1*phasecor;
		/*** generate estimate of gps tdev noise at crossover time for Lloop**/
		Pcomp.tdev_crossover=sqrt(ECD.GPS_mvar * GPS_PM_CONV/Lloop.Prop_Tau);
		Pcomp.TFOM=2.0*Pcomp.tdev_crossover;
		if(Pcomp.time_err_est>0.0)Pcomp.TFOM += Pcomp.time_err_est;
		else Pcomp.TFOM -= Pcomp.time_err_est;
		if((smi.Input_Mode&0xFF)!=SPAN_ONLY&&(smi.Input_Mode&0xFF)!=RO_ONLY)
		{
		/*** smoothly apply new frequency to not impact short term stability***/  
		 slew_delta=phasecor-smi.phase_state_cor;
	     if(slew_delta<-(2.0*LTBI.minphaseslew) || slew_delta>(2.0*LTBI.minphaseslew) ) 
	 	 {
			smi.phase_state_cor+= .05*slew_delta;   // was 0.05 GPZ Aug 2010 return from 0.01 back to 0.05
	  	 }
	  	 else  smi.phase_state_cor+= slew_delta;
		}
		else
		{
		     smi.phase_state_cor=0.0;
		} 
	} /*end 1PPS in range*/
	else 
	{
// GPZ JUNE 2010 Change behavior do not bleed out never use phase control unless tracessource is 3
	  smi.phase_state_cor=0;
#if 0	  		
	  smi.phase_state_cor=.95*smi.phase_state_cor;
	  if(smi.phase_state_cor>-LTBI.minphaseslew && smi.phase_state_cor<LTBI.minphaseslew )
	  {
	     smi.phase_state_cor=0.0;
	  }    
#endif	  
	} 
	if(phase_skip>0)    //GPZ June 2010 added phase skip blanking period 
	{
	 	phase_skip--;
		smi.phase_state_cor=0.0;
	}
	/****TEST FORCE NO phase control*****/
//	smi.phase_state_cor=0.0;
	Pcomp.prev_meas=cur_meas;
	/******* generate 1 second message from PCOMP update task to Estimation ********/
	ppcompestmsg=&(MSG_PCOMP_EST_A);
	ppcompestmsg->phase_state_cor=smi.phase_state_cor;
	ppcompestmsg->Ready=TRUE;
	/*************end message generation *********************/
	/********* Update Timing Status MailBox**/
	MSG_TSR.One_PPS_High= smi.One_PPS_High;
	MSG_TSR.PPS_Range_Alarm= (Pcomp.PPS_Range_Bucket<LBUCKETTHRES)?1:0;
	MSG_TSR.Phase_State_Correction= (float)smi.phase_state_cor;
	MSG_TSR.Time_Err_Est 	= (float)Pcomp.time_err_est;
	MSG_TSR.TDEV_Crossover	= (float)Pcomp.tdev_crossover;
	MSG_TSR.TFOM 				= (float)Pcomp.TFOM;
	if(MSG_TSR.PPS_Range_Alarm)
	 MSG_TSR.Time_Err_Meas=(float)(MAX_PPS_RANGE*cur_meas)*5.0e-8;
	else
	MSG_TSR.Time_Err_Meas 	= (float)(smi.One_PPS_Mag)*5.0e-8;
//	MSG_TSR.Time_Err_Meas 	= ((float)smi.One_PPS_phase)*COUNTER_RES;
	MSG_TSR.Ready=TRUE;
}
/***************************************************************************
Module for validity of One PPS output and input data and manage appropriate
actions. This module will run on both the GPS and OCXO module.
The sequence of execution is important. On the GPS side this module
should be called just after the MCCI input message is received. The
message will have last measured 1pps data.The OCXO module should
take the measurement after the 1 PPS interrupt  but before sending
the MCCI message. Likewise the GPS module should update the local
1 PPS measurement before the module is called. The resulting verified
local data shall be passed to the MCCI module on the outgoing
message. The OCXO module shall call the 1PPS update routine after
it receives the verified data.
***************************************************************************/
void PCOMP_Validate_1sec(void)
{
  short rvalid,lvalue,lmag,rvalue,diff;
  rvalid=FALSE;
  if(smi.One_PPS_Range==0&& GPS_Valid&&(!(SReg.GPS_Reg&0x3F)))
  {
		smi.LO_PPS_Data_Valid=TRUE;
		if(smi.One_PPS_Lead)
		{
		 	lvalue=smi.One_PPS_Mag;
		}
		else 
		{
			lvalue=-smi.One_PPS_Mag;
		}
  }
  else if(GPS_Valid&&(!(SReg.GPS_Reg&0x3F))) 
  {
  	lvalue=MAX_PPS_RANGE;
  }
  
  lmag=lvalue;
  if(lvalue<0) 
  {
  	lmag=-lmag;
  }
  
  if(smi.RO_PPS_Data_Valid&&GPS_Valid)
  {
	rvalid=TRUE;
	rvalue=smi.One_PPS_Est_RO;
  }
  
  if(rvalid) { /*can use remote data in validation*/
	  diff=lvalue-rvalue;
	  if(diff<0)diff=-diff;
	  if(diff<MAX_PPS_NORMAL&&lmag>MAX_PPS_NORMAL); /*Common GPS Hit detected*/
	  else {
		Pcomp.prev_error_est=Pcomp.cur_error_est;
		Pcomp.cur_error_est=lmag;
	  }
  }
  else
  {	/*need to base decision on local data only*/
	 if(GPS_Valid&&(!(SReg.GPS_Reg&0x3F)))
	 {
		Pcomp.prev_error_est=Pcomp.cur_error_est;
		Pcomp.cur_error_est=lmag;
	 }
  } 

  /*********** Manage One_PPS_High Decision ****************/
  if(GPS_Valid&&(!(SReg.GPS_Reg&0x3F))) {
	 if(smi.BT3_mode==Holdover) {
		if(lmag<MAX_PPS_FLYWHEEL)smi.One_PPS_High=FALSE;
		else smi.One_PPS_High=TRUE;
	 }
	 else if(smi.BT3_mode==GPS_Normal){
	  if(lmag<MAX_PPS_SINGLE) smi.One_PPS_High=FALSE;
	  else smi.One_PPS_High=TRUE;
	  if(lmag<MAX_PPS_NORMAL||Pcomp.prev_error_est<MAX_PPS_NORMAL)	smi.One_PPS_High=FALSE;
	 }
	 else if(lmag<MAX_PPS_SINGLE && Lloop.Cal_Mode>1) smi.One_PPS_High=FALSE;
	 /***********Manage Decision to Jam Output *******/
	 if((smi.BT3_mode!=GPS_Warmup) && (Holdover_Recovery_Jam>0) )
	 {
		 Holdover_Recovery_Jam--;
		 /****** Force a jam after extended holdover if error is 500ns*/
		 if((lmag>=10)&&(Pcomp.prev_error_est>=10))
/* 	 if((lmag>=MAX_PPS_NORMAL)&&(Pcomp.prev_error_est>=MAX_PPS_NORMAL))*/
		 {
			if(Pcomp.PPS_Range_Bucket> LBUCKETMIN )
			{
				Pcomp.PPS_Range_Bucket -= PPSRANGEBUCKETDOWN;
			}
			if(Pcomp.PPS_Range_Bucket<LBUCKETTHRES)
			{
				Holdover_Recovery_Jam=0;
				Pcomp.PPS_Range_Bucket = LBUCKETMAX;
				smi.One_PPS_Jam=TRUE;
				/* Pcomp.acquire_flag=TRUE; */
				Pcomp.attack_rate*=4.0;
				if(Pcomp.attack_rate>LTBI.maxphaseslew)
				{
					Pcomp.attack_rate=LTBI.maxphaseslew;
				}
				Pcomp.tgain1 = 1.0;
				Pcomp.tgain2 = 1.0-Pcomp.attack_rate*20.0e6;
			}
		 }
		 else
		 {
			if(Pcomp.PPS_Range_Bucket< LBUCKETMAX )Pcomp.PPS_Range_Bucket += PPSRANGEBUCKETUP;
		 }
	 }
	/*********** end 1 PPS jam decision ************/
  }
  else if(smi.BT3_mode==Holdover) smi.One_PPS_High=FALSE;
}
/******************************************************************************
MCCA tasks to be called once per second
*******************************************************************************/
void BT3_1Sec_Tasks()
{
	double dtemp;
	double GPS_Delta ,GPS_Mag_Delta;
	double GPS_Freq_Delta, GPS_Phase_Delta;   
	long ltemp;
	/**********************************************
	  process one second GPS frequency estimates
	***********************************************/
	/***** The clock bias state from the Kalman is used as the
	best estimate of incremental code time offset it is nominally
	expressed in seconds.
	**********/
	// Code for case when Trace Source is GPS  
	//GPZ June 2010 Always get RGP message 
		Get_RGP(&RGPD); 
		RGP_Indx=(RGP_Indx+1)&0x7;   
		RGPP= RGPH[RGP_Indx];
		RGPH[RGP_Indx]=RGPD;
		ltemp= RGPD.offsetTime - RGPP.offsetTime;   
		if(ltemp<1&&RGP_Freeze<100) RGP_Freeze++; // possibly frozen PPC communications
		else RGP_Freeze=0;

		if(trace_source==TRACE_GPS)
		{ 
		
			GPS_Delta= OneSecAvg[trace_source];    
			smi.time_acc_est=0; 
			smi.clock_bias_flag=0;	
			/***** control clock bias flag via PPC GPS information ****/	
//			Get_RGP(&RGPD); 
//			RGP_Indx=(RGP_Indx+1)&0x7;   
//			RGPP= RGPH[RGP_Indx];
//			RGPH[RGP_Indx]=RGPD;
//			ltemp= RGPD.offsetTime - RGPP.offsetTime;   
//			if(ltemp<1&&RGP_Freeze<100) RGP_Freeze++; // possibly frozen PPC communications
//			else RGP_Freeze=0;
			if((!IMC_flag)||((ltemp>-1 && ltemp <10)&&(RGP_Freeze<3))) /* only accept normally sequenced data*/
			{
				if((!IMC_flag)||(RGPD.offset<255)&&(RGPD.status>0)&&(RGPD.status<3)) 	
				{
					if(cts_reg==0) smi.clock_bias_flag=1;	
					cts_reg_latch= cts_reg;	
				} /*RGP GPS valid logic*/
				/* normal sequence logic*/ 
				else 
				{ 
					// 4/17/2009 GZ says so. 
					GM.Gdrift_bucket = 0;
				}
			} 
 			if(cts_reg_latch&0x8) Reset_RGP_Fifo(); /*keep Fifo in reset until good data*/ 
			/****** add sawtooth compensation ********/
			// calculate unmatched delta
			if(smi.clock_bias_flag==1&&(IMC_flag))
			{
				if((RGPD.status>0)&&(RGPP.status>0))   
				{
					stcomp=(double)(RGPD.offset - RGPP.offset)*(double)(1e-9/8.0);
					stcompsmooth=((double)(SFREQTAU-1)*stcompsmooth + stcomp)/(double)(SFREQTAU);  
					GPS_Delta =GPS_Delta-stcompsmooth;   // try different sign
				}				
			} 
			/****** end sawtooth compensation code *****/     
        }  
        else //trace source is NOT GPS
        {
	 		/******* select which measurement channel is source of traceabilty default is GPS ****/
			GPS_Delta= OneSecAvg[trace_source];    
			smi.time_acc_est=0; 
			smi.clock_bias_flag=0;	
			if(trace_source==0)
			{
				if(SReg.SPA_Reg==0) smi.clock_bias_flag=1;	
				else  Lloop.Trace_Skip=180; //GPZ Aug 2010 secure protection during transient    
				cts_reg_latch= SReg.SPA_Reg;	
			} 
			else if(trace_source==1)
			{
				if(SReg.SPB_Reg==0) smi.clock_bias_flag=1;	
				else  Lloop.Trace_Skip=180; //GPZ Aug 2010 secure protection during transient    
				cts_reg_latch= SReg.SPB_Reg;	
			}   
			else if(trace_source==2)
			{
				if(SReg.ROA_Reg==0) smi.clock_bias_flag=1;		
				else  Lloop.Trace_Skip=180; //GPZ Aug 2010 secure protection during transient    
				cts_reg_latch= SReg.ROA_Reg;
			}   
        }
		GPS_Freq_Delta= GPS_Delta - GM.GPS_freq_prev;
		GM.GPS_freq_prev = GPS_Delta;
		if(GPS_Drift_Count>0)
		{
			// GZ: 7/2/2009 Take off Threshhold
			if(/*(smi.time_acc_est <= TIME_ACC_THRES)&& */(smi.clock_bias_flag!=0))
			{
			  GPS_Drift_Count--;
			  GPS_Freq_Delta=0;
			  GM.GPS_clock_bias=smi.clock_bias; /*jam initial phase estimate*/
			}
		}
		GPS_Mag_Delta=GPS_Delta;
		if(GPS_Mag_Delta <0) GPS_Mag_Delta=-GPS_Mag_Delta;
		if(GPS_Freq_Delta < 0) GPS_Freq_Delta = -GPS_Freq_Delta;
		/* accumulate if data is good current A= R= alarms */
		GPS_Valid=TRUE;
		// GZ: 7/2/2009 Take off Threshhold
		if(/* (smi.time_acc_est > TIME_ACC_THRES)|| */(smi.clock_bias_flag==0))
		{
			 GPS_Valid=FALSE;
			 if (GM.Gfreq_bucket >LBUCKETMIN) GM.Gfreq_bucket -= GFREQBUCKETDOWN;
			/* if (GM.Gdrift_bucket >LBUCKETMIN)GM.Gdrift_bucket -= GDRIFTBUCKETDOWN;*/
		}  
		if(Get_Gear()>1)
		{
			if(GPS_Mag_Delta > TM.Gfreqmax)
			{
				GPS_Valid=FALSE;
				if (GM.Gfreq_bucket >LBUCKETMIN) GM.Gfreq_bucket -= GFREQBUCKETDOWN;
			}
			else if(GPS_Freq_Delta > TM.Gdriftmax)
			{
				GPS_Valid=FALSE;
				if (GM.Gdrift_bucket >LBUCKETMIN)GM.Gdrift_bucket -= GDRIFTBUCKETDOWN;
			}
		}
		if(GPS_Valid) /*good sample*/
		{
			if (GM.Gfreq_bucket <LBUCKETMAX) GM.Gfreq_bucket += GFREQBUCKETUP;
			if (GM.Gdrift_bucket <LBUCKETMAX)GM.Gdrift_bucket += GDRIFTBUCKETUP;
			GM.Ygps=GPS_Delta;
		}
		else  GM.Ygps= smi.freq_state_cor + smi.phase_state_cor;   /*Freeze loop input */
	  GM.GPS_clock_bias-=GM.Ygps;
#if (ANTENNA_MODE == WINDOW)
//		if(GPS_Valid)
//		{
//		  GPS_Phase_Delta= GM.GPS_clock_bias-smi.clock_bias;  
//		  if((GPS_Phase_Delta>MAX_PPS_BIAS)||(GPS_Phase_Delta<-MAX_PPS_BIAS))
//		  {
//			  GPS_Drift_Count=3;                                                     
//			  GM.GPS_clock_bias=smi.clock_bias;
//		  }
//		  else
//		  {
//			  GM.GPS_clock_bias+=GM.Ygps;
//			  GM.Ygps += GPS_Phase_Delta*0.1; /*was .001 11-2001 GPZ*/
//			  GM.GPS_clock_bias-=GM.Ygps;
//		  }
//		}
#endif
		// GZ: 7/2/2009 Take off Threshhold
		if(0/*smi.time_acc_est > TIME_ACC_THRES*/)
		 {
				if(pcount<PRINTMAX&&ASMM.GPS_Acc_Flag)
				{
					ASMM.GPS_Acc_Flag=FALSE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:BT3_1sec_task:EVENT-GPS_Time_Inaccuracy\n",PHEAD,ptime);
					#endif
				}
		 }
		 else
		 {
				if(pcount<PRINTMAX&&(ASMM.GPS_Acc_Flag==FALSE))
				{
					ASMM.GPS_Acc_Flag=TRUE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:BT3_1sec_task:CLEAR-GPS_Time_Inaccuracy\n",PHEAD,ptime);
					#endif
				}
		 }
		 if(smi.clock_bias_flag==0)
		 {
				if(pcount<PRINTMAX && ASMM.GPS_Bias_Flag)
				{
				  ASMM.GPS_Bias_Flag=FALSE;
				  #if PRINTLOG
				  pcount++;
				  printf("%s%5ld:BT3_1sec_task:EVENT-GPS_Bias_Flag_Invalid\n",PHEAD,ptime);
				  #endif
				}
		 }
		 else
		 {
				if(pcount<PRINTMAX && (ASMM.GPS_Bias_Flag==FALSE))
				{
				  ASMM.GPS_Bias_Flag=TRUE;
				  #if PRINTLOG
				  pcount++;
				  printf("%s%5ld:BT3_1sec_task:CLEAR-GPS_Bias_Flag_Invalid \n",PHEAD,ptime);
				  #endif
				}
		 }
		 if(GPS_Mag_Delta > TM.Gfreqmax)
		 {
			if(pcount<PRINTMAX && ASMM.GPS_Freq_Flag)
			{
				ASMM.GPS_Freq_Flag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:BT3_1sec_task:EVENT-GPS_Mag_Freq\n",PHEAD,ptime);
				#endif
			}
		 }
		 else
		 {
			if(pcount<PRINTMAX && (ASMM.GPS_Freq_Flag==FALSE))
			{
				ASMM.GPS_Freq_Flag= TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:BT3_1sec_task:CLEAR-GPS_Mag_Freq\n",PHEAD,ptime);
				#endif
			}
		 }
		 if(GPS_Freq_Delta > TM.Gdriftmax)
		 {
			if(pcount<PRINTMAX && ASMM.GPS_Drift_Flag)
			{
				ASMM.GPS_Drift_Flag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:BT3_1sec_task:EVENT-GPS_Mag_Drift\n",PHEAD,ptime);
				#endif
			}
		 }
		 else
		 {
			if(pcount<PRINTMAX && (ASMM.GPS_Drift_Flag==FALSE))
			{
				ASMM.GPS_Drift_Flag= TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:BT3_1sec_task:CLEAR-GPS_Mag_Drift\n",PHEAD,ptime);
				#endif
			}
		 }
/*		timing_stat_report(smi.timing_stats,smi.Timing_Chan,smi.Timing_Stat);*/
		/*TCOMP_Update_1sec(); /*call temperature compensation 1 sec task*/
		/********************************************************
		call shared memory override
		*********************************************************/
		if(smi.Overide_Mode) {
		  Shared_Mem_Override();
		  if(smi.Overide_Mode==1)smi.Overide_Mode=0;
		}
      Command_Mgr(); /*call input command manager*/
		/*************call adaptive state machine ***************/
		ASM();
}  
/******************************************************************************
Function call to set the source of traceability for the BesTime engine
TRACE_SPA =0,
TRACE_SPB =1,
TRACE_PRR =2,
TRACE_GPS =3
default is TRACE_GPS (3)....
******************************************************************************/
void set_trace_source(unsigned int tsource)
{      
	if(tsource<4)trace_source=tsource; 
	else  trace_source= TRACE_GPS;   
	if(tsource==3) IMC_flag=1;
	else  IMC_flag=0;     
   	Lloop.Trace_Skip=180; //GPZ June 2010 up from 30 to purge transients  KEEP ABOVE 110 seconds    
   	phase_skip= 240; //GPZ June 2010 allow 2 minutes to settle freq   
   	if(trace_source == TRACE_GPS) //GPZ June 2010 use acquire phase operation when returing to GPS
   	{
		Pcomp.acquire_flag=TRUE;      
	}	
}   
/******************************************************************************
Function call to set the source Time of Day Mode for the BesTime engine  
GPZ June 2010 Added for future hybrid mode support

******************************************************************************/
void set_TOD_mode(unsigned int tsource)
{     
	TOD_Mode= tsource; 
}

/******************************************************************************
Function call to set the PPS time align mode

******************************************************************************/
void set_phase_control(unsigned int mode)
{      
	if(mode)
	{
		phase_control=mode;   
		Pcomp.acquire_flag=TRUE;

	}
	else  
	{
	 phase_control=0; 
	 smi.phase_state_cor=0.0;   

	}
	
}     
/*****************************************************************
Time Provider Phase Control Debug Print Function
******************************************************************/
void Phase_Loop_Print()
{  
		char cbuf1[120];  
		phase_print=(phase_print+1)%16; 
//	    sprintf((char *)cbuf1,"Phase_Loop_Print:%4d\r\n",phase_print);    
//		DebugPrint(cbuf1, 9);     
		
		if(phase_print==2)
		{
		    sprintf((char *)cbuf1,"Phase_LoopA: RGPTime: %7ld, tsource: %4d, RGPoff:%4d, RGPst: %4d, biasflg: %4d, creg: %4x\r\n",    
    		RGPD.offsetTime,
     		(int) trace_source, 
			(int) RGPD.offset,
			(int)RGPD.status, 	
     		(int)smi.clock_bias_flag,   
	     	cts_reg_latch
		     );
			DebugPrint(cbuf1, 9);     
		}
		else if(phase_print==4)
		{
		    sprintf((char *)cbuf1,"Phase_LoopB: Drift_bkt: %4d, sawcomp ppt: %7ld, Ygps ppt:%7ld, phase_ctrl: %4d TOD_src: %4d\r\n",    
    		(int)GM.Gdrift_bucket, 
     		(long int)(stcompsmooth*(double)(1.0e12)),
	     	(long int)(GM.Ygps*(double)(1.0e12)),  
    	 	(int)(phase_control), 
    	 	(int)TOD_Mode
   	    	);
			DebugPrint(cbuf1, 9);   
		}
		else if(phase_print==6)
		{
		    sprintf((char *)cbuf1,"Phase_LoopC: PPSRange: %4d,PPSLead: %4d,PPSMag: %4ld, PPSJam:%4d,pstate ppt: %7ld, preg:%lx\r\n",    
	    	(int)(smi.One_PPS_Range),
		    (int)(smi.One_PPS_Lead),
			(long int)(smi.One_PPS_Mag),
			(int)(smi.One_PPS_Jam), 
			(long int)(smi.phase_state_cor*1e12),   
		    (long int) ppsreg
   	    	); 
			DebugPrint(cbuf1, 9); 
		} 
		
		
}
/******************************************************************************
Task to interpolate GPS clockbias data between updates
*****************************************************************************/
#if 0
void GPS_interp()
{
	/*generate interpolated clock bias*/
	if(GPS_Drift_Count>0)
	{
		if((smi.time_acc_est <= TIME_ACC_THRES)&&(smi.clock_bias_flag!=0))
		{
			  GM.inter_cbias=smi.clock_bias; /*jam initial phase estimate*/
		}
	}
	if(GM.gpssec==get_ticks())
	{
		 if((++GM.uptime)>10)
		 {
			smi.clock_bias_flag=0; /*force false clock data*/
			GM.uptime=10;
		 }
		 else	GM.inter_cbias+=GM.inter_inc;
	}
	else
	{
	  GM.gpssec=get_ticks();
	  if(GM.uptime)
	  {
		 GM.inter_inc=(smi.clock_bias-GM.inter_cbias)/GM.uptime;
		 GM.uptime=1;
     }
	  else GM.inter_inc=0.0;
	  GM.inter_cbias+=GM.inter_inc;
	}      
	smi.clock_bias=GM.inter_cbias;
}
#endif  

