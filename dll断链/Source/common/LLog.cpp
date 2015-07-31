#include <stdio.h>

#include <string>
#include "LLog.h"


void __stdcall Log(const char * str)
{
#if  0//LIN_DBG
	if (str == 0) return;
	FILE *fpText = fopen("C:\\test.txt", "a");
	if (fpText) {
		fprintf(fpText, "%s\n", str);
		fflush(fpText);
		fclose(fpText);
	}
#endif
}

void __stdcall NoDebugLog(const char * str)
{
#if  1
	if (str == 0) return;
	FILE *fpText = fopen("C:\\test.txt", "a");
	if (fpText) {
		fprintf(fpText, "%s\n", str);
		fflush(fpText);
		fclose(fpText);
	}
#endif
}

