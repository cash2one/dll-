#include "zjlcommon.h"
#include <atltime.h>
#include <Psapi.h>
#pragma  comment(lib, "Psapi.lib")

#pragma warning (disable:4800 4996)
#pragma warning (disable:4995)

const int bDebug = 0;

static BOOL DosPathToNtPath(LPSTR pszDosPath, LPSTR pszNtPath)  
{  
	CHAR           szDriveStr[500];  
	CHAR           szDrive[3];  
	CHAR           szDevName[100];  
	INT             cchDevName;  
	INT             i;  

	//检查参数  
	if(!pszDosPath || !pszNtPath )  
		return FALSE;  

	//获取本地磁盘字符串  
	if(GetLogicalDriveStringsA(sizeof(szDriveStr), szDriveStr))  
	{  
		for(i = 0; szDriveStr[i]; i += 4)  
		{  
			if(!lstrcmpiA(&(szDriveStr[i]), "A:\\") || !lstrcmpiA(&(szDriveStr[i]), "B:\\"))  
				continue;  

			szDrive[0] = szDriveStr[i];  
			szDrive[1] = szDriveStr[i + 1];  
			szDrive[2] = '\0';  
			if(!QueryDosDeviceA(szDrive, szDevName, 100))//查询 Dos 设备名  
				return FALSE;  

			cchDevName = lstrlenA(szDevName);  
			if(memcmp(pszDosPath, szDevName, cchDevName) == 0)//命中  
			{  
				lstrcpyA(pszNtPath, szDrive);//复制驱动器  
				lstrcatA(pszNtPath, pszDosPath + cchDevName);//复制路径  

				return TRUE;  
			}             
		}  
	}  

	lstrcpyA(pszNtPath, pszDosPath);  

	return FALSE;  
}  

BOOL GetProcessFullPath(DWORD dwPID, CHAR pszFullPath[MAX_PATH])  
{  
	CHAR       szImagePath[MAX_PATH];  
	HANDLE      hProcess;  

	if(!pszFullPath)  
		return FALSE;  

	pszFullPath[0] = '\0';  
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);  
	if(!hProcess)  
		return FALSE;  

	if(!GetProcessImageFileNameA(hProcess, szImagePath, MAX_PATH))  
	{  
		CloseHandle(hProcess);  
		return FALSE;  
	}  
	
	if(!DosPathToNtPath(szImagePath, pszFullPath))  
	{  
		CloseHandle(hProcess);  
		return FALSE;  
	}  

	CloseHandle(hProcess);  

	return TRUE;  
}  


//REG
CStringA __stdcall GetRegItemText(HKEY hKey, LPCTSTR szFullItemName, LPCTSTR szDefault)
{
	CString strFullItemName = szFullItemName;
	CString	strSubKey, strValueName, strValue;
	DWORD dwValueType, dwValueLength;
	unsigned int dwBufferSize=1024;
	TCHAR szBuffer[1024]={0};
	
	if (strFullItemName.Right(1) == TEXT("\\"))
	{
		strSubKey = strFullItemName.Left(strFullItemName.ReverseFind(TEXT('\\')));
		strValueName = "";
	}
	else
	{
		DWORD n = strFullItemName.ReverseFind(TEXT('\\'));
		strSubKey = strFullItemName.Left(n);

		strValueName = strFullItemName;
		strValueName.Replace(strSubKey + TEXT("\\"), TEXT(""));
	}

	lstrcpyn(szBuffer, szDefault, dwBufferSize);

	HKEY hSubKey;

	RegOpenKeyEx(hKey, strSubKey, NULL, KEY_READ, &hSubKey);

	RegQueryValueEx(hSubKey, strValueName, NULL, &dwValueType, NULL, &dwValueLength);
	RegQueryValueEx(hSubKey, strValueName, NULL, &dwValueType, (byte*)strValue.GetBuffer(dwValueLength + 1), &dwValueLength);

	RegCloseKey(hSubKey);

	lstrcpyn(szBuffer, strValue, dwBufferSize);
	CStringA szResuklt=szBuffer;
	return szResuklt;
}
//DBGPRINT
CStringA g_Tip;
CzjlTextLog* g_pDbgFile=0;
void OpenColsole(CStringA szTitle)
{
	if (AllocConsole())
	{
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		  SetConsoleTitleA(szTitle);
	}
}

void SetDbgPrintFilePath( CStringA szPath,bool bIsCreateFile,unsigned int AutoSaveSec,unsigned int AutoSaveCount )
{
	CreatePathA(szPath);
	g_pDbgFile=new CzjlTextLog(szPath.GetBuffer(),bIsCreateFile,AutoSaveSec,AutoSaveCount);
}


