/*                   
 * Filename: JumpList.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: JumpList.h 1.1 2007/12/06 11:39:19PST Zheng Miao (zmiao) Exp  $
 */             

enum {
	INT2 = 2,
	INT3 = 3,
	INT4 = 4,
	INT5 = 5,
	INT6 = 6,
	INT7 = 7,
	INT8 = 8,
	INT9 = 9,
	INT10 = 10,
	INT11 = 11,  
	INT12 = 12,
	INT13 = 13,
	INT14 = 14,
	INT15 = 15,
	INT16 = 16,
	INT17 = 17,
	INT18 = 18,
	INT19 = 19,
	
	INT20 = 20,
	INT21 = 21,  
	INT22 = 22,
	INT23 = 23,
	INT24 = 24,
	INT25 = 25,
	INT26 = 26,
	INT27 = 27,
	INT28 = 28,
	INT29 = 29,

	INT30 = 30,
	INT31 = 31,  
	INT32 = 32,
	INT33 = 33,
	INT34 = 34,
	INT35 = 35,
	INT36 = 36,
	INT37 = 37,
	INT38 = 38,
	INT39 = 39,

	INT40 = 40,
	INT41 = 41,  
	INT42 = 42,
	INT43 = 43,
	INT44 = 44,
	INT45 = 45,
	INT46 = 46,
	INT47 = 47,
	INT48 = 48,
	INT49 = 49,              
	
	INT50 = 50,
	INT51 = 51,  
	INT52 = 52,
	INT53 = 53,
	INT54 = 54,
	INT55 = 55,
	INT56 = 56,
	INT57 = 57,
	INT58 = 58,
	INT59 = 59,	  

	INT60 = 60,
	INT61 = 61,  
	INT62 = 62,
	INT63 = 63,
	INT64 = 64,
	INT65 = 65,
	INT66 = 66,
	INT67 = 67,
	INT68 = 68,
	INT69 = 69,	  

	INT70 = 70,
	INT71 = 71,  
	INT72 = 72,
	INT73 = 73,
	INT74 = 74,
	INT75 = 75,
	INT76 = 76,
	INT77 = 77,
	INT78 = 78,
	INT79 = 79,	  

	INT80 = 80,
	INT81 = 81,  
	INT82 = 82,
	INT83 = 83,
	INT84 = 84,
	INT85 = 85,
	INT86 = 86,
	INT87 = 87,
	INT88 = 88,
	INT89 = 89,	  

	INT90 = 90,
	INT91 = 91,  
	INT92 = 92,
	INT93 = 93,
	INT94 = 94,
	INT95 = 95,
	INT96 = 96,
	INT97 = 97,
	INT98 = 98,
	INT99 = 99,	  

	INT100 = 100,
	INT101 = 101,  
	INT102 = 102,
	INT103 = 103,
	INT104 = 104,
	INT105 = 105,
	INT106 = 106,
	INT107 = 107,
	INT108 = 108,
	INT109 = 109,

	INT110 = 110,
	INT111 = 111,  
	INT112 = 112,
	INT113 = 113,
	INT114 = 114,
	INT115 = 115,
	INT116 = 116,
	INT117 = 117,
	INT118 = 118,
	INT119 = 119,
	
	INT120 = 120,
	INT121 = 121,  
	INT122 = 122,
	INT123 = 123,
	INT124 = 124,
	INT125 = 125,
	INT126 = 126,
	INT127 = 127

};        

