#include <Windows.h>

#include <stdio.h>

#include "LolHelperInit.h"
#include "LolHelper_Adobe_Air_dll_Check.h"
#include "logfile/LoLHelperLocalLog.h"
#include "LLog.h"
#include "Hook.h"


extern "C"  __declspec(dllexport)  bool AirLib_Match_Init()
{
	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		__try
		{
			//Init
			BOOL bRet = LolHelperInit();

			//反调试 开启会闪退
			//protect::CProtect::ProtectAntiDebugRun();		

			//Hook
			LoLLocalLogInfo("after LolHelperInit ");
			if (bRet) {
				LoLLocalLogInfo("AirLib is running...");
				HookStart();
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Log("get some except");
			exit(1);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			//Exit
			LolHelperExit();

			//反调试线程
			//protect::CProtect::ProtectAntiDebugStop();
		}
		break;
	}
	return TRUE;
}