void DbgPrint(const char *fmt, ...)
{
	if ( bDebug==0)//只有调试版输出
	{
		return;
	}
	char buf[1024*100]={0};
	va_list arg = (va_list)((char*)(&fmt) + 4); 
	vsprintf_s(buf, fmt, arg);
	CStringA sz,szbuf;
	szbuf=buf;

	int _begin=0,_end=0;
	for (unsigned int i=0;true;i++)
	{
		CStringA szOut;
		_end=szbuf.Find("\r\n",_begin);
		bool bEnd=0;
		if (_end==-1 )
		{
			if (i==0)//没有换行符
			{
				szOut.Format("=,=[%s] %s",g_Tip,szbuf);
			}else
			{//最后一行
				szOut.Format("=,=[%s] %s",g_Tip,szbuf.Mid(_begin,szbuf.GetLength()-_begin));
			}			   
			bEnd=1;
		}else{
			szOut.Format("=,=[%s] %s",g_Tip,szbuf.Mid(_begin,_end-_begin));
		}
		if (szOut.GetLength()>5)
		{
			OutputDebugStringA(szOut);
			printf("%s\r\n",szOut);
			if (g_pDbgFile)
			{
				g_pDbgFile->AddString(szOut);
			}
		}
		_begin=_end+2;
		if (bEnd)
		{
			break;
		}
	}   
}

void DbgPrintBox( const char *fmt, ... )
{
	if (bDebug==0)//只有调试版输出
	{
		return;
	}
	char buf[1024*100]={0};
	va_list arg = (va_list)((char*)(&fmt) + 4); 
	vsprintf_s(buf, fmt, arg);
	CStringA sz,szbuf;
	szbuf=buf;

	int _begin=0,_end=0;
	for (unsigned int i=0;true;i++)
	{
		CStringA szOut;
		_end=szbuf.Find("\r\n",_begin);
		bool bEnd=0;
		if (_end==-1 )
		{
			if (i==0)//没有换行符
			{
				szOut.Format("%s",szbuf);
			}else
			{//最后一行
				szOut.Format("%s",szbuf.Mid(_begin,szbuf.GetLength()-_begin));
			}			   
			bEnd=1;
		}else{
			szOut.Format("%s",szbuf.Mid(_begin,_end-_begin));
		}
		if (szOut!="")
		{
			MessageBoxA(0,szOut,"DbgBox",0);
			//  OutputDebugStringA(szOut);
			//  printf("%s\r\n",szOut);
		}
		_begin=_end+2;
		if (bEnd)
		{
			break;
		}
	}   
}

void SetDbgPrintTip( CStringA szTip )
{
	g_Tip=szTip;
}

//Path
CStringA GetAppLocalPathA()
{
	CStringA strTemp;
	GetModuleFileNameA(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	strTemp=strTemp.Mid(0,strTemp.ReverseFind('\\'));
	strTemp+="\\";
	strTemp.Replace("\\\\","\\");
	return strTemp;
}

CStringW GetAppLocalPathW()
{
	CStringW strTemp;
	GetModuleFileNameW(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	strTemp=strTemp.Mid(0,strTemp.ReverseFind('\\'));
	strTemp+="\\";
	strTemp.Replace(L"\\\\",L"\\");
	return strTemp;
}

CStringA GetAppExePathA()
{
	CStringA strTemp;
	GetModuleFileNameA(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	return strTemp;
}

CStringW GetAppExePathW()
{
	CStringW strTemp;
	GetModuleFileNameW(NULL, strTemp.GetBuffer(MAX_PATH), MAX_PATH);
	strTemp.ReleaseBuffer();
	return strTemp;
}

DWORD GetPID( CStringA ProcessName )
{
	HANDLE hSnapshot;
	PROCESSENTRY32 lppe;
	BOOL Found;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//创建进程快照
	lppe.dwSize=sizeof(PROCESSENTRY32);
	Found=Process32First(hSnapshot,&lppe);//得到第一个进程
	while(Found)//进程不是空
	{
		if(_strcmpi(ProcessName,(const char*)lppe.szExeFile)==0)//进程名比较
			return lppe.th32ProcessID;//一样即反回进程ID
		Found = Process32Next(hSnapshot,&lppe);//得到下一个进程
	}
	CloseHandle(hSnapshot);

	return NULL;
}



//////////////////////////////////////////////////////////////////////////GetProcessCommandLine
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



CStringA GetProcessCommandLine( unsigned int PID )
{
	CStringA szResult;
	wchar_t zBuf[1024]={0};
	GetProcessCmdLine(PID,zBuf,1024);

	szResult=zBuf;
	return szResult;
}
//////////////////////////////////////////////////////////////////////////GetProcessCommandLine

bool CreatePathW( CStringW szPathName )
{
	CStringW strTemp=szPathName;
	strTemp.Replace(L"\\\\",L"\\");
	return (bool)SHCreateDirectory(::GetDesktopWindow(),strTemp);
}
bool CreatePathA( CStringA szPathName )
{
	CStringW strTemp=szPathName;
	strTemp.Replace(L"\\\\",L"\\");
	return SHCreateDirectory(::GetDesktopWindow(),strTemp);
}

//String
//---------------------------------------------------------------------------
CStringA UTF8ToAnsi( const char* str )
{
	int textlen ;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ); 
	wchar_t * Unicode = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(Unicode,0,(textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)Unicode,textlen ); 


	textlen = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, Unicode, -1, NULL, 0, NULL, NULL );
	char* result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, Unicode, -1, result, textlen, NULL, NULL );


	CStringA sz=result;
	delete[] Unicode;
	delete[] result;
	return sz;
}

