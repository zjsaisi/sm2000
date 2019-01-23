/***************************************************************************
$Header: bt3_def.h 1.10 2010/12/22 16:42:24PST Jining Yang (jyang) Exp  $
$Author: Jining Yang (jyang) $
$Date: 2010/12/22 16:42:24PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_def.h_v  $
$Modtime:   16 Feb 1999 15:49:22  $
$Revision: 1.10 $


                            CLIPPER SOFTWARE
                                 
                                BT3_DEF.H

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module maintains all global definitions for MCCA

MODIFICATION HISTORY:

$Log: bt3_def.h  $
Revision 1.10 2010/12/22 16:42:24PST Jining Yang (jyang) 
George updated Rb time constants.
Revision 1.9 2010/10/13 23:58:06PDT Jining Yang (jyang) 
Merged from 1.8.1.1.
Revision 1.8.1.1 2010/08/04 23:24:12PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.8 2009/06/11 10:46:42PDT Zheng Miao (zmiao) 
George's change
Revision 1.7 2009/06/01 09:14:44PDT zmiao 
Change pullin range according OSC type.
Revision 1.6 2009/04/23 09:48:31PDT zmiao 
One more GZ change
Revision 1.5 2009/03/27 09:32:23PDT zmiao 
George Z's change
Revision 1.4 2009/03/26 09:40:12PDT zmiao 
Change some constants according to George Z.
Revision 1.3 2008/04/01 09:14:29PDT gzampetti 
Modified to support proper configuration under change to LOTYPE. Added Trace_Skip parameter to mitigate
transients when switching trace sources
Revision 1.2 2008/03/27 07:48:43PDT gzampetti 
new time constants
Revision 1.1 2007/12/06 11:41:05PST zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.17 2006/07/19 10:14:19PDT gzampet 
add synthesizer control
Revision 1.16 2006/05/11 16:06:43PDT gzampet 
tweaked to improve transient protection
Revision 1.15 2006/05/02 12:50:47PDT gzampet 
Updated to support the BT63 command and general startup issues in the lower gears
Revision 1.14 2006/04/28 07:26:27PDT gzampet 
added extended phase slew control
Revision 1.13 2006/04/26 13:42:55PDT gzampet 
32 bit phase control
Revision 1.12 2006/04/25 14:17:51PDT gzampet 
max holdover bias now 2ppm max pps slew rate is 200ps/s
Revision 1.11 2006/04/21 14:41:23PDT gzampet 
make single and double oven timeconstants the  same 500 second ss proportional ...
Revision 1.10 2006/04/21 13:09:15PDT gzampet 
slower TC
Revision 1.9 2006/04/21 10:51:25PDT gzampet 
new slower timeconstants
Revision 1.8 2006/03/06 14:26:33PST gzampet 

Revision 1.7 2006/03/06 11:46:56PST gzampet 

Revision 1.6 2006/03/06 10:39:27PST gzampet 

Revision 1.5 2006/03/06 10:23:09PST gzampet 

Revision 1.4 2006/03/01 08:20:42PST gzampet 

Revision 1.2 2006/02/16 13:58:37PST gzampet 

Revision 1.1 2005/09/01 14:55:10PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.23 2004/12/21 15:43:06PST gzampet 

Revision 1.22  2004/12/10 15:39:54Z  gzampet
Revision 1.21  2004/12/09 21:56:48Z  gzampet
Revision 1.20  2004/12/09 01:08:41Z  gzampet
Revision 1.19  2004/12/01 15:36:51Z  gzampet
Revision 1.18  2004/10/18 17:40:08Z  gzampet
Revision 1.17  2004/10/14 21:47:25Z  gzampet
Revision 1.16  2004/09/20 21:03:31Z  gzampet
Revision 1.15  2004/09/17 17:41:50Z  gzampet
Revision 1.14  2004/09/17 16:36:15Z  gzampet
Revision 1.13  2004/08/27 15:12:00Z  gzampet
Revision 1.12  2004/07/30 23:58:54Z  gzampet
Revision 1.11  2004/07/30 14:25:56Z  gzampet
Revision 1.10  2004/07/29 18:11:45Z  gzampet
Revision 1.9  2004/07/01 22:21:36Z  zmiao
Revision 1.8  2004/07/01 13:11:16Z  gzampet
Revision 1.7  2004/06/24 16:48:46Z  gzampet
fully merge
Revision 1.6  2004/06/24 14:56:13Z  gzampet
partial merge with TS3X 
Revision 1.5  2004/06/23 15:13:51Z  gzampet
Baseline for GPS BesTime Version
Revision 1.4  2003/05/29 23:07:43Z  gzampet
Use 1 hour holdover update
Revision 1.3  2003/05/22 19:20:50Z  gzampet
Revision 1.2  2003/04/24 14:59:11Z  gzampet
Revision 1.1  2003/04/14 17:06:58Z  zmiao
Initial revision
Revision 1.15  2002/10/01 00:03:23Z  zmiao
Added CLEI length as 10
Revision 1.14  2002/09/13 21:09:25Z  gzampet
Added smart clock fm defines to set modulation during holdover to
keep predictors in nominal range
Revision 1.13  2002/09/06 00:20:18Z  gzampet
Modified DeltaFix and DeltaBias for both T1 and E1 to support
new CSM-RTHC measurement scheme
Revision 1.12  2002/09/03 18:55:06Z  zmiao
Added a #define
Revision 1.11  2002/08/16 19:34:51Z  gzampet
Changed SMART_WIDE and SMART_NARROW thresholds
Revision 1.10  2002/08/09 22:45:23Z  gzampet
Adjusted GFREQMAX from 1 to 11ppm to accomodate RTHC ST3E operation
Added SMARTCLK Defines for smart clock states
Revision 1.9  2002/07/09 19:04:49Z  zmiao
FOR GPZ
Revision 1.8  2002/07/05 22:17:57Z  zmiao
GZ's second wave change
Revision 1.7  2002/06/10 21:03:11Z  kho
1. Changed TPSLEW BUCKET UP/DOWN to 200
2. Changed TPSLEWMAX to 10.0.
Revision 1.6  2002/06/07 17:57:01Z  kho
This revision will help OCXO lock more smoothly
1. MAX_PPS_JAM was lowered to 200nsecs but will only occur once a minute
2. GDRIFTMAX_XO was lowered to 100nsecs
3. TPSLEWCHECK was changed to 1 mintute
4. TPRANGECHECK value was not change but was rescaled to mA
5.TPSLEW bucket filter was removed
Revision 1.5  2002/06/04 22:11:13Z  kho
Loosened parameters for GPS lock in order to lock to an OCXO that
is not yet centered MAXJAM back to 20 and GDRIFTMAX_XO up to 2e-7
Revision 1.4  2002/06/04 17:11:28Z  kho
Changed MAX_PPS_JAM   back to 4.
Revision 1.3  2002/06/03 22:23:43Z  kho
Change MAX_PPS_JAM from 8 to 20.  This will give gear 2 a 
 chance to settle.
--- Added comments ---  kho [2002/06/03 22:25:47Z]
Changed the leaky bucket values for GFREQ and GDRIFT.
Changed GDRIFTMAX_XO from 1e-6 to 1e-7.  This will take care 
of warmup.
Revision 1.2  2002/05/31 18:12:38Z  kho
Hitachi 1.01.01 build sent out 5/21/2002
Revision 1.1  2001/12/11 19:28:46Z  kho
Initial revision
 * 
 *    Rev 1.1   19 Feb 1999 10:58:46   S.Scott
 * For initial release. Change acceptable time accuracy threshold to 
 * get rid of the once an evening GPS error and Holdover event.
 * 
 *    Rev 1.0   06 Feb 1999 15:41:38   S.Scott
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
*     Multi tracking Core Control Algorithm  (MCCA) renamed to BT3 see History
*     Module: Dpll_Def.c
*     Description: Module maintains all global definitions for MCCA
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
*      Version 1.3
*                   1) Changed FCONV to reflect gain in integer based filter.
*      Version 1.4
*                   1) Added an additional channel to stability analysis to
*                      provide for future growth.
*                   2) Defined MINRESET and MAXRESET for MTIE calculation
*      Version 1.5  5/20/96
*                   1) Changed FCONV to reflect gain in integer based filter.
*                   2) Added PRIMDELAY to control delay into primary loop
*
*      5/23/96 Module Renamed MCCA_Def.c as part of cellsync 2000 project
*              revision history of this new name starts with 1.0
*
*      Version 1.0 5/23/06
*                   1) Add CELLSYNC2000 Project Definition
*                   2) Add RODELMAX default threshold
*                   3) Add ROBUCKET up and down for jitter detection
*                   4) Add RO_FCONV normalization constant
*                   5) Add Remote Oscillator Phase Freq and Drift Leaky Bucket
*                      control defines
*                   6) Add Time Constant Range limits explicitly for CELLSYNC2000
*                   7) Add TPDELMAX default threshold
*                   8) Add TPBUCKET up and down for jitter detection
*                   9) Add TPSTEPMAX, TPSLEWMAX and TPSLEWCHECK default thres.
*                  10) Add MAXPHASESLEW MINPHASESLEW and NOCROSSINGCNT to
*                      manage 1PPS phase control.
*       Version 2.0
*       Version 2.1
*                   1) Added PCSIM global def to control which base to compile
*
*       Version 2.2
*                   1) Add NCO_CENTER and NCO_CONVERT Global definition
*       Version 2.5
*                   1) Add calibration state definitions
*         MODULE RENAME BT3_DEF.H 9-15-97  as part of the BT3 Core Clock Engine
*         Project.
*     Version 1.0 Baseline BT3 clock engine
*                        1) No Change
*                        2) Remove ATT_TFG build option and associated code
*                           to make BT3 source code more manageable
*     Version 3.0
*       	               1) Added ANTENNA_MODE define to select either rooftop
*                          or window base installation
*                       2) Added precision to T1DELTABIAS definition use to be
*                          eight digits -.18652849 now 15 digits
*
*******************************************************************************/