unsigned long mirror_INT_Vectors[] = { 
	RAM_START + 4 * INT2,   
	RAM_START + 4 * INT3,	
	RAM_START + 4 * INT4,	
	RAM_START + 4 * INT5,	
	RAM_START + 4 * INT6,	
	((unsigned long)NMIcapture), //RAM_START + 4 * INT7,	
	((unsigned long)BreakPoint), // RAM_START + 4 * INT8,	
	RAM_START + 4 * INT9,	 
	
	RAM_START + 4 * INT10,	
	((unsigned long) INT_TRAP4),// RAM_START + 4 * INT11,	
	RAM_START + 4 * INT12,	  
	RAM_START + 4 * INT13,	
	RAM_START + 4 * INT14,	
	RAM_START + 4 * INT15,	
	RAM_START + 4 * INT16,	
	RAM_START + 4 * INT17,	
	RAM_START + 4 * INT18,	
	RAM_START + 4 * INT19,	 
	
	RAM_START + 4 * INT20,	
	RAM_START + 4 * INT21,	
	RAM_START + 4 * INT22,	  
	RAM_START + 4 * INT23,	
	RAM_START + 4 * INT24,	
	RAM_START + 4 * INT25,	
	RAM_START + 4 * INT26,	
	RAM_START + 4 * INT27,	
	RAM_START + 4 * INT28,	
	RAM_START + 4 * INT29,	

	RAM_START + 4 * INT30,	
	RAM_START + 4 * INT31,	
	RAM_START + 4 * INT32,	  
	RAM_START + 4 * INT33,	
	RAM_START + 4 * INT34,	
	RAM_START + 4 * INT35,	
	RAM_START + 4 * INT36,	
	RAM_START + 4 * INT37,	
	RAM_START + 4 * INT38,	
	RAM_START + 4 * INT39,	 
	
	RAM_START + 4 * INT40,	
	RAM_START + 4 * INT41,	
	RAM_START + 4 * INT42,	  
	RAM_START + 4 * INT43,	
	RAM_START + 4 * INT44,	
	RAM_START + 4 * INT45,	
	RAM_START + 4 * INT46,	
	RAM_START + 4 * INT47,	
	RAM_START + 4 * INT48,	
	RAM_START + 4 * INT49,	

	RAM_START + 4 * INT50,	
	RAM_START + 4 * INT51,	
	RAM_START + 4 * INT52,	  
	RAM_START + 4 * INT53,	
	RAM_START + 4 * INT54,	
	RAM_START + 4 * INT55,	
	RAM_START + 4 * INT56,	
	RAM_START + 4 * INT57,	
	RAM_START + 4 * INT58,	
	RAM_START + 4 * INT59,	 

	RAM_START + 4 * INT60,	
	RAM_START + 4 * INT61,	
	RAM_START + 4 * INT62,	  
	RAM_START + 4 * INT63,	
	RAM_START + 4 * INT64,	
	RAM_START + 4 * INT65,	
	RAM_START + 4 * INT66,	
	RAM_START + 4 * INT67,	
	RAM_START + 4 * INT68,	
	RAM_START + 4 * INT69,	 

	RAM_START + 4 * INT70,	
	RAM_START + 4 * INT71,	
	RAM_START + 4 * INT72,	  
	RAM_START + 4 * INT73,	
	RAM_START + 4 * INT74,	
	RAM_START + 4 * INT75,	
	RAM_START + 4 * INT76,	
	RAM_START + 4 * INT77,	
	RAM_START + 4 * INT78,	
	RAM_START + 4 * INT79,	 

	RAM_START + 4 * INT80,	
	RAM_START + 4 * INT81,	
	RAM_START + 4 * INT82,	  
	RAM_START + 4 * INT83,	
	RAM_START + 4 * INT84,	
	RAM_START + 4 * INT85,	
	RAM_START + 4 * INT86,	
	RAM_START + 4 * INT87,	
	RAM_START + 4 * INT88,	
	RAM_START + 4 * INT89,	 

	RAM_START + 4 * INT90,	
	RAM_START + 4 * INT91,	
	RAM_START + 4 * INT92,	  
	RAM_START + 4 * INT93,	
	RAM_START + 4 * INT94,	
	RAM_START + 4 * INT95,	
	RAM_START + 4 * INT96,	
	RAM_START + 4 * INT97,	
	RAM_START + 4 * INT98,	
	RAM_START + 4 * INT99,	 

	RAM_START + 4 * INT100,	
	RAM_START + 4 * INT101,	
	RAM_START + 4 * INT102,	  
	RAM_START + 4 * INT103,	
	RAM_START + 4 * INT104,	
	RAM_START + 4 * INT105,	
	RAM_START + 4 * INT106,	
	RAM_START + 4 * INT107,	
	RAM_START + 4 * INT108,	
	RAM_START + 4 * INT109,	 

	RAM_START + 4 * INT110,	
	RAM_START + 4 * INT111,	
	RAM_START + 4 * INT112,	  
	RAM_START + 4 * INT113,	
	RAM_START + 4 * INT114,	
	RAM_START + 4 * INT115,	
	RAM_START + 4 * INT116,	
	RAM_START + 4 * INT117,	
	RAM_START + 4 * INT118,	
	RAM_START + 4 * INT119,	 
	

	RAM_START + 4 * INT120,	
	RAM_START + 4 * INT121,	
	RAM_START + 4 * INT122,	  
	RAM_START + 4 * INT123,	
	RAM_START + 4 * INT124,	
	RAM_START + 4 * INT125,	
	RAM_START + 4 * INT126,	
	RAM_START + 4 * INT127	
};          
