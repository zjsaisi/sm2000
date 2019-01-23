/***************************************************************************
$Header: bt3_est.c 1.9 2010/10/13 23:59:29PDT Jining Yang (jyang) Exp  $
$Author: Jining Yang (jyang) $
$Date: 2010/10/13 23:59:29PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_est.c_v  $
$Modtime:   06 Feb 1999 12:36:38  $
$Revision: 1.9 $


                            CLIPPER SOFTWARE
                                 
                                BT3_EST.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module performs suite of 250 msec tasks to estimate and
                  manage input frequency and temperature states.

MODIFICATION HISTORY:

$Log: bt3_est.c  $
Revision 1.9 2010/10/13 23:59:29PDT Jining Yang (jyang) 
Merged from 1.8.1.1.
Revision 1.8.1.1 2010/08/04 23:24:13PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.8 2009/10/02 09:32:57PDT Zheng Miao (zmiao) 
George's change to fix pulling E1 problem.
Revision 1.7 2009/07/20 11:24:14PDT zmiao 
George's change
Revision 1.5 2009/03/26 09:36:52PDT zmiao 
Change some constants according to George Z.
Revision 1.4 2009/01/30 15:51:26PST zmiao 
Fix BT3
Revision 1.3 2008/05/27 08:40:13PDT gzampetti 
Changed synth LSB from 2.7e-11 to 1.7e-12
Revision 1.2 2008/04/01 09:15:01PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.1 2007/12/06 11:41:05PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.25 2006/07/28 16:29:33PDT gzampet 
mpll measurement fixed
Revision 1.24 2006/07/28 09:18:24PDT gzampet 
add synthesizer control
Revision 1.23 2006/05/11 17:05:47PDT gzampet 
lower set_gps_drift bucket threshold to extend protection time
Revision 1.22 2006/05/11 14:34:32PDT gzampet 
tweaked to improve transient protection
Revision 1.21 2006/05/10 08:02:16PDT gzampet 
modifications to improve standby to active performance
Revision 1.18 2006/05/05 09:36:37PDT zmiao 
fix efc ounter problem
Revision 1.17 2006/05/04 18:19:36PDT gzampet 
correct calculation to control maximum gps measurement frequency offset 
bug in last few versions would set frequency control to zero in gear 7!
Revision 1.16 2006/05/04 10:42:21PDT gzampet 
added place holder if isefcokay control of prediction data use in GPS loop
changed open loop feedback from FLL to EST to use global smi variables not messaging to see if it
make a difference
Revision 1.15 2006/05/03 16:02:39PDT gzampet 
fixed efc meaurement
Revision 1.13 2006/05/01 10:19:13PDT gzampet 
tweaked drift threshold for furino engine
Revision 1.11 2006/04/19 16:11:35PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.10 2006/04/19 07:00:51PDT gzampet 
Tweaked print statement
Revision 1.9 2006/03/29 10:18:38PST gzampet 
Fixed Debug Print of Fifo 
Revision 1.8 2006/03/29 09:00:41PST gzampet 
Debug added
Revision 1.7 2006/03/06 12:24:27PST gzampet 

Revision 1.6 2006/03/01 08:10:29PST gzampet 

Revision 1.3 2006/02/16 14:22:33PST gzampet 

Revision 1.2 2006/02/16 14:01:02PST gzampet 

Revision 1.1 2005/09/01 14:55:10PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.29 2005/01/12 07:11:24PST gzampet 

Revision 1.28  2004/12/20 17:53:34Z  gzampet
Revision 1.27  2004/12/08 16:15:17Z  gzampet
Revision 1.26  2004/12/02 15:30:37Z  gzampet
Revision 1.25  2004/12/01 15:36:53Z  gzampet
Revision 1.24  2004/11/24 22:07:20Z  zmiao
Revision 1.23  2004/11/23 23:31:09Z  gzampet
Revision 1.22  2004/11/23 18:49:20Z  gzampet
Revision 1.21  2004/11/03 20:03:24Z  zmiao
Revision 1.20  2004/10/18 17:39:57Z  gzampet
Revision 1.19  2004/10/14 21:47:35Z  gzampet
Revision 1.18  2004/09/03 16:52:38Z  gzampet
Revision 1.17  2004/08/27 15:12:05Z  gzampet
Revision 1.16  2004/08/25 23:04:39Z  gzampet
Revision 1.15  2004/08/24 21:45:56Z  gzampet
Revision 1.14  2004/07/30 23:59:06Z  gzampet
Revision 1.13  2004/07/27 14:52:16Z  gzampet
Revision 1.12  2004/07/09 18:28:22Z  gzampet
Added separate CTS Measurement channel
Revision 1.11  2004/07/01 15:43:24Z  gzampet
Revision 1.10  2004/07/01 13:11:42Z  gzampet
Revision 1.12  2004/06/25 17:55:32Z  gzampet
Stable 100ms 333 samples per second version
Revision 1.11  2004/06/24 23:20:37Z  gzampet
Revision 1.10  2004/06/24 20:50:29Z  gzampet
adjusted for 10Hz wander samples
Revision 1.9  2003/06/05 17:03:02Z  gzampet
Revision 1.8  2003/05/28 18:42:42Z  gzampet
Revision 1.7  2003/05/22 19:20:56Z  gzampet
Revision 1.6  2003/05/15 15:25:30Z  gzampet
Revision 1.5  2003/04/25 18:03:23Z  zmiao
Revision 1.4  2003/04/24 14:59:14Z  gzampet
Revision 1.3  2003/04/19 00:10:57Z  gzampet
Revision 1.2  2003/04/15 21:15:59Z  gzampet
Revision 1.8  2002/09/06 00:28:33Z  gzampet
Update BT3_Span_est routine to support new
CSM (RTHC) Measurement scheme
Revision 1.7  2002/08/16 18:01:25Z  gzampet
Adjusted the ken_Mpll_freq  (which is the one second average icw 
reported from the CSM FPGA) to include the smi.temp_state_cor value.
  Also calculate the FMD.Yroaavg which is used to in the collection 
of factory calibration data
Revision 1.6  2002/07/03 21:32:30Z  zmiao
Merge with GZ
Revision 1.5  2002/06/19 20:32:31Z  kho
Added RTHC measurement to span_est function
Revision 1.4  2002/06/10 20:59:09Z  kho
Added seed value for OCXO current.
Revision 1.3  2002/06/07 18:08:42Z  kho
1. Rescaled TPslewcheck value into seconds then changed it to 60 seconds in bt3_def.h
2. Bottom out TPSlew bucket at 0, in order to take out filtering.
3. Added a line to stuff the Temp_Est_Prev if the OCXO current was still saturated.
Revision 1.2  2002/05/31 17:59:34Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
 * 
 *    Rev 1.0   06 Feb 1999 15:41:40   S.Scott
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
*     Module: mcca_est.c
*     Description: Module performs suite of 250 msec tasks to estimate and
*                  manage input frequency and temperature states.
*     NOTE REVISION HISTORY PRIOR TO 2.4 IS FOR MCCA_TASK MODULE SINCE THIS
*     ROUTINES USED TO BE THERE
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
*    Version  2.4  10/24/96 Baseline Version these routines were moved intact
*                  from mcca_task.c to improve code organization (all input
*                  estimation tasks are in this source file).
*         MODULE RENAME BT3_EST.C 9-15-97  as part of the BT3 Core Clock Engine
*         Project.
*     Version 1.0 Baseline BT3 clock engine
*                        1) No Change
*                        2) Rename E1_Meas_Data to Span_Meas_Data
*                        3) Rename "E1" Tagged Measurement Related variables
*                           to SPAN
*                        4) Change Filtering in Freq_Est250. The old approach
*                           used two cascasded binary filters which are efficient
*									 to implement, but produce truncation error. The new
*									 approach uses two cascased moving average filters to
*									 smooth the delta phase samples and a single moving
*									 average filter to smooth the delta time samples.
*									 The moving average is efficient to implement (only
*									 2 adds per update) and eliminates truncation error
*									 issues.
*								 5) Add span type management to allow freqest to operate
*                           on either T1 or E1 spans.
*                           SPANTYPEA and SPANTYPEB are defined in bt3def.h
*                           The BT3 FPGA needs to be configured for either
*                           span operation using the control register.
*                           Based on the span type the DeltaBias ans
*                           DeltaFix are initialized to control estimation
*                           process.
*                        6) The Span Drift Check Process used to work on a block
*                           of data at a time. The new process uses an integrate and
*                           dump to generate a summed frequency every SPdrift-
*                           check interval. The drifcheck interval is kept short
*                           (1-4 sec). It is their just to decimate and the drift
*                           update to a reasonable rate. The actual averaging
*                           process uses a smoothing filter (new) which averages
*                           over a period less than the FIFO transient size of
*                           the SPAN FIFO to ensure the drift transient is
*                           detected before it impacts the output.
*     Version 2.0:
*                     1) Add support for Span_FLL intertask messaging
*                     2) Add support for RO_FLL intertask messaging
*                     3) Add support for FLL_EST intertask messaging
*                     4) Add support for NCO_Update EST intertask messaging
*                     5) Add support for PCOMP_EST intertask messaging
*                     6) Add Msg_Gen_250ms 250ms output message generation task
*                     7) remove extern smi global data
*
*      Version 2.1:
*                    1) Changed sign of RO input data to agree with FPGA
*                       operation.
*****************************************************************************/
//#include "proj_inc.h"
#include "bt3_def.h"
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#endif   
#include <stdio.h>  
/******** global data structures *********/
#include "bt3_dat.h"   /*main file for global data declarations*/
#include "bt3_msg.h"   /*main file for message buffer templates*/
//#include "bt3.h"
#include <cxfuncs.h> 
//#include "types.h"
//#include "task.h"
//#include "hardware.h"
#include "AllProcess.h"

