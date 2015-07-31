#include "CzjlStatusBar.h"
#include <windows.h>
#include <commctrl.h>
#include <atltime.h>



DWORD WINAPI TheadUpdateStatusBarTitle(LPVOID lp)
{
	CzjlStatusBar* p =(CzjlStatusBar*)lp;

//	thsClock.SetStartTimeNow();
	while(1)
	{
		stSBMsg msg;
		EnterCriticalSection(&p->m_CSMsg);
		//GetMsg
		vector<stSBMsg>::iterator pos=p->m_vMsg.begin();
		if (pos!=p->m_vMsg.end())
		{
			msg=*pos;
			p->m_vMsg.erase(pos);
		}
		LeaveCriticalSection(&p->m_CSMsg);

		if (msg.szMsg!="")
		{//空的
			if (p->m_hStatusBar)
			{
				CStringA szOut;
				CTime time(msg.Time);
				szOut.Format("%02d:%02d:%02d - %s",time.GetHour(),time.GetMinute(),time.GetSecond(),msg.szMsg);
				CStringW str=szOut;
				SendMessageA(p->m_hStatusBar,SB_SETTEXT,msg.IndexPart,(LPARAM)str.GetBuffer());
			}
			Sleep(2000);//2秒一个消息
		}else{
			Sleep(200);
		}
	}
	return 1;
}




CzjlStatusBar::CzjlStatusBar()
{
	InitializeCriticalSection(&m_CSMsg);
	m_hStatusBar=0;
}


CzjlStatusBar::~CzjlStatusBar(void)
{
}


bool CzjlStatusBar::CreateStatusbar( HWND hParent,int Parts[],int CountParts )
{
	m_hStatusBar=CreateWindowExW(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,\
		L"msctls_statusbar32",\
		L"",\
		SBT_TOOLTIPS | SBARS_TOOLTIPS | CCS_BOTTOM | CCS_NOMOVEY | CCS_TOP | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,\
		0,0,0,0,hParent,(HMENU)0,NULL,NULL);
	SendMessage(m_hStatusBar,SB_SETPARTS,CountParts,(LPARAM)Parts);
	CreateThread(0,0,TheadUpdateStatusBarTitle,this,0,0);
	return m_hStatusBar;
}


void CzjlStatusBar::AddMessage( int IndexPart/*=0*/,const char *fmt, ... )
{
	char buf[1024*100]={0};
	va_list arg = (va_list)((char*)(&fmt) + 4); 
	vsprintf_s(buf, fmt, arg);
	CStringA szbuf=buf;

	EnterCriticalSection(&m_CSMsg);
	stSBMsg add;
	time(&add.Time);
	add.IndexPart=IndexPart;
	add.szMsg=szbuf;
	m_vMsg.push_back(add);
	LeaveCriticalSection(&m_CSMsg);
}
