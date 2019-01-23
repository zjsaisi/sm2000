/***************************************************************************
$Author: Jining Yang (jyang) $
$Date: 2010/10/14 00:04:09PDT $
$Logfile:   Z:/pvcs/ts3000/ts3000/newmsgs.c_v  $
$Modtime:   30 Nov 2000 15:03:42  $
$Revision: 1.2 $


                            CLIPPER SOFTWARE
                                 
                                NEWMSGS.C

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

$Log: bt3_messages.c  $
Revision 1.2 2010/10/14 00:04:09PDT Jining Yang (jyang) 
Merged from 1.1.1.1.
Revision 1.1.1.1 2010/08/04 23:24:17PDT Jining Yang (jyang) 
George's change for reference switch.
Revision 1.1 2007/12/06 11:41:06PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.6 2005/02/03 11:23:21PST gzampet 

Revision 1.5  2004/07/29 18:12:25Z  gzampet
Revision 1.4  2004/07/09 18:28:52Z  gzampet
Revision 1.3  2004/07/01 22:21:37Z  zmiao
Revision 1.2  2004/07/01 17:42:46Z  gzampet
Add debug alarm report printing
Revision 1.1  2004/06/24 17:23:05Z  gzampet
Initial revision
 * 
 *    Rev 2.3   21 Dec 2000 16:53:42   JackHsia
 * Added "get_TRK_STAT(...)" for monitoring purpose.
 * 
 *    Rev 2.2   30 Mar 2000 14:36:00   JackHsia
 * Added "GetBt3CntLp_dat()" for control loop gear retrieval.
 * 
 *    Rev 2.1   27 Jan 2000 16:56:34   JackHsia
 * SRS-Rb merge
 * 
 *    Rev 2.0   15 Dec 1999 14:15:04   l.cheung
 * Beginning Ver 1.03.01, all files start from Rev 2.0.
 * 
 *    Rev 1.0   06 Feb 1999 15:44:24   S.Scott
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


/*
 * 9/14/98	tfs	modified put_CLK_STATE_INFO(), removed comments from
 *						around for() loop that swapped the USHORTS of the Rx'd
 *						Clock State Info message.
 *						Also, chgd GetBt3AlarmStatus() so don't copy AlmStatus
 *						structure, but a ptr to it into the passed variable.
 *						The above 2 chgs were found necessary after our trip to
 *						SJ on Friday, 9/11.
 * 9/24/98  tfs	Chgd GetBt3AlarmStatus() back so copy whole structure
 *						to the location specified by the input Ptr parameter.
 *						This mod was suggested by Lizzy, 9/21/98.
 * 10/16/98 tfs   Since Clock State Information was originally not to be
 *						a 'real' supported report, I had commented out the use
 *						of that command from this version.  As of 10/98, we now
 *						use this command to retrieve from the XR5(or in this
 *						case Clipper) the GPSSEC time information.  This data
 *						is used by BTMon to sync up it's PC clock to GPSSEC. So
 *						for this reason will uncomment the use of this ICD command.
 * 10/26/98 tfs   Added:  	PutBt3EnsembleControlWord(),
 *									GetBt3EnsembleControlWord(),
 *							     	PutBt3ProvisionControlMask()
 *							  and GetBt3ProvisionControlMask()
 * 11/18/98 tfs   Added Uplink Command 36, Factory Cal Data Download(to
 *						Clipper)
 *						For above added Freq_Comp[] array in which to hold Data
 *						prior to writing to PROM.
 *						Added Command 44 to 'newmsgd_()' routine, and routine
 *						'get_FLASH_CAL_DATA()' which is called to process
 *						Cmd 44.  Had to chg this routine to get the Data from
 *						E2, not Flash.
 * 11/20/98 tfs	Added PreSwapChars() routine to (actually) unswap any Char
 *						type arrays we Tx or Rx from BTMon.
 */
#include  "bt3_shell.h"
#include  "bt3_dat.h"
#include  "bt3_shl.h"
#include  "bt3_messages.h"   
#include "BT3Support.h" 
#include <stdio.h> 
extern struct HOLDOVER_DAILY_STRUCT  dHold[ NO_LOOPS ];
extern struct  HOLDOVER_MONTHLY_STRUCT  mHold[ NO_LOOPS ];
extern  struct  ALM_STAT_STRUCT AlmStCur;
extern  struct  ALM_STAT_STRUCT AlmSt[ NO_LATCHES ];
extern struct  CNTL_LOOP_LPDATA  new_CtlLp_lp[ NO_LOOPS ];      
extern struct PSM_STRUCT  psm[ NO_LOOPS ];
extern struct  TIMING_STATUS TmgStCur;
extern struct  TIMING_STATUS TmgSt[ NO_LATCHES ];
extern	ULONG		CLRL_CLS;
extern	ULONG		CLRL_ASR;
extern	ULONG		CLRL_TSR;

