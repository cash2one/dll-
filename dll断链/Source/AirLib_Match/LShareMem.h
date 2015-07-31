#ifndef LSHAREMEM_H
#define LSHAREMEM_H

#include <Windows.h>

BOOL LoLMemShareInit(int size);
BOOL LoLMemShareExit();
void* LoLMemShareMap(HANDLE *ph, int size);
void LoLMemShareUnMap(void* pBuf, HANDLE h);

#endif