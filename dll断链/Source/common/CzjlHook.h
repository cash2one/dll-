#pragma once
#include "atlstr.h"
#include "vector"
#include "zjlcommon.h"
using namespace std;




class CzjlHook
{
public:
	CzjlHook(void){};
	~CzjlHook(void){};

	//需要类实例
	bool HookAddrToMyFn(LPVOID pAddressToHook,LPVOID pNewAddress);
	bool HookAddrToMyFn_Start();
	bool HookAddrToMyFn_Stop();

	//Hook Dll
	bool HookDllAddr(CStringA szDll,char* pTag,unsigned int LenTag,char* pReplace,unsigned int LenReplace, int PosStartTag=0);
	static bool FindHookCallBack_JMP(CStringA szTargetDll,CStringA szHookDll);//查找模块中的hook
	static unsigned int FindDllTarget(CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen=0);//查找特征码
	HMODULE GetDllByTragetStr(CStringA szDll,CStringA szTarget);


	static unsigned int FindMemTargetFromSection(char* pStart,char* pTag,unsigned int LenTag);
	static unsigned int FindMemTargetFromStart(char* pStart,char* pEnd,char* pTag,unsigned int LenTag);
	static vector<unsigned int> SearchMemTargetFromSection(char* pStart,char* pTag,unsigned int LenTag,unsigned int SearchLen=0);
	//GetMem
	static vector<char*>  GetMemPtrByRegionSize(unsigned int RegionSize,int MemType=0);
	static unsigned int GetMemSizeFromPtr(char* pMem);
	static unsigned int GetRegionSizeFromPtr(char* pMem);
	static unsigned int GetMemLastSizeFromPtr(char* pMem);	
	static char* GetMemBySize(char* pStart,unsigned int RegionSizeMin,unsigned int RegionSizeMax,int MemType=0,unsigned int MemSearchCount=30);



	//Tool
	static int GetSystemVersion();
	static bool IsXpSystem();
	static bool IsWin7System();
	static bool ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size );
	static bool GetProcessCmdLine(DWORD dwId,wchar_t* wBuf,DWORD dwBufLen);
	static BOOL EnableDebugPrivilege();
	static int GetMainThreadID();
private:



	#define HOOK_SIZE 5
typedef struct tagHOOKDATA {
	LPVOID pAddress;
	LPVOID pNewAddress;
	DWORD dwOldProtect;
	byte pOldData[HOOK_SIZE];
	byte pNewData[HOOK_SIZE];
} HOOKDATA, *LPHOOKDATA;
HOOKDATA m_HookData;
};

