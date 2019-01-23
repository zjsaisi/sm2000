/*                   
 * Filename: DTC_Vector.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: DTC_Vector.cpp 1.1 2007/12/06 11:38:51PST Zheng Miao (zmiao) Exp  $
 * $Revision: 1.1 $
 */             

#include "CodeOpt.h"
#include "DataType.h"

#define DTC_START_ADDRESS 0xEBC0

#pragma section DTC
unsigned short const s_DTCVector[128] = {
#include "DTC_Vector.h"
};
#pragma section

unsigned short const s_DTCVectorMirror[128] = {
#include "DTC_Vector.h"
};

// 1: OK
// -1: vector inconsistent
EXTERN int CompDTCVector(void) {
	unsigned short const *ptr1, *ptr2;
	ptr1 = s_DTCVector;
	ptr2 = s_DTCVectorMirror;
	for (int i = 0; i < 128; i++) {
		if (*ptr1 != *ptr2) return -1;
		ptr1++;
		ptr2++;
	}
	return 1;
}
