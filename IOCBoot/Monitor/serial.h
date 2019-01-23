
/* Baud rate defintions for a 16Mhz clock source. */
#define BR_9600			64
#define BR_19200		32 
#define BR_38400		15
#define BR_57600		10

/* SCR bit definitions. */
#define		TXI_ENABLE	0x80
#define		RXI_ENABLE	0x40
#define		TX_ENABLE	0x20
#define		RX_ENABLE	0x10
#define		MPI_ENABLE	0x08
#define		TXEI_ENABLE	0x04
#define		RXEI_ENABLE	0x02
#define		CK0_ENABLE	0x01

/* SSR bit definitions. */
#define		TDRE		0x80
#define		RDRF		0x40

/* SMR bit definitions. */
#define		DATABITS_8	0
#define		DATABITS_7	0x40
#define		NOPARITY	0
#define		EVENPARITY	0x20
#define		ODDPARITY	0x30
#define		STOP1BIT	0
#define		STOP2BITS	0x08
#define		NODIVIDE	0
#define		DIVIDEBY4	1
#define		DIVIDEBY16	2
#define		DIVIDEBY64	3

#define DBGSCI

/* Functions declarations */
unsigned char GetChar1();
void GetStr1(unsigned char* str, int max);
void PutChar1(unsigned char c);
void PutStr1(unsigned char* str);
void InitSCI1();
void delay(unsigned char del);
