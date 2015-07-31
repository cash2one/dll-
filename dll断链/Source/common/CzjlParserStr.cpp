#include "CzjlParserStr.h"

int CzjlParserStr::ParserStrSectionsByTag( CStringA szTagBegin,CStringA szTagEnd )
{
	int _begin=0,_end=0;
	bool bFrist=1;
	while(1)
	{
		if (bFrist)
		{//第一个
			_begin=m_szData.Find(szTagBegin,0);
			bFrist=0;
		}else{//
			_begin=m_szData.Find(szTagBegin,_end+szTagEnd.GetLength());//开头
		}
		if (_begin==-1)
		break;

		_end=m_szData.Find(szTagEnd,_begin+szTagBegin.GetLength());//结尾
		if (_end==-1)//结尾
		_end=m_szData.GetLength();
		CStringA szAdd;
		_begin+=szTagBegin.GetLength();
		szAdd=m_szData.Mid(_begin,_end-_begin);
		if (szAdd.GetLength()>0)
		{
			m_vWords.push_back(szAdd);
		}
	}

	return m_vWords.size();
}

CStringA CzjlParserStr::GetElementByIndex( CStringA szBegin,CStringA szEnd,unsigned int WordsIndex )
{
	CStringA szTemp=GetStrByIndex(WordsIndex);
	if (szTemp=="")
	{
			return "";
	}
	CzjlParserStr Temp(szTemp);
	return Temp.FindStr(szBegin,szEnd);
}

CStringA CzjlParserStr::GetStrByIndex( unsigned int WordsIndex )
{
	if (WordsIndex<m_vWords.size())
	{
		return m_vWords[WordsIndex];
	}else{
			return "";
	}
}

bool CzjlParserStr::FindNumber( CStringA szTagBegin,int* pResult )
{
		int _begin=0,_end=0;
		_begin=m_szData.Find(szTagBegin,0);
		if (_begin==-1)
		return 0;

		_begin+=szTagBegin.GetLength();
		CStringA sz=m_szData.Mid(_begin,m_szData.GetLength()-_begin);
		*pResult=atoi(sz.GetBuffer());
		return 1;
}

CzjlParserStr::~CzjlParserStr()
{

}

CStringA CzjlParserStr::FindStr( CStringA szTagBegin,CStringA szTagEnd )
{
	int _begin=0,_end=0;
	_begin=m_szData.Find(szTagBegin,0);
	if (_begin==-1)
		return "";

	_end=m_szData.Find(szTagEnd,_begin+szTagBegin.GetLength());
	if (_end==-1)
		return "";

	_begin+=szTagBegin.GetLength();
	CStringA szResult=m_szData.Mid(_begin,_end-_begin);
	return szResult;
}

