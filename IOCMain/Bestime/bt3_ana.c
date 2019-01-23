/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:05PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_ana.c_v  $
$Modtime:   06 Feb 1999 12:42:38  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                BT3_ANA.C

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: Module for calculation of timing stability statistics.

MODIFICATION HISTORY:

$Log: bt3_ana.c  $
Revision 1.1 2007/12/06 11:41:05PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:10PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.5 2004/07/30 16:58:36PDT gzampet 

Revision 1.4  2004/07/01 22:21:35Z  zmiao
Revision 1.3  2004/06/24 16:39:28Z  gzampet
Fully Merged
Revision 1.2  2003/04/19 00:10:53Z  gzampet
Revision 1.2  2002/09/06 00:26:46Z  gzampet
Changed phase estimate smoothing filter to a 5 second time constant
from 20 seconds to supported higher speed update rate in
new BTmon
Revision 1.1  2001/12/06 18:02:21Z  kho
Initial revision
 * 
 *    Rev 1.0   06 Feb 1999 15:41:34   S.Scott
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
*     Multi tracking Core Control Algorithm  (MCCA)  renamed to BT3 see history
*     Module: Stab_ana.c
*     Description: Module for calculation of timing stability statistics.
*     Version 1.0  2/22/96 Baseline Version for Cellnet Application
*		Version 1.1  3/4/96  Includes changes required to integrate to Cell
*						 Net platform
*		Version 1.2  3/5/96   No Change to this module
*     Version 1.3  3/7/96   No Change to this module
*     Version 1.4  3/11/96
*                        1) Converted input buffers to long int to save
*                           RAM space.
*                        2) Added renormalization when inputs get too large
*                        3) Convert Estimators to float to conserve RAM space
*                        4) SAVINGS: 2735 bytes per channel OLD APPROACH
*                                    1766 bytes per channel NEW APPROACH
*                                    35% Reduction
*                        5) Added stat variables for seventh channel
*                        6) Added three corner estimate of E1B stability
*                        7) Added per channel stability flag to skip
*                           update
*                        8) Added octave based non-overlapping widow MTIE
*                           calculation algorithm
*		Version 1.5  5/22/96
*                        1) Added "duplex" 3 corner hat decomposition. With
*                           four inputs there are two "triangles" to use to
*                           estimate stability of any one input
*     Version 2.0 6/12/96 BASELINE DUAL INPUT PLATFORM
*                        1) Change filt1 calculation multiple by bscale not
*                           divide so that convergence speeds up for longer
*                           integration intervals and change bscale values
*     Version 2.5 7/8/97
*                        1) Added void tstat_report_fast() to provide quick
*                           reporting of channel timing stats.
*         MODULE RENAME BT3_ANA.C 9-15-97  as part of the BT3 Core Clock Engine
*         Project.
*     Version 1.0 Baseline BT3 clock engine
*                        1) No Change
*     Version 2.0
*                        1) Add messaging for 1 minute TCH task
*
*****************************************************************************/
/**********************************************************
	  Module for calculation of timing stability statistics.
	  The module is designed to operate in an embedded system with minimal
	  modification. To minimize memory utilization, the analysis engine is
	  designed around three circular buffers (short_time, med_time, and
	  long_time). In the simulation environment the module accepts input
	  in the form of X[n*MIN_INTERVAL] where X is the time error signal.

	  The module is sized to support 6 measurement channels. In the SYNCCELL
	  application the channel assignment is:
		1)GPS-TCXO
		2)E1-TCXO
		3)E1-GPS **** derived measurement
		4)E2-TCXO
		5)E2-GPS **** derived measurement
		6)Spare
	  In the simulation the channel assignment is re-coded as needed.

**************************************************************/
#include "bt3_def.h"
#if PCSIM
#include <stdio.h>  /*Not require for embedded system*/
#include "math.h"  /*Used only for sqrt function for stat reporting*/
#endif
/******** global data structures *********/
#include "bt3_dat.h"	/*main file for global data declarations*/
#include "bt3_msg.h" /*main file for message buffer templates*/
#include "bt3_shl.h"   /*main file for message shell templates*/
#if !PCSIM
double sqrt(double);
#endif
/**************** Include message structures ******************/
extern struct MSG_FLL_TCH MSG_FLL_TCH_A, MSG_FLL_TCH_B;
extern struct MSG_SR_TCH MSG_SR_TCH_A, MSG_SR_TCH_B;
struct  MSG_TCH_NCOUP MSG_TCH_NCOUP_A;
struct  MSG_PSM_Report MSG_PSM;
/***************End Message Structures*************************/
struct Channel_Data Cdata[MAXCHAN];
struct Three_Corner_Hat_Data TCH;
extern struct Shared_Memory_Interface smi;    
extern struct Dual_Input_FLL_Control_System DloopSA,DloopSB,DloopRA,DloopRB;
extern struct Single_Input_FLL_Control_System Lloop; 
#if 0
struct Per_Tau_Stats Short_Stats0[TAUCAT],Short_Stats1[TAUCAT],Short_Stats2[TAUCAT],
           Med_Stats0[TAUCAT],Med_Stats1[TAUCAT],Med_Stats2[TAUCAT],
			  Long_Stats0[TAUCAT],Long_Stats1[TAUCAT],Long_Stats2[TAUCAT];
struct Per_Tau_Stats *pshort[MAXCHAN],*pmed[MAXCHAN],*plong[MAXCHAN];
#endif
int Calc_TAU[TAUCAT];
double Smooth_TAU[TAUCAT];
double Tzero; /*Tau Zero sample time for analysis*/
int    Update_Count[MAXCHAN]; /*Counter to control calls to stat_update*/
unsigned int Mtie_Count[MAXCHAN];/*Counter to control mtie calcuations*/
unsigned char Mtie_Next;
int Medium_Update;
int Long_Update;
int Misc_Stab_Cnt;
unsigned short PSMR_Count;
#if  PRINTLOG
  int Print_Count=225;
  extern  unsigned long ptime;
#endif
/***** function templates *********/
//void stat_update(int,int);
void init_stat(double);
//void timing_stat_report(float[TOTAL_TAUCAT],unsigned char,unsigned char);
//void tstat_report_fast(float bundle_stats[TOTAL_STATS],unsigned char ,unsigned char);
void three_corner_decomp(void);
//void stat_normalize(int);
//void MTIEupdate(double Xin, int Which_Chan);
//void Mtie_Calc(unsigned char
//				,register struct Per_Tau_Stats *p,register struct Per_Tau_Stats *q);
void PSM_Report(void);  
#if 0
void stab_ana(int Chan_Max)
{
	 unsigned char i,j;
	 unsigned char Bindx;
	 long int Xinl;
	 for(i=0;i<Chan_Max;i++)
	 {
		if(Cdata[i].Data_Valid==TRUE)
		{
		 Update_Count[i]++;
		 if(Update_Count[i]>Long_Update) Update_Count[i]=1;
		 /*call long buffer updates*/
		 if(Update_Count[i]%Long_Update==0) /*1024 updates since last call*/
		 {
			/*Increment Sample Index for all TAU Categories (update of
			all categories is always synchronize for a given buffer)*/
			for(j=0;j<TAUCAT;j++)
			(plong[i]+j)->Cur_Samp= ((plong[i]+j)->Cur_Samp+1)%BUFMAX;
			Bindx=(plong[i])->Cur_Samp;
			Cdata[i].Long_Buf[Bindx]=(long int)(
				  ((pmed[i]+TAUCAT-1)->Cur_Sum +
					(pmed[i]+TAUCAT-1)->Lag_Sum  )
					*BUFSCALE/(2.0*NMAX));
			stat_update(LONG_LEN,i);
		 }
		 /*call medium buffer updates*/
		 if(Update_Count[i]%Medium_Update==0) /*32 updates since last call*/
		 {
			/*Increment Sample Index for all TAU Categories (update of
			all categories is always synchronize for a given buffer)*/
			for(j=0;j<TAUCAT;j++)
		  (pmed[i]+j)->Cur_Samp=
				 ((pmed[i]+j)->Cur_Samp+1)%BUFMAX;
			Bindx= (pmed[i])->Cur_Samp;
			Cdata[i].Med_Buf[Bindx]= (long int)(
				 ((pshort[i]+TAUCAT-1)->Cur_Sum +
				  (pshort[i]+TAUCAT-1)->Lag_Sum  )
				  *BUFSCALE/(2.0*NMAX));
			stat_update(MED,i);
		 }/* end if Medium Update*/
		 /*call short buffer updates*/
		 /*Increment Sample Index for all TAU Categories (update of
		 all categories is always synchronize for a given buffer)*/
		 for(j=0;j<TAUCAT;j++)
		 (pshort[i]+j)->Cur_Samp=
				 ((pshort[i]+j)->Cur_Samp+1)%BUFMAX;
		 Bindx=(pshort[i])->Cur_Samp;
		 Xinl=(long int)(Cdata[i].X1000ms*BUFSCALE);
		 Cdata[i].Short_Buf[Bindx]=Xinl;
		 if(Xinl>MAXSTATIN || Xinl < -MAXSTATIN) stat_normalize(i);
		 else
		 {
		 stat_update(SHORT_LEN,i);
		 MTIEupdate(Cdata[i].X1000ms,i);
		 }
	  }/*if data is valid*/
	 } /* end for all i channels */
    PSM_Report(); /*update outgoing shell mailboxes*/
	 /*miscellaneous house keeping tasks run every 60 updates*/
	 Misc_Stab_Cnt++;
	 if(Misc_Stab_Cnt>7200)
	 {
		 Misc_Stab_Cnt=0;
		 #if PRINTLOG
		 Print_Count++;
		 if(Print_Count>1)
		 {
			 Print_Count=0;
			 /* print out channel zero timing stats */
			 printf("  TAU     TDEV           MDEV            AFREQ           MTIE\n");
			 for(i=0;i<TAUCAT;i++)
			 {
				printf("%7.0f%15e%15e%15e%15e\n",(pshort[0]+i)->Tau,
				sqrt((pshort[0]+i)->TVAR_est),
				sqrt((pshort[0]+i)->MVAR_est),
					  (pshort[0]+i)->AFREQ_est,
					  (pshort[0]+i)->Mtie);
			 }
			 for(i=0;i<TAUCAT;i++)
			 {
				printf("%7.0f%15e%15e%15e%15e\n",(pmed[0]+i)->Tau,
				sqrt((pmed[0]+i)->TVAR_est),
				sqrt((pmed[0]+i)->MVAR_est),
					  (pmed[0]+i)->AFREQ_est,
					  (pmed[0]+i)->Mtie);
			 }
			 for(i=0;i<TAUCAT;i++)
			 {
				printf("%7.0f%15e%15e%15e%15e\n",(plong[0]+i)->Tau,
				sqrt((plong[0]+i)->TVAR_est),
				sqrt((plong[0]+i)->MVAR_est),
					  (plong[0]+i)->AFREQ_est,
					  (plong[0]+i)->Mtie);
			 }
		 }
		 #endif
	 }/* end if misc update time */
}
/**********************************************************************
This task is called each time a buffer receives a new data sample.
The Short Buffer is called every T0.
The Medium Buffer is called every 64*T0.
The Long Buffer is called every 64*64*T0.
The Stat update is performed for all TAUCAT categories
**********************************************************************/
struct Per_Tau_Stats *pstat_base,*pstat;

