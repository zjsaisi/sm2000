// IstateExec.h: interface for the CIstateExec class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateExec.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateExec.h 1.2 2008/01/29 11:29:21PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.2 $
 */

#if !defined(AFX_ISTATEEXEC_H__A8CF294F_DEFC_4BBF_8A4E_8D44E0126914__INCLUDED_)
#define AFX_ISTATEEXEC_H__A8CF294F_DEFC_4BBF_8A4E_8D44E0126914__INCLUDED_
       
class CIstateBlockTrans;
class CIstateItemTrans;
class CIstateCheckSum;
class CIstate;
class CPTPLink;
class CIstateKb;
class Ccomm;

// This class wrap the CIstate, CIstateBlockTrans, CIstateItemTrans together
class CIstateExec  
{
public:
	int IsTwinSameIstateVersion(void);
	CIstateCheckSum *GetCheckSum(void) { return m_pCheckSum_IOC; };
	void Init(void);
	void PrintIstateExec(Ccomm *pComm);
	void ResendIstateToTwin(void);
	int RelayItemToTwinIOC(int16 item, int16 index1, int16 index2);
	int IsAvailable(void);
//	int WriteOneIstateItem(int16 item, int16 index1, int16 index2, uint32 value);
//	int ReadOneIstateItem(int16 item, int16 index1, int16 index2, uint32 *pResult);
	int DispatchMessageFromIMC(const uint8 *ptr, uint16 len);
	void TickPerSecond(void);
//	void Init(void);
	int DispatchMessageFromIOC(const uint8 *ptr, uint16 len);
	CIstateExec(//CFPGA *pFPGA, CCardState *pCardState, 
		CIstate *pIstate, 
		// CPTPLink *pIMCLink, uint8 imcPeer, // Keep only RTE-Istate in RTE
		CPTPLink *pIOCLink,
		uint8 iocPeer);
	virtual ~CIstateExec();
private:
	int InitTwinIstate(void);
	int m_cnt;
	int m_iocCompatible;
	int CheckIocCompatibility(void);
	uint32 m_linkedTime;
	uint16    m_copyEngine;
	CPTPLink *m_pLink;

	// Istate data // It could be RTE Istate or PPC Istate
	CIstate *m_pIstate;
	
	// Knowledge base
	CIstateKb *m_pKB;

	// Istate block transceiver between RTE/RTE. RTE <---> RTE
	CIstateBlockTrans *m_pBlockTR_IOC;
	CIstateItemTrans *m_pItemTR_IOC;
	CIstateCheckSum *m_pCheckSum_IOC;

#if 0 // Keep only RTE-Istate in RTE
	// Istate Block transceiver facing PPC. PPC <---> RTE
	int8 m_bDominating; // For RTE Istate, the copy in RTE dominate. For PPC Istate, it's not.

	CIstateBlockTrans *m_pBlockTR_IMC;
	CIstateItemTrans *m_pItemTR_IMC;
	CIstateCheckSum *m_pCheckSum_IMC;
#endif
};

#endif // !defined(AFX_ISTATEEXEC_H__A8CF294F_DEFC_4BBF_8A4E_8D44E0126914__INCLUDED_)
