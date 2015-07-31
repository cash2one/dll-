#pragma once
#include <vector>
#include "atlstr.h"

using namespace std;



struct stActionStat{
	int Flag;
	unsigned int Count;

	bool bChange;
	stActionStat(){
		Flag=0;
		Count=0;
		bChange=1;
	}
};


class CCallStat
{
public:
	CCallStat(CStringA sz);
	~CCallStat(void);

unsigned int AddCount(int Flag);
unsigned int GetTotalAdd(){return iCount;};
CStringA PrintToDebugView();
CStringA PrintToDebugViewNew();

void SerRange(unsigned int MinCount=0,unsigned int MaxCount=0xFFFFFFFF);

private:
CStringA szName;
unsigned int iCount;
 vector<stActionStat> m_vData;
  vector<stActionStat> m_vLastData;
  //CRITICAL_SECTION m_cs;
  struct {
	  unsigned int MinShowCount;
	  unsigned int MaxShowCount;
  }m_Range;
};

