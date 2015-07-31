#pragma once
#include "windows.h"
#include <winreg.h>
#include <winbase.h>
#include <winuser.h>
#include <process.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream> 
#include <atlstr.h>
#include <tlhelp32.h>
#include <string.h>
#include <winbase.h>
#include <shlobj.h>
#include <winver.h>
#include <io.h>
#include "Shlwapi.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"version.lib")
using namespace std;


//REG
CStringA __stdcall GetRegItemText(HKEY hKey, LPCTSTR szFullItemName, LPCTSTR szDefault);

BOOL GetProcessFullPath(DWORD dwPID, CHAR pszFullPath[MAX_PATH])  ;

//DBG
void OpenColsole(CStringA szTitle);//打开控制台
void SetDbgPrintTip( CStringA szTip );//输出抬头
void SetDbgPrintFilePath(CStringA szPath,bool bIsCreateFile=1,unsigned int AutoSaveSec=10,unsigned int AutoSaveCount=1);//打印debug日志
void DbgPrint(const char *fmt, ...);
void DbgPrintBox( const char *fmt, ... );//MessageBox
CStringA PrintBytes(char* pFlag,char* pStart,unsigned int NumBytes);
CStringA PrintHexEX();
void GetLastErrorEx();

//Path
CStringA GetAppLocalPathA();//根目录
CStringW GetAppLocalPathW();
CStringA GetAppExePathA();//Exe目录
CStringW GetAppExePathW();
CStringA GetAppTempPathA(CStringA szTempFloderName);
DWORD GetPID( CStringA ProcessName );
CStringA GetProcessCommandLine( unsigned int PID );
bool CreatePathW( CStringW szPathName );
bool CreatePathA( CStringA szPathName );
CStringA GetWindowStrA(HWND hWnd);
//判断是否存在
bool IsExsitDirOrFile(const char* fullPath);
std::string  GetPEVersion(const std::string & Path);

//String
char* UnicodeToAnsi( const wchar_t* str );
char* UnicodeToUTF8( const wchar_t* str );
char* UTF8ToAnsip(const char* str);
wchar_t * UTF8ToUnicode( const char* str );
wchar_t * AnsiToUnicode( const char* str );
CStringA AnsiToUTF8(const char* str);
CStringA UTF8ToAnsi( const char* str );

//MD5
bool MD5(char * str,unsigned int size,char * out);
bool MD5(char* FilePath,char* out);
CStringA MD5(CStringA szPath);
//MEM
void* GetMemShare(CStringA szName,unsigned int Size);

//Version
CStringA GetFileVersion(CStringA szModuleName);

//System
int GetSystemVersion();
bool IsXpSystem();
bool IsWin7System();


//Class
class CzjlFile
{
public:
	CzjlFile(char* szPath);
	CzjlFile(wchar_t* wszPath);
	CzjlFile(CStringW strPath);
	CzjlFile(CStringA strPath);
	~CzjlFile(void);
	void LoadStringFileA(CStringA& szBuffer);
	void LoadStringFileW(CStringW& strBuffer);

	void WriteStringA(CStringA& szBuffer);
	void WriteStringW(CStringW& strBuffer);
	void Clear();

	void SetFileStatus(int attribute);
	int GetFileStatus();


	int FileSize();
	char* GetFileBuffer(unsigned int* pFileSize=0);

	unsigned int SaveFile(char* pBuffer,unsigned int FileSize);
	unsigned int  LoadFile(char* pBuffer=0,unsigned int fileSize=0);

	void OpenExplorer();
	//获取dll/exe的版本号
	CString GetFileVersionStr();
	unsigned int GetFileVersion();


private:
	char* m_pBuffer;
	unsigned int m_FileSize;
	CStringW m_strPath;
};

class CzjlTextLog
{
public:
	CzjlTextLog(CStringA szPathFileName,bool bIsCreateFile=1,unsigned int AutoSaveSec=10,unsigned int AutoSaveCount=1);
	~CzjlTextLog(void);

	void AddString(CStringA szString);
	void AddString(CStringA szString,long long FlagNumber);
	void AddStringLast(CStringA szString);
	void AddByte(CStringA sz,char* pBuffer,unsigned int _size);
	void BindWindow(HWND hWnd);
	void Update();

	unsigned int GetMsgCount(){return m_AddCount;};
	void Clear();
private:
	void Load();
	void Save();
	bool m_bIsCreateFile;
	CStringA m_szLogPath;
	CStringA m_szBuffer;
	HWND m_hWnd;
	unsigned int m_AutoSaveSec;
	unsigned int m_AutoSaveCount;
	unsigned int m_AddCount;//条目计数
	CComAutoCriticalSection m_CS;
	static  DWORD WINAPI ThreadAutoSaveLog(LPVOID lpThreadParameter );
};