/*
 * Filename: errorcode.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: errorcode.h 1.1 2007/12/06 11:39:11PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */
// This file defines error code for IMC-IOC communication
#ifndef _ERROR_CODE_04_08_2003
#define _ERROR_CODE_04_08_2003

// 1 is no error
#define OK_IOC_ISTATE_NO_CHANGE	100

#define ERROR_UNKNOWN_COMMAND	-1

// Istate error code
#define ERROR_UNKNOWN_IOC_ISTATE_ID -100
#define ERROR_INDEX1_OOR	-101
#define ERROR_INDEX2_OOR	-102
#define ERROR_VALUE_OOR		-103
#define ERROR_READ_ONLY_ISTATE -104
#define ERROR_ISTATE_PROCESS_BUSY -105

#define ERROR_IOC_ISTATE_INTERNAL01 -110 /*Internal error, unexpected item size from CIstateDesc */
#define ERROR_IOC_ISTATE_INTERNAL02 -111 /* Istate Item point to nothing */
#define ERROR_IOC_ISTATE_INTERNAL03 -112 /* Istate Item is not either 1 or 4 bytes long */

// Download error code
#define ERROR_DNLD_UNEXPECTED_FRAME_LENGTH -120
#define ERROR_DNLD_INVALID_OFFSET		-121 /* offset >= total, or not expected value */
#define ERROR_DNLD_WRITE_FLASH			-122 /* Write flash error */
#define ERROR_DNLD_INVALID_DLD			-123
#define ERROR_DNLD_TOO_BIG     			-124 /* Total size too big */ 
#define ERROR_DNLD_BUSY					-125 /* Download is going on */
#define ERROR_DNLD_INCONSISTENT_SIZE	-126 /* The total size field is different than the first packet */
#endif // _ERROR_CODE_04_08_2003
