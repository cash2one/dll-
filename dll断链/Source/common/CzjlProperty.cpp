
#include "CzjlProperty.h"


//Gobal
struct zjlProperty_State
{
	HWND hBase;
	WNDPROC OldBaseProc;
	CzjlProperty* pProperty;
};
//唯一全局变量,处理多个属性页实例并存的情况
vector<zjlProperty_State> g_zjlProperty_States;

 //容器Static控件,所有页与TreeView父窗口,处理TV消息.若要处理更多TreeView消息修改这里
 LRESULT CALLBACK  g_BaseFun( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
 { 
	  CzjlProperty* pthisProperty=0;
	  
	 //全局寻找,可能存在多个属性页实例
	  UINT indexGobal=0;
	 for (UINT indexGobal=0;indexGobal<g_zjlProperty_States.size();indexGobal++)
	 {
		 if (g_zjlProperty_States[indexGobal].hBase == hWnd)
		 {
			 pthisProperty=g_zjlProperty_States[indexGobal].pProperty;
			 break;
		 }
	 }
	 if ( !pthisProperty )
		 return DefWindowProc(hWnd,message,wParam,lParam);

	  switch(message)
	  {
	  case WM_NOTIFY:

						  if ( ( (NMHDR*)lParam)->code ==TVN_SELCHANGED ) //更改TV选项
						  {
							  pthisProperty->ActivePage( ( ( NMTREEVIEW *)lParam)->itemNew.hItem );
						  }else if ( ( (NMHDR*)lParam)->code ==TVN_DELETEITEM )	//有子项被删除
						  {
							  	HTREEITEM old=((NMTREEVIEW*)lParam)->itemOld.hItem;
							//有可能是删除完对话框后自动删除其余子节点
								pthisProperty->_TVN_DELETEITEM_ClearChildPage(old);
						  }
						  break;

	  default:
				return CallWindowProc(g_zjlProperty_States[indexGobal].OldBaseProc,hWnd,message,wParam,lParam);

	  }
	  return CallWindowProc(g_zjlProperty_States[indexGobal].OldBaseProc,hWnd,message,wParam,lParam);
 }
 //所有Page共用对话框函数
 INT_PTR CALLBACK g_PageFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
 {
	 HWND Parent=GetParent(hDlg);
	 CzjlProperty* pthisProperty=0;
	 //可能系统中存在多个属性页实例
	 for (UINT i=0;i<g_zjlProperty_States.size();i++)
	 {
		 if (g_zjlProperty_States[i].hBase ==Parent)
		 {
			 pthisProperty=g_zjlProperty_States[i].pProperty;
		 }
	 }
	 if ( !pthisProperty )
		return 0;
	 
	 CString szPath=pthisProperty->GetPagePath(hDlg);
	 vector<CString> vszTreePath;
	 pthisProperty->FormatTVPath(vszTreePath,szPath.GetBuffer());
	 int DialogID=pthisProperty->GetDlgID(hDlg);

	 //处理自定义绑定的消息
	 for (UINT i=0;i<pthisProperty->m_DialogMessage.size();i++)
	 {
		 if (DialogID==pthisProperty->m_DialogMessage[i].Dlgid)
		 {
			 (*((PFUNC_MESSAGE)(pthisProperty->m_DialogMessage[i].pfunce)))(hDlg,message,wParam,lParam,vszTreePath,DialogID);
		 }
	 }

	 switch (message)
	 {
	 case WM_COMMAND:
		 for (UINT j=0;j<pthisProperty->m_ItemMessage.size();j++) //MESSAGEMAP
		 {
			 if ( 
				 (	LOWORD(pthisProperty->m_ItemMessage[j].itemid) == LOWORD(wParam) )	&&
				 (	LOWORD(pthisProperty->m_ItemMessage[j].Code) == HIWORD(wParam) ) 
				 ){
					 (*((PFUNC_MESSAGE)(pthisProperty->m_ItemMessage[j].pFunc)))(hDlg,message,wParam,lParam,vszTreePath,DialogID);
					 return 1;
			 }
		 }
		 break;
	 case WM_NOTIFY:
		 for (UINT j=0;j<pthisProperty->m_ItemMessage.size();j++) //MESSAGEMAP
		 {
			 if ( 
				 (	LOWORD(pthisProperty->m_ItemMessage[j].itemid) == LOWORD(wParam) )	&&
				 (	pthisProperty->m_ItemMessage[j].Code == ( (NMHDR*)lParam)->code ) 
				 ){
					 (*((PFUNC_MESSAGE)(pthisProperty->m_ItemMessage[j].pFunc)))(hDlg,message,wParam,lParam,vszTreePath,DialogID);
					 return 1;
			 }
		 }
		 break;

	 case WM_INITDIALOG:
		 //创建后有了Hwnd,在这里注册m_LaterRegister中的消息
		 for (UINT i=0;i<pthisProperty->m_LaterRegister.size();i++)
		 {
				if (pthisProperty->m_LaterRegister[i].Dlgid == lParam &&  pthisProperty->m_LaterRegister[i].message==WM_INITDIALOG)
				{//是注册过Init消息的对话框创建
				 (*((PFUNC_MESSAGE)(pthisProperty->m_LaterRegister[i].pfunce)))(hDlg,message,wParam,lParam,vszTreePath,lParam);
				 return 1;
				}
		 }
		 break;
		
	 case WM_SHOWWINDOW:
		 for (UINT i=0;i<pthisProperty->m_LaterRegister.size();i++)
		 {
			 if ( pthisProperty->m_LaterRegister[i].Dlgid == pthisProperty->GetDlgID(hDlg) &&  pthisProperty->m_LaterRegister[i].message==WM_SHOWWINDOW && wParam==true)
			 {//是注册过Init消息的对话框创建
				 (*((PFUNC_MESSAGE)(pthisProperty->m_LaterRegister[i].pfunce)))(hDlg,message,wParam,lParam,vszTreePath,DialogID);
				 return 1;
			 }
		 }
		 break;
	 default:
		 //拦截普通消息
		 for (UINT i=0;i<pthisProperty->m_PagesMessage.size();i++)
		 {
			 if (pthisProperty->m_PagesMessage[i].message== message && pthisProperty->m_PagesMessage[i].msgFormH == hDlg )
			 {
				 ((PFUNC_MESSAGE)(pthisProperty->m_PagesMessage[i].func))(hDlg,message,wParam,lParam,vszTreePath,DialogID);
				 return 1;
			 }
		 }

	 }
	return 0;
 }

CzjlProperty::~CzjlProperty(void)
{
	ClearProperty();
	//清理本页在全局中的状态
	for(vector<zjlProperty_State>::iterator pos=g_zjlProperty_States.begin();
		pos !=	g_zjlProperty_States.end();)
	{
		if (	pos->hBase== m_hBase)
		{
				g_zjlProperty_States.erase(pos);
				return;
		}else{
			pos++;
		}
	}
}

CzjlProperty::CzjlProperty(HWND Parent,RECT* rect,float scaleTV)
{
	InitCommonControls();
	if (! IsWindow(Parent) )
		return;
	m_hParent=Parent;
	//还没有活动窗口
	m_hCurrentPage=0;
	_SetRECT(rect,scaleTV);

	m_hBase=CreateWindow(_T("Static"),
									0,	
									WS_VISIBLE | WS_CHILD |WS_BORDER,
									m_RECTBase.left,
									m_RECTBase.top,
									m_RECTBase.right,
									m_RECTBase.bottom,
									m_hParent,
									0,
									GetModuleHandle(0),
									0);
	if (!m_hBase)
		return;
	
	DWORD dwStyle= CCS_NORESIZE | WS_BORDER|WS_VISIBLE | WS_VSCROLL | WS_CHILD |WS_TABSTOP |TVS_HASLINES |TVS_LINESATROOT | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS;
	DWORD ExStyle=  0 ;

	m_hTreeView=CreateWindowEx(ExStyle ,_T("SysTreeView32"),
										0,	
										dwStyle,
											m_RECTLV.left,
											m_RECTLV.top,
											m_RECTLV.right,
											m_RECTLV.bottom,
										m_hBase,
										0,
										GetModuleHandle(0),
										0);

//AddThis
		zjlProperty_State addState;
		addState.hBase=m_hBase;
		//截取TV消息
		addState.OldBaseProc=(WNDPROC)SetWindowLong(m_hBase,GWL_WNDPROC,(LONG)g_BaseFun);
		addState.pProperty=this;
		g_zjlProperty_States.push_back(addState);
		SetTreeViewFont(_T("微软雅黑"));
}



HWND CzjlProperty::AddPage(TCHAR* _str,UINT Dlgid,bool bCreateNow,bool bEnable,bool bVisable)
{
	vector<CString> TVPath;
	zjlPAGE add;
	add.bEnable=bEnable;
	add.bVisable=bVisable;
	//格式化TreeView选项路径
	if (!FormatTVPath(TVPath,_str))
	{
		MessageBox(m_hParent,_str,_T("TreeView路径有误"),0);
		return 0;
	}

	//创建TreeView路径
	add.hTvItem=_CreateThePath(TVPath) ;
	if ( !add.hTvItem)
	{
		 MessageBox(m_hParent,_str,_T("添加TreeView子项失败"),0);
		return 0;
	}
	
	//只添加TreeView选项,该选项无对应对话框

	add.strPagePath=_str;
	add.Dlgid=Dlgid;//保存dlgid
	if ( bCreateNow)
	{//立即创建
			add.hDlg=_CreatePage(Dlgid);
			if (! add.hDlg)
			{
				MessageBox(m_hParent,_T("创建页失败"),0,0);
				return 0;
			}
	}else{//延迟创建
		add.hDlg=0;
	}
	m_Pages.push_back(add);
	InvalidateRect(m_hTreeView,0,1);
	return add.hDlg;
}

//激活窗口
bool CzjlProperty::ActivePage(HTREEITEM hti)
{
	if (m_Pages.empty())
		return 0;
	if (hti == 0)
	{
		EnableWindow(m_hCurrentPage,0);
		m_hCurrentPage=0;
		return 1;
	}

	//Find
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if ( m_Pages[i].hTvItem == hti)
		{

			if (m_hCurrentPage==m_Pages[i].hDlg)
			{//当前页,不动
				return 1;
			}
				if (m_hCurrentPage)//之前有页隐藏之
				{
			//	EnableWindow(m_hCurrentPage,0);
				ShowWindow(m_hCurrentPage,SW_HIDE);
				}
				if ( ! m_Pages[i].hDlg )//首次激活,Create之
				{
					m_Pages[i].hDlg=_CreatePage(m_Pages[i].Dlgid);
				}

				//处理Enable
				if (m_Pages[i].bEnable)
				{//没被禁用的页
					EnableWindow(m_Pages[i].hDlg,1);
				}else{//被禁用
					EnableWindow(m_Pages[i].hDlg,0);
				}
				//处理Visable
				if (m_Pages[i].bVisable)
				{//没隐藏的页
					ShowWindow(m_Pages[i].hDlg,SW_SHOW);			
				} 
				else{//隐藏的
					ShowWindow(m_Pages[i].hDlg,SW_HIDE);
				}
				m_hCurrentPage =m_Pages[i].hDlg;
				InvalidateRect(m_hBase,0,1);
				TreeView_SelectItem(m_hTreeView,m_Pages[i].hTvItem);
				return 1;
		}
	}


	//没有对话框的空项
	if (m_hCurrentPage)//之前有页隐藏之
	{
		EnableWindow(m_hCurrentPage,0);
		ShowWindow(m_hCurrentPage,SW_HIDE);
		m_hCurrentPage=0;
	}
	return 0;
}