wchar_t * UTF8ToUnicode( const char* str )
{
	int textlen ;
	wchar_t * result;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result,0,(textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen ); 
	return result; 
}

char* UnicodeToAnsi( const wchar_t* str )
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, str, -1, NULL, 0, NULL, NULL );
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, str, -1, result, textlen, NULL, NULL );
	return result;
}
//---------------------------------------------------------------------------
char* UnicodeToUTF8( const wchar_t* str )
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
wchar_t * AnsiToUnicode( const char* str )
{
	int textlen ;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_THREAD_ACP, 0, str,-1, NULL,0 ); 
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(result,0,(textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_THREAD_ACP, 0,str,-1,(LPWSTR)result,textlen ); 
	return result; 
}

CStringA AnsiToUTF8(const char* str)
{
	WCHAR* strSrc    = NULL;
	TCHAR* szRes    = NULL;

	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL,0);

	unsigned short* wszUtf8 = new unsigned short[len+1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);

	char* szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
	delete[] wszUtf8;

	CStringA sz=szUtf8;
	delete[] szUtf8;
	return sz;
}


char* UTF8ToAnsip(const char* str)
{
	char* pResult=0;
	wchar_t* pUnicode=UTF8ToUnicode(str);
	if (pUnicode)
	{
		pResult=UnicodeToAnsi(pUnicode);
		delete[] pUnicode;
	}
	return pResult;
}

//MD5

bool MD5(char * str,unsigned int size,char * out)
{
	typedef struct {
		ULONG i[2]; 
		ULONG buf[4]; 
		unsigned char in[64]; 
		unsigned char digest[16]; 
	} MD5_CTX;
	typedef void (WINAPI* PMD5Init) (MD5_CTX *);
	typedef void (WINAPI* PMD5Update) (MD5_CTX *,const  char *, unsigned int);
	typedef void (WINAPI* PMD5Final ) (MD5_CTX *);
	MD5_CTX ctx;
	HINSTANCE hDll = LoadLibraryW(L"advapi32.dll");
	if(hDll==NULL)
		return false;
	PMD5Init MD5Init = (PMD5Init)GetProcAddress(hDll,"MD5Init");
	PMD5Update MD5Update = (PMD5Update)GetProcAddress(hDll,"MD5Update");
	PMD5Final MD5Final = (PMD5Final)GetProcAddress(hDll,"MD5Final");
	MD5Init(&ctx);
	MD5Update(&ctx,str,size);
	MD5Final(&ctx);
	for (int i = 0; i < 16; i++)
	{
		out[i * 2] = "0123456789ABCDEF"[ctx.digest[i] >> 4];
		out[i * 2 + 1] = "0123456789ABCDEF"[ctx.digest[i] & 0x0F];
	}
	out[32] = '\0';

	return true;
}


bool MD5( char* FilePath,char* out )
{
	HANDLE hFile;
	hFile=CreateFileA(FilePath,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

	if (hFile==INVALID_HANDLE_VALUE)
		return 0;
	int FileSize=GetFileSize(hFile,0);
	if (!FileSize)
	{
		CloseHandle(hFile);
		return 0;
	}
	char* pBuffer=new char[FileSize];
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return 0;
	}else{
		RtlZeroMemory(pBuffer,FileSize);
	}

	DWORD readBytes=0;
	if (ReadFile(hFile,pBuffer,FileSize,&readBytes,0)==0	)
	{
		CloseHandle(hFile);
		delete[] pBuffer;
		return 0;
	}
	CloseHandle(hFile);

	bool ret =  MD5(pBuffer,FileSize,out);
	delete[] pBuffer;

	return ret;

}

