// DtiOutputTrace.cpp: implementation of the CDtiOutputTrace class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Author: Zheng Miao
 * All rights reserved.
 * $Header: DtiOutputTrace.cpp 1.2 2009/05/01 09:48:18PDT Zheng Miao (zmiao) Exp  $
 * $Revision: 1.2 $
 * Handle Output Path Traceability
 */

#include "DtiOutputTrace.h"
#include "machine.h"
#include "string.h"
#include "DtiOutput.h"
#include "IstateDti.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDtiOutputTrace * CDtiOutputTrace::s_pDtiOutputTrace[NUM_OUTPUT_CHAN];
uint32 CDtiOutputTrace::m_myIpFour;
uint32 CDtiOutputTrace::m_myIpSix[4];
int CDtiOutputTrace::m_todSource = 0;

int CDtiOutputTrace::m_rootPortNumber = 1;
uint32 CDtiOutputTrace::m_rootIpFour = 0L;
uint32 CDtiOutputTrace::m_rootIpSix[4] = {0L, 0L, 0L, 0L };
int CDtiOutputTrace::m_rootVersion = 0;

CDtiOutputTrace::CDtiOutputTrace(int chan)
{
	m_myPortNumber = 0;
	m_chan = chan;
}

CDtiOutputTrace::~CDtiOutputTrace()
{

}

