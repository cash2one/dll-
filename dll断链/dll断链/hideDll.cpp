#include <Windows.h>
#include <stdio.h>

struct _peb_ldr_data
{

};

void test32()
{
	PCHAR peb = NULL, ldr = NULL, dllPath;
	__asm {
		mov eax, fs:[0x30];		//PEB
		mov eax, [eax + 0xc];			//LDR
		mov eax, [eax + 0x30];	//
	}
}

int main()
{
	test32();

	return 0;
}