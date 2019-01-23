/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:07PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/shell.h_v  $
$Modtime:   06 Feb 1999 15:45:34  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                SHELL.H

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION:

MODIFICATION HISTORY:

$Log: bt3_shell.h  $
Revision 1.1 2007/12/06 11:41:07PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.4 2004/07/01 15:21:38PDT zmiao 

Revision 1.3  2004/07/01 18:44:51Z  gzampet
Revision 1.2  2004/07/01 17:43:52Z  gzampet
Revision 1.1  2004/06/24 17:23:01Z  gzampet
Initial revision
 * 
 *    Rev 2.0   15 Dec 1999 14:37:32   l.cheung
 * Beginning Ver 1.03.01, all files start from Rev 2.0.
 * 
 *    Rev 1.0   06 Feb 1999 15:45:34   S.Scott
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


/***********************************************************************
 * Cell Sync 1100 Project
 *
 * COPYRIGHT (C) Telecom Solutions, Livermore Skunk Works
 *	Filename:  shell.h
 *	    Date:  4/14/98
 *
 * $Header: bt3_shell.h 1.1 2007/12/06 11:41:07PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 ***********************************************************************
 */
#ifndef BT3_SHELL_H_0701
#define BT3_SHELL_H_0701

#ifdef   INTEL_SRC
#define  TYPEDEFS
#endif


#ifndef TYPEDEFS
typedef unsigned 	char 	UCHAR;
typedef        	char 	CHAR;
typedef unsigned 	short	USHORT;
typedef          	short	SHORT_TYPE;
typedef unsigned 	long	ULONG;
typedef        	long	LONG_TYPE;
typedef unsigned 	int 	UINT;
typedef          	int 	INT;

#define TYPEDEFS
#endif

#define	NO_USERS 4
#define	NO_LATCHES NO_USERS

#define	NO_LOOPS	5
#define	LOOP_GPS 0
#define	LOOP_A	1
#define	LOOP_B	2
#define  LOOP_ROA 3
#define  LOOP_ROB 4

/*  these define the downlink msg sub-command field values, 36 command
*/
#define	GPS_SUB	1
#define	SPA_SUB	2
#define	SPB_SUB	3
#define	ROA_SUB	4
#define	ROB_SUB	5

/*  Used these structures for Returning the Holdover Data
 *		to BTMon.  (the 36 Message)
 */
struct   TIME_PERIOD
{
   LONG_TYPE    base;
   SHORT_TYPE   off4;
   SHORT_TYPE   off3;
   SHORT_TYPE   off2;
   SHORT_TYPE   off1;
   SHORT_TYPE   off0;
}; 

struct  HOLDOVER_DAILY_STRUCT
{
   struct   TIME_PERIOD avg_freq[ 4 ];    /* average frequency		*/
   struct   TIME_PERIOD pred_freq[ 4 ];   /* predicted frequency	*/
   long     pred_drift;                   /* predicted drift 		*/
   long     pred_error;                   /* predicted error		*/
};


/********************************************************************/

struct  HOLDOVER_MONTHLY_STRUCT
{
   struct   TIME_PERIOD DailyFreq[ 5 ];  /* Daily Avg Frequency 	*/
   struct   TIME_PERIOD DailyPred[ 5 ];  /* Daily Prediction Err  */
};





/********************************************************************/

/*  Need multiple copies of these, so put them in their
 *		own structure.
 */
struct  REGISTERS
{
	USHORT  GPS;
	USHORT  SPA;
	USHORT  SPB;
	USHORT  ROA;
	USHORT  ROB;
	USHORT  PL;
	USHORT  TP;
};
	
/*  the Control Loop Report does not need the TP & PL Registers
*/
struct  REG_SHORT
{
	USHORT  GPS;
	USHORT  SPA;
	USHORT  SPB;
	USHORT  ROA;
	USHORT  ROB;
};
	
struct  ALM_STAT_STRUCT 
{
	struct  REGISTERS  reg;
	
   USHORT  bt3_mode;   		/*  0 = Warmup, 1 = Normal, 2 = Holdover   */
   USHORT  bt3_mode_dur;

