#include "StdAfx.h"
#include "CzjlGetPath.h"
#include <shlobj.h>


CzjlGetPath::CzjlGetPath(void)
{
}


CzjlGetPath::~CzjlGetPath(void)
{
}

CString CzjlGetPath::GetTempPath(CStringA szPathName)
{
    CStringW strTemp;
	::GetTempPathW(1024,strTemp.GetBuffer(1024));
	strTemp.ReleaseBuffer();
	strTemp+=L"\\";
	strTemp+=szPathName;
	strTemp.Replace(L"\\\\",L"\\");
	SHCreateDirectory(::GetDesktopWindow(),strTemp);
    return strTemp;
}

CString CzjlGetPath::GetDeskopPath()
{
    LPITEMIDLIST ppidl = NULL; 

    CString strTemp;

    CoInitialize(NULL);
    if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl) == S_OK) // ���ϵͳ����·�� 
    {  
        BOOL flag = SHGetPathFromIDList(ppidl,      // ��ָ��һ���ĵ���Ŀ¼�ص��һ����Ŀ��ʶ����ĵ�ַ
            strTemp.GetBuffer(MAX_PATH));                                             // �����ĵ�ϵͳ·���Ļ����ַ����С������MAX_PATH
        CoTaskMemFree(ppidl);  
    }
    strTemp.ReleaseBuffer();
    strTemp+=L"\\";
    strTemp.Replace(L"\\\\",L"\\");
    return strTemp;
}


CStringW CzjlGetPath::GetLocalPath()
{
	CStringW strTemp;
	GetModuleFileNameW(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	strTemp=strTemp.Mid(0,strTemp.ReverseFind('\\'));
	strTemp+="\\";
	strTemp.Replace(L"\\\\",L"\\");
	return strTemp;
}

bool CzjlGetPath::CreatePath( CStringW szPath )
{
return SHCreateDirectory(::GetDesktopWindow(),szPath);
}