CStringA MD5( CStringA szPath )
{
	CStringA szMD5;
	MD5(szPath.GetBuffer(),szMD5.GetBuffer(1024));
	szMD5.ReleaseBuffer();
	szMD5.MakeUpper();
	DbgPrint("MD5校验[%s] md5=[%s]",szPath,szMD5);
	return szMD5;
}


//Mem
void* GetMemShare(CStringA szName,unsigned int Size)
{
	void* pResult=0;
	HANDLE hMap;
	hMap=CreateFileMappingA((HANDLE)-1,0,PAGE_READWRITE,0,Size,szName);


	int LE_CReateMap=GetLastError();

	if (hMap!=INVALID_HANDLE_VALUE)
	{
		pResult=MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0);
	}else{
		return 0;
	}

	if (pResult && LE_CReateMap!=ERROR_ALREADY_EXISTS)
	{//第一次创建
		//初始化数据
		DbgPrint("GetMemShare - 内存%s Size=%d 第一次创建,初始化成功[pMem=%08x]",szName,Size,pResult);
		RtlZeroMemory(pResult,Size);
	}else{
		DbgPrint("GetMemShare - 内存%s Size=%d 打开成功[pMem=%08x]",szName,Size,pResult);
	}
	return pResult;
}

CStringA GetWindowStrA( HWND hWnd )
{
	char szTxt[1024]={0};
	GetWindowTextA(hWnd,szTxt,1024);
	CStringA szRes=szTxt;
	return szRes;
}
BOOL __stdcall zGetProduceVersion(char* szFile, char* pszProductVersion, int nszSize)
{
	int nSize = GetFileVersionInfoSizeA(szFile, NULL);

	void * pInfo = new char[nSize];
	RtlZeroMemory(pInfo, nSize);

	if (!GetFileVersionInfoA(szFile, NULL, nSize, pInfo))
	{
		delete [] pInfo;
		return FALSE;
	}

	void * lpBuffer = NULL;
	UINT nBufferSize = 0;

	VS_FIXEDFILEINFO * pFileInfo = NULL;
	UINT nFileInfoSize = NULL;

	if (!VerQueryValue(pInfo, TEXT("\\"), (void**)&pFileInfo, &nFileInfoSize))
	{
		delete [] pInfo;
		return FALSE;
	}

	CStringA str;

	if (pFileInfo)
	{
		if (pFileInfo->dwSignature == VS_FFI_SIGNATURE)
		{
			str.Format("%d.%d.%d.%d", HIWORD(pFileInfo->dwProductVersionMS), LOWORD(pFileInfo->dwProductVersionMS), HIWORD(pFileInfo->dwProductVersionLS), LOWORD(pFileInfo->dwProductVersionLS));
			lstrcpynA(pszProductVersion, str.GetBuffer(), nszSize);
			return TRUE;
		}
	}

	if (!VerQueryValue(pInfo, TEXT("\\VarFileInfo\\Translation"), &lpBuffer, &nBufferSize))
	{
		delete [] pInfo;
		return FALSE;
	}

	WORD wLangID = NULL;
	WORD wCharSet = NULL;
	if (lpBuffer)
	{
		wLangID = LOWORD(*(DWORD*)lpBuffer);
		wCharSet = HIWORD(*(DWORD*)lpBuffer);
	}
	else
	{
		delete [] pInfo;
		return FALSE;
	}

	str.Format("\\StringFileInfo\\%.4X%.4X\\ProductVersion", wLangID, wCharSet);

	if (!VerQueryValueA(pInfo, str.GetBuffer(), &lpBuffer, &nBufferSize))
	{
		delete [] pInfo;
		return FALSE;
	}

	lstrcpynA(pszProductVersion, (LPCSTR)lpBuffer, nszSize);

	delete [] pInfo;
	return TRUE;
}
CStringA GetFileVersion( CStringA szModuleName )
{
	CStringA szPath;
	GetModuleFileNameA(GetModuleHandleA("League of Legends.exe"),szPath.GetBuffer(1024),1024);
	szPath.ReleaseBuffer();
	CStringA szVersion;
	zGetProduceVersion(szPath.GetBuffer(),szVersion.GetBuffer(100),100);
	szVersion.ReleaseBuffer();
	szVersion.Replace(" ","");
	DbgPrint("当前文件 %s 版本 : %s",szModuleName,szVersion);
	return szVersion;
}


