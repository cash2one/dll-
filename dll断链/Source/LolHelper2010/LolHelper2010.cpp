// LolHelper2010.cpp : �������̨Ӧ�ó������ڵ㡣
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

