#include "CzjlListView.h"
#include <commctrl.h>
#include <atlstr.h>




CzjlListView::CzjlListView( HWND thisView )
{
m_hListView=thisView;
int style=SendMessage(m_hListView,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
style |=LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES;
SendMessage(m_hListView,LVM_SETEXTENDEDLISTVIEWSTYLE,0,style);
}


void CzjlListView::InsterTitle( int Index,int width,CString strTitle )
{

	LV_COLUMN lvm;
	RtlZeroMemory(&lvm,sizeof(LVCOLUMN));
	lvm.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
	lvm.fmt=LVCFMT_RIGHT;

	lvm.cx =width;
	lvm.pszText=strTitle.GetBuffer();
	SendMessage(m_hListView,LVM_INSERTCOLUMN,Index,(LPARAM)&lvm);
}



CzjlListView::~CzjlListView( void )
{

}

void CzjlListView::AddEleement( CStringW strAdd,bool IsNew,int Index1,int index2 )
{
	if (strAdd.IsEmpty())
	return;
	LV_ITEM item;
	int count=0;
	RtlZeroMemory(&item,sizeof(LV_ITEM));
	item.mask=LVIF_TEXT ;
	item.pszText=strAdd.GetBuffer();
	item.iSubItem=index2;

	if (IsNew)
	{//新加行
		if (Index1 == -1)
		{//添加至末尾
			count=SendMessage(m_hListView,LVM_GETITEMCOUNT,0,0);
			if (count >0)
				count++;
			item.iItem=count;
			SendMessage(m_hListView,LVM_INSERTITEM,0,(WPARAM)&item);
		}else{//添加至指定行
			item.iItem=--Index1;
			SendMessage(m_hListView,LVM_INSERTITEM,0,(WPARAM)&item);
		}
	}else{//修改行
		if (Index1 == -1)
		{//修改最后一行
			count=SendMessage(m_hListView,LVM_GETITEMCOUNT,0,0);
			if (count)
				count--;
			item.iItem=count;
			SendMessage(m_hListView,LVM_SETITEM,0,(WPARAM)&item);
		}else{//修改指定行
			item.iItem=Index1;
			CStringW szBuf;
			ListView_GetItemText(m_hListView,Index1,index2,szBuf.GetBuffer(1024),1024);
			szBuf.ReleaseBuffer();
			if (szBuf!=strAdd)
			{
				SendMessage(m_hListView,LVM_SETITEM,0,(WPARAM)&item);
			}
		}
	}

}


void CzjlListView::AddEleement( CStringA strAdd,bool IsNew/*=1*/,int index1/*=-1*/,int index2/*=0*/ )
{
	CString strtemp=strAdd;
	AddEleement(strtemp,IsNew,index1,index2);
}

void CzjlListView::AddEleement( char* pchar,bool IsNew/*=1*/,int index1/*=-1*/,int index2/*=0*/ )
{
	CStringA strtemp=pchar;
	AddEleement(strtemp,IsNew,index1,index2);
}

void CzjlListView::Clear()
{
bool i=	SendMessage(m_hListView,LVM_DELETEALLITEMS,0,0);
}

unsigned int CzjlListView::SelectAll()
{
	unsigned int Count=SendMessage(m_hListView,LVM_GETITEMCOUNT,0,0);
	if (	Count>0)
	{
		LVITEM lviSelect;
		lviSelect.stateMask = LVIS_SELECTED|LVIS_FOCUSED;
		lviSelect.state = LVIS_SELECTED|LVIS_FOCUSED;
		for (unsigned int i=0;i<Count;i++)
		{
			SendMessage(m_hListView,LVM_SETITEMSTATE,i,(WPARAM)&lviSelect);
		}
	}
	return Count;
}

void CzjlListView::SetLVStyle( int Style )
{
    int i=ListView_GetExtendedListViewStyle(m_hListView);
    i|=Style;
    ListView_SetExtendedListViewStyle(m_hListView,i);
}

unsigned int CzjlListView::CheckAll(bool bSelect)
{
	int count=GetItemCount();
	for (unsigned int i=0;i<count;i++)
	{
		SetCheck(i,bSelect);
	}
	return count;
}

void CzjlListView::SetStyleEx( int Style )
{
	DWORD OldStyle=GetWindowLong(m_hListView,GWL_EXSTYLE);
	OldStyle=Style | OldStyle;
	SetWindowLong(m_hListView,GWL_EXSTYLE,OldStyle);
}

int CzjlListView::GetItemCount()
{
	return (int) ::SendMessage(m_hListView, LVM_GETITEMCOUNT, 0, 0L);
}

int CzjlListView::SetCheck( int nItem, BOOL fCheck )
{
	LVITEM lvi;
	lvi.stateMask = LVIS_STATEIMAGEMASK;

	/*
	Since state images are one-based, 1 in this macro turns the check off, and
	2 turns it on.
	*/
	lvi.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));
	return (BOOL) ::SendMessage(m_hListView, LVM_SETITEMSTATE, nItem, (LPARAM)&lvi);
}

bool CzjlListView::GetCheck( int nItem )
{
	int nState = (int)::SendMessage(m_hListView, LVM_GETITEMSTATE, (WPARAM)nItem,
		(LPARAM)LVIS_STATEIMAGEMASK);
	// Return zero if it's not checked, or nonzero otherwise.
	return ((BOOL)(nState >> 12) -1);
}

void CzjlListView::AddEleementNum( unsigned int Num,bool IsNew/*=1*/,int index1/*=-1*/,int index2/*=0*/ )
{
	CStringA sz;
	sz.Format("%d",Num);
	AddEleement(sz,IsNew,index1,index2);
}

void CzjlListView::ShowWindow( int bShow )
{
	::ShowWindow(m_hListView,bShow);
}

int CzjlListView::GetFirstSelect()
{
	for (int i=0;i<GetItemCount();i++)
	{
			int nState = (int)::SendMessage(m_hListView, LVM_GETITEMSTATE, (WPARAM)i,(LPARAM)LVIS_SELECTED);
		if (nState)
		{
			return i;
		}
	}
	return -1;
}

void CzjlListView::SetFirstSelect( int index, BOOL fCheck )
{
	LVITEM item;
	item.stateMask = LVIS_SELECTED;
	item.state =LVIS_SELECTED;
	SendMessage(m_hListView, LVM_SETITEMSTATE, (WPARAM)index,(LPARAM)&item);
	SetFocus(m_hListView);
}

void CzjlListView::DeleteElement( int DeleteInex )
{

	SendMessage(m_hListView,LVM_DELETEITEM,WPARAM(DeleteInex),0);
}
