// IstateParser.h: interface for the CIstateParser class.
//
//////////////////////////////////////////////////////////////////////
/*
 * Filename: IstateParser.h
 * Author: Zheng Miao of Symmetricom Inc.
 * All rights reserved.
 * RCS: $Header: IstateParser.h 1.1 2007/12/06 11:41:19PST Zheng Miao (zmiao) Exp  $
 *		$Revision: 1.1 $
 */

#if !defined(AFX_ISTATEPARSER_H__56A70870_BB54_4FD4_9F13_F671A280FC98__INCLUDED_)
#define AFX_ISTATEPARSER_H__56A70870_BB54_4FD4_9F13_F671A280FC98__INCLUDED_

class CStorage;
class CIstate;
class CIstateKb;
class CIstateProcess;

class CIstateParser  
{
public:
	enum { ISTATE_PARSER_VERSION = 1 };
	int ReadIstate(void);
	int PrintIstate(int bWithOpen = 1);
	CIstateParser(CStorage *pStorage, CIstateProcess *pIstate);
	virtual ~CIstateParser();
private:
	CStorage *m_pStorage;
	CIstateProcess *m_pIstateProcess;
	CIstate *m_pIstate;
	CIstateKb *m_pIstateKB;
};

#endif // !defined(AFX_ISTATEPARSER_H__56A70870_BB54_4FD4_9F13_F671A280FC98__INCLUDED_)
