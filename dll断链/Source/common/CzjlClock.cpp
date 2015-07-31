#include "CzjlClock.h"
#include <sys\utime.h>



CzjlClock::CzjlClock(void)
{
	m_StartTime=0;
}


CzjlClock::~CzjlClock(void)
{
}

void CzjlClock::SetStartTimeNow()
{
	time_t ttime;
	time(&ttime);
	m_StartTime=ttime;
}

CTimeSpan CzjlClock::CmpNow()
{
	time_t ttimenow;
	time(&ttimenow);
	CTime cTimeNow=ttimenow;

	return cTimeNow-m_StartTime;
}
