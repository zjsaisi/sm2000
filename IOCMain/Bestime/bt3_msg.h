/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:06PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_msg.h_v  $
$Modtime:   06 Feb 1999 12:58:10  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                BT3_MSG.H

                             COPYRIGHT 1999
                            TELECOM SOLUTIONS,
                      a division of SYMMETRICOM, Inc.
                         ALL RIGHTS RESERVED

     This program contains proprietary information and is confidential.
     Its contents may not be disclosed or reproduced in whole or
     in part without the written authorization of Telecom Solutions.


FILE AUTHOR: G. Zampetti

FILE DESCRIPTION: This header file was created to support inter task communication
                  messaging.

MODIFICATION HISTORY:

$Log: bt3_msg.h  $
Revision 1.1 2007/12/06 11:41:06PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.1 2003/04/15 12:46:22PDT gzampet 
Initial revision
Revision 1.1  2001/12/11 19:28:53Z  kho
Initial revision
 * 
 *    Rev 1.0   06 Feb 1999 15:41:50   S.Scott
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
*     Version 2.0 Baseline BT3 clock engine
*     This header file was created to support inter task communication
*     messaging.
*
*****************************************************************************/
/*****************************************************************************
  Message: MSG_Span_FLL

  Description: Provides the span frequency estimation data for both A and B spans
  to the FLL 1 second update task

  Date: Created 3-6-98
*****************************************************************************/
struct MSG_Span_FLL
{
	 /*************************************************************************
		Note Yspavg data is smoothed frequency estimates (open loop) of the
		input with respect to the local oscillator (sign convention + means
		input is running faster than local oscillator). The smoothing time is
		controlled by SFREQTAU (15 seconds default)
	 *************************************************************************/
	 double YspavgA; /*SPAN A frequency estimate */
	 double YspavgB; /*SPAN B frequency estimate */
	 /*************************************************************************
		Event Register Data (XXX_Reg_Events) is event data latched over one
		second indicating a transient on the input.
		The Register Assignment for frequency estimates (Span and RO) is:
		bit 0 (lsb) Freq  Event
		bit 1       Drift Event
		bit 6       Phase Event
	 *************************************************************************/
	 unsigned short SPA_Reg_Events; /*current event level register*/
	 unsigned short SPB_Reg_Events; /*current event level register*/
	 unsigned short Ready;
} ;
/*****************************************************************************
  Message: MSG_RO_FLL

  Description: Provides the RO frequency estimation data for both A and B spans
  to the FLL 1 second update task

  Date: Created 3-6-98
*****************************************************************************/
struct MSG_RO_FLL
{
	 double YroavgA; /*RO A frequency estimate */
	 double YroavgB; /*RO B frequency estimate */
	 unsigned short ROA_Reg_Events; /*current event level register*/
	 unsigned short ROB_Reg_Events; /*current event level register*/
	 unsigned short Ready;
} ;
/*****************************************************************************
  Message: MSG_FLL_EST

  Description: Provides the SPAN and RO 250 ms task with the following
  1) Current  Yprimout so that drift can be tested. Yprimout is the correction
	  frequency of the primary portion of the dual input FLL  indicating
	  the best estimate of average input frequency over the loop time constant.
	  If the current smoothed Yspavg or Yroavg drifts from this average a drift
	  transient is detected. This is the primary method for detecting a pointer
	  transient on incoming span lines.

  Date: Created 3-6-98
*****************************************************************************/
 struct MSG_FLL_EST {
		 double YprimoutSPA;
		 double YprimoutSPB;
		 double YprimoutROA;
		 double YprimoutROB;
		 unsigned short Ready;
} ;
/*****************************************************************************
  Message: MSG_NCOUP_EST

  Description: Provides the SPAN and RO 250 ms task with
  1) Current frequency corrections so that measurements
	  can be converted to "open loop"
  The actual FPGA measurement data is comparing the input SPAN or RO with
  respect to the OUTPUT SYNTHESIZED CLOCK. This is a closed loop measurement.
  The open loop measurement is what the input would do against the free running
  local oscillator (with temperature correction included). To "back out" the
  synthesizer correction both the current frequency correction state (this
  message) and the phase correction state (next message) must be fed back to
  the estimation task. Since we use digital synthesis and not varactor pulled
  oscillators we can "back out" the correction essentially perfectly. THIS IS
  A KEY POINT IN RUNNING MULTIPLE ENSEMBLED LOOPS.

  Date: Created 3-9-98
*****************************************************************************/
struct MSG_NCOUP_EST {
		 double freq_state_cor;  /*current ensembled best estimate frequency
											correction required to put local synthesizer
											on perfect frequency*/
		 unsigned short Ready;
} ;
/*****************************************************************************
  Message: MSG_PCOMP_EST

  Description: Provides the SPAN and RO 250 ms task with
  1) Current phase corrections so that measurements
	  can be converted to "open loop" (see previous message discussion)

  Date: Created 3-10-98
*****************************************************************************/
struct MSG_PCOMP_EST {
		 double phase_state_cor; /*current frequency (phase slew) correction
		                            required to put local output on time*/
		 unsigned short Ready;
} ;
/*****************************************************************************
  Message: MSG_EST_ATC

  Description: Provides the RO frequency estimation data and
  temperature data to the ATC 1 second update task .

  Date: Created 3-10-98
*****************************************************************************/
struct MSG_EST_ATC
{
	 double YroavgA; /*RO A frequency estimate */
	 double YroavgB; /*RO B frequency estimate */
	 double Temp_Est; /*current temperature estimate (Oven Current in 4*ma */
	 double Ytpavg;   /*average delta oven current (temperature) over slew interval*/
	 unsigned short ROA_Reg_Events; /*current event level register*/
	 unsigned short ROB_Reg_Events; /*current event level register*/
	 unsigned short TP_Reg_Events;  /*current event level register*/
	 unsigned short TP_Slew_Update;   /*slew interval update completed*/
	 unsigned short Ready;
} ;
/*****************************************************************************
	Message: MSG_EST_SRPT
	Description: Provides the status report register update task with current
	latched event data from the estimation task

	Date: Created 3-10-98
*****************************************************************************/
struct MSG_EST_SRPT
{

