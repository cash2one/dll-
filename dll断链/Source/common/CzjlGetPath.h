#pragma once
#include <atlstr.h>


class CzjlGetPath
{
public:
    CzjlGetPath(void);
    ~CzjlGetPath(void);
  static  CString GetTempPath(CStringA szPathName);
 static   CString GetDeskopPath();
  static  CString GetLocalPath();
static bool CreatePath(CStringW szPath);
};