void stat_update(int Which_Buf,int Which_Chan)
{
	 long int *pbuf;
	 double filt1,filt2;
	 double bscale; /*scale average time for variance calculation */
	 int i;
	 int n; /*size of current lag*/
	 struct Per_Tau_Stats *pstat_base,*pstat;
	 if(Which_Buf==SHORT_LEN)
	 {
	 pbuf= &(Cdata[Which_Chan].Short_Buf[0]);
	 pstat_base= pshort[Which_Chan];
	 bscale=1.0;
	 }
	 if(Which_Buf==MED)
	 {
	 pbuf= &(Cdata[Which_Chan].Med_Buf[0]);
	 pstat_base= pmed[Which_Chan];
	 bscale=1.5;
	 }
	 if(Which_Buf==LONG_LEN)
	 {
	 pbuf= &(Cdata[Which_Chan].Long_Buf[0]);
	 pstat_base= plong[Which_Chan];
	 bscale=2.0;
	 }
	 for(i=0;i<TAUCAT;i++)
	 {
		 pstat=(pstat_base+i);
		 n=Calc_TAU[i];
		 if(pstat->Ntotal < BUFMAX) pstat->Ntotal++;
		 /* update indexes of lag start samples
	  (current lag is updated before call)*/
		 pstat->Lag_Samp=  (pstat->Cur_Samp + BUFMAX - n)%BUFMAX;
		 pstat->Dlag_Samp= (pstat->Lag_Samp + BUFMAX - n)%BUFMAX;
		 /* process current lag */
		 if(pstat->Cur_Sum_Full==0)
		 {
	  if(pstat->Ntotal>n) pstat->Cur_Sum_Full=1;
		 }
		 /* update current sum */
		 if(pstat->Cur_Sum_Full==1)
		 {
	  pstat->Cur_Sum +=(double)(*(pbuf+(pstat->Cur_Samp))  -
				 *(pbuf+(pstat->Lag_Samp)))/BUFSCALE;
		 }
		 else
		 {
	  pstat->Cur_Sum += (double)(*(pbuf+(pstat->Cur_Samp)))/BUFSCALE;
		 }
		 /* process one lag behind current */
		 if(pstat->Lag_Sum_Full==0)
		 {
	  if(pstat->Ntotal>2*n) pstat->Lag_Sum_Full=1;
		 }
		 /* update lag sum */
		 if(pstat->Lag_Sum_Full==1)
		 {
	  pstat->Lag_Sum += (double)(*(pbuf+(pstat->Lag_Samp))  -
				 *(pbuf+(pstat->Dlag_Samp)))/BUFSCALE;
		 }
		 else
		 {
	  pstat->Lag_Sum += (double)(*(pbuf+(pstat->Lag_Samp)))/BUFSCALE;
		 }
		 /* process two lags behind current */
		 if(pstat->Dlag_Sum_Full==0)
		 {
	  if(pstat->Ntotal>3*n) pstat->Dlag_Sum_Full=1;
		 }
		 /* update Dlag sum */
		 if(pstat->Dlag_Sum_Full==1)
		 {
	  pstat->Dlag_Sum += (double)(*(pbuf+(pstat->Dlag_Samp))	-
				 *(pbuf+((pstat->Dlag_Samp+BUFMAX-n)%BUFMAX)))/BUFSCALE;
		 }
		 else
		 {
	  pstat->Dlag_Sum += (double)(*(pbuf+(pstat->Dlag_Samp)))/BUFSCALE;
		 }
		 /* Update Second Difference Value */
	  if(pstat->Dlag_Sum_Full==1)
	  {
		  pstat->Sec_Diff=( pstat->Cur_Sum -
				 2.0 * pstat->Lag_Sum +
				 pstat->Dlag_Sum)/n;
	  }

		 /* Update TVAR Estimate */
	  filt1 =Smooth_TAU[i]*(bscale); /*Scaled to trade off more
				speed to achieve estimate versus accuracy of estimate
				for larger buffers */
	  filt2 = 1.0 -filt1;
	  pstat->TVAR_est= filt2 * pstat->TVAR_est +
			  pstat->Sec_Diff * pstat->Sec_Diff* filt1/
			  6.0;

		 /* Update MVAR Estimate*/
	  pstat->MVAR_est= pstat->TVAR_est*3.0/pstat->Tau_Sq;
		 /* Update AFREQ Estimate*/
		 if(pstat->Lag_Sum_Full==1)
		 {
	  pstat->AFREQ_est=( pstat->Cur_Sum -
				  pstat->Lag_Sum)/(pstat->Tau*n);
		 }
	 }  /* end for each TAUCAT category */
}
/*********************************************************************
This Task Reports the Requested Timing Statistics into shared
memory.
**********************************************************************/
void timing_stat_report(float statout[TOTAL_TAUCAT],unsigned char Chan,unsigned char Stat)
{
  unsigned char i;
  if(Chan<MAXCHAN)
  {
	if(Stat==MDEV)
	{
			 for(i=0;i<TAUCAT;i++)
			 {
				statout[i]              =sqrt((pshort[Chan]+i)->MVAR_est);
				statout[i+TAUCAT]       =sqrt((pmed[Chan]  +i)->MVAR_est);
				statout[i+TAUCAT+TAUCAT]=sqrt((plong[Chan] +i)->MVAR_est);
			 }
	}
	else if(Stat==TDEV)
	{
			 for(i=0;i<TAUCAT;i++)
			 {
				statout[i]              =sqrt((pshort[Chan]+i)->TVAR_est);
				statout[i+TAUCAT]       =sqrt((pmed[Chan]  +i)->TVAR_est);
				statout[i+TAUCAT+TAUCAT]=sqrt((plong[Chan] +i)->TVAR_est);
			 }
	}
	else if(Stat==AFREQ)
	{
			 for(i=0;i<TAUCAT;i++)
			 {
				statout[i]              =((pshort[Chan]+i)->AFREQ_est);
				statout[i+TAUCAT]       =((pmed[Chan]  +i)->AFREQ_est);
				statout[i+TAUCAT+TAUCAT]=((plong[Chan] +i)->AFREQ_est);
			 }
	}
	else if(Stat==MTIE)
	{
			 for(i=0;i<TAUCAT;i++)
			 {
				statout[i]              =((pshort[Chan]+i)->Mtie);
				statout[i+TAUCAT]       =((pmed[Chan]  +i)->Mtie);
				statout[i+TAUCAT+TAUCAT]=((plong[Chan] +i)->Mtie);
			 }
	}
  }/*only report valid channels*/
}

