#include "CzjlHook.h"
#include <tlhelp32.h>

bool CzjlHook::HookDllAddr( CStringA szDll,char* pTag,unsigned int LenTag,char* pReplace,unsigned int LenReplace, int PosStartTag/*=0*/ )
{
#ifdef _DEBUG
	zjlDebugOut("CzjlHook::HookDllAddr Begin");
	zjlDebugOut(szDll);
#endif
	bool Result=0;
	unsigned int  hDllOffset=(unsigned int)GetModuleHandleA(szDll);
	if (hDllOffset)
	{
		//获取DLL代码段
		unsigned int CodeStart=0,CodeEnd=0;
		unsigned int SearchS=hDllOffset;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{
				if (st.Protect==PAGE_EXECUTE_READ)
				{
					CodeStart=SearchS;
					CodeEnd=SearchS+st.RegionSize;
					break;
				}
				SearchS+=st.RegionSize;
			}
		}
#ifdef _DEBUG
		zjlDebugOut("dll代码段起始地址",CodeStart);
		zjlDebugOut("dll代码段长度",CodeEnd-CodeStart);
#endif
		//查找标志位
		unsigned int searchPos=CodeStart;
		while(searchPos<CodeEnd-LenTag)
		{
			if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
				break;
			searchPos++;
		}
#ifdef _DEBUG
		zjlDebugOut("searchPos",searchPos);
#endif
		//替换
		if (searchPos<CodeEnd-LenTag)
		{

			if (	ReplaceMem((char*)(searchPos+PosStartTag),pReplace,LenReplace))
			{
#ifdef _DEBUG
				zjlDebugOut("替换成功,LenReplace=",LenReplace);
#endif
				Result=1;
			}
		}
	}
#ifdef _DEBUG
	zjlDebugOut("CzjlHook::HookDllAddr End=",Result);
#endif
	return Result;
}

vector<char*> CzjlHook::GetMemPtrByRegionSize( unsigned int RegionSize,int MemType )
{
	vector<char*> vResult;
	int Address=0;

	while(1)
	{
		MEMORY_BASIC_INFORMATION  st={0};
		if (!VirtualQuery((void*)Address,&st,sizeof(st)))
		{
			return vResult;
		}

		if(st.RegionSize==RegionSize)
		{
			if (MemType)
			{
				if(st.Protect==MemType)
				vResult.push_back((char*)Address);
			}else{
				vResult.push_back((char*)Address);
			}
		}
		int oldptr = Address;
		Address = (DWORD)st.BaseAddress + st.RegionSize;
		if(Address <= oldptr)
			break;
	}
	return vResult;
}

unsigned int CzjlHook::GetMemSizeFromPtr(char* pMem )
{
	MEMORY_BASIC_INFORMATION  st={0};
	if (!VirtualQuery((void*)pMem,&st,sizeof(st)))
	{ //PAGE_EXECUTE_READWRITE 
		return 0;
	}else{
		return (int)st.AllocationBase+st.RegionSize-(int)pMem; //Protect==PAGE_EXECUTE_READ时为代码
	}
}



bool CzjlHook::ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size )
{
	DWORD dwOldFlag;
	HANDLE hProc = GetCurrentProcess();
	if(VirtualProtectEx(hProc,lpAddr,Size,PAGE_EXECUTE_READWRITE,&dwOldFlag)) 
	{  
		memcpy(lpAddr,pBuffer,Size);
		DWORD dwNew;
		VirtualProtectEx(hProc,lpAddr,Size,dwOldFlag,&dwNew);
		return TRUE;
	}else{
		return 0;
	}
}



#define ProcessBasicInformation 0
typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
static PROCNTQSIP g_NtQueryInformationProcess=0;
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


