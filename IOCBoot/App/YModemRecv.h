// YModemRecv.h: interface for the CYModemRecv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_)
#define AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_

#include "DataType.h"

// Set 1 to use table ( more space, quick )
// Set 0 to calculate CRC16 at run time
#define QUICK_CRC_TABLE 0

class Ccomm; // Communication port 
class CStorage; // Class to store received data

class CYModemRecv  
{
public:
	int Start(Ccomm *pPort, CStorage *pStorage);
	CYModemRecv();
	virtual ~CYModemRecv();
private:
	void UpdateCrc(uint8 newData, uint16 *cur_crc);
	enum { MODE_CRC16, MODE_CHECKSUM};
	static const uint8 SOH;
	static const uint8 STX;
	static const uint8 EOT;
	static const uint8 ACK;
	static const uint8 NAK;
	static const uint8 CAN;
	static const uint8 C;
private:
	int Purge(void);
	int RecvBlock(uint8 *blocknum, char *pBlock, int timeout = 5000);
	int RecvBlockZero(char *buff);
	Ccomm *m_pPort;
	CStorage *m_pStorage;
	int m_nTry;
	int8 m_mode;
	// Filelength
	uint32 m_fileLength;

	// Checksum
	uint16 m_crc16;
	uint8 m_checksum;

private:
#if QUICK_CRC_TABLE
	static unsigned short crcTable[256];
	static void InitCRC16(void);
#endif
};

#endif // !defined(AFX_YMODEMRECV_H__BB8924FC_CEFD_4929_A930_EC9EA939E816__INCLUDED_)
