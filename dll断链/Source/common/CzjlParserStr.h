#pragma once
#include <atlstr.h>
#include <vector>
using namespace std;

class CzjlParserStr
{
public:
	CzjlParserStr();
	CzjlParserStr(CStringA sz){m_szData=sz;m_vWords.clear();};
CzjlParserStr(CStringW sz){m_szData=sz;m_vWords.clear();};
	~CzjlParserStr();

	void Attath(CStringA sz){m_szData=sz;m_vWords.clear();};


	

	//查找特定数据
	bool FindNumber(CStringA szTagBegin,int* pResult);
	CStringA FindStr(CStringA szTagBegin,CStringA szTagEnd);


	//分段查找数据
	int ParserStrSectionsByTag(CStringA szTagBegin,CStringA szTagEnd);
	CStringA GetElementByIndex(CStringA szBegin,CStringA szEnd,unsigned int WordsIndex);
	CStringA GetStrByIndex(unsigned int WordsIndex);
	
private:
	CStringA m_szData;
	vector<CStringA> m_vWords;
};