#include <Windows.h>

#include "AirLibRunOneInstance.h"
#include "logfile/LoLHelperLocalLog.h"

//��dllֻ����һ��ʵ�����
static const char* strSemOnlyRunOnce = "AirLibDLLstrSemOnlyRunOnce";
static HANDLE hOnlyRunOnce = 0;
BOOL AirLibOnlyRunOnceInit()	//��һ��AirLib�����У�����ǵĻ�������FALSE
{
	hOnlyRunOnce = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, strSemOnlyRunOnce);
	if (hOnlyRunOnce == NULL)	{
		hOnlyRunOnce = CreateSemaphoreA(NULL, 1, 1, strSemOnlyRunOnce);	//fail return NULL
		if (hOnlyRunOnce == NULL) {
			LoLLocalLogErr("OnlyRunOnceInit call CreateSemaphoreA fail");
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}


void AirLibOnlyRunOnceExit()
{
	if (hOnlyRunOnce){
		CloseHandle(hOnlyRunOnce);
		hOnlyRunOnce = 0;
	}
}
