/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:06PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/shell.c_v  $
$Modtime:   30 Nov 2000 15:04:48  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                SHELL.C

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

$Log: bt3_shell.c  $
Revision 1.1 2007/12/06 11:41:06PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.3 2004/07/01 11:44:46PDT gzampet 

Revision 1.2  2004/07/01 17:43:38Z  gzampet
Revision 1.1  2004/06/24 17:23:03Z  gzampet
Initial revision
 * 
 *    Rev 2.1   21 Dec 2000 16:52:22   JackHsia
 * added struct member ".Time_Err_Meas" in function Upd8_TmgSt().
 * 
 *    Rev 2.0   15 Dec 1999 14:18:58   l.cheung
 * Beginning Ver 1.03.01, all files start from Rev 2.0.
 * 
 *    Rev 1.0   06 Feb 1999 15:45:32   S.Scott
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


/* **********************************************************************
*
*  SHELL.C        04/14/98    tfs
*
*************************************************************************
*/   
#include	"bt3_shl.h"
#include	"bt3_shell.h"

struct HOLDOVER_DAILY_STRUCT  dHold[ NO_LOOPS ];  
struct  HOLDOVER_MONTHLY_STRUCT  mHold[ NO_LOOPS ];
struct  ALM_STAT_STRUCT AlmStCur;
struct  ALM_STAT_STRUCT AlmSt[ NO_LATCHES ]; 
struct  CNTL_LOOP_LPDATA  new_CtlLp_lp[ NO_LOOPS ];
struct  CNTL_LOOP_STRUCT  CtlLp[ NO_LATCHES ];
struct PSM_STRUCT  psm[ NO_LOOPS ];
struct  TIMING_STATUS TmgStCur;
struct  TIMING_STATUS TmgSt[ NO_LATCHES ];
ULONG		CLRL_CLS;
ULONG		CLRL_ASR;
ULONG		CLRL_TSR;
struct CNTL_LOOP_LPDATA            *pMSG_CLS[NO_LOOPS];
struct CNTL_LOOP_COMMON 				*pMSG_CLC;
struct ALM_STAT_STRUCT 				*pMSG_ASR;
struct HOLDOVER_DAILY_STRUCT   		*pMSG_DHR;
struct HOLDOVER_MONTHLY_STRUCT 		*pMSG_MHR;
struct PSM_STRUCT 				   	*pMSG_PSM;
struct TIMING_STATUS    				*pMSG_TSR;
extern struct MSG_Control_Loop_Section 	  MSG_CLS[NO_LOOPS];
extern struct MSG_Control_Loop_Common 		  MSG_CLC;
extern struct MSG_Alarm_Status_Report 		  MSG_ASR;
extern struct MSG_Daily_Holdover_Report 	  MSG_DHR;
extern struct MSG_Monthly_Holdover_Report   MSG_MHR;
extern struct MSG_PSM_Report 				  	  MSG_PSM;
extern struct MSG_Timing_Status_Report 	  MSG_TSR;

/*  When Latched data is retrieved by BTMon, the current Latched data
 *		can be cleared.  We do not want the Report retriever(data consumer)
 *		to clear the Latched Data(LD).  We only want the LD to be
 *		written to by one process/task.  Thus we'll use these LatchFlags
 *		to signal when the LD clearing should be done.  Thus,
 *		the task which retrieves the Report sets the Flag according to
 *		which User it is(probably Com1, Com2, etc.).  This signifies that
 *		it has read the current LD, and that a new
 *		latch period can begin.  The Shell task which writes the LD,
 *		reads these Flag bits, then clears the LD accordingly.
 */
void	ClrLatch( ULONG *l, USHORT user )
{
 	*l = *l&~(1<<user);
}

void	SetLatch( ULONG *l, USHORT user )
{
 	*l = *l|(1<<user);
}

/*  returns the 1/0 value of the Flag bit
 */
USHORT	LatchSet( ULONG *l, USHORT user )
{
 	return( *l&(1<<user) );
}


/*  This routine should be called after the current Alarm Information
 *		has been read into the AlmStCur structure.  From there this routine
 *		will distribute the Alarm info. to the Alarm Status and Control
 *		Loop Reports which both contain Alarm Status information.
 */
