// InputSelect.h: interface for the CInputSelect class.
//
//////////////////////////////////////////////////////////////////////
/*                   
 * Filename: InputSelect.h
 * Author: Zheng Miao
 * All rights reserved. 
 * RCS: $Header: InputSelect.h 1.8 2011/04/22 10:43:30PDT Dongsheng Zhang (dzhang) Exp  $
 *		$Revision: 1.8 $
 */             

#if !defined(AFX_INPUTSELECT_H__22B208B3_A9C4_4684_81D9_BACB42EC9D46__INCLUDED_)
#define AFX_INPUTSELECT_H__22B208B3_A9C4_4684_81D9_BACB42EC9D46__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "comm.h"

class CInputSelect  
{
private:
	enum { MAX_SSM_BLOCK_PERIOD_NORMAL = 20 };
	// Block SSM propagation under normal tracking.
	int m_ssmBlockCntUnderNormal;
public:
	void InputSelectTick(void);
	int MakeSelect(void); // main entry
public:
	static void CfTodSource(int tod);
	int IsSelectLocked(void);
	static void RefreshSelect(void);
	void SetTwinSelect(int select);
	void InputSelectPrint(Ccomm *pComm);
	void InputSetDebug(int debugflag);
	int GetActiveInput(void) { return m_activeInputSelected; };
	CInputSelect();
	virtual ~CInputSelect();
	static CInputSelect *s_pInputSelect;
	void InitInputSelect(void);
private:
	void UpdateSystemSsm(void);
	void EvalReferenceLed(int clear);
	int ChooseActiveInput(void);
	void SelectActiveInput(int select);
	int m_activeInputSelected;
	static int m_todSource;
	int volatile m_twinSelect;
public: 
	static int m_refSelectMethod;
	void CfRefSelectMethod(int method);
	void CfManualInput(int chan);
private:
	int m_manualInputChan;
	int m_systemSsm;
	int m_inputSelectDebug;
};

#endif // !defined(AFX_INPUTSELECT_H__22B208B3_A9C4_4684_81D9_BACB42EC9D46__INCLUDED_)
