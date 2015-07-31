#include "LShareMem.h"
#include "logfile/LOLHelperLocalLog.h"

static HANDLE hMapFile = NULL;

static const TCHAR szName[] = TEXT("Global\\MyLolFileMappingObject");

BOOL LoLMemShareInit(int shareMemSize)
{
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		shareMemSize,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object
	if (hMapFile)
		return TRUE;
	
	LoLLocalLogErr("LoLMemShareInit CreateFileMapping  Fail");
	return FALSE;
}

BOOL LoLMemShareExit()
{
	if (hMapFile)
		CloseHandle(hMapFile);
	hMapFile = NULL;

	return TRUE;
}

void* LoLMemShareMap(HANDLE * ph, int dSize)
{
	*ph = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		dSize,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object
	if (*ph == 0) {
		LoLLocalLogErr("LoLMemShareMap CreateFileMapping  Fail");
		return 0;
	}
	void *pBuf = (void*)MapViewOfFile(*ph,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		dSize);

	return pBuf;
}

void LoLMemShareUnMap(void* pBuf, HANDLE h)
{
	if (pBuf)
		UnmapViewOfFile(pBuf);
	if (h)
		CloseHandle(h);
}