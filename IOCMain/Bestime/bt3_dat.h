/***************************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/13 23:56:53PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_dat.h_v  $
$Modtime:   06 Feb 1999 12:53:50  $
$Revision: 1.3 $


                            CLIPPER SOFTWARE
                                 
                                BT3_DAT.H

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module maintains all global declarations for MCCA.

MODIFICATION HISTORY:

$Log: bt3_dat.h  $
Revision 1.3 2010/10/13 23:56:53PDT Jining Yang (jyang) 
Merged from 1.2.1.1.
Revision 1.2.1.1 2010/08/04 23:24:10PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.2 2008/04/01 07:38:37PDT George Zampetti (gzampetti) 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.1 2007/12/06 11:41:05PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.3 2006/05/02 12:09:21PDT gzampet 
Updated to support the BT63 command and general startup issues in the lower gears
Revision 1.2 2006/04/19 15:57:15PDT gzampet 
Updates associated with reading back the efc samples and processing them in the ssu_fll file to generate 
holdover information 
Revision 1.1 2005/09/01 14:55:10PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.5 2004/10/14 14:47:15PDT gzampet 

Revision 1.4  2004/09/03 16:52:56Z  gzampet
Revision 1.3  2004/07/01 13:10:50Z  gzampet
Revision 1.2  2003/04/19 00:10:55Z  gzampet
Revision 1.1  2003/04/14 17:07:00Z  zmiao
Initial revision
Revision 1.6  2002/10/01 00:02:59Z  zmiao
Added CLEI code in factory setting
Revision 1.5  2002/09/06 00:22:07Z  gzampet
Modified span measurement data coverted Xsp back to
a double and changed the 256 element accum from longs to
words
Revision 1.4  2002/09/03 18:55:31Z  zmiao
Added factory setting
Revision 1.3  2002/06/19 21:21:26Z  kho
Added MPLL_Meas_Data struct
Revision 1.2  2002/05/31 18:12:36Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
Revision 1.1  2001/12/11 19:28:44Z  kho
Initial revision
 * 
 *    Rev 1.0   06 Feb 1999 15:41:36   S.Scott
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
*     Multi tracking Core Control Algorithm  (MCCA)  Rename to BT3 see history
*     Module: Dpll_Dat.c
*     Description: Module maintains all global declarations for MCCA
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
*      Version 1.3 3/7/96 Convert Freq_Est_250 to integer algorithm
*                  1) Delta, DeltaF1, DeltaF2 converted to long int
*                     and shifted after doubles in FIFO structure
*                  2) Added Delta_F1_Cnt and Delta_F1_Sum to FIFO to control
*                     decimation to second filter to every fifth 2ms update
*      Version 1.4 3/11/96 Add User adjustable thresholds.
*                  1) Added Threshold Management Structure
*                  2) Added Bidirectional Threshold Data Elements to common
*                     memory FIFO.
*                  3) Changed Cdata structure to all floats to save RAM
*                      space.
*                  4) Added Eb Mdev Crossover stabililty estimate in common
*                     memory
*                  5) Add overall health flags to ASM structure.
*                  6) Add weight factors to commom memory to control ensembling
*                  7) Added variance estimates of crossover stability to
*                    common memory
*                  8) Added per channel validity flag to Data Measurement
*                     structure.
*                  9) Add MTIE analysis variables in PER_Tau_Stats
*      Version 1.5 5/20/96
*                  1) Add Primary Correction State 	 double Prim_Cor_State
*							 to Dual Input Control System Structure
*                  2) Added Yprim_Buf[PRIMDELAY]to Dual Input Control System
*                     Structure
*                  3) Added Prim_Skip count to skip input to primary loop
*                     during a transient
*       5/23/96 Module renamed MCCA_dat.c as part of CellSync 2000 project
*               revision history of this "new" module starts with 1.0
*        Version 1.0 5/23/96
*                  1) Add Remote Oscillator Measurement Data Structure
*                  2) Add ROdelmax threshold in management structure and shared
*                     memory structure.
*                  3) Add Phase Freq and Drift thresholds in TM and SMI struct
*                     ures
*                  4) Add recalibration flag in Dual Loop Structure
*
*                  5) Add Remote Oscillator Control Loop Structure to manage
*                     both ensembled and non-ensembled remote oscillator freg
*                     lock
*                  6) Added remote oscillator frequency state correction output
*                     to the  smi.
*                  7) Added remote oscillator time constants to smi
*
*                  8) Added Temperature Measurement Structure 7/16/96
*
*                  9) Add TPdelmax threshold in management structure and shared
*                     memory structure.
*                  10)Add Step and Slew thresholds in TM and SMI structures
*                  11) Added Remote Oscillator and Temperature Shared Memory
*                      Registers
*                  12) Add Remote Oscillator and Temperature alarm flags to
*                      ASMM structure.
*                  13) Added Temperature Calibration Related Data Structures.
*                  14) Add TCcycle flag to smi to control starting and stopping
*                      the temperature calibration process.
*                  15) Add Temperature Compensation Structure to manage temperature
*                      compensation process.
*                  16) Add One PPS phase input from FPGA to SMI.
*        Version 2.0
*
*        Version 2.1
*                   1) Renamed the Temperature_Calibration Data Structure Tag
*                      to be Tmperature_Calibration to prevent any confusion
*                      with the Temperature_Calibration_Data_Record
*                   2) Moved static part_indx and prev_part_indx into
*                      TCOMP structure to prevent compiler issues
*                   3) Move static prev_meas into Phase_Compensation
*                      structure
*         Version 2.2
*                   1) Add NCO_Low and NCO_Dither to smi to support CS2000
*                      32 bit NCO control
*                   2) Add NCO_High 32 bit control word to smi.
*       Version 2.4
*                   1) 1/20/97 add One_PPS_Mag and One_PPS_Range to smi
*                      structure to manage LUCENT RFTG-u 1 pps locking
*                      algorithm
*                   2) Add Power Level Measurement Data Structure
*        Version 2.5
*                   1) Add Calibration Data Structure. This structure contains
*                      the basic frequency calibration drift calibration and
*                      temperature calibration non-volatile data needed to
*                      provide rapid and seamless restart of a unit.
*         MODULE RENAME BT3_DAT.H 9-15-97  as part of the BT3 Core Clock Engine
*         Project.
*     Version 1.0 Baseline BT3 clock engine
*                     1) ASMM data structure expanded to include all state flags
*                        that were previously variables declared in mcca_tsk.c
*                     2) Rename E1_Meas_Data to  Span_Meas_Data
*                     3) Moved X250ms and Data_Valid from general Data250ms
*                        structure to channel_data structure. It is the input
*                        data to the stability calculator and belongs with the
*                        per channel data.
*
*****************************************************************************/
#ifndef _BT3_DAT_H_04_09_2003
#define _BT3_DAT_H_04_09_2003

#include "bt3_def.h"
struct LO_TYPE_BUILD_INFO
{
	 unsigned short lotype; /*global type control 0=OCXO, 1=Rb*/

	 /* PPS control */

    double maxphaseslew; 
    double minphaseslew; 
    unsigned short nocrossingcnt; 

