/*                   
 * Filename: version.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Define version for the application
 * RCS: $Header: version.h 1.20 2011/08/29 09:59:42PDT Dongsheng Zhang (dzhang) Exp  $
 * $Revision: 1.20 $
 */             

#ifndef _VERSION_H_10_07
#define _VERSION_H_10_07

/////  Key parameters for firmware build
// Make 1 if this is formal build !!!!!!!!!!!!
#define IS_FORMAL_BUILD      1 

#ifndef VERSION_NUMBER
//   X.XX.XX in hex number
#define VERSION_NUMBER  0x10001L
#endif                         

#ifndef VERSION_STRING
#if		IS_FORMAL_BUILD
#define VERSION_STRING CAllProcess::g_versionString
#else
#define VERSION_STRING "02.00.002"                                             
#endif
#endif


#ifndef BUILD_NAME                   
#define BUILD_NAME "SM2000 STM"
#endif

#ifndef BUILD_DAY
#define BUILD_DAY    29
#endif

#ifndef BUILD_MONTH
#define BUILD_MONTH	 8
#endif

#ifndef BUILD_YEAR                           
#define BUILD_YEAR	2011
#endif

// For the first release it's 1. Read by PPC
#define COMPATIBILITY_ID	(1L)

// Compared agaist settable IOCHW_IOCFW_IOCHW
// Rev A hardware: not supported. Assumed 0
// Rev B hardware: 1                           
#define IOCHW_IOCFW (1L)  

// export this to istate for PPC to read
#define IMCFW_IOCFW (0x01L)
                      
////   End of Key parameters


#ifndef VERSION_DATE
#define VERSION_DATE (long)(BUILD_DAY*10000 + BUILD_MONTH * 100 + (BUILD_YEAR % 100))
#endif

extern char const * const g_sBuildDate;
extern char const * const g_sBuildTime;

#endif // _VERSION_H_10_07