bool CzjlHook::GetProcessCmdLine( DWORD dwId,wchar_t* wBuf,DWORD dwBufLen )
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


	//Init
	if (	g_NtQueryInformationProcess==0)
	{
		g_NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandleA("ntdll"),
			"NtQueryInformationProcess"
			);

		if (!g_NtQueryInformationProcess)
			return 0;
	}



	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwId);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = g_NtQueryInformationProcess( hProcess,
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

unsigned int CzjlHook::FindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen)
{
	unsigned int  hDllOffset=0;
	unsigned int Result=0;
	szDll.MakeLower();
	szTarget.MakeLower();
#ifdef _DEBUG
	zjlDebugOut("CzjlHook::FindDllTarget Begin : "+szDll+" - "+szTarget);
	/*	zjlDebugOutHex(szDll+"Offset",hDllOffset);*/
#endif
	if (szDll.Find(":")!=-1 && PathFileExistsA(szDll))
	{//完整路径
#ifdef _DEBUG
		zjlDebugOut("FindDllTarget "+szDll+" - "+"匹配完整路径");
#endif
		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		bool bRet=Module32First(hModuleSnap,&me32);

		
		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll==szDll)
			{
				hDllOffset=(unsigned int)me32.hModule;
#ifdef _DEBUG
				zjlDebugOutHex("FindDllTarget "+szDll+" - "+"完整路径匹配成功",hDllOffset);
#endif
				break;
			}
		}
		if (hDllOffset==0)
		{
#ifdef _DEBUG
			zjlDebugOut("FindDllTarget "+szDll+" - "+"匹配完整路径失败");
#endif
			return 0;
		}
	}else if (szTarget.GetLength()>0)
	{//相对路径
#ifdef _DEBUG
		zjlDebugOut("FindDllTarget "+szDll+" - "+"匹配相对路径");
#endif
		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		bool bRet=Module32First(hModuleSnap,&me32);
		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll.Find(szTarget)!=-1 && szTheDll.Find(szDll)!=-1)
			{
				hDllOffset=(unsigned int)me32.hModule;
#ifdef _DEBUG
				zjlDebugOutHex("FindDllTarget "+szDll+" - "+"匹配相对路径成功",hDllOffset);
#endif
				break;
			}
		}

		if (hDllOffset==0)
		{
#ifdef _DEBUG
			zjlDebugOut("FindDllTarget "+szDll+" - "+"匹配相对路径失败");
#endif
			return 0;
		}
	}
	
	if (hDllOffset==0)
	{
		 hDllOffset=(unsigned int)GetModuleHandleA(szDll);
#ifdef _DEBUG
		 zjlDebugOutHex("FindDllTarget "+szDll+" - "+"通过GetModuleHandleA查找起始位置",hDllOffset);
#endif
	}

	zjlDebugOutHex("hDllOffset",(int)hDllOffset);//
	if (hDllOffset)
	{
		//获取DLL代码段
		unsigned int CodeStart=0,CodeEnd=0,CodeSize=0;
		unsigned int SearchS=hDllOffset;
		//计算代码段长度
		bool bCodeing=0;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{			
				if (st.Protect&PAGE_EXECUTE || st.Protect&PAGE_EXECUTE_READ || st.Protect&PAGE_EXECUTE_READWRITE ||st.Protect&PAGE_EXECUTE_WRITECOPY)
				{//代码段
						if (bCodeing==0)
						{//开始记录
							bCodeing=1;
							CodeStart=(int)st.BaseAddress;
						}
						CodeSize+=st.RegionSize;
						break;
				}else{//不是代码段了
					if (bCodeing==1)
					{//结束代码段
						break;
					}
				}
				SearchS+=st.RegionSize;
			}
		}

		if (CodeSize==0)
		{
#ifdef _DEBUG
			zjlDebugOutHex(szDll+"没有找到代码段",CodeStart);
#endif
		}

		//查找标志位
		unsigned int searchPos=CodeStart;
		if (SearchLen)
		{//用指定长度
			CodeEnd=CodeStart+SearchLen-LenTag;
		}else{//用默认长度
			CodeEnd=CodeStart+CodeSize-LenTag;
		}
#ifdef _DEBUG
		zjlDebugOutHex(szDll+"搜索起始地址",searchPos);
		zjlDebugOutHex(szDll+"搜索长度",CodeSize);
#endif
		while(searchPos<CodeEnd)
		{
			if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
				break;
			searchPos++;
		}
#ifdef _DEBUG
//		zjlDebugOutHex("searchPos=",searchPos);
#endif
		//找到
		if (searchPos<CodeEnd-LenTag)
		{
			Result=searchPos;
		}
	}
#ifdef _DEBUG
	zjlDebugOutHex("CzjlHook::FindDllTarget End=",Result);
#endif
	return Result;
}

