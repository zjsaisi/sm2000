/*                   
 * Filename: version.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Define version for the application
 * RCS: $Header: version.h 1.2 2008/03/12 13:28:45PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 */             

#ifndef _VERSION_H_10_07
#define _VERSION_H_10_07

#ifndef VERSION_NUMBER
//   X.XX.XX in hex number
#if RUN_IN_ROM
#define VERSION_NUMBER 0x10001L
#else 
#define VERSION_NUMBER 0x90201L
#endif 
#endif

#ifndef BUILD_NAME
#define BUILD_NAME "SM2000 STM"
#endif

#ifndef BUILD_DAY
#define BUILD_DAY	12
#endif

#ifndef BUILD_MONTH
#define BUILD_MONTH	3
#endif

#ifndef BUILD_YEAR
#define BUILD_YEAR	2008
#endif

#ifndef VERSION_DATE
#define VERSION_DATE (long)(BUILD_DAY*10000 + BUILD_MONTH * 100 + (BUILD_YEAR % 100))
#endif

extern char const * const g_sBuildDate;
extern char const * const g_sBuildTime;

#endif // _VERSION_H_10_07