	 /*GPS firewall*/

    double gfreqmax; 
    double gdriftmax; 

	 /* LO control loop */

	 double single_prop_tau;
	 double single_int_tau;
	 double single_dint_tau;
	 double single_accel;
	 double single_recal;
	 double gear_space;

	 /* NCO control */
	 unsigned long nco_center;
	 unsigned long nco_max;
	 unsigned long nco_min;
	 unsigned long nco_range;
	 double   nco_convert;
	 double   dither_bias;
};
#if 0
struct Per_Tau_Stats
{
	 double Cur_Sum   ;  /* Sum over current TAU interval */
	 double Lag_Sum   ;  /* Sum over TAU interval one lag behind */
	 double Dlag_Sum  ;  /* Sum over TAU interval two lags behind*/
	 double Sec_Diff  ;  /* Result of TAU spaced 2nd diff operator*/
	 float  TVAR_est  ;  /* Smoothed estimate of TVAR */
	 float  MVAR_est  ;  /* Smoothed estimate of MVAR */
	 float  AFREQ_est ;  /* Average Freq Estimate*/
	 float  Tau       ;  /* Tau of data interval */
	 float  Tau_Sq    ;  /* Square of Tau of data interval */
	 long int  Min_Cur   ;  /* Current minimum time error in TAU window */
	 long int  Max_Cur   ;  /* Current maximum time error in TAU window */
	 float     Mtie      ;  /* MTIE over all TAU windows since reset */
	 unsigned char Mtie_Reset;  /*controls reset of MTIE statistics*/
	 unsigned char Ntotal       ;  /*total cumulative updates of stats*/
	 unsigned char Cur_Sum_Full ;  /* flag controlling start up of sums */
	 unsigned char Lag_Sum_Full ;  /* flag controlling start up of sums */
	 unsigned char Dlag_Sum_Full;  /* flag controlling start up of sums */
	 unsigned char Cur_Samp     ;  /* index to most recent sample in circular buffer */
	 unsigned char Lag_Samp     ;  /* index to sample one TAU lag behind */
	 unsigned char Dlag_Samp    ;  /* index to sample two TAU lags behind*/
} ;  
#endif
struct Channel_Data
{
	 long int  Short_Buf[BUFMAX];
	 long int  Med_Buf  [BUFMAX];
	 long int  Long_Buf [BUFMAX];
	 double X1000ms; /*1000ms time error updates for each channel*/
	 unsigned short Data_Valid; /* per channel validity flag */
};
struct Three_Corner_Hat_Data
{
	  double Xgps_lo, Xsa_lo, Xsb_lo, Xra_lo, Xrb_lo; /*input average phase data */
	  double Ygps_lo, Ysa_lo, Ysb_lo, Yra_lo, Yrb_lo; /*freq data smoothed*/
	  double Ysa_gps, Ysb_gps,Yra_gps,Yrb_gps;        /*derived measurements*/
	  double Pgps_lo, Psa_lo, Psb_lo, Pra_lo, Prb_lo; /*previous input freq data smoothed*/
	  double Psa_gps, Psb_gps,Pra_gps,Prb_gps;        /*previous derived measurements*/
	  double Vgps_lo, Vsa_lo, Vsb_lo, Vra_lo, Vrb_lo; /*Mvar sample data*/
	  double Vsa_gps, Vsb_gps,Vra_gps,Vrb_gps;        /*Mvar sample data*/
	  double Mgps_lo, Msa_lo, Msb_lo, Mra_lo, Mrb_lo; /*Mvar estimate*/
	  double Msa_gps, Msb_gps,Mra_gps,Mrb_gps;        /*Mvar estimate*/
	  double Filt1,Filt2;                             /*Mvar smoothing filters*/
	  double GPS_mvar, PA_mvar,PB_mvar,LO_mvar;       /*Decomposed mvar estimates*/
	  unsigned short XGPS_cnt;
	  unsigned short XSPA_cnt,XSPB_cnt;
	  unsigned short XROA_cnt,XROB_cnt;
	  unsigned short Fshift;                         /*update count to shift smoothing filter*/
	  unsigned short Skip;                           /* control start up blanking period*/
	  unsigned short Min_Cnt;                        /*Minute Count to control integration TAU*/
	  unsigned short Pmode;                          /*Primary mode assignment (Span Line or External)*/
};
struct Ensembling_Control_Data
{
	  double GPS_mvar, PA_mvar,PB_mvar,LO_mvar;         /*Decomposed mvar estimates*/
	  double SA24_mvar,SB24_mvar;                     /*24 hour pred. tvar estimates*/
	  double RA24_mvar,RB24_mvar,LO24_mvar;           /*24 hour pred. tvar estimates*/
     double PAC_mvar,PBC_mvar,LOC_mvar;              /* combined 24 hour and decomposed mvar est*/
	  double GPS_weight,PA_weight,PB_weight;          /*weighting factors*/
	  double freq_state_cor;                          /*output frequency state correction*/
	  unsigned short Pmode;                           /*Primary mode assignment (Span Line or External)*/
	  unsigned short Input_Mode;                      /*Ensembling mode control*/
};
struct Input_Freq_One_Minute_Data
{
	double XGPS; /* cumulative phase error (after transients removed)
							with respect to open loop OCXO*/
	double XSPA,XSPB,XROA,XROB; /*cumulative phase error (after transients removed)
											with respect to open loop OCXO*/
	unsigned short XGPS_cnt;
	unsigned short XSPA_cnt,XSPB_cnt,XROA_cnt,XROB_cnt;
};
struct Factory_Measurement_Data
{
	double YROA_Smooth; /*open loop ROA frequency measurement (with Temp Corr
                         smoothed over 1 minute */

};

struct MPLL_Meas_Data
{
	unsigned long sum[MBSIZE];
  	unsigned char PPS_Nwrite_marker;
	unsigned char Nwrite;     /*write index of FIFO*/
  	unsigned char Nread;      /*read index of FIFO */
  	unsigned short Ndata;      /* total number of valid Delta Counts */
  	unsigned long status;
};

