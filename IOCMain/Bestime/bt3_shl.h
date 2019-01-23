/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:07PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_shl.h_v  $
$Modtime:   06 Feb 1999 12:59:00  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                BT3_SHL.H

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: This header file was created to support external messaging
                  to the BT3 Shell as document in the 
                  BT3_Messaging_Interface_ver_1.x

MODIFICATION HISTORY:

$Log: bt3_shl.h  $
Revision 1.1 2007/12/06 11:41:07PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.6 2005/02/03 11:23:23PST gzampet 

Revision 1.5  2004/07/29 18:12:36Z  gzampet
Revision 1.4  2004/07/21 23:03:11Z  zmiao
Revision 1.3  2004/07/09 18:28:56Z  gzampet
Revision 1.2  2004/07/01 17:44:20Z  gzampet
added gear to alarm report
Revision 1.1  2003/04/15 19:46:25Z  gzampet
Initial revision
Revision 1.1  2001/12/11 19:28:56Z  kho
Initial revision
 * 
 *    Rev 1.0   06 Feb 1999 15:41:52   S.Scott
 * TS3000 initial release.

***************************************************************************/    

#ifndef _BT3_SHL_H_07_21_2004
#define _BT3_SHL_H_07_21_2004

/******************** INCLUDE FILES ************************/

/******************** LOCAL DEFINES ************************/

/************ PROTOTYPES EXTERNAL FUNCTIONS ****************/

/************ PROTOTYPES INTERNAL FUNCTIONS ****************/

/******************** EXTERNAL DATA ************************/

/******************** INTERNAL DATA ************************/

/************************ TABLE ****************************/
     
/************************ CODE *****************************/


/*****************************************************************************
*     Version 2.1 Baseline BT3 Shell Interface
*     This header file was created to support external messaging
*     to the BT3 Shell as document in the BT3_Messaging_Interface_ver_1.x
*****************************************************************************/

/******************************************************************************
  The Control Loop Status Report is built from five individual sections plus
  a common section. The Input Register data included in the reported must be
  obtained from the status register message with latch management to prevent
  losing any events or alarms. There should be four user report copies maintained
  by the report generation function.
*****************************************************************************/
/*****************************************************************************
  Message: MSG_Control_Loop_Section

  Description: Structure for each of the five mailboxes containing a section of
  the control loop status report.
  Date: Created 4-13-98
*****************************************************************************/
struct MSG_Control_Loop_Section
{
	float IN_Prop_State;   /*proportional control state*/
	float IN_Int_State;    /*integral control state*/
	float IN_Dint_State;   /*double integral control state (not used in std. apps)*/
	float IN_GPS_Cal_State; /*GPS calibration state*/
	unsigned short IN_Recalibration_Mode; /*Re-Calibration Mode stage 1-5*/
	unsigned short IN_Mode_Shift;         /*Time in minutes until next shift*/
	unsigned short IN_Weight;             /*Weight in percentage*/
	unsigned short Ready;                 /*Mail Box Ready Flag*/
} ;
/*****************************************************************************
Message: MSG_Control_Loop_Common

Description: Structure for the common control loop status mailbox.
The BT3 updates the sections round robin over a 5 second window and updates
the common data every second
Date: Created 4-13-98
*****************************************************************************/
struct MSG_Control_Loop_Common
{
	 float Freq_State_Cor;
	 float Phase_State_Cor;
	 float Temp_State_Cor;
	 unsigned short Input_Mode;
	 unsigned short Ready;                 /*Mail Box Ready Flag*/
};
/*****************************************************************************
Message: MSG_Alarm_Status_Report

Description: Structure for the Current Alarm and Status Data mailbox. Updated
every second.
Date: Created 4-13-98
*****************************************************************************/
struct MSG_Alarm_Status_Report
{
	unsigned short GPS_Reg;
	unsigned short SPA_Reg;
	unsigned short SPB_Reg;
	unsigned short ROA_Reg;
	unsigned short ROB_Reg;
	unsigned short PL_Reg;
	unsigned short TP_Reg;
	unsigned short BT3_Mode;
	unsigned short BT3_Mode_Dur;
	unsigned short Gear;
	unsigned short Ready;                 /*Mail Box Ready Flag*/
};
/*****************************************************************************
Message: MSG_Daily_Holdover_Report

Description: Structure for each of the five channels of daily holdover report
data
Date: Created 4-13-98
*****************************************************************************/
struct Time_Period
{
  long Freq_Base;  /*6 hour group hourly frequency base ps/s */
  short Freq_Off_4;/*6 hour group hourly frequency offset 0 ps/s*/
  short Freq_Off_3;
  short Freq_Off_2;
  short Freq_Off_1;
  short Freq_Off_0;
};
struct MSG_Daily_Holdover_Report
{
  struct Time_Period avg_freq[4];
  struct Time_Period pred_freq[4];
  long  Freq_Drift_Estimate; /*Current estimate of drift (ps/s) per day*/
  long  Freq_Pred_Err_24;     /*RMS estimate of 24 hour holdover time error (ns)*/
  unsigned short Channel;
  unsigned short Ready;
};
/*****************************************************************************
Message: MSG_Monthly_Holdover_Report

Description: Structure for each of the five channels of Monthly holdover report
data
Date: Created 4-13-98
*****************************************************************************/
struct MSG_Monthly_Holdover_Report
{
  struct Time_Period Daily_Freq[5]; /*Daily Average Frequency Group (ps/s)*/
  struct Time_Period Daily_Pred[5]; /*Daily Prediction Error Group  (NS)*/
  unsigned short Channel;
  unsigned short Ready;
};