CStringA PrintBytes( char* pFlag,char* pStart,unsigned int NumBytes )
{
	if (bDebug==0)//只有调试版输出
	{
		return "";
	}
	CStringA szBytes;
	szBytes.Format("PrintMem Tag=[%s]  pStart=[0x%08x]  Len=[0x%08x] Byte \r\n",pFlag,pStart,NumBytes);
	for (unsigned int i=0;i<NumBytes;i++)
	{
		int By=*((char*)(pStart)+i) & 0xFF;

		CStringA szTemp;
		szTemp.Format(" %02x",By);

		if ((i+1)%16==0)
		{
			szTemp+="    ";
			szTemp+=pFlag;
			if (i+1<NumBytes)
			{
				szTemp+=" \r\n";
			}
			
		}else{
			if ((i+1)%4==0)
			{
				szTemp+=" | ";
			}
		}
		szBytes+=szTemp;
	}
	szBytes+="    ";
	szBytes+=pFlag;
	return szBytes;
}

CStringA PrintHexEX()
{
	int _EAX,_EBX,_ECX,_EDX,_ESI,_EDI,_EBP,_ESP;
	__asm{
		mov _EAX,eax;
		mov _EBX,ebx;
		mov _ECX,ecx;
		mov _EDX,edx;
		mov _ESI,esi;
		mov _EDI,edi;
		mov _EBP,ebp;
		mov _ESP,esp;
	}

	CStringA sz;
	sz.Format("Eax=0x%08x,Ebx=0x%08x,Ecx=0x%08x,Edx=0x%08x\r\nESI=0x%08x,_EDI=0x%08x,_EBP=0x%08x,_ESP=0x%08x\r\n",_EAX,_EBX,_ECX,_EDX,_ESI,_EDI,_EBP,_ESP);

	return sz;

}


int GetSystemVersion()
{
	/*
	Operating system Version number dwMajorVersion  dwMinorVersion  Other 
	Windows 7 6.1 6 1 OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION 
	Windows Server 2008 R2 6.1 6 1 OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION 
	Windows Server 2008 6.0 6 0 OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION 
	Windows Vista 6.0 6 0 OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION 
	Windows Server 2003 R2 5.2 5 2 GetSystemMetrics(SM_SERVERR2) != 0 
	Windows Home Server 5.2 5 2 OSVERSIONINFOEX.wSuiteMask == VER_SUITE_WH_SERVER 
	Windows Server 2003 5.2 5 2 GetSystemMetrics(SM_SERVERR2) == 0 
	Windows XP Professional x64 Edition 5.2 5 2 (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) 
	Windows XP 5.1 5 1 Not applicable 
	Windows 2000 5.0 5 0 Not applicable
	*/
	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX); 

	if(GetVersionEx((OSVERSIONINFO *)&os))
	{
		DbgPrint("GetSystemVersion %d",os.dwMajorVersion);
		return os.dwMajorVersion;//5xp,6win7
	}else{
		return 0;
	}
}

bool IsXpSystem()
{
	if (GetSystemVersion()==5)
	{
		return 1;
	}else{
		return 0;
	}
}

bool IsWin7System()
{
	if (GetSystemVersion()==6)
	{
		return 1;
	}else{
		return 0;
	}
}

CStringA GetAppTempPathA(CStringA szTempFloderName)
{
	CStringA szResult;
	GetTempPathA(MAX_PATH,szResult.GetBuffer(MAX_PATH));
	szResult.ReleaseBuffer();
	szResult+="\\";
	szResult+=szTempFloderName;
	szResult.Replace("\\\\","\\");
	bool b=CreatePathA(szResult);
	if (b)
	{
		DbgPrint("临时目录 %s 创建成功",szResult);
	}else{
		DbgPrint("临时目录 %s 创建失败",szResult);
	}
	
	return szResult;
}

void GetLastErrorEx()
{
	DWORD dwMessageID = ::GetLastError();
	char *szMessage = NULL;
	::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwMessageID, 0, (LPSTR) &szMessage, 0, NULL);
	CStringA szTitle;
	szTitle.Format("::GetLastError() = %d", dwMessageID);

	DbgPrint("%s [%s]",szTitle,szMessage);
	::MessageBoxA(NULL, szMessage, szTitle, MB_OK);
}


//Class
	//File


