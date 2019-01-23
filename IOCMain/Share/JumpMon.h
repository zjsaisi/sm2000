/* JumpMon.h
 * Author: Zheng Miao 
 * $Header: JumpMon.h 1.1 2008/06/02 15:37:18PDT Zheng Miao (zmiao) Exp  $
 */
#include "CodeOpt.h"
#include "DataType.h"
#include "ConstDef.h"
#include <string.h>

class Ccomm;
#ifndef JUMP_MONITOR_06_02_2008
#define JUMP_MONITOR_06_02_2008


class CJumpMon {
public:
	CJumpMon(int size);
	virtual ~CJumpMon();
	void Update(int32 value);
	void GoNext(void);
	void JumpMonHack(Ccomm *pComm, uint32 *param);
private:
	int m_tail;
	int m_size;
	int32 *m_pMin;
	int32 *m_pMax;
	int32 m_current;
	int32 m_min;
	int32 m_max;
	int m_running;
	int m_cnt;
};
#endif
