#include "LSemaphore.h"
#include "logfile/LOLHelperLocalLog.h"
#include <Windows.h>

#define LOLSEMSTR L"hlsLoLStreamSemaphore"

static HANDLE mSemaphore = NULL;


//Init ��  Exitֻ��Ҫͨ�Ŷ෽�е�һ�����þͺá�
BOOL LoLMemShareAccessInit()
{
	mSemaphore = CreateSemaphoreW(NULL, 1, 1, (LPCWSTR)LOLSEMSTR);	//fail return NULL
	if (mSemaphore)
		return TRUE;
	LoLLocalLogErr("LoLMemShareAccessInit CreateSemaphoreW  Fail");
	return  FALSE;
}

BOOL LoLMemShareAccessExit()
{
	if (mSemaphore) {
		LoLMemShareAccessOut(mSemaphore);
		CloseHandle(mSemaphore);	//sem will release when all Handle was Closed 
		mSemaphore = NULL;
	}

	return  TRUE;
}


HANDLE LoLMemShareAccessIn()
{
	HANDLE h = CreateSemaphoreW(NULL, 1, 1, (LPCWSTR)LOLSEMSTR);
	if (h == NULL)	return FALSE;

	DWORD ret = WaitForSingleObject(h, INFINITE);
	if (ret == WAIT_FAILED) {
		CloseHandle(h);
		
		LoLLocalLogErr("LoLMemShareAccessIn WaitForSingleObject  Fail");

		return 0;
	}

	return h;
}

BOOL LoLMemShareAccessOut(HANDLE h)
{
	if (h == 0)	{
		LoLLocalLogErr("LoLMemShareAccessOut param  Fail");
		return FALSE;
	}
	ReleaseSemaphore(h, 1, NULL);
	CloseHandle(h);

	return TRUE;
}