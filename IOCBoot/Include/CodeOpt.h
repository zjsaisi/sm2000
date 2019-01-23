/*                   
 * Filename: CodeOpt.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Compiling option for code generation
 * Simple Data type definition for the project
 * RCS: $Header: CodeOpt.h 1.1 2007/12/06 11:39:10PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */             
#ifndef _CODE_OPT_H_12_03_2007_BOOT
#define _CODE_OPT_H_12_03_2007_BOOT


#define TO_BE_COMPLETE 1

// Check RAM with starting up
#ifndef CHECK_RAM
#define CHECK_RAM 	1      
#endif

#define RAM_2M_BYTE		1
//#define RAM_1M_BYTE	1
                            
#if _DEBUG
#define ENABLE_SCI0 1
#endif

#ifndef ENABLE_SCI0
#define ENABLE_SCI0 1
#endif

#ifndef DEFAULT_BAUD0
#define DEFAULT_BAUD0	9600
#endif

#ifndef ENABLE_SCI1
#define ENABLE_SCI1 0
#endif

#ifndef DEFAULT_BAUD1
#define DEFAULT_BAUD1	9600
#endif

#ifndef ENABLE_SCI3   // TO IMC
#define ENABLE_SCI3 1 
#endif

// For polestart. This comm goes to PPC8313
#ifndef DEFAULT_BAUD3
#define DEFAULT_BAUD3	38400 // 115200
#endif

#ifndef ENABLE_SCI4   // To Twin IOC
#define ENABLE_SCI4	1
#endif

#ifndef DEFAULT_BAUD4
#define DEFAULT_BAUD4	115200 // 57600
#endif

#endif /* _CODE_OPT_H_12_03_2007_BOOT */
