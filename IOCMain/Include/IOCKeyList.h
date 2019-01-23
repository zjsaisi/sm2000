/*                   
 * Filename: IOCKeyList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: IOCKeyList.h 1.18 2011/04/22 10:46:19PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.18 $
 */             
#include "CodeOpt.h"

// Use 5XXX for debug/development version
#ifndef IOC_ISTATE_VERSION
#if IS_FORMAL_BUILD
#define IOC_ISTATE_VERSION 0x6
#else
#define IOC_ISTATE_VERSION 0x5025 //0x5026
#endif
#endif

// Istate number history
// 1: For first alpha release. Built on 3/15/2006
// 2: Get ready for RC1 of 1.0 release. Build on 7/22/2008 
// 3: Get ready for RC1 of 1.1 release. 
// 4: Get ready for RC1 of 1.2 release.
// 5: Not sure it RC1 is out. 1/19/2010. 
// 6: Get ready for RC3 of 1.3 release. 1/9/2011. 
#if (!defined(BASIC_ISTATE_ONLY)) || (BASIC_ISTATE_ONLY == 0)
#ifndef DTI_ISTATE
#define DTI_ISTATE 1
#endif
#include "IOCKey.h"
#undef	_IOCKEYLIST_H_8_16_20035
#define _IOCKEYLIST_H_8_16_20035
#endif

