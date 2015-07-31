#include "CzjlPath.h"
#include <time.h>
#include <shlobj.h>
#include <tlhelp32.h>

CzjlPath::CzjlPath( char* pAppName )
{
	if (pAppName==0)
	{
		srand((unsigned int)time(0));
		m_szAppName.Format("AppPath_%d",rand()%10000);
	}else{
		m_szAppName=pAppName;
	}

}


CzjlPath::~CzjlPath(void)
{
}

CStringA CzjlPath::GetAppLocalPathA()
{
	CStringA strTemp;
	GetModuleFileNameA(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	strTemp=strTemp.Mid(0,strTemp.ReverseFind('\\'));
	strTemp+="\\";
	strTemp.Replace("\\\\","\\");
	return strTemp;
}

CStringW CzjlPath::GetAppLocalPathW()
{
	CStringW strTemp;
	GetModuleFileNameW(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	strTemp=strTemp.Mid(0,strTemp.ReverseFind('\\'));
	strTemp+="\\";
	strTemp.Replace(L"\\\\",L"\\");
	return strTemp;
}


CStringA CzjlPath::GetAppExePathA()
{
	CStringA strTemp;
	GetModuleFileNameA(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	return strTemp;
}

CStringW CzjlPath::GetAppExePathW()
{
	CStringW strTemp;
	GetModuleFileNameW(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	return strTemp;
}






#define ProcessBasicInformation 0

typedef struct
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	UNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	UNICODE_STRING Unknown3;
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE           ProcessHinstance;
	PVOID               ListDlls;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

PROCNTQSIP NtQueryInformationProcess;

BOOL GetProcessCmdLine(DWORD dwId,LPWSTR wBuf,DWORD dwBufLen)
{
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwId);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	if (status)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		pbi.PebBaseAddress,
		&Peb,
		sizeof(PEB),
		&dwDummy
		)
		)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		Peb.ProcessParameters,
		&ProcParam,
		sizeof(PROCESS_PARAMETERS),
		&dwDummy
		)
		)
		goto cleanup;

	lpAddress = ProcParam.CommandLine.Buffer;
	dwSize = ProcParam.CommandLine.Length;

	if (dwBufLen<dwSize)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		lpAddress,
		wBuf,
		dwSize,
		&dwDummy
		)
		)
		goto cleanup;

	bRet = TRUE;

cleanup:

	CloseHandle (hProcess);


	return bRet;
} 



CStringA CzjlPath::GetProcessCommandLine( unsigned int PID )
{
	CStringA szResult;
	wchar_t zBuf[1024]={0};
	GetProcessCmdLine(PID,zBuf,1024);

	szResult=zBuf;
	return szResult;
}

DWORD CzjlPath::GetPID( CStringA ProcessName )
{
		HANDLE hSnapshot;
		PROCESSENTRY32 lppe;
		BOOL Find;
		BOOL Found;
		hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//�������̿���
		lppe.dwSize=sizeof(PROCESSENTRY32);
		Found=Process32First(hSnapshot,&lppe);//�õ���һ������
		while(Found)//���̲��ǿ�
		{
			if(strcmpi(ProcessName,(const char*)lppe.szExeFile)==0)//�������Ƚ�
				return lppe.th32ProcessID;//һ�������ؽ���ID
			Found = Process32Next(hSnapshot,&lppe);//�õ���һ������
		}
		CloseHandle(hSnapshot);

		return NULL;
}







CString CzjlPath::GetDeskopPath()
{
	LPITEMIDLIST ppidl = NULL; 

	CStringW strTemp;

	CoInitialize(NULL);
	if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl) == S_OK) // ���ϵͳ����·�� 
	{  
		BOOL flag = SHGetPathFromIDListW(ppidl,      // ��ָ��һ���ĵ���Ŀ¼�ص��һ����Ŀ��ʶ����ĵ�ַ
			strTemp.GetBuffer(MAX_PATH));                                             // �����ĵ�ϵͳ·���Ļ����ַ����С������MAX_PATH
		CoTaskMemFree(ppidl);  
	}
	strTemp.ReleaseBuffer();
	strTemp+=L"\\";
	strTemp.Replace(L"\\\\",L"\\");
	return strTemp;
}


bool CzjlPath::CreatePath( CStringW szPath )
{
	return SHCreateDirectory(::GetDesktopWindow(),szPath);
}

CStringW CzjlPath::GetTempPathW( CStringW szPathName )
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