void  Upd8_AlmLatches( void )
{
	register USHORT latch;

	for( latch = 0; latch < NO_LATCHES; latch++ )
	{
		if( LatchSet( &CLRL_ASR, latch ) )
		{
			AlmSt[ latch ].reg.GPS =	AlmStCur.reg.GPS;
			AlmSt[ latch ].reg.SPA =	AlmStCur.reg.SPA;
			AlmSt[ latch ].reg.SPB =	AlmStCur.reg.SPB;
			AlmSt[ latch ].reg.ROA =	AlmStCur.reg.ROA;
			AlmSt[ latch ].reg.ROB =	AlmStCur.reg.ROB;
			AlmSt[ latch ].reg.TP  =	AlmStCur.reg.TP ;
			AlmSt[ latch ].reg.PL  =	AlmStCur.reg.PL ;

			ClrLatch( &CLRL_ASR, latch );
		}
		else
		{
			AlmSt[ latch ].reg.GPS |=	AlmStCur.reg.GPS;
			AlmSt[ latch ].reg.SPA |=	AlmStCur.reg.SPA;
			AlmSt[ latch ].reg.SPB |=	AlmStCur.reg.SPB;
			AlmSt[ latch ].reg.ROA |=	AlmStCur.reg.ROA;
			AlmSt[ latch ].reg.ROB |=	AlmStCur.reg.ROB;
			AlmSt[ latch ].reg.TP  |=	AlmStCur.reg.TP ;
			AlmSt[ latch ].reg.PL  |=	AlmStCur.reg.PL ;
		}

		AlmSt[ latch ].bt3_mode 	 = AlmStCur.bt3_mode;
		AlmSt[ latch ].bt3_mode_dur = AlmStCur.bt3_mode_dur;
		AlmSt[ latch ].Gear = AlmStCur.Gear;

//   	AlmSt[ latch ].fault_sec_threshold = minor_alm_secs.user_delay_secs;
//   	AlmSt[ latch ].GPS_fault_secs = minor_alm_secs.GPS;
//   	AlmSt[ latch ].SpA_fault_secs = minor_alm_secs.SPA;
//   	AlmSt[ latch ].SpB_fault_secs = minor_alm_secs.SPB;
//   	AlmSt[ latch ].ROA_fault_secs = minor_alm_secs.ROA;
//   	AlmSt[ latch ].ROB_fault_secs = minor_alm_secs.ROB;

//   	AlmSt[ latch ].selftest = AlmStCur.selftest;

		if( LatchSet( &CLRL_CLS, latch ) )
		{
			CtlLp[ latch ].reg.GPS =	AlmStCur.reg.GPS;
			CtlLp[ latch ].reg.SPA =	AlmStCur.reg.SPA;
			CtlLp[ latch ].reg.SPB =	AlmStCur.reg.SPB;
			CtlLp[ latch ].reg.ROA =	AlmStCur.reg.ROA;
			CtlLp[ latch ].reg.ROB =	AlmStCur.reg.ROB;

			ClrLatch( &CLRL_CLS, latch );
		}
		else
		{
			CtlLp[ latch ].reg.GPS |=	AlmStCur.reg.GPS;
			CtlLp[ latch ].reg.SPA |=	AlmStCur.reg.SPA;
			CtlLp[ latch ].reg.SPB |=	AlmStCur.reg.SPB;
			CtlLp[ latch ].reg.ROA |=	AlmStCur.reg.ROA;
			CtlLp[ latch ].reg.ROB |=	AlmStCur.reg.ROB;
		}
	}

}

/*  Since the Control Loop structure contains the Alarm Status data
 *		we'll keep the same number of copies of the whole structure for latch
 *		purposes.  On updating, the new data will be copied to the correct
 *		element of the NewLpData[] array, indexed by which Loop was just
 *		updated.  From there this routine will replicate the data to the 
 *		elements of the CtlLp[] array.
 */
