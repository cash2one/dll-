#include "CallStat.h"
#include <winbase.h>
#include <zjlcommon.h>


CCallStat::CCallStat(CStringA sz)
{
	szName=sz;
	iCount=0;
	//InitializeCriticalSection(&m_cs);
	m_Range.MinShowCount=0;
	m_Range.MaxShowCount=0xFFFFFFFF;
}


CCallStat::~CCallStat(void)
{
}

unsigned int CCallStat::AddCount( int Flag )
{
	//EnterCriticalSection(&m_cs);
	iCount++;
	for (unsigned int i=0;i<m_vData.size();i++)
	{
		if (m_vData[i].Flag==Flag)
		{
			m_vData[i].Count++;
			m_vData[i].bChange=1;
			return m_vData[i].Count;
		}
	}
	stActionStat Add;
	Add.Flag=Flag;
	Add.Count=1;
	m_vData.push_back(Add);
	//LeaveCriticalSection(&m_cs);
	return 1;
}

CStringA CCallStat::PrintToDebugView()
{
		//EnterCriticalSection(&m_cs);
	for (int j=m_vData.size()-1 ; j>0 ; j--)
	{
		for(int i=0 ; i<j ; i++)
		{
			if (m_vData[i].Count>m_vData[i+1].Count)
			{
				stActionStat stTemp=m_vData[i];
				m_vData[i]=m_vData[i+1];
				m_vData[i+1]=stTemp;
			}
		}
	}

	CStringA szOut;
	szOut.Format("************************** [%s] 统计:\r\n",szName);
	for (unsigned int i=0;i<m_vData.size();i++)
	{
		CStringA szTemp;
		szTemp.Format("*Index:%03d  |  Flag=0x%08x , Count=%08d",i+1,m_vData[i].Flag,m_vData[i].Count);

		if (m_vData[i].bChange)
		{
			if (m_vData[i].Count==1)
			{
				szTemp+="  <-*New!";
			}else{
				szTemp+="  <-*Change!";
			}
			m_vData[i].bChange=0;
		}

		szTemp+="\r\n";
		szOut+=szTemp;
	}
	m_vLastData=m_vData;
	//LeaveCriticalSection(&m_cs);
	return szOut;
}

CStringA CCallStat::PrintToDebugViewNew()
{
		//EnterCriticalSection(&m_cs);
	for (int j=m_vData.size()-1 ; j>0 ; j--)
	{
		for(int i=0 ; i<j ; i++)
		{
			if (m_vData[i].Count>m_vData[i+1].Count)
			{
				stActionStat stTemp=m_vData[i];
				m_vData[i]=m_vData[i+1];
				m_vData[i+1]=stTemp;
			}
		}
	}

	CStringA szOut;
	szOut.Format("************************** [%s] 统计,总数%d:\r\n",szName,m_vData.size());

	for (unsigned int i=0;i<m_vData.size();i++)
	{

		if ( m_vData[i].bChange && m_vData[i].Count>=m_Range.MinShowCount && m_vData[i].Count<=m_Range.MaxShowCount  )
		{
			CStringA szTemp;
			szTemp.Format("*Index:%03d  |  Flag=0x%08x , Count=%08d",i+1,m_vData[i].Flag,m_vData[i].Count);
			if (m_vData[i].Count==1)
			{
				szTemp+="  <-*New!";
			}else{
				szTemp+="  <-*Change!";
			}

			szTemp+="\r\n";
			szOut+=szTemp;
			m_vData[i].bChange=0;
		}
	}
	m_vLastData=m_vData;
	//LeaveCriticalSection(&m_cs);
	return szOut;
}

void CCallStat::SerRange( unsigned int MinCount/*=0*/,unsigned int MaxCount/*=0xFFFFFFFF*/ )
{
	m_Range.MinShowCount=MinCount;
	m_Range.MaxShowCount=MaxCount;
}	
