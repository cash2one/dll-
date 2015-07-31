#pragma once
#include <windows.h>

#ifndef _HOOK_API_JMP_
#define _HOOK_API_JMP_
class CHookApi_Jmp  
{
public: 
	HANDLE hProc; 
	void Unlock(void);
	void Lock(void); 
	BOOL Initialize(wchar_t*  ModuleName, char*  ApiName, FARPROC lpNewFunc);
    BOOL HookAddr(char* lpAddr, FARPROC lpNewFunc);
	void SetHookOn(void); 
	void SetHookOff(void); 
	CHookApi_Jmp(void); 
	virtual ~CHookApi_Jmp();

	bool ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size );

	BYTE m_OldFunc[8]; 
protected: 
	BYTE m_NewFunc[8]; 
	FARPROC m_lpHookFunc; 
	CRITICAL_SECTION m_cs;
};
#endif