vector<unsigned int> CzjlHook::SearchMemTargetFromSection(char* pStart,char* pTag,unsigned int LenTag,unsigned int SearchLen/*=0*/ )
{
#ifdef _DEBUG
	//zjlDebugOut("CzjlHook::FindMemTarget Begin");
#endif
		vector<unsigned int> vResult;
		unsigned int CodeStart,CodeEnd=0,CodeSize=0;
		CodeStart=(int)pStart;
		CodeSize=GetMemSizeFromPtr(pStart);
		CodeEnd=CodeStart+CodeSize-LenTag;

		if (CodeSize==0)
		return vResult;
		
		//开始搜索
		unsigned int searchPos=CodeStart;

		while(searchPos<CodeEnd)
		{
			if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
			{//纪录
				vResult.push_back(searchPos);
				searchPos+=LenTag;
				continue;
			}
			searchPos++;
		}

#ifdef _DEBUG
	//zjlDebugOut("CzjlHook::FindMemTarget Find",vResult.size());
#endif
	return vResult;
}

unsigned int CzjlHook::GetMemLastSizeFromPtr( char* pMem )
{
	MEMORY_BASIC_INFORMATION  st={0};
	if (!VirtualQuery((void*)pMem,&st,sizeof(st)))
	{ //PAGE_EXECUTE_READWRITE 
		return 0;
	}else{
		return (int)st.BaseAddress+st.RegionSize-(int)pMem; //Protect==PAGE_EXECUTE_READ时为代码
	}
}

unsigned int CzjlHook::FindMemTargetFromStart( char* pStart,char* pEnd,char* pTag,unsigned int LenTag/*=0*/ )
{
#ifdef _DEBUG
	zjlDebugOut("CzjlHook::FindMemTargetFromStart Find");
#endif
	unsigned int Start,End;
	Start=(int)pStart;
	End=(int)pEnd;
	unsigned int SearchPos=Start;
//	DeleteFileA("C:\\test.txt");
		while(SearchPos<End)
		{
//zjlDebugBoxOutHex("Begin VirtualQuery  SearchPos",SearchPos);
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchPos,&st,sizeof(st)))
			{
				return 0;
			}

			//跳过dll自己
			if (st.Protect==PAGE_READWRITE && st.State==MEM_COMMIT  && (pTag<st.BaseAddress || unsigned int(pTag)>(unsigned int)(st.BaseAddress)+st.RegionSize))
			{
#ifdef _DEBUG
	//		zjlDebugOutHex("SearchMemTargetFromSection SearchPos",(int)SearchPos);
#endif
/*
				CzjlFile file("C:\\Test.txt");
				CStringA szFile;
				file.LoadStringFileA(szFile);
				CStringA sz;
				sz.Format("BaseAddress: 0x%x,RegionSize : 0x%x,Type: 0x%x\r\n",st.BaseAddress,st.RegionSize,st.Type);
				szFile+=sz;
				file.WriteStringA(szFile);*/

				unsigned int Now=(int)st.BaseAddress;
				while(Now<unsigned int(st.BaseAddress) + st.RegionSize-LenTag)
				{
					if (    memcmp(pTag, (char*)Now,LenTag)==0)
					{//纪录
#ifdef _DEBUG
						zjlDebugOutHex("BaseAddress",(int)st.BaseAddress);
						zjlDebugOutHex("RegionSize",(int)st.RegionSize);
						zjlDebugOutHex("State",(int)st.State);
						zjlDebugOutHex("Type",(int)st.Type);
#endif
/*
					sz.Format("FindPt 0x%x",Now);
					szFile+=sz;
						file.WriteStringA(szFile);*/
						return Now;
					}
					Now++;
				}
			}

			unsigned int oldptr = SearchPos;
			SearchPos = (DWORD)st.BaseAddress + st.RegionSize;
			if(SearchPos <= oldptr)
				break;
		}


		return 0;
}

BOOL CzjlHook::EnableDebugPrivilege()
{
	HANDLE   hToken;    
	BOOL   fOk=FALSE;    
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))    
	{    
		TOKEN_PRIVILEGES   tp;    
		tp.PrivilegeCount=1;    
		if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid)) 
		{ 

		} 
		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;    
		if(!AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL)) 
		{ 

		} 
		fOk = (GetLastError()==ERROR_SUCCESS) ;    
		CloseHandle(hToken);    
	}    
	return   fOk;    
}