struct Span_Meas_Data
{
  long int Xsp    ;         /*cumulative  delta error*/
  double Ysp    ;         /* one  fractional freq estimate wrt LO*/
  double Yspavg;          /*average span freq used for drift check */
  double Yspsum;          /* current sum of Ysp data */
  double Sfilt1,Sfilt2;   /*weight factors for frequency smoothing filter*/
  long int Delta_F1;      /*Output of first LPF binary filter*/
  long int Delta_F1_Sum;  /*Summed Output of first LPF binary filter*/
  long int Delta_F2;       /*Output of second LPF binary stage */
  short  Int_Time;       /* Integration time for current block of data*/
  short  Jit_Bucket; /* test for excessive input jitter*/
  short  Sphase_bucket;  /*leaky bucket for excessive span freq offset */
  short  Sfreq_bucket;  /*leaky bucket for excessive span freq offset */
  short  Sdrift_bucket; /* leaky bucket for excessive span freq drift */
  unsigned char SdriftCheck;    /*Drift Check Period */
  unsigned char SdriftCount;    /*Drift Checking Blanking Count*/
  unsigned char SPANJitterFlag;
  unsigned char SPANDriftFlag;
  unsigned char SPANFreqFlag;
  unsigned char SPANPhaseFlag;
  unsigned int  accum[MBSIZE]; /*sum of NCO corrections*/
  unsigned char count[MBSIZE]; /*number of accum sample*/
  unsigned char PPS_Nwrite_marker; /*(kjh) 2/18/02 marker in fifo for 1 second tick */
  unsigned char Nwrite;     /*write index of FIFO*/
  unsigned char Nread;      /*read index of FIFO */
  unsigned short Ndata;      /* total number of valid Delta Counts */
  unsigned short SPAN_Present; /*SPAN Signal is present No Loss of Signal, No out of Frame*/
  unsigned short SPAN_Jitter;  /*Excessive Jitter is detected on input signal*/
  unsigned short SPAN_Valid;   /*Summary Flag of Health of SPAN */
  unsigned short Delta_F1_Cnt; /*Controls decimation of first filter output */
  short MAbuf1[MASIZE1]; /*Buffer to support moving average of Delta Count Data*/
  unsigned short M1write;
  long int MAbuf2[MASIZE2]; /*Buffer to support moving average of Delta Count Data*/
  unsigned short M2write;
  short Tdelta_F1;         /*Output of filter 1 for delta time stamps*/
  short MATbuf1[MATSIZE1]; /*Buffer to support moving average of Delta Count Data*/
  unsigned short MT1write;
  unsigned short Span_Type; /*Flag indicating either T1 or E1 span type*/
  double DeltaBias;        /*fractional bias error per strobe update in count*/
  short  DeltaFix;         /*integer bias error per strobe update in count*/
};
struct Remote_Oscillator_Meas_Data
{
  double Xro    ;         /*cumulative 250 ms delta error*/
  double Yro    ;         /*one 250ms fractional freq estimate wrt Local Osc*/
  double Yroold;          /*last average fractional freq estimate*/
  double Yroavg;          /*average RO freq used for drift check */
  double Yrosum;          /* current sum of Ye1 data */
  long int Delta;         /*current delta count */
  long int Delta_F1;      /*Output of first LPF binary filter*/
  long int Delta_F1_Sum;  /*Summed Output of first LPF binary filter*/
  long int Delta_F2;      /*Output of second LPF binary stage */
  short    Int_Time;      /* Integration time for current block of data*/
  short Prev_Int_Time;
  short  Jit_Bucket;     /* test for excessive input jitter*/
  short  Rphase_bucket;  /*leaky bucket for excessive freq offset */
  short  Rfreq_bucket;   /*leaky bucket for excessive freq offset */
  short  Rdrift_bucket;  /* leaky bucket for excessive freq drift */
  unsigned char ROdriftCheck;    /*Drift Check Period */
  unsigned char ROdriftCount;    /*Drift Checking Blanking Count*/
  unsigned char ROJitterFlag;
  unsigned char RODriftFlag;
  unsigned char ROFreqFlag;
  unsigned char ROPhaseFlag;
  unsigned long  accum[MBSIZE]; /*vector of counts*/
  unsigned char count[MBSIZE]; /*vector of Time msec timestamps*/
  unsigned char Nwrite;     /*write index of FIFO*/
  unsigned char Nread;      /*read index of FIFO */
  unsigned short Ndata;      /* total number of valid Delta Counts */
  unsigned short RO_Present; /*E1 Signal is present No Loss of Signal, No out of Frame*/
  unsigned short RO_Jitter;  /*Excessive Jitter is detected on input signal*/
  unsigned short RO_Valid;   /*Summary Flag of Health of E1 */
  unsigned short Delta_F1_Cnt; /*Controls decimation of first filter output */
  short Tdelta_F;         /*Output of filter  for delta time stamps*/
  short MATbuf[MATSIZE]; /*Buffer to support moving average of Delta Count Data*/
  unsigned short MTwrite;
};
struct GPS_Measurement_Data
{
	 double Ygps; /*Current frequency offset of GPS wrt LO based on delta cbias*/
	 double GPS_prev;
	 double GPS_freq_prev;
	 double GPS_clock_bias; /*estimate of GPS clockbias*/
	 double inter_cbias;    /*interpolation clockbias for CLPR type application*/
	 double inter_inc;      /*incremental bias update*/
	 unsigned long gpssec;  /*last updated gpssec timestamp*/
	 short  uptime;         /*time between updates*/
	 short  Gfreq_bucket;  /*leaky bucket for excessive freq offset */
	 short  Gdrift_bucket; /* leaky bucket for excessive freq drift */
};

