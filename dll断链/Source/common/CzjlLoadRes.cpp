#include "StdAfx.h"
#include "CzjlLoadRes.h"
#include <winuser.h>


CzjlLoadRes::CzjlLoadRes( int RCDATAID )
{
	m_DataId=RCDATAID;
}


CzjlLoadRes::~CzjlLoadRes(void)
{
}

bool CzjlLoadRes::ExtraToFile( CString strLocalFile )
{
	HINSTANCE hInst=0;
	if (m_strDllName=="")
	{//exe
		 hInst=GetModuleHandle(0);
	}else{//dll
		hInst=GetModuleHandle(m_strDllName);
	}
	


	HRSRC hsrc=FindResource(hInst,MAKEINTRESOURCE(m_DataId),RT_RCDATA);
	if (!hsrc)
		return 0;
	DWORD size=SizeofResource(hInst,hsrc);
	if (!size)
		return 0;
	HGLOBAL hGlobal =LoadResource(hInst,hsrc);
	if (!size)
	{
		FreeResource(hGlobal);
		return 0;
	}
	char* pBuffer = (char*)LockResource(hGlobal); 
	if (!pBuffer)
	{
		FreeResource(hGlobal);
		return 0;
	}


	HANDLE hFile=CreateFile(strLocalFile,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		FreeResource(hGlobal);
		return 0;
	}

	DWORD byteWrite;
	WriteFile(hFile,pBuffer,size,&byteWrite,0);
	if (byteWrite != size)
	{
		CloseHandle(hFile);
		FreeResource(hGlobal);
		return 0;
	}

	CloseHandle(hFile);
	FreeResource(hGlobal);
	return 1;
}

void* CzjlLoadRes::ExtraToMemory()
{
	return 0;
}
/*
DWORD CzjlLoadRes::_LoadPbDllFromMemory( LPVOID lpRawDll, LPVOID lpImageDll )
{

}
*/