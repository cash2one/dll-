#ifndef LOLHELPERLOCALLOG_H
#define LOLHELPERLOCALLOG_H

#include "logfile/log_file.h"
#include "logfile/crypt.hpp"
#include "logfile/LOLInfo.h"

#include "LSemaphore.h"
#include "LShareMem.h"
#include "llog.h"

#include <string>
extern BOOL InitFlag;
void LoLHelperLocalLogInit(const std::string &str);
void ShowLastError(LPSTR lpszFunction = "");

#define LoLLocalLogInfo(str, ...) do {if (InitFlag) GenericLog(LogInfo, "[LolHelper-info]"##str, __VA_ARGS__);}while (0);
#define LoLLocalLogWarn(str, ...) do {if (InitFlag) GenericLog(LogWarning, "[LolHelper-warning]"##str, __VA_ARGS__);}while (0);
#define LoLLocalLogErr(str, ...)  do {if (InitFlag) { ShowLastError(); GenericLog(LogError, "[LolHelper-err]"##str, __VA_ARGS__);}}while (0);


void ShareMemSendErr(ReportType type);

#endif