extern struct MPLL_Meas_Data mpll;
static char cbuf[200];     
// extern void DebugPrint(const char *str);

struct Span_Meas_Data *pmda, *pmdb,*pmdc,*pmdd, *pmdg;
struct Remote_Oscillator_Meas_Data *prma,*prmb,*prmg;
struct Temperature_Meas_Data *ptmd;
struct Plevel_Meas_Data      *ppmd;
struct Alevel_Meas_Data      *pamd;
struct Factory_Measurement_Data  FMD;
extern struct Span_Meas_Data GPS_Fifo,SPANA_Fifo, SPANB_Fifo,PRS_Fifo; 
extern struct Remote_Oscillator_Meas_Data RO_Fifo;
extern struct Temperature_Meas_Data TP_Fifo;
extern struct Threshold_Management TM;
extern struct LO_TYPE_BUILD_INFO LTBI;    
extern struct Shared_Memory_Interface smi;   
extern unsigned short trace_source; //controls traceability source  
extern struct Single_Input_FLL_Control_System Lloop; //GPZ June 2010 used to control convergence during trace_skip
/******* messaging related data ********/
unsigned char msg_count25ms; /*control one second message  generation process*/
unsigned short spa_reg_local, spb_reg_local,cts_reg_local,cts_reg; /*local span event registers*/
struct MSG_Span_FLL MSG_Span_FLL_A, MSG_Span_FLL_B,*pspmsg;
unsigned short roa_reg_local, rob_reg_local; /*local span event registers*/
struct MSG_RO_FLL MSG_RO_FLL_A, MSG_RO_FLL_B,*promsg;
struct MSG_EST_ATC MSG_EST_ATC_A, MSG_EST_ATC_B, *ptpmsg;
struct MSG_EST_ASM MSG_EST_ASM_A, MSG_EST_ASM_B, *pbktmsg;
unsigned short tp_reg_local; /*local temperature event registers*/
unsigned short pl_reg_local; /*local 60 Mhz pll event registers*/
unsigned short at_reg_local;
struct MSG_EST_SRPT MSG_EST_SRPT_A,MSG_EST_SRPT_B, *psrmsg;
/******* local input message data *****/
static double YprimoutSPA_rec,YprimoutSPB_rec,YprimoutROA_rec,YprimoutROB_rec;
extern struct MSG_FLL_EST MSG_FLL_EST_A;
struct MSG_FLL_EST *pfllestrec;
double *pYprimout;
extern struct MSG_NCOUP_EST MSG_NCOUP_EST_A;
struct MSG_NCOUP_EST *pncoupestrec;
double freq_state_cor_rec;
extern struct MSG_PCOMP_EST MSG_PCOMP_EST_A;
struct MSG_PCOMP_EST *ppcompestrec;
double phase_state_cor_rec;
extern struct MSG_CAL_EST MSG_CAL_EST_A;
double factory_freq_rec;
double sqrt(double);
#if PRINTLOG
extern unsigned long ptime; /* print log time stamp in cumulative minutes*/
#endif
extern unsigned short pcount; /*manage rate of print logging*/
#if((!PCSIM)) /*include SRS Rb Data in PL_Reg*/
extern unsigned char srs_status[6];
unsigned short ad6_avgd = 10;
float Temp_Est_Prev;
#endif
/***** function templates *********/
void Freq_Est250ms();
void RO_Freq_Est_250ms();
void TP_Est_250ms();
void PL_Est_250ms();
void AL_Est_250ms();
void Msg_Gen_25ms();
void get_TimeProvider_Delta(long Delta[]);  
void Set_GPS_Drift_Low(void);
double weight_correct(int);
double GPS_phase = 100e-9;
extern unsigned short Get_Gear(void);