#ifndef _BT3_DEF_H_04_09_2003
#define _BT3_DEF_H_04_09_2003

/************** START BUILD RELATED CONFIGURATION INFO ************************/
#define PCSIM 0    /*global control to use simulator tool under PC */
#define HOTDLOAD 0 /*used during test to restart after download in final gear*/
/****** Select Hardware  ****************/

/****** Select Target  ****************/
#define BT3_XR5   0
#define CLPR      1
#define XRCLP     2
#define TARGET CLPR
/******* Pick LO nominal frequency ******/
#define LO5MHZ  0
#define LO10MHZ 1
#define LO5_1525MHZ 2
#define LOFREQ LO10MHZ
/****** Select LO Type *****************/
#define OCXO  0
#define DOCXO 1
#define RB    2
#define LOTYPE DOCXO
/*******Select Antenna Installation*****/
#define ROOFTOP 0
#define WINDOW  1

#define ANTENNA_MODE WINDOW /*ken always use window*/
/*******Select Temperature Calibration Mode ***/
#define STATIC_TEMP 1 /*true to use temperature table else use adapative*/
/************** END BUILD RELATED CONFIGURATION INFO *************************/
// !!!!!!!!! Set to 1 for release holdover acceleration in effect for test
#define NORMAL_HOUR 1 /*use normal hourly holdover update*/
#if PCSIM
#define PRINTLOG 1 /* global control of printing status information*/
#else
#define PRINTLOG 0
#endif
#define PHEAD "---->" /*standard printing header*/
#define PRINTMAX 60 /*maximum print messages per hour */
#if NORMAL_HOUR
#define MIN_PER_HOUR 59 /*normal minutes per hour*/
#define HOUR_FREQ_CNT_MIN  1000  /*minimum number of 1sec updates before
											  qualifying and hour freq count to include
											  in the holdover regression*/
