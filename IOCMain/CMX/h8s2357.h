#ifndef CMX_H8S2357_H
#define CMX_H8S2357_H

#define ICC_UNSIGN_CHAR		*(unsigned char *)
#define ICC_UNSIGN_SHORT	*(unsigned short *)
#define ICC_UNSIGN_LONG		*(unsigned long *)

//#define SYSCR		  (ICC_UNSIGN_CHAR	( 0xffff39 ))
#define ICRB    	  (ICC_UNSIGN_CHAR	( 0xfffec1 ))  /* interrupt control reg B */
#define MSTPCRH	  (ICC_UNSIGN_CHAR	( 0xffff3c ))  /* Module stop control  H  */
#define MSTPCRL     (ICC_UNSIGN_CHAR	( 0xffff3d ))  /* Module stop control  L  */
#define MSTPCR      (ICC_UNSIGN_SHORT  ( 0xffff3c ))  /* Module stop control */

#define IPRA		  (ICC_UNSIGN_CHAR	( 0xfffec4 ))
#define IPRB		  (ICC_UNSIGN_CHAR	( 0xfffec5 ))
#define IPRC		  (ICC_UNSIGN_CHAR	( 0xfffec6 ))
#define IPRD		  (ICC_UNSIGN_CHAR	( 0xfffec7 ))
#define IPRE		  (ICC_UNSIGN_CHAR	( 0xfffec8 ))
#define IPRF		  (ICC_UNSIGN_CHAR	( 0xfffec9 ))
#define IPRG		  (ICC_UNSIGN_CHAR	( 0xfffeca ))
#define IPRH		  (ICC_UNSIGN_CHAR	( 0xfffecb ))
#define IPRI		  (ICC_UNSIGN_CHAR	( 0xfffecc ))
#define IPRJ		  (ICC_UNSIGN_CHAR	( 0xfffecd ))
#define IPRK		  (ICC_UNSIGN_CHAR	( 0xfffece ))

#define SCI0_SMR    (ICC_UNSIGN_CHAR	 (0xffff78))  /* Serial mode        ch 0 */
#define SCI0_BRR    (ICC_UNSIGN_CHAR	 (0xffff79))  /* Bit rate           ch 0 */
#define SCI0_SCR    (ICC_UNSIGN_CHAR	 (0xffff7a))  /* Serial control     ch 0 */
#define SCI0_TDR    (ICC_UNSIGN_CHAR	 (0xffff7b))  /* Transmit data      ch 0 */
#define SCI0_SSR    (*(unsigned volatile char *)	 (0xffff7c))  /* Serial status      ch 0 */
#define SCI0_RDR    (ICC_UNSIGN_CHAR	 (0xffff7d))  /* Receive data       ch 0 */

#define SCI1_SMR    (ICC_UNSIGN_CHAR	 (0xffff80))  /* Serial mode        ch 1 */
#define SCI1_BRR    (ICC_UNSIGN_CHAR	 (0xffff81))  /* Bit rate           ch 1 */
#define SCI1_SCR    (ICC_UNSIGN_CHAR	 (0xffff82))  /* Serial control     ch 1 */
#define SCI1_TDR    (ICC_UNSIGN_CHAR	 (0xffff83))  /* Transmit data      ch 1 */
#define SCI1_SSR    (*(unsigned volatile char *)	 (0xffff84))  /* Serial status      ch 1 */
#define SCI1_RDR    (ICC_UNSIGN_CHAR	 (0xffff85))  /* Receive data       ch 1 */

#define SCI2_SMR    (ICC_UNSIGN_CHAR	 (0xffff88))  /* Serial mode        ch 1 */
#define SCI2_BRR    (ICC_UNSIGN_CHAR	 (0xffff89))  /* Bit rate           ch 1 */
#define SCI2_SCR    (ICC_UNSIGN_CHAR	 (0xffff8a))  /* Serial control     ch 1 */
#define SCI2_TDR    (ICC_UNSIGN_CHAR	 (0xffff8b))  /* Transmit data      ch 1 */
#define SCI2_SSR    (*(unsigned volatile char *)	 (0xffff8c))  /* Serial status      ch 1 */
#define SCI2_RDR    (ICC_UNSIGN_CHAR	 (0xffff8d))  /* Receive data       ch 1 */

#define TPU_TSTR    (ICC_UNSIGN_CHAR	 (0xffffc0))  /* Timer start             */

#define TPU_TCR0    (ICC_UNSIGN_CHAR	 (0xffffd0))  /* Timer control      ch 0 */
#define TPU_TMDR0   (ICC_UNSIGN_CHAR	 (0xffffd1))  /* Timer mode         ch 0 */
#define TPU_TIOR0H  (ICC_UNSIGN_CHAR	 (0xffffd2))  /* Timer I/O ctrl H   ch 0 */
#define TPU_TIOR0L  (ICC_UNSIGN_CHAR	 (0xffffd3))  /* Timer I/O ctrl L   ch 0 */
#define TPU_TIER0   (ICC_UNSIGN_CHAR	 (0xffffd4))  /* Timer intrpt enab  ch 0 */
#define TPU_TSR0    (ICC_UNSIGN_CHAR	 (0xffffd5))  /* Timer status       ch 0 */
#define TPU_TCNT0   (ICC_UNSIGN_SHORT	 (0xffffd6))  /* Timer counter      ch 0 */
#define TPU_TGR0A   (ICC_UNSIGN_SHORT	 (0xffffd8))  /* Timer general A    ch 0 */
#define TPU_TGR0B   (ICC_UNSIGN_SHORT	 (0xffffda))  /* Timer general B    ch 0 */
#define TPU_TGR0C   (ICC_UNSIGN_SHORT	 (0xffffdc))  /* Timer general C    ch 0 */
#define TPU_TGR0D   (ICC_UNSIGN_CHAR	 (0xffffde))  /* Timer general D    ch 0 */

#define TPU_TCR1    (ICC_UNSIGN_CHAR	 (0xffffe0))  /* Timer control      ch 1 */
#define TPU_TMDR1   (ICC_UNSIGN_CHAR	 (0xffffe1))  /* Timer mode         ch 1 */
#define TPU_TIOR1   (ICC_UNSIGN_CHAR	 (0xffffe2))  /* Timer I/O ctrl H   ch 1 */
#define TPU_TIER1   (ICC_UNSIGN_CHAR	 (0xffffe4))  /* Timer intrpt enab  ch 1 */
#define TPU_TSR1    (ICC_UNSIGN_CHAR	 (0xffffe5))  /* Timer status       ch 1 */
#define TPU_TCNT1   (ICC_UNSIGN_SHORT	 (0xffffe6))  /* Timer counter      ch 1 */
#define TPU_TGR1A   (ICC_UNSIGN_SHORT	 (0xffffe8))  /* Timer general A    ch 1 */
#define TPU_TGR1B   (ICC_UNSIGN_SHORT	 (0xffffea))  /* Timer general B    ch 1 */

#endif		/* #ifndef CMX_H8S2357_H */
