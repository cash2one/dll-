//属性表类
//蟠龙山制造QQ234073438
#pragma once
#include "windows.h"
#include <iterator>
#include "commctrl.h"
#pragma comment (lib,"comctl32.lib")
#include "vector"
#include <atlstr.h>

using namespace std;


//定义响应消息函数
typedef void (*PFUNC_MESSAGE)(HWND hwnd,UINT message,UINT wParam,UINT lParam,vector<CString> vszTreePath,int DialogID);

//拦截对话框消息
struct stMessage_Pages
{
	HWND msgFormH;
	UINT message;
	PFUNC_MESSAGE func;
};
//一个页上的控件感兴趣的消息记录,用于截取控件消息
struct stMessage_Items
{
	UINT  itemid;	//控件ID
	UINT Code;	//消息类型
	void* pFunc;   //处理函数指针
};
//WM_INITDIALOG 与WM_SHOWWINDOW这类消息需要HWND做标识,创建时得到HWND后绑定WM_SHOWWINDOW消息
struct stMessage_Risgers{
	UINT Dlgid;
	UINT message; //WM_INITDIALOG or WM_SHOWWINDOW
	PFUNC_MESSAGE pfunce;
};
//绑定所有消息
struct stMessage_Dialgs{
	UINT Dlgid;
	UINT message;
	PFUNC_MESSAGE pfunce;
};

//管理一页
struct zjlPAGE
{
	//TreeView
	HTREEITEM hTvItem;
	//选项全路径
	CString strPagePath;
	//对话框Hwnd
	HWND hDlg;
	//对话框id
	UINT Dlgid;
	//Enable/Visable
	bool bEnable;
	bool bVisable;
};


class CzjlProperty
{
public:
	//在hParent内构建一个属性表
	//二参数:属性表大小,默认大小为占满父窗口全部区域
	//三参数:左侧树状属性表目录占属性表的比例,默认是四分之一
	CzjlProperty(HWND hParent,RECT* rect=0,float scaleTV=0.25f);
	//析构时属性表自动销毁,不会影响父窗口
	~CzjlProperty(void);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*参数
TCHAR* _strpath;		目录路径,"\"代表节点,如遇字符中带"\"的用双斜杠代替,例如 _T("计算机\软件类\C\\C++")可以添加一个三层目录
UINT Dlgid;			对话框模板ID
HTREEITEM hitem;	每一个目录的选项节点有一个HTREEITEM句柄
int Dlgid;				页对话框id
int Dlgitemid;			控件id
*/
	//页控制函数
	HWND AddPage(TCHAR* _strPath,UINT Dlgid=0,bool bCreateNow=0,bool bEnable=1,bool bVisable=1);//bCreateNow立即创建还是选择时创建,bEnable是否禁用该页
	void DeletePage(TCHAR* _str);							//删除页
	void DeletePageByFather(TCHAR* _strFather);							//删除父节点下的所有页,不删除父节点
	void DeletePage(HTREEITEM deleteitem);				//删除页
	void ClearProperty();						//删除所有内容,销毁所有对话框页
	bool ActivePage(TCHAR* _str);				//激活页,通过路径
	bool ActivePage(HTREEITEM hti);			//激活页,通过目录句柄
	HTREEITEM ChangPagePath(TCHAR* _strDest,TCHAR* _strSrc);		//变更目录的位置,原配的对话框页随之改变,_strDest新路径,_strSrc老路径
	void VisablePage(bool Visable=1,TCHAR* _str=0,HTREEITEM itm=0,HWND dlg=0);	//显示/隐藏页.不指定参数为选择当前页
	void EnablePage(bool Enable=1,TCHAR* _str=0,HTREEITEM itm=0,HWND dlg=0);	//使用/禁用页,不指定参数为选择当前页.Enable=1,Enable=0关闭对话框

	//左侧TreeView控制栏
	HTREEITEM CreatePath(TCHAR* _str);							//创建新的目录路径,不创建对话框
	void	DeleteTVItem(HTREEITEM hitem);						//删除HTREEITEM指定的目录
	HTREEITEM AddTreeViewChildItem(TCHAR* _str,HTREEITEM htim,bool IsROOT=0);	//添加目录而不添加对话框, IsROOT=1时创建根目录;

	void Expand(TCHAR* _strpath,HTREEITEM hitm=0);	//根据路径打开目录
	void Close(TCHAR* _strpath,HTREEITEM hitm=0);	//根据路径关闭目录
	void ExpandAll(void);									//打开所有目录
	void CloseAll(void);										//关闭所有目录

