#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <time.h>

#include "LoLHelperLocalLog.h"
#include "zjlcommon.h"
#include "LOL\LOLDef.h"

BOOL InitFlag = FALSE;

static std::string  GetDay()
{
	time_t t = time(0);
	char tmp[64];
	strftime( tmp, sizeof(tmp), "%Y_%m_%d",localtime(&t) );

	return tmp;
}

void LoLHelperLocalLogInit(const std::string &strLog)
{
	std::string str = strLog + "_" + GetDay();
	char SystemPath[MAX_PATH] = {0};
	UINT uRet = GetSystemDirectoryA(SystemPath, sizeof(SystemPath));
	if (uRet == 0) {
		Log("LoLHelperLocalLogInit fail");
		return;
	}
	std::string strSysPath = SystemPath;
	strSysPath += "\\Wxlog";
	
	if (IsExsitDirOrFile(strSysPath.c_str()) == false)
		CreateDirectoryA(strSysPath.c_str(), NULL);
	if (IsExsitDirOrFile(strSysPath.c_str())) {
		bool bRet = InitLog(0, 0, 0, 10, strSysPath.c_str(), str.c_str(), true);
		if (bRet == true) {
			us::LogFile::Instance().SetEncryptHandle(std::bind(wx::crypt::encrypt_data, std::placeholders::_1, "lgkanen"));
			us::LogFile::Instance().SetOuputType(LogDiskFile);
			InitFlag = TRUE;
			return ;
		}
	}
}

void ShareMemSendErr(ReportType type)
{
	HANDLE hMemShareAcc = LoLMemShareAccessIn();
	if (hMemShareAcc != 0) {

		HANDLE hMem = NULL;
		stLOLShareMem *game = (stLOLShareMem*)LoLMemShareMap(&hMem, sizeof(stLOLShareMem));	//初始化大小应该和这个一致
		if (game != 0)	{
			game->ShareDataErrNum |= type;
			game->ShareDataReady |= ShareDataErrInfoReadyType_Ready;	//有错误信息上传

			LoLMemShareUnMap(game, hMem);
		}
		LoLMemShareAccessOut(hMemShareAcc);
	}
}

void ShowLastError(LPSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintfA((LPSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        "%s failed with error %d: %s", 
        lpszFunction, dw, lpMsgBuf); 

	GenericLog(LogError, "[LolHelper-err]%s", lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

}