unsigned int CzjlHook::FindMemTargetFromSection( char* pStart,char* pTag,unsigned int LenTag )
{
#ifdef _DEBUG
	//zjlDebugOut("CzjlHook::FindMemTarget Begin");
#endif
	unsigned int CodeStart,CodeEnd=0,CodeSize=0;
	CodeStart=(int)pStart;
	CodeSize=GetMemSizeFromPtr(pStart);
	CodeEnd=CodeStart+CodeSize;
	if (CodeSize==0)
		return 0;

	//开始搜索
	unsigned int searchPos=CodeStart;

	while(searchPos<CodeEnd-LenTag)
	{
		if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
		{//纪录
			return searchPos;
		}
		searchPos++;
	}

#ifdef _DEBUG
	//zjlDebugOut("CzjlHook::FindMemTarget Find",vResult.size());
#endif
	return 0;
}

unsigned int CzjlHook::GetRegionSizeFromPtr( char* pMem )
{
	MEMORY_BASIC_INFORMATION  st={0};
	if (!VirtualQuery((void*)pMem,&st,sizeof(st)))
	{ //PAGE_EXECUTE_READWRITE 
		return 0;
	}else{
		return st.RegionSize; //Protect==PAGE_EXECUTE_READ时为代码
	}
}

char* CzjlHook::GetMemBySize( char* pStart,unsigned int RegionSizeMin,unsigned int RegionSizeMax,int MemType/*=0*/,unsigned int MemSearchCount/*=30*/ )
{
	vector<char*> vResult;
	unsigned int Address=(int)pStart;

	unsigned int Count=0;
	while(Count<MemSearchCount)
	{
		Count++;
		MEMORY_BASIC_INFORMATION  st={0};
		if (!VirtualQuery((void*)Address,&st,sizeof(st)))
		{
			return 0;
		}
//zjlDebugBoxOutHex("SIZE",(int)st.RegionSize);
		if(st.RegionSize>=RegionSizeMin && st.RegionSize<=RegionSizeMax)
		{
			if (MemType)
			{
				if(st.Protect==MemType)
					return (char*)st.AllocationBase;
			}else{
				return (char*)st.AllocationBase;
			}
		}
		unsigned int oldptr = Address;
		Address = (DWORD)st.BaseAddress + st.RegionSize;
		if(Address <= oldptr)
			break;
	}
	return 0;
}

int CzjlHook::GetMainThreadID()
{
	DWORD dwProcessID, dwThreadID;

		dwThreadID = 0;
		dwProcessID=GetCurrentProcessId();

		THREADENTRY32 te32 = {sizeof(te32)};
		HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
		if( Thread32First( hThreadSnap, &te32) )
		{
			do{
				if( dwProcessID == te32.th32OwnerProcessID )
				{
					dwThreadID = te32.th32ThreadID;
					break;
				}
			}while( Thread32Next( hThreadSnap, &te32) );
		}
		

		return dwThreadID;

}

HMODULE CzjlHook::GetDllByTragetStr( CStringA szDll,CStringA szTarget )
{
	HMODULE hDllOffset=0;
	MODULEENTRY32 me32={0};
	me32.dwSize=sizeof(MODULEENTRY32);
	HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	int i=0;
	bool bRet=Module32First(hModuleSnap,&me32);

	while(bRet)
	{ 
		i+=1; 
		bRet=Module32Next(hModuleSnap,&me32);
		CStringW strTheDll=me32.szExePath;
		CStringA szTheDll=strTheDll;
		szTheDll.MakeLower();
#ifdef _DEBUG
		//	zjlDebugOut(szDll+" Is Target");
		//	zjlDebugOutHex(szTheDll+" Offset : ",(int)me32.hModule);
#endif
		if (szTheDll.Find(szDll)!=-1 && szTheDll.Find(szTarget))
		{
			hDllOffset=me32.hModule;
			break;
		}
	}
return hDllOffset;
}

int CzjlHook::GetSystemVersion()
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
		  return os.dwMajorVersion;//5xp,6win7
	  }else{
		  return 0;
	  }
}

bool CzjlHook::IsXpSystem()
{
	if (GetSystemVersion()==5)
	{
		return 1;
	}else{
		return 0;
	}
}