struct Temperature_Meas_Data
{
  float Xtp;         /*cumulative 250 ms delta error*/
  float Ytp;         /*one 250ms temp slew rate estimate*/
  float Ytpavg;      /*average temperature slew rate over
							  integration interval used for drift check */
  float Ytpsum;      /* current sum of Ytp data */
  float Temp_Est;	     /* Current temp estimate in units 0-1024 over	 range*/
  long int Delta;         /*current delta count */
  long int Delta_F1;      /*Output of first LPF binary filter*/
  long int Delta_F1_Sum;  /*Summed Output of first LPF binary filter*/
  long int Delta_F2;      /*Output of second LPF binary stage */
  short    Int_Time;      /* Integration time for current block of data*/
  short  Prev_Int_Time;
  short  Jit_Bucket;     /* test for excessive input jitter*/
  short  TPstep_bucket;  /*leaky bucket for excessive Temperature step */
  short  TPslew_bucket;   /*leaky bucket for excessive Temperature slew rate */
  unsigned short TPslewCheck;    /*Slew Check Period */
  unsigned short TPslewCount;    /*Slew Checking Blanking Count*/
  unsigned char TPJitterFlag;   /*active jitter event */
  unsigned char TPStepFlag;     /*active step event*/
  unsigned char TPSlewFlag;     /*active slew event*/
  unsigned char TPSlewUpdate;   /*Indicates new slew update ready*/
  unsigned short Count[MBSIZE]; /*vector of counts*/
  unsigned char Nwrite;     /*write index of FIFO*/
  unsigned char Nread;      /*read index of FIFO */
  unsigned short Ndata;      /* total number of valid Delta Counts */
  unsigned short TP_Present; /*Temperature signal is present */
  unsigned short TP_Jitter;  /*Excessive Jitter is detected on input signal*/
  unsigned short TP_Valid;   /*Summary Flag of Health of Temperature Signal */
  unsigned short Delta_F1_Cnt; /*Controls decimation of first filter output */
};
struct Plevel_Meas_Data
{
  float Xpl;         /*cumulative 250 ms delta error*/
  float Ypl;         /*one 250ms power slew rate estimate*/
  float Plev_Est;	   /* Current power level estimate in units 0-1024 over range*/
  long int Delta;         /*current delta count */
  long int Delta_F1;      /*Output of first LPF binary filter*/
  long int Delta_F1_Sum;  /*Summed Output of first LPF binary filter*/
  long int Delta_F2;      /*Output of second LPF binary stage */
  short    Int_Time;      /* Integration time for current block of data*/
  short  Prev_Int_Time;
  short  Jit_Bucket;     /* test for excessive input jitter*/
  short  PLstep_bucket;  /*leaky bucket for excessive power step */
  short  PLrange_bucket;   /*leaky bucket for power level out of range */
  unsigned short  PLJitterFlag;   /*active jitter event */
  unsigned short  PLStepFlag;     /*active step event*/
  unsigned short  PLRangeFlag;     /*active slew event*/
  unsigned short Count[MBSIZE]; /*vector of counts*/
  unsigned char Nwrite;     /*write index of FIFO*/
  unsigned char Nread;      /*read index of FIFO */
  unsigned short Ndata;      /* total number of valid Delta Counts */
  unsigned short PL_Present; /*Power Level signal is present */
  unsigned short PL_Jitter;  /*Excessive Jitter is detected on input signal*/
  unsigned short PL_Valid;   /*Summary Flag of Health of Power Level Signal */
  unsigned short Delta_F1_Cnt; /*Controls decimation of first filter output */
  unsigned short Plevel_Set;   /*1 is 23dbm 0 is 17 dbm*/
};
struct Alevel_Meas_Data
{
  float Xal;         /*cumulative 250 ms delta error*/
  float Yal;         /*one 250ms power slew rate estimate*/
  float Alev_Est;	   /* Current power level estimate in units 0-1024 over range*/
  long int Delta;         /*current delta count */
  long int Delta_F1;      /*Output of first LPF binary filter*/
  long int Delta_F1_Sum;  /*Summed Output of first LPF binary filter*/
  long int Delta_F2;      /*Output of second LPF binary stage */
  short    Int_Time;      /* Integration time for current block of data*/
  short  Prev_Int_Time;
  short  Jit_Bucket;     /* test for excessive input jitter*/
  short  ALstep_bucket;  /*leaky bucket for excessive power step */
  short  ALrange_bucket;   /*leaky bucket for power level out of range */
  unsigned short  ALJitterFlag;   /*active jitter event */
  unsigned short  ALStepFlag;     /*active step event*/
  unsigned short  ALRangeFlag;     /*active slew event*/
  unsigned short Count[MBSIZE]; /*vector of counts*/
  unsigned char Nwrite;     /*write index of FIFO*/
  unsigned char Nread;      /*read index of FIFO */
  unsigned short Ndata;      /* total number of valid Delta Counts */
  unsigned short AL_Present; /*Power Level signal is present */
  unsigned short AL_Jitter;  /*Excessive Jitter is detected on input signal*/
  unsigned short AL_Valid;   /*Summary Flag of Health of Power Level Signal */
  unsigned short Delta_F1_Cnt; /*Controls decimation of first filter output */
};
struct Temperature_Calibration_Data_Record
{
/*  signed char    Freq_Est[MAXCALRAMPS]; /* Freq Estimate list resolution 1e-12*/
/*  signed char    Slew_Est[MAXCALRAMPS]; /* Temperature Slew rate list */
  /*for debug make int for easier viewing*/
  short    Freq_Est[MAXCALRAMPS]; /* Freq Estimate list resolution 1e-12*/
  short    Slew_Est[MAXCALRAMPS]; /* Temperature Slew rate list */
};
struct Temperature_Compensation
{
	short Slew_Comp;  /* slew rate compensation factor */
	short Freq_Comp[MAXTEMPRECORDS]; /* static incremental freq compensation ps/s */
	short Part_Base; /*index to base of partition cache. The cache holds freq_comp
						  data interpolated on a per temperature unit bases. There are
						  4 temperature units in each region*/
	short part_indx;
	short prev_part_indx;
	short Partition[TEMPERATURE_CACHE_LENGTH]; /*partition cache*/
	short Static_Temp_Cor;
	short Dynamic_Temp_Cor;
	short Part_Init;
};
struct Adaptive_Temperature_Compensation
{
	 double IFA, IFB; /*input temperaure and freq smoothing filter weights*/
	 double Stemp1, Sfreq1; /*smoothing filter 1st stage outputs*/
	 double Ptemp, Pfreq;   /*previous temperature and frequency data*/
	 double Stslew1_pos,Stslew1_neg; /*smoothing TC filter first stage outputs*/
	 double Sfslew1_pos,Sfslew1_neg; /*smoothing TC filter first stage outputs*/
	 double Stslew2_pos,Stslew2_neg; /*smoothing TC filter first stage outputs*/
	 double Sfslew2_pos,Sfslew2_neg; /*smoothing TC filter first stage outputs*/
	 double TCFA,TCFB;      /*TC smoothing filter weights*/
	 double TC_pos, TC_neg; /* positive and negative temperature sensitivity coef.*/
	 double TC_avg;         /*average TC*/
	 unsigned short ATAU; /*Adaptive temperature update time interval*/
	 float   Tslew_Thres;/*Threshold to validate sufficient temperature slew*/
	 unsigned short Slew_Bucket; /*leaky bucket to manage Tslew_Thres setting*/
	 unsigned short Acnt; /*Adaptive update count*/
	 unsigned short Settle_Cnt; /*initial count to skip update until filters settle*/
    unsigned short Min_Cnt;    /*minute counter controls call of ATC_mode*/
	 unsigned short Mode;       /*operating mode ATCGPSMODE,ATCEXTAMODE,ATCEXTBMODE*/
};
struct Phase_Compensation
{
	float attack_rate; /* gain constant of proportional phase control*/
	double time_err_est; /*current estimate of time error (bias) for phase control loop
								 based average phase error over attack time (sec)*/
	double tdev_crossover; /*tdev stability estimate at Lloop crossover point*/
	double TFOM;         /*estimate of timing stability at crossover point in Lloop (2 sigma)*/
	double tgain1, tgain2; /*smoothing time constants use to calculate time err est*/
	short acquire_flag;  /* controls initial acquire mode to find zero crossing
								 of FGPA phase detector by slewing output at MAXPHASESLEW*/
	short no_crossing_cnt; /* number of consecutive updates with no zero crossings*/
	short prev_meas;      /* last phase measurement sign */
	short cur_error_est;  /*verified estimates of timing error 50ns LSB*/
	short prev_error_est;
	short PPS_Range_Bucket; 
};
struct Threshold_Management
{
	double Gfreqmax;    /*Maximum allowed GPS-LO frequency bias*/
	double Gdriftmax;   /*Maximum allowed GPS-LO drift instability over 1 sec*/
	double Sphasemax;   /*Maximum allowed SPAN-LO phase step over 250 msec
								 measured as a 250 msec frequency impulse compared to
								 the intergrated frequency over Edriftcheck period*/
	double Sfreqmax;    /*Maximum allowed SPAN-LO frequency bias*/
	double Sdriftmax;   /*Maximum allowed SPAN-LO drift instability over
								 Edriftcheck integration period*/ 
	double Sdriftvar[4];  	/*adaptive thresholds to establish dynamic drift threshold*/
	double Sdriftthres[4];	
	int    Sdriftblank[4]; /*blanking interval after transient to allow drift alarm recal process*/								 
							 
