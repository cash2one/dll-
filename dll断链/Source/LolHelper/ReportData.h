#ifndef REPORTDATA_H
#define REPORTDATA_H

#include <Windows.h>

#include <string>

#include "json/json.h"
#include "LOL/LoLDef.h"
#include "logfile/LOLInfo.h"

static const char* LolHelperVersion = "1.0.0.0";

BOOL ReportData(const stLOLGame &GameData);
BOOL ReportLog(unsigned int type, const std::string &msg = "");
BOOL IsReportWxClientReady();

#endif