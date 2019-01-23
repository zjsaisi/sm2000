/*                   
 * Filename: download.cpp
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved. 
 * RCS: $Header: BTMONDL.cpp 1.1 2007/12/06 11:41:18PST Zheng Miao (zmiao) Exp  $
 */             
                     
//#include "zmdefine.h"    
//#include "project.h"                 
#include "btmondl.h" 
#include "comm.h"
#include "flash.h"    
#include "string.h"
#include "Amd29.h"

#if DEBUG_PORT
#define DEBUG_LOOP 0
#else
#define DEBUG_LOOP 1
#endif

CBtmonDL::CBtmonDL(void)
{
}

CBtmonDL::~CBtmonDL(void)
{
}

void CBtmonDL::Start(Ccomm *pWork, Ccomm *pDebug, CFlash *pFlash, unsigned long initAddress)
{  
	int ret = 1, i, len;                           
	char buff[3];     
	int nTry;
	long ready_to_boot = 0x70000000;
	
	m_pWork = pWork;
	m_pDebug = pDebug;                                         
	m_pFlash = pFlash;
	m_oldBaudrate = pWork->GetBaudRate();
	do {
    	ret = ReadMessage();
		if (ret == 3) {
			// Reboot
			FUNC_PTR pFunc;
			unsigned long adr; 
			adr = (*((unsigned long *)0x0));
			adr &= 0xffffff;     
			pFunc = (FUNC_PTR)adr;
			(*pFunc)(); 	
		}
    	if (ret == 1) {
    		// Start download        
    		nTry = 10; 
    		m_dataAcc = 0;
    		m_startAddress = initAddress; 
    		m_prevChecksum = 1;
    		memset(&m_prevRecord, 0, sizeof(dld_struct));
    		do {                
    			ret = ReceiveDLDRecord();
    			if (ret == 1) {
    				buff[0] = 6;
    				m_pWork->Send(buff, 1);
    				nTry = 10;
    			} else if (ret == 2) {
    				// Last record
    				for (i = 0; i < 3; i++) {
    					buff[i] = 6;
    				}               
    				m_pWork->Send(buff, 3);
    				// Should go to somewhere 
    				// m_pWork->WaitTxDone(50);
    				// m_pWork->SetBaudRate(9600);
    				break;
    			}         
    			char *ptr = NULL;
    			if (ret < 0) {
    				// read the rest and throw away
    				do {
    					int len = m_pWork->Receive(buff, 1, 500); 
    					if (len != 1) break;
    				} while (1);            
    				
    				switch (ret) {
    				case -1:  
    					ptr = "\r\nReceive DLD header error\r\n";
    					nTry--;
    					break;
    				case -2:                     
    					ptr = "\r\nFlash error\r\n";
    					nTry = 0;
    					break;      
    				case -3:  
    					ptr = "\r\nTotal check sum failed\r\n";  
    					//nTry = 0;
    					// ZM: Give it more chance. Maybe it's just the last one 
    					nTry--;
    					break;	 
    				case -4:
    					ptr = "\r\nErase flash failed\r\n";
    					nTry = 0;
    					break;	
					case -5: 
						ptr = "\r\nReceive DLD data body error\r\n";
						nTry--;
						break;
					case -6:
						ptr = "\r\nDLD checksum error\r\n";
						nTry--;
						break;
					case -7:
						ptr = "\r\nDLD starts without \'%\'\r\n";
						nTry--;
						break;
    				default:
    					ptr = "\r\nUnknown error\r\n";
    					nTry = 0;
    					break;	
    				}             
    			}
    			if (ptr && m_pDebug) {
    				len = strlen(ptr);
    				m_pDebug->Send(ptr, len);
    			}
    		} while (ret == 1 || nTry);
			m_pWork->WaitTxDone(500);
			m_pWork->SetBaudRate(m_oldBaudrate/*9600*/);    		    		
    	}
#if 0		
		nTry = 10;
		while (1) {
    		ret = ReadMessage();
			if (ret == 2) {     
				// End of download, Just leave 
				return;
				// ready_to_boot = 60L; // 1 minute before reboot
			}
			if (nTry-- <= 0) return;
		}
#endif
		
    	// End of download, go reboot
#if 0    	        
    	if ((ready_to_boot--) == 0L) {
			FUNC_PTR func;
			unsigned long adr;   
#if _DEBUG			
			adr = (*((unsigned long *)0x200000));    
#else
			adr = (*((unsigned long *)0x0));
#endif	
			adr &= 0xffffff;     
			func = (FUNC_PTR)adr;
			(*func)(); 	
    	}
#endif    	
    } while (1);
}

                                 
void CBtmonDL::DumpDLDRecord(dld_struct *pRec)
{
	int i;
	char *ptr;
	
	if (m_pDebug == NULL) return;
	ptr = "\r\nDLD record: "; 
	i = strlen(ptr);
	m_pDebug->Send(ptr, i);
	ptr = (char *)pRec;
	i = pRec->Data_Size + 6;	
	ShowData(ptr, i);
	m_pDebug->Send("\r\n", 2);
}