	double ROphasemax;   /*Maximum allowed RO-LO phase step over 250 msec
								 measured as a 250 msec frequency impulse compared to
								 the intergrated frequency over ROdriftcheck period*/
	double ROfreqmax;    /*Maximum allowed RO-LO frequency bias*/
	double ROdriftmax;   /*Maximum allowed RO-LO drift instability over
								 ROdriftcheck integration period*/
	float  TPstepmax;    /* maximum allowed temperature step on input 0-1024
									temperature scale */
	float  TPslewmax;    /* maximum allowed temperature slew on input 0-1024
									temperature scale */
	short    Sdelmax;     /*maximum allowed delta count over 2 msec SPAN FIFO update
								 each count is 50ns (measures short term jitter) */
	short    ROdelmax;     /*maximum allowed delta count over 8 msec RO FIFO update
								 each count is 380.9ps (measures short term jitter) */
	short    TPdelmax;     /*maximum allowed delta count over 8 msec TP FIFO update
								 each count is one unit of 0-255 range over temp 0-70 */
	short    Sfreqtau;   /* Time constant of span line frequency smoothing filter*/
	short    ROdriftcheck; /*Number of TAU Zero (250)ms intervals to integrate drift
								 measurement for Remote Oscillator*/
	short    TPslewcheck; /*Number of TAU Zero (250)ms intervals to integrate slew
								 rate measurement for Temperature*/
	float    TPrangecheck;  /* OCXO current full on threshold in Amps (kjh)*/
};
struct Dual_Input_FLL_Control_System
{
	 double Prop_Prim_Tau; /*Time Constant of Primary Loop (proportional)*/
	 double Int_Prim_Tau;     /*Time Constant of Primary Loop (Integral) */
	 double Dint_Prim_Tau;	/*Time constant of second integral E1 Loop */
	 double Prop_Gps_Tau;/*Time Constant of GPS Loop (proportional)*/
	 double Prim_Prop_State; /*Proportional State of Prim */
	 double Prim_Int_State;  /*Integral State of Prim */
	 double Prim_Dint_State; /*Second Integrater state for 3rd order loop */
	 double Prim_Cor_State;/*Correction State of Primary Loop*/
	 double GPS_Prop_State; /*Proportional State of GPS*/
	 double NCO_Cor_State;  /*Correction State of NCO */
	 double Prim_Freq_Pred; /*Primary frequency prediction to be used during holdover*/
	 double Yprim_Buf[PRIMDELAY+8]; /*Buffer to control input delay to prim loop*/
	 double Yfback_Buf[PRIMDELAY+8]; /*Buffer to control feedback delay to prim loop*/
	 double Ygps_Buf[GPSDELAY+8];   /*Buffer to control input delay to gps loop*/
	 double Ygps_scal1,Ygps_scal2;
	 double Gcalf1,Gcalf2;  /*smoothing filter weights to suppress SA in calibration*/
	 unsigned short Prim_Skip; /*mode of operation of the control loop */
	 unsigned short minute_cnt; /*minute counter to control recalibration*/
	 unsigned short shift_cnt;  /*number of minutes to stay at current Time Const*/
	 unsigned short jamloop;
	 unsigned short Recalibrate;
	 unsigned short Start_Recal;
	 unsigned short Cal_Mode;   /*mode (1-5) of control loop 5 is steady state*/
};
struct Single_Input_FLL_Control_System
{
	 double Prop_Tau; /*Time Constant of Loop (proportional)*/
	 double Int_Tau;     /*Time Constant of Loop (Integral) */
	 double Dint_Tau;	/*Time constant of second integral  Loop */
	 double Prop_State; /*Proportional State  */
	 double Int_State;  /*Integral State  */
	 double Dint_State; /*Second Integrater state for 3rd order loop */
	 double NCO_Cor_State;  /*Correction State of NCO */
	 double Freq_Err_Est;  /*Open Loop Estimate of Frequency Error after Temperature
									 compensation*/
	 double LO_Freq_Pred;  /*Frequency prediction input to use during holdover*/
	 double Freq_Err_Sum;  /*Cumulative Freq Error over Hourly Integration*/
	 double Sprop1,Sprop2; /*smoothed frequency output of proportional filters*/
	 double Sprop_prev;    /*last minute proportional output*/
	 double Propf1,Propf2;  /*smoothing filter weights to suppress SA in proportional filter*/
	 double Intf1,Intf2;   /*smoothing filter weights to suppress SA in integral filter*/
	 double Ygps_Buf[GPSDELAY+8];   /*Buffer to control input delay to gps loop*/
	 unsigned short Freq_Sum_Cnt;
	 unsigned short min_cnt; /*minute counter to decimate integral update*/
	 float  LO_weight; /* ensemble weight contribution of Local Oscillator */
	 unsigned short minute_cnt; /*minute counter to control recalibration*/
	 unsigned short shift_cnt;  /*number of minutes to stay at current Time Const*/
	 unsigned short jamloop;
	 unsigned short Recalibrate;
	 unsigned short Cal_Mode;   /*mode (1-5) of control loop 5 is steady state*/
	 unsigned short Trace_Skip;   /*Allow short blanking period after change of trace source*/
};
struct Holdover_Management
{
	double Daily_Freq_Log[24];    /*24 Hour Frequency Log */
	double Holdover_Base[24];     /*  starting point for holdover period */
	double Holdover_Drift[24];    /*  incremental offset per hour */
	double Prediction_Var24[24];  /* log of prediction variance sec^2*/
	double Monthly_Freq_Log[30]; /*daily Frequency Log*/
	float Monthly_Pred_Log[30]; /*monthly prediction error log*/
	double FreqSum;           /*hourly frequency sum*/
	double Predict_Err24Hr;  /*estimate of 1PPS Flywheel Error of previous
										 24 Hr flywheel sec rms*/
	double Freq_Sum_Daily;    /*current sum of daily freq log data */
	double Drift_Sum_Weekly;  /*  sum of delta freq per day from monthly log*/
	double Osc_Freq_State;    /*Estimate of oscillator free-running freq state*/
	double Long_Term_Drift;   /*  incremental offset per hour */
	double Var_Sum_Daily;     /*current sum of prediction variance prediction errors*/
	unsigned short FreqSumCnt;/*number of valid 1 second updates in sum*/
	unsigned short Cur_Hour; /* Range 0-23 index to current hour in 24 hour freq log*/
	unsigned short Cur_Day;   /* Range 0-30 index to current day in 30 day freq log */
	unsigned short Nsum_Hour;  /*total number of data points in Daily Log*/
	unsigned short Nsum_Day;   /*total number of data points in Monthly log*/
   unsigned short Restart;    /*Restart flag to manage start up */
};
struct Holdover_LSF
{
  double mfit,bfit;  /*estimated slope and intercept*/
  double sx,sy,sxy,sx2; /*lsf parameters*/
  double finp;      /*previous frequency input*/
  double dsmooth;   /*smoothed drift sample*/
  double wa,wb;     /*smoothing filter weight functions*/
  unsigned short N; /*points in fit*/
};
struct Tracking_Stats
{
	 unsigned short Good_Track_Min[24]; /*number of successful tracking minutes
													  per hour*/
	 unsigned short Total_Good_Min;
    unsigned short Cur_Hour;
	 float Daily_Track_Success; /* Probability of successful tracking over 24 hours*/
	 unsigned short Hold_Stats;   /* cumulative per minute holdover stats
											 No. of events upper byte
											 Total duration lower byte
											 Check and Reset every four hours            */
	 unsigned short Hold_Stats_Latch; /*Set to holdstats every 4 hours BT3 shell
													clears after reporting*/
	 unsigned short Last_Mode;
	 unsigned short Min_Cnt; 	
};
struct ASM_Management
{
	unsigned char  Sec_Cnt;    /* number of seconds in current minute */
	unsigned char  Min_Cnt;    /* number of minutes in current hour */
	unsigned char SPA_Flag;
	unsigned char SPB_Flag;
	unsigned char GPS_Flag;
	unsigned char ROA_Flag;
	unsigned char ROB_Flag;
	unsigned char TP_Flag;
	unsigned char PL_Flag;
	unsigned char AL_Flag;
	unsigned char SPA_Hflag;
	unsigned char SPB_Hflag;
	unsigned char GPS_Hflag;
	unsigned char ROA_Hflag;
	unsigned char ROB_Hflag;
	unsigned char TP_Hflag;
	unsigned char PL_Hflag;
	unsigned char AL_Hflag;
	unsigned char GPS_Acc_Flag;
	unsigned char GPS_Bias_Flag;
	unsigned char GPS_Freq_Flag;
	unsigned char GPS_Drift_Flag;
	unsigned char GPS_Freq_Bucket_Flag;
	unsigned char GPS_Drift_Bucket_Flag;
	unsigned char GPS_Freq_Bucket_Hyst_Flag;
	unsigned char GPS_Drift_Bucket_Hyst_Flag ;
	unsigned char SPA_Freq_Bucket_Flag;
	unsigned char SPA_Phase_Bucket_Flag;
	unsigned char SPA_Drift_Bucket_Flag;
	unsigned char SPA_Phase_Bucket_Hyst_Flag;
	unsigned char SPA_Freq_Bucket_Hyst_Flag;
	unsigned char SPA_Drift_Bucket_Hyst_Flag;
	unsigned char SPB_Freq_Bucket_Flag;
	unsigned char SPB_Phase_Bucket_Flag;
	unsigned char SPB_Drift_Bucket_Flag;
	unsigned char SPB_Phase_Bucket_Hyst_Flag;
	unsigned char SPB_Freq_Bucket_Hyst_Flag;
	unsigned char SPB_Drift_Bucket_Hyst_Flag;
	unsigned char ROA_Freq_Bucket_Flag;
	unsigned char ROA_Phase_Bucket_Flag;
	unsigned char ROA_Drift_Bucket_Flag;
	unsigned char ROA_Phase_Bucket_Hyst_Flag;
	unsigned char ROA_Freq_Bucket_Hyst_Flag;
	unsigned char ROA_Drift_Bucket_Hyst_Flag;
	unsigned char ROB_Freq_Bucket_Flag;
	unsigned char ROB_Phase_Bucket_Flag;
	unsigned char ROB_Drift_Bucket_Flag;
	unsigned char ROB_Phase_Bucket_Hyst_Flag;
	unsigned char ROB_Freq_Bucket_Hyst_Flag;
	unsigned char ROB_Drift_Bucket_Hyst_Flag;
	unsigned char TP_Step_Bucket_Flag;
	unsigned char TP_Slew_Bucket_Flag;
	unsigned char TP_Step_Bucket_Hyst_Flag;
	unsigned char TP_Slew_Bucket_Hyst_Flag;
	unsigned char PL_Step_Bucket_Flag;
	unsigned char PL_Range_Bucket_Flag;
	unsigned char PL_Step_Bucket_Hyst_Flag;
	unsigned char PL_Range_Bucket_Hyst_Flag;
	unsigned char AL_Step_Bucket_Flag;
	unsigned char AL_Range_Bucket_Flag;
	unsigned char AL_Step_Bucket_Hyst_Flag;
	unsigned char AL_Range_Bucket_Hyst_Flag;
};
struct Status_Registers
{
	unsigned short SPA_Reg;         /*Status Reporting Registers (see Report    */
	unsigned short SPB_Reg;         /*_Status in Task.c for definitions )       */
	unsigned short GPS_Reg;         /*                  ""                      */
	unsigned short ROA_Reg;          /*                  ""                      */
	unsigned short ROB_Reg;          /*                  ""                      */
	unsigned short TP_Reg;          /*                  ""                      */
	unsigned short PL_Reg;          /*    Registers to report power level       */
	unsigned short AT_Reg;          /*    and antenna current see status_rept() */
	/******* One Minute Latched Registers ******/
	unsigned short SPA_Mreg;         /*Status Reporting Registers (see Report    */
	unsigned short SPB_Mreg;         /*_Status in Task.c for definitions )       */
	unsigned short GPS_Mreg;         /*                  ""                      */
	unsigned short ROA_Mreg;          /*                  ""                      */
	unsigned short ROB_Mreg;          /*                  ""                      */
	unsigned short TP_Mreg;          /*                  ""                      */
	unsigned short PL_Mreg;          /*    Registers to report power level       */
	unsigned short AT_Mreg;          /*    and antenna current see status_rept() */
};