/*********************************************************************
This Task Reports the Requested Timing Statistics for a given channel
in a fast (no sqrt on VARS) and bundled approach.  bundle_stats is a arrange as
follows:
Current Phase index 0
MTIE          index 1-15
AFREQ         index 16-30
MVAR          index 31-45
**********************************************************************/
void tstat_report_fast(float bundle_stats[TOTAL_STATS],unsigned char Chan,unsigned char All)
{
	unsigned char i;
   if(Chan<MAXCHAN)
	{
		  bundle_stats[0]= ((pshort[Chan])->Mtie);
		  if(All)
		  {
			 for(i=0;i<TAUCAT;i++)
			 {
				bundle_stats[i+1] =((pshort[Chan]+i)->Mtie);
				bundle_stats[i+6] =((pmed[Chan]  +i)->Mtie);
				bundle_stats[i+11]=((plong[Chan] +i)->Mtie);
				bundle_stats[i+16]=((pshort[Chan]+i)->AFREQ_est);
				bundle_stats[i+21]=((pmed[Chan]  +i)->AFREQ_est);
				bundle_stats[i+26]=((plong[Chan] +i)->AFREQ_est);
				bundle_stats[i+31]=((pshort[Chan]+i)->MVAR_est);
				bundle_stats[i+36]=((pmed[Chan]  +i)->MVAR_est);
				bundle_stats[i+41]=((plong[Chan] +i)->MVAR_est);
			 }
			 bundle_stats[1]= bundle_stats[2];
		  }
		} /*only report valid channels*/
}    
#endif
/**********************************************************************
This Routine performs a three corner hat decomposition on the MVAR
stability data. The routine has been modified from the original cellsync
application. In the original application the mvar data was obtained from
the stab_ana calculator. In the BT3 application the mvar is calculated
directly from the FLL frequency input data after transient are removed.
This routine should be called once per minute to minimize load on system.
/**********************************************************************/
void three_corner_decomp(void)
{
	double tempA ,tempB,varsum;
	double Mpa_lo,Mpa_gps,Mpb_lo,Mpb_gps;
	short i,n;
	unsigned short pa_status, pb_status,fasttc;
	struct MSG_FLL_TCH *ptchrec;
	struct MSG_SR_TCH *ptchsrrec;
	/**** local message variables ******/
	double XGPS_rec,XSPA_rec,XSPB_rec,XROA_rec,XROB_rec;
	unsigned short XGPS_cnt_rec,XSPA_cnt_rec,XSPB_cnt_rec,XROA_cnt_rec,XROB_cnt_rec;
	unsigned short SPA_Mreg_rec, SPB_Mreg_rec, GPS_Mreg_rec;
	unsigned short ROA_Mreg_rec, ROB_Mreg_rec;
	/*******end local message variables*****/
	/* receive 1 sec FLL input message*/
	if(MSG_FLL_TCH_A.Ready) ptchrec= &(MSG_FLL_TCH_A);
	else ptchrec= &(MSG_FLL_TCH_B);
	XGPS_rec=ptchrec->XGPS_1Min;
	XSPA_rec=ptchrec->XSPA_1Min; XSPB_rec=ptchrec->XSPB_1Min;
	XROA_rec=ptchrec->XROA_1Min; XROB_rec=ptchrec->XROB_1Min;
	XGPS_cnt_rec=ptchrec->XGPS_cnt;
	XSPA_cnt_rec=ptchrec->XSPA_cnt; XSPB_cnt_rec=ptchrec->XSPB_cnt;
	XROA_cnt_rec=ptchrec->XROA_cnt; XROB_cnt_rec=ptchrec->XROB_cnt;
	TCH.Pmode=ptchrec->Pmode;
	ptchrec->Ready=FALSE;
	/******* complete receive of local input data *****/
	/******** check in current 1 minute data ********/
	TCH.Xgps_lo+=XGPS_rec; TCH.XGPS_cnt+=XGPS_cnt_rec;
	TCH.Xsa_lo+=XSPA_rec; TCH.XSPA_cnt+=XSPA_cnt_rec;
	TCH.Xsb_lo+=XSPB_rec; TCH.XSPB_cnt+=XSPB_cnt_rec;
	TCH.Xra_lo+=XROA_rec; TCH.XROA_cnt+=XROA_cnt_rec;
	TCH.Xrb_lo+=XROB_rec; TCH.XROB_cnt+=XROB_cnt_rec;
	/********* end check in***************************/
	/* receive 1 sec SR input message*/
	if(MSG_SR_TCH_A.Ready) ptchsrrec= &(MSG_SR_TCH_A);
	else ptchsrrec= &(MSG_SR_TCH_B);
	SPA_Mreg_rec=ptchsrrec->SPA_Mreg;
	SPB_Mreg_rec=ptchsrrec->SPB_Mreg;
	ROA_Mreg_rec=ptchsrrec->ROA_Mreg;
	ROB_Mreg_rec=ptchsrrec->ROB_Mreg;
	GPS_Mreg_rec=ptchsrrec->GPS_Mreg;
	ptchsrrec->Ready=FALSE;
	/******* complete receive of local input data *****/
	TCH.Min_Cnt++;
	if(TCH.Min_Cnt== TAU_ENSEMBLE)
	{
	  TCH.Min_Cnt=0;
	  if(TCH.XGPS_cnt) TCH.Ygps_lo =TCH.Xgps_lo/TCH.XGPS_cnt;
	  if(TCH.XSPA_cnt) TCH.Ysa_lo =  TCH.Xsa_lo/TCH.XSPA_cnt;
	  if(TCH.XSPB_cnt) TCH.Ysb_lo =  TCH.Xsb_lo/TCH.XSPB_cnt;
	  if(TCH.XROA_cnt) TCH.Yra_lo =  TCH.Xra_lo/TCH.XROA_cnt;
	  if(TCH.XROB_cnt) TCH.Yrb_lo =  TCH.Xrb_lo/TCH.XROB_cnt;
	  TCH.Xgps_lo=0.0;TCH.Xsa_lo=0.0;TCH.Xsb_lo=0.0;TCH.Xra_lo=0.0;TCH.Xrb_lo=0.0;
	  TCH.XGPS_cnt=0;TCH.XSPA_cnt=0;TCH.XSPB_cnt=0;TCH.XROA_cnt=0;TCH.XROB_cnt=0;
	  /* Update Derived Measurements */
	  TCH.Ysa_gps= TCH.Ysa_lo - TCH.Ygps_lo;
	  TCH.Ysb_gps= TCH.Ysb_lo - TCH.Ygps_lo;
	  TCH.Yra_gps= TCH.Yra_lo - TCH.Ygps_lo;
	  TCH.Yrb_gps= TCH.Yrb_lo - TCH.Ygps_lo;

	  /*update var sample estimates*/
	  TCH.Vgps_lo=TCH.Ygps_lo-TCH.Pgps_lo;TCH.Vgps_lo*=TCH.Vgps_lo/2.0;TCH.Pgps_lo=TCH.Ygps_lo;
	  TCH.Vsa_lo=TCH.Ysa_lo-TCH.Psa_lo;TCH.Vsa_lo*=TCH.Vsa_lo/2.0;TCH.Psa_lo=TCH.Ysa_lo;
	  TCH.Vsb_lo=TCH.Ysb_lo-TCH.Psb_lo;TCH.Vsb_lo*=TCH.Vsb_lo/2.0;TCH.Psb_lo=TCH.Ysb_lo;
	  TCH.Vra_lo=TCH.Yra_lo-TCH.Pra_lo;TCH.Vra_lo*=TCH.Vra_lo/2.0;TCH.Pra_lo=TCH.Yra_lo;
	  TCH.Vrb_lo=TCH.Yrb_lo-TCH.Prb_lo;TCH.Vrb_lo*=TCH.Vrb_lo/2.0;TCH.Prb_lo=TCH.Yrb_lo;
	  TCH.Vsa_gps=TCH.Ysa_gps-TCH.Psa_gps;TCH.Vsa_gps*=TCH.Vsa_gps/2.0;TCH.Psa_gps=TCH.Ysa_gps;
	  TCH.Vsb_gps=TCH.Ysb_gps-TCH.Psb_gps;TCH.Vsb_gps*=TCH.Vsb_gps/2.0;TCH.Psb_gps=TCH.Ysb_gps;
	  TCH.Vra_gps=TCH.Yra_gps-TCH.Pra_gps;TCH.Vra_gps*=TCH.Vra_gps/2.0;TCH.Pra_gps=TCH.Yra_gps;
	  TCH.Vrb_gps=TCH.Yrb_gps-TCH.Prb_gps;TCH.Vrb_gps*=TCH.Vrb_gps/2.0;TCH.Prb_gps=TCH.Yrb_gps;
	  TCH.Ygps_lo=0.0;TCH.Ysa_lo=0.0;TCH.Ysb_lo=0.0;TCH.Yra_lo=0.0;TCH.Yrb_lo=0.0;

	  /* reinitialize Sample Var of alarmed resources*/
	  fasttc=0;
	  if(GPS_Mreg_rec&0x0C||(TCH.Mgps_lo>MVARMAX))
	  {
		  TCH.Mgps_lo=GPS_MDEV_10*GPS_MDEV_10+LO_MDEV_10*LO_MDEV_10;
		  TCH.Msa_gps= SP_MDEV_10*SP_MDEV_10+GPS_MDEV_10*GPS_MDEV_10;
		  TCH.Msb_gps= MVARMAX;
		  TCH.Mra_gps= MVARMAX;
		  TCH.Mrb_gps= MVARMAX;
		  fasttc=1;
	  }
	  if(SPA_Mreg_rec&0x1BC||(TCH.Msa_lo>MVARMAX))
	  {
		 TCH.Msa_lo= MVARMAX;
		 TCH.Msa_gps= MVARMAX;
		 fasttc=1;
	  }
	  if(SPB_Mreg_rec&0x1BC||(TCH.Msb_lo>MVARMAX))
	  {
		 TCH.Msb_lo= MVARMAX;
		 TCH.Msb_gps= MVARMAX;
		 fasttc=1;
	  }
	  if(ROA_Mreg_rec&0x1BC||(TCH.Mra_lo>MVARMAX))
	  {
		 TCH.Mra_lo= MVARMAX;
		 TCH.Mra_gps= MVARMAX;
		 fasttc=1;
	  }
	  if(ROB_Mreg_rec&0x1BC||(TCH.Mrb_lo>MVARMAX))
	  {
		 TCH.Mrb_lo= MVARMAX;
		 TCH.Mrb_gps= MVARMAX;
		 fasttc=1;
	  }
	  if(TCH.Skip>0) TCH.Skip--;
	  if(!TCH.Skip)
	  {
		 TCH.Mgps_lo= TCH.Filt2*TCH.Mgps_lo+TCH.Filt1*TCH.Vgps_lo;
		 TCH.Msa_lo= TCH.Filt2*TCH.Msa_lo+TCH.Filt1*TCH.Vsa_lo;
		 TCH.Msb_lo= TCH.Filt2*TCH.Msb_lo+TCH.Filt1*TCH.Vsb_lo;
		 TCH.Mra_lo= TCH.Filt2*TCH.Mra_lo+TCH.Filt1*TCH.Vra_lo;
		 TCH.Mrb_lo= TCH.Filt2*TCH.Mrb_lo+TCH.Filt1*TCH.Vrb_lo;
		 TCH.Msa_gps= TCH.Filt2*TCH.Msa_gps+TCH.Filt1*TCH.Vsa_gps;
		 TCH.Msb_gps= TCH.Filt2*TCH.Msb_gps+TCH.Filt1*TCH.Vsb_gps;
		 TCH.Mra_gps= TCH.Filt2*TCH.Mra_gps+TCH.Filt1*TCH.Vra_gps;
		 TCH.Mrb_gps= TCH.Filt2*TCH.Mrb_gps+TCH.Filt1*TCH.Vrb_gps;
	  } /*end update mvar estimates*/
		if(TCH.Fshift) TCH.Fshift--;
		if(TCH.Fshift==1)
		{
			if(TCH.Filt1>MVARFILT1) TCH.Filt1/=2.0;
			if(TCH.Filt1<MVARFILT1)
			{
				 TCH.Filt1=MVARFILT1;
			}
			else
			{
			  TCH.Fshift = (unsigned short)(2.0/TCH.Filt1);
			}
			TCH.Filt2=1.0 - TCH.Filt1;
	  }
	  if(fasttc)
	  {
		  TCH.Filt1=0.2;
		  TCH.Filt2=1.0 - TCH.Filt1;
		  TCH.Fshift=20;
	  }
	  if(TCH.Pmode==EXTERNAL_MODE)
	  {
		 pa_status= ROA_Mreg_rec;
		 pb_status= ROB_Mreg_rec;
		 Mpa_gps=TCH.Mra_gps;
		 Mpa_lo=TCH.Mra_lo;
		 Mpb_gps=TCH.Mrb_gps;
		 Mpb_lo=TCH.Mrb_lo;
	  }
	  else
	  {
		 pa_status= SPA_Mreg_rec;
		 pb_status= SPB_Mreg_rec;
		 Mpa_gps=TCH.Msa_gps;
		 Mpa_lo=TCH.Msa_lo;
		 Mpb_gps=TCH.Msb_gps;
		 Mpb_lo=TCH.Msb_lo;
	  }

	  /******* GPS mvar estimation ********/
	  tempA = (TCH.Mgps_lo + Mpa_gps -
						Mpa_lo)/2;
	  tempB = (TCH.Mgps_lo + Mpb_gps -
						Mpb_lo)/2;
	  varsum=0.0;
	  n=0;
	  /**** test for excessive noise on external input and skip decomposition***/
	  if(Mpa_lo<(TCH.Mgps_lo*50.0))
	  {
		 if(!pa_status && !GPS_Mreg_rec)
		 {
			n++;
			varsum += tempA;
		 }
	  }
	  if(Mpb_lo<(TCH.Mgps_lo*50.0))
	  {
		if(!pb_status&&!GPS_Mreg_rec)
		{
		  n++;
		  varsum += tempB;
		 }
	  }
	  if(n>0)TCH.GPS_mvar= varsum/n;
	  else TCH.GPS_mvar= TCH.Mgps_lo; /*use lo as reference if no external inputs*/
	  if(TCH.GPS_mvar<0.0) TCH.GPS_mvar = -TCH.GPS_mvar;
	  if(TCH.GPS_mvar<MVARMIN)TCH.GPS_mvar=MVARMIN;
	  smi.GPS_mvar= TCH.GPS_mvar;
	  smi.GPS_mdev=sqrt(TCH.GPS_mvar);
	  /*********** END GPS mvar estimation ****************/
	  if(!pa_status && !GPS_Mreg_rec)
	  {
		 TCH.PA_mvar = (Mpa_lo + Mpa_gps -
						TCH.Mgps_lo)/2;
		 if(TCH.PA_mvar<0.0) TCH.PA_mvar = -TCH.PA_mvar;
		 if(TCH.PA_mvar<MVARMIN)TCH.PA_mvar=MVARMIN;
		 smi.PA_mvar= TCH.PA_mvar;
		 smi.PA_mdev=sqrt(TCH.PA_mvar);
	  }
	  if(!pb_status && !GPS_Mreg_rec)
	  {
		 TCH.PB_mvar = (Mpb_lo + Mpb_gps -
						TCH.Mgps_lo)/2;
		 if(TCH.PB_mvar<0.0) TCH.PB_mvar = -TCH.PB_mvar;
		 if(TCH.PB_mvar<MVARMIN)TCH.PB_mvar=MVARMIN;
		 smi.PB_mvar= TCH.PB_mvar;
		 smi.PB_mdev=sqrt(TCH.PB_mvar);
	  }
	  /******* LO mvar estimation ********/
	  tempA = (Mpa_lo + TCH.Mgps_lo -
						Mpa_gps)/2;
	  tempB = (Mpb_lo + TCH.Mgps_lo -
						Mpb_gps)/2;
	  varsum=0.0;
	  n=0;
	  if(Mpa_lo<(TCH.Mgps_lo*50.0))
	  {
		 if(!pa_status && !GPS_Mreg_rec)
		 {
			n++;
			varsum += tempA;
		 }
	  }
	  if(Mpb_lo<(TCH.Mgps_lo*50.0))
	  {
		 if(!pb_status&&!GPS_Mreg_rec)
		 {
			n++;
			varsum += tempB;
		 }
	  }
	  if(n>0)varsum = varsum/n;
	  if(varsum<0.0) varsum = -varsum;
	  if(varsum<MVARMIN)varsum=MVARMIN;
     TCH.LO_mvar=varsum;
	  smi.LO_mvar=varsum;
	  smi.LO_mdev=sqrt(varsum);
	  /*********** END LO mvar estimation ****************/
	  /****** Generate output message to NCO Update task*/
	  MSG_TCH_NCOUP_A.GPS_mvar=TCH.GPS_mvar;
	  MSG_TCH_NCOUP_A.PA_mvar=TCH.PA_mvar;
	  MSG_TCH_NCOUP_A.PB_mvar=TCH.PB_mvar;
	  MSG_TCH_NCOUP_A.LO_mvar=TCH.LO_mvar;
	  MSG_TCH_NCOUP_A.Ready=TRUE;
	}/*end ensemble update logic */

} 

