#ifndef LolHelper_Adobe_Air_dll_Check_h
#define LolHelper_Adobe_Air_dll_Check_h

#include <Windows.h>

typedef BOOL (*HookFuncType)();
HookFuncType IsSupportAdobeAirDllVersion(const char* path);

#endif