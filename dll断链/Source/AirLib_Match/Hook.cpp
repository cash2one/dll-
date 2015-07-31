#include <Windows.h>
#include <TlHelp32.h>


#include <vector>

#include "Hook.h"
#include "detours.h"
#include "LLog.h"
#include "LolHelperInit.h"
#include "LolHelper_Adobe_Air_dll_Check.h"
#include "logfile/LoLHelperLocalLog.h"
#include "injectDll.h"

static BOOL ListProcessThreads(DWORD dwOwnerPID, std::vector<DWORD> &th32VecThreadID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
 
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);    
		return(FALSE);
	}

	th32VecThreadID.clear();
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			th32VecThreadID.push_back(te32.th32ThreadID);
		}
	} while (Thread32Next(hThreadSnap, &te32));


	CloseHandle(hThreadSnap);
	return(TRUE);
}


BOOL HookFunc(void **pOldFunc, void *pNewFunc)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();

	std::vector<DWORD> threadVec;
	ListProcessThreads(0, threadVec);

	std::vector<HANDLE> threadVecHandle;
	threadVecHandle.clear();
	HANDLE h = 0;
	for (std::vector<DWORD>::iterator it = threadVec.begin(); it != threadVec.end(); it++) {
		h = OpenThread(READ_CONTROL | ACCESS_SYSTEM_SECURITY,
			0,
			*it);
		if (h) {
			if (h != GetCurrentThread())
				DetourUpdateThread(h);			//挂起所有的非当前线程，非当前线程在DetourUpdateThread里面有判断
			threadVecHandle.push_back(h);
		}
	}

	DetourAttach(pOldFunc, pNewFunc);

	LONG error = DetourTransactionCommit();		//恢复所有的线程

	for (std::vector<HANDLE>::iterator it = threadVecHandle.begin(); it != threadVecHandle.end(); it++) {
		CloseHandle(*it);		
	}

	if (error != NO_ERROR) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

void HookStart()
{
	char bufModulePath[MAX_PATH] = { 0 };
	HMODULE hAdobeAir = NULL;
	int t = 10;

	//load Adobe AIR.dll  secru32.dll
	GetModuleFileNameA(NULL, bufModulePath, sizeof(bufModulePath));

	std::string  adobeAirPath = std::string(bufModulePath);
	adobeAirPath = adobeAirPath.substr(0, adobeAirPath.rfind("\\"));
	adobeAirPath += "\\Adobe AIR\\Versions\\1.0\\"INJECTADOBEAIRDLLNAME;
	LoLLocalLogInfo("%s", adobeAirPath.c_str());
	HMODULE hAdboe = LoadLibraryA(adobeAirPath.c_str());
	if (hAdboe) {
		LoLLocalLogInfo("%s  load success", adobeAirPath.c_str());
	}else {
		LoLLocalLogInfo("%s  load fail", adobeAirPath.c_str());
	}

	HMODULE hSec = LoadLibraryA(INJECTSECURDLLNAME);
	if (hSec) {
		LoLLocalLogInfo("%s  load success", INJECTSECURDLLNAME);
	}else {
		LoLLocalLogInfo("%s  load fail", INJECTSECURDLLNAME);
	}

	while (t--) {
		Sleep(1000);
		hAdobeAir = GetModuleHandleA(INJECTADOBEAIRDLLNAME);
		if (hAdobeAir != NULL)	break;
		else
			LoLLocalLogErr(INJECTADOBEAIRDLLNAME "was not loaded  Fail");
	}
	if (hAdobeAir) {
		GetModuleFileNameA(hAdobeAir, bufModulePath, sizeof(bufModulePath));
	}
	else {
		LoLLocalLogErr(INJECTADOBEAIRDLLNAME " was not loaded  Fail");
		return ;
	}

	Log("before IsSupportAdobeAirDllVersion");
	HookFuncType pFunc = IsSupportAdobeAirDllVersion(bufModulePath);		//Hook
	if (pFunc) {		//拿到了Hook函数，如果没有那么不匹配
		Log("IsSupportAdobeAirDllVersion success\n");
		BOOL ref = pFunc();
		if (ref)
			Log("Hook success\n");
		else {
			ShareMemSendErr(ReportAirLibErrNum_HookFail);
			LoLLocalLogErr("Hook Fail");
		}
	}
	else {
		LoLLocalLogErr("search HookFunc fail");
	}
}