// 1 for success                           
// 2 for the last record
// -1 Receive header error
// -2 device error    
// -3 Total file checksum failed 
// -4 Erase flash error
// -5 Receive data body error
// -6 check sum error
// -7 '%' missing
int CBtmonDL::ReceiveDLDRecord(void)
{   
	int i, ret;
	dld_struct rec;
	uint8 *ptr;
	unsigned long startAddress = m_startAddress;  
	unsigned long nextAddress;
	uint8 chksum = 0;
	int8 bUsingNewBlock = 0;
	int recordLen;   
	uint16 wTmp, wAcc = 0;                               
	
	ptr = (uint8 *)&rec;
	for (i = 0; i < 6; i++) {
		ret = ReadByteFromCom(ptr, 5000);
		if (ret != 1) 
			return -1;      
		chksum += *ptr;
		ptr++;		
		
	}	      
	if (rec.Flag != '%') return -7;
	
	ptr = rec.Data_Field;	       
	for (i = 0; i < rec.Data_Size; i++) {
		ret = ReadByteFromCom(ptr, 1000);
		if (ret != 1) 
			return -5;    
		chksum += *ptr;
		wTmp = (i & 1) ? ((uint16)(*ptr)) : ((uint16)(*ptr) << 8);
		wAcc += wTmp; //m_dataAcc += wTmp;	
		ptr++;		
	}                  
	 
	chksum -= '%';
	chksum -= rec.Data_Size;
	chksum -= rec.Chk_Sum;
	if (rec.Data_Size == 0) {
	    wTmp = ((uint16)rec.Address[0] << 8) + rec.Address[1];
	    if (m_dataAcc != wTmp * 2) {                          
	    	DumpDLDRecord(&rec);                       
	    	if (m_pDebug) {
	    		m_pDebug->Send("File Sum: ", 9);
	    		ShowData((char *)&m_dataAcc, 2);
	    		m_pDebug->Send("\r\n", 2);
	    	}
	    	return -3;
	    }
	} else if (chksum != rec.Chk_Sum) {
		// Chksum error;
		DumpDLDRecord(&rec);
		return -6;
	}                            
	
	// Check if it's dup record
	if (m_prevChecksum == chksum) {
        char *ptr1, *ptr2;
        ptr1 = (char *)&m_prevRecord;
        ptr2 = (char *)&rec;
        for (i = 0; i < rec.Data_Size + 6; i++) {
        	if (*ptr1 != *ptr2) { i = 0; break; }
        	ptr1++;
        	ptr2++; 
        }               
        if ( i ) {   
        	// Dup record 
        	return 1;
        }
	}

	m_prevChecksum = chksum;
	m_prevRecord = rec;

	// Check if it's entering new block
	recordLen = 6 + rec.Data_Size;
	nextAddress = startAddress + recordLen - 1;
	if ((startAddress & 0xffff) == 0) {
		bUsingNewBlock = 1;
	} else if ((startAddress & 0xff0000) != (nextAddress & 0xff0000)) {
		bUsingNewBlock = 1;
	}                                        
	if (bUsingNewBlock) {
		unsigned long begin, end, adr;
		int8 bEraseOnce = 0;
		begin = nextAddress & 0xff0000;
		end = nextAddress | 0xffff;
		adr = begin;

		m_pFlash->Open();
		do { 
			if (*((uint16 *)adr) != 0xffff) {
				if (bEraseOnce) {
					m_pFlash->Close();
					return -4;
				}
				m_pFlash->Close();
				m_pFlash->EraseSector(begin);
				m_pFlash->Open();
				bEraseOnce = 1;
				adr = begin;
				continue;
			}               
			adr += 2;
		} while (adr < end);
		m_pFlash->Close();
	}                
	// Write the record to flash    
	ptr = (uint8 *)&rec;
	ret = m_pFlash->WriteBytes(startAddress, recordLen, ptr);
	if (ret != 1) return -2;    
	m_startAddress = nextAddress + 1;    
	m_dataAcc += wAcc;
	if (rec.Data_Size == 0) 
		return 2;
	return 1;
}

