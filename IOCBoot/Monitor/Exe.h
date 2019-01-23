/// exe.h

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL -- No mods

#include "mon_type.h"
#define STEP_SINGLE 	0
#define STEP_OVER 	1
#define RUN		2

extern void Execute(void); 
ERCD Execute_return_code;

ERCD ExecuteUserCode(BYTE mode, WORD no_times);
void InitExecution(void);