/********************************************************************
Initialization Task
Called only at restart of analysis
**********************************************************************/

void init_stat(double TAU_Zero)
{
	 int i,j;   
#if 0	 
	 for(i=0;i<MAXCHAN; i++)
	 {
	  Update_Count[i] =0L;
	  Mtie_Count[i]= 0L;
	 }
	 Misc_Stab_Cnt=0L;
	 Medium_Update=NMAX*2;
	 Long_Update  =NMAX*NMAX*4;
	 /*set up "N factors" for each of the TAU Categories*/
	 Calc_TAU[0]=1;
	 Smooth_TAU[0] = 1.0/(Calc_TAU[0]*SMOOTH_FACTOR);
	 for(i=1;i<TAUCAT;i++)
	 {
	Calc_TAU[i]=Calc_TAU[i-1]*2; /*octet spaced TAUs*/
	Smooth_TAU[i] = 1.0/(Calc_TAU[i]*SMOOTH_FACTOR);
	 }
	 /* Setup statistic pointers */
	  pshort[0]=Short_Stats0;
	  pshort[1]=Short_Stats1;
	  pshort[2]=Short_Stats2;
	  pmed[0]=Med_Stats0;
	  pmed[1]=Med_Stats1;
	  pmed[2]=Med_Stats2;
	  plong[0]=Long_Stats0;
	  plong[1]=Long_Stats1;
	  plong[2]=Long_Stats2;
	 /* Initialize Channel Data */
 for(i=0;i<MAXCHAN;i++)
 {
	#if PCSIM
	for(j=0;j<BUFMAX;j++)
	{
		 Cdata[i].Short_Buf[j]=0.0;
		 Cdata[i].Med_Buf[j]	 =0.0;
		 Cdata[i].Long_Buf[j] =0.0;
	}
	Cdata[i].X1000ms=0.0;
	#endif
	Cdata[i].Data_Valid=TRUE; /* per channel validity flag */
	for(j=0;j<TAUCAT;j++)
	{
		 #if PCSIM
		 (pshort[i]+j)->Ntotal= 0;
		 (pmed[i]+j)->Ntotal= 0;
		 (plong[i]+j)->Ntotal= 0;
		 (pshort[i]+j)->Cur_Sum_Full= 0;
		 (pmed[i]+j)->Cur_Sum_Full= 0;
		 (plong[i]+j)->Cur_Sum_Full= 0;
		 (pshort[i]+j)->Lag_Sum_Full= 0;
		 (pmed[i]+j)->Lag_Sum_Full= 0;
		 (plong[i]+j)->Lag_Sum_Full= 0;
		 (pshort[i]+j)->Dlag_Sum_Full= 0;
		 (pmed[i]+j)->Dlag_Sum_Full= 0;
		 (plong[i]+j)->Dlag_Sum_Full= 0;
		 (pshort[i]+j)->Cur_Samp = 0;
		 (pmed[i]+j)->Cur_Samp = 0;
		 (plong[i]+j)->Cur_Samp = 0;
		 (pshort[i]+j)->Lag_Samp = 0;
		 (pmed[i]+j)->Lag_Samp = 0;
		 (plong[i]+j)->Lag_Samp = 0;
		 (pshort[i]+j)->Dlag_Samp = 0;
		 (pmed[i]+j)->Dlag_Samp = 0;
		 (plong[i]+j)->Dlag_Samp = 0;
		 (pshort[i]+j)->Cur_Sum = 0;
		 (pmed[i]+j)->Cur_Sum = 0;
		 (plong[i]+j)->Cur_Sum = 0;
		 (pshort[i]+j)->Lag_Sum = 0;
		 (pmed[i]+j)->Lag_Sum = 0;
		 (plong[i]+j)->Lag_Sum = 0;
		 (pshort[i]+j)->Dlag_Sum = 0;
		 (pmed[i]+j)->Dlag_Sum = 0;
		 (plong[i]+j)->Dlag_Sum = 0;
		 (pshort[i]+j)->Sec_Diff = 0.0;
		 (pmed[i]+j)->Sec_Diff = 0.0;
		 (plong[i]+j)->Sec_Diff= 0.0;
		 (pshort[i]+j)->TVAR_est  = 0.0;
		 (pmed[i]+j)->TVAR_est = 0.0;
		 (plong[i]+j)->TVAR_est= 0.0;
		 (pshort[i]+j)->MVAR_est  = 0.0;
		 (pmed[i]+j)->MVAR_est = 0.0;
		 (plong[i]+j)->MVAR_est= 0.0;
		 (pshort[i]+j)->AFREQ_est= 0.0;
		 (pmed[i]+j)->AFREQ_est	= 0.0;
		 (plong[i]+j)->AFREQ_est = 0.0;
		 #endif
		 (pshort[i]+j)->Tau	 =Calc_TAU[j]*TAU_Zero;
		 (pshort[i]+j)->Tau_Sq= (pshort[i]+j)->Tau  *  (pshort[i]+j)->Tau;
		 (pmed[i]+j)->Tau  = Calc_TAU[j]*TAU_Zero*  Medium_Update;
		 (pmed[i]+j)->Tau_Sq= (pmed[i]+j)->Tau	*  (pmed[i]+j)->Tau;
		 (plong[i]+j)->Tau =  Calc_TAU[j]*TAU_Zero* Long_Update;
		 (plong[i]+j)->Tau_Sq =	(plong[i]+j)->Tau *  (plong[i]+j)->Tau;
		 /*** MTIE Stat Init****/
		 (pshort[i]+j)->Min_Cur = MINRESET;
		 (pmed[i]+j)->Min_Cur	= MINRESET;
		 (plong[i]+j)->Min_Cur  = MINRESET;
		 (pshort[i]+j)->Max_Cur = MAXRESET;
		 (pmed[i]+j)->Max_Cur	= MAXRESET;
		 (plong[i]+j)->Max_Cur  = MAXRESET;
		 (pshort[i]+j)->Mtie = 0.0;
		 (pmed[i]+j)->Mtie	= 0.0;
		 (plong[i]+j)->Mtie  = 0.0;
		 (pshort[i]+j)->Mtie_Reset = FALSE;
		 (pmed[i]+j)->Mtie_Reset	= FALSE;
		 (plong[i]+j)->Mtie_Reset  = FALSE;
	}
 }   
 #endif
 
 /********************* Special Initialization of Stability Masks ****/
/*******************************
(pshort[GPS_LO]+0)->TVAR_est =((GPS_MDEV_0*GPS_MDEV_0)+(LO_MDEV_0*LO_MDEV_0))
											* (pshort[GPS_LO]+0)->Tau_Sq/3.0;
(pshort[GPS_LO]+1)->TVAR_est =((GPS_MDEV_1*GPS_MDEV_1)+(LO_MDEV_1*LO_MDEV_1))
											* (pshort[GPS_LO]+1)->Tau_Sq/3.0;
(pshort[GPS_LO]+2)->TVAR_est =((GPS_MDEV_2*GPS_MDEV_2)+(LO_MDEV_2*LO_MDEV_2))
											* (pshort[GPS_LO]+2)->Tau_Sq/3.0;
(pshort[GPS_LO]+3)->TVAR_est =((GPS_MDEV_3*GPS_MDEV_3)+(LO_MDEV_3*LO_MDEV_3))
											* (pshort[GPS_LO]+3)->Tau_Sq/3.0;
(pshort[GPS_LO]+4)->TVAR_est =((GPS_MDEV_4*GPS_MDEV_4)+(LO_MDEV_4*LO_MDEV_4))
											* (pshort[GPS_LO]+4)->Tau_Sq/3.0;
(pmed[GPS_LO]+0)->TVAR_est =((GPS_MDEV_5*GPS_MDEV_5)+(LO_MDEV_5*LO_MDEV_5))
											* (pmed[GPS_LO]+0)->Tau_Sq/3.0;
(pmed[GPS_LO]+1)->TVAR_est =((GPS_MDEV_6*GPS_MDEV_6)+(LO_MDEV_6*LO_MDEV_6))
											* (pmed[GPS_LO]+1)->Tau_Sq/3.0;
(pmed[GPS_LO]+2)->TVAR_est =((GPS_MDEV_7*GPS_MDEV_7)+(LO_MDEV_7*LO_MDEV_7))
											* (pmed[GPS_LO]+2)->Tau_Sq/3.0;
(pmed[GPS_LO]+3)->TVAR_est =((GPS_MDEV_8*GPS_MDEV_8)+(LO_MDEV_8*LO_MDEV_8))
											* (pmed[GPS_LO]+3)->Tau_Sq/3.0;
(pmed[GPS_LO]+4)->TVAR_est =((GPS_MDEV_9*GPS_MDEV_9)+(LO_MDEV_9*LO_MDEV_9))
											* (pmed[GPS_LO]+4)->Tau_Sq/3.0;
(plong[GPS_LO]+0)->TVAR_est =((GPS_MDEV_10*GPS_MDEV_10)+(LO_MDEV_10*LO_MDEV_10))
											* (plong[GPS_LO]+0)->Tau_Sq/3.0;
(plong[GPS_LO]+1)->TVAR_est =((GPS_MDEV_11*GPS_MDEV_11)+(LO_MDEV_11*LO_MDEV_11))
											* (plong[GPS_LO]+1)->Tau_Sq/3.0;
(plong[GPS_LO]+2)->TVAR_est =((GPS_MDEV_12*GPS_MDEV_12)+(LO_MDEV_12*LO_MDEV_12))
											* (plong[GPS_LO]+2)->Tau_Sq/3.0;
(plong[GPS_LO]+3)->TVAR_est =((GPS_MDEV_13*GPS_MDEV_13)+(LO_MDEV_13*LO_MDEV_13))
											* (plong[GPS_LO]+3)->Tau_Sq/3.0;
(plong[GPS_LO]+4)->TVAR_est =((GPS_MDEV_14*GPS_MDEV_14)+(LO_MDEV_14*LO_MDEV_14))
											* (plong[GPS_LO]+4)->Tau_Sq/3.0;



(pshort[SA_LO]+0)->TVAR_est =((SP_MDEV_0*SP_MDEV_0)+(LO_MDEV_0*LO_MDEV_0))
											* (pshort[SA_LO]+0)->Tau_Sq/3.0;
(pshort[SA_LO]+1)->TVAR_est =((SP_MDEV_1*SP_MDEV_1)+(LO_MDEV_1*LO_MDEV_1))
											* (pshort[SA_LO]+1)->Tau_Sq/3.0;
(pshort[SA_LO]+2)->TVAR_est =((SP_MDEV_2*SP_MDEV_2)+(LO_MDEV_2*LO_MDEV_2))
											* (pshort[SA_LO]+2)->Tau_Sq/3.0;
(pshort[SA_LO]+3)->TVAR_est =((SP_MDEV_3*SP_MDEV_3)+(LO_MDEV_3*LO_MDEV_3))
											* (pshort[SA_LO]+3)->Tau_Sq/3.0;
(pshort[SA_LO]+4)->TVAR_est =((SP_MDEV_4*SP_MDEV_4)+(LO_MDEV_4*LO_MDEV_4))
											* (pshort[SA_LO]+4)->Tau_Sq/3.0;
(pmed[SA_LO]+0)->TVAR_est =((SP_MDEV_5*SP_MDEV_5)+(LO_MDEV_5*LO_MDEV_5))
											* (pmed[SA_LO]+0)->Tau_Sq/3.0;
(pmed[SA_LO]+1)->TVAR_est =((SP_MDEV_6*SP_MDEV_6)+(LO_MDEV_6*LO_MDEV_6))
											* (pmed[SA_LO]+1)->Tau_Sq/3.0;
(pmed[SA_LO]+2)->TVAR_est =((SP_MDEV_7*SP_MDEV_7)+(LO_MDEV_7*LO_MDEV_7))
											* (pmed[SA_LO]+2)->Tau_Sq/3.0;
(pmed[SA_LO]+3)->TVAR_est =((SP_MDEV_8*SP_MDEV_8)+(LO_MDEV_8*LO_MDEV_8))
											* (pmed[SA_LO]+3)->Tau_Sq/3.0;
(pmed[SA_LO]+4)->TVAR_est =((SP_MDEV_9*SP_MDEV_9)+(LO_MDEV_9*LO_MDEV_9))
											* (pmed[SA_LO]+4)->Tau_Sq/3.0;
(plong[SA_LO]+0)->TVAR_est =((SP_MDEV_10*SP_MDEV_10)+(LO_MDEV_10*LO_MDEV_10))
											* (plong[SA_LO]+0)->Tau_Sq/3.0;
(plong[SA_LO]+1)->TVAR_est =((SP_MDEV_11*SP_MDEV_11)+(LO_MDEV_11*LO_MDEV_11))
											* (plong[SA_LO]+1)->Tau_Sq/3.0;
(plong[SA_LO]+2)->TVAR_est =((SP_MDEV_12*SP_MDEV_12)+(LO_MDEV_12*LO_MDEV_12))
											* (plong[SA_LO]+2)->Tau_Sq/3.0;
(plong[SA_LO]+3)->TVAR_est =((SP_MDEV_13*SP_MDEV_13)+(LO_MDEV_13*LO_MDEV_13))
											* (plong[SA_LO]+3)->Tau_Sq/3.0;
(plong[SA_LO]+4)->TVAR_est =((SP_MDEV_14*SP_MDEV_14)+(LO_MDEV_14*LO_MDEV_14))
											* (plong[SA_LO]+4)->Tau_Sq/3.0;


(pshort[SB_LO]+0)->TVAR_est =((SP_MDEV_0*SP_MDEV_0)+(LO_MDEV_0*LO_MDEV_0))
											* (pshort[SB_LO]+0)->Tau_Sq/3.0;
(pshort[SB_LO]+1)->TVAR_est =((SP_MDEV_1*SP_MDEV_1)+(LO_MDEV_1*LO_MDEV_1))
											* (pshort[SB_LO]+1)->Tau_Sq/3.0;
(pshort[SB_LO]+2)->TVAR_est =((SP_MDEV_2*SP_MDEV_2)+(LO_MDEV_2*LO_MDEV_2))
											* (pshort[SB_LO]+2)->Tau_Sq/3.0;
(pshort[SB_LO]+3)->TVAR_est =((SP_MDEV_3*SP_MDEV_3)+(LO_MDEV_3*LO_MDEV_3))
											* (pshort[SB_LO]+3)->Tau_Sq/3.0;
(pshort[SB_LO]+4)->TVAR_est =((SP_MDEV_4*SP_MDEV_4)+(LO_MDEV_4*LO_MDEV_4))
											* (pshort[SB_LO]+4)->Tau_Sq/3.0;
(pmed[SB_LO]+0)->TVAR_est =((SP_MDEV_5*SP_MDEV_5)+(LO_MDEV_5*LO_MDEV_5))
											* (pmed[SB_LO]+0)->Tau_Sq/3.0;
(pmed[SB_LO]+1)->TVAR_est =((SP_MDEV_6*SP_MDEV_6)+(LO_MDEV_6*LO_MDEV_6))
											* (pmed[SB_LO]+1)->Tau_Sq/3.0;
(pmed[SB_LO]+2)->TVAR_est =((SP_MDEV_7*SP_MDEV_7)+(LO_MDEV_7*LO_MDEV_7))
											* (pmed[SB_LO]+2)->Tau_Sq/3.0;
(pmed[SB_LO]+3)->TVAR_est =((SP_MDEV_8*SP_MDEV_8)+(LO_MDEV_8*LO_MDEV_8))
											* (pmed[SB_LO]+3)->Tau_Sq/3.0;
(pmed[SB_LO]+4)->TVAR_est =((SP_MDEV_9*SP_MDEV_9)+(LO_MDEV_9*LO_MDEV_9))
											* (pmed[SB_LO]+4)->Tau_Sq/3.0;
(plong[SB_LO]+0)->TVAR_est =((SP_MDEV_10*SP_MDEV_10)+(LO_MDEV_10*LO_MDEV_10))
											* (plong[SB_LO]+0)->Tau_Sq/3.0;
(plong[SB_LO]+1)->TVAR_est =((SP_MDEV_11*SP_MDEV_11)+(LO_MDEV_11*LO_MDEV_11))
											* (plong[SB_LO]+1)->Tau_Sq/3.0;
(plong[SB_LO]+2)->TVAR_est =((SP_MDEV_12*SP_MDEV_12)+(LO_MDEV_12*LO_MDEV_12))
											* (plong[SB_LO]+2)->Tau_Sq/3.0;
(plong[SB_LO]+3)->TVAR_est =((SP_MDEV_13*SP_MDEV_13)+(LO_MDEV_13*LO_MDEV_13))
											* (plong[SB_LO]+3)->Tau_Sq/3.0;
(plong[SB_LO]+4)->TVAR_est =((SP_MDEV_14*SP_MDEV_14)+(LO_MDEV_14*LO_MDEV_14))
											* (plong[SB_LO]+4)->Tau_Sq/3.0;


(pshort[SA_GPS]+0)->TVAR_est =((SP_MDEV_0*SP_MDEV_0)+(GPS_MDEV_0*GPS_MDEV_0))
											* (pshort[SA_GPS]+0)->Tau_Sq/3.0;
(pshort[SA_GPS]+1)->TVAR_est =((SP_MDEV_1*SP_MDEV_1)+(GPS_MDEV_1*GPS_MDEV_1))
											* (pshort[SA_GPS]+1)->Tau_Sq/3.0;
(pshort[SA_GPS]+2)->TVAR_est =((SP_MDEV_2*SP_MDEV_2)+(GPS_MDEV_2*GPS_MDEV_2))
											* (pshort[SA_GPS]+2)->Tau_Sq/3.0;
(pshort[SA_GPS]+3)->TVAR_est =((SP_MDEV_3*SP_MDEV_3)+(GPS_MDEV_3*GPS_MDEV_3))
											* (pshort[SA_GPS]+3)->Tau_Sq/3.0;
(pshort[SA_GPS]+4)->TVAR_est =((SP_MDEV_4*SP_MDEV_4)+(GPS_MDEV_4*GPS_MDEV_4))
											* (pshort[SA_GPS]+4)->Tau_Sq/3.0;
(pmed[SA_GPS]+0)->TVAR_est =((SP_MDEV_5*SP_MDEV_5)+(GPS_MDEV_5*GPS_MDEV_5))
											* (pmed[SA_GPS]+0)->Tau_Sq/3.0;
(pmed[SA_GPS]+1)->TVAR_est =((SP_MDEV_6*SP_MDEV_6)+(GPS_MDEV_6*GPS_MDEV_6))
											* (pmed[SA_GPS]+1)->Tau_Sq/3.0;
(pmed[SA_GPS]+2)->TVAR_est =((SP_MDEV_7*SP_MDEV_7)+(GPS_MDEV_7*GPS_MDEV_7))
											* (pmed[SA_GPS]+2)->Tau_Sq/3.0;
(pmed[SA_GPS]+3)->TVAR_est =((SP_MDEV_8*SP_MDEV_8)+(GPS_MDEV_8*GPS_MDEV_8))
											* (pmed[SA_GPS]+3)->Tau_Sq/3.0;
(pmed[SA_GPS]+4)->TVAR_est =((SP_MDEV_9*SP_MDEV_9)+(GPS_MDEV_9*GPS_MDEV_9))
											* (pmed[SA_GPS]+4)->Tau_Sq/3.0;
(plong[SA_GPS]+0)->TVAR_est =((SP_MDEV_10*SP_MDEV_10)+(GPS_MDEV_10*GPS_MDEV_10))
											* (plong[SA_GPS]+0)->Tau_Sq/3.0;
(plong[SA_GPS]+1)->TVAR_est =((SP_MDEV_11*SP_MDEV_11)+(GPS_MDEV_11*GPS_MDEV_11))
											* (plong[SA_GPS]+1)->Tau_Sq/3.0;
(plong[SA_GPS]+2)->TVAR_est =((SP_MDEV_12*SP_MDEV_12)+(GPS_MDEV_12*GPS_MDEV_12))
											* (plong[SA_GPS]+2)->Tau_Sq/3.0;
(plong[SA_GPS]+3)->TVAR_est =((SP_MDEV_13*SP_MDEV_13)+(GPS_MDEV_13*GPS_MDEV_13))
											* (plong[SA_GPS]+3)->Tau_Sq/3.0;
(plong[SA_GPS]+4)->TVAR_est =((SP_MDEV_14*SP_MDEV_14)+(GPS_MDEV_14*GPS_MDEV_14))
											* (plong[SA_GPS]+4)->Tau_Sq/3.0;

(pshort[SB_GPS]+0)->TVAR_est =((SP_MDEV_0*SP_MDEV_0)+(GPS_MDEV_0*GPS_MDEV_0))
											* (pshort[SB_GPS]+0)->Tau_Sq/3.0;
(pshort[SB_GPS]+1)->TVAR_est =((SP_MDEV_1*SP_MDEV_1)+(GPS_MDEV_1*GPS_MDEV_1))
											* (pshort[SB_GPS]+1)->Tau_Sq/3.0;
(pshort[SB_GPS]+2)->TVAR_est =((SP_MDEV_2*SP_MDEV_2)+(GPS_MDEV_2*GPS_MDEV_2))
											* (pshort[SB_GPS]+2)->Tau_Sq/3.0;
(pshort[SB_GPS]+3)->TVAR_est =((SP_MDEV_3*SP_MDEV_3)+(GPS_MDEV_3*GPS_MDEV_3))
											* (pshort[SB_GPS]+3)->Tau_Sq/3.0;
(pshort[SB_GPS]+4)->TVAR_est =((SP_MDEV_4*SP_MDEV_4)+(GPS_MDEV_4*GPS_MDEV_4))
											* (pshort[SB_GPS]+4)->Tau_Sq/3.0;
(pmed[SB_GPS]+0)->TVAR_est =((SP_MDEV_5*SP_MDEV_5)+(GPS_MDEV_5*GPS_MDEV_5))
											* (pmed[SB_GPS]+0)->Tau_Sq/3.0;
(pmed[SB_GPS]+1)->TVAR_est =((SP_MDEV_6*SP_MDEV_6)+(GPS_MDEV_6*GPS_MDEV_6))
											* (pmed[SB_GPS]+1)->Tau_Sq/3.0;
(pmed[SB_GPS]+2)->TVAR_est =((SP_MDEV_7*SP_MDEV_7)+(GPS_MDEV_7*GPS_MDEV_7))
											* (pmed[SB_GPS]+2)->Tau_Sq/3.0;
(pmed[SB_GPS]+3)->TVAR_est =((SP_MDEV_8*SP_MDEV_8)+(GPS_MDEV_8*GPS_MDEV_8))
											* (pmed[SB_GPS]+3)->Tau_Sq/3.0;
(pmed[SB_GPS]+4)->TVAR_est =((SP_MDEV_9*SP_MDEV_9)+(GPS_MDEV_9*GPS_MDEV_9))
											* (pmed[SB_GPS]+4)->Tau_Sq/3.0;
(plong[SB_GPS]+0)->TVAR_est =((SP_MDEV_10*SP_MDEV_10)+(GPS_MDEV_10*GPS_MDEV_10))
											* (plong[SB_GPS]+0)->Tau_Sq/3.0;
(plong[SB_GPS]+1)->TVAR_est =((SP_MDEV_11*SP_MDEV_11)+(GPS_MDEV_11*GPS_MDEV_11))
											* (plong[SB_GPS]+1)->Tau_Sq/3.0;
(plong[SB_GPS]+2)->TVAR_est =((SP_MDEV_12*SP_MDEV_12)+(GPS_MDEV_12*GPS_MDEV_12))
											* (plong[SB_GPS]+2)->Tau_Sq/3.0;
(plong[SB_GPS]+3)->TVAR_est =((SP_MDEV_13*SP_MDEV_13)+(GPS_MDEV_13*GPS_MDEV_13))
											* (plong[SB_GPS]+3)->Tau_Sq/3.0;
(plong[SB_GPS]+4)->TVAR_est =((SP_MDEV_14*SP_MDEV_14)+(GPS_MDEV_14*GPS_MDEV_14))
											* (plong[SB_GPS]+4)->Tau_Sq/3.0;
*******************************************/
 #if PCSIM
 TCH.Xgps_lo=0.0;TCH.Xsa_lo=0.0;TCH.Xsb_lo=0.0;TCH.Xra_lo=0.0;TCH.Xrb_lo=0.0;
 TCH.Ygps_lo=0.0;TCH.Ysa_lo=0.0;TCH.Ysb_lo=0.0;TCH.Yra_lo=0.0;TCH.Yrb_lo=0.0;
 TCH.Ysa_gps=0.0;TCH.Ysb_gps=0.0;TCH.Yra_gps=0.0;TCH.Yrb_gps=0.0;
 TCH.Pgps_lo=0.0;TCH.Psa_lo=0.0;TCH.Psb_lo=0.0;TCH.Pra_lo=0.0;TCH.Prb_lo=0.0;
 TCH.Psa_gps=0.0;TCH.Psb_gps=0.0;TCH.Pra_gps=0.0;TCH.Prb_gps=0.0;
 TCH.Vgps_lo=0.0;TCH.Vsa_lo=0.0;TCH.Vsb_lo=0.0;TCH.Vra_lo=0.0;TCH.Vrb_lo=0.0;
 TCH.Vsa_gps=0.0;TCH.Vsb_gps=0.0;TCH.Vra_gps=0.0;TCH.Vrb_gps=0.0;
 TCH.GPS_mvar=0.0; TCH.PA_mvar=0.0;
 TCH.PB_mvar=0.0;  TCH.LO_mvar=0.0;
 TCH.XGPS_cnt=0;
 TCH.XSPA_cnt=0; TCH.XSPB_cnt=0;
 TCH.XROA_cnt=0; TCH.XROB_cnt=0;
 #endif
 TCH.Mgps_lo=GPS_MDEV_10*GPS_MDEV_10+LO_MDEV_10*LO_MDEV_10;
 TCH.Msa_lo= SP_MDEV_10*SP_MDEV_10+LO_MDEV_10*LO_MDEV_10;
 TCH.Msb_lo= TCH.Msa_lo;
 TCH.Mra_lo= 2.0*LO_MDEV_10*LO_MDEV_10;
 TCH.Mrb_lo= TCH.Mra_lo;
 TCH.Msa_gps= SP_MDEV_10*SP_MDEV_10+GPS_MDEV_10*GPS_MDEV_10;
 TCH.Msb_gps= TCH.Msa_gps;
 TCH.Mra_gps= TCH.Mgps_lo;
 TCH.Mrb_gps= TCH.Mgps_lo;
 TCH.Filt1 = 0.2;
 TCH.Filt2 = 1.0-TCH.Filt1;
 TCH.Fshift= 10;
 TCH.Skip=3; /* allow several minute start-up*/
 TCH.Min_Cnt=0;
 TCH.Pmode= SPAN_MODE;
} 
#if 0
void stat_normalize(int Which_Chan)
{
	 long int *pbufs,*pbufm,*pbufl;
	 int i,j,n;
	 struct Per_Tau_Stats *pstats,*pstatm,*pstatl;
	 long int Norml;
	 double   Normd;
	 #if PRINTLOG
	 printf("%s%5ld:stat_normalize:Renormalize stat database channel %5d\n",PHEAD,ptime,Which_Chan);
	 #endif
	 pbufs = &(Cdata[Which_Chan].Short_Buf[0]);
	 pstats= pshort[Which_Chan];
	 pbufm = &(Cdata[Which_Chan].Med_Buf[0]);
	 pstatm= pmed[Which_Chan];
	 pbufl = &(Cdata[Which_Chan].Long_Buf[0]);
	 pstatl= plong[Which_Chan];
	 /* add normalization to all buffers*/
	 Normd = - Cdata[Which_Chan].X1000ms;
	 Norml = (long int)(Normd*BUFSCALE);
	/*Reset accumulated phase error in channel*/
	Cdata[Which_Chan].X1000ms=0.0;
	Mtie_Count[Which_Chan]=0L;
	for(i=0;i<BUFMAX;i++)
	{
		 *(pbufs+i)+= Norml;
		 *(pbufm+i)+= Norml;
		 *(pbufl+i)+= Norml;
	}
	 for(i=0;i<TAUCAT;i++)
	 {
		  n=Calc_TAU[i];  /*number of samples in steady state sum */
		  /* normalize all sums */
		  if((pstats+TAUCAT-1)->Dlag_Sum_Full)
		  {
		  (pstats+i)->Cur_Sum  += Normd*n;
		  (pstats+i)->Lag_Sum  += Normd*n;
		  (pstats+i)->Dlag_Sum += Normd*n;
		  }
		  else
		  {
			(pstats+i)->Cur_Sum  = 0.0;
			(pstats+i)->Lag_Sum  = 0.0;
			(pstats+i)->Dlag_Sum = 0.0;
			(pstats+i)->Cur_Sum_Full  = 0;
			(pstats+i)->Lag_Sum_Full  = 0;
			(pstats+i)->Dlag_Sum_Full = 0;
			(pstats+i)->Ntotal   =   0;
			(pstats+i)->Cur_Samp  =   0;
			(pstats+i)->Lag_Samp  =   0;
			(pstats+i)->Dlag_Samp =   0;
			if(i==(TAUCAT-1))
			{
			  for(j=0;j<BUFMAX;j++)
			  {
			   *(pbufs+j)=0.0;
			  }
			}
		  }
		  (pstats+i)->Min_Cur  = MINRESET;
		  (pstats+i)->Max_Cur  = MAXRESET;
		  (pstats+i)->Mtie     = 0.0;
		  if((pstatm+TAUCAT-1)->Dlag_Sum_Full)
		  {
		  (pstatm+i)->Cur_Sum  += Normd*n;
		  (pstatm+i)->Lag_Sum  += Normd*n;
		  (pstatm+i)->Dlag_Sum += Normd*n;
		  }
		  else
		  {
			(pstatm+i)->Cur_Sum  = 0.0;
			(pstatm+i)->Lag_Sum  = 0.0;
			(pstatm+i)->Dlag_Sum = 0.0;
			(pstatm+i)->Ntotal   =   0;
			(pstatm+i)->Cur_Sum_Full  = 0;
			(pstatm+i)->Lag_Sum_Full  = 0;
			(pstatm+i)->Dlag_Sum_Full = 0;
			(pstatm+i)->Cur_Samp  =   0;
			(pstatm+i)->Lag_Samp  =   0;
			(pstatm+i)->Dlag_Samp =   0;
			if(i==(TAUCAT-1))
			{
			  for(j=0;j<BUFMAX;j++)
			  {
			   *(pbufm+j)=0.0;
			  }
			}
		  }
		  (pstatm+i)->Min_Cur  = MINRESET;
		  (pstatm+i)->Max_Cur  = MAXRESET;
		  (pstatm+i)->Mtie     = 0.0;
		  if((pstatl+TAUCAT-1)->Dlag_Sum_Full)
		  {
		  (pstatl+i)->Cur_Sum  += Normd*n;
		  (pstatl+i)->Lag_Sum  += Normd*n;
		  (pstatl+i)->Dlag_Sum += Normd*n;
		  }
		  else
		  {
			(pstatl+i)->Cur_Sum  = 0.0;
			(pstatl+i)->Lag_Sum  = 0.0;
			(pstatl+i)->Dlag_Sum = 0.0;
			(pstatl+i)->Cur_Sum_Full  = 0;
			(pstatl+i)->Lag_Sum_Full  = 0;
			(pstatl+i)->Dlag_Sum_Full = 0;
			(pstatl+i)->Ntotal   =   0;
			(pstatl+i)->Cur_Samp  =   0;
			(pstatl+i)->Lag_Samp  =   0;
			(pstatl+i)->Dlag_Samp =   0;
			if(i==(TAUCAT-1))
			{
			  for(j=0;j<BUFMAX;j++)
			  {
				 *(pbufl+j)=0.0;
			  }
			}
		  }
		  (pstatl+i)->Min_Cur  = MINRESET;
		  (pstatl+i)->Max_Cur  = MAXRESET;
		  (pstatl+i)->Mtie     = 0.0;
	 }

} 