// Return 1 for start of download
// return 2 for end of download
// return 3 to reboot
int CBtmonDL::ReadMessage(void)
{                                
	uint16 head;
	uint16 chksum;
	int i;
	                            
	do {                        
		i = ReadWordFromCom(&head);
		if (i != 1) return i;
		switch (head) {
		case 0x0600: // Ping
			i = ReadWordFromCom(&chksum);
			if (i != 1) break;
			if ((uint16)(chksum + head) != 0) {
				// Checksum error
				break;
			}         
			RespondPing();
			break;           
			               
#if 0			
		case 0x3c9: {
			uint16 tmp;
			int ret;
			for (i = 0; i < 23; i++) {
				ret = ReadWordFromCom(&tmp);
				if (ret != 1) break;
			}
			if (ret != 1) break;
			RespondAck();
			} break;	
#endif
			      
		case 0x0250: {  // Start, or End Download
			uint16 rx_id;
			uint16 cntl_word;
			uint16 packet_cnt;        
			uint16 sum = head;
			
			i = ReadWordFromCom(&rx_id);
			sum += rx_id;
			i &= ReadWordFromCom(&cntl_word);
			sum += cntl_word;
			i &= ReadWordFromCom(&packet_cnt);
			sum += packet_cnt;
			i &= ReadWordFromCom(&chksum);
			sum += chksum;
			if ((i != 1) || (sum != 0)) break;
			RespondAck();
			if (cntl_word & 0x20) { 
			    uint16 baud = cntl_word & 0xc0;
			    long rate = 9600;
			    switch (baud) {
			    case 0x0: 
			    	// 4800     
			    	rate = 4800;
			    	break; 
			    case 0x40:
			    	// 9600     
			    	rate = 9600;
			    	break;
			    case 0x80:
			    	// 19200    
			    	rate = 19200;
			    	break;
			    case 0xc0:
			    	// 38400     
			    	rate = 38400;
			    	break;				
			    }
			    m_pWork->WaitTxDone(500);
			    m_pWork->SetBaudRate(rate);
				return 1;    // Return to start to Receive file
			} else if (cntl_word == 0) {
				// control word == 0 is the end of download.
				return 2;
			} else if (cntl_word == 0x4) {
				// reset command
				return 3;
			}
			// Get back to normal 			 
			} break;	  
			  
		default: {
			uint16 tmp;  
			do {
				i = ReadWordFromCom(&tmp);
			} while (i ==1 );
			} break;
		} 
		
#if _DEBUG 
		if (m_pDebug != NULL) {
			m_pDebug->Send("\r\n", 2);
		}
#endif	
		
	} while (1);
	return 0;
}
                         