/********************************************************

*********************************************************/
double ken_Ysp; 
double OneSecDelta[5],OneSecAvg[4]; //GPZ store deltas for Timeprovider PM
static double OneSecErr[5]; //cumulative error in delta calculation   
static double dtestphase[5]; 
static double pull_freq_meas[5]={0.0,0.0,0.0,0.0,0.0}; 
static short phasegate[5]={10,10,10,10,10};
static short phaseprint=0;
long dphase[5]; //test storage for timeprovider phase deltas
long int ken_Xsp[5];
unsigned short ken_Ndata[5],Debug_Nwrite[5],Debug_Nwrite_Last[5],Debug_Bad[5],Debug_Good[5],Debug_Skip[5];
int ken_Nread[5]; 
int ken_delta[5];
long int ken_delta_f2;
long int ken_delta_f1_sum; 
float ken_phase;
float ken_freq;
float ken_drift;    
double ken_MPLL_freq;     
double freq_drift[5], freq_drift_max;
unsigned short ROB_ASSIGN=0;/* 0= SPA duplicate 1=SPB duplicate note 4th channel is spare for TimeProvider*/
void BT3_span_est(void)
{
	short Tdelta,Tsum;
	int i;
	unsigned char indx; 
	long Delta;
	long temp1,temp2,temp3;
	register struct Span_Meas_Data *pg;
//	register int Thres;
	float Thres;
	double SPAN_Mag_Phase,SPAN_Mag_Freq, SPAN_Mag_Drift;
	double dtemp;
	unsigned short *pspreg;
	int count;
	/***** if feedback data from FLL/NCO/PCOMP or CAL is ready then receive */
	if(MSG_FLL_EST_A.Ready)
	{
		pfllestrec= &(MSG_FLL_EST_A);
		YprimoutSPA_rec=pfllestrec->YprimoutSPA;
	 	YprimoutSPB_rec=pfllestrec->YprimoutSPB;
	  	YprimoutROA_rec=pfllestrec->YprimoutROA;
	  	YprimoutROB_rec=pfllestrec->YprimoutROB;
	  	pfllestrec->Ready=FALSE;
	}
	if(MSG_NCOUP_EST_A.Ready)
	{
	  	pncoupestrec= &(MSG_NCOUP_EST_A);
	  	freq_state_cor_rec=pncoupestrec->freq_state_cor;
	  	pncoupestrec->Ready=FALSE;
	}
	if(MSG_PCOMP_EST_A.Ready)
	{
	  	ppcompestrec= &(MSG_PCOMP_EST_A);
	  	phase_state_cor_rec=ppcompestrec->phase_state_cor;
	  	ppcompestrec->Ready=FALSE;
	}
	if(MSG_CAL_EST_A.Ready)
	{
	  	factory_freq_rec=MSG_CAL_EST_A.Factory_Freq_Cal;
	  	MSG_CAL_EST_A.Ready=FALSE;
	}
	
	/******* complete receive of local input data *****/
	for(i=0;i<4;i++) /*for both Span Channels */
	{   
	  	if(i==0)
	  	{
			pmdg=pmda;
			pg=pmda;
			pYprimout=&(YprimoutSPA_rec);
			pspreg=&(spa_reg_local);
	  	}
	  	else if(i==1)
	  	{
			pmdg=pmdb;
			pg=pmdb;
			pYprimout=&(YprimoutSPB_rec);
			pspreg=&(spb_reg_local);
	 	} 
	  	else if(i==2)
	  	{
			pmdg=pmdc;
			pg=pmdc;
			pYprimout=&(YprimoutROA_rec);
			pspreg=&(roa_reg_local);
	 	}  
		else if(i==3)
	  	{
			pmdg=pmdd;
			pg=pmdd;
			pspreg=&(cts_reg_local);
	 	} 
	 	
	 	if((pg->Nread != pg->Nwrite)) /*used to be Present =0x101*/
	 	{
	 	
	  		Thres=TM.Sdelmax;
	  		count = 0;
			ken_Nread[i] = pg->Nread;  
			Debug_Nwrite_Last[i]=Debug_Nwrite[i] ;
			Debug_Nwrite[i]= pg->PPS_Nwrite_marker;   
			Debug_Bad[i]=0;
			Debug_Good[i]=0;
			Debug_Skip[i]=0;
			while(((pg->PPS_Nwrite_marker)%MBSIZE) != pg->Nread) /*read index is still behind */
			{ 
		  		indx= (pg->Nread+1)%MBSIZE;  
		  		temp3= pg->accum[indx];
		  		if((temp3&0x8000)) // we have an update
		  		{   
		  		/* find the difference between last 2 measurements*/
		  		  temp1=(temp3&0x7FFF)-pg->DeltaFix;
		  		  temp2= (pg->accum[pg->Nread])&0x7FFF; 
	              Delta=temp1-temp2;
		  		} 
		  		else Delta =0;
			    /* unwrap the rollovers */
			    if(Delta < -16384) /* -2^15 */
			    {
			    	Delta += 32768; /* 2^16 */
			    }    
			    else if (Delta > 16384)  
			    {
			    	Delta -= 32768; /* 2^16 */
			    }    
                ken_delta[i] = Delta;
		  		pg->Nread = indx; 

		  		if((temp3&0x8000)) // only recurse filter in valid sample
		  		{

		  		if((Delta<Thres)&& Delta>-Thres) /*good sample*/
		  		{   
		  		   Debug_Good[i]++;
			 		pg->Ndata++; /*increment number of good data points*/
//			 		pg->Int_Time = Tsum;
			 		if(pg->Jit_Bucket< LBUCKETMAX) pg->Jit_Bucket+=SBUCKETUP;
			 		if(pg->Jit_Bucket> LBUCKETTHRES)pg->SPAN_Jitter=FALSE;
			 /***** Implement MA 1 Filter**************************/
			 		pg->Delta_F1+=Delta;
			 		pg->Delta_F1-= pg->MAbuf1[pg->M1write]; 
			 		pg->MAbuf1[pg->M1write]=Delta;  /* 32 bits into 16 bits */
			 		pg->M1write++;
			 		if(pg->M1write>=MASIZE1)pg->M1write=0L;
			 		pg->Delta_F1_Cnt++;
			 		pg->Delta_F1_Sum+= pg->Delta_F1;
			 /*****************************************************/
			 		if(pg->Delta_F1_Cnt> 4) /* once every fifth count*/
			 		{
			  /***** Implement MA 2 Filter**************************/
			  			pg->Delta_F2+=pg->Delta_F1_Sum;
			  			pg->Delta_F2-= pg->MAbuf2[pg->M2write];
			  			pg->MAbuf2[pg->M2write]=pg->Delta_F1_Sum;
			  			pg->M2write++;
			  			if(pg->M2write>=MASIZE2) pg->M2write=0L;
			  			pg->Delta_F1_Sum=0L;
			  			pg->Delta_F1_Cnt=0;
			 		}    
			 		ken_delta_f2 =  pg->Delta_F2;
			 		ken_delta_f1_sum = pg->Delta_F1_Sum;
					/****** end LPF filter *******/
			 		pg->Xsp+=(pg->Delta_F2); /*sum into cumulative time error */
		  		} /*end good sample*/
		  		else
		  		{ 
		  			Debug_Bad[i]++;
			 		if(pg->Jit_Bucket> LBUCKETMIN) pg->Jit_Bucket-=SBUCKETDOWN;
			 		if(pg->Jit_Bucket< LBUCKETTHRES)
			 		{
				 		pg->SPAN_Jitter=TRUE;
				 		*pspreg|=16;   
			 		}    
		  		} /*end bad sample*/
				}/*end recurse on valid sample only*/		  		
		  		Debug_Skip[i]++;		  		
	  		
	  		} /*end while nread behind nwrite */
	  		if(pg->Ndata < NDATAMIN || pg->SPAN_Jitter==TRUE)  /*invalid block */
		 	{
			 	pg->SPAN_Jitter=TRUE;
			 	*pspreg|=16;
			 	pg->Xsp=0.0;
			 	pg->Ndata=0; /*Zero out integrate and dump*/    
				dtestphase[i]=0.0;		
				phasegate[i]=10;	 		
		 	}         
		  	if(!(pmdg->SPAN_Present&0x100))
		  	{
		  		*pspreg |= 0x100; /*test if qualified*/  
				dtestphase[i]=0.0;	
				phasegate[i]=10;			  		 
			}		  		 
		}/*end if SPAN present*/
	 	else
	 	{
		  	if(!(pmdg->SPAN_Present&0x01)) *pspreg  |= 0x20;
		  	if(!(pmdg->SPAN_Present&0x100)) *pspreg |= 0x100;  
			dtestphase[i]=0.0;	
			phasegate[i]=10;	
		 	pmdg->SdriftCheck=0;   
		 	pmdg->Yspsum=0.0;

	 	}
	/**********************************************
		 process  SPAN frequency estimates
	***********************************************/
	  	if((pmdg->SPAN_Jitter==FALSE))
	  	{  	
	  		if(pmdg->Ndata)
		 	{
		 		pmdg->Ysp=(double)(pmdg->Xsp)*(SPAN_FCONV); 
		 		ken_Xsp[i]=(pmdg->Xsp);
		 		if(pmdg->Ndata==36)
		 		{
		 		 	pmdg->Ysp= (pmdg->Ysp - SPAN_BIAS_36)*SPAN_TRIM_36;
		 		}
		 		else
		 		{
		 		 	pmdg->Ysp= (pmdg->Ysp - SPAN_BIAS_35)*SPAN_TRIM_35;
		 		}  
//		 		pmdg->Ysp= -pmdg->Ysp; /*GPZ need sign reversal to use a correction value*/  
		 		ken_Ndata[i] =  pmdg->Ndata;    
		 		ken_Ysp = pmdg->Ysp;
		 	}
		 	else
		 	{
		 		pmdg->Ysp=0;
		 	}
		 	pmdg->Xsp=0.0;
		 	pmdg->Ndata=0; /*Zero out integrate and dump*/ 
		 	pmdg->Int_Time=0;

		 	/* Convert to an open loop measurement */  
		 	OneSecDelta[i]= pmdg->Ysp ;   
		 	if(phasegate[i]>0) phasegate[i]--;	 		
		 	else
		 	{     
		 		dtestphase[i]-= pmdg->Ysp ;
		 	}	
//		 		pmdg->Ysp += freq_state_cor_rec + phase_state_cor_rec; 
//		 		pmdg->Ysp += smi.freq_state_cor + smi.phase_state_cor; 
			pmdg->Ysp += OneSecDelta[4]; // use actual synthesizer correction GPZ Jan 30 2009
		 	/* accumulate if data is good */   
			if(!(pmdg->SPAN_Present&0x100)) {
					pmdg->SPAN_Valid=FALSE;
					pmdg->Yspavg = pmdg->Ysp = OneSecDelta[4]; // GPZ added on 10/2/2009
			}
		 	else pmdg->SPAN_Valid=TRUE;
		 	/* stability leaky bucket management   */
		 	/****** DRIFT CHECK CODE *******/
		 	pmdg->SdriftCheck ++;
		 	pmdg->Yspsum += pmdg->Ysp;   
		 	if(i== trace_source)
		 	{
	 	  		if(pmdg->Sdrift_bucket<LBUCKETTHRES)
	 	  		{
	 	  		     *pspreg|=8;
	 	  		}
		 	
		 	}
		 	if(pmdg->SdriftCheck>=SDRIFTCHECK)
			{  
			
			    freq_drift[i] = (pmdg->Yspsum/SDRIFTCHECK);  
//   				OneSecAvg[i]=dtemp;								 

			    /*GPZ  slew limit to worse case oscillator requirements*/  
			    freq_drift_max= (double)((8-Get_Gear()))*TM.Sfreqmax*2.0;//GPZ June 2009 double to ensure enough range
			    if(freq_drift[i]>freq_drift_max) freq_drift[i]=freq_drift_max;
			    else if(freq_drift[i]<(freq_drift_max*(-1.0))) freq_drift[i]=(freq_drift_max*(-1.0));  
    		 	if(pmdg->SdriftCount > 0)  freq_drift[i] =0.0;   
			 	if(i==trace_source) /*GPZ June 2010 quick convergence added*/
			 	{
	    		 	if(Lloop.Trace_Skip>100) //GPZ June 2010 accelerate convergence    
    			 	{
  					 	pmdg->Yspavg = freq_drift[i];     		 	
    			 	}
    		 		else if(Lloop.Trace_Skip>40) 
    		 		{
    			 		pmdg->Yspavg = 0.8*pmdg->Yspavg +
								 0.2*freq_drift[i];   		 	
    		 		}
    		 		else
    		 		{     
				 		pmdg->Yspavg = pmdg->Sfilt2*pmdg->Yspavg +
									 pmdg->Sfilt1*freq_drift[i];   
      		 		}   
      		 	}
      		 	else
      		 	{
				 	pmdg->Yspavg = pmdg->Sfilt2*pmdg->Yspavg +       
									 pmdg->Sfilt1*freq_drift[i];       
				}
//			 	pmdg->Yspavg = freq_drift;   
				OneSecAvg[i]=pmdg->Yspavg;								 
			 	pmdg->SdriftCheck=0;
			 	if(i!=trace_source)
			 	{
			 		SPAN_Mag_Drift= pmdg->Yspavg - *pYprimout;   
			 	}
			 	else
			 	{	
			 		SPAN_Mag_Drift= (pmdg->Yspavg-pmdg->Ysp);  /*alternate drift test traceable input*/
			 	}	
			 	pmdg->Yspsum=0.0;
			 	if(pmdg->SdriftCount >0)
			 	{
				  	pmdg->SdriftCount--;
				  	SPAN_Mag_Drift=0;
			 	}  
			 	else if(Lloop.Trace_Skip>30) //GPZ June 2010 blank drift check during convergence  
			 	{
				  	SPAN_Mag_Drift=0;
			 	}  
			 	if(SPAN_Mag_Drift <0.0)  SPAN_Mag_Drift= -SPAN_Mag_Drift;
			 	if(i == 3) 	ken_drift = SPAN_Mag_Drift;
			 	/*update dynamic threshold*/       
			 	if(i!=trace_source) /*GPZ only use dynamic for non "GPS" inputs*/
			 	{
   				 	if( SPAN_Mag_Drift< TM.Sdriftthres[i])
				 	{    
				 		if(TM.Sdriftblank[i]>0) TM.Sdriftblank[i]--;
				    }
				   	else if(TM.Sdriftblank[i]==0)
				 	{   
				 		TM.Sdriftblank[i]=40;
				    }  
				    if(TM.Sdriftblank[i]<10)
				    {  
			    	TM.Sdriftvar[i]=0.95*TM.Sdriftvar[i]+.05*(SPAN_Mag_Drift*SPAN_Mag_Drift); 
            	    }
				    if(TM.Sdriftvar[i]>1.6e-17) TM.Sdriftvar[i]=1.6e-17;
					dtemp=5.0*sqrt(TM.Sdriftvar[i]);   
					TM.Sdriftthres[i]=dtemp;
					if(TM.Sdriftthres[i]<2e-10) TM.Sdriftthres[i]=2e-10;
				} /*end if non-GPS input*/ 
				else
				{ 
					if(Get_Gear()>2)
					{ 
						// zmiao 3/26/2009: Changed from 4.0 to 8.0 according to George Z. 
						TM.Sdriftthres[i]=(8.0*SDRIFTMAX); /*accomodate furino sawtooth*/  
					}	
					else  TM.Sdriftthres[i]=(12.0*SDRIFTMAX) ;
				}	
				/* end dynamic threshold update*/
			 	if(SPAN_Mag_Drift > TM.Sdriftthres[i])
			 	{
					pmdg->SPAN_Valid = FALSE;
					*pspreg|=2;
			  		if (pmdg->Sdrift_bucket >LBUCKETMIN) pmdg->Sdrift_bucket -= SDRIFTBUCKETDOWN;
				}
				else
				{
			  		if (pmdg->Sdrift_bucket <LBUCKETMAX) pmdg->Sdrift_bucket += SDRIFTBUCKETUP;
				}
		 	}
		 /*********** FREQ CHECK CODE ******/ 
		 if(Get_Gear()>2) SPAN_Mag_Freq=pmdg->Yspavg - factory_freq_rec;    
		 else SPAN_Mag_Freq=0.0;
		 if(i==3) ken_freq = SPAN_Mag_Freq;
		 if(SPAN_Mag_Freq <0.0)  SPAN_Mag_Freq= -SPAN_Mag_Freq;
		 if(SPAN_Mag_Freq > TM.Sfreqmax)
		 {
			pmdg->SPAN_Valid = FALSE;
			*pspreg|=1;
			if (pmdg->Sfreq_bucket >LBUCKETMIN) pmdg->Sfreq_bucket -= SFREQBUCKETDOWN;
			if(pcount<PRINTMAX && pmdg->SPANFreqFlag)
			{
				pmdg->SPANFreqFlag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:FreqEst250ms:EVENT-SPAN_Mag_Freq for Input: %d\n",PHEAD,ptime,i);
				#endif
			}
		 }
		 else
		 {
			if (pmdg->Sfreq_bucket <LBUCKETMAX) pmdg->Sfreq_bucket += SFREQBUCKETUP;
			if(pcount<PRINTMAX && (pmdg->SPANFreqFlag==FALSE))
			{
				pmdg->SPANFreqFlag=TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:FreqEst250ms:CLEAR-SPAN_Mag_Freq for Input: %d\n",PHEAD,ptime,i);
				#endif
			}
		 }
		 /*************** PHASE CHECK CODE **********/
		 SPAN_Mag_Phase=pmdg->Ysp - pmdg->Yspavg; 
		 if(i==3)ken_phase = SPAN_Mag_Phase;
		 if(pmdg->SdriftCount > 0)  SPAN_Mag_Phase=0.0;
		 if(SPAN_Mag_Phase <0.0)  SPAN_Mag_Phase= -SPAN_Mag_Phase;
		 if(SPAN_Mag_Phase > TM.Sphasemax)
		 {
			pmdg->SPAN_Valid = FALSE;
			*pspreg|=64;
			if (pmdg->Sphase_bucket >LBUCKETMIN) pmdg->Sphase_bucket -= SPHASEBUCKETDOWN;
			if(pcount<PRINTMAX && pmdg->SPANPhaseFlag)
			{
				pmdg->SPANPhaseFlag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:FreqEst250ms:EVENT-SPAN_Mag_Phase for Input: %d\n",PHEAD,ptime,i);
				#endif
			}
		 }
		 else
		 {
			if (pmdg->Sphase_bucket <LBUCKETMAX) pmdg->Sphase_bucket += SPHASEBUCKETUP;
			if(pcount<PRINTMAX && (pmdg->SPANPhaseFlag==FALSE))
			{
				pmdg->SPANPhaseFlag=TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:FreqEst250ms:CLEAR-SPAN_Mag_Phase for Input: %d\n",PHEAD,ptime,i);
				#endif
			}
		 }
		 /******** END LEAKY BUCKET MANAGEMENT *******/
		} /* end if present and no jitter */
		else
		{
		 pmdg->SPAN_Valid=FALSE;
		//pmdg->Sdrift_bucket=(LBUCKETTHRES-SDRIFTBUCKETDOWN); /*gpz force reference to restabilized before using!!*/		 
		 /*if (pmdg->Sfreq_bucket >LBUCKETMIN) pmdg->Sfreq_bucket -= SFREQBUCKETDOWN;*/
		 sprintf(cbuf, "Jitter on chan %d\r\n", i);
		 DebugPrint(cbuf, 10);
		}
	}/*end for both channels*/
			
	/**** debug printout *****/ 	
	#if 1  
	if(phaseprint<10)
	{  
			//GPZ July 2009 use agresssive phase printing	  
//			if(phaseprint==0||phaseprint==5||phaseprint==0||phaseprint==0||phaseprint==0) /*decimate  update*/
			if(1) 
			{ 
			//	get_TimeProvider_Delta(&dphase[0]);	
			sprintf((char *)cbuf, "cts meas june 2010:%5d,%5d,%5d,%5d,%5d,%5d,%5d%5d\r\n", 
			phaseprint,  
			trace_source, 
			Debug_Nwrite[trace_source],
			Debug_Nwrite_Last[trace_source],
			Debug_Good[trace_source],
			Debug_Bad[trace_source],
			pmdd->SPAN_Jitter,
			pmdd->SPAN_Present
			);   
			DebugPrint(cbuf, 10);  

			sprintf((char *)cbuf, "cts meas adv:%7ld,%7ld,%7ld,%7ld,%7ld,%7ld,%7ld,%7ld,%7ld,%7ld:\r\n", 
			(long int)(ken_freq*(double)1e12),
			(long int)(ken_drift*(double)1e12),
			(long int)(freq_drift[trace_source]*(double)1e12), 
			(long int)(smi.freq_state_cor*(double)1e12), 
			(long int)(smi.phase_state_cor*(double)1e12), 
			(long int) (factory_freq_rec * (double)1e12), 
			(long int) (pmdd->Ysp * (double)1e12),
			(long int)(OneSecAvg[trace_source] * (double)1e12),
			(long int) (TM.Sfreqmax*(double)1e12), 
			(long int)(OneSecDelta[trace_source]*1e12));   
			DebugPrint(cbuf, 10);  
			}   
//			phaseprint++; GPZ July 2009 force print every second
	}
	else
	{
			phaseprint=0;
	}			

	  		
	#endif      
	/**** end debug print *****/	

	/******* generate 1 second message for FLL, ATC, Status_Report and
	 ASM tasks   ******/
	 pbktmsg=&(MSG_EST_ASM_A);
	 pspmsg=&(MSG_Span_FLL_A);
	 promsg=&(MSG_RO_FLL_A);
	 ptpmsg=&(MSG_EST_ATC_A);
	 psrmsg=&(MSG_EST_SRPT_A);
	 cts_reg=cts_reg_local;
	 cts_reg_local=0;
	  pspmsg->SPA_Reg_Events=spa_reg_local;
	  pspmsg->SPB_Reg_Events=spb_reg_local;
	  pspmsg->YspavgA=pmda->Ysp;
	  pspmsg->YspavgB=pmdb->Ysp;
	  pspmsg->Ready=TRUE;
	  promsg->ROA_Reg_Events=roa_reg_local;
  	  promsg->YroavgA=pmdc->Ysp;  
	  if(!ROB_ASSIGN)
	  {
		  promsg->ROB_Reg_Events=spa_reg_local;  
		  promsg->YroavgB=pmda->Ysp;  
		  ptpmsg->ROB_Reg_Events=spa_reg_local;   
		  ptpmsg->YroavgB=pmda->Yspavg;  
	  	  pbktmsg->ROBphase_bkt= pmda->Sphase_bucket;   
		  pbktmsg->ROBfreq_bkt= pmda->Sfreq_bucket;
		  pbktmsg->ROBdrift_bkt= pmda->Sdrift_bucket;
		  psrmsg->ROB_Reg_Events=spa_reg_local; rob_reg_local=0;

	  }
	  else
	  {
		  promsg->ROB_Reg_Events=spb_reg_local;  
		  promsg->YroavgB=pmdb->Ysp;  
		  ptpmsg->ROB_Reg_Events=spb_reg_local;
		  ptpmsg->YroavgB=pmdb->Yspavg;
		  pbktmsg->ROBphase_bkt= pmdb->Sphase_bucket;
		  pbktmsg->ROBfreq_bkt= pmdb->Sfreq_bucket;
		  pbktmsg->ROBdrift_bkt= pmdb->Sdrift_bucket;
		  psrmsg->ROB_Reg_Events=spa_reg_local; rob_reg_local=0;

	  }  
	  promsg->Ready=TRUE;
	  ptpmsg->ROA_Reg_Events=roa_reg_local;
	  ptpmsg->TP_Reg_Events=tp_reg_local;
	  ptpmsg->TP_Slew_Update=ptmd->TPSlewUpdate;  ptmd->TPSlewUpdate=FALSE;
	  ptpmsg->YroavgA=pmdc->Yspavg;
	  ptpmsg->Ytpavg =ptmd->Ytpavg;
	  ptpmsg->Temp_Est=ptmd->Temp_Est;
	  ptpmsg->Ready=TRUE;
	  psrmsg->SPA_Reg_Events=spa_reg_local; spa_reg_local=0;
	  psrmsg->SPB_Reg_Events=spb_reg_local; spb_reg_local=0;
	  psrmsg->ROA_Reg_Events=roa_reg_local; roa_reg_local=0;
	  psrmsg->TP_Reg_Events=tp_reg_local;    tp_reg_local=0;
	  psrmsg->PL_Reg_Events=pl_reg_local;    pl_reg_local=0;
	  psrmsg->AT_Reg_Events=at_reg_local;    at_reg_local=0;
	  psrmsg->Ready=TRUE;

	  pbktmsg->SPAphase_bkt= pmda->Sphase_bucket;
	  pbktmsg->SPBphase_bkt= pmdb->Sphase_bucket;
	  pbktmsg->SPAfreq_bkt= pmda->Sfreq_bucket;
	  pbktmsg->SPBfreq_bkt= pmdb->Sfreq_bucket;
	  pbktmsg->SPAdrift_bkt= pmda->Sdrift_bucket;
	  pbktmsg->SPBdrift_bkt= pmdb->Sdrift_bucket;
	  pbktmsg->ROAphase_bkt= pmdc->Sphase_bucket;
	  pbktmsg->ROAfreq_bkt= pmdc->Sfreq_bucket;
	  pbktmsg->ROAdrift_bkt= pmdc->Sdrift_bucket;
	  pbktmsg->TPstep_bkt= ptmd->TPstep_bucket;
	  pbktmsg->TPslew_bkt= ptmd->TPslew_bucket;
	  pbktmsg->PLstep_bkt= ppmd->PLstep_bucket;
	  pbktmsg->PLrange_bkt= ppmd->PLrange_bucket;
	  pbktmsg->ALstep_bkt= pamd->ALstep_bucket;
	  pbktmsg->ALrange_bkt= pamd->ALrange_bucket;
     pbktmsg->Ready=TRUE;
}

