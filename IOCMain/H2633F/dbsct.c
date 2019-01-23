/***********************************************************************/
/*                                                                     */
/*  FILE        :dbsct.c                                               */
/*  DATE        :Wed, Sep 18, 2002                                     */
/*  DESCRIPTION :Setting of B,R Section                                */
/*  CPU TYPE    :H8S/2633F                                             */
/*                                                                     */
/*  This file is generated by Hitachi Project Generator (Ver.1.1).     */
/*                                                                     */
/***********************************************************************/
                  


#pragma section $DSEC
static const struct {
	char *rom_s;       /* Start address of the initialized data section in ROM */
	char *rom_e;       /* End address of the initialized data section in ROM   */
	char *ram_s;       /* Start address of the initialized data section in RAM */
}DTBL[]= {
	{__sectop("D"), __secend("D"), __sectop("R")},
	{__sectop("DHEAP"), __secend("DHEAP"), __sectop("RHEAP")},
//	{__sectop("$ABS8D"), __secend("$ABS8D"), __sectop("$ABS8R")},
//	{__sectop("$ABS16D") , __secend("$ABS16D") , __sectop("$ABS16R") }
};

#pragma section $BSEC
static const struct {
	char *b_s;         /* Start address of non-initialized data section */
	char *b_e;         /* End address of non-initialized data section */
}BTBL[]= {
	{__sectop("B"), __secend("B")},
//	{__sectop("$ABS8B"), __secend("$ABS8B")},
//	{__sectop("$ABS16B"), __secend("$ABS16B")}
};

#pragma section 
char * const g_pHeapStart = __sectop("BHEAP");
char * const g_pHeapEnd = __secend("BHEAP");