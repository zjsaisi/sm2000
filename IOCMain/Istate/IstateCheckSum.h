// IstateCheckSum.h: interface for the CIstateCheckSum class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateCheckSum.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateCheckSum.h 1.2 2008/01/28 17:28:43PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ISTATECHECKSUM_H__CE0F5345_402B_4B2C_91FE_98D3861D800C__INCLUDED_)
#define AFX_ISTATECHECKSUM_H__CE0F5345_402B_4B2C_91FE_98D3861D800C__INCLUDED_

class CIstate;
class CPTPLink;
class CIstateKb;
class Ccomm;

class CIstateCheckSum  
{
private:
	typedef struct {
		uint16 cmd;
		// Version info
		uint16 version;
		uint32 hardware_option_istate; // hardware feature that might affect Istate structure.
		// Check sum
		uint32 checksum;
		// Master/slave info
		int8   is_active;
		int8   slot_id;
		int16  bInited;
		uint16 hardware_compatibility; // On 1/4/2006: hardware feature that might affect compatibility between IOCs.
		uint32 hardware_capbility; // Informational only.

		// 8/9/2005: Now this code is ported from TP RTE. We shouldn't have the same problem. 
		// 5/27/2004: For versions till now (actually only 1.01.02), If RTE receive different size,
		// it will ingnore it and don't count the message. That's a problem to receive 
		// Istate from 1.01.02(first release). 
		// If we have to change this structure, should be ok from now on after changing the code.
		// But if we do change this structure, there might be a need to do two-step process for upgrading from 1.01.02.
		// First attempt to change this without success.
		// After all these trouble, I just combine JIOSwitch into MSB of version to go round the problem.
		//int8   bJIOSwitch;
		//int8   spare01;
		// Expand only here. If you add any here, change the istate version.
	} SIstateCheckSum;

#if 0
	typedef struct { // This is added in TP 2.1 1.02.XX
		uint16 cmd;
		uint16 cmd_version; // version of this packet
		uint32 ioc_istate_version_extension; // combined with main istate version, it functions similar to IOCFW_PIOCFW_IOCFW
		uint32 ioc_hardware_ability; // IOCHW_PIOCHW_IOCHW
		uint32 extra[4];
	} SIstateSupplement;
#endif

public:
	int IsTwinCompatible(void);
	void LinkDown(void);
	int IsMaster(void);
	int IsTwinActive(void);
	//int IsTwinHardwareCompatible(void);
	void Init(void);
	static uint32 GetHardwareMask(void);
	void PrintIstateCheckSum(Ccomm *pComm);
	// static uint32 GetIstateVersionExtension(void);
	void ClearTwinInitFlag(void);
	void ResetMismatchCounter(void);
	void SetInited(void);
	int IsMatch(uint32 *pTimes);
	int IsTwinSameVersion(void);
	int IsTwinInited(void);
	int DispatchMessage(const uint8 *ptr, uint16 len);
	void TickPerSecondIstateCheckSum(void);
	CIstateCheckSum(/*CCardState *pCardState,*/ CIstate *pIstate, CPTPLink *pLink, uint8 peer);
	virtual ~CIstateCheckSum();
	enum {CMD_START = 0x30, CMD_CHECKSUM, // Periodic checksum exchange
		CMD_FORCE_UNINIT, // Force standby card to be un-initialized 
						  // so that it will copy from active one again.
		// CMD_SUPPLEMENT,				  
		CMD_END};
	// enum { CMD_SUPPLEMENT_VERSION = 1 };
private:
	int m_linkCnt;
	//int ReceiveSupplement(const uint8 *ptr, uint16 len);
	//int SendSupplement(void);
	int ReceiveCheckSum(const uint8 *ptr, uint16 len);
	uint32 GetCheckSum(void);
	int SendCheckSum(uint16 cmd = CMD_CHECKSUM);
	//CCardState *m_pCardState;
	CIstate *m_pIstate;
	CPTPLink *m_pLink;
	uint8 m_peer;
	// Get from m_pIstate
	CIstateKb *m_pKB;
	// From Knowledge base
	const uint8 *m_pIstateLocation;
	uint16 m_istateSize;
	uint16 m_istateVersion;

	// twin supplement
//	uint32 m_twinIstateExtension;
//	uint32 m_twinHardwareMask;

	// my own checksum
	uint32 m_thisIstateCheckSum;
	//int16 m_thisCardState;
	int16 m_bThisIstateInited;

	// Twin info
	uint32 m_twinIstateCheckSum;
	uint16 m_twinIstateVersion;
	uint32 m_twinHardwareOptionIstate;
	uint16 m_twinHardwareCompatibility;
	uint32 m_twinHardwareCapbility;
	int8 m_twinIsActive;
	int8 m_twinSlotId;
	int16 m_bTwinIstateInited;
	uint32 m_nCheckSumReceived;
	//uint32 m_nSupplementReceived;
	//int8 m_twinJIOSwitch;

	// Send a checksum package every idle 3 seconds
	int m_sendTimer;
	// Send extension packet timer
	//int m_extensionSendTimer;

	// mismatch counter
	int32 m_nMismatchCnt;
};

#endif // !defined(AFX_ISTATECHECKSUM_H__CE0F5345_402B_4B2C_91FE_98D3861D800C__INCLUDED_)