#else
#define MIN_PER_HOUR  4 /* default 9 accelerate minute per hour for holdover test*/
#define HOUR_FREQ_CNT_MIN 30
#endif
/*********************************************************
The first application for this software was an OEM single card
Timing Frequency Generator (TFG) for the AT&T Fixed Wireless Application
This was first built in December 1995 and the project was halted since
AT&T change direction. The ATT_TFG build option has not been tested on the
new CellNet and CellSync 2000 platforms so use with care!.
The CellNet build option was added next to support first truly "BestTime"
application (Dual E1/T1 and GPS inputs) The CellNet name is historical. The
proper name is CellSync1000.
5/23/96 the build option for the CELLSYNC2000 project was added this supports
Dual E1/T1, GPS and Remote Oscillators.
The ATT_TFG Option is deleted from the BT3 Core Engine Build as it is
obsolete.
*********************************************************/
#define MAXCHAN 3  /*Maximum analysis channels*/
  /***** The following defined the analysis and control measurement channels **/
#define GPS_LO   0
#define SA_LO    1
#define SB_LO    2
#define RA_LO    3
#define RB_LO    4

#define E1 0             /*E1 span type */
#define T1 1            /* T1 span type */
#define SPANTYPEA E1   /*default span type */
#define SPANTYPEB E1    /*default span type */
#define REMOTE 0  /*if true then remote oscillator control loop is operational
						  under this setting the R0A measurements are used generate
						  an ensembled frequency correction from a mate system without
						  GPS. This mode was required by LUCENT in the RFTG-u to steer
						  the mate module. The mate module reports over the MCCI link
						  required temperature correction data and 5MHz RO signal*/
#define ALL_INPUTS  0 /*all valid inputs are provisioned for use*/
#define SPAN_ONLY   1 /*only use span inputs*/
#define GPS_ONLY    2 /*only use GPS input*/
#define RO_ONLY     3 /*only use Remote Oscillator Inputs*/
#define DEFAULT_SET 0xFF /*command input to return to default settings*/
#define OVERRIDE_ACCEPT 1
#define OVERRIDE_REJECT 2
#define OVERRIDE_CLEAR  0
#define CLOOPS      5 /*number of control loops in application*/
#define SPA_LOOP 1   /* Control loop indexes*/
#define SPB_LOOP 2
#define ROA_LOOP 3
#define ROB_LOOP 4
#define GPS_LOOP 0
#define MDEV  0   /*shared memory requesting MDEV stats */
#define TDEV  1   /*shared memory requesting TDEV stats */
#define AFREQ 2   /*shared memory requesting average frequency stats */
#define MTIE  3   /*shared memory requesting MTIE stats */
#define NONE 99   /*shared memory requesting to do nothing (MTIE Reset)*/
#define ALL  100   /*shared memory requesting to reset all channels*/
#define BUFMAX 49  /*Related to largest TAU (N*T0) by 3*N+1
			  N=16 for this analyzer*/
#define NMAX  16  /* Highest TAU Lag supported in buffer*/
#define TAUCAT 5   /* Number of TAU Categories for each buffer
				[1,2,4,8,16]*T0 */
#define TOTAL_TAUCAT 15 /*Total TAU Categories TAUCAT * 3 buffers */
#define TOTAL_STATS  46 /*total bundles stats per channel TOTAL_TAUCAT *3+1
								  for phase value at index 0 and MTIE,AFREQ,and MVAR stats
								  TVAR can be easily calculated from MVAR so its not reported*/
#define SMOOTH_FACTOR 40.0 /*Can be viewed as the number of "decorrelated"
									  input samples to be included in the
									  variance estimates. See initialization for
									  exact relationship*/
#define BUFSCALE 1.0e10 /*scaling factor used in stab ana to permit 32bit buffers
								 to be used to save RAM space*/
#define MAXCALRAMPS 6  /*maximum number of calibration up and down ramps*/
#define MAXTEMPRECORDS 256 /*maximum number of temperature calibration records
									  temperature range is 0-1024 each record covers 4
									  input temp grid points */
#define MAX_SERIAL_NUM 20 /* MAX length of serial number string */
#define MAX_CLEI_NUM	10 /* Max length of CLEI code */
#define MAXFREQSLEW 126   /* all valid freq values in calibration database less
									  than this level */
#define TEMPERATURE_CACHE_LENGTH 64 /* partition cache size for
													 temperature comp normally 64*/
/*#if (LOTYPE==OCXO)*/
#define MAXPHASESLEW_XO 5.0e-10 /* GPZ JUne 2010 Increase from 2e-10 to 5e-10 
//#define MAXPHASESLEW_XO 5e-10 /*Maximum output 1PPS slew rate during acquisition
//										 normally 1e-9*/
//#define MINPHASESLEW_XO 5e-12 /*Minimum output 1PPS slew rate  was 2e-12*/
#define MINPHASESLEW_XO 2e-11   /*Minimum output 1PPS slew rate  was trying to make it more on edge (kjh)*/
#define NOCROSSINGCNT_XO 800    /* number of consective non-zero crossing update
										 before increasing gain (800)*/
/*#else*/
#define MAXPHASESLEW_RB 5.0e-10 /* GPZ June 2010 increase from 1e-10 to 5e-10  */
#define MINPHASESLEW_RB 1e-11   /* GPZ Dec 2010 need to increase from 1e-12 to address popcorn noise in MAC Rb*/
#define NOCROSSINGCNT_RB 1800   /* number of consective non-zero crossing update
										 before increasing gain (1800)*/
