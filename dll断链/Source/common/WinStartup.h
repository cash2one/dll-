// WinStartup.h: interface for the CWinStartup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINSTARTUP_H__61AB2BB8_97E3_47BB_91A2_17D620985B73__INCLUDED_)
#define AFX_WINSTARTUP_H__61AB2BB8_97E3_47BB_91A2_17D620985B73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum StartupUser { CurrentUser, AllUsers };

class CWinStartup  
{
public:
	CWinStartup();
	virtual ~CWinStartup();

public:	
	static bool AddApp(HINSTANCE hInst, LPCTSTR lpszName, StartupUser user);
	static bool AddApp(LPCTSTR lpszName, LPCTSTR lpszPath, StartupUser user);
	static bool RemoveApp(LPCTSTR lpszName, StartupUser user);

protected:
	static LPCTSTR GetCurrentAppPath(HINSTANCE hInst, LPTSTR lpszPath, int nSize);

};


#endif // !defined(AFX_WINSTARTUP_H__61AB2BB8_97E3_47BB_91A2_17D620985B73__INCLUDED_)
