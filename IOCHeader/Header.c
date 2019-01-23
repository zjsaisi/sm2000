/* This is a file to generate header info per function spec. 
 */ 
/*
 * 
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: Header.c 1.1 2007/12/06 11:40:33PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 *
 */
 
#include "version.h"
#include "constdef.h"           

#pragma section HeadSec
char const projectName[HEADER_PRODUCT_NAME_LENGTH] = "RTE090-50321-01";
char const firmwareVersion[HEADER_FIRMWARE_VERSION_LENGTH] 
= {
	((VERSION_NUMBER >> 16) & 0xff) / 100 + '0',
	(((VERSION_NUMBER >> 16) & 0xff) / 10) % 10 + '0', 
	(((VERSION_NUMBER >> 16) & 0xff) ) % 10 + '0',	
	'.',
	((VERSION_NUMBER >> 8) & 0xff) / 100 + '0',
	(((VERSION_NUMBER >> 8) & 0xff) / 10) % 10 + '0', 
	(((VERSION_NUMBER >> 8) & 0xff) ) % 10 + '0',
	'.',
	((VERSION_NUMBER) & 0xff) / 100 + '0',
	(((VERSION_NUMBER) & 0xff) / 10) % 10 + '0', 
	(((VERSION_NUMBER) & 0xff) ) % 10 + '0',
    0, 0, 0, 0
};
char const magic[HEADER_MAGIC_LENGTH]="M\rA\nG\r\nI\n\rC1234";
// List hardware rev supported in decimal number
// lower 4 bit is from hw rev
// high 4 bit is from back plane. 
// max length is 0xC8(192) limited by conv tool.
#pragma section Head1
char const hardwareSupported1[180] = " This firmware can work with " ;	 
#pragma section Head2
char const hardwareSupported2[20] = " 0 1 ";
#pragma section Head3
char const hardwareSupported3[20] = "  ";   
#pragma section Head4
char const hardwareSupported4[20] = " 240 241 ";
#pragma section Head5
char const hardwareSupported5[2] = "\0";  
#if 0
char const testSize1[31 * 1024] = "something";
char const testSize2[31 * 1024] = "something";
char const testSize3[31 * 1024] = "something";
char const testSize4[31 * 1024] = "something";
char const testSize5[31 * 1024] = "something";
char const testSize6[31 * 1024] = "something";
char const testSize7[31 * 1024] = "something";
char const testSize8[31 * 1024] = "something";
char const testSize9[31 * 1024] = "something";
char const testSize10[31 * 1024] = "something";
char const testSize11[31 * 1024] = "something";
char const testSize12[31 * 1024] = "something";
char const testSize13[31 * 1024] = "something";
char const testSize14[31 * 1024] = "something";
char const testSize15[31 * 1024] = "something";
char const testSize16[31 * 1024] = "something";
char const testSize17[31 * 1024] = "something";
char const testSize18[31 * 1024] = "something";
char const testSize19[31 * 1024] = "something";
char const testSize20[31 * 1024] = "something";
char const testSize21[31 * 1024] = "something";
char const testSize22[31 * 1024] = "something";
char const testSize23[31 * 1024] = "something";
char const testSize24[31 * 1024] = "something";
char const testSize25[31 * 1024] = "something";
char const testSize26[31 * 1024] = "something";
char const testSize27[31 * 1024] = "something";
char const testSize28[31 * 1024] = "something";
char const testSize29[31 * 1024] = "something";
char const testSize30[31 * 1024] = "something";
char const testSize31[31 * 1024] = "something";
char const testSize32[31 * 1024] = "something";
char const testSize33[31 * 1024] = "something";
char const testSize34[31 * 1024] = "something";
char const testSize35[31 * 1024] = "something";
char const testSize36[31 * 1024] = "something";
char const testSize37[31 * 1024] = "something";
char const testSize38[31 * 1024] = "something";
char const testSize39[31 * 1024] = "something";
#endif
#pragma section
