;/********************************************************************
; *                                                                  *
; *  FILE          : 2623bus.inc                                       *
; *  PURPOSE       : Bus register definitions for the EVB2623        *
; *                  #included in CSTARTUP.S20                       *
; *  AUTHOR        : D T Hartley                                     *
; *  CREATED       : 19/02/99                                        *
; *                                                                  *
; ********************************************************************/

 
BASE	.equ 	H'FFFF0000

ABWCR	.equ 	(BASE + H'FED0)
ASTCR	.equ	(BASE + H'FED1)
WCRH	.equ	(BASE + H'FED2)
WCRL	.equ	(BASE + H'FED3)
BCRH	.equ	(BASE + H'FED4)
BCRL	.equ	(BASE + H'FED5)
PFCR	.equ	(BASE + H'FDEB)

P1DR	.equ	(BASE + H'FF00)
P1DDR	.equ	(BASE + H'FE30)
PADDR	.equ	(BASE + H'FE39)
PBDDR	.equ	(BASE + H'FE3A)
PADR	.equ	(BASE + H'FF09)
PCDDR	.equ	(BASE + H'FE3B) 
PFDDR	.equ	(BASE + H'FE3E)