/*#endif*/   

#define MAX_PPS_FLYWHEEL 220   /*Maximum PPS Phase Error in Lucent Application*/
#define MAX_PPS_NORMAL 40      /*was 10 */
#define MAX_PPS_SINGLE 60		 /*was 140*/
#define MAX_PPS_RANGE  60		 /*was 280*/
#define MAX_PPS_JAM   3
/*#define MAX_PPS_BIAS_RB  1.0e-6/*maximum bias error between XR-5 clockbias state and
										 BT3 clockbias estimate  was 3e-6*/
#define MAX_PPS_BIAS_RB  1.0e-5/*maximum bias error between XR-5 clockbias state and
										 BT3 clockbias estimate increased to prevent recentering
										 except under true large reinit situations GPZ 10-01-2001 */
#define MAX_PPS_BIAS_XO  1.0e-5/*maximum bias error between XR-5 clockbias state and
										 BT3 clockbias estimate  was 3e-6*/
#define MAX_PPS_BIAS  MAX_PPS_BIAS_XO	/*for now jch 12/20/00 */


#define MAXSTATIN 700000000L /*Maximum input range before renormalizing stats*/
#define MAXRESET 0x80000000L
#define MINRESET 0x7fffffffL 

//#define TAUZERO  0.250       /*Update interval for 250ms processes
//										must call every TAUZERO seconds  */
#define TAUZERO  1.0       /*Update interval for 1 second processes (kjh 3/5/02)
										must call every TAUZERO seconds  */
#define SHORT_LEN 0
#define MED   1
#define LONG_LEN  2
#define MASTER_B 0
#define EXTERNAL_B 1      /* Source of ROB channel data*/
#define LBUCKETMAX   1000
#define LBUCKETMIN   0
#define LBUCKETTHRES 750
#define LBUCKETHYS   50 /* leaky bucket hysteresis  */
#define MBSIZE 256 /* size of General purpose Measurement Buffers
							 should always be 256 to simplify modulo operation*/
//#define MASIZE1  137 /*size of moving average buffer in freqest filter 1*/
//#define MASIZE2  255  /*size of moving average buffer in freqest filter 2*/
#define MASIZE1  23 /*size of moving average buffer in freqest filter 1*/
#define MASIZE2  42  /*size of moving average buffer in freqest filter 2*/
#define MATSIZE1 37  /*size of moving average buffer in freqest dtime filter 1*/
#define MATSIZE  37  /*size of moving average time filter is RO_Freq_Est */
#define NDATAMIN 20 /* minimum number of new data points in SPAN FIFO if FIFO
								is less than this then declare as jitter problem */
#define RO_NDATAMIN 25 /* minimum number of new data points in RO FIFO if FIFO
								is less than this then declare as jitter problem
								normally 30*/
#define TP_NDATAMIN 11 /* minimum number of new data points in TP FIFO if FIFO
								is less than this then declare as jitter problem */
#define PL_NDATAMIN 11
#define AL_NDATAMIN 11

#define TSTROBE_DS1  (double)6436.0/1544.0 /* strobe period in ms */
#define TSYST        (double) 1.0/30.0e3     /* sys clock period in ms */
#define T1DELTAFIX   24824	/*same as E1  was 13107 for TimeProvider*/
//#define T1DELTABIAS  (double)(0.2*(25.0/28.0)*MASIZE1*MASIZE2*5.0) /*same as E1*/
#define T1DELTABIAS  (double)((8.0/33.0)*(25.0/28.0)*MASIZE1*MASIZE2*5.0) /*same as E1*/

#define TSTROBE_E1  (double)8531.0/2048.0 /* strobe period in ms */
#define E1DELTAFIX   24824	/* integer correction was 13107*/
//#define E1DELTABIAS (double)(0.2*(25.0/28.0)*MASIZE1*MASIZE2*5.0)
#define E1DELTABIAS (double)((8.0/33.0)*(25.0/28.0)*MASIZE1*MASIZE2*5.0)
#define TSTROBE_CAR (double)(32000.0/(30.0e6/71.0 - 5.0e6/12.0))/*nominal period of carrier strobe*/

#define SDELMAX 1000 /* Maximum Allowed SPAN Delta Count accomodates jitter*/
#define SBUCKETUP 1
#define SBUCKETDOWN 10

#define RODELMAX 25 /* Maximum Allowed Remote Oscillator Delta Count accomodates
							  worse case offset*/
#define TPDELMAX 10  /* Maximum Allowed Temperature Delta Count accomodates jitter and
								worse case slew rate*/
#define PLDELMAX 10 /* Maximum Allowed Power Level Delta Count to prevent
							  overflow in filter*/
#define ALDELMAX 14 /* Maximum Allowed Power Level Delta Count to prevent
							  overflow in filter*/

#define ROBUCKETUP 5 /*Normally 1 set to 5 to handle occasional jumps*/
#define ROBUCKETDOWN 10
#define TPBUCKETUP 2
#define TPBUCKETDOWN 1
#define PLBUCKETUP 1
#define PLBUCKETDOWN 10
#define ALBUCKETUP 1
#define ALBUCKETDOWN 10


/*#if (LOTYPE!=RB)*/
#define GFREQMAX_XO  2.1e-6   /* Maximum GPS Freq Threshold*/       
#if 0
#define GDRIFTMAX_XO 5.0e-7   /* Maximum GPS Delta Freq Threshold */
#endif
 #define GDRIFTMAX_XO	2.5e-5