typedef struct {
	unsigned short bToBeUpdated;
	char card_serial_no[MAX_SERIAL_NUM];
	char osc_serial_no[MAX_SERIAL_NUM];
	char clei_code[MAX_CLEI_NUM + 2];
	char factory_reserve[100];
	unsigned long magic;
	unsigned long version;
	unsigned short checksum;
} Factory_Setting;

struct Calibration_Data {
  unsigned short cal_state;     /*state of calibration data 0 is not initialized
											 1 is requires nvm update and 2 is stable*/
  unsigned long  date;          /*GPS time when oscillator was last calibrated*/
  unsigned short osc_sn;        /*serial number of matched oscillator*/
  unsigned short init_age_type; /*type of initialization aging model to use
											for given oscillator*/
  float LO_Freq_Cal;              /*last best estimate of oscillator correction
												frequency*/
  float LO_Drift_Cal;             /*last best estimate of oscillator drift per
												hour*/
  float LO_Freq_Cal_Factory;      /* factory based frequency calibration*/
  float LO_Drift_Cal_Factory;     /*factory base drift rate calibration*/
  float ATC_TC_Cal;                /*Adaptive Temperature Compensation Calibration*/
  short Slew_Comp;  /* non-volatile copy of slew rate compensation factor */
  signed char Freq_Comp[MAXTEMPRECORDS]; /* non-volatile copy of static incremental
												  freq compensation ps/s*/
  float efc_slope;
  unsigned short checksum;
  unsigned short update_cnt;      /*count in minutes until next update*/
};
struct Shared_Memory_Interface
{
	/***************************************************************************
	Note: Input and Output are reference with respect to the XR5 Fortran System
	An Input is data from the Dual Core Control Algorithm (DCCA) toward the XR5.
	An Output is data from the XR5 toward the DCCA
	**************************************************************************/
	double clock_bias;      /*Estimate of time error of local time base with
									respect to the GPS timescale (sec) OUTPUT*/  
	/***************************************************************************
	Note: freq, phase and temp state correction factors need to summed together
			and applied to the local NCO to achieve optimal time scale. Update
			rate to NCO is nominally 1ms for CELLSYNC 1000 and 8ms
			for CELLSYNC 2000. A dithering algorithm is used to toggle LSB to NCO
			to gain effective resolution.
	***************************************************************************/
	double freq_state_cor;  /*local feedback frequency correction OUTPUT */
	double temp_state_cor;  /*local oscillator temperature state correction*/
	double phase_state_cor; /*local oscillator phase state correction*/
	double RO_freq_state_cor;/*remote oscillator frequency state correction
										 Output to MCCA com link*/
	double RO_temp_state_cor;/*remote oscillator temperature state correction
										 Input from MCCA com link*/
	double GPS_mdev;       /*Stability estimate of GPS crossover TAU INPUT*/
	double PA_mdev;       /*Stability estimate of  Primary A crossover TAU INPUT*/
	double PB_mdev;       /*Stability estimate of  Primary B TAU INPUT*/
	double LO_mdev;       /*Stability estimate of Local Osc crossover TAU INPUT*/
	double GPS_mvar;       /*Variance estimate of GPS crossover TAU INPUT*/
	double PA_mvar;       /*Variance estimate of  Primary A crossover TAU INPUT*/
	double PB_mvar;       /*Variance estimate of  Primary B crossover TAU INPUT*/
	double LO_mvar;      /*Variance estimate of Local Osc crossover TAU INPUT*/
	double GPS_weight;    /*Weight controls medium term stability influence*/
	double PA_weight;    /*Weight controls medium term stability influence*/
	double PB_weight;    /*Weight controls medium term stability influence*/
	double timing_stats[TOTAL_TAUCAT]; /*vector of timing stats in ascending
													 TAU from TAUZERO in octave steps INPUT*/
	double clock_drift; /* Used as intermediate variable to calculate clock_bias
									during end of week type activity*/
	double NCO_Dither_Error; /*cumulative fractional error in NCO word*/

