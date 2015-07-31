// WinStartup.cpp: implementation of the CWinStartup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinStartup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const TCHAR KEY_STARTUP[] = { _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") };



CWinStartup::CWinStartup()
{

}

CWinStartup::~CWinStartup()
{

}

bool CWinStartup::AddApp(HINSTANCE hInst, LPCTSTR lpszName, StartupUser user)
{
	TCHAR szPath[MAX_PATH];
	GetCurrentAppPath(hInst, szPath, sizeof(szPath));

	return AddApp(lpszName, szPath, user);
}

bool CWinStartup::AddApp(LPCTSTR lpszName, LPCTSTR lpszPath, StartupUser user)
{
	HKEY hRootKey;

	if (user == CurrentUser)
	{
		hRootKey = HKEY_CURRENT_USER;
	}
	else
	{
		hRootKey = HKEY_LOCAL_MACHINE;
	}

	HKEY hKey = 0;
	HRESULT hr = RegOpenKey(hRootKey, KEY_STARTUP, &hKey);

	if (hr == ERROR_SUCCESS)
	{
		hr = RegSetValueEx(hKey, lpszName, 0, REG_SZ, (const BYTE*)lpszPath, strlen((char*)lpszPath));

		RegCloseKey(hKey);
	}

	return (hr == ERROR_SUCCESS);
}

bool CWinStartup::RemoveApp(LPCTSTR lpszName, StartupUser user)
{
	HKEY hRootKey;

	if (user == CurrentUser)
	{
		hRootKey = HKEY_CURRENT_USER;
	}
	else
	{
		hRootKey = HKEY_LOCAL_MACHINE;
	}

	HKEY hKey = 0;
	HRESULT hr = RegOpenKey(hRootKey, KEY_STARTUP, &hKey);

	if (hr == ERROR_SUCCESS)
	{
		hr = RegDeleteValue(hKey, lpszName);

		RegCloseKey(hKey);
	}

	return (hr == ERROR_SUCCESS);
}

LPCTSTR CWinStartup::GetCurrentAppPath(HINSTANCE hInst, LPTSTR lpszPath, int nSize)
{
	GetModuleFileName(hInst, lpszPath, nSize);

	return lpszPath;
}