// zmiao 6/1/2009: Separate number for Quartz and Rubidium */
#define SFREQMAX_XO  4.0e-6   /* Maximum SPAN Freq Threshold for Quartz OSC */
/*#else*/
#define GFREQMAX_RB  2.0e-8 /*Maximum GPS Freq Threshold*/                 
#if 0
#define GDRIFTMAX_RB 5.0e-7   /* Maximum GPS Delta Freq Threshold */
#endif
#define GDRIFTMAX_RB		2.5e-5
#define SFREQMAX_RB  2.0e-8 /* Maximum SPAN Freq Threshold for Rubidium OSC */
/*#endif*/
#define GFREQBUCKETUP    25 /*was 25 */
#define GFREQBUCKETDOWN  70 /*was 2  */
#define GDRIFTBUCKETUP   5 /*GZ 4/23/2009: Changed from 20*/ /*was 25 */
#define GDRIFTBUCKETDOWN 200 /*GZ 4/23/2009: Changed from 100*/ /*was 2  */
//#define SPHASEMAX 1e-6 /*Maximum SPAN Phase Step Threshold 250 ms Freq Deviation
//								 from Ye1_avg*/
// zmiao 3/26/2009: According to George Z. 
#define SPHASEMAX 2e-6 /* 1e-6 */ /*Maximum SPAN Phase Step Threshold 250 ms Freq Deviation
								 from Ye1_avg*/
// zmiao 3/26/2009: According to GZ
//#define SFREQMAX	g_freqMax
extern double g_freqMax;
//#define SFREQMAX  4.0e-6 /* 2.1e-6 */  /*Maximum SPAN Freq Threshold was 12.1ppm*/

#define SDRIFTMAX 1e-7 /* 2e-8 */ /* Maximum SPAN Drift Threshold */

#define SFREQTAU 30     /*smoothing time constant (sec) for input frequency data to
								  drift detection and transient PLL FIFOs was 15*/
//#define SDRIFTCHECK 16  /*Number of TAUZERO intervals between Drift Checks*/
#define SDRIFTCHECK 4 /*Number of TAUZERO intervals between Drift Checks (kjh 3/5/02)*/
#define ROPHASEMAX 1e-7 /*Maximum RO Phase Step Threshold 250 ms Freq Deviation
								  from Ye1_avg*/
#define ROFREQMAX  2e-6 /*Maximum RO Freq Threshold*/
#define RODRIFTMAX 2e-8 /* Maximum RO Drift Threshold */
#define RODRIFTCHECK 60 /*Number of TAUZERO intervals between Drift Checks*/
#define TDELTA   16     /*Number of ms per update of A/D based measurements*/
#define TPSTEPMAX 12.0     /* Maximum allowed Temperature Step*/
#define TPSLEWMAX 10.0    /* Maximum allowed Temperature Slew over Slew Interval (60)*/
#define TPSLEWCHECK 60 /* Number of TAUZERO intervals between Slew Checks (every minute)*/
#define TPRANGECHECK 650.0 /* saturation threshold error used for OCXO warmup */
#define TPNOMINAL  80   /* Nominal inner oven current when in CLPR application*/
#define PLSTEPMAX 12.0      /* Maximum allowed Power Step was 5.0 8-3-98*/
#define ALSTEPMAX 12     /* Maximum allowed Antenna level Step*/

#define PLRANGEMAX_HIGH 950.0 /*970.0*/
#define PLRANGEMAX_LOW  900   /*921.0*/
#define PLRANGEMIN_HIGH 50.0 /*51.0*/ /* set for 90% adjustment */
#define PLRANGEMIN_LOW  100.0 /*102.0*/
#define ALRANGEMAX  864.0
#define ALRANGEMIN  224.0


#define RPHASEBUCKETUP 1
#define RPHASEBUCKETDOWN 10
#define RFREQBUCKETUP 10
#define RFREQBUCKETDOWN 70
#define RDRIFTBUCKETUP  25
#define RDRIFTBUCKETDOWN 100
#define TPSTEPBUCKETUP   10
#define TPSTEPBUCKETDOWN 70
#define TPSLEWBUCKETUP   200    
#define TPSLEWBUCKETDOWN 200     
#define PLSTEPBUCKETUP 100
#define PLSTEPBUCKETDOWN 100
#define PLRANGEBUCKETUP 100
#define PLRANGEBUCKETDOWN 100
#define PPSRANGEBUCKETUP 200
#define PPSRANGEBUCKETDOWN 100

#define ALSTEPBUCKETUP 10
#define ALSTEPBUCKETDOWN 70
#define ALRANGEBUCKETUP 50
#define ALRANGEBUCKETDOWN 100

#define SPHASEBUCKETUP 1
#define SPHASEBUCKETDOWN 10
#define SFREQBUCKETUP 10
#define SFREQBUCKETDOWN 70
#define SDRIFTBUCKETUP   25
#define SDRIFTBUCKETDOWN 75 /*GPZ was 15*/



#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
//#define SPAN_FCONV (double)(MATSIZE1/(30e3*5.0*MASIZE1*MASIZE2))
									  /* Span Conversion factor for BT3 Engine 1000.0
										  divide by 30e6 * 5*13*2^22*/
//#define SPAN_FCONV (double)(-1.3802622498274672e-12) /* 1/(150e6)/(5)/(23)/(42) */
//#define SPAN_FCONV (double)  (-1.3164350960489169e-12) /* 1/(157.272727e6)/(5)/(23)/(42) */

#define SPAN_FCONV (double)(-1.30315745098991e-12) /* 1/(10.24e6*512/33)/(5)/(23)/(42) */  
#define SPAN_BIAS_35  (double)((8.0/33.0)*(35.0)*5.0*23.0*42.0*(double)(-1.30315745098991e-12))
#define SPAN_BIAS_36  (double)((8.0/33.0)*(36.0)*5.0*23.0*42.0*(double)(-1.30315745098991e-12))
#define SPAN_TRIM_35 (double)(1.0/0.98)   /* minor compensation for meas time slightly less than 1 second*/
#define SPAN_TRIM_36 (double)(1.0/1.008)  /*minor compensation for meas time slightly greater than 1 second*/


