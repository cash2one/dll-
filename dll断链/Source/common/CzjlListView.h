#pragma once
#include "windows.h"
#include <atlstr.h>




class CzjlListView
{
public:
	CzjlListView(HWND thisView);
	~CzjlListView(void);

	void InsterTitle(int Index,int width,CString strTitle);
	void AddEleement(CStringW strAdd,bool IsNew=1,int index1=-1,int index2=0);//文字,列
	void AddEleement(CStringA strAdd,bool IsNew=1,int index1=-1,int index2=0);//文字,列
	void AddEleement(char* pchar,bool IsNew=1,int index1=-1,int index2=0);//文字,列
	void AddEleementNum(unsigned int Num,bool IsNew=1,int index1=-1,int index2=0);//文字,列

	void DeleteElement(int DeleteInex);

	void Clear();
	unsigned int SelectAll();
	unsigned int CheckAll(bool bSelect=1);

	int GetItemCount();
	int SetCheck(int nItem, BOOL fCheck=1);
	bool GetCheck(int nItem) ;
	
	int GetFirstSelect();
	void SetFirstSelect(int index, BOOL fCheck=1);

	void SetLVStyle(int Style);
	void SetStyleEx(int Style);
	HWND m_hListView;

	void ShowWindow(int bShow);
};