/*****************************************************************************
This function processes the stream of phase data for each of the MAXCHAN
pairwise measurments channels and updates the Maximum Time Interval Error
statistics. The routine use an octave base non-overlapping approach to
calculate MTIE without sorting.
For the inputs channels that are reference to the Local Oscillator the algorithm
compensates the local oscillator with the frequency correction state to produce
MRTIE stats of the input compared to the output.
The maximum MTIE for a given interval will saturate unless reset. The reset is
performed either every MTIERESET minutes (24 Hours) or through a user request
via common memory.   smi.Mtie_Reset {0 - MAXCHAN } to select which channel
to reset. Note -1 (None) means perform no reset.
******************************************************************************/
void MTIEupdate(double Xin, int Which_Chan)
{
	 long int Xinl;
	 register unsigned int Mcount;
	 struct Per_Tau_Stats *pstats,*pstatm,*pstatl;
	 unsigned char i,j,Reset;
	 double temp;
	 pstats= pshort[Which_Chan];
	 pstatm= pmed[Which_Chan];
	 pstatl= plong[Which_Chan];
	 Mtie_Next=TRUE;
	 Mtie_Count[Which_Chan]++; /* routine is octave based so rollover of counter
											is synchronous with all TAU categories*/
	 Mcount=Mtie_Count[Which_Chan];
	/* set up per TAU reset requests as needed*/
	if(smi.Mtie_Reset==Which_Chan)
	{
		 smi.Mtie_Reset=NONE;
		 stat_normalize(Which_Chan);
		 Xin=0.0;
		 for(i=0;i<TAUCAT;i++)
		 {
			 (pstats+i)->Mtie_Reset=TRUE;
			 (pstatm+i)->Mtie_Reset=TRUE;
			 (pstatl+i)->Mtie_Reset=TRUE;
		 }
	}
	else if (smi.Mtie_Reset == ALL)
	{
		 smi.Mtie_Reset=NONE;
		 for(j=0;j<MAXCHAN;j++)
		 {
			stat_normalize(j);
			Xin=0.0;
			for(i=0;i<TAUCAT;i++)
			{
			 (pshort[j]+i)->Mtie_Reset=TRUE;
			 (pmed[j]+i)  ->Mtie_Reset=TRUE;
			 (plong[j]+i) ->Mtie_Reset=TRUE;
			}
		 }
	}
	/* convert to long int */
	  Xinl=Xin*BUFSCALE;
	/* need to generate first TAUZERO min max pair */
	if((pstats+1)->Min_Cur > Xinl) (pstats+1)->Min_Cur=Xinl;
	if((pstats+1)->Max_Cur < Xinl) (pstats+1)->Max_Cur=Xinl;
	if(Mcount%2==0)
	{
	  Reset= pstats->Mtie_Reset;
	  Mtie_Calc(Reset,pstats+1,pstats+2);
	  if(Reset)
	  {
		 pstats->Mtie_Reset=FALSE;
		 (pstats+1)->Mtie_Reset=FALSE;
	  }
	}
	 /* use first MTIE bin as special case to report current accumulate
		 time error*/
	temp= (float)(Xinl)/BUFSCALE;
	(pstats)->Mtie = 0.8*(pstats)->Mtie + 0.2*temp; /*use smoothing filter*/
	/*(pstats)->Mtie = temp; /*use smoothing filter*/
	(pstats)->Min_Cur = (pstats+1)->Min_Cur;
	(pstats)->Max_Cur = (pstats+1)->Max_Cur;
	if(Mcount%4==0)
	{
	  Reset= (pstats+2)->Mtie_Reset;
	  Mtie_Calc(Reset,pstats+2,pstats+3);
	  if(Reset) (pstats+2)->Mtie_Reset=FALSE;
	}
	if(Mcount%8==0)
	{
	  Reset= (pstats+3)->Mtie_Reset;
	  Mtie_Calc(Reset,pstats+3,pstats+4);
	  if(Reset) (pstats+3)->Mtie_Reset=FALSE;
	}
	if(Mcount%16==0)
	{
	  Reset= (pstats+4)->Mtie_Reset;
	  Mtie_Calc(Reset,pstats+4,pstatm);
	  if(Reset) (pstats+4)->Mtie_Reset=FALSE;
	}
	if(Mcount%32==0)
	{
	  Reset= (pstatm)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatm,(pstatm+1));
	  if(Reset) (pstatm)->Mtie_Reset=FALSE;
	}
	if(Mcount%64==0)
	{
	  Reset= (pstatm+1)->Mtie_Reset;
	  Mtie_Calc(Reset,(pstatm+1),pstatm+2);
	  if(Reset) (pstatm+1)->Mtie_Reset=FALSE;
	}
	if(Mcount%128==0)
	{
	  Reset= (pstatm+2)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatm+2,pstatm+3);
	  if(Reset) (pstatm+2)->Mtie_Reset=FALSE;
	}
	if(Mcount%256==0)
	{
	  Reset= (pstatm+3)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatm+3,pstatm+4);
	  if(Reset) (pstatm+3)->Mtie_Reset=FALSE;
	}
	if(Mcount%512==0)
	{
	  Reset= (pstatm+4)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatm+4,pstatl);
	  if(Reset) (pstatm+4)->Mtie_Reset=FALSE;
	}
	if(Mcount%1024==0)
	{
	  Reset= (pstatl)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatl,(pstatl+1));
	  if(Reset) (pstatl)->Mtie_Reset=FALSE;
	}
	if(Mcount%2048==0)
	{
	  Reset= (pstatl+1)->Mtie_Reset;
	  Mtie_Calc(Reset,(pstatl+1),(pstatl+2));
	  if(Reset) (pstatl+1)->Mtie_Reset=FALSE;
	}
	if(Mcount%4096==0)
	{
	  Reset= (pstatl+2)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatl+2,pstatl+3);
	  if(Reset) (pstatl+2)->Mtie_Reset=FALSE;
	}
	if(Mcount%8192==0)
	{
	  Reset= (pstatl+3)->Mtie_Reset;
	  Mtie_Calc(Reset,pstatl+3,pstatl+4);
	  if(Reset) (pstatl+3)->Mtie_Reset=FALSE;
	}
	if(Mcount%16384==0)
	{
	  Reset= (pstatl+4)->Mtie_Reset;
	  Mtie_Next=FALSE;
	  Mtie_Calc(Reset,pstatl+4,pstatl+4);
	  if(Reset) (pstatl+4)->Mtie_Reset=FALSE;
	}
}
void Mtie_Calc(unsigned char Reset,
				  register struct Per_Tau_Stats *p,register struct Per_Tau_Stats *q)
{
	  float temp;
	  temp = (float)((p->Max_Cur - p->Min_Cur))/BUFSCALE;
	  /* Check if new peak MTIE */
	  if(temp > p->Mtie)
	  {
		 p->Mtie = temp;
	  }
	  /* Update current Min Max for next category*/
	  if(Mtie_Next)
	  {
		 if((q)->Min_Cur>p->Min_Cur)(q)->Min_Cur=p->Min_Cur ;
		 if((q)->Max_Cur<p->Max_Cur)(q)->Max_Cur=p->Max_Cur ;
	  }
	  /* Reset MTIE if requested or time */
	  if(Reset)
	  {
		  p->Mtie=temp;
	  }
	  /* Reset Current Min Max*/
	  p->Max_Cur=MAXRESET;
	  p->Min_Cur=MINRESET;
}  
#endif
/*********************************************************************
 This function updated the outgoing mailboxes to report
 the  PSM report information
*********************************************************************/
void PSM_Report()
{
	float *fstats;
	struct MSG_PSM_Report *psmp;
	psmp=&(MSG_PSM);
	fstats=(float *)psmp;
	switch(PSMR_Count){
	 case SA_LO :
//		tstat_report_fast(fstats,SA_LO,TRUE);
		psmp->Channel=SA_LO;
		psmp->Ready=TRUE;
		break;
	 case SB_LO :
//		tstat_report_fast(fstats,SB_LO,TRUE);
		psmp->Channel=SB_LO;
		psmp->Ready=TRUE;
		break;
	 case RA_LO :
//		tstat_report_fast(fstats,RA_LO,TRUE);
		psmp->Channel=RA_LO;
		psmp->Ready=TRUE;
		break;
	 case RB_LO :
//		tstat_report_fast(fstats,RB_LO,TRUE);
		psmp->Channel=RB_LO;
		psmp->Ready=TRUE;
		break;
	 case GPS_LO :
//		tstat_report_fast(fstats,GPS_LO,TRUE);
		psmp->Channel=GPS_LO;
		psmp->Ready=TRUE;
		break;
	 default : break;
	}
	PSMR_Count++;
	if(PSMR_Count>=CLOOPS)PSMR_Count=0;
}

