#include "SpyMem.h"


struct{
	CzjlHook hookNtAllocateVirtualMemory;
	pNtAllocateVirtualMemory pOldNtAllocateVirtualMemory;

	CzjlHook hookVirtualAlloc;
	CzjlHook hookHeapAlloc;
}hookSpyMem;


HANDLE g_hThreadGetKeyState=0;
CCallStat g_StackBackCall_HeapAlloc("StackBack_HeapAlloc");
CCallStat g_StackBackCall_VirtualAlloc("StackBack_VirtualAlloc");
DWORD WINAPI ThreadGetKeyState(LPVOID lp)
{
	while(1)
	{
		if ( GetKeyState(VK_CONTROL)<0 &&  GetKeyState(VK_F1)<0)  
		{
			hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Stop();
			hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Stop();
			DbgPrint("Key F2 %s",g_StackBackCall_HeapAlloc.PrintToDebugView());
			DbgPrint("Key F2 %s",g_StackBackCall_VirtualAlloc.PrintToDebugView());
			hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Start();
			hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Start();
			Sleep(100);
		}

		if ( GetKeyState(VK_CONTROL)<0 &&  GetKeyState(VK_F2)<0)  
		{
			hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Stop();
			hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Stop();
			DbgPrint("Key F3 %s",g_StackBackCall_HeapAlloc.PrintToDebugViewNew());
			DbgPrint("Key F3 %s",g_StackBackCall_VirtualAlloc.PrintToDebugViewNew());
			hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Start();
			hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Start();
			Sleep(100);
		}
		Sleep(100);
	}
}


//»ØËÝ¼ì²â
void StackBack(int CheckRange,CStringA szType)
{
	int CurrentEbp=0;

	__asm{
		mov [CurrentEbp],ebp;
	}

	int* pCurrentEbp=&CurrentEbp;

	CStringA szCallBack,szTemp;
	szCallBack.Format("Õ»»ØËÝ,²ãÊý%d,%s:",CheckRange,szType);
	for (unsigned int i=0;i<CheckRange;i++)
	{
		int pRetAddr=*(pCurrentEbp+1);//EBP+0x04
		szTemp.Format("[%d-0x%08X]",i+1,pRetAddr);
		szCallBack+=szTemp;
		//DbgPrint("StackBack : %d  BackAddr=%08X",i+1,pRetAddr);
		 if ( IsBadReadPtr((const void*)(*pCurrentEbp),4) )
		 {
			//  DbgPrint("StackBack Error : %d  BackAddr=%08X  pCurrentEbp=%08X",i+1,pRetAddr,pCurrentEbp);
			 break;
		 }else{
			 if (szType=="HeapAlloc")
			 {
				 g_StackBackCall_HeapAlloc.AddCount(pRetAddr);
			 }else if (szType=="VirtualAlloc")
			 {
				 g_StackBackCall_VirtualAlloc.AddCount(pRetAddr);
			 }
			 pCurrentEbp=(int*)(*pCurrentEbp);
		 }
	}

	DbgPrint("%s",szCallBack);
}




//NtAllocateVirtualMemory
int MyNtAllocateVirtualMemory( __in HANDLE ProcessHandle, __inout PVOID *BaseAddress, __in ULONG_PTR ZeroBits, __inout PSIZE_T RegionSize, __in ULONG AllocationType, __in ULONG Protect )
{
	
		if (hookSpyMem.pOldNtAllocateVirtualMemory)
		{
			hookSpyMem.hookNtAllocateVirtualMemory.HookAddrToMyFn_Stop();
			int pResult= (*hookSpyMem.pOldNtAllocateVirtualMemory)(ProcessHandle,BaseAddress,ZeroBits,RegionSize,AllocationType,Protect);
			hookSpyMem.hookNtAllocateVirtualMemory.HookAddrToMyFn_Start();
			DbgPrint("[NtAllocateVirtualMemory]~ProcessHandle=%08X,BaseAddress=%08X,ZeroBits=%08X,RegionSize=%08X,AllocationType=%08X,Protect=%08X  [Result=%08X]",ProcessHandle,BaseAddress,ZeroBits,RegionSize,AllocationType,Protect,pResult);
			return pResult;
		}
		return 0;
}
bool Spy_NtAllocateVirtualMemory()
{
	HMODULE hNTDLL= LoadLibraryA("ntdll.dll");
	if (hNTDLL)
	{
		hookSpyMem.pOldNtAllocateVirtualMemory=(pNtAllocateVirtualMemory)GetProcAddress(hNTDLL,"NtAllocateVirtualMemory");
		hookSpyMem.hookNtAllocateVirtualMemory.HookAddrToMyFn(hookSpyMem.pOldNtAllocateVirtualMemory,MyNtAllocateVirtualMemory);
		return hookSpyMem.hookNtAllocateVirtualMemory.HookAddrToMyFn_Start();
	}
	return 0;
}
//VirtualAlloc
LPVOID	WINAPI MyVirtualAlloc( __in_opt LPVOID lpAddress, __in SIZE_T dwSize, __in DWORD flAllocationType, __in DWORD flProtect )
{
	LPVOID lpResult=0;
	hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Stop();
	StackBack(3,"VirtualAlloc");
	lpResult=	VirtualAlloc(lpAddress,dwSize,flAllocationType,flProtect);
	DbgPrint("[MyVirtualAlloc]~lpAddress:%08X,dwSize:%08X,flAllocationType:%08X,flProtect:%08X [pResult=0x%08X,ThreadID=%d]",lpAddress,dwSize,flAllocationType,flProtect,lpResult,GetCurrentThreadId());
	hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Start();
	return lpResult;
}

bool Spy_NtVirtualAlloc()
{
	if (g_hThreadGetKeyState==0)
	{
		g_hThreadGetKeyState=CreateThread(0,0,ThreadGetKeyState,0,0,0);
	}
	hookSpyMem.hookVirtualAlloc.HookAddrToMyFn(VirtualAlloc,MyVirtualAlloc);
	return hookSpyMem.hookVirtualAlloc.HookAddrToMyFn_Start();
}
//HeapAlloc
bool Spy_NtHeapAlloc()
{
	if (g_hThreadGetKeyState==0)
	{
		g_hThreadGetKeyState=CreateThread(0,0,ThreadGetKeyState,0,0,0);
	}
	hookSpyMem.hookHeapAlloc.HookAddrToMyFn(HeapAlloc,MyHeapAlloc);
	return hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Start();
}

LPVOID
	WINAPI MyHeapAlloc( __in HANDLE hHeap, __in DWORD dwFlags, __in SIZE_T dwBytes )
{
	LPVOID lpResult=0;
	hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Stop();
	StackBack(3,"HeapAlloc");
	lpResult=	HeapAlloc(hHeap,dwFlags,dwBytes);
	DbgPrint("[HeapAlloc]~hHeap:%08X,dwFlags:%08X,dwBytes:%08X [pResult=0x%08X,ThreadID=%d]",hHeap,dwFlags,dwBytes,lpResult,GetCurrentThreadId());
	hookSpyMem.hookHeapAlloc.HookAddrToMyFn_Start();
	return lpResult;
}