//根据Base大小计算TV大小
void CzjlProperty::_SetRECT( RECT* rect,float scaleTV )
{
	if (scaleTV >1 || scaleTV <=0)
		scaleTV=0.3f;
	//Base
	if (!rect)
	{
		RECT ParentRect;
		GetClientRect(m_hParent,&ParentRect);
		m_RECTBase=ParentRect;
	}else{
		m_RECTBase=*rect;
	}
	m_RECTBase.left++;

	//TV
	m_RECTLV.top=m_RECTLV.left=5;	//xy
	m_RECTLV.right=long ((m_RECTBase.right*scaleTV)-5);	//w
	m_RECTLV.bottom=m_RECTBase.bottom-10;	//h
	//Page
	m_RECTPage.top=5;	//y
	m_RECTPage.left=long (m_RECTBase.right*scaleTV+5);	//x
	m_RECTPage.right=m_RECTBase.right-m_RECTPage.left-5;	//w
	m_RECTPage.bottom=m_RECTBase.bottom-10;	//h
}
//格式化TV路径
bool CzjlProperty::FormatTVPath(vector<CString>& result,TCHAR* _str)
{
	CString str= _str,temp;
	int pos=0;

	if (!str.GetLength() )
		return 0;

	do{
		if ( str[pos]== *_T("/")  )
		{
			//双斜杠
			if ( str[pos+1]== *_T("/") && pos<str.GetLength()-1 )
			{
				temp+=_T("/");
				pos+=2;//跳过"//"
				continue;
			}else{//单斜杠
							if (temp.IsEmpty())//第一个'/'跳过
							{
								pos++;
								continue;
							}else{	//Next
										result.push_back(temp);
										temp.Empty();
										pos++;
										continue;
								  }
				  }
		}else if(pos == str.GetLength()){//最后一个
													temp+=str[pos];
													result.push_back(temp);
													break;
													}else{	//copy
															temp+=str[pos];
															pos++;
												}	
	}while( pos<=str.GetLength() );

	return 1;
}