	unsigned long    EFC;
	unsigned long    NCO_Low; /*Base NCO 32 bit control word
										 for CS2000 applications*/
	unsigned long    NCO_High; /*High NCO 32 bit control word
										 for CS2000 applications*/
	unsigned short  NCO_Dither; /*Dither setting for controlling duty cycle of
										  NCO lsb*/
	unsigned short  NCO_Status; /* contains status information on nco update*/
										 /* as defined:
											 bit0 : update control flag asserted to permit
													  write cleared after write is complete
											 bit 4-7: error code for nco update see
											 NCO_Control for definition
										 *********************************************/
	 /*************************************************************************
				NOTE TIME CONSTANTS ARE Bidirectional. The user change a time
				time constant by using Data Monitor or other realtime approach.
				The DCCA C based system will check the shared memory every 1 sec
				and if the timeconstant is within range accept it. If time
				time constants can't be changed at the same time, they should
				be changed starting with the longest timeconstants down to the
				shortest. In general there should be at least a factor of 4 spacing
				in the proportional, integral and double integral time constants.
	****************************************************************************/
	unsigned short dloopsa_prop_tc; /*Dual GPS SPAN proportional time constant  */
	unsigned short dloopsa_int_tc;  /*Dual GPS SPAN integral time constant      */
	unsigned short dloopsa_dint_tc; /*Dual GPS SPAN second integral time constant*/
	unsigned short dloopsa_cal_tc;  /*Dual GPS SPAN calibration time constant   */
	unsigned short dloopsb_prop_tc; /*Dual GPS SPAN proportional time constant  */
	unsigned short dloopsb_int_tc;  /*Dual GPS SPAN integral time constant      */
	unsigned short dloopsb_dint_tc; /*Dual GPS SPAN second integral time constant*/
	unsigned short dloopsb_cal_tc;  /*Dual GPS SPAN calibration time constant   */
	unsigned short dloopra_prop_tc; /*Dual GPS RMT proportional time constant  */
	unsigned short dloopra_int_tc;  /*Dual GPS RMT integral time constant      */
	unsigned short dloopra_dint_tc; /*Dual GPS RMT second integral time constan */
	unsigned short dloopra_cal_tc;  /*Dual GPS RMT calibration time constant   */
	unsigned short dlooprb_prop_tc; /*Dual GPS RMT proportional time constant  */
	unsigned short dlooprb_int_tc;  /*Dual GPS RMT integral time constant      */
	unsigned short dlooprb_dint_tc; /*Dual GPS RMT second integral time constant*/
	unsigned short dlooprb_cal_tc;  /*Dual GPS RMT calibration time constant   */
	unsigned short sloop_prop_tc;  /*Single GPS Only proportional time constant*/
	unsigned int   sloop_int_tc;   /*Single GPS Only integral time constant    */
	unsigned int   sloop_dint_tc;  /*Single GPS second integral time constant  */
	unsigned short rloop_prop_tc;  /*Remote GPS Only proportional time constant*/
	unsigned short rloop_int_tc;   /*Remote GPS Only integral time constant    */
	unsigned short rloop_dint_tc;  /*Remote GPS second integral time constant  */
	/***************************END OF Time Constant Definition*****************/