	//消息控制
	//绑定消息:所有消息处理函数原形都是 void 函数名( HWND hwnd,UINT message,UINT wParam,UINT lParam );
	//响应页上控件消息,拦截注册的WM_COMMAND与WM_NOTIFY
	void Message_PageCOMMAND_NOTIFY(UINT Dlgitemid,DWORD commandCode,PFUNC_MESSAGE pOnMessage);
	//页创建时初始化控件状态.处理函数运行且只运行一次,(截取WM_INITDIALOG)
	void Message_PageInit(UINT dlgid,PFUNC_MESSAGE pfunc);//wParam是Dialog的ID
	//每次切换到该页时更新控件内的数据,每次页显示前调用处理函数,(截取WM_SHOWWINDOW)
	void Message_PageShowing(UINT dlgid,PFUNC_MESSAGE pfunc);
	//绑定一个页的所有消息
	void Message_Page(UINT DlgID,UINT message,PFUNC_MESSAGE pfunc);

	//GET
	//返回目录中当前选中的项
	HWND GetCurrentPage();					//返回当前页
	CString GetCurrentPath(bool isFullPath=1);			//FullPath=1为完整目录路径,FullPath=0为选项名
	HTREEITEM GetCurrentHTREEITEM();					//当前选择的目录句柄
	//返回左侧TreeView中的选项路径
	CString GetTreeViewItemName(HTREEITEM hItem) const;					//根据TreeView子项句柄返回该项名
	CString GetTreeViewItemPath(HTREEITEM hItem) const;					//根据TreeView子项句柄返回该项目录路径
	HTREEITEM GetTVItemCtrl(TCHAR* _strpath);									//指定路径的HTREEITEM句柄,如果没有便创建
	CString GetPageName(HWND hdlg);											//对话框对应的TreeView子项名
	CString GetPagePath(HWND hdlg);											//对话框对应的TreeView子项完整目录路径
	unsigned int GetDlgID(HWND hWnd);										//返回对话框ID
	//属性表属性
	HWND GetParent()const;							//整个属性表父窗口句柄
	HWND GetTreeViewCtrl()const;					//左侧TreeView句柄
	HWND GetPageCtrl(TCHAR* _str)const;			//查找已创建的页对话框句柄
	RECT GetPropertyRect(void)const;				//整个属性页的RECT
	RECT GetPageRect(void)const;					//页的CRET
	//返回容器句柄
	HWND GetBKHWND(){return m_hBase;};

	//查找
	//所有页中寻找控件,在Message_PageInit消息绑定的对话框初始化用户函数中无法使用用此函数,可用
	HWND FindDlgItem(int Dlgitemid)const;
	UINT GetPageCount(int OnlyCreate=0);	//OnlyCreate=1返回已创建过的页,0返回所有页的数量
	const zjlPAGE* GetPages(UINT index)const;		//遍历所有页,出界返回0
	HTREEITEM GetHTREEITEMs(UINT index)const;		//遍历TreeView目录子项,出界返回0

	const zjlPAGE* GetPage(HWND hdlg)const;
	const zjlPAGE* GetPage(TCHAR* _str)const;
	const zjlPAGE* GetPage(HTREEITEM htm)const;
	//设置字体
	void SetTreeViewFont(TCHAR* _fontname,int fontHeight=20);

//////////////////公有变量MESSAGEMAP,供窗口函数调用,勿自己改动或调用
	vector<stMessage_Items> m_ItemMessage;		//WM_COMMAND与WM_NOTIFY控件消息
	vector<stMessage_Pages> m_PagesMessage;	//除了控件外的PAGE拦截消息
	vector<stMessage_Risgers> m_LaterRegister;	//有些消息需要窗口句柄做标识,创建窗口以后才能绑定
	vector<stMessage_Dialgs> m_DialogMessage;//自己的对话框处理器


	//删除了一个节点后系统自动删除其所有的子节点,由窗口函数调用
	void _TVN_DELETEITEM_ClearChildPage(HTREEITEM thischild);
	//TV_FormatPath
	bool FormatTVPath(vector<CString>& result,TCHAR* _str);
private:
	HWND m_hParent;			//父窗口
	HWND m_hBase;				//背景静态控件(容器)
	HWND m_hTreeView;		//TreeView
	vector<zjlPAGE> m_Pages;	//所有页
	vector<HTREEITEM> m_TreeItems;	//所有创建过的目录

	HWND m_hCurrentPage;		//当前活动窗口
	//Size
	RECT m_RECTBase;				//属性表大小
	RECT m_RECTLV;					//左侧树状控件大小
	RECT m_RECTPage;				//右侧页大小
	//内部函数
	HWND _CreatePage(UINT Dlgid);
	void _SetRECT(RECT* rect,float scaleTV=0.3);

	HTREEITEM _FindTreeChild( TCHAR* _str ,HTREEITEM Parent) const;
	HTREEITEM _InsertTVItem(bool isROOT,HTREEITEM ParentTree,TCHAR* _str);
	HTREEITEM _CreateThePath(vector<CString>& result);
	void _Expand(TCHAR* tchar,int misk);
	void _PathBackToInsertStr(CString& ItemName);//把名字还原成添加它时的选项(单/变成双//);
	//清除该对话框的消息
	void _ClearPageMessage(UINT dlgid);
	//取得窗口句柄后在_CreatePage中注册消息
	void _AttachHwndFormDlgid(HWND hdlg,UINT WM_Message,PFUNC_MESSAGE func);
};