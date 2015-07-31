// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <Windows.h>
#include <tlhelp32.h>
#include <process.h>
#include <time.h>
#include <atltime.h>
#include <strsafe.h>
#include <algorithm>
#include <string>

#include "LShareMem.h"
#include "LSemaphore.h"
#include "LOL/LoLDef.h"
#include "llog.h"
#include "ReportData.h"
#include "RemoteThreadInject.h"
#include "zjlcommon.h"
#include "logfile/LoLHelperLocalLog.h"
#include "injectDll.h"

#pragma warning (disable:4995)


static CStringA PrintGameInfo(const stLOLGame &GameData)
{
	CStringA szResult, szTemp;
	szResult += "本局游戏信息\r\n";
	szTemp.Format("GameID : %I64u ", GameData.GameID);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameLengh : %d ", GameData.GameLengh);
	szResult += szTemp + "\r\n";

	szTemp.Format("PlayerCount : %d ", GameData.PlayerCount);
	szResult += szTemp + "\r\n";

	szTemp.Format("WinnerTeam : %d ", GameData.WinnerTeam);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameMode : %d ", GameData.GameMode);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameType : %d ", GameData.GameType);
	szResult += szTemp + "\r\n";

	szResult += "上传者信息\r\n";
	szTemp.Format("QQ : %I64u ", GameData.GameState.QQ);
	szResult += szTemp + "\r\n";

	szTemp.Format("ServerID : %u ", GameData.GameState.ServerID);
	szResult += szTemp + "\r\n";

	szTemp.Format("LOLGlobalID : %u ", GameData.GameState.LOLGlobalID);
	szResult += szTemp + "\r\n";

	szTemp.Format("szMainPlayer : %s ", GameData.GameState.szMainPlayer);
	szResult += szTemp + "\r\n";

	szTemp.Format("Level : %d ", GameData.GameState.Level);
	szResult += szTemp + "\r\n";

	CTime time = GameData.GameState.GameEndTime;
	szTemp.Format("GameEndTime : %d-%d-%d %d:%d:%d \r\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	szResult += szTemp + "\r\n";

	szTemp.Format("szRankLevel : %s ", GameData.GameState.szRankLevel);
	szResult += szTemp + "\r\n";

	szTemp.Format("HeadImgID : %d ", GameData.GameState.HeadImgID);
	szResult += szTemp + "\r\n";

	szTemp.Format("RateNumber : %d ", GameData.GameState.RateNumber);
	szResult += szTemp + "\r\n";

	for (unsigned int i = 0; i<LOLMAXPLAYER; i++)
	{
		long long PGID = GameData.Players[i].globalPlayerID;

		if (PGID)
		{
			szResult += "______________________________\r\n";
			szTemp.Format("Index : %d ", i + 1);
			szResult += szTemp + "\r\n";

			szTemp.Format("TEAM : %d ", GameData.Players[i].TEAM);
			szResult += szTemp + "\r\n";

			szTemp.Format("Win? : %d ", GameData.Players[i].Win);
			szResult += szTemp + "\r\n";

			szTemp.Format("globalPlayerID : %I64u ", PGID);
			szResult += szTemp + "\r\n";

			szTemp.Format("szPlayerName : %s ", GameData.Players[i].szPlayerName);
			szResult += szTemp + "\r\n";

			szTemp.Format("HeroID : %d ", GameData.Players[i].HeroID);
			szResult += szTemp + "\r\n";

			szTemp.Format("szHeroName : %s ", GameData.Players[i].szHeroName);
			szResult += szTemp + "\r\n";

			szTemp.Format("LEVEL : %d ", GameData.Players[i].LEVEL);
			szResult += szTemp + "\r\n";

			szTemp.Format("Kill : %d ", GameData.Players[i].CHAMPIONS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("Dead : %d ", GameData.Players[i].NUM_DEATHS);
			szResult += szTemp + "\r\n";

			szTemp.Format("ASSISTS : %d ", GameData.Players[i].ASSISTS);
			szResult += szTemp + "\r\n";

			szTemp.Format("小兵击杀 : %d ", GameData.Players[i].MINIONS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("兵营 : %d ", GameData.Players[i].BARRACKS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("拆塔 : %d ", GameData.Players[i].TURRETS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("插眼 : %d ", GameData.Players[i].WARD_PLACED);
			szResult += szTemp + "\r\n";

			szTemp.Format("排眼 : %d ", GameData.Players[i].WARD_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("最高多杀 : %d ", GameData.Players[i].LARGEST_MULTI_KILL);
			szResult += szTemp + "\r\n";

			szTemp.Format("最高连杀 : %d ", GameData.Players[i].LARGEST_KILLING_SPREE);
			szResult += szTemp + "\r\n";

			szTemp.Format("金钱 : %d ", GameData.Players[i].GOLD_EARNED);
			szResult += szTemp + "\r\n";

			szTemp.Format("造成伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_DEALT);
			szResult += szTemp + "\r\n";

			szTemp.Format("承受伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_TAKEN);
			szResult += szTemp + "\r\n";

			szTemp.Format("ITEM0 : %d ", GameData.Players[i].ITEM0);
			szResult += szTemp + "\r\n";

			szTemp.Format("ITEM1 : %d ", GameData.Players[i].ITEM1);
			szResult += szTemp + "\r\n";
		}
	}

	return szResult;
}

/*跟AirLib.dll 通信线程*/
static unsigned int __stdcall GetMemShareGameData(void *p);
static BOOL IsGetMemShareGameDataThreadRun = FALSE;
static HANDLE beginThreadMemSem = 0;
static HANDLE hMemShareAcc = NULL;

static void GetMemShareGameDataInit()
{
	LoLLocalLogInfo("%s", __FUNCTION__);

	unsigned tid = 0;
	IsGetMemShareGameDataThreadRun = TRUE;
	beginThreadMemSem = (HANDLE)_beginthreadex(NULL,	//安全
		0,		//StackSize
		GetMemShareGameData,
		NULL,	//arglist
		0,		//initFlag
		&tid
		);
	if (beginThreadMemSem == 0) {
		LoLLocalLogErr("GetMemShareGameDataInit call _beginthreadex GetMemShareGameData fail");
	}
}
static void GetMemShareGameDataExit()
{
	/*IsGetMemShareGameDataThreadRun = FALSE;
	if (beginThreadMemSem) {
		DWORD dwRet = WaitForSingleObject(beginThreadMemSem, INFINITE);
		if (WAIT_FAILED == dwRet) {
			Log(__FUNCTION__);
			Log("WaitForSingleObject fail");
		}
		else {
			Log(__FUNCTION__);
			Log("WaitForSingleObject success");
		}
		CloseHandle(beginThreadMemSem);
	}*/

	LoLMemShareAccessOut(hMemShareAcc);
}
static unsigned int __stdcall GetMemShareGameData(void *p)
{
	//(void*)p;
	LoLLocalLogInfo("%s", __FUNCTION__);

	HANDLE hMem = NULL;
	stLOLShareMem *game = NULL;
	Json::Value ReportRootJson;
	int IsReportWxClientReadyReportNum = 0;
	int logCount = 0;

	while (IsGetMemShareGameDataThreadRun) {
		Sleep(1000);

		if (IsReportWxClientReady() == FALSE) {
			if (IsReportWxClientReadyReportNum++ > 60 * 20) {	//20分钟还拿不到计费信息就上报
				IsReportWxClientReadyReportNum = 0;
				std::string str = "游戏启动的时候获取Head失败";
				ReportLog(ReportGetReportHeadFail, str);  
				LoLLocalLogErr("游戏启动的时候获取Head失败");
			}
			continue;
		}

#if LIN_DBG1
		NoDebugLog("hello");
#endif
		hMemShareAcc = LoLMemShareAccessIn();	//进入临界区
#if LIN_DBG1
		NoDebugLog("world");
#endif
		if ((logCount++) % 20 == 0)
			LoLLocalLogInfo("GetMemShareGameData is in ShareMem");
		if (hMemShareAcc != 0) {
			hMem = NULL;
			game = (stLOLShareMem*)LoLMemShareMap(&hMem, sizeof(stLOLShareMem));	//初始化大小应该和这个一致
			if (game != 0)	{
				if (game->ShareDataReady & ShareDataReadyType_Ready) {
					//Handle stLOLGame

					LoLLocalLogInfo("抓到数据了");

					CStringA szDebugFile = PrintGameInfo(game->game);
					std::string str = szDebugFile.GetBuffer(0);

					LoLLocalLogInfo("%s\n", str.c_str());

					ReportData(game->game);
#if  LIN_DBG
					FILE *fp = fopen("c:\\LoLGameStats.txt", "ab");
					if (fp) {
						fwrite(str.c_str(), 1, str.length(), fp);
						fclose(fp);
					}
#endif
				}

				if (game->ShareDataReady & ShareDataErrInfoReadyType_Ready) {
					ReportLog(game->ShareDataErrNum);
				}

#if LIN_DBG1 == 0 && LIN_DBG == 0
				//memset(game, 0, sizeof(stLOLShareMem));  多盘游戏，QQ号和服务器不会被再次写进共享内存，所以不要memset为0
				game->ShareDataReady = ShareDataReadyType_NOT_Ready;
#endif
				LoLMemShareUnMap(game, hMem);
			}
			else 
				LoLLocalLogErr("LoLMemShareMap fail");
			LoLMemShareAccessOut(hMemShareAcc);			//出来
		}
		else {
			LoLLocalLogErr("GetMemShareGameData call LoLMemShareAccessIn fail");
		}
	}

	return 0;
}

/*注入AirLib.dll到LolClient.exe线程*/
static BOOL IsInjectThreadRun = FALSE;
static HANDLE beginThreadInject = 0;

BOOL IsTecentGameLoL(DWORD dwPid)
{
	char buf[MAX_PATH] = {0};
	BOOL bRet = GetProcessFullPath(dwPid, buf);
	if (bRet == FALSE)
		return FALSE;
	std::string lolPath = buf;
	if (lolPath.empty())
		return FALSE;

	std::string str = lolPath.substr(0, lolPath.rfind("Air\\LolClient.exe"));
	str = str + "TCLS\\Client.exe";
	
	if (_access(str.c_str(), 0) == 0)
		return TRUE;
	return FALSE;
}



static BOOL IsLoadedDll(int dwRemoteProcessId, const char* dllName) {
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = { 0 };

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
			if (me32.th32ProcessID == dwRemoteProcessId)
			{
				//LoLLocalLogInfo("%s", me32.szModule);
				std::string str1 = dllName, str2 = me32.szModule;
				transform (str1.begin(),str1.end(), str1.begin(), toupper); 
				transform (str2.begin(),str2.end(), str2.begin(), toupper); 
				if (/*memcmp(me32.szModule, dllName, strlen(dllName) ) == 0*/ str1 == str2){
					if (hModuleSnap)
						CloseHandle(hModuleSnap);
					return TRUE;
				}
			}
		}
		while (::Module32Next(hModuleSnap, &me32));
	}

	if (hModuleSnap)
		CloseHandle(hModuleSnap);

	return FALSE;
}

static unsigned int __stdcall InjectAirLibIntoLolClient(void *p)
{
	DWORD dwPid = 0, dwPrePid = 0;
	const char* AirLibName = "AirLib.dll";
	char lolClientPath[MAX_PATH] = {0};
	int logCount = 0;

	LoLLocalLogInfo("%s", __FUNCTION__);
	while (IsInjectThreadRun == TRUE) {
		Sleep(50);		//
		
		dwPid = GetProcessID("LolClient.exe");
		BOOL IsLoLGuoFu = FALSE;
		if (dwPid) {
			IsLoLGuoFu = IsTecentGameLoL(dwPid);
		}
		if ((logCount++) % 20 == 0)
			LoLLocalLogInfo("InjectAirLibIntoLolClient is in while(1)");
		if (IsLoLGuoFu && dwPid && dwPid != dwPrePid) {
			std::string strAirLibPath = GetAppLocalPathA().GetBuffer(0);
			strAirLibPath += AirLibName;
			if (IsExsitDirOrFile(strAirLibPath.c_str())) {
				//Sleep(500);		

				BOOL bRet = InjectDll(strAirLibPath.c_str(), dwPid, AirLibName);
				if (bRet == FALSE) {
					ReportLog(ReportInjectAdobeAirDLLFail);
					LoLLocalLogErr("InjectAirLibIntoLolClient call InjectDll fail");
				}else {
					dwPrePid = dwPid;
				}
			}
		}
	}

	return 0;
}

static void InjectAirLibIntoLolClientInit()
{
	unsigned tid = 0;
	IsInjectThreadRun = TRUE;
	beginThreadInject = (HANDLE)_beginthreadex(NULL,	//安全
		0,		//StackSize
		InjectAirLibIntoLolClient,
		NULL,	//arglist
		0,		//initFlag
		&tid
		);
	if (beginThreadInject == 0) {
		LoLLocalLogErr("InjectAirLibIntoLolClientInit call _beginthreadex InjectAirLibIntoLolClient fail");
	}
}

static void InjectAirLibIntoLolClientExit()
{
	/*IsInjectThreadRun = FALSE;
	if (beginThreadInject) {
		DWORD dwRet = WaitForSingleObject(beginThreadInject, INFINITE);
		if (WAIT_FAILED == dwRet) {
			Log(__FUNCTION__);
			Log("WaitForSingleObject fail");
		}
		else {
			Log(__FUNCTION__);
			Log("WaitForSingleObject success");
		}
		CloseHandle(beginThreadInject);
	}*/
}


//本dll只运行一个实例检测
static const char* strSemOnlyRunOnce = "LOLHelperDLLstrSemOnlyRunOnce";
static HANDLE hOnlyRunOnce = 0;
static BOOL OnlyRunOnceInit()
{
	hOnlyRunOnce = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, strSemOnlyRunOnce);
	if (hOnlyRunOnce == NULL)	{
		hOnlyRunOnce = CreateSemaphoreA(NULL, 1, 1, strSemOnlyRunOnce);	//fail return NULL
		if (hOnlyRunOnce == NULL) {
			LoLLocalLogErr("OnlyRunOnceInit call CreateSemaphoreA fail");
			return FALSE;
		}
		ReportLog(ReportLoLHelper_Arrive);
		return TRUE;
	}

	//不允许运行多个实例dll
	return FALSE;
}


static void OnlyRunOnceExit()
{
	if (hOnlyRunOnce){
		CloseHandle(hOnlyRunOnce);
		hOnlyRunOnce = 0;
	}
}

/*模块初始化，开启上面两个线程，只运行一个实例初始化*/

static void LolHelperInit()
{
	LoLHelperLocalLogInit("WxLOLHelper");
	BOOL bRet = OnlyRunOnceInit();
	if (bRet) {
		LoLLocalLogInfo("OnlyRunOnceInit success");
		GetMemShareGameDataInit();
		InjectAirLibIntoLolClientInit();
	}else {
		LoLLocalLogInfo("OnlyRunOnceInit fail");
	}
}

/*模块结束，结束上面两个线程，只运行一个实例结束*/
static void LolHelperExit()
{
	InjectAirLibIntoLolClientExit();
	GetMemShareGameDataExit();
	OnlyRunOnceExit();
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if AddVEHFLAG
		AddVEH();
#endif
		LolHelperInit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		LolHelperExit();
		break;
	}
	return TRUE;
}


#define AddVEHFLAG 0
#if AddVEHFLAG   //VEH
static LONG WINAPI CrashCallBack(
	_In_ struct _EXCEPTION_POINTERS *ExceptionInfo
	)
{
	return 0;
}

static void AddVEH()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashCallBack);
}
#endif 
