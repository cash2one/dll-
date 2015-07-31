#pragma once
#include <atlstr.h>



class CzjlLoadRes
{
public:
	CzjlLoadRes(int RCDATAID);
	~CzjlLoadRes(void);

	void* ExtraToMemory();
	bool ExtraToFile(CString strLocalFile);

	void SetDllName(CStringW strDllName){m_strDllName=strDllName;};
private:
	int m_DataId;
	void* pImage;
	CStringW m_strDllName;
//	DWORD _LoadPbDllFromMemory(LPVOID lpRawDll, LPVOID lpImageDll);
};