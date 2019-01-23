// PTPFactory.h: interface for the CPTPFactory class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao of Symmetricom
 * All rights reserved.
 *
 * $Header: PTPFactory.h 1.2 2009/04/30 17:10:21PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 *
 */

#if !defined(AFX_PTPFACTORY_H__49A2BDEC_F4E5_4B0C_9E84_B2E5E9EAB2B3__INCLUDED_)
#define AFX_PTPFACTORY_H__49A2BDEC_F4E5_4B0C_9E84_B2E5E9EAB2B3__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"

class CFactory;
class CMemBlock;
class CPTPLink;

class CPTPFactory  
{
public:
	int ReceiveWritePacket(CMemBlock *pBlock);
	int ReceiveReadPacket(CMemBlock *pBlock);
	CPTPFactory(CFactory *pFactory, CPTPLink *pLink, uint8 outPort);
	virtual ~CPTPFactory();
private:
	CPTPLink *m_pLink;
	uint8 m_outPort;
	enum { PTP_STRING_SIZE = 32 };
	enum FACTORY_ID_LIST { FACTORY_ID_CARD_SERIAL_NO = 1, FACTORY_ID_HARDWARE_REV = 2,
		FACTORY_ID_CLEI_CODE = 3, FACTORY_ID_PART_NUMBER = 4,
		FACTORY_ID_OSC_SERIAL_NO = 5,
		FACTORY_ID_ECI_CODE = 6,
		FACTORY_ID_VERSION_STRING = 7,

		// Retimer retrieval 
		// 0x1MI: M 0..3 for A..D
		//		i: 4= partnum. 1=serial number  3=clei    2=hardware rev

		FACTORY_ID_MAX
	};

	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 id;
	} SCmdReadFactory;

	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 error_code;
		char value[PTP_STRING_SIZE];
	} SRespReadFactory;

	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 id;
		char value[PTP_STRING_SIZE];
	} SCmdWriteFactory;

	typedef struct {
		int16 cmd;
		uint32 handle;
		int16 error_code;
	} SRespWriteFactory;
	
	CFactory *m_pFactory;

};

#endif // !defined(AFX_PTPFACTORY_H__49A2BDEC_F4E5_4B0C_9E84_B2E5E9EAB2B3__INCLUDED_)
