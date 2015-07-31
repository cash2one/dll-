#pragma once
#include <CzjlHook.h>
#include <CallStat.h>

bool Spy_NtAllocateVirtualMemory();
bool Spy_NtVirtualAlloc();
bool Spy_NtHeapAlloc();

//HeapAlloc
LPVOID
	WINAPI
	MyHeapAlloc(
	__in HANDLE hHeap,
	__in DWORD dwFlags,
	__in SIZE_T dwBytes
	);



//NtVirtualAlloc
LPVOID
	WINAPI
	MyVirtualAlloc(
	__in_opt LPVOID lpAddress,
	__in     SIZE_T dwSize,
	__in     DWORD flAllocationType,
	__in     DWORD flProtect
	);
//NtAllocateVirtualMemory
typedef  int		(*pNtAllocateVirtualMemory) (
	__in HANDLE ProcessHandle,
	__inout PVOID *BaseAddress,
	__in ULONG_PTR ZeroBits,
	__inout PSIZE_T RegionSize,
	__in ULONG AllocationType,
	__in ULONG Protect
	);
int	MyNtAllocateVirtualMemory (
	__in HANDLE ProcessHandle,
	__inout PVOID *BaseAddress,
	__in ULONG_PTR ZeroBits,
	__inout PSIZE_T RegionSize,
	__in ULONG AllocationType,
	__in ULONG Protect
	);
