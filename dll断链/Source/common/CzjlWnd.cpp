#include "CzjlWnd.h"
#include <windef.h>
#include "atlstr.h"
#include <commctrl.h>
#include <vector>



CStringA CzjlWnd::GetText()
{
	CStringA strResult;
	GetWindowTextA(m_hWnd,strResult.GetBuffer(1024),1024);
	strResult.ReleaseBuffer();
	return strResult;
}

CStringW CzjlWnd::GetDlgText( int ItemID )
{
	CStringW strResult;
	GetDlgItemTextW(m_hWnd,ItemID,strResult.GetBuffer(),1024*10);
	strResult.ReleaseBuffer();
	return strResult;
}


void CzjlWnd::SetPos( int Current,int Max )
{
	float x=((float)(Current)/(float)Max);
	int ProgressPos=x*100;
	SendMessage(m_hWnd,PBM_SETPOS,ProgressPos,0);
}

void CzjlWnd::SetTextA( CStringA sz )
{
	CStringA szBuffer;
	GetWindowTextA(m_hWnd,szBuffer.GetBuffer(1024),1024);
	szBuffer.ReleaseBuffer();
	if (sz!=szBuffer)
	{
		SetWindowTextA(m_hWnd,sz.GetBuffer());
	}
}

void CzjlWnd::SetTextW( CStringW sz )
{
	CStringW szBuffer;
	GetWindowTextW(m_hWnd,szBuffer.GetBuffer(1024),1024);
	szBuffer.ReleaseBuffer();
	if (sz!=szBuffer)
	{
		SetWindowTextW(m_hWnd,sz.GetBuffer());
	}
}

int CzjlWnd::CB_GetCount()
{
	unsigned int i=SendMessage(m_hWnd,CB_GETCOUNT,0,0);
	return i;
}

void CzjlWnd::CB_Insert( CStringA szData )
{
	int index=CB_GetCount();
	SendMessageA(m_hWnd,CB_INSERTSTRING,index,(WPARAM)szData.GetBuffer());
}

void CzjlWnd::CB_SetCursel( int Index )
{
	if (Index!=CB_GetCursel())
	{
		SendMessage(m_hWnd,CB_SETCURSEL,Index,0);
	}
	
}


bool CzjlWnd::BM_GetCheck()
{
	return (SendMessage(m_hWnd,BM_GETCHECK,0,0) == BST_CHECKED );
}

void CzjlWnd::BM_SetCheck( bool bCheck )
{
	if (bCheck!=BM_GetCheck())
	{
		if ( bCheck )
		{
			SendMessage(m_hWnd,BM_SETCHECK,BST_CHECKED,0);
		}else{
			SendMessage(m_hWnd,BM_SETCHECK,BST_UNCHECKED,0);
		}
	}

}

int CzjlWnd::CB_GetCursel()
{
	return SendMessage(m_hWnd,CB_GETCURSEL,0,0);
}

HWND CzjlWnd::CreateDlgInTab( HWND hTab,int DlgID,CString Title,int Index,DLGPROC proc)
{
	RECT rectTab;
	POINT ptPos;
	GetWindowRect(hTab,&rectTab);
	ptPos.x=rectTab.left;
	ptPos.y=rectTab.top;
	ScreenToClient(hTab,&ptPos);
	
	TCITEM addtic;
	RtlZeroMemory(&addtic,sizeof(TCITEM));
	addtic.mask=TCIF_TEXT|TCIF_IMAGE;

	HWND hResult=0;

	hResult=CreateDialogW(GetModuleHandle(0),MAKEINTRESOURCE(DlgID),hTab,proc);
	MoveWindow(hResult,ptPos.x+1,ptPos.y+22,rectTab.right-rectTab.left-4,rectTab.bottom-rectTab.top-25,0);
	addtic.pszText=Title.GetBuffer();
	addtic.iImage=0;
	SendMessage(hTab,TCM_INSERTITEM,Index,LPARAM(&addtic));
	return hResult;
}

struct stFoneType
{
	CStringA szFontName;
	int FonstSize;
	int FontWidth;
	HFONT hfont;
};
std::vector<stFoneType> vFonts;
void CzjlWnd::SetFontA(CStringA szFontName,int FontSize,int FontWidth)
{
	stFoneType tf;
	tf.szFontName=szFontName;
	tf.FonstSize=FontSize;
	tf.FontWidth=FontWidth;
	tf.hfont=0;
	for (unsigned int i=0;i<vFonts.size();i++)
	{
		if (vFonts[i].szFontName==tf.szFontName && vFonts[i].FonstSize==FontSize && vFonts[i].FontWidth==FontWidth )
		{
			tf=vFonts[i];
			break;
		}
	}
	if (tf.hfont==0)
	{
		tf.hfont=CreateFontA(FontSize,0,0,0,FontWidth,0,0,0,0,0,0,0,0,szFontName);
	}
	vFonts.push_back(tf);
	SendMessageA(m_hWnd,WM_SETFONT,(WPARAM)tf.hfont,0);
}


void CzjlWnd::SetFocus()
{
	SendMessageA(m_hWnd,WM_SETFOCUS,0,0);
	//SetFocus(m_hWnd);
}