CzjlFile::CzjlFile( CStringW strPath )
{
	m_strPath=strPath;
	m_pBuffer=0;
	if (!PathFileExistsW(m_strPath))
	{
		HANDLE hFile=CreateFileW(m_strPath,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(hFile);
	}
}

CzjlFile::CzjlFile( CStringA szPath )
{
	m_strPath=szPath;
	m_pBuffer=0;
	if (!PathFileExistsA(szPath))
	{
		HANDLE hFile=CreateFileA(szPath,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(hFile);
	}
}

CzjlFile::CzjlFile( char* szPath )
{
	m_strPath=szPath;
	m_pBuffer=0;
	if (!PathFileExistsA(szPath))
	{
		HANDLE hFile=CreateFileA(szPath,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(hFile);
	}
}

CzjlFile::CzjlFile( wchar_t* wszPath )
{
	m_strPath=wszPath;
	m_pBuffer=0;
	if (!PathFileExistsW(m_strPath))
	{
		HANDLE hFile=CreateFileW(m_strPath,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		CloseHandle(hFile);
	}
}




CzjlFile::~CzjlFile(void)
{
	Clear();
}

unsigned int  CzjlFile::LoadFile(char* pBuffer,unsigned int fileSize)
{
	HANDLE hFile;
	if (pBuffer!=0 && fileSize!=0)
	{//读入指定缓冲区

		hFile=CreateFileW(m_strPath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

		if (hFile==INVALID_HANDLE_VALUE)
			return 0;

		DWORD readBytes=0;
		ReadFile(hFile,pBuffer,fileSize,&readBytes,0);
		CloseHandle(hFile);

		return readBytes;
	}

	//读到内置缓冲区

	if (m_pBuffer  )
	{//加载过
		return 1;
	}


	hFile=CreateFileW(m_strPath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

	if (hFile==INVALID_HANDLE_VALUE)
		return 0;


	m_FileSize=GetFileSize(hFile,0);
	if (!m_FileSize)
	{
		CloseHandle(hFile);
		return 0;
	}

	m_pBuffer=new char[m_FileSize];
	if (!m_pBuffer)
	{
		CloseHandle(hFile);
		return 0;
	}else{
		RtlZeroMemory(m_pBuffer,m_FileSize);
	}

	DWORD readBytes=0;
	ReadFile(hFile,m_pBuffer,m_FileSize,&readBytes,0);
	CloseHandle(hFile);

	return readBytes;
}

void CzjlFile::LoadStringFileA( CStringA& szBuffer )
{
	if (			LoadFile()&&m_pBuffer	)
	{
		memcpy(szBuffer.GetBuffer(m_FileSize),m_pBuffer,m_FileSize);
		Clear();
		szBuffer.ReleaseBuffer(m_FileSize);
		return ;
	}
}

void CzjlFile::LoadStringFileW( CStringW& strBuffer )
{
	if (			LoadFile()&&m_pBuffer	)
	{
		memcpy(strBuffer.GetBuffer(m_FileSize),m_pBuffer,m_FileSize);
		Clear();
		strBuffer.ReleaseBuffer(m_FileSize);
		return ;
	}
}

void CzjlFile::Clear()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer=0;
	}
}

void CzjlFile::WriteStringA( CStringA& szBuffer )
{
	SaveFile(szBuffer.GetBuffer(),szBuffer.GetLength());
}

void CzjlFile::WriteStringW( CStringW& strBuffer )
{
	SaveFile((char*)strBuffer.GetBuffer(),strBuffer.GetLength()*2);
}

unsigned int CzjlFile::SaveFile(char* pBuffer,unsigned int FileSize)
{
	HANDLE hFile;

	hFile=CreateFileW(m_strPath,GENERIC_WRITE,0,0,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

	if (hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	//清零
	SetFilePointer(hFile,0,0,FILE_BEGIN);

	DWORD WriteBytes;
	if (	! WriteFile(hFile,pBuffer,FileSize,&WriteBytes,0) )
	{
		CloseHandle(hFile);
		return 0;
	}else{
		CloseHandle(hFile);
		return WriteBytes;
	}

}

void CzjlFile::SetFileStatus( int attribute )
{
	::SetFileAttributesW(m_strPath,attribute);
}

int CzjlFile::GetFileStatus()
{
	return ::GetFileAttributesW(m_strPath);
}

int CzjlFile::FileSize()
{
	HANDLE hFile=CreateFileW(m_strPath,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	int size= GetFileSize(hFile,0);
	CloseHandle(hFile);
	return size;
}

char* CzjlFile::GetFileBuffer( unsigned int* pFileSize )
{

	unsigned int readBytes=LoadFile();
	if (pFileSize)
	{
		*pFileSize=readBytes;
	}

	if (	readBytes	)
	{
		return m_pBuffer;
	}
	return 0;
}


CString CzjlFile::GetFileVersionStr()
{
	CStringA szFile=m_strPath;
	CString strVersion;

	DWORD szLen=GetFileVersionInfoSizeA(szFile,0);
	char* szVersion=new char[szLen+128];
	GetFileVersionInfoA(szFile,0,szLen,szVersion);

	for (unsigned int i=0;i<szLen;i++)
	{
		if (!	StrCmpW((wchar_t*)&szVersion[i],L"Version")	)
		{
			strVersion=(wchar_t*)&(szVersion[i+18]);
			break;
		}
	}
	if (		szVersion)
	{
		delete	[]szVersion;
	}

	return strVersion;
}

unsigned int CzjlFile::GetFileVersion()
{
	CStringA szFile=m_strPath;
	CStringW strVersion;
	DWORD Version=0;

	DWORD szLen=GetFileVersionInfoSizeA(szFile,0);
	char* szVersion=new char[szLen+128];
	if (!szVersion)
		return 0;
	GetFileVersionInfoA(szFile,0,szLen,szVersion);

	for (unsigned int i=0;i<szLen;i++)
	{
		if (!	StrCmpW((wchar_t*)&szVersion[i],L"Version")	)
		{
			strVersion=(wchar_t*)&(szVersion[i+18]);
			break;
		}
	}
	if (szVersion	)
	{
		delete	[]szVersion;
	}

	if (strVersion.IsEmpty())
		return 0;

	int begin=strVersion.ReverseFind('.');
	if (begin==-1)
		return 0;

	strVersion=strVersion.Mid(begin+1,strVersion.GetLength());
	Version=_wtoi(strVersion);

	return Version;
}

void CzjlFile::OpenExplorer()
{
	ShellExecuteW(0,L"open",m_strPath,0,0,SW_SHOW);
}
	//Log
CzjlTextLog::CzjlTextLog(CStringA szPathFileName,bool bIsCreateFile,unsigned int AutoSaveSec,unsigned int AutoSaveCount)
{
	m_AddCount=0;
	m_hWnd=0;


	m_bIsCreateFile=bIsCreateFile;
	if (AutoSaveSec==0)
	AutoSaveSec=1;
	m_AutoSaveSec=AutoSaveSec;
	m_AutoSaveCount=AutoSaveCount;
	GetModuleFileNameA(NULL, m_szLogPath.GetBuffer(MAX_PATH), MAX_PATH);
	m_szLogPath.ReleaseBuffer();
	m_szLogPath.MakeLower();

	//路径
	if (szPathFileName=="")
	{
		m_szLogPath=m_szLogPath;
		m_szLogPath+=".log";
	}else{
		if (szPathFileName.Find(".")!=-1)
		{//绝对路径
			m_szLogPath=szPathFileName;
		}else{//文件夹路径
			CreatePathA(szPathFileName);
			time_t tt;time(&tt);
			CTime time=tt;
			CStringA szFile;
			m_szLogPath.Format("%s\\%d%02d%02d %02d-%02d-%02d.log",szPathFileName,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
			m_szLogPath.Replace("\\\\","\\");
		}

		
	}
	DbgPrint("日志文件已生成[%s]",m_szLogPath);
	if (m_bIsCreateFile)
	{//创建新文件
		DeleteFileA(m_szLogPath);
	}
	Load();
	CreateThread(0,0,ThreadAutoSaveLog,(LPVOID)this,0,0);
}


CzjlTextLog::~CzjlTextLog(void)
{
	Save();
}

void CzjlTextLog::Load()
{
	Clear();
	CzjlFile file(m_szLogPath);
	file.LoadStringFileA(m_szBuffer);



	int pos=0;
	while(1)
	{
		pos=m_szBuffer.Find("\r\n",pos);
		if (pos!=-1)
		{
			m_AddCount++;
			pos+=2;
		}else{
			break;
		}
	}
}

void CzjlTextLog::Save()
{
	if (m_bIsCreateFile==0)
		return;

	m_CS.Lock();
	CzjlFile file(m_szLogPath);
	file.WriteStringA(m_szBuffer);
	m_CS.Unlock();
}

void CzjlTextLog::AddByte(CStringA sz,char* pBuffer,unsigned int _size)
{
	CStringA szBytes,szTemp;
	for (unsigned int i=0;i<_size;i++)
	{
		int ch=pBuffer[i];
		ch&=0xFF;
		szTemp.Format("0x%x,",ch);
		szBytes+=szTemp;
	}
	CStringA szOut;
	szOut.Format("%s [%s]",sz,szBytes);
	AddString(szOut);
}


void CzjlTextLog::AddString( CStringA szString )
{
	m_CS.Lock();
	CStringA szTemp;
	time_t _time=0;
	time(&_time);
	CTime T(_time);

	if (szString.Right(2)=="\r\n")
	{
		szString=szString.Mid(0,szString.GetLength()-2);
	}

	szTemp.Format("(%04d) %d-%02d-%02d %02d:%02d:%02d  %s \r\n",m_AddCount+1,T.GetYear(),T.GetMonth(),T.GetDay(),T.GetHour(),T.GetMinute(),T.GetSecond(),szString);
	m_szBuffer+=szTemp;
	//自动保存
	unsigned int countT=InterlockedExchangeAdd(&m_AddCount,1);
	if (countT%m_AutoSaveCount==0)//添加10次就保存
	{
		Save();
	}


	Update();
	m_CS.Unlock();

}

void CzjlTextLog::AddString( CStringA szString,long long FlagNumber )
{
	CStringA sz;
	sz.Format("%s  ( %I64u )",szString,FlagNumber);
	AddString(sz);
}


DWORD WINAPI CzjlTextLog::ThreadAutoSaveLog( LPVOID lpThreadParameter )
{
	CzjlTextLog* p=(CzjlTextLog*)lpThreadParameter;
	while(1)
	{
		Sleep(1000 *p->m_AutoSaveSec);//5秒
		p->Save();
	}
}

void CzjlTextLog::BindWindow( HWND hWnd )
{
	if (IsWindow(hWnd))
	{
		m_hWnd=hWnd;
	}
}

void CzjlTextLog::Clear()
{
	InterlockedExchange(&m_AddCount,0);
	m_szBuffer="";
}

void CzjlTextLog::Update()
{
	if (m_hWnd)
	{
		//字数太多添加最后的
		if (m_szBuffer.GetLength()>1000)//5千字
		{
			CStringA szTemp=m_szBuffer.Right(10000);
			SetWindowTextA(m_hWnd,szTemp);
		}else{
			SetWindowTextA(m_hWnd,m_szBuffer);
		}
		unsigned int Index=SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0);
		SendMessage(m_hWnd, EM_LINESCROLL, 0, Index); 
	}
}

void CzjlTextLog::AddStringLast( CStringA szString )
{
	m_CS.Lock();
	if (m_szBuffer.Right(2)=="\r\n")
	{
		m_szBuffer=m_szBuffer.Mid(0,m_szBuffer.GetLength()-2);
	}
	m_szBuffer+=szString+"\r\n";
	Update();
	m_CS.Unlock();
}

bool IsExsitDirOrFile(const char* fullPath)
{
	if (_access(fullPath, 0) == 0)
		return true;
	else
		return false;
}

std::string  GetPEVersion(const std::string & Path)
{
	const char *szFile = Path.c_str();
	if (Path.length() == 0 || PathFileExistsA(Path.c_str()) == 0)
		return "";

	int nSize = GetFileVersionInfoSizeA(szFile, NULL);

	char buf[256] = { 0 };
	void * pInfo = new char[nSize];
	if (pInfo == 0)		return "";
	RtlZeroMemory(pInfo, nSize);

	if (!GetFileVersionInfoA(szFile, NULL, nSize, pInfo))
	{
		delete[] pInfo;
		return "";
	}

	void * lpBuffer = NULL;
	UINT nBufferSize = 0;

	VS_FIXEDFILEINFO * pFileInfo = NULL;
	UINT nFileInfoSize = NULL;

	if (!VerQueryValue(pInfo, TEXT("\\"), (void**)&pFileInfo, &nFileInfoSize))
	{
		delete[] pInfo;
		return "";
	}

	if (pFileInfo)
	{
		if (pFileInfo->dwSignature == VS_FFI_SIGNATURE)
		{
			sprintf((char*)buf, "%d.%d.%d.%d", HIWORD(pFileInfo->dwProductVersionMS), LOWORD(pFileInfo->dwProductVersionMS), HIWORD(pFileInfo->dwProductVersionLS), LOWORD(pFileInfo->dwProductVersionLS));
			delete[] pInfo;
			return buf;
		}
	}

	if (!VerQueryValue(pInfo, TEXT("\\VarFileInfo\\Translation"), &lpBuffer, &nBufferSize))
	{
		delete[] pInfo;
		return "";
	}

	WORD wLangID = NULL;
	WORD wCharSet = NULL;
	if (lpBuffer)
	{
		wLangID = LOWORD(*(DWORD*)lpBuffer);
		wCharSet = HIWORD(*(DWORD*)lpBuffer);
	}
	else
	{
		delete[] pInfo;
		return "";
	}

	sprintf(buf, "\\StringFileInfo\\%.4X%.4X\\ProductVersion", wLangID, wCharSet);

	if (!VerQueryValueA(pInfo, buf, &lpBuffer, &nBufferSize))
	{
		delete[] pInfo;
		return "";
	}

	lstrcpynA(buf, (LPCSTR)lpBuffer, sizeof(buf));

	delete[] pInfo;
	return buf;
}