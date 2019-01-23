/// instrctn.h

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL -- No mods
	
/* following two typedefs give ability to store two
	bytes as a word */            

typedef struct {
	unsigned char b0;
	unsigned char b1;
} TWO_BYTES;
	
typedef union {

	TWO_BYTES twobytes;
	unsigned short word;
} INSTRUCTION;
