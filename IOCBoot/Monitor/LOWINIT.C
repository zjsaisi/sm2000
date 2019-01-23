#pragma language=extended

#pragma codeseg(RCODE)

#pragma function=C_task

#if (__TID__ & 0x200)
unsigned char far_func low_level_init (void)
#else
unsigned char near_func low_level_init (void)
#endif

  {
	#define BWCR  (*(volatile char *)(0xffffffec)) /* Bus width Control Register */	
	#define P8DDR (*(volatile char *)(0xffffffcd))
    /*==================================*/
    /*  Check your watch dog if needed	*/
    /*==================================*/
    
    /*==================================*/
    /*  Init I/O which must be enabled	*/
    /*  before segment initializations	*/
    /*  (as address lines).		*/
    /*==================================*/
    
    /*==================================*/
    /* Choose if segment initialization	*/
    /* should be done or not.		*/
    /* Return: 0 to omit seg_init	*/
    /*	       1 to run seg_init	*/
    /*==================================*/

	BWCR &= 0xfb;  /* set area 2 to 16-bit access */
	P8DDR = 0xf4;
	
	/* must explicitly set ports 1,2 and 5 to address lines */
	#define P1DDR (*(volatile unsigned char *)(0xffffffc0))
	#define P2DDR (*(volatile unsigned char *)(0xffffffc1))
	#define P5DDR (*(volatile unsigned char *)(0xffffffc8))
	
	P1DDR = 0xff;
	P2DDR = 0xff;
	P5DDR = 0xff;	

    return (1);
  }
