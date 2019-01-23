// IstateKb.h: interface for the CIstateKb class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateKb.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateKB.h 1.3 2008/01/29 11:29:15PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.3 $
 */

#if !defined(AFX_ISTATEKB_H__6EF25D18_807C_4DC4_81C0_9269F753323E__INCLUDED_)
#define AFX_ISTATEKB_H__6EF25D18_807C_4DC4_81C0_9269F753323E__INCLUDED_

#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include "IOCKeyList.h"
#include "IOCEvtList.h"
#include "comm.h"
#include "IstateIntf.h"
#include "string.h"

typedef void (*FUNC_INIT)(void *);

typedef enum {
	BIT8 = 1,
	BIT32 = 4,
	BITMAX = 1000
} IstateSize;

typedef enum {
	FLAG_NULL = 0,
	FLAG_IS_TOGGLE_ISTATE = 1,
	FLAG_READ_ONLY_ISTATE = 2, // Don't write to the buffer pointed by istate description
	FLAG_STANDALONE_ISTATE = 4, // This Istate item is outside the istate block to 
						// be copied between cards. It stays in RAM only.
						// This is experience from TimeHub. All the *SEC keyword
						// had better to stay in each individual card.
	FLAG_VOLATILE_ISTATE = 8, // Stay in RAM but not saved in Flash. Added on 4/14/2004.
					// It supposed to be shared between cards otherwise just make it standalone.
    FLAG_SILENT_ISTATE = 16, // If true, don't send response back to PPC.
	FLAG_AUTO_RESET_ISTATE = 32, // Auto reset after sending its copy to twin RTE.
	FLAG_IMC_READ_ONLY_ISTATE = 64, // Make it read only for PPC. RTE can change the content.

	ISTATE_FLAG_MAX = 10000
} IstateFlag;

typedef int (*CheckFunc)(int16 index1, int16 index2, int32 value, int16 cmd);
typedef void (*ActionRoutine)(int16 index1, int16 index2, uint32 value);

typedef struct {
	char const *pIstateHelp;
	IstateFlag flag;
	char const * const name;
	int16 max_first_index;
	int16 max_second_index;
	void *pVariable;	
	ActionRoutine pAction;
	FUNC_INIT pInit;
	int32	 initParam;
	CheckFunc pSpecialValueCheck;
	CIstateIntf *pInterface;

	// SToggleIstate, SEntryIstate are the same up to this point
	IstateSize size_type; // length of unit 8-bit 32-bit

} SToggleIstate;

typedef struct {
	char const *pIstateHelp;
	IstateFlag flag;
	char const * const name;
	int16 max_first_index;
	int16 max_second_index;
	void *pVariable;
	ActionRoutine pAction;
	FUNC_INIT pInit;
	int32 initParam;
	CheckFunc pSpecialValueCheck;	
	CIstateIntf *pInterface;
	
	// SToggleIstate, SEntryIstate are the same up to this point
	int32 minEntryValue; // inclusive
	int32 maxEntryValue; // inclusive
} SEntryIstate;


class CIstateKb  
{
public:
	int AddIstateItemDesc(int16 evtId, void *pDesc);
	uint16 GetIstateFlag(int16 id);
	virtual void ResetIstateValue(int16 id);
	// Get the memory location of the block to be saved in flash. 
	virtual uint8 * GetNonVolatileIstateLocation(uint16 *pSize, uint16 *pVersion) = 0;
	virtual uint8 * GetIstateLocation(uint16 *pSize, uint16 *pVersion) = 0;
	char const * GetHelpString(int16 id);
	virtual void VerifyIntegrity(void);
	virtual void InitOldValue(void) {}; 
	virtual int IsValueOK(uint16 id, int16 index1, int16 index2, int32 value, int16 cmd = CMD_WRITE_ISTATE);
	// int GetEntryMinMax(int16 id, int32 *pMin, int32 *pMax);
	void GetItemRange(int16 *pStart, int16 *pEnd);
	// Function called after transfer  
	virtual void ActionAll(void);
	int GetItemInfo(int16 id, uint16 *pFlag, int16 *pItemSize, int16 *pIndex1, int16 *pIndex2,
		void **pAddress, const char **pName, CIstateIntf **pExtraIntf = NULL);
	// Init all istate items that Knowbase is aware of.
	// make it virtual so it might be replace.
	// The user of CIstateKb is not aware of derived class.
	virtual void InitAllIstate(void);
	virtual void ExecAction(int16 id, int16 index1, int16 index2, uint32 value);
	CIstateKb(uint16 version, int16 start, int16 end);
	virtual ~CIstateKb();
protected:
	static void InitToggleIstate(void *ptr); // This is used by description struct. So it has to be static.
	static void InitEntryIstate(void *ptr); // Used by description struct. So it has to be static.
	virtual void *GetDesc(int16 id); 
		// return a pointer to SToggleIstate or SEntryIstate.
		// return NULL if not found.
protected:
	int InitOneIstate(int16 item);
	static void ActionNull(int16 index1, int16 index2, uint32 value);
	static void InitCopyIstate(void *ptr);
	uint16 m_istateVersion;
private:                
	int16 m_istateStart; // Non inclusive
	int16 m_istateEnd; // Non inclusive. 
	void ** m_keyList;
private:
};

#endif // !defined(AFX_ISTATEKB_H__6EF25D18_807C_4DC4_81C0_9269F753323E__INCLUDED_)