bool CzjlHook::IsWin7System()
{
	if (GetSystemVersion()==6)
	{
		return 1;
	}else{
		return 0;
	}
}

bool CzjlHook::FindHookCallBack_JMP( CStringA szTargetDll,CStringA szHookDll )
{
	bool Result=0;
	//HOOK模块的代码范围
	unsigned int hHookDllOffset=(unsigned int)GetModuleHandleA(szHookDll);
	unsigned int CallBackAddrStart=0,CallBackAddrEnd=0;
	if (hHookDllOffset)
	{
		//获取DLL代码段
		unsigned int SearchS=hHookDllOffset;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{
				if (st.Protect==PAGE_EXECUTE_READ)
				{
					CallBackAddrStart=SearchS;
					CallBackAddrEnd=SearchS+st.RegionSize;
					break;
				}
				SearchS+=st.RegionSize;
			}
		}
	}else{
		return Result;
	}

	if (CallBackAddrStart==0 || CallBackAddrEnd==0)
	return 0;


	//被查找的模块
	unsigned int  hDllOffset=(unsigned int)GetModuleHandleA(szTargetDll);
	if (hDllOffset)
	{
		//获取DLL代码段
		unsigned int CodeStart=0,CodeEnd=0;
		unsigned int SearchS=hDllOffset;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{
				if (st.Protect==PAGE_EXECUTE_READ)
				{
					CodeStart=SearchS;
					CodeEnd=SearchS+st.RegionSize;
					break;
				}
				SearchS+=st.RegionSize;
			}
		}

		if (CodeStart==0 || CodeEnd==0)
			return 0;


		//查找标志位
		unsigned int searchPos=CodeStart;
		while(searchPos<CodeEnd-5)
		{
			//JMP偏移
			 int JmpMin=CallBackAddrStart-searchPos+5;
			 int JmpMax=CallBackAddrEnd-searchPos+5;

			 
			 if (*(char*)searchPos==0xE9)
			 {//E9
				int CmpAddr= *(int*)(searchPos+1);
				if (CmpAddr>=JmpMin && CmpAddr<=JmpMax)
				{
					CStringA sz;
					sz.Format("FindHookAddr : 0x%08x,JmpTo : 0x%08x  ",searchPos,CmpAddr+searchPos);
					MessageBoxA(0,sz,0,0);
				}
			 }


			searchPos++;
		}

	}
	return Result;
}

bool CzjlHook::HookAddrToMyFn( LPVOID pAddressToHook,LPVOID pNewAddress )
{
	if (pAddressToHook == (LPVOID)0 || pAddressToHook == (LPVOID)-1 || pNewAddress == (LPVOID)0 || pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}


	RtlZeroMemory(&m_HookData,sizeof(m_HookData));
	m_HookData.pAddress = pAddressToHook;
	m_HookData.pNewAddress = pNewAddress;

	if (!VirtualProtect(m_HookData.pAddress, HOOK_SIZE, PAGE_EXECUTE_READWRITE ,&m_HookData.dwOldProtect))
	{
		return FALSE;
	}

	RtlMoveMemory(m_HookData.pOldData, m_HookData.pAddress, HOOK_SIZE);

	*(byte*)m_HookData.pNewData = 0xE9;
	*(DWORD*)((DWORD)m_HookData.pNewData + 1) = (DWORD) m_HookData.pNewAddress;
	*(DWORD*)((byte*)m_HookData.pNewData + 1) = (DWORD) m_HookData.pNewAddress - (DWORD)m_HookData.pAddress - 5;

	return TRUE;

}

bool CzjlHook::HookAddrToMyFn_Start()
{
	if (m_HookData.pAddress == (LPVOID)0 || m_HookData.pAddress == (LPVOID)-1 || m_HookData.pNewAddress == (LPVOID)0 || m_HookData.pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlMoveMemory (m_HookData.pAddress,m_HookData.pNewData, HOOK_SIZE);
	return TRUE;
}

bool CzjlHook::HookAddrToMyFn_Stop()
{
	if (m_HookData.pAddress == (LPVOID)0 || m_HookData.pAddress == (LPVOID)-1 || m_HookData.pNewAddress == (LPVOID)0 || m_HookData.pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlMoveMemory (m_HookData.pAddress,m_HookData.pOldData, HOOK_SIZE);
	return TRUE;
}

