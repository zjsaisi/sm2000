/// 2623regs.h

/********************************************************************
 *                                                                  *
 *  FILE          : 2623regs.h                                      *
 *  PURPOSE       : Register definitions for the EVB2623            *
 *  AUTHOR        : D T Hartley                                     *
 *  CREATED       : 19/02/99                                        *
 *                                                                  *
 ********************************************************************/

 
#define BASE    0xFFFF0000


/*--- Port Registers ---*/
#define P1DDR       (*((volatile unsigned char *)(BASE + 0xFE30)))
#define P1DR        (*((volatile unsigned char *)(BASE + 0xFF00)))
#define PORT1       (*((volatile unsigned char *)(BASE + 0xFFB0)))

#define PORT4       (*((volatile unsigned char *)(BASE + 0xFFB3)))

#define PORT9       (*((volatile unsigned char *)(BASE + 0xFFB8)))

#define PADDR       (*((volatile unsigned char *)(BASE + 0xFE39)))
#define PADR        (*((volatile unsigned char *)(BASE + 0xFF09)))
#define PORTA       (*((volatile unsigned char *)(BASE + 0xFFB9)))

#define PBDDR       (*((volatile unsigned char *)(BASE + 0xFE3A)))
#define PBDR        (*((volatile unsigned char *)(BASE + 0xFF0A)))
#define PORTB       (*((volatile unsigned char *)(BASE + 0xFFBA)))

#define PCDDR       (*((volatile unsigned char *)(BASE + 0xFE3B)))
#define PCDR        (*((volatile unsigned char *)(BASE + 0xFF0B)))
#define PORTC       (*((volatile unsigned char *)(BASE + 0xFFBB)))

#define PDDDR       (*((volatile unsigned char *)(BASE + 0xFE3C)))
#define PDDR        (*((volatile unsigned char *)(BASE + 0xFF0C)))
#define PORTD       (*((volatile unsigned char *)(BASE + 0xFFBC)))

#define PEDDR       (*((volatile unsigned char *)(BASE + 0xFE3D)))
#define PEDR        (*((volatile unsigned char *)(BASE + 0xFF0D)))
#define PORTE       (*((volatile unsigned char *)(BASE + 0xFFBD)))

#define PFDDR       (*((volatile unsigned char *)(BASE + 0xFE3E)))
#define PFDR        (*((volatile unsigned char *)(BASE + 0xFF0E)))
#define PORTF       (*((volatile unsigned char *)(BASE + 0xFFBE)))


/*--- System Control Registers ---*/
#define MDCR        (*((volatile unsigned char *)(BASE + 0xFDE7)))
#define SYSCR       (*((volatile unsigned char *)(BASE + 0xFDE5)))
#define MSTPCRA     (*((volatile unsigned char *)(BASE + 0xFDE8)))
#define MSTPCRB     (*((volatile unsigned char *)(BASE + 0xFDE9)))
#define MSTPCRC     (*((volatile unsigned char *)(BASE + 0xFDEA)))


/*--- BUS control Registers ---*/
#define ABWCR       (*((volatile unsigned char *)(BASE + 0xFED0)))
#define ASTCR       (*((volatile unsigned char *)(BASE + 0xFED1)))
#define WCRH        (*((volatile unsigned char *)(BASE + 0xFED2)))
#define WCRL        (*((volatile unsigned char *)(BASE + 0xFED3)))
#define BCRH        (*((volatile unsigned char *)(BASE + 0xFED4)))
#define BCRL        (*((volatile unsigned char *)(BASE + 0xFED5)))
#define PFCR		(*((volatile unsigned char *)(BASE + 0xFDEB)))


/*--- Interrupt Controller Registers ---*/
#define ISCRH       (*((volatile unsigned char *)(BASE + 0xFE12)))
#define ISCRL       (*((volatile unsigned char *)(BASE + 0xFE13)))
#define IER         (*((volatile unsigned char *)(BASE + 0xFE14)))
#define ISR         (*((volatile unsigned char *)(BASE + 0xFE15)))
#define IPRA        (*((volatile unsigned char *)(BASE + 0xFEC0)))
#define IPRB        (*((volatile unsigned char *)(BASE + 0xFEC1)))
#define IPRC        (*((volatile unsigned char *)(BASE + 0xFEC2)))
#define IPRD        (*((volatile unsigned char *)(BASE + 0xFEC3)))
#define IPRE        (*((volatile unsigned char *)(BASE + 0xFEC4)))
#define IPRF        (*((volatile unsigned char *)(BASE + 0xFEC5)))
#define IPRG        (*((volatile unsigned char *)(BASE + 0xFEC6)))
#define IPRH        (*((volatile unsigned char *)(BASE + 0xFEC7)))
#define IPRI        (*((volatile unsigned char *)(BASE + 0xFEC8)))
#define IPRJ        (*((volatile unsigned char *)(BASE + 0xFEC9)))
#define IPRK        (*((volatile unsigned char *)(BASE + 0xFECA)))
#define IPRM        (*((volatile unsigned char *)(BASE + 0xFECC)))


/*--- FLASH Registers ---*/
#define FLMCR1      (*((volatile unsigned char *)(BASE + 0xFFA8)))
#define FLMCR2      (*((volatile unsigned char *)(BASE + 0xFFA9)))
#define EBR1        (*((volatile unsigned char *)(BASE + 0xFFAA)))
#define EBR2        (*((volatile unsigned char *)(BASE + 0xFFAB)))
#define RAMER       (*((volatile unsigned char *)(BASE + 0xFEDB)))
#define FLPWCR      (*((volatile unsigned char *)(BASE + 0xFFAC)))
#define SCRX        (*((volatile unsigned char *)(BASE + 0xFDB4)))


