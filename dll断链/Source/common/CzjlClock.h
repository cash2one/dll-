#pragma once
#include <atltime.h>
//#include <afx.h>
class CzjlClock
{
public:
	CzjlClock(void);
	~CzjlClock(void);

	void SetStartTimeNow();
	CTimeSpan CmpNow();


	CTime m_StartTime;
};

