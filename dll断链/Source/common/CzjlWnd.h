#pragma once
#include "atlstr.h"


class CzjlWnd
{
public:
	CzjlWnd(HWND h){m_hWnd=h;};
	CzjlWnd(){};
	~CzjlWnd(){};

	CStringA GetText();
	CStringW GetDlgText(int ItemID);

	void SetTextA(CStringA sz);
	void SetTextW(CStringW sz);

	void SetFontA(CStringA szFontName,int FontSize=14,int FontWidth=400);
	void SetFocus();

	void Enable(){EnableWindow(m_hWnd,1);};
	void Disable(){EnableWindow(m_hWnd,0);};

	//下拉类表
	void CB_Insert(CStringA szData);
	int CB_GetCount();
	void CB_SetCursel(int Index);
	int CB_GetCursel();

	//进度条
	void SetPos(int Current,int Max);//Progress

	//选择按钮
	bool BM_GetCheck();
	void BM_SetCheck(bool bCheck=1);

	//Tab
static	HWND CreateDlgInTab( HWND hTab,int DlgID,CString Title,int Index,DLGPROC proc);
private:
	HWND m_hWnd;
};