void CBtmonDL::RespondAck(void)
{  
	const uint8 ack[4] = { 0, 0x20, 0, 0xe0 };
	if (m_pWork == NULL) return;
	m_pWork->Send((char *)ack, 4);
}

void CBtmonDL::RespondPing(void)
{     
	int i; 
	uint16 chksum;          
	uint8 buff[2];
	const uint8 ping[62] = {
		0, 0x02, 0xff, 0x81, // Head + Sync
		0x6f, 0, // s1
		0xde, 0, // s2
		0x4d, 1, // s3
		0x03, 4, // fw_info // used to be 0xa4, 0
		0x2f, 0xae, 3, 0, // reldate0
		0x2f, 0xae, 3, 0, // reldate1
		0x06, 0, 0x4, 0, // rel_full0
		0x6, 0, 0x4, 0, // rel_full1
		0x62, 0x11, // cksum0
		0x62, 0x11, // cksum1
		0xaa, 0x55, 0xaa, 0x55, // valid
		0x50, 0x41, 0x24, 0xf1, 0, 0x80, 0, 0, // d4
		0, 0, 0x67, 0x2b, // L1    
		0, 0, 0xce, 0x56, // L2
		0x7e, 0,           // PMASK
		2, 0x1e,           // ECW
		0, 0, 0xf4, 0x86,  // l4 
		0x15, 0xf8
	};            
	if (m_pWork == NULL) return;
	// m_pWork->Send((char *)ping, 62);      
	m_pWork->Send((char *)ping, 60);
	chksum = 0;
	for (i = 2; i < 30; i++) {
		chksum += ((uint16)ping[i*2 + 1] << 8) + (uint16)ping[i*2];
	}
	chksum = (~chksum) + 1;                                                              
	buff[1] = chksum >> 8;
	buff[0] = (uint8)chksum;
	m_pWork->Send((char *)buff, 2);
	return;
}

// Return 1 for success            
// -1 failure
int CBtmonDL::ReadByteFromCom(uint8 *pRet, long ms)
{   
	int ret;         
	if (m_pWork == NULL) return -1;
	ret = m_pWork->Receive((char *)pRet, 1, ms);
	if (ret != 1) return -1;                                          
	return 1;
}

// Return 1 with success, otherwise 0            
int CBtmonDL::ReadWordFromCom(uint16 *pRet, long ms)
{   
	int ret;
	union headtype {                 
		uint8 buff[2];
		uint16 head;
	} hw;
	
	if (m_pWork == NULL) return 0;
	ret = m_pWork->Receive((char *)&(hw.buff[1]), 1, ms); 
	if (ret != 1) {
		return 0;
	}            
#if DEBUG_LOOP
	ShowData((char *)&(hw.buff[1]), 1);	
#endif	
	ret = m_pWork->Receive((char *)&(hw.buff[0]), 1, ms);
	if (ret != 1) {
		return 0;
	}            
#if DEBUG_LOOP
	ShowData((char *)&(hw.buff[0]), 1);	
#endif	
	*pRet = hw.head;
	return 1;
	
}

void CBtmonDL::ShowData(char *buffer, int len)
{              
	if (m_pDebug == NULL) return;
	
	int i;
	char buff[10];
	unsigned char *pStart;                   
	unsigned char ch;
	buff[2] = ' ';
	pStart = (unsigned char *)buffer;
	for (i = 0; i < (int)len; i++) {
		ch = ((*pStart >> 4) & 0xf) + '0';
		buff[0] = (ch > '9') ? (ch - '9' - 1 + 'A') : (ch);
		ch = (*pStart & 0xf) + '0';
		buff[1] = (ch > '9') ? (ch - '9' - 1 + 'A') : (ch);
		m_pDebug->Send(buff, 3); 
		pStart++;
	}  
	// m_pDebug->Send("\r\n", 2);
}
	