#define RO_FCONV (double) (MATSIZE/(2097152.0*5.0*2.13e6))
						  /* The relationship between the average Delta over 1ms and
						  the fractional freq error is Avg_Delta = 2.13e6*(Yro-Ylo)
						  per ms. The gain in the digital filter is 2^21 * 5 so :
						  Calc_Delta= 2^21*5* Avg_Delta and (Yro-Ylo)= Calc_Delta/
						  (2^21 *5* 2.1e6) or Calc_Delta * 4.477344e-14 */

#define TP_FCONV 1.52587890625e-6  /*Conversion from accumulated delta
							temperature to current delta temperature estimate.
							The gain in the digital filter
							is 2^21*5. The gain normalized to the 8 ms update is:
							16/(2^21*5) or 3.8146972656e-7.*/

#define PL_FCONV 1.52587890625e-6  /*Conversion from accumulated delta
							temperature to current delta temperature estimate.
							The gain in the digital filter
							is 2^21*2. The gain normalized to the 8 ms update is:
							16/(2^22*5) or 7.62939453125e-7.*/

#define AL_FCONV 1.52587890625e-6  /*Conversion from accumulated delta
							temperature to current delta temperature estimate.
							The gain in the digital filter
							is 2^21*2. The gain normalized to the 8 ms update is:
							16/(2^22*5) or 7.62939453125e-7.*/
 
/* GPZ March 2006 add support for TimeCreator Varactor Steered Oscillator*/
#define MAX_VAR_SENSITIVITY (double)(1.5E-11)  /*maximum LSB sensitivity over the control range of the CMAC oscillator*/
#define MAX_VAR_CONVERSION (double)(1.0/1.5E-11)  /*convert from fractional frequency to bits*/  
#define MPLL_SUM_SECOND (double)(10000.0/256.0) /*number of control words summed over a second*/
#define VAR_CENTER 0x8000L 
#define VAR_RANGE 0x10000L

/*if(LOFREQ == LO10MHZ)*/
 #define NCO_CENTER_10 0x2AAAAAAAL  /*base setting for nominal 10 MHz NCO output for
											 BT3 applications using 60MHz system clock input to NCO*/
 #define DITHER_BIAS_10 (double)(2.0/3.0) /*additional compensation for for dither bias*/
 #define NCO_MAX_10    0x2AAAB041L /* - 2PPM frequency correction*/
 #define NCO_MIN_10    0x2AAAA513L /* + 2PPM frequency correction*/
 #define NCO_RANGE_10  0x5000L      /*maximum linear conversion range 28ppm*/
 #define NCO_CONVERT_10 (double)(-2147483648.0/3.0)/*conversion factor from
												NCO fractional freq correction to DELTA bit o
												offset from NCO_CENTER in BT3 applications*/
/*#elif(LOFREQ == LO5MHZ)*/
 #define NCO_CENTER_5 0x15555555L  /*base setting for nominal 10 MHz NCO output for
											 BT3 applications using 60MHz system clock input to NCO*/
 #define DITHER_BIAS_5 (double)(1.0/3.0) /*additional compensation for for dither bias*/
 #define NCO_MAX_5    0x15555B55L /* - 2PPM frequency correction*/
 #define NCO_MIN_5    0x15554955L /* + 2PPM frequency correction*/
 #define NCO_RANGE_5  0x5000L      /*maximum linear conversion range 28ppm*/
 #define NCO_CONVERT_5 (double)(-1073741824.0/3.0)/*conversion factor from
												NCO fractional freq correction to DELTA bit o
												offset from NCO_CENTER in BT3 applications*/
/*#elif (LOFREQ == LO5_1525MHZ)*/
 #define NCO_CENTER_51 0x15FBE76CL  /*base setting for nominal 5.1525MHz NCO output for
											 BT3 applications using 60MHz system clock input to NCO*/
 #define DITHER_BIAS_51 (double)(0.544) /*additional compensation for for dither bias*/
 #define NCO_MAX_51    0x15FBEB00L /* - 2PPM frequency correction*/
 #define NCO_MIN_51    0x15FBE500L /* + 2PPM frequency correction*/
 #define NCO_RANGE_51  0x1000L      /*maximum linear conversion range */
 #define NCO_CONVERT_51 (double)(-1073741824.0*(687.0)/(2000.0))/*conversion factor from
												NCO fractional freq correction to DELTA bit o
												offset from NCO_CENTER in BT3 applications*/
/*#else
  #endif*/

//#define NCO_LOW_ADR0   (*(unsigned char *)0x440001)
//#define NCO_LOW_ADR1   (*(unsigned char *)0x440002)
//#define NCO_HIGH_ADR0  (*(unsigned char *)0x440003)
//#define NCO_HIGH_ADR1  (*(unsigned char *)0x440004)
//#define NCO_DITHER_ADR (*(unsigned char *)0x440000)

/*****************************************************************/
#define SOL	299792458.0 /*Speed of Light meters per sec same as XR5*/
/*********************** Calibration State Definitions *****************/
#define CAL_None   0  /*no calibration data available*/
#define CAL_Update 1 /*Calibration data updated in ram structure but not in NVM yet*/
#define CAL_Stable 2/*Calibration data is stable ram and nvm agree*/
#define CAL_Reset  3/*Calibration NVM reset request*/
#define CAL_Dnld   4/*Calibration  flag indicates stable data after download*/
/******* The following define modes of the Dual Tracking Loop Operation***/
#define GPS_Warmup  0
#define GPS_Normal  1
#define Holdover    2
#define FAILED_MODE 3
#define E1_Normal   4
#define E1_Warmup   5
#define GPS_Only    6
#define Temp_Transient 7
/********* ASM Related Parameters ********/
#define GPSSTABLE 1000 /*number of seconds of valid gps timing data to complete warmup */
#define SLOOPOUT  1     /*use single loop output to steer NCO */
#define DLOOPAOUT 2     /*use dual loop A output to steer NCO */
#define DLOOPBOUT 3     /*use dual loop B output to steer NCO */
#define HMSOUT    4     /*use single holdover manage output to steer NCO */
#define HMDAOUT   5     /*use holdover manager dual loop A output */
#define HMDBOUT   6     /*use holdover manager dual loop B output */
#define NCO_REVERT_TIME 120 /*number of seconds before permitting switch
									  back to SPAN Dual Tracking Loop */
