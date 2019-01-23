/*                   
 * Filename: DataType.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Simple Data type definition for the project
 * RCS: $Header: DataType.h 1.1 2007/12/06 11:39:10PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */             

#ifndef _DataType_H_09_19
#define _DataType_H_09_19

#pragma pack 2

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#if !defined(uint16)           
#define uint16 unsigned short
#endif

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef  uint32
#define uint32 unsigned long
#endif

#ifndef int16
#define int16  int
#endif

#ifndef int8
#define int8   signed char
#endif

#ifndef int32
#define int32  long
#endif

#ifndef float64
#define float64 double
#endif

typedef struct
{
    uint32 valuehi;
    uint32 valuelo;
} int64;

typedef  void (*FUNC_PTR)(void);              

typedef struct {
	uint16 cmd;
	uint32 handle;
} SCmdGeneral;

typedef struct {
	uint16 cmd;
	uint32 handle;
	int16 error;
} SRespGeneral;

#endif /* _DataType_H_09_19 */