	/****************************************************************************
		The DCCA calculates every 250 msec a set of timing statistics for up
		to six measurement channels. For Example, the measurement channels
		assignments for the CELLNET application are (see def.c) :
		GPS_TCXO 0
		EA_TCXO  1
		EA_GPS   2
		EB_TCXO  3
		EB_GPS   4
		SPARE    5
		SPARE    6
		Each measurement channel is observing a pair of clocks. Three sets of
		statistics are extracted from the data:
		MDEV     0  Modified Allan Standard Deviation: A measure of rms frequency
						stability as a function of integration time TAU.
		TDEV     1  Modified Allan Timing Standard Dev: A measure of rms timing
						stability as a function of integration time TAU.
		AFREQ    2  Average Frequency: a measure of the average fractional
						frequency error as a  function of integration time TAU
		MTIE     3  Maximum Time Interval Error: a measure of peak to peak timing
						error over integration time.
		The statistics vector is 15 elements with the following TAU spacing
		Assuming TAUZERO is 250 msec: 0.25, 0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256
												512, 1024, 2048 and 4096 seconds
		The DCCA will report the vector in timing_stats[TOTAL_TAUCAT] based on
		the following control outputs from the XR5 system:
		Timing_Chan: Which Timing Channel to report stats [0-5]
		Timing_Stat: Which Timing Stat to report [0-3]
		The timing stats will be updated within 1 sec of requesting a
		change.
	****************************************************************************/
	unsigned short Timing_Chan;
	unsigned short Timing_Stat;
	unsigned short Mtie_Reset;      /*Which timing channel to reset MTIE timing
												 stats to current TAU period value [0-5]
												 note: 99 (NONE) is no reset 100 (ALL)
												 is reset all channels NOTE the ALL  command
												 should be issued once per day*/
	unsigned short clock_bias_flag; /*Summary flag of current critical gps
											 receiver ,sattelite or antenna alarms OUTPUT*/
	unsigned short  kalman_mode;    /*Current Kalman Operating Mode OUTPUT      */
	unsigned short oven_current;    /* current 8 bit estimate of OCXO
												 oven current OUTPUT                      */
	unsigned short excessive_temp;  /*Oven current low threshold exceeded INPUT */
	unsigned short BT3_mode;        /*current mode of operation of BT3 Clock
												Engine                                    */
	unsigned short SPA_Reg;         /*Status Reporting Registers (see Report    */
	unsigned short SPB_Reg;         /*_Status in Task.c for definitions )       */
	unsigned short GPS_Reg;         /*                  ""                      */
	unsigned short ROA_Reg;          /*                  ""                      */
	unsigned short ROB_Reg;          /*                  ""                      */
	unsigned short TP_Reg;          /*                  ""                      */
	unsigned short PL_Reg;          /*    Registers to report power level       */
	unsigned short AT_Reg;          /*    and antenna current see status_rept() */
	/**** Latched registers status latched until explicitly cleared by setting
			 CLR_Reg TRUE
	*****/
	unsigned short SPA_Lreg;         /*Status Reporting Registers (see Report    */
	unsigned short SPB_Lreg;         /*_Status in Task.c for definitions )       */
	unsigned short GPS_Lreg;         /*                  ""                      */
	unsigned short ROA_Lreg;          /*                  ""                      */
	unsigned short ROB_Lreg;          /*                  ""                      */
	unsigned short TP_Lreg;          /*                  ""                      */
	unsigned short PL_Lreg;          /*    Registers to report power level       */
	unsigned short AT_Lreg;          /*    and antenna current see status_rept() */
	unsigned short CLR_Lreg;          /* Controls clearing of latched register status*/
	unsigned short Overide_Mode;    /*control mode of writing to shared memory from
												 external requests
												 0: don't look at overides
												 1: look at overides once reverts to zero
												 2: continously look at overides
											  **************************************/

	unsigned short time_acc_est;    /*95% Confidence Estimate of
												 clock bias timing error OUTPUT           */
	unsigned short BT3_mode_dur;    /*Duration in minutes of mode*/
	/****************************************************************************
	  The following threshold data elements are bidirectional and permit the
	  user real time adjustment of input qualification thresholds within
	  specified limits. The update rate is 1 second. If value is rejected old
	  value will be reported. If the users enters zero then the system will
	  return to default for that threshold.
	****************************************************************************/
	unsigned short Sdelmax;    /*Maximum allowed peak to peak short term jitter
										  in NS Range 100-2000 NS*/
	unsigned short Sfreqtau;  /* Smoothing filter TC to establish best measurement
										  SPAN-LO stability used to smooth inputs to PLL and
										  measuring drift stability and as a baseline to
										  check for phase  steps (Units of seconds)
										  Range 10-60 seconds. */
	unsigned short Sdriftmax;  /*Maximum allowed frequency step over the specified
										  integration time (units ppb) Range 1-200  */
	unsigned short Sfreqmax;   /*Maximum allowed frequency bias between LO and
										  SPAN must accomodated local oscillator aging
										  units ppb Range 6000-20000*/
	unsigned short Sphasemax;  /*Maximum allowed phase step over a 250 ms
										  interval with respect to the normal phase ramp
										  over the specified integration
										  time (Sdriftcheck) units NS Range 10-2000NS*/
	unsigned short Gfreqmax;   /*Maximum allowed frequency bias between LO and
										  GPS must accomodated local oscillator aging
										  units ppb Range 6000-20000*/
	unsigned short Gdriftmax;  /*Maximum allowed frequency step over a fixed 1
										  second integration interval units ppb
										  Range 1-200*/
	unsigned short ROdelmax;    /*Maximum allowed peak to peak short term jitter
										  in NS Range 10-2000 NS*/
	unsigned short ROdriftcheck;/*Integration time to establish best measurement
										  LO-RO stability used in measuring drift
										  stability and as a baseline to check for phase
										  steps (Units of seconds) Range 1-60 seconds.  */
	unsigned short ROdriftmax;  /*Maximum allowed frequency step over the specified
										  integration time (units ppb) Range 1-200  */
	unsigned short ROfreqmax;   /*Maximum allowed frequency bias between LO and
										  RO must accomodated local oscillator aging
										  units ppb Range 2000-20000*/
	unsigned short ROphasemax;  /*Maximum allowed phase step over a 250 ms
										  interval with respect to the normal phase ramp
										  over the specified integration
										  time (ROdriftcheck) units NS Range 10-2000NS*/
	unsigned short TPdelmax;    /*Maximum allowed peak to peak Temperature Step
										  in Range 1-10 units*/
	unsigned short TPslewcheck; /*Number of TAU Zero (250)ms intervals to integrate slew
											rate measurement for Temperature*/
	unsigned short TPstepmax;  /* maximum allowed temperature step on input 0-1024
										 temperature scale */
	unsigned short TPslewmax;  /* maximum allowed temperature slew on input 0-1024
											 temperature scale */
	unsigned short TCcycle;    /* Flag controlling start of
											temperature calibration cycle*/
	unsigned short One_PPS_Lead; /*bit indicating if output One_PPS is leading
											 input 1PPS*/
	unsigned short One_PPS_Range; /*bit indicating if output One_PPS is out of
											  normal range */
	short One_PPS_Mag;            /*Indicates magnitude of PPS Error 0-16*/
  	short One_PPS_phase;    /*Indicates magnitude of PPS phase*/
//  	short Last_PPS_phase;    /*Indicates magnitude of PPS phase*/

	unsigned short One_PPS_High;  /*Indicates output PPS exceeds high threshold
											  CDMA requirements*/

	unsigned short One_PPS_Jam;   /*Jam request to force 1PPS FPGA output into
											  alighment with the input*/

	unsigned short R0B_Select;   /*Selects either EXTERNAL input or local MASTER
                                  oscillator input for Remote B channel meas*/

	short One_PPS_Est_RO;         /*Composite  estimate of 1PPS from remote*/

	unsigned short LO_PPS_Data_Valid; /*indicates local data is validated*/

	unsigned short RO_PPS_Data_Valid; /*indicates remote data is usuable good
													MCCI link for OCXO data and both good
													MCCI link and Veried data for GPS data*/

	unsigned short Input_Mode;       /* provisions what input resources should
													be used in operation. Four modes encoded
													in lower byte All, GPS Only, Span Only,
													Remote Osc. Only. Upper byte controls
													disable overides:
													 bit 0 disable GPS
													 bit 1 disable SPA bit 2 disable SPB
													 bit 3 disable ROA bit 4 disable ROB
													When asserted a disable prevents resource
													from being part of output ensemble.*/
	unsigned short GPS_Locked;      /* True if in Gear 3, GPS is valid and measurement is 
									* less than +/- 100 nsecs
									*/  

};


#endif // _BT3_DAT_H_04_09_2003

