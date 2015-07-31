#ifndef HOOK_H
#define HOOK_H

#include <Windows.h>

BOOL HookFunc(void **pOldFunc, void *pNewFunc);
void HookStart();

#endif