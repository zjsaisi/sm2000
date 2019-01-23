/***************************************************************************
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:06PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/newmsgs.h_v  $
$Modtime:   06 Feb 1999 15:44:26  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                NEWMSGS.H

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

$Log: bt3_messages.h  $
Revision 1.1 2007/12/06 11:41:06PST Zheng Miao (zmiao) 
Initial revision
Member added to project e:/MKS_Projects/TimeProvider_Polestar/IOC_Hitachi/IOCMain/Bestime/project.pj
Revision 1.1 2005/09/01 14:55:11PDT zmiao 
Initial revision
Member added to project e:/MKS_Projects/TimeCreator1000/src/IocSrc/IOCMain/Bestime/project.pj
Revision 1.1 2004/06/24 10:23:02PDT gzampet 
Initial revision
 * 
 *    Rev 2.0   15 Dec 1999 14:35:12   l.cheung
 * Beginning Ver 1.03.01, all files start from Rev 2.0.
 * 
 *    Rev 1.0   06 Feb 1999 15:44:26   S.Scott
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
 *	Filename:  newmsgs.c
 *
 * $Header: bt3_messages.h 1.1 2007/12/06 11:41:06PST Zheng Miao (zmiao) Exp  $
 *
 ***********************************************************************
#define INTEL_SRC
 */
#ifndef TYPEDEFS
typedef unsigned char 	UCHAR;
typedef unsigned short	USHORT;
typedef unsigned long	ULONG;
typedef unsigned int 	UINT;
typedef enum t_f BOOL;
#define TYPEDEFS
#endif


#ifdef  INTEL_SRC
/********************************************************************
   (Formerly the STATS.H file)
*/

#define  RESULT_BAD   1
#define  RESULT_OK    0

#define  MAX_PH_CHANS 4

#define  MAX_CHANS   5
#define  GPS_TCXO    0
#define  EA_TCXO     1
#define  EA_GPS      2
#define  EB_TCXO     3
#define  EB_GPS      4
#define  EA_TCXO_RAW 5
#define  EB_TCXO_RAW 6

#define  MAX_TYPES   3                  

#define  P_MDEV      0
#define  P_MTIE      1
#define  P_AFREQ     2
#define  P_TDEV      3

#define  NO_STATS    15


struct TYPE_DATAQ
{
	float phase;
   float t_data[ MAX_TYPES ][ NO_STATS ];
};

struct MTQ
{
	short chan;    /*  0..MAX_CHANS-1   */

	struct TYPE_DATAQ  data;
};

//  This structure holds the data returned & processed, ie. Sqrt() 
//    completed, converting MVAR to MDEV & TDEV.
//
struct TYPE_DATAP
{
	float phase;
   float pdata[ MAX_TYPES+1 ][ NO_STATS ];
};
#endif

union   SELFTEST_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int pwrup_eprom  :1;  /*  lsb's  */
      unsigned  int na1          :1;
      unsigned  int pwrup_ram    :1;
      unsigned  int na2          :5;
      unsigned  int bkgrnd_eprom :1;
      unsigned  int na3          :2;
      unsigned  int bkgrnd_ram   :5;
#else
      unsigned  int bkgrnd_ram   :5;
      unsigned  int na3          :2;
      unsigned  int bkgrnd_eprom :1;
      unsigned  int na2          :5;
      unsigned  int pwrup_ram    :1;
      unsigned  int na1          :1;
      unsigned  int pwrup_eprom  :1;  /*  lsb's  */
#endif

   } b;

   unsigned short whole;
};


union   SP_RO_REG_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int freq_evt     :1;  /*  lsb's	 */
      unsigned  int drift_evt    :1;
      unsigned  int freq_alm     :1;
      unsigned  int drift_alm    :1;
      unsigned  int jitter_alm   :1;
      unsigned  int present      :1;
      unsigned  int phase_evt    :1;
      unsigned  int phase_alm    :1;
      unsigned  int unused       :8;
#else
      unsigned  int unused       :8;
      unsigned  int phase_alm    :1;
      unsigned  int phase_evt    :1;
      unsigned  int present      :1;
      unsigned  int jitter_alm   :1;
      unsigned  int drift_alm    :1;
      unsigned  int freq_alm     :1;
      unsigned  int drift_evt    :1;
      unsigned  int freq_evt     :1;  /*  lsb's	 */
#endif
   } b;

   unsigned short whole;
};

union   GPS_REG_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int freq_evt     :1;  /*  lsb's  */
      unsigned  int drift_evt    :1;
      unsigned  int freq_alm     :1;
      unsigned  int drift_alm    :1;
      unsigned  int timg_accur   :1;
      unsigned  int clk_bias_evt :1;
      unsigned  int unused       :10;
#else
      unsigned  int unused       :10;
      unsigned  int clk_bias_evt :1;
      unsigned  int timg_accur   :1;
      unsigned  int drift_alm    :1;
      unsigned  int freq_alm     :1;
      unsigned  int drift_evt    :1;
      unsigned  int freq_evt     :1;  /*  lsb's  */
