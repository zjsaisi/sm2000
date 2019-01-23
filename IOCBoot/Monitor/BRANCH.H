/// branch.h

/* Ported to H8S2623 - 01/03/99 (DTH) - No modifications */
// Ported to 2633 1/10/01 DGL -- No mods

int IsBranch(unsigned short);
int IsJmp(unsigned short);
short get_displacement_of_branch (unsigned long);  
unsigned long get_address_of_jump (unsigned long);