void  Upd8_CtlLp_lp( USHORT lp )
{
	USHORT  latch;

	for( latch = 0; latch < NO_LATCHES; latch++ )
		CtlLp[ latch ].lpdata[ lp ] = new_CtlLp_lp[ lp ];
}

void  Upd8_CtlLp_cm( void )
{
	USHORT  latch;

	for( latch = 0; latch < NO_LATCHES; latch++ )
		CtlLp[ latch ].common = CtlLp[ 0 ].common;
}

void  Upd8_TmgSt( void )
{
	register USHORT latch;

	for( latch = 0; latch < NO_LATCHES; latch++ )
	{
		if( LatchSet( &CLRL_TSR, latch ) )
		{
			TmgSt[ latch ].One_PPS_high  = TmgStCur.One_PPS_high;
			TmgSt[ latch ].PPS_Range_Alm = TmgStCur.PPS_Range_Alm;

			ClrLatch( &CLRL_TSR, latch );
		}
		else
		{
			TmgSt[ latch ].One_PPS_high  |= TmgStCur.One_PPS_high;
			TmgSt[ latch ].PPS_Range_Alm |= TmgStCur.PPS_Range_Alm;
		}

		TmgSt[ latch ].Phase_State_Correction = TmgStCur.Phase_State_Correction;
		TmgSt[ latch ].Time_Err_Est 	= TmgStCur.Time_Err_Est;
		TmgSt[ latch ].TDEV_Crossover = TmgStCur.TDEV_Crossover;
		TmgSt[ latch ].TFOM 				= TmgStCur.TFOM;
		TmgSt[ latch ].Time_Err_Meas	= TmgStCur.Time_Err_Meas; /*GPZ add to complete report*/
	}
}


void	check_MSGs( void )
{
	USHORT lp;

	if( MSG_ASR.Ready )
	{
		AlmStCur		  		= *pMSG_ASR;
//   	AlmStCur.selftest = 0;
		MSG_ASR.Ready 		= 0;

		Upd8_AlmLatches();
	}

	for( lp = 0; lp < NO_LOOPS; lp++ )
	{
		if( MSG_CLS[ lp ].Ready )
		{
			new_CtlLp_lp[ lp ] = *pMSG_CLS[ lp ];
			Upd8_CtlLp_lp( lp );

			MSG_CLS[ lp ].Ready = 0;
		}

	}

	if( MSG_CLC.Ready )
	{
		CtlLp[ 0 ].common = *pMSG_CLC;
		Upd8_CtlLp_cm();

		MSG_CLC.Ready = 0;
	}

	if( MSG_PSM.Ready )
	{
		psm[ MSG_PSM.Channel ] = *pMSG_PSM;

		MSG_PSM.Ready = 0;
	}

	if( MSG_TSR.Ready )
	{
		TmgStCur = *pMSG_TSR;
		MSG_TSR.Ready = 0;

		Upd8_TmgSt();
	}

	if( MSG_DHR.Ready )
	{
		dHold[ MSG_DHR.Channel ] = *pMSG_DHR;
		MSG_DHR.Ready = 0;
	}

	if( MSG_MHR.Ready )
	{
		mHold[ MSG_MHR.Channel ] = *pMSG_MHR;
		MSG_MHR.Ready = 0;
	}
}


void  ShellMain( void )
{
	check_MSGs();
}

/*  This routine should be called once at initialization.
 */
void  ShellInit( void )
{
	USHORT lp;

	for( lp = 0; lp < NO_LOOPS; lp++ )
		pMSG_CLS[lp] = (struct CNTL_LOOP_LPDATA *)&MSG_CLS[lp];

	pMSG_CLC = (struct CNTL_LOOP_COMMON *)&MSG_CLC;
	pMSG_ASR = (struct ALM_STAT_STRUCT  *)&MSG_ASR;
	pMSG_DHR = (struct HOLDOVER_DAILY_STRUCT   *)&MSG_DHR;
	pMSG_MHR = (struct HOLDOVER_MONTHLY_STRUCT *)&MSG_MHR;
	pMSG_PSM = (struct PSM_STRUCT    *)&MSG_PSM;
	pMSG_TSR = (struct TIMING_STATUS *)&MSG_TSR;
}
