/***********************************************************************/
/*                                                                     */
/*  FILE        :vect.h                                                */
/*  DATE        :Wed, May 01, 2002                                     */
/*  DESCRIPTION :Definition of Vector                                  */
/*  CPU TYPE    :H8S/2633F                                             */
/*                                                                     */
/*  This file is generated by Hitachi Project Generator (Ver.1.1).     */
/*                                                                     */
/***********************************************************************/
                  


#ifdef __cplusplus
extern "C" {
#endif  

//;<<VECTOR DATA START (POWER ON RESET)>>
//;0 Power On Reset 
extern void PowerON_Reset(void);                                                                                                                   
//;<<VECTOR DATA END (POWER ON RESET)>>
//;<<VECTOR DATA START (MANUAL RESET)>>
//;1 Manual Reset
extern void Manual_Reset(void);                                                                                                                    
//;<<VECTOR DATA END (MANUAL RESET)>>
// 2 Reserved

// 3 Reserved

// 4 Reserved

// 5 Treace
#pragma interrupt INT_Treace
extern void INT_Treace(void);
// 6 Direct Transition
#pragma interrupt INT_Direct_Transition
extern void INT_Direct_Transition(void);
// 7 NMI 
#pragma interrupt INT_NMI
extern void INT_NMI(void);
// 8 User breakpoint trap
#pragma interrupt INT_TRAP1
extern void INT_TRAP1(void);
// 9 User breakpoint trap
#pragma interrupt INT_TRAP2
extern void INT_TRAP2(void);
// 10 User breakpoint trap
#pragma interrupt INT_TRAP3
extern void INT_TRAP3(void);
// 11 User breakpoint trap
#pragma interrupt INT_TRAP4
extern void INT_TRAP4(void);
// 12 Reserved

// 13 Reserved

// 14 Reserved

// 15 Reserved

// 16 External trap IRQ0
#pragma interrupt INT_IRQ0
extern void INT_IRQ0(void);
// 17 External trap IRQ1
#pragma interrupt INT_IRQ1
extern void INT_IRQ1(void);
// 18 External trap IRQ2
#pragma interrupt INT_IRQ2
extern void INT_IRQ2(void);
// 19 External trap IRQ3
#pragma interrupt INT_IRQ3
extern void INT_IRQ3(void);
// 20 External trap IRQ4
#pragma interrupt INT_IRQ4
extern void INT_IRQ4(void);
// 21 External trap IRQ5
#pragma interrupt INT_IRQ5
extern void INT_IRQ5(void);
// 22 External trap IRQ6
#pragma interrupt INT_IRQ6
extern void INT_IRQ6(void);
// 23 External trap IRQ7
#pragma interrupt INT_IRQ7
extern void INT_IRQ7(void);
// 24 SWDTEND DTC
#pragma interrupt INT_SWDTEND_DTC
extern void INT_SWDTEND_DTC(void);
// 25 WOVI0 
#pragma interrupt INT_WOVI0
extern void INT_WOVI0(void);
// 26 CMI
#pragma interrupt INT_CMI
extern void INT_CMI(void);
// 27 PC Break
#pragma interrupt INT_PC_Break
extern void INT_PC_Break(void);
// 28 ADI
#pragma interrupt INT_ADI
extern void INT_ADI(void);
// 29 WOVI1
#pragma interrupt INT_WOVI1
extern void INT_WOVI1(void);
// 30 Reserved

// 31 Reserved

// 32 TGI0A TPU0
#pragma interrupt INT_TGI0A_TPU0
extern void INT_TGI0A_TPU0(void);
// 33 TGI0B TPU0
#pragma interrupt INT_TGI0B_TPU0
extern void INT_TGI0B_TPU0(void);
// 34 TGI0C TPU0
#pragma interrupt INT_TGI0C_TPU0
extern void INT_TGI0C_TPU0(void);
// 35 TGI0D TPU0
#pragma interrupt INT_TGI0D_TPU0
extern void INT_TGI0D_TPU0(void);
// 36 TCI0V TPU0
#pragma interrupt INT_TCI0V_TPU0
extern void INT_TCI0V_TPU0(void);
// 37 Reserved

// 38 Reserved

// 39 Reserved

// 40 TGI1A TPU1
#pragma interrupt INT_TGI1A_TPU1
extern void INT_TGI1A_TPU1(void);
// 41 TGI1B TPU1
#pragma interrupt INT_TGI1B_TPU1
extern void INT_TGI1B_TPU1(void);
// 42 TCI1V TPU1
#pragma interrupt INT_TCI1V_TPU1
extern void INT_TCI1V_TPU1(void);
// 43 TCI1U TPU1
#pragma interrupt INT_TCI1U_TPU1
extern void INT_TCI1U_TPU1(void);
// 44 TGI2A TPU2
#pragma interrupt INT_TGI2A_TPU2
extern void INT_TGI2A_TPU2(void);
// 45 TGI2B TPU2
#pragma interrupt INT_TGI2B_TPU2
extern void INT_TGI2B_TPU2(void);
// 46 TCI2V TPU2
#pragma interrupt INT_TCI2V_TPU2
extern void INT_TCI2V_TPU2(void);
// 47 TCI2U TPU2
#pragma interrupt INT_TCI2U_TPU2
extern void INT_TCI2U_TPU2(void);
// 48 TGI3A TPU3
#pragma interrupt INT_TGI3A_TPU3
extern void INT_TGI3A_TPU3(void);
// 49 TGI3B TPU3
#pragma interrupt INT_TGI3B_TPU3
extern void INT_TGI3B_TPU3(void);
// 50 TGI3C TPU3
#pragma interrupt INT_TGI3C_TPU3
extern void INT_TGI3C_TPU3(void);
// 51 TGI3D TPU3
#pragma interrupt INT_TGI3D_TPU3
extern void INT_TGI3D_TPU3(void);
// 52 TCI3V TPU3
#pragma interrupt INT_TCI3V_TPU3
extern void INT_TCI3V_TPU3(void);
// 53 Reserved

// 54 Reserved

// 55 Reserved

// 56 TGI4A TPU4
#pragma interrupt INT_TGI4A_TPU4
extern void INT_TGI4A_TPU4(void);
// 57 TGI4B TPU4
#pragma interrupt INT_TGI4B_TPU4
extern void INT_TGI4B_TPU4(void);
// 58 TCI4V TPU4
#pragma interrupt INT_TCI4V_TPU4
extern void INT_TCI4V_TPU4(void);
// 59 TCI4U TPU4
#pragma interrupt INT_TCI4U_TPU4
extern void INT_TCI4U_TPU4(void);
// 60 TGI5A TPU5
#pragma interrupt INT_TGI5A_TPU5
extern void INT_TGI5A_TPU5(void);
// 61 TGI5B TPU5
#pragma interrupt INT_TGI5B_TPU5
extern void INT_TGI5B_TPU5(void);
// 62 TCI5V TPU5
#pragma interrupt INT_TCI5V_TPU5
extern void INT_TCI5V_TPU5(void);
// 63 TCI5U TPU5
#pragma interrupt INT_TCI5U_TPU5
extern void INT_TCI5U_TPU5(void);
// 64 CMIA0A
#pragma interrupt INT_CMIA0A
extern void INT_CMIA0A(void);
// 65 CMIB0A
#pragma interrupt INT_CMIB0A
extern void INT_CMIB0A(void);
// 66 OVI0A
#pragma interrupt INT_OVI0A
extern void INT_OVI0A(void);
// 67 Reserved

// 68 CMIA1A
#pragma interrupt INT_CMIA1A
extern void INT_CMIA1A(void);
// 69 CMIB1A
#pragma interrupt INT_CMIB1A
extern void INT_CMIB1A(void);
// 70 OVI1A
#pragma interrupt INT_OVI1A
extern void INT_OVI1A(void);
// 71 Reserved

// 72 DEND0A
#pragma interrupt INT_DEND0A
extern void INT_DEND0A(void);
// 73 DEND0B
#pragma interrupt INT_DEND0B
extern void INT_DEND0B(void);
// 74 DEND1A
#pragma interrupt INT_DEND1A
extern void INT_DEND1A(void);
// 75 DEND1B
#pragma interrupt INT_DEND1B
extern void INT_DEND1B(void);
// 76 Reserved

// 77 Reserved

// 78 Reserved

// 79 Reserved

// 80 ERI0 SCI0
#pragma interrupt INT_ERI0_SCI0
extern void INT_ERI0_SCI0(void);
// 81 RXI0 SCI0
#pragma interrupt INT_RXI0_SCI0
extern void INT_RXI0_SCI0(void);
// 82 TXI0 SCI0
#pragma interrupt INT_TXI0_SCI0
extern void INT_TXI0_SCI0(void);
// 83 TEI0 SCI0
#pragma interrupt INT_TEI0_SCI0
extern void INT_TEI0_SCI0(void);
// 84 ERI1 SCI1
#pragma interrupt INT_ERI1_SCI1
extern void INT_ERI1_SCI1(void);
// 85 RXI1 SCI1
#pragma interrupt INT_RXI1_SCI1
extern void INT_RXI1_SCI1(void);
// 86 TXI1 SCI1
#pragma interrupt INT_TXI1_SCI1
extern void INT_TXI1_SCI1(void);
// 87 TEI1 SCI1
#pragma interrupt INT_TEI1_SCI1
extern void INT_TEI1_SCI1(void);
// 88 ERI2 SCI2
#pragma interrupt INT_ERI2_SCI2
extern void INT_ERI2_SCI2(void);
// 89 RXI2 SCI2
#pragma interrupt INT_RXI2_SCI2
extern void INT_RXI2_SCI2(void);
// 90 TXI2 SCI2
#pragma interrupt INT_TXI2_SCI2
extern void INT_TXI2_SCI2(void);
// 91 TEI2 SCI2
#pragma interrupt INT_TEI2_SCI2
extern void INT_TEI2_SCI2(void);
// 92 CMIA0B
#pragma interrupt INT_CMIA0B
extern void INT_CMIA0B(void);
// 93 CMIB0B
#pragma interrupt INT_CMIB0B
extern void INT_CMIB0B(void);
// 94 OVI0B
#pragma interrupt INT_OVI0B
extern void INT_OVI0B(void);
// 95 Reserved

// 96 CMIA1B
#pragma interrupt INT_CMIA1B
extern void INT_CMIA1B(void);
// 97 CMIB1B
#pragma interrupt INT_CMIB1B
extern void INT_CMIB1B(void);
// 98 OVI1B
#pragma interrupt INT_OVI1B
extern void INT_OVI1B(void);
// 99 Reserved

// 100 IICI0
#pragma interrupt INT_IICI0
extern void INT_IICI0(void);
// 101 DDCSWI
#pragma interrupt INT_DDCSWI
extern void INT_DDCSWI(void);
// 102 IICI1
#pragma interrupt INT_IICI1
extern void INT_IICI1(void);
// 103 Reserved

// 104 Reserved

// 105 Reserved

// 106 Reserved

// 107 Reserved

// 108 Reserved

// 109 Reserved

// 110 Reserved

// 111 Reserved

// 112 Reserved

// 113 Reserved

// 114 Reserved

// 115 Reserved

// 116 Reserved

// 117 Reserved

// 118 Reserved

// 119 Reserved

// 120 ERI3 SCI3
#pragma interrupt INT_ERI3_SCI3
extern void INT_ERI3_SCI3(void);
// 121 RXI3 SCI3
#pragma interrupt INT_RXI3_SCI3
extern void INT_RXI3_SCI3(void);
// 122 TXI3 SCI3
#pragma interrupt INT_TXI3_SCI3
extern void INT_TXI3_SCI3(void);
// 123 TEI3 SCI3
#pragma interrupt INT_TEI3_SCI3
extern void INT_TEI3_SCI3(void);
// 124 ERI4 SCI4
#pragma interrupt INT_ERI4_SCI4
extern void INT_ERI4_SCI4(void);
// 125 RXI4 SCI4
#pragma interrupt INT_RXI4_SCI4
extern void INT_RXI4_SCI4(void);
// 126 TXI4 SCI4
#pragma interrupt INT_TXI4_SCI4
extern void INT_TXI4_SCI4(void);
// 127 TEI4 SCI4
#pragma interrupt INT_TEI4_SCI4
extern void INT_TEI4_SCI4(void);
// Dummy
#pragma interrupt Dummy
extern void Dummy(void);

#ifdef __cplusplus
}
#endif  
