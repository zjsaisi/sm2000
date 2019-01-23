/*                   
 * Filename: DataType.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * Simple Data type definition for the project
 * RCS: $Header: DataType.h 1.3 2011/06/10 13:24:07PDT Jining Yang (jyang) Exp  $
 * $Revision: 1.3 $
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


#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


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
