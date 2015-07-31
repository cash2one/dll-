#include <windows.h>
#include <tlhelp32.h>

#include <stdio.h>
#include <string>

#include "LLog.h"
#include "logfile/LoLHelperLocalLog.h"
#include "AirLibRunOneInstance.h"

#pragma warning (disable:4995)



int EnableDebugPriv(const char * name)  //��������ΪDEBUGȨ��
{
	HANDLE hToken = 0;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//�򿪽������ƻ�
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		LoLLocalLogErr("OpenProcessToken error\n");
		return 1;
	}
	//��ý��̱���ΨһID
	if (!LookupPrivilegeValueA(NULL, name, &luid))
	{
		LoLLocalLogErr("LookupPrivilege error!\n");
		CloseHandle(hToken);
		return 1;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//��������Ȩ��
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(hToken);
		LoLLocalLogErr("AdjustTokenPrivileges error!\n");
		return 1;
	}
	CloseHandle(hToken);
	return 0;
}

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId, const char* dllName) //ע�뺯��
{
	BOOL bRet = FALSE;
	HANDLE hRemoteProcess = NULL;
	char *pszLibFileRemote = NULL;
	HANDLE hRemoteThread = NULL;

	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = { 0 };

	Log(__FUNCTION__);
	__try {
		//��õ���Ȩ��
		if (EnableDebugPriv("SeDebugPrivilege"))
		{
			LoLLocalLogErr("add privilege error");
			return FALSE;
		}
		//��Ŀ�����
		if ((hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId)) == NULL)
		{
			LoLLocalLogErr("OpenProcess error\n");
			return FALSE;
		}

		//����Ѿ�ע����ˣ�����
#if 0
		hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwRemoteProcessId);
		if (hModuleSnap == INVALID_HANDLE_VALUE) {
			LoLLocalLogErr("InjectDll call CreateToolhelp32Snapshot error\n");
			return FALSE;
		}
		me32.dwSize = sizeof(MODULEENTRY32);
		if (::Module32First(hModuleSnap, &me32))
		{
			do
			{
				//cout<<me32.szExePath<<"\t\t\t"<<me32.th32ProcessID<<endl;
				if (me32.th32ProcessID == dwRemoteProcessId)
				{
					Log(me32.szModule);
					if (memcmp(me32.szModule, dllName, strlen(dllName)) == 0){
						Log("�Ѿ���ע�����");
						return TRUE;
					}
				}
			}
			while (::Module32Next(hModuleSnap, &me32));
		}
#else 
		//��������
		BOOL bRet = AirLibOnlyRunOnceInit();
		if (bRet == FALSE)	{	//��AirLib�����У�֤����ע�����
			AirLibOnlyRunOnceExit();
			LoLLocalLogInfo("AirLib is already running...");
			return TRUE;
		}
		LoLLocalLogInfo("AirLib is not running...");
		AirLibOnlyRunOnceExit();
#endif

		//������dll�ļ�����·��
		pszLibFileRemote = (char *)VirtualAllocEx(hRemoteProcess,
			NULL, lstrlen(DllFullPath) + 1,
			MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL)
		{
			LoLLocalLogErr("VirtualAllocEx error\n");
			return FALSE;
		}
		//��dll������·��д�뵽�ڴ棬
		if (WriteProcessMemory(hRemoteProcess,
			pszLibFileRemote, (void *)DllFullPath, lstrlen(DllFullPath) + 1, NULL) == 0)
		{
			LoLLocalLogErr("WriteProcessMemory error\n");
			return FALSE;
		}
		//�õ�LoadLibraryA������ַ
		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");
		if (pfnStartAddr == NULL)
		{
			LoLLocalLogErr("GetProcAddress error\n");
			return FALSE;
		}
		
		//����Զ���߳�
		if ((hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0,
			pfnStartAddr, pszLibFileRemote, 0, NULL)) == NULL)
		{
			LoLLocalLogErr("CreateRemoteThread error\n");
			return FALSE;
		}
	}
	__finally {

#if 1
		if (hRemoteThread) {
			DWORD dwRet = WaitForSingleObject(hRemoteThread, INFINITE);	
			if (dwRet == WAIT_OBJECT_0 && pszLibFileRemote && hRemoteProcess) {
				bRet = VirtualFreeEx(hRemoteProcess, pszLibFileRemote, 0, MEM_RELEASE);
				if (bRet == FALSE) {
					LoLLocalLogErr("VirtualFreeEx error");
				}
				else {
					LoLLocalLogInfo("VirtualFreeEx success");
				}
			}
		}
#endif
		if (hRemoteProcess)
			CloseHandle(hRemoteProcess);
		if (hModuleSnap)
			CloseHandle(hModuleSnap);
		if (hRemoteThread)
			CloseHandle(hRemoteThread);
	}
	

	return TRUE;
}

DWORD GetProcessID(char *ProcessName)  //��ý���PID
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	//���ϵͳ�����н��̿���
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error");
		return 0;
	}
	//ö���б��еĵ�һ������
	BOOL bProcess = Process32First(hProcessSnap, &pe32);
	char buf1[MAX_PATH] = { 0 };
	char buf2[MAX_PATH] = { 0 };
	while (bProcess)
	{
		//�Ƚ��ҵ��Ľ�����������Ҫ���ҵĽ�������һ���򷵻ؽ���id
		memset(buf1, 0, sizeof(buf1));
		memset(buf2, 0, sizeof(buf2));
		memcpy(buf1, pe32.szExeFile, strlen(pe32.szExeFile));
		memcpy(buf2, ProcessName, strlen(ProcessName));
		if (strcmp(_strupr(buf1), _strupr(buf2)) == 0)
			return pe32.th32ProcessID;
		//��������
		bProcess = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}