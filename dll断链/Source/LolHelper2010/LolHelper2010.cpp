// LolHelper2010.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	HMODULE hm = LoadLibraryA("LolHelper.dll");
	if (hm) {
		getchar();
	}
#endif
	
	return 0;
}