void CDtiOutputTrace::SendAsRoot()
{
	uint8 tmp_exr;
	int chan, i;
	CDtiOutputTrace *pTrace;
	char *ptr;

	tmp_exr = get_exr();
	set_exr(7);
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pTrace = s_pDtiOutputTrace[chan];
		if (pTrace == NULL) continue;
		*OUTPUT_PORT_SELECT = chan;
		
		// Send IPV4 as type 1
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 4;
		ptr = (char *)&m_myIpFour;
		for (i = 0; i < 4; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send Port number as Type 2
		*OUTPUT_PATH_TRACE = 2;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = pTrace->m_myPortNumber;

		// Send IPV6 as type 5
		*OUTPUT_PATH_TRACE = 5;
		*OUTPUT_PATH_TRACE = 16;
		ptr = (char *)m_myIpSix;
		for (i = 0; i < 16; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send DTI version number as type 7
		*OUTPUT_PATH_TRACE = 7;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = DTI_VERSION;

		// Send end of text
		*OUTPUT_PATH_TRACE = 9;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 0;
	}
	set_exr(tmp_exr);
}

void CDtiOutputTrace::SendAsSubtend()
{
	uint8 tmp_exr;
	int chan, i;
    CDtiOutputTrace *pTrace;
    char *ptr;
    
	tmp_exr = get_exr();
	set_exr(7);

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pTrace = s_pDtiOutputTrace[chan];
		if (pTrace == NULL) continue;

		*OUTPUT_PORT_SELECT = chan;

		// Send Root IPV4. Type 1
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 4;
		ptr = (char *)&m_rootIpFour;
		for (i = 0; i < 4; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send Root Port #. Type 2
		*OUTPUT_PATH_TRACE = 2;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = m_rootPortNumber;

		// Send my IPV4. Type 3
		*OUTPUT_PATH_TRACE = 3;
		*OUTPUT_PATH_TRACE = 4;
		ptr = (char *)&m_myIpFour;
		for (i = 0; i < 4; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send my port #. Type 4
		*OUTPUT_PATH_TRACE = 4;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = pTrace->m_myPortNumber;

		// Send Root IPV6. Type 5
		*OUTPUT_PATH_TRACE = 5;
		*OUTPUT_PATH_TRACE = 16;
		ptr = (char *)m_rootIpSix;
		for (i = 0; i < 16; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send my IPV6. Type 6
		*OUTPUT_PATH_TRACE = 6;
		*OUTPUT_PATH_TRACE = 16;
		ptr = (char *)m_myIpSix;
		for (i = 0; i < 16; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send Root version. Type 7
		*OUTPUT_PATH_TRACE = 7;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = m_rootVersion;

		// Send my version. Type 8
		*OUTPUT_PATH_TRACE = 8;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = DTI_VERSION;

		// End of text
		*OUTPUT_PATH_TRACE = 9;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 0;
	}

	set_exr(tmp_exr);

}

void CDtiOutputTrace::CfSelfPortNumber(int chan, int port)
{
	CDtiOutputTrace *pTrace;
	uint8 tmp_exr;

	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;

	pTrace = s_pDtiOutputTrace[chan];
	if (pTrace == NULL) return;

	if (pTrace->m_myPortNumber != port) {
		tmp_exr = get_exr();
		set_exr(7);
		pTrace->m_myPortNumber = port;
		set_exr(tmp_exr);
	}
}

void CDtiOutputTrace::CfSelfIpFour(uint32 ip)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	m_myIpFour = ip;
	set_exr(tmp_exr);
}

void CDtiOutputTrace::CfSelfIpSix(char *ptr)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	memcpy(m_myIpSix, ptr, sizeof(m_myIpSix));
	set_exr(tmp_exr);
}

void CDtiOutputTrace::Create()
{
	int chan;
	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		s_pDtiOutputTrace[chan] = new CDtiOutputTrace(chan);
	}
}


void CDtiOutputTrace::SendForFun()
{
	uint8 tmp_exr;
	int chan, i;
    CDtiOutputTrace *pTrace;
    char *ptr;
    
	tmp_exr = get_exr();
	set_exr(7);

	for (chan = 0; chan < NUM_OUTPUT_CHAN; chan++) {
		pTrace = s_pDtiOutputTrace[chan];
		if (pTrace == NULL) continue;

		*OUTPUT_PORT_SELECT = chan;

		// Send Root IPV4. Type 1
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 4;
		ptr = (char *)&m_rootIpFour;
		for (i = 0; i < 4; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send Root Port #. Type 2
		*OUTPUT_PATH_TRACE = 2;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = m_rootPortNumber;

		// Send my IPV4. Type 3
		*OUTPUT_PATH_TRACE = 3;
		*OUTPUT_PATH_TRACE = 4;
		ptr = (char *)&m_myIpFour;
		for (i = 0; i < 4; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send my port #. Type 4
		*OUTPUT_PATH_TRACE = 4;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = pTrace->m_myPortNumber;

		// Send Root IPV6. Type 5
		*OUTPUT_PATH_TRACE = 5;
		*OUTPUT_PATH_TRACE = 16;
		ptr = (char *)m_rootIpSix;
		for (i = 0; i < 16; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send my IPV6. Type 6
		*OUTPUT_PATH_TRACE = 6;
		*OUTPUT_PATH_TRACE = 16;
		ptr = (char *)m_myIpSix;
		for (i = 0; i < 16; i++) {
			*OUTPUT_PATH_TRACE = *ptr++;
		}

		// Send Root version. Type 7
		*OUTPUT_PATH_TRACE = 7;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = m_rootVersion;

		// Send my version. Type 8
		*OUTPUT_PATH_TRACE = 8;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = DTI_VERSION;

		// End of text
		*OUTPUT_PATH_TRACE = 9;
		*OUTPUT_PATH_TRACE = 1;
		*OUTPUT_PATH_TRACE = 0;
	}

	set_exr(tmp_exr);
}

void CDtiOutputTrace::SendTrace()
{
	if (m_todSource == CIstateDti::TOD_SOURCE_DTI) {
		SendAsSubtend();
	} else {
		SendAsRoot();
	}
}

void CDtiOutputTrace::CfTodSource(int source)
{
	m_todSource = source;
}

void CDtiOutputTrace::CfRootIpFour(uint32 ip)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	m_rootIpFour = ip;
	set_exr(tmp_exr);
}

void CDtiOutputTrace::CfRootIpSix(char *ptr)
{
	uint8 tmp_exr;
	tmp_exr = get_exr();
	set_exr(7);
	memcpy(m_rootIpSix, ptr, sizeof(m_rootIpSix));
	set_exr(tmp_exr);
}

void CDtiOutputTrace::CfRootPortNumber(int port)
{
	m_rootPortNumber = port;
}

void CDtiOutputTrace::CfRootDtiVersion(int version)
{
	m_rootVersion = version;
}

void CDtiOutputTrace::DtiOutputTraceHack(Ccomm *pComm)
{
	char buff[150];
	int i;

	sprintf(buff, "RootIP4=0x%08lX RootPort=%d RootVer=%d\r\nRootIP6:",
		m_rootIpFour, m_rootPortNumber, m_rootVersion);
	pComm->Send(buff);
	for (i = 0; i < 4; i++) {
		sprintf(buff, "0x%08lX ", m_rootIpSix[i]);
		pComm->Send(buff);
	}
	pComm->Send("\r\n");

	sprintf(buff, "MyIp4=0x%08lX  MyPort=%d  MyVer=%d\r\nMyIp6:"
		, m_myIpFour, m_myPortNumber, DTI_VERSION);
	pComm->Send(buff);
	for (i = 0; i < 4; i++) {
		sprintf(buff, "0x%08lX ", m_myIpSix[i]);
		pComm->Send(buff);
	}
	pComm->Send("\r\n");
}

void CDtiOutputTrace::TraceHack(int chan, Ccomm *pComm)
{
	if ((chan < 0) || (chan >= NUM_OUTPUT_CHAN)) return;
	s_pDtiOutputTrace[chan]->DtiOutputTraceHack(pComm);
}