#define MODE_DUR_MAX 15360  /*maximum loop duration in minutes */
/**************************************/
/************************************************************************/
#define DUAL_SP_PROP_PRIM_TAU   200.0 /*Default Time Constants used to be 300*/
#define DUAL_SP_INT_PRIM_TAU    880.0 /* used to be 1800*/
#define DUAL_SP_DINT_PRIM_TAU   4800.0
#define DUAL_SP_CAL_TAU         1800.0 /*used to be 3600  speed up for quicker recal of small steps*/

#define DUAL_RO_PROP_PRIM_TAU   200.0 /*Default Time Constants used to be 600*/
#define DUAL_RO_INT_PRIM_TAU    880.0 /* used to be 3600*/
/*#define DUAL_RO_PROP_PRIM_TAU   120.0 *//*Default Time Constants*/
/*#define DUAL_RO_INT_PRIM_TAU    720.0*/
#define DUAL_RO_DINT_PRIM_TAU   3600.0
#define DUAL_RO_CAL_TAU         7200.0

/****** IMPORTANT BOTH PRIMDELAY AND GPSDELAY SHOULD BE THE SAME TO
		  ENSURE 3 CORNER HAT ALGORITM RECEIVE CORRELATED DELAYED DATA *****/
#define PRIMDELAY          30  /*Delay in seconds into primary loop*/
#define GPSDELAY           30  /*Delay in seconds into calibration loop*/
#define TAU_ENSEMBLE       5   /*number of minute intervals for ensemble mvar integration */
/****** Conversion factor from GPS mvar at TAU_Ensemble out to Lloop Time Constant
		  based on expected White PM roll-off TAU_ENSEMBLE should be 10-30 minutes**/
#define GPS_PM_CONV  (TAU_ENSEMBLE*TAU_ENSEMBLE*TAU_ENSEMBLE*216000.0/3.0)
#define MVARFILT1 (double) 0.025  /* Steady state smoothing filter gain*/
#define MVARFILT2 ((double) 1.0 - MVARFILT1)
#define MVARMIN (double)(1e-24)  /*lower bound on mvar estimates*/
#define MVARMAX (double)(5.0e-21)  /*upper bound on mvar estimates*/
#define EXTERNAL_MODE 1         /*mode assignment for ensembling include external inputs*/
#define SPAN_MODE 2             /*mode assignment for ensembling include span inputs*/
/*#if (LOTYPE==OCXO)*/   //GPZ March 2008 adjust for G.812 Type I compliance
#define SINGLE_PROP_TAU_XO 100.0   /*1000 TC prev*/
#define SINGLE_INT_TAU_XO  640.0   /*5500 prev*/
#define SINGLE_DINT_TAU_XO 0.0  /*Not Used in OCXO Application*/
#define SINGLE_ACCEL_XO   5.0  /* 12 16 acceleration factor during intial recal June 2010 GPZ was 3*/
#define SINGLE_RECAL_XO   5.0   /*12 16 acceleration binary factor during recal June 2010 GPZ was 3*/
#define GEAR_SPACE_XO     1.39   /* 1.91 1.77 gain in time constants between gears June 2010 GPZ was 1.25*/
/*#elif	(LOTYPE==DOCXO)*/  
#define SINGLE_PROP_TAU_DO 100.0   /*250 prev*/
#define SINGLE_INT_TAU_DO  640.0   /*1125 prev*/
#define SINGLE_DINT_TAU_DO 0.0  /*Not Used in OCXO Application*/
#define SINGLE_ACCEL_DO   5.0 /*(8) acceleration factor during intial recal GPZ June 2010*/
#define SINGLE_RECAL_DO   5.0   /*(8)acceleration binary factor during recal GPZ June 2010*/
#define GEAR_SPACE_DO     1.39   /*(1.77)gain in time constants between gears GPZ June 2010*/

/*#else*/
/****** IMPORTANT MAKE SURE FINAL Lloop.Cal_Mode is 7 for Rb
based system as Pcomp_Update uses this as to control final attack rate***/
#define SINGLE_PROP_TAU_RB (5.0*100.0)   /*1000.0 prev*/
#define SINGLE_INT_TAU_RB  (5.0*640.0)   /*6400.0  prev*/
#define SINGLE_DINT_TAU_RB 0.0  /*Not Used in OCXO Application*/
#define SINGLE_ACCEL_RB   8.0  /*(15) acceleration factor during intial recal GPZ June 2010 was  3*/
#define SINGLE_RECAL_RB   8.0   /*15 acceleration binary factor during recal GPZ June 2010 was 3*/
#define GEAR_SPACE_RB     1.535  /*1.75 gain in time constants between gears GPZ June 2010 was 1.25 */ 
/* Loran Time Constants*/  
//#define SINGLE_PROP_TAU_RB 1800.0   /*21600 prev*/
//#define SINGLE_INT_TAU_RB  8100.0   /*129600 prev*/
//#define SINGLE_DINT_TAU_RB 0.0  /*Not Used in OCXO Application*/
//#define SINGLE_ACCEL_RB   15.0  /*(20) acceleration factor during intial recal*/
//#define SINGLE_RECAL_RB   15.0   /*32 acceleration binary factor during recal*/
//#define GEAR_SPACE_RB     1.72   /*2.0 gain in time constants between gears */
/*#endif*/

