/*                   
 * Filename: IntrList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: IntrList.h 1.1 2007/12/06 11:39:18PST Zheng Miao (zmiao) Exp  $
 */             

	{
	// 2 Reserved
	((char  *) Dummy) + JUMP_INSTR,
	// 3 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 4 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 5 Treace
	((char *) INT_Treace) + JUMP_INSTR,
	// 6 Direct Transition
	((char *) INT_Direct_Transition) + JUMP_INSTR,
	// 7 NMI 
	((char *) INT_NMI) + JUMP_INSTR,
	// 8 User breakpoint trap
	((char *) INT_TRAP1) + JUMP_INSTR,
	// 9 User breakpoint trap
	((char *) INT_TRAP2) + JUMP_INSTR,
	// 10 User breakpoint trap
	((char *) INT_TRAP3) + JUMP_INSTR,
	// 11 User breakpoint trap
	((char *) INT_TRAP4) + JUMP_INSTR,
	// 12 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 13 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 14 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 15 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 16 External trap IRQ0
	((char *) INT_IRQ0) + JUMP_INSTR,
	// 17 External trap IRQ1
	((char *) INT_IRQ1) + JUMP_INSTR,
	// 18 External trap IRQ2
	((char *) INT_IRQ2) + JUMP_INSTR,
	// 19 External trap IRQ3
	((char *) INT_IRQ3) + JUMP_INSTR,
	// 20 External trap IRQ4
	((char *) INT_IRQ4) + JUMP_INSTR,
	// 21 External trap IRQ5
	((char *) INT_IRQ5) + JUMP_INSTR,
	// 22 External trap IRQ6
	((char *) INT_IRQ6) + JUMP_INSTR,
	// 23 External trap IRQ7
	((char *) INT_IRQ7) + JUMP_INSTR,
	// 24 SWDTEND DTC
	((char *) INT_SWDTEND_DTC) + JUMP_INSTR,
	// 25 WOVI0 
	((char *) INT_WOVI0) + JUMP_INSTR,
	// 26 CMI
	((char *) INT_CMI) + JUMP_INSTR,
	// 27 PC Break
	((char *) INT_PC_Break) + JUMP_INSTR,
	// 28 ADI
	((char *) INT_ADI) + JUMP_INSTR,
	// 29 WOVI1
	((char *) INT_WOVI1) + JUMP_INSTR,
	// 30 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 31 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 32 TGI0A TPU0
	((char *) INT_TGI0A_TPU0) + JUMP_INSTR,
	// 33 TGI0B TPU0
	((char *) INT_TGI0B_TPU0) + JUMP_INSTR,
	// 34 TGI0C TPU0
	((char *) INT_TGI0C_TPU0) + JUMP_INSTR,
	// 35 TGI0D TPU0
	((char *) INT_TGI0D_TPU0) + JUMP_INSTR,
	// 36 TCI0V TPU0
	((char *) INT_TCI0V_TPU0) + JUMP_INSTR,
	// 37 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 38 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 39 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 40 TGI1A TPU1
	((char *) INT_TGI1A_TPU1) + JUMP_INSTR,
	// 41 TGI1B TPU1
	((char *) INT_TGI1B_TPU1) + JUMP_INSTR,
	// 42 TCI1V TPU1
	((char *) INT_TCI1V_TPU1) + JUMP_INSTR,
	// 43 TCI1U TPU1
	((char *) INT_TCI1U_TPU1) + JUMP_INSTR,
	// 44 TGI2A TPU2
	((char *) INT_TGI2A_TPU2) + JUMP_INSTR,
	// 45 TGI2B TPU2
	((char *) INT_TGI2B_TPU2) + JUMP_INSTR,
	// 46 TCI2V TPU2
	((char *) INT_TCI2V_TPU2) + JUMP_INSTR,
	// 47 TCI2U TPU2
	((char *) INT_TCI2U_TPU2) + JUMP_INSTR,
	// 48 TGI3A TPU3
	((char *) INT_TGI3A_TPU3) + JUMP_INSTR,
	// 49 TGI3B TPU3
	((char *) INT_TGI3B_TPU3) + JUMP_INSTR,
	// 50 TGI3C TPU3
	((char *) INT_TGI3C_TPU3) + JUMP_INSTR,
	// 51 TGI3D TPU3
	((char *) INT_TGI3D_TPU3) + JUMP_INSTR,
	// 52 TCI3V TPU3
	((char *) INT_TCI3V_TPU3) + JUMP_INSTR,
	// 53 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 54 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 55 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 56 TGI4A TPU4
	((char *) INT_TGI4A_TPU4) + JUMP_INSTR,
	// 57 TGI4B TPU4
	((char *) INT_TGI4B_TPU4) + JUMP_INSTR,
	// 58 TCI4V TPU4
	((char *) INT_TCI4V_TPU4) + JUMP_INSTR,
	// 59 TCI4U TPU4
	((char *) INT_TCI4U_TPU4) + JUMP_INSTR,
	// 60 TGI5A TPU5
	((char *) INT_TGI5A_TPU5) + JUMP_INSTR,
	// 61 TGI5B TPU5
	((char *) INT_TGI5B_TPU5) + JUMP_INSTR,
	// 62 TCI5V TPU5
	((char *) INT_TCI5V_TPU5) + JUMP_INSTR,
	// 63 TCI5U TPU5
	((char *) INT_TCI5U_TPU5) + JUMP_INSTR,
	// 64 CMIA0A
	((char *) INT_CMIA0A) + JUMP_INSTR,
	// 65 CMIB0A
	((char *) INT_CMIB0A) + JUMP_INSTR,
	// 66 OVI0A
	((char *) INT_OVI0A) + JUMP_INSTR,
	// 67 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 68 CMIA1A
	((char *) INT_CMIA1A) + JUMP_INSTR,
	// 69 CMIB1A
	((char *) INT_CMIB1A) + JUMP_INSTR,
	// 70 OVI1A
	((char *) INT_OVI1A) + JUMP_INSTR,
	// 71 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 72 DEND0A
	((char *) INT_DEND0A) + JUMP_INSTR,
	// 73 DEND0B
	((char *) INT_DEND0B) + JUMP_INSTR,
	// 74 DEND1A
	((char *) INT_DEND1A) + JUMP_INSTR,
	// 75 DEND1B
	((char *) INT_DEND1B) + JUMP_INSTR,
	// 76 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 77 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 78 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 79 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 80 ERI0 SCI0
	((char *) INT_ERI0_SCI0) + JUMP_INSTR,
	// 81 RXI0 SCI0
	((char *) INT_RXI0_SCI0) + JUMP_INSTR,
	// 82 TXI0 SCI0
	((char *) INT_TXI0_SCI0) + JUMP_INSTR,
	// 83 TEI0 SCI0
	((char *) INT_TEI0_SCI0) + JUMP_INSTR,
	// 84 ERI1 SCI1
	((char *) INT_ERI1_SCI1) + JUMP_INSTR,
	// 85 RXI1 SCI1
	((char *) INT_RXI1_SCI1) + JUMP_INSTR,
	// 86 TXI1 SCI1
	((char *) INT_TXI1_SCI1) + JUMP_INSTR,
	// 87 TEI1 SCI1
	((char *) INT_TEI1_SCI1) + JUMP_INSTR,
	// 88 ERI2 SCI2
	((char *) INT_ERI2_SCI2) + JUMP_INSTR,
	// 89 RXI2 SCI2
	((char *) INT_RXI2_SCI2) + JUMP_INSTR,
	// 90 TXI2 SCI2
	((char *) INT_TXI2_SCI2) + JUMP_INSTR,
	// 91 TEI2 SCI2
	((char *) INT_TEI2_SCI2) + JUMP_INSTR,
	// 92 CMIA0B
	((char *) INT_CMIA0B) + JUMP_INSTR,
	// 93 CMIB0B
	((char *) INT_CMIB0B) + JUMP_INSTR,
	// 94 OVI0B
	((char *) INT_OVI0B) + JUMP_INSTR,
	// 95 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 96 CMIA1B
	((char *) INT_CMIA1B) + JUMP_INSTR,
	// 97 CMIB1B
	((char *) INT_CMIB1B) + JUMP_INSTR,
	// 98 OVI1B
	((char *) INT_OVI1B) + JUMP_INSTR,
	// 99 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 100 IICI0
	((char *) INT_IICI0) + JUMP_INSTR,
	// 101 DDCSWI
	((char *) INT_DDCSWI) + JUMP_INSTR,
	// 102 IICI1
	((char *) INT_IICI1) + JUMP_INSTR,
	// 103 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 104 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 105 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 106 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 107 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 108 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 109 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 110 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 111 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 112 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 113 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 114 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 115 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 116 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 117 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 118 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 119 Reserved
	((char *) Dummy) + JUMP_INSTR,
	// 120 ERI3 SCI3
	((char *) INT_ERI3_SCI3) + JUMP_INSTR,
	// 121 RXI3 SCI3
	((char *) INT_RXI3_SCI3) + JUMP_INSTR,
	// 122 TXI3 SCI3
	((char *) INT_TXI3_SCI3) + JUMP_INSTR,
	// 123 TEI3 SCI3
	((char *) INT_TEI3_SCI3) + JUMP_INSTR,
	// 124 ERI4 SCI4
	((char *) INT_ERI4_SCI4) + JUMP_INSTR,
	// 125 RXI4 SCI4
	((char *) INT_RXI4_SCI4) + JUMP_INSTR,
	// 126 TXI4 SCI4
	((char *) INT_TXI4_SCI4) + JUMP_INSTR,
	// 127 TEI4 SCI4
	((char *) INT_TEI4_SCI4) + JUMP_INSTR
};
