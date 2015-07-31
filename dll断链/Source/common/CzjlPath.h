#pragma once
#include "windows.h"
#include "atlstr.h"

class CzjlPath
{
public:
	CzjlPath(char* pAppName=0);
	~CzjlPath(void);

	static CStringA GetAppLocalPathA();
	static CStringW GetAppLocalPathW();

	static CStringA GetAppExePathA();
	static CStringW GetAppExePathW();

	static CStringA GetProcessCommandLine(unsigned int PID);
	static DWORD GetPID(CStringA szName);

	static   CString GetDeskopPath();
	static bool CreatePath(CStringW szPath);
	static CStringW GetTempPathW(CStringW szPathName);
private:
	CStringA m_szAppName;
};