   USHORT  Gear;   
   USHORT  Spare;   
	

};

typedef  struct ALM_STAT_STRUCT	Bt3AlarmReportType;

/*  Write current alarm message here. Then OR into AlmSt[] struct.
*/



/********************************************************************/

struct  CNTL_LOOP_LPDATA
{
	float  Prim_Prop_State; /* Proportional State of E1 						 */
	float  Prim_Int_State;  /* Integral State of E1 						    */
	float  Prim_Dint_State; /* Second Integrater state for 3rd order loop */
	float  GPS_Prop_State;  /* Proportional State of GPS						 */
	
	SHORT_TYPE Recalibrate;
	SHORT_TYPE Mode_Shift;
	SHORT_TYPE weight;
};

struct  CNTL_LOOP_COMMON 
{
	float  freqstatecor;
	float  phasestatecor;
	float  tempstatecor;
	USHORT Input_Mode;
};

struct  CNTL_LOOP_STRUCT 
{
	struct CNTL_LOOP_LPDATA  lpdata[ NO_LOOPS ];
	struct CNTL_LOOP_COMMON  common;

	struct REG_SHORT  reg;

};






/********************************************************************/

#define  NO_STATS     15

struct PSM_STRUCT
{
	float phase;
   float mtie[  NO_STATS ];
   float afreq[ NO_STATS ];
   float mvar[  NO_STATS ];
};




/********************************************************************/

struct  TIMING_STATUS
{
	USHORT	One_PPS_high;
	USHORT	PPS_Range_Alm;
	float		Phase_State_Correction;
	float		Time_Err_Est;	
	float		TDEV_Crossover;
	float		TFOM;				
	float		Time_Err_Meas;
};




/********************************************************************/
/********************************************************************/

#define	CMD_ECW_INPUTS	1
#define	CMD_ECW_PROVSN	2

struct	CMD_CNTL_STRUCT
{
	USHORT	cmd;
	USHORT	data1;
	USHORT	data2;
	USHORT	data3;
};

struct	MISC_CFG_STRUCT
{
	USHORT	glob_t1;			/*  this is glob_t1 information      	*/
	USHORT	ECWInputMode;	/*	 this is ECW Input Mode		    		*/
	USHORT	data2;
	USHORT	data3;
	USHORT	data4;
	USHORT	data5;
	USHORT	data6;
	USHORT	data7;
	USHORT	data8;
	USHORT	data9;
};

/********************************************************************/
/********************************************************************/












struct SW_RETRV_STRUCT
{
	short  s1;    
	short  s2;
	short  s3;
	short  fw_info;   /*   8  */
	ULONG  reldate0;
	ULONG  reldate1;
	ULONG  rel_full0;
	ULONG  rel_full1;
   USHORT cksum0;
   USHORT cksum1;
	ULONG  valid;
	double d4;   /*  32  */
	ULONG	 l1;
	ULONG	 l2;
	ULONG	 l3;
	ULONG	 l4;   /*  16  */
};              /*  56 bytes in size */


struct test_struct
{
	short  s1;    
	short  s2;
	short  s3;
	short  s4;   /*   8  */
	double d1;
	double d2;
	double d3;
	double d4;   /*  32  */
	ULONG	 l1;
	ULONG	 l2;
	ULONG	 l3;
	ULONG	 l4;   /*  16  */
};              /*  56 bytes in size */


#ifndef  INTEL_SRC
extern  struct test_struct  t;
#endif

/*  use this struct with the 34 command.  req gets set to a value
//    corresponding to the data to be retrieved next.
*/
struct  req_struct
{
   short req;
};

struct   FACTORY_CAL_DATA
{
   short    YroAvg;
   USHORT   TempEst;
   USHORT   Heater;
   USHORT   na1;
};

union	FW_INFO	
{
   struct
   {
      unsigned unused :  7;   /*   msb    							*/
      unsigned fl_01  :  1;   /*   Online Flash, from FPGA    	*/
      unsigned antenna:  2;
      unsigned LO_type:  2;
      unsigned LO_freq:  2;
      unsigned target :  2;
   } b;
   
   USHORT whole;
};

#endif // BT3_SHELL_H_0701