//返回HTREEITEM的text
CString CzjlProperty::GetTreeViewItemName( HTREEITEM hItem ) const
{
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_TEXT;
	CString str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		item.pszText = str.GetBufferSetLength(nLen);
		item.cchTextMax = nLen;
		::SendMessage(m_hTreeView, TVM_GETITEM, 0, (LPARAM)&item);
		nRes = lstrlen(item.pszText);
	} while (nRes >= nLen-1);
	str.ReleaseBuffer();
	return str;

}
//根据str查找同一目录下的子项
HTREEITEM CzjlProperty::_FindTreeChild( TCHAR* _str ,HTREEITEM Parent) const
{
		HTREEITEM BeginPos;
		CString cstr=_str;
		//根目录?
		if (!Parent)
		{
				BeginPos =(HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_ROOT, (LPARAM)Parent);
		}else{
				BeginPos =(HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)Parent);
		}

		while(BeginPos)
		{
			if ( cstr==GetTreeViewItemName(BeginPos) )
			{
				return BeginPos;
			}
			BeginPos=(HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)BeginPos);
		}
		return 0;
}



//在一个目录下插入新条目
HTREEITEM CzjlProperty::_InsertTVItem( bool isROOT,HTREEITEM ParentTree,TCHAR* _str )
{
	TVINSERTSTRUCT  tie;
	if (isROOT)
	{
		tie.hParent=0;
		tie.hInsertAfter=TVI_ROOT;
	} 
	else
	{
		tie.hParent=ParentTree;
		tie.hInsertAfter=TVI_LAST;
	}
	tie.item.mask=TCIF_TEXT;
	tie.item.pszText=_str;
	return (HTREEITEM)SendMessage(m_hTreeView,TVM_INSERTITEM,0,(LPARAM)&tie);
}

