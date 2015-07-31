#ifndef REMOTETHREADINJECT_H
#define REMOTETHREADINJECT_H

#include <windows.h>

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId, const char* dllName);
DWORD GetProcessID(char *ProcessName);
int EnableDebugPriv(const char * name);

#endif