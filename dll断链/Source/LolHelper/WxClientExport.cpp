#include "StdAfx.h"
#include <Windows.h>

#include "logfile/LoLHelperLocalLog.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////// COMMAND HANDLERS ///////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////A SET OF INTERFACES FOR OLD PLUGIN STANDARD//////////////////// 
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 

typedef int (__stdcall *RequestClientProc)(int operation_id, void* wParam, void* lParam);

typedef bool (__stdcall *_callbackinfo)(const char* instr, char* outdata, int* outlen);

static _callbackinfo fee_callback = NULL;

bool _stdcall clientsocketrecSvrIP(const char* indata, int inlen) 
{
	return true;
}

char __stdcall getdllinfo(int a1, int a2, int a3, int a4)
{
	return 1;
}

char _stdcall checkfun(const char* instr) 
{
	return 1;
}

typedef char (_stdcall* pfnsetcallbackinfo)(void* indata);


char _stdcall setcallbackinfofirst(void* indata) 
{
	return 1;
}


static volatile pfnsetcallbackinfo _setcallbackinfo= setcallbackinfofirst;

char _stdcall setcallbackinfonext(void* indata) 
{
	return 1;
}

char _stdcall setcallbackinfo(void* indata) 
{
	return 1;
}


bool _stdcall testcall(const char* indata, char* outdata) 
{
	return true;
}

void  __stdcall dlllockformcreate(void* inform)
{
}

void _stdcall dllunlockform(void) 
{
}


void _stdcall dlllockform(void) 
{
}

void _stdcall dlladvgetdata(void) 
{
}

void _stdcall dllendsession(void) 
{
}

bool _stdcall dllgetdata(const char* instr, char* outstr, int* outlen) 
{
	return true;
}

void _stdcall dllgjlock(void) 
{
}

void _stdcall dllgjunlock(void) 
{
}

const char* _stdcall wxdllfuncall(int function_index, int tag, int wparam, int lparam, void* data) 
{
	return NULL;
}

unsigned short _stdcall AdvDLLMsg(const char* title, const char* url, const char* text, int show_time) 
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////// A NEW SET OF PLUGIN INTERFACES //////////////////////// 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//using namespace wx::clt_aidex::plugin;

static RequestClientProc gRequestProc = NULL;


int __stdcall plugin_init(RequestClientProc request_proc)
{
	return 0;
}

void __stdcall plugin_uninit()
{
}

void __stdcall plugin_event_notify(int event_id, void* wParam, void* lParam)
{
	// 如果是用户ID
	//LoLLocalLogInfo("%s %d", __FUNCTION__, event_id);
	if ( 1017 == event_id && wParam && lParam)
	{
		/*LoLLocalLogInfo("%s", wParam);
		MessageBoxA(0, (char *)wParam, "", MB_OK);*/
	}
}

int __stdcall get_plugin_name(char* name_buffer, int size, int* need_size)
{
	return 0;
}

