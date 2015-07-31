#include <Shlwapi.h>

#include <string>

#include "LolHelper_Adobe_Air_dll_Check.h"
#include "zjlcommon.h"
#include "CzjlHook.h"
#include "ParseLoLData.h"
#include "Hook.h"
#include "LLog.h"
#include "logfile/LoLHelperLocalLog.h"
#include "injectDll.h"

static BOOL CheckMd5AdobeAirDll0(const char* path);
static BOOL PickHookPoint0();


static void(__stdcall *pLoLGetQQFunc)(void) = 0;
static void(__stdcall *pLoLGetDataFunc)(void) = 0;




HookFuncType IsSupportAdobeAirDllVersion(const char* path)
{
	if (path == 0) {
		Log("path is null");
		LoLLocalLogErr("IsSupportAdobeAirDllVersion path is null");
		return NULL;
	}
	if (strlen(path) == 0 || PathFileExistsA(path) == 0)	{
		Log("PathFileExistsA is null");
		LoLLocalLogErr("IsSupportAdobeAirDllVersion path is null");
		return NULL;
	}
 
	static struct VersionsStru{
		const char* version;
		BOOL(*CheckFunc)(const char* path);
		HookFuncType HookFunc;
	}SupportDllVersions[] = {
			{ "3.7.0.1530", /*CheckMd5AdobeAirDll0 暂时废弃，发现文件可能被修改*/NULL, PickHookPoint0 }
	};

	int i = 0;
	std::string tmp = "";
	std::string version = GetPEVersion(path);
	Log("version is ");
	Log(version.c_str());
	for (; i < sizeof(SupportDllVersions) / sizeof(SupportDllVersions[0]); i++) {
		tmp = SupportDllVersions[i].version;
		Log("tmp is ");
		Log(tmp.c_str());
		if (tmp == version /*&& SupportDllVersions[i].CheckFunc && SupportDllVersions[i].CheckFunc(path)*/)
			break;
	}
	if (i != sizeof(SupportDllVersions) / sizeof(SupportDllVersions[0])) {		//找到匹配的项
		return SupportDllVersions[i].HookFunc;
	}

	ShareMemSendErr(ReportAirLibAdobeAirVersionNotSupported);
	LoLLocalLogErr("Adobe Air version :%s is not for support", version.c_str());

	return NULL;
}


BOOL CheckMd5AdobeAirDll0(const char* path)		//不可用,PE可能被修改
{
	if (path == 0)
		return FALSE;
	if (strlen(path) == 0 || PathFileExistsA(path) == 0)
		return FALSE;

	Log("in CheckMd5AdobeAirDll0");
	static const unsigned char md5Key[] = {
		0x45, 0x44, 0x31, 0x42, 0x33, 0x32, 0x31, 0x37, 
		0x46, 0x37, 0x44, 0x42, 0x45, 0x37, 0x30, 0x42,
		0x35, 0x39, 0x46, 0x46, 0x35, 0x33, 0x44, 0x33,
		0x46, 0x42, 0x36, 0x33, 0x37, 0x30, 0x39, 0x37
	};

	unsigned char md5New[64] = {};			
	bool ret = MD5((char*)path, (char*)md5New);
	if (ret == true) {
		if (memcmp(md5Key, md5New, sizeof(md5Key)) == 0) {
			return TRUE;
		}
	}
	Log("MD5 failed");

	return FALSE;
}



__declspec(naked) void   __stdcall MyCallLog()
{
	__asm{
		pushad;
		pushfd;
		push eax;
		call ParserLog;
		popfd;
		popad;
		jmp pLoLGetQQFunc;
	}
}


__declspec(naked) void   __stdcall MyCallData()
{
	__asm{
		pushad;
		pushfd;
		Test eax, eax;
		je Go;
		Test esi, esi;
		je Go;
		push[esi + 0x60];
		push[esi + 0x64];
		call ParserData;
	Go:
		popfd;
		popad;
		jmp pLoLGetDataFunc;
	}
}

