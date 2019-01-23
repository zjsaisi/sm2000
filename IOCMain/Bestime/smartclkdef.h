/***************************************************************************
$Header: smartclkdef.h 1.1 2007/12/06 11:41:07PST Zheng Miao (zmiao) Exp  $
$Author: Zheng Miao (zmiao) $
$Date: 2007/12/06 11:41:07PST $
$Logfile:   Z:/pvcs/ts3000/ts3000/bt3_def.h_v  $
$Modtime:   16 Feb 1999 15:49:22  $
$Revision: 1.1 $


                            CLIPPER SOFTWARE
                                 
                                SMARTCLK_DEF.H

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



***************************************************************************/

/******************** INCLUDE FILES ************************/

/******************** LOCAL DEFINES ************************/

/************ PROTOTYPES EXTERNAL FUNCTIONS ****************/

/************ PROTOTYPES INTERNAL FUNCTIONS ****************/

/******************** EXTERNAL DATA ************************/

/******************** INTERNAL DATA ************************/

/************************ TABLE ****************************/
     
/************************ CODE *****************************/



 
#define SMART_OFF 0
#define SMART_DEGRADE 1
#define SMART_ON 2
#define SMART_WIDE 4.0e-6
#define SMART_WIDE_FM SMART_WIDE/(double)(3600.0*4.0)
#define SMART_NARROW 2.5e-6
#define SMART_NARROW_FM SMART_NARROW/(double)(3600.0*4.0)
