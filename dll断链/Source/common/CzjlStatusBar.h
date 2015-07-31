#pragma once
#include "windows.h"
#include <windef.h>
#include "atlstr.h"
#include <vector>
using namespace std;

struct stSBMsg{
	time_t Time;
	unsigned int IndexPart;
	CStringA szMsg;
};

class CzjlStatusBar
{
public:
	CzjlStatusBar();
	~CzjlStatusBar(void);

	bool CreateStatusbar(HWND hParent,int Parts[],int CountParts);
	void AddMessage(int IndexPart,const char *fmt, ...);
	vector<stSBMsg> m_vMsg;
	CRITICAL_SECTION m_CSMsg;
	HWND m_hStatusBar;
private:
};