/***** Function to temporarily suspend loop update if GPS is questionable****/    
void Set_GPS_Drift_Low(void)
{
#if 0 // GPZ 7/20/2009: Not to use
		pmdd->Sdrift_bucket=(LBUCKETTHRES-3*SDRIFTBUCKETDOWN); /*gpz force reference to restabilized before using!!*/		 
#endif
}		


/**********************************************************
Weight Correction Compensation Span Function
To reduce tracking weight of inputs that have higher dynamic
drift thresholds
***********************************************************/
double weight_correct(int span)
{
	double in,out;  
	if(smi.BT3_mode!=Holdover)
	{
	 in=TM.Sdriftthres[span]*1e12;
	 out=1 + in*(.000025 - 6.25e-10*in*in);
	 if(out<0.0) out=0.0;
	}
	else
	{
		out=1.0;   /*always use full weight in holdover-bridging mode*/
	} 
	return(out);
}



int ken_cnt;
void BT3_mpll_est(void)
{    
	double efc_sum = 0;
	unsigned char indx;   
	char lcbuf1[120];  
	long int efc_sample;

	
	mpll.Ndata = 0;   
	efc_sum=0.0;
	       
/* sum all the valid MPLL measurements that occurred over the last second */   
//	if(Is_EFC_OK())
    {
		while(((mpll.PPS_Nwrite_marker)%MBSIZE) != mpll.Nread) /*read index is still behind */
		{ 
			indx = (mpll.Nread+1)%MBSIZE;     
//			if(mpll.sum[indx]&0x80000000)  
			if(1)
			{    
//			     if(mpll.sum[indx]&0xF00000L)
//			     {
//			 	    efc_sample= (mpll.sum[indx]&0x00FFFFFFL)||0xFF000000L;
//			 	 } 
//			 	 else efc_sample= (mpll.sum[indx]&0x00FFFFFFL); 
			 	 efc_sample=(long int) (mpll.sum[indx]); 
		  		 efc_sum += (double)(efc_sample);
		      	mpll.Ndata++; /*increment number of good data points*/
			}
			mpll.Nread = indx; 
		}  
	}
	if(mpll.Ndata>0)
	{
//		ken_MPLL_freq = (efc_sum-((double)(VAR_CENTER)*mpll.Ndata))*MAX_VAR_SENSITIVITY/mpll.Ndata;   
		ken_MPLL_freq = ((double)(efc_sum)*1.70298985e-12)/(double)(250.0*mpll.Ndata); //was 2.72478E-11  
//		ken_MPLL_freq = ((double)(efc_sum)*2.72478E-11)/(double)(1.0*mpll.Ndata);   //test only
		
		//GPZ adjust with holdover temperature compensation  
		ken_MPLL_freq -= smi.temp_state_cor; 
		OneSecDelta[4]= ken_MPLL_freq ; 
		ken_cnt = mpll.Ndata;     
	}
	
    sprintf((char *)lcbuf1,"mpll_ndata:%5d ",mpll.Ndata );
	DebugPrint(lcbuf1, 12);
    sprintf((char *)lcbuf1,"efc_sample:%8lx ",efc_sample );
	DebugPrint(lcbuf1, 12);
    sprintf((char *)lcbuf1,"efc_sum:%.3e ",efc_sum );
	DebugPrint(lcbuf1, 12);
    sprintf((char *)lcbuf1,"mpll_freq:%.3e\r\n  ",ken_MPLL_freq );
	DebugPrint(lcbuf1, 12);
	FMD.YROA_Smooth = 0.01*ken_MPLL_freq + 0.99*FMD.YROA_Smooth;
   	
}
/******
Time Provider delta phase reporting routine
return delta phase every second with units
of ns   
assignments:
indx	channel
0		span 1
1		span 2
2		PRS
3		LO
*******/
void get_TimeProvider_Delta(long *pD)
{
	char i;         
	double Delta_f; 
	unsigned short ireg;
	/* note make channel 3 GPS */
	for(i=0;i<4;i++)
	{
		if(i==0)ireg=psrmsg->SPA_Reg_Events;
		else if(i==1) ireg=psrmsg->SPB_Reg_Events;
		else if(i==2) ireg=psrmsg->ROA_Reg_Events;
		else ireg=ireg=cts_reg;  /*this needs to be GPS*/
		if (1) // (ireg<16) // ZMIAO(11/3/2004): This causes NA when input disqualified. But we do need data to clear PM event.
		{
		 	Delta_f=OneSecDelta[i]*1e10+OneSecErr[i];  
		 	*(pD+i)=(long)(Delta_f+0.5); // ZMIAO(11/24/2004): Changed from int to long
		 	OneSecErr[i]=Delta_f- *(pD+i); 
	 	}
	 	else
	 	{
	 		 *(pD+i)=0x7FFFFFFFL; 
	 	}
	 		 
	}
}



