#include "lin_common.h"

using namespace  lin_common;


BOOL sIsEqualUnicodeString(const _UNICODE_STRING &uStr1, const std::wstring & wStr)
{
	if (uStr1.Length == wStr.length()) {
		if (memcmp(uStr1.Buffer, wStr.c_str(), uStr1.Length) == 0)
			return TRUE;
	}

	return FALSE;
}

void test32()
{
	PVOID peb = NULL, ldr = NULL;
	__asm {
		mov eax, fs:[0x30];		//PEB
		mov peb, eax;
		mov eax, [eax + 0xc];	//LDR
		mov ldr, eax;
		mov eax, [eax + 0x30];	//
	}

	_peb_ldr_data *pDllData = (_peb_ldr_data*)ldr;
	LDR_DATA_TABLE_ENTRY *p = (LDR_DATA_TABLE_ENTRY*)container_of(pDllData->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks), *q = NULL;

	q = p;
	do {
		WCHAR buf[MAX_PATH] = {0};
		memcpy(buf, q->FullDllName.Buffer, q->FullDllName.Length);
		wprintf(L"%s\n", buf);

		std::wstring wstr = L"ntdll.dll";
		if (lin_common::IsEqualUnicodeString(q->BaseDllName, wstr)) {
			//Ö´ÐÐÕªÁ´
			q->InLoadOrderLinks.Flink->Blink = q->InLoadOrderLinks.Blink->Flink;
			q->InLoadOrderLinks.Blink->Flink = q->InLoadOrderLinks.Flink;
		}
		q = container_of(q->InLoadOrderLinks.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	}while (q != p);
}

int main()
{
	test32();
	getchar();

	return 0;
}