/*****************************************************************************
Message: MSG_PSM_Report

Description: Structure for each of the five channels of PSM report
data
Date: Created 4-13-98
*****************************************************************************/
struct MSG_PSM_Report
{
  float Phase;
  float MTIE1;
  float MTIE2;
  float MTIE4;
  float MTIE8;
  float MTIE16;
  float MTIE32;
  float MTIE64;
  float MTIE128;
  float MTIE256;
  float MTIE512;
  float MTIE1K;
  float MTIE2K;
  float MTIE4K;
  float MTIE8K;
  float MTIE16K;
  float AFREQ1;
  float AFREQ2;
  float AFREQ4;
  float AFREQ8;
  float AFREQ16;
  float AFREQ32;
  float AFREQ64;
  float AFREQ128;
  float AFREQ256;
  float AFREQ512;
  float AFREQ1K;
  float AFREQ2K;
  float AFREQ4K;
  float AFREQ8K;
  float AFREQ16K;
  float MVAR1;
  float MVAR2;
  float MVAR4;
  float MVAR8;
  float MVAR16;
  float MVAR32;
  float MVAR64;
  float MVAR128;
  float MVAR256;
  float MVAR512;
  float MVAR1K;
  float MVAR2K;
  float MVAR4K;
  float MVAR8K;
  float MVAR16K;
  unsigned short Channel;
  unsigned short Ready;
};
/*****************************************************************************
Message: MSG_Timing_Status_Report

Description: Structure for the timing status report mailbox updated every
second
Date: Created 4-13-98
*****************************************************************************/
struct MSG_Timing_Status_Report
{
  unsigned short One_PPS_High;  /*Aggregate event level flag indicated current
											 PPS degraded*/
  unsigned short PPS_Range_Alarm; /*Alarm indicating PPS outpur failed*/
  float Phase_State_Correction;   /*current phase slew correction (frac freq)*/
  float Time_Err_Est;            /*Current bias error between GPS PPS in and BT3
											 PPS out*/
  float TDEV_Crossover;          /*GPS Tdev Stability at the crossover integration
											  time*/
  float TFOM;                    /*Estimated (2sigma) of accuracy of 1 PPS output*/
  float Time_Err_Meas;           /* Current measurment time error between 1 PPS in
												and 1 PPS out*/
  unsigned short Ready;
}; 
/*****************************************************************************
Message: MSG_BesTime_Engine_Report

Description: Structure for the summary BesTime Engine Report
Date: Created 7-7-2004
*****************************************************************************/
struct MSG_BesTime_Engine_Report
{
  unsigned short LO_Weight;  /*Current Weight of Local Oscillator*/
  unsigned short SPA_Weight;  /*Current Weight of Span-A */
  unsigned short SPB_Weight;  /*Current Weight of Span-B */
  unsigned short PRR_Weight;  /*Current Weight of PRR */
  long LO_Prediction;  /*Current Bridging Prediction in ns for local oscillator*/
  long SPA_Prediction;  /*Current Bridging Prediction in ns for of Span-A channel*/
  long SPB_Prediction;  /*Current Bridging Prediction in ns of Span-B channel*/
  long PRR_Prediction;  /*Current Bridging Prediction in ns of PRR channel*/
  long TFOM;                    /*Estimated (2sigma) of accuracy of 1 PPS output*/
  long GPS_mdev;		/*current modified allan standard deviation noise estimate for GPS*/
  long PA_mdev;    /*current estimate for ensemble channel 1 (either span or remote)*/
  long PB_mdev;   
  long LO_mdev;    
  unsigned short Backup_Reg;	/*Backup assigments 0000 0 PRR SPB SPA */
};