//创建完整路径
HTREEITEM CzjlProperty::_CreateThePath( vector<CString>& TVPath )
{
	HTREEITEM temp,creating;
	//Create Tree
	for (UINT i=0; i<TVPath.size();i++)
	{
		if ( i==0 )//ROOT?
		{

					temp = _FindTreeChild(TVPath[i].GetBuffer(),0);
					if (!temp)//Create New ROOT
					{
						creating=_InsertTVItem(1,0,TVPath[i].GetBuffer());
						if ( !creating )
							return 0;
						//保存供ExpandAll()使用
						m_TreeItems.push_back(creating);
					}else{
						creating=temp;
					}

		}else{

					temp = _FindTreeChild(TVPath[i].GetBuffer(),creating);
					if (!temp)//Create new Path
					{
						temp=_InsertTVItem(0,creating,TVPath[i].GetBuffer());
						if ( !temp )
							return 0;
						//展开
						m_TreeItems.push_back(creating);
						creating=temp;
					}else{
						creating=temp;
					}
		}
	}
	return creating;
}

void CzjlProperty::DeletePage( TCHAR* _str )
{
	if (!_str)
		return;
	//删除页
	for (vector<zjlPAGE>::iterator pos=m_Pages.begin();
		pos != m_Pages.end();
		pos++)
	{
		if ( pos->strPagePath == _str )
		{//找到页
					if ( pos->hDlg )
						{//激活过的页
							if (m_hCurrentPage ==pos->hDlg)//删除者为当前页
							m_hCurrentPage =0;
							EndDialog(pos->hDlg,0);
							_ClearPageMessage(pos->Dlgid);
						}
					
			//删除TreeView对应项
			HTREEITEM deleteItem=pos->hTvItem;
			pos=m_Pages.erase(pos);
			SendMessage(m_hTreeView,TVM_DELETEITEM,0,(LPARAM)deleteItem);
			return;
		}
	}
	//只有目录,没有对应属性页的情况,单独删除目录选项
	HTREEITEM htim=GetTVItemCtrl(_str);
		DeleteTVItem(htim);
	return;
}
//删除页
void CzjlProperty::DeletePage( HTREEITEM deleteitem )
{
	for (vector<zjlPAGE>::iterator pos=m_Pages.begin();
		pos!=m_Pages.end();pos++)
	{
		if (pos->hTvItem ==deleteitem)
		{//找到页
				if ( pos->hDlg )
				{//激活过的页
					if (m_hCurrentPage ==pos->hDlg)//删除者为当前页
						m_hCurrentPage =0;
					EndDialog(pos->hDlg,0);
					_ClearPageMessage(pos->Dlgid);
				}
				//删除TreeView对应项
				HTREEITEM h=pos->hTvItem;
				m_Pages.erase(pos);
				SendMessage(m_hTreeView,TVM_DELETEITEM,0,(LPARAM)h);
				return;
		}
	}
	//只有目录,没有对应属性页的情况,单独删除目录选项
	DeleteTVItem(deleteitem);
	return;
}
void CzjlProperty::ClearProperty()
{
	m_TreeItems.clear();
	m_PagesMessage.clear();
	m_ItemMessage.clear();
	m_LaterRegister.clear();
	//清除页
	vector<zjlPAGE>::iterator pos=m_Pages.begin();
		while(pos != m_Pages.end())
		{
			DestroyWindow(pos->hDlg);
			pos=m_Pages.erase(pos);
		}
	SendMessage(m_hTreeView, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	m_hCurrentPage=0;
	return;
}
//拦截所有页的WM_COMMAND与WM_NOTIFY消息
void CzjlProperty::Message_PageCOMMAND_NOTIFY( UINT ItemID,DWORD commandID,PFUNC_MESSAGE pOnMessage)
{
	//是否已添加
	for (vector<stMessage_Items>::iterator pos=m_ItemMessage.begin();
		pos != m_ItemMessage.end();
		pos++)
	{	
		if (	(pos->itemid == ItemID) && (pos->Code ==commandID))
		{
			//更新已添加的
			pos->pFunc=pOnMessage;
			return;
		}
	}
	stMessage_Items itm;
	itm.itemid=ItemID;
	itm.Code=commandID;
	itm.pFunc=pOnMessage;
	m_ItemMessage.push_back(itm);
	return;
}
//对话框页HWND
HWND CzjlProperty::GetPageCtrl( TCHAR* _str )const
{
	if (_str)
	{
		for (UINT i=0;i<m_Pages.size();i++)
		{
			if (	m_Pages[i].strPagePath == _str)
			{
				return m_Pages[i].hDlg;
			}
		}
	}
	return 0;
}

//页所有消息拦截
void CzjlProperty::_AttachHwndFormDlgid( HWND hdlg,UINT Message,PFUNC_MESSAGE func)
{

			if (!hdlg || !Message || !func)
				return;
//是否注册过
	for (UINT i=0;i<m_PagesMessage.size();i++)
	{
		if (m_PagesMessage[i].msgFormH==hdlg && m_PagesMessage[i].message ==Message )
		{
			m_PagesMessage[i].func=func;
			return;
		}
	}

	stMessage_Pages msg;
			msg.msgFormH=hdlg;
			msg.message=Message;
			msg.func=func;
			m_PagesMessage.push_back(msg);
}

//激活页
bool CzjlProperty::ActivePage( TCHAR* _strPath )
{
	if (m_Pages.size() ==0 || !_strPath)
		return 0;
	//展开
	Expand(_strPath);
	//Find
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if ( m_Pages[i].strPagePath == _strPath)
		{
			if (m_hCurrentPage==m_Pages[i].hDlg && m_hCurrentPage)
			{//当前页,不动
				return 1;
			}

			if (m_hCurrentPage)//之前有页隐藏之
			{
				EnableWindow(m_hCurrentPage,0);
				ShowWindow(m_hCurrentPage,SW_HIDE);
			}
			if ( ! m_Pages[i].hDlg )//首次激活,Create之
			{
				m_Pages[i].hDlg=_CreatePage(m_Pages[i].Dlgid);
			}
			//处理Enable
			if (m_Pages[i].bEnable)
			{//没被禁用的页
				EnableWindow(m_Pages[i].hDlg,1);
			}else{//被禁用
				EnableWindow(m_Pages[i].hDlg,0);
			}
			//处理Visable
			if (m_Pages[i].bVisable)
			{//没隐藏的页
				ShowWindow(m_Pages[i].hDlg,SW_SHOW);
			} 
			else{//隐藏的
				ShowWindow(m_Pages[i].hDlg,SW_HIDE);
			}
			m_hCurrentPage =m_Pages[i].hDlg;
			TreeView_SelectItem(m_hTreeView,m_Pages[i].hTvItem);
			return 1;
		}
	}
	return 0;
}
//设置TreeView字体
void CzjlProperty::SetTreeViewFont( TCHAR* _fontname,int fontHeight)
{
	HFONT font=CreateFont(fontHeight,0,0,0,0,0,0,0,DEFAULT_CHARSET,0,0,0,0,_fontname);
	int i=SendMessage(m_hTreeView,WM_SETFONT,(WPARAM)font,1);
	return;
}
//打开TreeView一选项
void CzjlProperty::Expand( TCHAR* _strpath,HTREEITEM hitm )
{
	if ( hitm)
	{
			SendMessage(m_hTreeView,TVM_EXPAND,TVE_EXPAND,(LPARAM)hitm);
	}else{
			_Expand(_strpath,TVE_EXPAND);
	}
 return;
}