extern struct  CNTL_LOOP_STRUCT  CtlLp[ NO_LATCHES ];
extern struct Temperature_Meas_Data *ptmd;
extern struct Calibration_Data Cal_Data;
extern struct MSG_ECW_Command  	MSG_ECW_C;
extern struct MSG_ECW_Report  	MSG_ECW_R;
extern struct MSG_RCW_Command  MSG_RCW_C;
extern struct MSG_RCW_Report   MSG_RCW_R;
extern struct MSG_PMASK_Command 	MSG_PMASK_C;
extern struct Tracking_Stats TrkStats;   
extern struct MSG_BesTime_Engine_Report   MSG_BER;
extern struct Single_Input_FLL_Control_System Lloop; //GPZ June 2010 Need Trace_Skip

// typedef  struct ALM_STAT_STRUCT	Bt3AlarmReportType;       
Bt3AlarmReportType alarm_copy;  
struct MSG_BesTime_Engine_Report engine_copy;
#define  SSWAP(x)  ((x>>8)|(x<<8))


/*	This routine will be called by Clipper to configure the
 *		Reset Control Word
 */
void	PutBt3ResetControlWord( unsigned short ResetControlWord )
{
	MSG_RCW_C.RCW 	 	= ResetControlWord;
	MSG_RCW_C.Ready 	= 1;
}
/*  Use this routine to retrieve the current Reset Control Word.
 */
void	GetBt3ResetControlWord( unsigned short *ResetControlWord,
										   unsigned short *Override )
{
  *ResetControlWord =    MSG_RCW_R.RCW;
  *Override			     = MSG_RCW_R.Override;
}


/*	This routine  configures the
 *		Ensemble Control Word, Ensembling Control.
 */
void	PutBt3EnsembleControlWord( unsigned short EnsembleControlWord )
{
	MSG_ECW_C.ECW 	 	= EnsembleControlWord;
	MSG_ECW_C.Ready 	= 1;


}

/*  Use this routine to retrieve the current Ensembling Inputs.
 */
void	GetBt3EnsembleControlWord( unsigned short *EnsembleControlWord,
										   unsigned short *Override )
{
  *EnsembleControlWord = MSG_ECW_C.ECW;
  *Override			     = MSG_ECW_R.Override;
}

/*  Use this routine to change the current Provisioning Information.
 */
void	PutBt3ProvisionControlWord( unsigned short ProvisionControlWord )
{
	MSG_PMASK_C.PMASK	= ProvisionControlWord;
	MSG_PMASK_C.Ready = 1;


}

/*  Use this routine to retrieve the current Provisioning Information.
 */
void	GetBt3ProvisionControlWord( unsigned short *ProvisionControlWord )
{
	*ProvisionControlWord = MSG_PMASK_C.PMASK;
}

/*	Will use this routine to swap an array of 'characters' prior to Tx.
 *	 	Must do this because just prior to Txg, another swap is done of
 *		the whole msg, since are Txg to an INTEL machine.  After that swap
 *		the Character arrays will be OK.
 *
 *		*p 	= ptr to element [0] of the array to swap
 *		 cnt	= No of chars to be swapped
 */
void	PreSwapChars( void *p, USHORT cnt )
{
 	USHORT	*sp;
 	USHORT	 i;

	cnt = cnt >>1;

	sp = (USHORT *)p;

	for( i = 0; i < cnt; i++ )
		sp[i] = SSWAP( sp[i] );
}



USHORT get_CAL_DATA( USHORT *bufp )
{
	struct  Calibration_Data  *calp;

	calp = (struct Calibration_Data *)bufp;

	*calp = Cal_Data;

	PreSwapChars( (void *)&(*calp).Freq_Comp[0], MAXTEMPRECORDS );

	return( sizeof(struct  Calibration_Data)/2 ); 

}  /*   get_CAL_DATA()    */



USHORT get_TIMINGST( USHORT *bufp, USHORT port )
{
	struct  TIMING_STATUS  *tsrp;

	tsrp = (struct TIMING_STATUS *)bufp;

	*tsrp = TmgSt[ port ];

	SetLatch( &CLRL_TSR, port );

	return( sizeof(struct  TIMING_STATUS)/2 ); 

}  /*   get_TIMINGST()    */

USHORT get_D_HOLDOVER( USHORT *bufp, USHORT Channel )
{
	struct  HOLDOVER_DAILY_STRUCT  *hp;

	hp = (struct  HOLDOVER_DAILY_STRUCT *)bufp;

	*hp = dHold[ Channel-1 ];

	return( sizeof(struct  HOLDOVER_DAILY_STRUCT)/2 ); 

}  /*   get_D_HOLDOVER()    */

