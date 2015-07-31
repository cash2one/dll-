#include "HookApi_Jmp.h"
#include <atlstr.h>



//---------------------------------------------------------------------------
CHookApi_Jmp::CHookApi_Jmp(void)
{ 
	InitializeCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
CHookApi_Jmp::~CHookApi_Jmp()
{ 
	CloseHandle(hProc); 
	DeleteCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
void CHookApi_Jmp::SetHookOn(void)
{ 
	DWORD dwOldFlag;
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag))
	{  
		if(WriteProcessMemory(hProc,m_lpHookFunc,m_NewFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))    
				return;  
		} 
	} 
	MessageBoxA(NULL,"SetHookOn","fail",MB_OK); 
	return;
}
//---------------------------------------------------------------------------
void CHookApi_Jmp::SetHookOff(void)
{ 
	DWORD dwOldFlag; 
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
	{  
		if(WriteProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))    
				return;  
		}
	}
	MessageBox(NULL,L"SetHookOff",L"fail",MB_OK);
	return;
}
//---------------------------------------------------------------------------

BOOL CHookApi_Jmp::Initialize(wchar_t* ModuleName,char* ApiName, FARPROC lpNewFunc)
{
	m_lpHookFunc=0;
	m_lpHookFunc = GetProcAddress(GetModuleHandle(ModuleName),ApiName);
	hProc = GetCurrentProcess();
	DWORD dwOldFlag;
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
	{  
		if(ReadProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))   
			{    
                m_NewFunc[0]=0xe9;    
                DWORD*pNewFuncAddress;    
                pNewFuncAddress=(DWORD*)&m_NewFunc[1];    
                *pNewFuncAddress=(DWORD)lpNewFunc-(DWORD)m_lpHookFunc-5;
				return TRUE;   
			}  
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
void CHookApi_Jmp::Lock(void) //多线程下使用
{
	EnterCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
void CHookApi_Jmp::Unlock(void)
{ 
	LeaveCriticalSection(&m_cs);
}

BOOL CHookApi_Jmp::HookAddr( char* lpAddr, FARPROC lpNewFunc )
{
    m_lpHookFunc=0;
    m_lpHookFunc =(FARPROC) lpAddr;
    hProc = GetCurrentProcess();
    DWORD dwOldFlag;
    if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
    {  
        if(ReadProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
        {   
            if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))   
            {    
                m_NewFunc[0]=0xe9;    
                DWORD*pNewFuncAddress;    
                pNewFuncAddress=(DWORD*)&m_NewFunc[1];    
                *pNewFuncAddress=(DWORD)lpNewFunc-(DWORD)m_lpHookFunc-5;
                return TRUE;   
            }  
        }
    }
    return FALSE;
}

bool CHookApi_Jmp::ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size )
{
	 DWORD dwOldFlag;
	 hProc = GetCurrentProcess();
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

//---------------------------------------------------------------------------
