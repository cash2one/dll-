#ifndef LLOG_H
#define LLOG_H

//这两个开关打开了会不停上报数据
#define LIN_DBG 0					
#define LIN_DBG1 0


void __stdcall Log(const char * str);
void __stdcall NoDebugLog(const char * str);


#endif