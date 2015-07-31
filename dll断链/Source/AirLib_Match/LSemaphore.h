#ifndef LSEMAPHORE_H
#define LSEMAPHORE_H

#include <Windows.h>

BOOL LoLMemShareAccessInit();
BOOL LoLMemShareAccessExit();
HANDLE LoLMemShareAccessIn();
BOOL LoLMemShareAccessOut(HANDLE h);

#endif