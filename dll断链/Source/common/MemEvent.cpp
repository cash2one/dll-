#include "MemEvent.h"



//Mem
void* CMemEvent::GetMemShare1(CStringA szName,unsigned int Size)
{
	void* pResult=0;
	m_hMap=CreateFileMappingA((HANDLE)-1,0,PAGE_READWRITE,0,Size+4,szName);
	int LE_CReateMap=GetLastError();
	if (m_hMap!=INVALID_HANDLE_VALUE)
	{
		pResult=MapViewOfFile(m_hMap,FILE_MAP_WRITE,0,0,0);
	}else{
		return 0;
	}
	if (pResult && LE_CReateMap!=ERROR_ALREADY_EXISTS)
	{//第一次创建
		//初始化数据
		RtlZeroMemory(pResult,Size+4);
	}else{
	}
	return pResult;
}



DWORD WINAPI CMemEvent::ThreadEvent( LPVOID lp )
{
	CMemEvent* pThis=(CMemEvent*)lp;
	DbgPrint("ThreadEvent");
	while(pThis->bRun)
	{
		DbgPrint("等待客户端信息中...",pThis->m_hEvent);
		WaitForSingleObject(pThis->m_hEvent,INFINITE);
		

		int Size=(*(int*)(pThis->m_pData));
		DbgPrint("收到信息,处理  Len=%06x",Size);
		if (pThis->m_pFunCallback)
		{
			(*pThis->m_pFunCallback)((pThis->m_pData+4),Size);
			
		}
		ResetEvent(pThis->m_hEvent);
	}

	return 0;
}
bool CMemEvent::StartServer( pCallBackEvent pFun )
{

		if (m_hEvent)
		{
			return 1;
		}
		m_hEvent=CreateEventA(0,1,0,(char*)m_szEventName.AllocSysString());
	//	GetLastErrorEx();
	//	SetEvent(m_hEvent);
		if (m_hEvent && m_hThread==0)
		{
			m_pFunCallback=pFun;
			m_hThread=CreateThread(0,0,CMemEvent::ThreadEvent,this,0,0);
			return 1;
		}else{
			DbgPrint("CMemEvent::StartServer Error");
		}
		return 0;
	
}


bool CMemEvent::SendData( char* pBuffer,unsigned int Len )
{
	if (m_hEvent==0)
	{
		m_hEvent=CreateEventA(0,1,0,(char*)m_szEventName.AllocSysString());
		if (m_hEvent==0)
		{
			return 0;
		}
	}

	if (WAIT_TIMEOUT==WaitForSingleObject(m_hEvent,100))
	{
		if (m_pData && Len<m_MemSize)
		{
			*(int*)(m_pData)=Len;
			memcpy(   m_pData+4,pBuffer,Len);
			SetEvent(m_hEvent);
			DbgPrint("SendData成功,Len=%06x",Len);
			return 1;
		}
		DbgPrint("error SendData失败,Server没有处理结束");
	}
	return 0;

}

CMemEvent::CMemEvent( char* pName,unsigned int MemSize )
{
	m_szEventName=pName;
	m_MemSize=MemSize;
	m_pData=(char*)GetMemShare1(pName,MemSize);
	m_pFunCallback=0;
	m_hMap=0;
	m_hEvent=0;
	m_hThread=0;
	bRun=1;
};

CMemEvent::~CMemEvent( void )
{
	bRun=0;
	TerminateThread(m_hThread,0);
	if (UnmapViewOfFile(m_pData)==0)
	{
		DbgPrint("error UnmapViewOfFile %d",GetCurrentThreadId());
	}
	CloseHandle(m_hEvent);
	CloseHandle(m_hMap);
}