/********************************************************
 The following task is called every 250 msecs.
 It extracts a smooth temperature estimate of the local oscillator and
 a slew rate estimate.
 The input data is collected in the Temperature_Meas_Data structure.
*********************************************************/
void TP_Est_250ms()
{
	unsigned char indx;
	int Delta,Tsum,Temp_Est;
	register int Thres;
	float TP_Mag_Step,TP_Mag_Slew;
#if((PCSIM)) /*Use On-Board Oven Current Monitor*/
  if(LTBI.lotype!=RB)
  {
	 if(ptmd->TP_Present)
	 {
	  Tsum=0; /*zero out cumulative time sum*/
	  Thres=TM.TPdelmax;
	  Temp_Est=(int)(ptmd->Temp_Est+.5);
	  while(ptmd->Nwrite != ptmd->Nread) /*read index is still behind */
		{
		  indx= ptmd->Nread+1; /* Depends on Nread being a single byte */
		  /* Delta is with respect to current Temp_Est */
		  #if (TARGET==CLPR)
		  Delta= (ptmd->Count[indx])
			-(Temp_Est);
		  #else
		  Delta= (ptmd->Count[indx]<<2)
			-(Temp_Est);
		  #endif
		  ptmd->Nread = indx;
		  if((Delta<=Thres)&& Delta>=-Thres) /*good sample*/
		  {
			 ptmd->Ndata++; /*increment number of good data points*/
			 Tsum += TDELTA; /*keep track of cumulative measurement time in block */
			 ptmd->Int_Time = Tsum;
			 if(ptmd->Jit_Bucket< LBUCKETMAX) ptmd->Jit_Bucket+=TPBUCKETUP;
			 if(ptmd->Jit_Bucket> LBUCKETTHRES)ptmd->TP_Jitter=FALSE;
			 /*Delta is bounded to 8 TPDELMAX  the filter gain is 40 bits so
				the maximum resolution we can multiple by
				31-9 22 shifts*/
			 ptmd->Delta=((long int)Delta)<<21;
			 /***** implement  LPF Binary Smoothing Filter *******/
			 ptmd->Delta_F1= ((ptmd->Delta_F1<<3)- ptmd->Delta_F1 + ptmd->Delta)>>3;
			 ptmd->Delta_F1_Cnt++;
			 ptmd->Delta_F1_Sum+= ptmd->Delta_F1;
			 if(ptmd->Delta_F1_Cnt > 4) /* once every fifth count*/
			 {
				ptmd->Delta_F1_Cnt=0;
				ptmd->Delta_F2=((ptmd->Delta_F2<<4)-ptmd->Delta_F2 + ptmd->Delta_F1_Sum)>>4;
				ptmd->Delta_F1_Sum=0L;
			 }
			 /****** end binary LPF filter *******/
			 ptmd->Xtp+=(double)ptmd->Delta_F2; /*sum into cumulative temp error */
		  } /*end good sample*/
		  else
		  {
			 if(ptmd->Jit_Bucket> LBUCKETMIN) ptmd->Jit_Bucket-=TPBUCKETDOWN;
			 if(ptmd->Jit_Bucket< LBUCKETTHRES)
			 {
				 ptmd->TP_Jitter=TRUE;
				 tp_reg_local|=16;
			 }
			 /* move output  to converge to input */
			  if((Delta >Thres)){
				ptmd->Temp_Est++;
				ptmd->Ytpsum ++;
			  }
			  else {
				 ptmd->Temp_Est--;
				 ptmd->Ytpsum --;
			  }

		  } /*end bad sample*/
	  } /*end while nread behind nwrite */
	  if(ptmd->Ndata < TP_NDATAMIN || ptmd->TP_Jitter==TRUE)  /*invalid block */
		 {
			 ptmd->TP_Jitter=TRUE;
			 ptmd->Xtp=0.0;
			 ptmd->Ndata=0; /*Zero out integrate and dump*/
			 ptmd->Int_Time=0;
			 tp_reg_local|=16;
		 }
	  if(pcount<PRINTMAX && ptmd->TP_Jitter==TRUE && ptmd->TPJitterFlag==TRUE)
		 {
			 ptmd->TPJitterFlag= FALSE;
			 #if PRINTLOG
			 pcount++;
			 printf("%s%5ld:TP_Est250ms:EVENT-TP_Jitter for Input:\n",PHEAD,ptime);
			 #endif
		 }
	  if(pcount<PRINTMAX && ptmd->TP_Jitter==FALSE && ptmd->TPJitterFlag==FALSE )
		{
			 ptmd->TPJitterFlag= TRUE;
			 #if PRINTLOG
			 pcount++;
			 printf("%s%5ld:TP_Est250ms:CLEAR-TP_Jitter for Input:\n",PHEAD,ptime);
			 #endif
		}
	 }/*end if TP present*/
	 else   tp_reg_local|=32;
  }/*end not RB*/
#endif
	/**********************************************
		 process 250ms TP frequency estimates
	***********************************************/
	  if((ptmd->TP_Present)&&(ptmd->TP_Jitter==FALSE))
	  {
#if((!PCSIM)) 
//	if(LTBI.lotype==RB)/*Use SRS Rb Temperature Data*/
	if(Temp_Est_Prev == 0) /* seed value */
	{ 
//		Temp_Est_Prev = read_ADC(0);
		Temp_Est_Prev = 64;//dummy code for now
	}
	if(1)/*Use SRS Rb Temperature Data (kjh)*/
	{
//		 ptmd->Temp_Est=read_ADC(0); /* read OCXO current in mA */
		 ptmd->Temp_Est=64; /* dummy code for now */
		 ptmd->Ytp = ptmd->Temp_Est-Temp_Est_Prev;
		 Temp_Est_Prev = ptmd->Temp_Est;
	}  
	else
	{
		 ptmd->Ytp=ptmd->Xtp/(ptmd->Int_Time)*TP_FCONV;
		 ptmd->Delta_F2-= ptmd->Xtp/ptmd->Ndata;
		 ptmd->Delta_F1=0;
		 ptmd->Delta_F1_Sum=0;
		 ptmd->Xtp=0.0;
		 ptmd->Ndata=0; /*Zero out integrate and dump*/
		 ptmd->Prev_Int_Time = ptmd->Int_Time;
		 ptmd->Int_Time=0;
		 ptmd->TP_Valid=TRUE;
		 ptmd->Temp_Est+=ptmd->Ytp;
   }
#else
		 ptmd->Ytp=ptmd->Xtp/(ptmd->Int_Time)*TP_FCONV;
		 ptmd->Delta_F2-= ptmd->Xtp/ptmd->Ndata;
		 ptmd->Delta_F1=0;
		 ptmd->Delta_F1_Sum=0;
		 ptmd->Xtp=0.0;
		 ptmd->Ndata=0; /*Zero out integrate and dump*/
		 ptmd->Prev_Int_Time = ptmd->Int_Time;
		 ptmd->Int_Time=0;
		 ptmd->TP_Valid=TRUE;
		 ptmd->Temp_Est+=ptmd->Ytp;
#endif
		 /* stability leaky bucket management   */
		 /****** SLEW CHECK CODE *******/
		 ptmd->TPslewCheck++;
		 ptmd->Ytpsum += ptmd->Ytp;
		 if(ptmd->TPslewCheck>=TM.TPslewcheck)
		 {
			 ptmd->TPSlewUpdate=TRUE;
			 ptmd->Ytpavg = ptmd->Ytpsum;
			 ptmd->TPslewCheck=0;
			 TP_Mag_Slew= ptmd->Ytpavg;
			 ptmd->Ytpsum=0.0;
			 if(ptmd->TPslewCount >0)
			 {
				  ptmd->TPslewCount--;
				  TP_Mag_Slew=0;
			 }
			 if(TP_Mag_Slew <0.0)  TP_Mag_Slew= -TP_Mag_Slew;
			 if(TP_Mag_Slew > TM.TPslewmax)
			 {
				ptmd->TP_Valid = FALSE;
				if (ptmd->TPslew_bucket >LBUCKETMIN) ptmd->TPslew_bucket -= TPSLEWBUCKETDOWN;
				if (ptmd->TPslew_bucket <0) ptmd->TPslew_bucket = 0;
				tp_reg_local|=2;
				if(pcount<PRINTMAX && ptmd->TPSlewFlag)
				{
					ptmd->TPSlewFlag=FALSE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:TP_est250ms:EVENT-TP_Mag_Slew for Input:\n",PHEAD,ptime);
					#endif
				}
			 }
			 else /*slew rate is okay */
			 {
				if (ptmd->TPslew_bucket <LBUCKETMAX) ptmd->TPslew_bucket += TPSLEWBUCKETUP;
				if(pcount<PRINTMAX && (ptmd->TPSlewFlag==FALSE))
				{
					ptmd->TPSlewFlag=TRUE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:TP_Est250ms:CLEAR-TP_Mag_Drift for Input:\n",PHEAD,ptime);
					#endif
				}
				/***** apply adaptive temperature correction******/
				/*smi.temp_state_cor+=ptmd->Ytpavg*ATC.TC_avg;*/
				/*smi.temp_state_cor+=ptmd->Ytpavg*-2.5e-12;*/
			 }
		 }
		 /*********** STEP CHECK CODE ******/
		 TP_Mag_Step=ptmd->Ytp;
		 if(TP_Mag_Step <0.0)  TP_Mag_Step= -TP_Mag_Step;
		 if(TP_Mag_Step > TM.TPstepmax)
		 {
			 ptmd->TP_Valid = FALSE;
			 tp_reg_local|=1;
			if (ptmd->TPstep_bucket >LBUCKETMIN) ptmd->TPstep_bucket -= TPSTEPBUCKETDOWN;
			if(pcount<PRINTMAX && ptmd->TPStepFlag)
			{
				ptmd->TPStepFlag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:TP_Est250ms:EVENT-TP_Mag_Step for Input:\n",PHEAD,ptime);
				#endif
			}
		 }
		 else
		 {
			if (ptmd->TPstep_bucket <LBUCKETMAX) ptmd->TPstep_bucket += TPSTEPBUCKETUP;
			if(pcount<PRINTMAX && (ptmd->TPStepFlag==FALSE))
			{
				ptmd->TPStepFlag=TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:TP_Est250ms:CLEAR-TP_Mag_Step for Input:\n",PHEAD,ptime);
				#endif
			}
		 }     
		 /****** check to see if OCXO current is below a threshold value *******/
		 if(ptmd->Temp_Est>TM.TPrangecheck)
		 {                 
		 	tp_reg_local|=0x40;
		 	Temp_Est_Prev += 10.0; /* this will stuff the integral if saturated (kjh)*/ 
		 }
		 /******** END LEAKY BUCKET MANAGEMENT *****/
	  } /*end if present and no jitter*/
		else
		{
		 ptmd->TP_Valid=FALSE;
		}
}
/********************************************************
 The following task is called every 250 msecs.
 It extracts a smooth estimate of the current 60 MHz varactor
 level in the BT3 synthesizer loop. 

 The input data is collected in the Power_Level_Meas_Data structure.
*********************************************************/
void PL_Est_250ms()
{
	unsigned char indx;
	int Delta,Tsum,Plev_Est;
	register int Thres;
	float PL_Mag_Step,PL_Mag_Range;
	float	PLrangemax,PLrangemin;

#if 0 /* gut this function out and return no errors (kjh)*/
	ppmd->PLJitterFlag=TRUE; 
	ppmd->PLStepFlag=TRUE; 
	ppmd->PLRangeFlag=TRUE;
	ppmd->PL_Present=TRUE;
	ppmd->PLstep_bucket=0;
	ppmd->PLrange_bucket=0;
	ppmd->Jit_Bucket=0;
#else 
	 if(ppmd->PL_Present)
	 {
	  Tsum=0; /*zero out cumulative time sum*/
	  Thres=PLDELMAX;
	  Plev_Est=(int)(ppmd->Plev_Est+.5);
	  while(ppmd->Nwrite != ppmd->Nread) /*read index is still behind */
		{
		  indx= ppmd->Nread+1; /* Depends on Nread being a single byte */
		  /* Delta is with respect to current Plevel_Est */
		  #if (TARGET==CLPR)
		  Delta= (ppmd->Count[indx])
			-Plev_Est;
		  #else
		  Delta= (ppmd->Count[indx]<<2)
			-Plev_Est;
		  #endif
		  ppmd->Nread = indx; 
		  Delta = 0; /* zero out measurement to get rid of PLL alarm (kjh) */
		  if((Delta<Thres)&& Delta>-Thres) /*good sample*/
		  {
			 ppmd->Ndata++; /*increment number of good data points*/
			 Tsum += TDELTA; /*keep track of cumulative measurement time in block */
			 ppmd->Int_Time = Tsum;
			 if(ppmd->Jit_Bucket< LBUCKETMAX) ppmd->Jit_Bucket+=PLBUCKETUP;
			 if(ppmd->Jit_Bucket> LBUCKETTHRES)ppmd->PL_Jitter=FALSE;
			 /*Delta is bounded to 8 PLDELMAX  the filter gain is 40 bits so
				the maximum resolution we can multiple by
				31-9 22 shifts*/
			 ppmd->Delta=((long int)Delta)<<21;
			 /***** implement  LPF Binary Smoothing Filter *******/
			 ppmd->Delta_F1= ((ppmd->Delta_F1<<3)- ppmd->Delta_F1 + ppmd->Delta)>>3;
			 ppmd->Delta_F1_Cnt++;
			 ppmd->Delta_F1_Sum+= ppmd->Delta_F1;
			 if(ppmd->Delta_F1_Cnt > 4) /* once every fifth count*/
			 {
				ppmd->Delta_F1_Cnt=0;
				ppmd->Delta_F2=((ppmd->Delta_F2<<4)-ppmd->Delta_F2 + ppmd->Delta_F1_Sum)>>4;
				ppmd->Delta_F1_Sum=0L;
			 }
			 /****** end binary LPF filter *******/
			 ppmd->Xpl+=(double)ppmd->Delta_F2; /*sum into cumulative temp error */
		  } /*end good sample*/
		  else
		  {
			 if(ppmd->Jit_Bucket> LBUCKETMIN) ppmd->Jit_Bucket-=PLBUCKETDOWN;
			 if(ppmd->Jit_Bucket< LBUCKETTHRES)
			 {
				ppmd->PL_Jitter=TRUE;
			 	pl_reg_local|=16;
			 }
			 /* move output as maximum slew rate to converge to input */
			  if((Delta >Thres)){
				ppmd->Plev_Est++;
			  }
			  else {
				 ppmd->Plev_Est--;
			  }

		  } /*end bad sample*/
	  } /*end while nread behind nwrite */
#if 0 /* (kjh) */
	  if(ppmd->Ndata < PL_NDATAMIN || ppmd->PL_Jitter==TRUE)  /*invalid block */
		 {
			 ppmd->PL_Jitter=TRUE;
			 pl_reg_local|=16;
			 ppmd->Xpl=0.0;
			 ppmd->Ndata=0; /*Zero out integrate and dump*/
			 ppmd->Int_Time=0;
		 }
#endif
	  if(pcount<PRINTMAX && ppmd->PL_Jitter==TRUE && ppmd->PLJitterFlag==TRUE)
		 {
			 ppmd->PLJitterFlag= FALSE;
			 #if PRINTLOG
			 pcount++;
			 printf("%s%5ld:PL_Est250ms:EVENT-PL_Jitter for Input:\n",PHEAD,ptime);
			 #endif
		 }
	  if(pcount<PRINTMAX && ppmd->PL_Jitter==FALSE && ppmd->PLJitterFlag==FALSE )
		{
			 ppmd->PLJitterFlag= TRUE;
			 #if PRINTLOG
			 pcount++;
			 printf("%s%5ld:PL_Est250ms:CLEAR-PL_Jitter for Input:\n",PHEAD,ptime);
			 #endif
		}
		#if((!PCSIM)) 
		if(LTBI.lotype==RB)/*include SRS Rb Data in PL_Reg*/
		{
//(kjh)			 if(srs_status[0]) pl_reg_local|=0x8000;
//(kjh)			 if(srs_status[1]) pl_reg_local|=0x4000;
//(kjh)			 if(srs_status[2]) pl_reg_local|=0x2000;
//(kjh)			 if(srs_status[3]) pl_reg_local|=0x1000;
		}
		#endif
	 }/*end if PL present*/
//(kjh)		 else  pl_reg_local|=32;
	/**********************************************
		 process 250ms Power Level estimates
	***********************************************/
	  if((ppmd->PL_Present)&&(ppmd->PL_Jitter==FALSE))
	  {
		 ppmd->Ypl=ppmd->Xpl/(ppmd->Int_Time)*PL_FCONV;
		 ppmd->Delta_F2-= ppmd->Xpl/ppmd->Ndata;
		 ppmd->Delta_F1=0;
		 ppmd->Delta_F1_Sum=0;
		 ppmd->Xpl=0.0;
		 ppmd->Ndata=0; /*Zero out integrate and dump*/
		 ppmd->Prev_Int_Time = ppmd->Int_Time;
		 ppmd->Int_Time=0;
		 ppmd->PL_Valid=TRUE;
		 ppmd->Plev_Est+=ppmd->Ypl;
		 /* stability leaky bucket management   */
		 /****** RANGE CHECK CODE *******/
		 PL_Mag_Range= ppmd->Plev_Est;
		 if(ppmd->Plevel_Set){
			PLrangemax=PLRANGEMAX_HIGH;
			PLrangemin=PLRANGEMIN_HIGH;
		 }
		 else {
			PLrangemax=PLRANGEMAX_LOW;
			PLrangemin=PLRANGEMIN_LOW;
		 }
		 if((PL_Mag_Range>PLrangemax) ||(PL_Mag_Range< PLrangemin))
			 {
				ppmd->PL_Valid = FALSE;
//(kjh)					pl_reg_local|=2;
				if (ppmd->PLrange_bucket >LBUCKETMIN) ppmd->PLrange_bucket -= PLRANGEBUCKETDOWN;
				if(pcount<PRINTMAX && ppmd->PLRangeFlag)
				{
					ppmd->PLRangeFlag=FALSE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:PL_est250ms:EVENT-PL_Mag_Range for Input:\n",PHEAD,ptime);
					#endif
				}
			 }
			 else /*range is okay */
			 {
				if (ppmd->PLrange_bucket <LBUCKETMAX) ppmd->PLrange_bucket += PLRANGEBUCKETUP;
				if(pcount<PRINTMAX && (ppmd->PLRangeFlag==FALSE))
				{
					ppmd->PLRangeFlag=TRUE;
					#if PRINTLOG
					pcount++;
					printf("%s%5ld:PL_Est250ms:CLEAR-PL_Mag_Range for Input:\n",PHEAD,ptime);
					#endif
				}
			 }
		 /*********** STEP CHECK CODE ******/
		 PL_Mag_Step=ppmd->Ypl;
		 if(PL_Mag_Step <0.0)  PL_Mag_Step= -PL_Mag_Step;
		 if(PL_Mag_Step > PLSTEPMAX)
		 {
			 ppmd->PL_Valid = FALSE;
//(kjh)			 pl_reg_local|=1;
			if (ppmd->PLstep_bucket >LBUCKETMIN) ppmd->PLstep_bucket -= PLSTEPBUCKETDOWN;
			if(pcount<PRINTMAX && ppmd->PLStepFlag)
			{
				ppmd->PLStepFlag=FALSE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:PL_Est250ms:EVENT-PL_Mag_Step for Input:\n",PHEAD,ptime);
				#endif
			}
		 }
		 else
		 {
			if (ppmd->PLstep_bucket <LBUCKETMAX) ppmd->PLstep_bucket += PLSTEPBUCKETUP;
			if(pcount<PRINTMAX && (ppmd->PLStepFlag==FALSE))
			{
				ppmd->PLStepFlag=TRUE;
				#if PRINTLOG
				pcount++;
				printf("%s%5ld:PL_Est250ms:CLEAR-PL_Mag_Step for Input:\n",PHEAD,ptime);
				#endif
			}
		 }
		 /******** END LEAKY BUCKET MANAGEMENT *****/
	  } /*end if present and no jitter*/
		else
		{
		 ppmd->PL_Valid=FALSE;
		} 
#endif
}