/*--- SCI Registers ---*/
/*--- SCI Channel 0 ---*/
#define SCI0_SMR    (*((volatile unsigned char *)(BASE + 0xFF78)))
#define SCI0_BRR    (*((volatile unsigned char *)(BASE + 0xFF79)))
#define SCI0_SCR    (*((volatile unsigned char *)(BASE + 0xFF7A)))
#define SCI0_TDR    (*((volatile unsigned char *)(BASE + 0xFF7B)))
#define SCI0_SSR    (*((volatile unsigned char *)(BASE + 0xFF7C)))
#define SCI0_RDR    (*((volatile unsigned char *)(BASE + 0xFF7D)))
#define SCI0_SCMR   (*((volatile unsigned char *)(BASE + 0xFF7E)))

/*--- SCI Channel 1 ---*/
/*--- Don't define these as we will redefine them as SCI2 values or SCI0 values ---*/
/*--- in sci_mon.h. This is to avoid annoying compiler warnings! ---*/
/*
#define SCI1_SMR    (*((volatile unsigned char *)(BASE + 0xFF80)))
#define SCI1_BRR    (*((volatile unsigned char *)(BASE + 0xFF81)))
#define SCI1_SCR    (*((volatile unsigned char *)(BASE + 0xFF82)))
#define SCI1_TDR    (*((volatile unsigned char *)(BASE + 0xFF83)))
#define SCI1_SSR    (*((volatile unsigned char *)(BASE + 0xFF84)))
#define SCI1_RDR    (*((volatile unsigned char *)(BASE + 0xFF85)))
#define SCI1_SCMR   (*((volatile unsigned char *)(BASE + 0xFF86)))
*/

/*--- SCI Channel 2 ---*/
#define SCI2_SMR    (*((volatile unsigned char *)(BASE + 0xFF88)))
#define SCI2_BRR    (*((volatile unsigned char *)(BASE + 0xFF89)))
#define SCI2_SCR    (*((volatile unsigned char *)(BASE + 0xFF8A)))
#define SCI2_TDR    (*((volatile unsigned char *)(BASE + 0xFF8B)))
#define SCI2_SSR    (*((volatile unsigned char *)(BASE + 0xFF8C)))
#define SCI2_RDR    (*((volatile unsigned char *)(BASE + 0xFF8D)))
#define SCI2_SCMR   (*((volatile unsigned char *)(BASE + 0xFF8E)))


/*--- HCAN ---*/
#define MCR     	(*(volatile unsigned char *)(0xFFF800))
#define GSR     	(*(volatile unsigned char *)(0xFFF801))

#define BCR     	(*(volatile unsigned short *)(0xFFF802))
#define BCR1     	(*(volatile unsigned char *)(0xFFF802))
#define BCR0     	(*(volatile unsigned char *)(0xFFF803))

#define MBCR     	(*(volatile unsigned short *)(0xFFF804))
#define MBCR1     	(*(volatile unsigned char *)(0xFFF804))
#define MBCR0     	(*(volatile unsigned char *)(0xFFF805))

#define TXPR     	(*(volatile unsigned short *)(0xFFF806))
#define TXPR1     	(*(volatile unsigned char *)(0xFFF806))
#define TXPR0     	(*(volatile unsigned char *)(0xFFF807))

#define TXCR     	(*(volatile unsigned short *)(0xFFF808))
#define TXCR1     	(*(volatile unsigned char *)(0xFFF808))
#define TXCR0     	(*(volatile unsigned char *)(0xFFF809))

#define TXACK     	(*(volatile unsigned short *)(0xFFF80A))
#define TXACK1     	(*(volatile unsigned char *)(0xFFF80A))
#define TXACK0     	(*(volatile unsigned char *)(0xFFF80B))

#define ABACK     	(*(volatile unsigned short *)(0xFFF80C))
#define ABACK1     	(*(volatile unsigned char *)(0xFFF80C))
#define ABACK0     	(*(volatile unsigned char *)(0xFFF80D))

#define RXPR     	(*(volatile unsigned short *)(0xFFF80E))
#define RXPR1     	(*(volatile unsigned char *)(0xFFF80E))
#define RXPR0     	(*(volatile unsigned char *)(0xFFF80F))

#define RFPR     	(*(volatile unsigned short *)(0xFFF810))
#define RFPR1     	(*(volatile unsigned char *)(0xFFF810))
#define RFPR0     	(*(volatile unsigned char *)(0xFFF811))

#define IRR     	(*(volatile unsigned short *)(0xFFF812))
#define IRR1     	(*(volatile unsigned char *)(0xFFF812))
#define IRR0     	(*(volatile unsigned char *)(0xFFF813))

#define MBIMR     	(*(volatile unsigned short *)(0xFFF814))
#define MBIMR1     	(*(volatile unsigned char *)(0xFFF814))
#define MBIMR0     	(*(volatile unsigned char *)(0xFFF815))

#define IMR     	(*(volatile unsigned short *)(0xFFF816))
#define IMR1     	(*(volatile unsigned char *)(0xFFF816))
#define IMR0     	(*(volatile unsigned char *)(0xFFF817))

#define TEC     	(*(volatile unsigned char *)(0xFFF818))
#define REC     	(*(volatile unsigned char *)(0xFFF819))

#define UMSR      	(*(volatile unsigned short *)(0xFFF81A))
#define UMSR1     	(*(volatile unsigned char *)(0xFFF81A))
#define UMSR0     	(*(volatile unsigned char *)(0xFFF81B))

#define LAFM0     	(*(volatile unsigned char *)(0xFFF81D))
#define LAFM1     	(*(volatile unsigned char *)(0xFFF81C))
#define LAFM2     	(*(volatile unsigned char *)(0xFFF81F))
#define LAFM3     	(*(volatile unsigned char *)(0xFFF81E))