	 unsigned short SPA_Reg_Events; /*current event level register*/
	 unsigned short SPB_Reg_Events; /*current event level register*/
	 unsigned short ROA_Reg_Events; /*current event level register*/
	 unsigned short ROB_Reg_Events; /*current event level register*/
	 unsigned short TP_Reg_Events;  /*current event level register*/
	 unsigned short PL_Reg_Events;  /*current event level register*/
	 unsigned short AT_Reg_Events;  /*current event level register*/
	 unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_EST_ASM
	Description: Provides the adaptive state machine task with
	current leaky bucket data from the 250ms estimation task
	Note all buckets are nominally limited in a range of 0 to 1000.
	However buckets can go a maximum of one upcount step or one downcount step
	over these limits. The nominal threshold (low water mark) to assert an
	alarm is LBUCKETTHRES (default 750 ver2.0) with a hysterises of
	LBUCKETHYS (default 50 ver2.0).  
	Date: Created 3-11-98
*****************************************************************************/
struct MSG_EST_ASM
{
	 short SPAphase_bkt, SPAfreq_bkt, SPAdrift_bkt;
	 short SPBphase_bkt, SPBfreq_bkt, SPBdrift_bkt;
	 short ROAphase_bkt, ROAfreq_bkt, ROAdrift_bkt;
	 short ROBphase_bkt, ROBfreq_bkt, ROBdrift_bkt;
	 short TPstep_bkt, TPslew_bkt;
	 short PLstep_bkt, PLrange_bkt;
	 short ALstep_bkt, ALrange_bkt;
	 unsigned short Ready;
};

/*****************************************************************************
	Message: MSG_FLL_HOLDOVER
	Description: Provides the holdover over update routine with the frequency
	state data required to operate the drift estimation process.


	Date: Created 3-13-98
*****************************************************************************/
struct MSG_FLL_HOLDOVER
{
	 double Lloop_Freq_Err_Sum, Rloop_Freq_Err_Sum;
	 double SPA_GPS_Prop_State,SPB_GPS_Prop_State;
	 double ROA_GPS_Prop_State,ROB_GPS_Prop_State;
	 unsigned short Lloop_Freq_Sum_Cnt, Rloop_Freq_Sum_Cnt;
	 unsigned short SPA_Recal,SPB_Recal,ROA_Recal,ROB_Recal;
	 unsigned short SPA_Mreg,SPB_Mreg,ROA_Mreg,ROB_Mreg;
	 unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_HOLDOVER_NCOUP
	Description: Provide the NCOUP task the 24 hour predicition error variance
	estimates to use in the weight calculation under GPS Holdover conditions