//收缩TreeView一选项
void CzjlProperty::Close( TCHAR* _strpath,HTREEITEM hitm )
{
	if ( hitm)
	{
			SendMessage(m_hTreeView,TVM_EXPAND,TVE_COLLAPSE,(LPARAM)hitm);
	}else{
			_Expand(_strpath,TVE_COLLAPSE);
	}
 return;
}
//展开或关闭一项
void CzjlProperty::_Expand( TCHAR* _strpath,int mask )
{
	HTREEITEM hFindTVPos=0;
	vector<CString> TVPath;
	if(!FormatTVPath(TVPath,_strpath))
		return;

	for (UINT i=0; i<TVPath.size();i++)
	{
		if ( i==0 )//ROOT
		{
			hFindTVPos = _FindTreeChild(TVPath[i].GetBuffer(),0);
			if (!hFindTVPos)//没找到
				return;
			SendMessage(m_hTreeView,TVM_EXPAND,mask,(LPARAM)hFindTVPos);
		}else{	//子项
			hFindTVPos = _FindTreeChild(TVPath[i].GetBuffer(),hFindTVPos);
			if (!hFindTVPos)
				return;
			SendMessage(m_hTreeView,TVM_EXPAND,mask,(LPARAM)hFindTVPos);
		}
	}
}