/*********************************************
ATC Algorithm Definitions
**********************************************/
#define ATCUPDATETAU 1200          /*ATC slow (GPS) update and input filter TAU 1200*/
#define ATCSMOOTHTAU 10000         /*ATC slow (GPS coefficient output smoothing TAU 10000*/
#define ATCMINSLEW   4.0          /*minimum temp slew to include data*/
#define ATCMAXSLEW   12.0         /*maximum temp slew to include data*/
#define ATCSLEWUP    20
#define ATCSLEWDOWN  75
#define ATCEXTAMODE   0            /*operating mode using external A input*/
#define ATCEXTBMODE   1            /*operating mode using external B input*/
#define ATCGPSMODE    2            /*operating mode using GPS input*/
/***************************************
Kalman Operating Mode Definitions
***************************************/
#define POSITION	0
#define DUAL		1
#define TIMING		2 


/* Shared Memory Range Limits */
#define CLOCK_BIAS_MAX 20e-3 /* 20 msec maximum bias offset */
#define TIME_ACC_MAX	9999	/* 9999 ns */
#define MDEV_THRES 5e-9    /*maximum mdev crossover point instability */
#define MAX_HOLD_RANGE 2e-6 /*maximum range of holdover control word */
#define MAX_HOLD_SLEW  2e-9 /* maximum s.s. rate of change per hour of holdover word*/
#define MAX_DRIFT_RATE 1e-10 /*maximum hourly aging or drift rate */
#if (ANTENNA_MODE==ROOFTOP)
#define TIME_ACC_THRES 3000 /* glh 750*/ /* maximum acceptable time accuracy */
#define MAX_PROP_TC  2500L    /* Time Constant Constraints*/
#define MAX_INT_TC	50000L
#else
#define TIME_ACC_THRES 1000 /* maximum acceptable time accuracy was 3000*/
#define MAX_PROP_TC   28800L    /* Time Constant Constraints*/
#define MAX_INT_TC	172800L
#endif
#define MAX_DINT_TC  50000L
#define MAX_CAL_TC   50000L
#define MIN_TC	15L
/*************************************************************************
  The following definitions control the initial stability
  masks for GPS, SPAN and the Local Oscillator Assume first MDEV is
  at 250ms TAUZERO
**************************************************************************/
/*******
GPS MDEV Model based on XR5 residual SA performance
*******/
#define GPS_MDEV_0  3.0e-9
#define GPS_MDEV_1  2.2e-9
#define GPS_MDEV_2  1.6e-9
#define GPS_MDEV_3  1.1e-9
#define GPS_MDEV_4  8.0e-10
#define GPS_MDEV_5  5.0e-10
#define GPS_MDEV_6  4.0e-10
#define GPS_MDEV_7  3.2e-10
#define GPS_MDEV_8  2.0e-10
#define GPS_MDEV_9  1.4e-10
//#define GPS_MDEV_10 1.0e-10 assume SA is off    
#define GPS_MDEV_10 3.5e-11
#define GPS_MDEV_11 3.5e-11
#define GPS_MDEV_12 1.2e-11
#define GPS_MDEV_13 4.4e-12
#define GPS_MDEV_14 1.5e-12
/******
SPAN MDEV Model Composite White PM White FM. White PM set at 50 ns rms and
White FM at 2e-9 at 1 second */
#define SP_MDEV_0  1.5e-8
#define SP_MDEV_1  1.2e-9
#define SP_MDEV_2  6.0e-9
#define SP_MDEV_3  3.2e-9
#define SP_MDEV_4  1.0e-9
#define SP_MDEV_5  7.0e-10
#define SP_MDEV_6  5.0e-10
#define SP_MDEV_7  3.5e-10
#define SP_MDEV_8  2.5e-10
#define SP_MDEV_9  1.7e-10
/*#define SP_MDEV_10 1.2e-10*/
#define SP_MDEV_10 3.5e-11  /*test with good span lines*/
#define SP_MDEV_11 8.8e-11
#define SP_MDEV_12 6.2e-11
#define SP_MDEV_13 4.4e-11
#define SP_MDEV_14 3.1e-11
/*****
Local Oscillator 0CXO Model
7e-12 Floor
*******/
#define LO_MDEV_0  2.0e-11
#define LO_MDEV_1  1.0e-11
#define LO_MDEV_2  7.0e-12
#define LO_MDEV_3  7.0e-12
#define LO_MDEV_4  8.0e-12
#define LO_MDEV_5  9.0e-12
#define LO_MDEV_6  1.0e-11
#define LO_MDEV_7  1.0e-11
#define LO_MDEV_8  1.0e-11
#define LO_MDEV_9  1.0e-11
#define LO_MDEV_10 1.0e-11
#define LO_MDEV_11 1.0e-11
#define LO_MDEV_12 1.0e-11
#define LO_MDEV_13 1.4e-11
#define LO_MDEV_14 2.0e-11

/*****************************************************************
The following setting control the initial estimate of 24 hour
tdev stability for holdover operation
*****************************************************************/
#define SP_TDEV_24   800.0e-9
#define RO_TDEV_24   1000.0e-9
#define LO_TDEV_24  15000.0e-9
#define SECINDAY 86400.0      

#define EFC_SLOPE 7.27e-13 /* CTS */
//#define EFC_SLOPE 1.53456e-13 /* MTI270 */
//#define EFC_SLOPE -8.21e-13 /* MTI260 */ 
//#define EFC_SLOPE -7e-13 /* MTI260 off by 100%*/ 
//#define EFC_SLOPE -2e-13 /* KSS OCVCXO-120E */
//#define SCALE_SPAN_TO_DELTA_FREQ (double)(6.08554763448e-10) 
#define SCALE_SPAN_TO_DELTA_FREQ (double)(-1.27165755208333e-7) 


#endif // _BT3_DEF_H_04_09_2003