	Date: Created 4-29-98
*****************************************************************************/
struct MSG_HOLDOVER_NCOUP
{
	 double Lloop_Pred24_Var, Rloop_Pred24_Var;
	 double SPA_Pred24_Var, SPB_Pred24_Var;
	 double ROA_Pred24_Var, ROB_Pred24_Var;
	 unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_HOLDOVER_FLL
	Description: Provide the FLL task with the current best estimates of input
	frequency data to use when GPS is not available


	Date: Created 3-13-98
*****************************************************************************/
struct MSG_HOLDOVER_FLL
{
	 double Lloop_Osc_Freq_Pred, Rloop_Osc_Freq_Pred;
	 double SPA_Cal_Freq_Pred, SPB_Cal_Freq_Pred;
	 double ROA_Cal_Freq_Pred, ROB_Cal_Freq_Pred;
    float  Tracking_Success;
	 unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_FLL_TCH
	Description: Provide Three Corner Hat Task with 1 minute average
	frequency data (with transients removed) required to estimated input
	stabilities

	Date: Created 3-13-98
*****************************************************************************/
struct MSG_FLL_TCH
{
	 double XGPS_1Min, XSPA_1Min, XSPB_1Min, XROA_1Min, XROB_1Min;
	 unsigned short XGPS_cnt, XSPA_cnt, XSPB_cnt, XROA_cnt, XROB_cnt;
	 unsigned short Pmode;
	 unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_SR_TCH
	Description: Provide Status Report Latched Register Data to the
	One Minute TCH function

	Date: Created 3-16-98
*****************************************************************************/
struct MSG_SR_TCH
{
	unsigned short SPA_Mreg;
	unsigned short SPB_Mreg;
	unsigned short GPS_Mreg;
	unsigned short ROA_Mreg;
	unsigned short ROB_Mreg;
	unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_TCH_NCOUP
	Description: Transfers the three corner hat decomposed estimated
	of noise for each of the four active resources to the ensembling
	routine in NCO_Update

	Date: Created 3-16-98
*****************************************************************************/
struct MSG_TCH_NCOUP
{
  double GPS_mvar;
  double	PA_mvar;
  double PB_mvar;
  double LO_mvar;
	unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_FLL_CAL
	Description: Transfer Local Loop States required for seamless
	restart 
	Date: Created 3-16-98
*****************************************************************************/
struct MSG_FLL_CAL
{
  double Lloop_NCO;
  double	Lloop_Prop_Tau;
  double Lloop_Int_State;
  unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_CAL_EST
	Description: Transfer factory frequency calibration to estimation module
   to support calculation of frequency events.
	Date: Created 3-23-98
*****************************************************************************/
struct MSG_CAL_EST
{
  double Factory_Freq_Cal;
  unsigned short Ready;
};
/*****************************************************************************
	Message: MSG_CAL_FLL
	Description: Transfer factory frequency calibration to FLL module
	to support reporting of states normalized to factory calibration.
	Date: Created 3-23-98
*****************************************************************************/
struct MSG_CAL_FLL
{
  double Factory_Freq_Cal;
  unsigned short Ready;
};