//打开所有节点
void CzjlProperty::ExpandAll( void )
{
	for (UINT i=0;i<m_TreeItems.size();i++)
	{
		SendMessage(m_hTreeView,TVM_EXPAND,TVE_EXPAND,(LPARAM)m_TreeItems[i]);
	}
}
//关闭所有节点
void CzjlProperty::CloseAll( void )
{
	for (UINT i=0;i<m_TreeItems.size();i++)
	{
		SendMessage(m_hTreeView,TVM_EXPAND,TVE_COLLAPSE,(LPARAM)m_TreeItems[i]);
	}
}
HWND CzjlProperty::_CreatePage( UINT Dlgid )
{
	HWND hDlg=0;
	if (! Dlgid )
	return 0;
	//创建时得到句柄,通过WM_INITDIALOG的Lparam把对话框ID发送到g_PageFunc,在g_PageFunc完成注册
	hDlg=CreateDialogParam(GetModuleHandle(0),MAKEINTRESOURCE(Dlgid),m_hBase,&g_PageFunc,Dlgid);
	if (!hDlg)
	{
		MessageBox(m_hParent,_T("创建页失败"),0,0);
		return 0;
	}
	//有窗口句柄后注册该窗口其他所有需要延迟注册依赖句柄的消息
	for (vector<stMessage_Risgers>::iterator pos=m_LaterRegister.begin();
		pos!=m_LaterRegister.end();pos++)
	{
		if (pos->Dlgid == Dlgid )
		{
			//拦截一个新的窗口消息
			_AttachHwndFormDlgid(hDlg,pos->message,pos->pfunce);
				//同一个对话框模板可能创建多个窗口,共享用户
		/*	pos=m_LaterRegister.erase(pos);
		}else{*/
		}
	}
#ifdef _DEBUG
	//检查窗口风格
	long style = GetWindowLong(hDlg,GWL_STYLE);
	if  ( (style & WS_POPUP ) || 
		( style &  WS_CAPTION ) ||
		( !( style & WS_CHILD) ) )
	{
		CString str;
		str.Format(_T("确认ID为%d的属性页子对话框复合下列要求:\n \
				    1.对话框风格'Border'设置为'none'.\n \
				    2.对话框风格'Style'设置为'Child'\n \
				    3.不要为页对话框设置窗口函数."),Dlgid);
		MessageBox(m_hParent,str,_T("属性页对话框出错"),0);
	}
#endif // _DEBUG
	MoveWindow(hDlg,m_RECTPage.left,m_RECTPage.top,m_RECTPage.right,m_RECTPage.bottom,0);
	EnableWindow(hDlg,0);
	ShowWindow(hDlg,SW_HIDE);
	return hDlg;
}

RECT CzjlProperty::GetPropertyRect()const
{
	return m_RECTBase;
}

HWND CzjlProperty::GetTreeViewCtrl()const
{
	return m_hTreeView; 
}

UINT CzjlProperty::GetPageCount( int OnlyCreate/*=0*/ )
{
	UINT Result=0;

	if (!OnlyCreate)
	{//返回已添加的页数量
		 Result=m_Pages.size();
	}else{
		for (UINT i=0;i<m_Pages.size();i++)
		{
			if (m_Pages[i].hDlg)
			{//创建过的页
				Result++;
			}
		}
	}
	return Result;
}

CString CzjlProperty::GetPagePath( HWND hdlg )
{
	CString str=_T("");
	if (!IsWindow(hdlg))
		return str;

	for (UINT i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].hDlg==hdlg)
			return m_Pages[i].strPagePath;
	}
	return str;
}

CString CzjlProperty::GetPageName( HWND hdlg )
{
	vector<CString> vstr;
	CString str=_T("");
	if ( !IsWindow(hdlg))
		return str;

	for (UINT i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].hDlg==hdlg)
		{
			if ( FormatTVPath(vstr,m_Pages[i].strPagePath.GetBuffer() ) )
			{
				return vstr[vstr.size()-1];
			}
		}
	}
	return str;
}

CString CzjlProperty::GetCurrentPath(bool FullPath )
{
	CString str,temp;
	HTREEITEM hti=(HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_CARET, 0);
	bool pass=1;
	
	if (FullPath)
	{//完整路径
		//从端开始找到根
		while(hti)
		{
			if (!pass)//第一次是最后一项,不加"/"
			{	
				str.Insert(0,_T("/"));
			}else{
				pass=0;
			}
			temp=GetTreeViewItemName(hti).GetBuffer();//这一层的名字
			temp.Replace(_T("/"),_T("//"));	//还原,'/'->'//'
			str.Insert(0,temp);//添加时的//在名字中只有单/,还原
			hti=(HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hti);//Next
		}
	}else{//只返回该项
		return GetTreeViewItemName(hti);
	}

	return str;
}

HWND CzjlProperty::GetParent()const
{
	return m_hParent;
}


const zjlPAGE* CzjlProperty::GetPages( UINT index ) const
{
	if (index >= m_Pages.size())
	return 0;
	return &m_Pages[index];
}


HTREEITEM CzjlProperty::GetHTREEITEMs( UINT index )const
{
	if (index >= m_TreeItems.size())
		return 0;
	return m_TreeItems[index];
}

HTREEITEM CzjlProperty::GetTVItemCtrl( TCHAR* _strpath )
{
	vector<CString> vstr;
	FormatTVPath(vstr,_strpath);
	return _CreateThePath(vstr);
}

HTREEITEM CzjlProperty::ChangPagePath( TCHAR* _strDest,TCHAR* _strSrc )
{
	HTREEITEM itm;
	if ( !_strSrc ||  !_strDest)
	return 0;
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].strPagePath == _strSrc)
		{
			itm=CreatePath(_strDest);
			if (itm)
			{//新建成功
				SendMessage(m_hTreeView,TVM_DELETEITEM,0,(WPARAM)m_Pages[i].hTvItem); //删除老的
				//设置新页
				itm=CreatePath(_strDest);
				m_Pages[i].hTvItem=itm;
				m_Pages[i].strPagePath=_strDest;
				return itm;
			}else{
				return 0;
			}
		}
	}
	return 0;
}