/*******************************************************************************
THIS NEXT SECTION INCLUDE THE INPUT COMMAND MAILBOXES WITH
THE ASSOCIATED OUTPUT REPORT MAILBOX
Each input command mailbox is checked once per second in the
Command_Mgr() routine.
******************************************************************************/

/*****************************************************************************
MSG: MSG_ECW_Command

Description: Structure for the ensembling control word mailbox.
Date: Created 5-7-98
*****************************************************************************/
struct MSG_ECW_Command
{
  unsigned short ECW;      /* ensemble control word (see smi.input_mode)*/
  unsigned short Ready;
};
struct MSG_ECW_Report
{
  unsigned short ECW;      /* ensemble control word (see smi.input_mode)*/
  unsigned short Override; /* Current override status ACCEPT, REJECT or CLEAR*/
  unsigned short Ready;
};
/*****************************************************************************
MSG: MSG_PMASK_Command

Description: Structure for the provisioning mask mailbox.
Date: Created 5-7-98
*****************************************************************************/
struct MSG_PMASK_Command
{
  unsigned short PMASK;      /* PMASK  bit 1= Disable GPS bit, 2= Disable SPA*/
                             /*bit3=Disable SPB, bit4=Disable ROA, bit5=Disable ROB*/
  unsigned short Ready;
};
struct MSG_PMASK_Report
{
  unsigned short PMASK;
  unsigned short Override; /* Current override status ACCEPT, REJECT or CLEAR*/
  unsigned short Ready;
};
/*****************************************************************************
MSG: MSG_QMASK_Command

Description: Structure for the qualification mask mailbox.
Date: Created 6-1-98
*****************************************************************************/
struct MSG_QMASK_Command
{
  unsigned short QMASK;      /* QMASK  bit 1= Disable GPS bit, 2= Disable SPA*/
									  /*bit3=Disable SPB, bit4=Disable ROA, bit5=Disable ROB*/
  unsigned short Ready;
};
struct MSG_QMASK_Report
{
  unsigned short QMASK;
  unsigned short Override; /* Current override status ACCEPT, REJECT or CLEAR*/
  unsigned short Ready;
};
/*****************************************************************************
MSG: MSG_RCW_Command

Description: Structure for the reset control word mailbox.
Date: Created 10-25-99
*****************************************************************************/
struct MSG_RCW_Command
{
  unsigned short RCW;      /* reset control word */
  unsigned short Ready;
};
struct MSG_RCW_Report
{
  unsigned short RCW;      /* reset control word*/
  unsigned short Override; /* Current override status ACCEPT, REJECT or CLEAR*/
  unsigned short Ready;
};



#endif // #ifndef _BT3_SHL_H_07_21_2004