#endif
   } b;

   unsigned short whole;
};



union   PL_REG_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int step_evt     :1;  /*  lsb's    */
      unsigned  int range_evt    :1;
      unsigned  int step_alm     :1;
      unsigned  int range_alm    :1;
      unsigned  int jitter_alm   :1;
      unsigned  int present      :1;
      unsigned  int unused       :6;
      unsigned  int Rb_lock_alm  :1;   /*  SRS Rubidium alarms  */
      unsigned  int Rb_temp_alm  :1;
      unsigned  int Rb_rf_alm    :1;
      unsigned  int Rb_pwr_alm   :1;
#else
      unsigned  int Rb_pwr_alm   :1;   /*  SRS Rubidium alarms  */
      unsigned  int Rb_rf_alm    :1;
      unsigned  int Rb_temp_alm  :1;
      unsigned  int Rb_lock_alm  :1;
      unsigned  int unused       :6;
      unsigned  int present      :1;
      unsigned  int jitter_alm   :1;
      unsigned  int range_alm    :1;
      unsigned  int step_alm     :1;
      unsigned  int range_evt    :1;
      unsigned  int step_evt     :1;  /*  lsb's    */
#endif
   } b;

   unsigned short whole;
};

union   TP_REG_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int step_evt     :1;  /*  lsb's    */
      unsigned  int slew_evt     :1;
      unsigned  int step_alm     :1;
      unsigned  int slew_alm     :1;
      unsigned  int jitter_alm   :1;
      unsigned  int present      :1;
      unsigned  int unused       :10;
#else
      unsigned  int unused       :10;
      unsigned  int present      :1;
      unsigned  int jitter_alm   :1;
      unsigned  int slew_alm     :1;
      unsigned  int step_alm     :1;
      unsigned  int slew_evt     :1;
      unsigned  int step_evt     :1;  /*  lsb's    */
#endif
   } b;

   unsigned short whole;
};


union   XR_REG_BITS
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int clk_bias_flag:1;  /*  lsb's    */
      unsigned  int res0         :3;
      unsigned  int antenna_flt  :1;
      unsigned  int flashmem_flt :1;
      unsigned  int res1         :10;
#else
      unsigned  int res1         :10;
      unsigned  int flashmem_flt :1;
      unsigned  int antenna_flt  :1;
      unsigned  int res0         :3;
      unsigned  int clk_bias_flag:1;  /*  lsb's    */
#endif
   } b;

   unsigned short whole;
};


struct  CSI_STRUCT
{
	ULONG		gps_time;
	double	clk_bias;
	USHORT	time_acc;	/*  timing accuracy		*/
	USHORT	XR_Reg;
};

/*  Ensemble Control Word bits
*/
union   ECW_bits
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int mode         :8;  /*  lsb's     */
      unsigned  int disable_GPS  :1;
      unsigned  int disable_SPA  :1;
      unsigned  int disable_SPB  :1;
      unsigned  int disable_ROA  :1;
      unsigned  int disable_ROB  :1;
      unsigned  int na           :3;
#else
      unsigned  int na           :3;
      unsigned  int disable_ROB  :1;
      unsigned  int disable_ROA  :1;
      unsigned  int disable_SPB  :1;
      unsigned  int disable_SPA  :1;
      unsigned  int disable_GPS  :1;
      unsigned  int mode         :8;  /*  lsb's     */
#endif
   } b;

   USHORT   whole;
};

/*  Ensemble Control Word bits
*/
union   PROV_bits
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int disable_GPS  :1;  /*  lsb's     */
      unsigned  int disable_SPA  :1;
      unsigned  int disable_SPB  :1;
      unsigned  int disable_ROA  :1;
      unsigned  int disable_ROB  :1;
      unsigned  int na           :11;
#else
      unsigned  int na           :11;
      unsigned  int disable_ROB  :1;
      unsigned  int disable_ROA  :1;
      unsigned  int disable_SPB  :1;
      unsigned  int disable_SPA  :1;
      unsigned  int disable_GPS  :1;  /*  lsb's     */
#endif
   } b;

   USHORT   whole;
};


#define  T1_D4          1
#define  T1_ESF         2
#define  T1_TEST        3

/*  use the following for configuring the T1 Error output(m_T1_ERR_FORMAT)
*/
#define  T1_AIS         1
#define  T1_SQUELCH     2
#define  T1_SSM         3


union   T1_CONFIG
{
   struct
   {
#ifdef INTEL_SRC
      unsigned  int OnError      :3;  /*  lsb's 1=AIS, 2=Squelch, 3=SSM   */
      unsigned  int Format       :3;  /*        1=D4,  2=ESF, 3=Test		  */
      unsigned  int na           :10;
#else
      unsigned  int na           :10;
      unsigned  int Format       :3;  /*        1=D4,  2=ESF, 3=Test      */
      unsigned  int OnError      :3;  /*  lsb's 1=AIS, 2=Squelch, 3=SSM	  */
#endif
   } b;

   short  whole;
};