HTREEITEM CzjlProperty::CreatePath( TCHAR* _str )
{
	vector<CString> vstr;
	FormatTVPath(vstr,_str);
	return _CreateThePath(vstr);
}

void CzjlProperty::EnablePage(bool Enable,TCHAR* _str,HTREEITEM itm,HWND dlg)
{
	int count=0;
	if (  !_str)
		count++;
	if (! itm)
		count++;
	if (dlg)
		count++;
	if ( count==0 )
	{
		itm=GetCurrentHTREEITEM();
	}else if(count >1){//参数太多
		return;
	}


	if (_str)//根据目录路径
	{
			for (UINT i=0;i<m_Pages.size();i++)
			{
				if (m_Pages[i].strPagePath == _str)
				{
					m_Pages[i].bEnable=Enable;
					if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
						ActivePage(_str);
				}
			}
	return;
	} 
	else if (itm)//根据目录句柄
	{
				for (UINT i=0;i<m_Pages.size();i++)
				{
					if (m_Pages[i].hTvItem == itm)
					{
						m_Pages[i].bEnable=Enable;
						if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
							ActivePage(_str);
					}
				}
	return;
	}else if (dlg)
	{
					for (UINT i=0;i<m_Pages.size();i++)
					{
						if (m_Pages[i].hDlg == dlg)
						{
							m_Pages[i].bEnable=Enable;
							if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
								ActivePage(_str);
						}
					}
	return;
	}
	return;
}

void CzjlProperty::Message_PageShowing( UINT _dlgid,PFUNC_MESSAGE _pfunc )
{
	for (UINT i=0;i<m_LaterRegister.size();i++)
	{
		if ((m_LaterRegister[i].Dlgid == _dlgid )&&  (m_LaterRegister[i].message == WM_SHOWWINDOW) )
		{
			m_LaterRegister[i].pfunce=_pfunc;
			return;
		}
	}

	stMessage_Risgers dlg;
	dlg.Dlgid=_dlgid;
	dlg.pfunce=_pfunc;
	dlg.message=WM_SHOWWINDOW;
	m_LaterRegister.push_back(dlg);
}

HTREEITEM CzjlProperty::GetCurrentHTREEITEM()
{
		return (HTREEITEM)SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_CARET, 0);
}

void CzjlProperty::VisablePage( bool Visable,TCHAR* _str/*=0*/,HTREEITEM itm/*=0*/,HWND dlg/*=0*/ )
{
	int count=0;
	if (  !_str)
		count++;
	if (! itm)
		count++;
	if (dlg)
		count++;
	if ( count==0 )
	{
		itm=GetCurrentHTREEITEM();
	}else if(count >1){//参数太多
		return;
	}

	if (_str)//根据目录路径
	{
		for (UINT i=0;i<m_Pages.size();i++)
		{
			if (m_Pages[i].strPagePath == _str)
			{
				m_Pages[i].bVisable=Visable;
				if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
					ActivePage(_str);
			}
		}
		return;
	} 
	else if (itm)//根据目录句柄
	{
		for (UINT i=0;i<m_Pages.size();i++)
		{
			if (m_Pages[i].hTvItem == itm)
			{
				m_Pages[i].bVisable=Visable;
				if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
					ActivePage(_str);
			}
		}
		return;
	}else if (dlg)
	{
		for (UINT i=0;i<m_Pages.size();i++)
		{
			if (m_Pages[i].hDlg == dlg)
			{
				m_Pages[i].bVisable=Visable;
				if (m_hCurrentPage == m_Pages[i].hDlg)//重新激活当前页
					ActivePage(_str);
			}
		}
		return;
	}
	return;
}

void CzjlProperty::Message_PageInit( UINT _dlgid,PFUNC_MESSAGE  _pfunc )
{	//WM_INITDIALOG消息只能通过Hwnd鉴别,先保存Dlgid
	for (UINT i=0;i<m_LaterRegister.size();i++)
	{
		if ( (m_LaterRegister[i].Dlgid == _dlgid )&& (m_LaterRegister[i].message == WM_INITDIALOG))
		{
			m_LaterRegister[i].pfunce=_pfunc;
			return;
		}
	}
	//_Create对话框时再创建
	stMessage_Risgers dlg;
	dlg.Dlgid=_dlgid;
	dlg.pfunce=_pfunc;
	dlg.message=WM_INITDIALOG;
	m_LaterRegister.push_back(dlg);
}

RECT CzjlProperty::GetPageRect( void )const
{
	return m_RECTPage;
}

HWND CzjlProperty::FindDlgItem( int itemid )const
{
	HWND result=0;
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].hDlg)
		{
			result =GetDlgItem(m_Pages[i].hDlg,itemid);
			if (result)
				return result;
		}
	}
	return result;
}

