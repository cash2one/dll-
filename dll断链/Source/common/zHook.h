// ============================================
// zHook
// ============================================

#include <Windows.h>

#pragma once

#define HOOK_DATA_SIZE 5

typedef struct tagHOOKDATA {
	LPVOID pAddress;
	LPVOID pNewAddress;
	DWORD dwOldProtect;
	byte pOldData[HOOK_DATA_SIZE];
	byte pNewData[HOOK_DATA_SIZE];
} HOOKDATA, *LPHOOKDATA;

BOOL __stdcall zHookInstall (LPVOID pAddressToHook,LPVOID pNewAddress,LPHOOKDATA pHookData)
{
	if (pAddressToHook == (LPVOID)0 || pAddressToHook == (LPVOID)-1 || pNewAddress == (LPVOID)0 || pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlZeroMemory(pHookData,sizeof(LPHOOKDATA));

	pHookData->pAddress = pAddressToHook;
	pHookData->pNewAddress = pNewAddress;

	if (!VirtualProtect(pHookData->pAddress, HOOK_DATA_SIZE, PAGE_EXECUTE_READWRITE ,&pHookData->dwOldProtect))
	{
		return FALSE;
	}

	RtlMoveMemory(pHookData->pOldData, pHookData->pAddress, HOOK_DATA_SIZE);

	*(byte*)pHookData->pNewData = 0xE9;
	*(DWORD*)((DWORD)pHookData->pNewData + 1) = (DWORD) pHookData->pNewAddress;
	*(DWORD*)((byte*)pHookData->pNewData + 1) = (DWORD) pHookData->pNewAddress - (DWORD)pHookData->pAddress - 5;

	return TRUE;
}

BOOL __stdcall zHookStart (LPHOOKDATA pHookData)
{
	if (pHookData == (LPVOID)0 || pHookData == (LPVOID)-1 || pHookData->pAddress == (LPVOID)0 || pHookData->pAddress == (LPVOID)-1 || pHookData->pNewAddress == (LPVOID)0 || pHookData->pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlMoveMemory (pHookData->pAddress, pHookData->pNewData, HOOK_DATA_SIZE);
	return TRUE;
}

BOOL __stdcall zHookStop (LPHOOKDATA pHookData)
{
	if (pHookData == (LPVOID)0 || pHookData == (LPVOID)-1 || pHookData->pAddress == (LPVOID)0 || pHookData->pAddress == (LPVOID)-1 || pHookData->pNewAddress == (LPVOID)0 || pHookData->pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlMoveMemory (pHookData->pAddress, pHookData->pOldData, HOOK_DATA_SIZE);
	return TRUE;
}

BOOL __stdcall zHookUninstall (LPHOOKDATA pHookData)
{
	if (pHookData == (LPVOID)0 || pHookData == (LPVOID)-1 || pHookData->pAddress == (LPVOID)0 || pHookData->pAddress == (LPVOID)-1 || pHookData->pNewAddress == (LPVOID)0 || pHookData->pNewAddress == (LPVOID)-1)
	{
		return FALSE;
	}

	RtlMoveMemory (pHookData->pAddress, pHookData->pOldData, HOOK_DATA_SIZE);

	if(!VirtualProtect(pHookData->pAddress, HOOK_DATA_SIZE, pHookData->dwOldProtect, NULL))
	{
		return FALSE;
	}

	RtlZeroMemory(pHookData,sizeof(LPHOOKDATA));

	return TRUE;
}

BOOL __stdcall HookToCallBack(void * lpAddress,void * lpCallBackProc, DWORD dwCallBackDataSize, void * lpCallBackData)
{
	if (!lpAddress)
	{
		return FALSE;
	}

	DWORD dwOldProtect;
	VirtualProtect(lpAddress, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	byte lpOldData[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	RtlMoveMemory(lpOldData, lpAddress, 5);

	void * lpNewAddress = VirtualAlloc(NULL, 0x60 + dwCallBackDataSize + 0x02 + 0x10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	byte lpNewData[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)(&lpNewData[0x01]) = (DWORD)lpNewAddress - (DWORD)lpAddress - 5;

/*
	0040101E      90            nop                                      ;  说明：以!开头的注释，操作数是需要重新计算的
	0040101F      90            nop                                      ;  说明：以*开头的注释，操作数是需要临时重定位的
	00401020      E8 00000000   call nop.00401025
	00401025      58            pop eax                                  ;   取出当前EIP到EAX
	00401026      83C0 23       add eax,23                               ;   计算代码偏移
	00401029      8B1C24        mov ebx,dword ptr ss:[esp]
	0040102C      8918          mov dword ptr ds:[eax],ebx               ;   将原返回地址写到计算好偏移的代码里，用以在调用函数之后恢复
	0040102E      3E:C70424 471>mov dword ptr ds:[esp],nop.00401047      ;  *修改原函数的返回地址，让它返回到我们这儿来                   [P + 0x12] = [P + 0x27]
	00401036      B8 00114000   mov eax,nop.00401100                     ;  *将原函数地址放到eax，恢复hook的数据                          [P + 0x17] = [lpAddress]
	0040103B      C600 55       mov byte ptr ds:[eax],55                 ;  *                                                             [P + 0x1D] = [lpOldData]
	0040103E      C740 01 8BECB>mov dword ptr ds:[eax+1],10B8EC8B        ;  *将原函数的前5个字节恢复回去                                  [P + 0x21] = [lpOldData + 0x01]
	00401045      FFE0          jmp eax                                  ;   跳转至原函数
	00401047      68 0D104000   push nop.0040100D                        ;   原返回地址修正 [自动重定位]
	0040104C      60            pushad                                   ;   保存原函数返回后的寄存器
	0040104D      B8 00114000   mov eax,nop.00401100                     ;  *将原函数地址放到eax，准备继续hook                            [P + 0x2E] = [lpAddress]
	00401052      C600 E9       mov byte ptr ds:[eax],0E9                ;  *                                                             [P + 0x34] = [lpNewData]
	00401055      C740 01 1BFFF>mov dword ptr ds:[eax+1],-0E5            ;  *修改原函数的前5个字节，继续hook                              [P + 0x38] = [lpNewData + 0x01]
	0040105C      68 80104000   push nop.00401080                        ;  *压入输出的信息                                               [P + 0x3D] = [P + 0x60]
	00401061      B8 F0104000   mov eax,nop.004010F0                     ;  *输出函数地址                                                 [P + 0x42] = zdLogMsg
	00401066      FFD0          call eax                                 ;  *调用输出信息的函数
	00401068      61            popad                                    ;   恢复原函数返回后的寄存器
	00401069      C3            retn                                     ;   返回
*/
	byte lpCode[] = {
		0xE8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x83, 0xC0, 0x23, 0x8B, 0x1C, 0x24, 0x89, 0x18, 0x3E, 0xC7,
		0x04, 0x24, 0x47, 0x10, 0x40, 0x00, 0xB8, 0x00, 0x11, 0x40, 0x00, 0xC6, 0x00, 0x55, 0xC7, 0x40,
		0x01, 0x8B, 0xEC, 0xB8, 0x10, 0xFF, 0xE0, 0x68, 0xCC, 0xCC, 0xCC, 0xCC, 0x60, 0xB8, 0x00, 0x11,
		0x40, 0x00, 0xC6, 0x00, 0xE9, 0xC7, 0x40, 0x01, 0x1B, 0xFF, 0xFF, 0xFF, 0x68, 0x80, 0x10, 0x40,
		0x00, 0xB8, 0xF0, 0x10, 0x40, 0x00, 0xFF, 0xD0, 0x61, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
	};

	byte lpNop[] = {
		0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
	};

	*(DWORD*)(&lpCode[0x12]) = (DWORD)lpNewAddress + 0x27;

	*(DWORD*)(&lpCode[0x17]) = (DWORD)lpAddress;
	*(byte*) (&lpCode[0x1D]) = (DWORD)lpOldData[0];
	*(DWORD*)(&lpCode[0x21]) = *(DWORD*)(&lpOldData[1]);

	*(DWORD*)(&lpCode[0x2E]) = (DWORD)lpAddress;
	*(byte*) (&lpCode[0x34]) = (DWORD)lpNewData[0];
	*(DWORD*)(&lpCode[0x38]) = *(DWORD*)(&lpNewData[1]);

	*(DWORD*)(&lpCode[0x3D]) = (DWORD)lpNewAddress + 0x60;
	*(DWORD*)(&lpCode[0x42]) = (DWORD)lpCallBackProc; // 当HOOK后的函数被调用时，自动被调用的函数，只有一个参数，可以是任何函数。

//	把做好重定位的lpCode写到申请的内存里
	RtlMoveMemory(lpNewAddress, lpCode, 0x60);

//	把回调参数写到lpCode里的数据里
	RtlMoveMemory((void*)((DWORD)lpNewAddress + 0x60), lpCallBackData, dwCallBackDataSize);

//	在后面加上一个Unicode字符串结束标记和14个nop
	RtlMoveMemory((void*)((DWORD)lpNewAddress + 0x60 + dwCallBackDataSize), lpNop, 0x10);


//	开始Hook
	RtlMoveMemory(lpAddress, lpNewData, 5);

	return TRUE;
}

BOOL __stdcall HookCallBackBefore(void * pAddress, DWORD dwCallBackDataSize, void * pCallBackData, void * pCallBackProc)
{
	if (!pAddress)
	{
		return FALSE;
	}

	DWORD dwOldProtect;
	VirtualProtect(pAddress, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	byte pOldData[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	RtlMoveMemory(pOldData, pAddress, 5);

	byte pStack[8192] = {0};
	*(void**)(&pStack[0x1FF0]) = pAddress;

	byte pDataEnd[16] = {0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

	byte pAsm[160] = {
		0x89, 0x2D, 0xFC, 0x10, 0x40, 0x00, 0x89, 0x25, 0xF8, 0x10, 0x40, 0x00, 0xBD, 0xE4, 0x10, 0x40,
		0x00, 0xBC, 0xE0, 0x10, 0x40, 0x00, 0x60, 0x68, 0x00, 0x11, 0x40, 0x00, 0xB8, 0x60, 0x10, 0x40,
		0x00, 0xFF, 0xD0, 0xB8, 0x80, 0x10, 0x40, 0x00, 0xC6, 0x00, 0x55, 0xC7, 0x40, 0x01, 0x8B, 0xEC,
		0xB8, 0x10, 0xA1, 0xF8, 0x10, 0x40, 0x00, 0x8B, 0x08, 0x89, 0x0D, 0xF4, 0x10, 0x40, 0x00, 0xC7,
		0x00, 0x58, 0x12, 0x40, 0x00, 0x61, 0x8B, 0x2D, 0xFC, 0x10, 0x40, 0x00, 0x8B, 0x25, 0xF8, 0x10,
		0x40, 0x00, 0xFF, 0x25, 0xF0, 0x10, 0x40, 0x00, 0x89, 0x2D, 0xFC, 0x10, 0x40, 0x00, 0x89, 0x25,
		0xF8, 0x10, 0x40, 0x00, 0xBD, 0xE4, 0x10, 0x40, 0x00, 0xBC, 0xE0, 0x10, 0x40, 0x00, 0x60, 0xB8,
		0x80, 0x10, 0x40, 0x00, 0xC6, 0x00, 0xE9, 0xC7, 0x40, 0x01, 0x7B, 0x01, 0x00, 0x00, 0x61, 0x8B,
		0x2D, 0xFC, 0x10, 0x40, 0x00, 0x8B, 0x25, 0xF8, 0x10, 0x40, 0x00, 0xFF, 0x25, 0xF4, 0x10, 0x40,
		0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
	};
/*
	00401200      892D FC104000       mov dword ptr ds:[4010FC],ebp            ;                                     [C + 0x02] = P + 0xFC
	00401206      8925 F8104000       mov dword ptr ds:[4010F8],esp            ;  备份栈顶栈底                       [C + 0x08] = P + 0xF8
	0040120C      BD E4104000         mov ebp,nop.004010E4                     ;                                     [C + 0x0D] = P + 0xE4
	00401211      BC E0104000         mov esp,nop.004010E0                     ;  修改栈顶栈底                       [C + 0x12] = P + 0xE0
	00401216      60                  pushad                                   ;  备份寄存器
	00401217      68 00114000         push nop.00401100                        ;  压入参数                           [C + 0x18] = pCallBackParam
	0040121C      B8 60104000         mov eax,nop.00401060                     ;                                     [C + 0x1D] = pCallBackAddress
	00401221      FFD0                call eax                                 ;  调用通知回调函数
	00401223      B8 80104000         mov eax,nop.00401080                     ;                                     [C + 0x23] = pAddress
	00401228      C600 55             mov byte ptr ds:[eax],55                 ;                                     [C + 0x2A] = pOldData[0]
	0040122B      C740 01 8BECB810    mov dword ptr ds:[eax+1],10B8EC8B        ;  恢复原函数前5个字节，暂停HOOK      [C + 0x2E] = pOldData[1]
	00401232      A1 F8104000         mov eax,dword ptr ds:[4010F8]            ;                                     [C + 0x33] = P + 0xF8
	00401237      8B08                mov ecx,dword ptr ds:[eax]
	00401239      890D F4104000       mov dword ptr ds:[4010F4],ecx            ;  备份原函数返回地址                 [C + 0x3B] = P + 0xF4
	0040123F      C700 58124000       mov dword ptr ds:[eax],nop.00401258      ;  修改原函数的返回地址               [C + 0x41] = C + 0x58
	00401245      61                  popad                                    ;  恢复寄存器
	00401246      8B2D FC104000       mov ebp,dword ptr ds:[4010FC]            ;                                     [C + 0x48] = P + 0xFC
	0040124C      8B25 F8104000       mov esp,dword ptr ds:[4010F8]            ;  恢复栈顶栈底                       [C + 0x4E] = P + 0xF8
	00401252    ^ FF25 F0104000       jmp dword ptr ds:[4010F0]                ;  跳转回原函数执行                   [C + 0x54] = P + 0xF0
	00401258      892D FC104000       mov dword ptr ds:[4010FC],ebp            ;                                     [C + 0x5A] = P + 0xFC
	0040125E      8925 F8104000       mov dword ptr ds:[4010F8],esp            ;  备份栈顶栈底                       [C + 0x60] = P + 0xF8
	00401264      BD E4104000         mov ebp,nop.004010E4                     ;                                     [C + 0x65] = P + 0xE4
	00401269      BC E0104000         mov esp,nop.004010E0                     ;  修改栈顶栈底                       [C + 0x6A] = P + 0xE0
	0040126E      60                  pushad                                   ;  备份寄存器
	0040126F      B8 80104000         mov eax,nop.00401080                     ;                                     [C + 0x70] = pAddress
	00401274      C600 E9             mov byte ptr ds:[eax],0E9                ;                                     [C + 0x76] = pNewData[0]
	00401277      C740 01 7B010000    mov dword ptr ds:[eax+1],17B             ;  修改原函数前5个字节，继续HOOK      [C + 0x7A] = pNewData[1]
	0040127E      61                  popad                                    ;  恢复寄存器
	0040127F      8B2D FC104000       mov ebp,dword ptr ds:[4010FC]            ;                                     [C + 0x81] = P + 0xFC
	00401285      8B25 F8104000       mov esp,dword ptr ds:[4010F8]            ;  恢复栈顶栈底                       [C + 0x87] = P + 0xF8
	0040128B    - FF25 F4104000       jmp dword ptr ds:[4010F4]                ;  跳转回原函数返回地址               [C + 0x8D] = P + 0xF4
*/

	DWORD dwSize = 8192 + dwCallBackDataSize + 16 + 160;
//	               堆栈  数据 + 结尾标记           代码

	byte * lpCode = (byte*)VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!lpCode)
	{
		return FALSE;
	}

	RtlMoveMemory(lpCode, pStack, 8192);
	RtlMoveMemory(&lpCode[8192], pCallBackData, dwCallBackDataSize);
	RtlMoveMemory(&lpCode[8192 + dwCallBackDataSize], pDataEnd, 16);
	RtlMoveMemory(&lpCode[8192 + dwCallBackDataSize + 16], pAsm, 160);

	byte * C = (byte*)((DWORD)lpCode + dwSize - 160);

	byte pNewData[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)(&pNewData[1]) = (DWORD)C - (DWORD)pAddress - 5;

	*(DWORD*)(&C[0x02]) = (DWORD)(&lpCode[0x1FFC]);
	*(DWORD*)(&C[0x08]) = (DWORD)(&lpCode[0x1FF8]);
	*(DWORD*)(&C[0x0D]) = (DWORD)(&lpCode[0x1FE4]);
	*(DWORD*)(&C[0x12]) = (DWORD)(&lpCode[0x1FE0]);

	*(DWORD*)(&C[0x18]) = (DWORD)(&lpCode[8192]);
	*(DWORD*)(&C[0x1D]) = (DWORD)(pCallBackProc);

	*(DWORD*)(&C[0x24]) = (DWORD)(pAddress);
	*(byte*) (&C[0x2A]) = pOldData[0x00];
	*(DWORD*)(&C[0x2E]) = *(DWORD*)(&pOldData[0x01]);
	*(DWORD*)(&C[0x33]) = (DWORD)(&lpCode[0x1FF8]);

	*(DWORD*)(&C[0x3B]) = (DWORD)(&lpCode[0x1FF4]);
	*(DWORD*)(&C[0x41]) = (DWORD)(&C[0x58]);

	*(DWORD*)(&C[0x48]) = (DWORD)(&lpCode[0x1FFC]);
	*(DWORD*)(&C[0x4E]) = (DWORD)(&lpCode[0x1FF8]);
	*(DWORD*)(&C[0x54]) = (DWORD)(&lpCode[0x1FF0]);
	*(DWORD*)(&C[0x5A]) = (DWORD)(&lpCode[0x1FFC]);
	*(DWORD*)(&C[0x60]) = (DWORD)(&lpCode[0x1FF8]);
	*(DWORD*)(&C[0x65]) = (DWORD)(&lpCode[0x1FE4]);
	*(DWORD*)(&C[0x6A]) = (DWORD)(&lpCode[0x1FE0]);
	
	*(DWORD*)(&C[0x70]) = (DWORD)(pAddress);
	*(byte*) (&C[0x76]) = pNewData[0x00];
	*(DWORD*)(&C[0x7A]) = *(DWORD*)(&pNewData[0x01]);

	*(DWORD*)(&C[0x81]) = (DWORD)(&lpCode[0x1FFC]);
	*(DWORD*)(&C[0x87]) = (DWORD)(&lpCode[0x1FF8]);
	*(DWORD*)(&C[0x8D]) = (DWORD)(&lpCode[0x1FF4]);

	RtlMoveMemory(pAddress, pNewData, 5);

	return TRUE;
}