USHORT get_M_HOLDOVER( USHORT *bufp, USHORT Channel )
{
	struct  HOLDOVER_MONTHLY_STRUCT  *hp;

	hp = (struct  HOLDOVER_MONTHLY_STRUCT *)bufp;

	*hp = mHold[ Channel-1 ];

	return( sizeof(struct  HOLDOVER_MONTHLY_STRUCT)/2 ); 

}  /*   get_M_HOLDOVER()    */


/*  This routine is used to Retrieve the Satellite Tracking Stats
 *		Report Data.
 */
USHORT get_TRK_STATS( USHORT *bufp )
{
	struct 	Tracking_Stats *trkp;

	trkp = (struct  Tracking_Stats *)bufp;

	(*trkp) = TrkStats;

	return( sizeof(struct Tracking_Stats)/2 ); 

}  /*   get_TRK_STATUS()    */
void GetBt3AlarmStatus( Bt3AlarmReportType *AlarmReport )
{  
	*AlarmReport = AlmStCur;  
	if(Lloop.Trace_Skip>0)  //GPZ June 2010 add blanking during trace skip
	{
	  AlarmReport->reg.GPS=0;         
	} 
//	  AlarmReport->reg.GPS=0; //GPZ June 2010 TEST TEST TEST force clean GPS all the time
	

}   /*   get_alm_status()    */

USHORT get_alm_status( void *bufp, USHORT port )
{
	struct ALM_STAT_STRUCT *almst;

	almst = (struct ALM_STAT_STRUCT *)bufp;

	*almst = AlmSt[ port ];

	SetLatch( &CLRL_ASR, port );

	return( sizeof(struct ALM_STAT_STRUCT)/2 ); 

}   /*   get_alm_status()    */

void GetBt3CntlLp_dat( struct CNTL_LOOP_STRUCT *cntl)
{

	*cntl = CtlLp[0];
   /****  Do not set latch so we don't hose up the real user*/
	/*SetLatch( &CLRL_CLS, port );*/

}   /*   get_BT3Cntrlp_dat    */
USHORT get_CntlLp_dat( USHORT *bufp, USHORT port )
{
	struct CNTL_LOOP_STRUCT *cntl;

	cntl = (struct CNTL_LOOP_STRUCT *)bufp;

	*cntl = CtlLp[ port ];

	SetLatch( &CLRL_CLS, port );

	return( sizeof(struct CNTL_LOOP_STRUCT)/2 );

}  /*   get_CntlLp_dat()    */


USHORT get_PSM( USHORT *bufp, USHORT ch )
{
	struct PSM_STRUCT  *lpsm;

	lpsm = (struct PSM_STRUCT *)bufp;

	*lpsm = psm[ ch ];

	return( sizeof(struct PSM_STRUCT)/2 ); 

}  /*   get_PSM()    */ 
void Get_BesTime_Engine_Report( struct MSG_BesTime_Engine_Report *Engine_Report)
{
	*Engine_Report= MSG_BER;

}   /*   get_BesTime Engine Report    */
/*****************************************************************
Alarm Report Debug Print
******************************************************************/  

void Alarm_Report_Loop_Print()
{      
		char cbuf1[120];  
		GetBt3AlarmStatus( &alarm_copy);
	    sprintf((char *)cbuf1,"ALARM RPT:%d,%d,%d,%d,%d,%d,%d,%d\r\n",   
	    (int)(alarm_copy.reg.GPS),
	    (int)(alarm_copy.reg.SPA),
	    (int)(alarm_copy.reg.SPB),
	    (int)(alarm_copy.reg.ROA),
	    (int)(alarm_copy.reg.ROB),
	    (int)(alarm_copy.bt3_mode),
	    (int)(alarm_copy.bt3_mode_dur),
	    (int)(alarm_copy.Gear)
	    );
		DebugPrint(cbuf1, 5);
}
/*****************************************************************
Engine Report Report Debug Print
******************************************************************/  

void Engine_Report_Loop_Print()
{      
		char cbuf1[120];  
		Get_BesTime_Engine_Report( &engine_copy);
	    sprintf((char *)cbuf1,"ENGINE STATS:%d,%d,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%d\r\n",   
	    (int)(engine_copy.LO_Weight),
	    (int)(engine_copy.SPA_Weight),
	    (int)(engine_copy.SPB_Weight),
	    (int)(engine_copy.PRR_Weight),
	    (engine_copy.LO_Prediction),
	    (engine_copy.SPA_Prediction),
	    (engine_copy.SPB_Prediction),
	    (engine_copy.PRR_Prediction), 
   	    (engine_copy.TFOM),
   	    (engine_copy.GPS_mdev),  
   	    (engine_copy.PA_mdev),  
   	    (engine_copy.PB_mdev),  
   	    (engine_copy.LO_mdev),
   	    (engine_copy.Backup_Reg)  
	    );
		DebugPrint(cbuf1, 6);
}
