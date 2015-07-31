#pragma once
#include <atlstr.h>
#include <winnt.h>
#include <winbase.h>
#include "zjlcommon.h"

typedef void (*pCallBackEvent)(char* pMem,unsigned int Size);

/*
void CallBackEvent(char* pMem,unsigned int Size)
{
	DbgPrint(" CallBack = %s  Len=%d",pMem,Size);
}
*/



class CMemEvent
{

public:
	CMemEvent(char* pName,unsigned int MemSize);
	~CMemEvent(void);

	bool  StartServer(pCallBackEvent pFun);
	bool SendData(char* pBuffer,unsigned int Len);


private:
	static DWORD WINAPI ThreadEvent(LPVOID lp);
	void* GetMemShare1(CStringA szName,unsigned int Size);
	pCallBackEvent m_pFunCallback;
	HANDLE m_hMap;
	HANDLE m_hEvent;
	HANDLE m_hThread;
	CStringA m_szEventName;
	unsigned int m_MemSize;
	char* m_pData;
	bool bRun;

};


