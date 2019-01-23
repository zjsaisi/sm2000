/*                   
 * Filename: resource.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: resource.h 1.1 2007/12/06 11:39:05PST Zheng Miao (zmiao) Exp  $
 */                 

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char GetResourceAssignment(void);
extern unsigned char GetResourceUsed(void);
extern unsigned char GetSemaAssignment(void);
extern unsigned char GetSemaUsed(void);
extern unsigned char GetMailBoxAssignment(void);
extern unsigned char GetMailBoxUsed(void);

#ifdef __cplusplus
}
#endif
