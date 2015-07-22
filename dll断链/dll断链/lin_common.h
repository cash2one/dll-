#ifndef LIN_COMMON_H
#define LIN_COMMON_H

#include <windows.h>
#include <string>
#include <stdio.h>

namespace lin_common 
{
#ifndef offsetof
#define offsetof(s,m) (size_t)&(((s *)0)->m)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ( \
	(type *)( (char *)(ptr) - offsetof(type,member) )\
	)
#endif

	struct _peb_ldr_data
	{
		UINT Length;
		UINT Initialized;
		PVOID SsHandle;
		_LIST_ENTRY InLoadOrderModuleList;				//Ö¸Ïò LDR_DATA_TABLE_ENTRY 
		_LIST_ENTRY InMemoryOrderModuleList;
		_LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		PVOID ShutdownInProgress;
		PVOID ShutdownThreadId;
	};

	struct _UNICODE_STRING
	{
		USHORT Length  ;
		USHORT MaximumLength ;
		PVOID  Buffer;
	};

	struct LDR_DATA_TABLE_ENTRY 
	{
		_LIST_ENTRY InLoadOrderLinks;
		_LIST_ENTRY	InMemoryOrderLinks;
		_LIST_ENTRY InInitializationOrderLinks;
		PVOID		DllBase;
		PVOID		EntryPoint;
		UINT        SizeOfImage;
		_UNICODE_STRING FullDllName;
		_UNICODE_STRING BaseDllName;
		UINT		Flags;
		USHORT		LoadCount;
		USHORT		TlsIndex;
		_LIST_ENTRY HashLinks;
		PVOID		SectionPointer;
		UINT		TimeDateStamp;
		PVOID		LoadedImports;
		PVOID		EntryPointActivationContext;	//_ACTIVATION_CONTEXT
		PVOID		PatchInformation;
		_LIST_ENTRY ForwarderLinks;
		_LIST_ENTRY ServiceTagLinks;
		_LIST_ENTRY StaticLinks;
		PVOID		ContextInformation;
		UINT		OriginalBase;
		_LARGE_INTEGER LoadTime;
	};

	//BOOL IsEqualUnicodeString(const _UNICODE_STRING &uStr1, const _UNICODE_STRING &uStr2);
	BOOL IsEqualUnicodeString(const _UNICODE_STRING &uStr1, const std::wstring &wStr);
}

#endif