static void HookSspi();
BOOL PickHookPoint0()
{
	//point1 Get QQ ...
	BOOL ret;
	unsigned char targetQQ[] = { 0x3B, 0xC1, 0x73, 0x0A, 0x80, 0x3A, 0x00, 0x74, 0x05, 0x42, 0x3B, 0xD1 };
	unsigned int  hookQQAddr = CzjlHook::FindDllTarget(INJECTADOBEAIRDLLNAME, "", (char*)targetQQ, sizeof(targetQQ));
	if (hookQQAddr) {
		pLoLGetQQFunc = (void(__stdcall *)(void))(hookQQAddr - 5);	//它前面是5个字节的指令 lea ecx, [eax + edi];  mov edx,eax;

		Log("before HookFunc hookQQAddr");
		LoLLocalLogInfo("before pLoLGetQQFunc Hook 0x%x", (void*)pLoLGetQQFunc);
		ret = HookFunc((void**)(&pLoLGetQQFunc), (void*)MyCallLog);
		Log("after HookFunc hookQQAddr");

		if (ret == FALSE) {
			LoLLocalLogInfo("hookQQAddr Hook Fail");
			return ret;
		}else {
			LoLLocalLogInfo("hookQQAddr Hook success: 0x%x", (void*)pLoLGetQQFunc);
		}
	}
	Log("hookQQAddr success\n");

#if 0
	unsigned char targetGameStats[] = { 0xC2, 0x08, 0x00, 0x6A, 0x01, 0x8D, 0x4D, 0xB4, 0xE8, 0x50, 0x09, 0x07, 0x00 };
	unsigned int  hookGameAddr = CzjlHook::FindDllTarget(INJECTADOBEAIRDLLNAME, "", (char*)targetGameStats, sizeof(targetGameStats));
	if (hookGameAddr) {
		pLoLGetDataFunc = (void(__stdcall *)(void))(hookGameAddr - 5);
		LoLLocalLogInfo("before targetGameStats Hook 0x%x", (void*)pLoLGetDataFunc);
		ret = HookFunc((void**)(&pLoLGetDataFunc), (void*)MyCallData);
		if (ret == FALSE) {
			LoLLocalLogInfo("targetGameStats Hook Fail");
			return ret;
		}else {
			LoLLocalLogInfo("targetGameStats Hook success 0x%x", (void*)pLoLGetDataFunc);
		}
	}
	Log("hookGameAddr success\n");
#endif

#if 1	//第二个Hook点增加一个地方拦截
	HookSspi();
#endif 
	return TRUE;
}

#define SECURITY_WIN32
#include <TlHelp32.h>
#include <process.h>
#include <Security.h>
#include <time.h>

typedef SECURITY_STATUS(__stdcall *UnsealMessageType)(
	_In_    PCtxtHandle    phContext,
	_Inout_ PSecBufferDesc pMessage,
	_In_    ULONG          MessageSeqNo,
	_Out_   PULONG         pfQOP
	);
static UnsealMessageType foldUnSealMessage = 0;
static SECURITY_STATUS __stdcall fNewUnSealMessage(
	_In_    PCtxtHandle    phContext,
	_Inout_ PSecBufferDesc pMessage,
	_In_    ULONG          MessageSeqNo,
	_Out_   PULONG         pfQOP
	)
{
	SECURITY_STATUS status = 0;

	if (foldUnSealMessage) {
		status = foldUnSealMessage(phContext, pMessage, MessageSeqNo, pfQOP);
	}

#if LIN_DBG
	//get file path
	time_t t = time(NULL);
	CreateDirectoryA("c:\\amf3", NULL);
	char time_buf[64] = { 0 };
	sprintf(time_buf, "%ld", t);
	std::string fileName = std::string("c:\\amf3\\recv_") + time_buf + ".rtmp";
#endif

	LoLLocalLogInfo("fNewUnSealMessage is running...");

	PSecBuffer psec = (PSecBuffer)pMessage->pBuffers;
	psec++;		//first: header   second: data		third: tail
	BYTE *p = (BYTE*)psec->pvBuffer;
	if (p && psec->cbBuffer /*&& (*(p + 7) == 0x11 || *(p + 7) == 0x14)*/) {
		int len = psec->cbBuffer;
		
#if LIN_DBG
		FILE *fp = fopen(fileName.c_str(), "wb");
		if (fp) {
			fwrite(p, 1, len, fp);
			fclose(fp);
		}
#endif
		ParserData(len, (char*)p);
	}

	return  status;
}


void HookSspi()
{
	HMODULE  handleSspi = GetModuleHandleA(INJECTSECURDLLNAME);
	if (handleSspi) {
		foldUnSealMessage = (UnsealMessageType)GetProcAddress(handleSspi, "UnsealMessage");
		BOOL bRet = HookFunc((void**)(&foldUnSealMessage), (void*)fNewUnSealMessage);
		if (bRet == FALSE) {
			LoLLocalLogInfo("foldUnSealMessage Hook Fail");
		}else {
			LoLLocalLogInfo("foldUnSealMessage Hook success 0x%x", (void*)foldUnSealMessage);
		}
	}else {
		LoLLocalLogInfo("HookSspi GetModuleHandleA fail");
	}
}