HTREEITEM CzjlProperty::AddTreeViewChildItem( TCHAR* _str,HTREEITEM htim,bool IsROOT)
{
		HTREEITEM result;
		result=_InsertTVItem(IsROOT,htim,_str);
		if (result)
		m_TreeItems.push_back(result);
		return result;
}

void CzjlProperty::DeleteTVItem( HTREEITEM hitem )
{
 SendMessage(m_hTreeView,TVM_DELETEITEM,0,(LPARAM)hitem);
}
//
void CzjlProperty::_TVN_DELETEITEM_ClearChildPage( HTREEITEM child )
{
	//有对话框的
	for (vector<zjlPAGE>::iterator pos=m_Pages.begin();
		pos!=m_Pages.end();pos++)
	{
		if ( pos->hTvItem == child ) 
		{
			if (pos->hDlg)
				EndDialog(pos->hDlg,0);

			m_Pages.erase(pos);
			break;
		}
	}

	for (vector<HTREEITEM>::iterator pos=m_TreeItems.begin();
		pos!=m_TreeItems.end();pos++)
	{
		if (*pos==child)
		{
			m_TreeItems.erase(pos);
			return;
		}
	}

}

void CzjlProperty::_ClearPageMessage( UINT dlgid )
{
	//Command/notify;其余2个消息不删除
	for (vector<stMessage_Items>::iterator pos= m_ItemMessage.begin();
		pos!=m_ItemMessage.end();)
	{
		if (pos->itemid == dlgid)
		{
			pos=m_ItemMessage.erase(pos);
		}else{
			pos++;
		}
	}
}

void CzjlProperty::Message_Page( UINT _dlgid,UINT _message,PFUNC_MESSAGE _pfunc )
{
	for (UINT i=0;i<m_DialogMessage.size();i++)
	{
		if ((m_DialogMessage[i].Dlgid == _dlgid )&&  (m_DialogMessage[i].message == _message) )
		{
			//修改原来的
			m_DialogMessage[i].pfunce=_pfunc;
			return;
		}
	}

	stMessage_Dialgs dlg;
	dlg.Dlgid=_dlgid;
	dlg.pfunce=_pfunc;
	dlg.message=_message;
	m_DialogMessage.push_back(dlg);
}

const zjlPAGE* CzjlProperty::GetPage( HWND hdlg ) const
{
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if ( m_Pages[i].hDlg == hdlg )
			return &m_Pages[i];
	}
	return 0;
}

const zjlPAGE* CzjlProperty::GetPage( TCHAR* _str ) const
{
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if ( m_Pages[i].strPagePath == _str )
			return &m_Pages[i];
	}
	return 0;
}

const zjlPAGE* CzjlProperty::GetPage( HTREEITEM htm ) const
{
	for (UINT i=0;i<m_Pages.size();i++)
	{
		if ( m_Pages[i].hTvItem == htm )
			return &m_Pages[i];
	}
	return 0;
}

CString CzjlProperty::GetTreeViewItemPath( HTREEITEM hItem ) const
{
	HTREEITEM positem=hItem;
	CString Result,temp;
	bool IsEnd=1;
	while(1)
	{
		temp=GetTreeViewItemName(positem);
		if (temp.IsEmpty())
			break;
		temp.Replace(_T("/"),_T("//"));
		if (IsEnd)
		{
			IsEnd=0;
		}else{
			temp+=_T("/");
		}
		Result.Insert(0,temp);

		positem=(HTREEITEM)::SendMessage(m_hTreeView, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)positem);
		if(! positem)
			break;
	}
		return Result;
}

void CzjlProperty::DeletePageByFather( TCHAR* _strFather )
{
	if (!_strFather)
		return;
	CStringW strFather=_strFather;
	strFather+=L"/";
	//删除页
	for (vector<zjlPAGE>::iterator pos=m_Pages.begin();
		pos != m_Pages.end();
		pos++)
	{
		if ( pos->strPagePath.Left(strFather.GetLength()) == strFather )
		{//找到页
			if ( pos->hDlg )
			{//激活过的页
				if (m_hCurrentPage ==pos->hDlg)//删除者为当前页
					m_hCurrentPage =0;
				EndDialog(pos->hDlg,0);
				_ClearPageMessage(pos->Dlgid);
			}

			//删除TreeView对应项
			HTREEITEM deleteItem=pos->hTvItem;
			pos=m_Pages.erase(pos);
			SendMessage(m_hTreeView,TVM_DELETEITEM,0,(LPARAM)deleteItem);

			if (pos==m_Pages.end())
			break;
		}
	}
	return;
}

HWND CzjlProperty::GetCurrentPage( )
{
	HTREEITEM htv=GetCurrentHTREEITEM();
	for (unsigned int i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].hTvItem==htv)
		{
			return m_Pages[i].hDlg;
		}
	}
	return 0;
}

unsigned int CzjlProperty::GetDlgID( HWND hWnd )
{
	for (unsigned int i=0;i<m_Pages.size();i++)
	{
		if (m_Pages[i].hDlg==hWnd)
		{
			return m_Pages[i].Dlgid;
		}
	}
	return 0;
}
