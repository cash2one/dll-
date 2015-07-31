//���Ա���
//���ɽ����QQ234073438
#pragma once
#include "windows.h"
#include <iterator>
#include "commctrl.h"
#pragma comment (lib,"comctl32.lib")
#include "vector"
#include <atlstr.h>

using namespace std;


//������Ӧ��Ϣ����
typedef void (*PFUNC_MESSAGE)(HWND hwnd,UINT message,UINT wParam,UINT lParam,vector<CString> vszTreePath,int DialogID);

//���ضԻ�����Ϣ
struct stMessage_Pages
{
	HWND msgFormH;
	UINT message;
	PFUNC_MESSAGE func;
};
//һ��ҳ�ϵĿؼ�����Ȥ����Ϣ��¼,���ڽ�ȡ�ؼ���Ϣ
struct stMessage_Items
{
	UINT  itemid;	//�ؼ�ID
	UINT Code;	//��Ϣ����
	void* pFunc;   //������ָ��
};
//WM_INITDIALOG ��WM_SHOWWINDOW������Ϣ��ҪHWND����ʶ,����ʱ�õ�HWND���WM_SHOWWINDOW��Ϣ
struct stMessage_Risgers{
	UINT Dlgid;
	UINT message; //WM_INITDIALOG or WM_SHOWWINDOW
	PFUNC_MESSAGE pfunce;
};
//��������Ϣ
struct stMessage_Dialgs{
	UINT Dlgid;
	UINT message;
	PFUNC_MESSAGE pfunce;
};

//����һҳ
struct zjlPAGE
{
	//TreeView
	HTREEITEM hTvItem;
	//ѡ��ȫ·��
	CString strPagePath;
	//�Ի���Hwnd
	HWND hDlg;
	//�Ի���id
	UINT Dlgid;
	//Enable/Visable
	bool bEnable;
	bool bVisable;
};


class CzjlProperty
{
public:
	//��hParent�ڹ���һ�����Ա�
	//������:���Ա��С,Ĭ�ϴ�СΪռ��������ȫ������
	//������:�����״���Ա�Ŀ¼ռ���Ա�ı���,Ĭ�����ķ�֮һ
	CzjlProperty(HWND hParent,RECT* rect=0,float scaleTV=0.25f);
	//����ʱ���Ա��Զ�����,����Ӱ�츸����
	~CzjlProperty(void);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*����
TCHAR* _strpath;		Ŀ¼·��,"\"����ڵ�,�����ַ��д�"\"����˫б�ܴ���,���� _T("�����\�����\C\\C++")�������һ������Ŀ¼
UINT Dlgid;			�Ի���ģ��ID
HTREEITEM hitem;	ÿһ��Ŀ¼��ѡ��ڵ���һ��HTREEITEM���
int Dlgid;				ҳ�Ի���id
int Dlgitemid;			�ؼ�id
*/
	//ҳ���ƺ���
	HWND AddPage(TCHAR* _strPath,UINT Dlgid=0,bool bCreateNow=0,bool bEnable=1,bool bVisable=1);//bCreateNow������������ѡ��ʱ����,bEnable�Ƿ���ø�ҳ
	void DeletePage(TCHAR* _str);							//ɾ��ҳ
	void DeletePageByFather(TCHAR* _strFather);							//ɾ�����ڵ��µ�����ҳ,��ɾ�����ڵ�
	void DeletePage(HTREEITEM deleteitem);				//ɾ��ҳ
	void ClearProperty();						//ɾ����������,�������жԻ���ҳ
	bool ActivePage(TCHAR* _str);				//����ҳ,ͨ��·��
	bool ActivePage(HTREEITEM hti);			//����ҳ,ͨ��Ŀ¼���
	HTREEITEM ChangPagePath(TCHAR* _strDest,TCHAR* _strSrc);		//���Ŀ¼��λ��,ԭ��ĶԻ���ҳ��֮�ı�,_strDest��·��,_strSrc��·��
	void VisablePage(bool Visable=1,TCHAR* _str=0,HTREEITEM itm=0,HWND dlg=0);	//��ʾ/����ҳ.��ָ������Ϊѡ��ǰҳ
	void EnablePage(bool Enable=1,TCHAR* _str=0,HTREEITEM itm=0,HWND dlg=0);	//ʹ��/����ҳ,��ָ������Ϊѡ��ǰҳ.Enable=1,Enable=0�رնԻ���

	//���TreeView������
	HTREEITEM CreatePath(TCHAR* _str);							//�����µ�Ŀ¼·��,�������Ի���
	void	DeleteTVItem(HTREEITEM hitem);						//ɾ��HTREEITEMָ����Ŀ¼
	HTREEITEM AddTreeViewChildItem(TCHAR* _str,HTREEITEM htim,bool IsROOT=0);	//���Ŀ¼������ӶԻ���, IsROOT=1ʱ������Ŀ¼;

	void Expand(TCHAR* _strpath,HTREEITEM hitm=0);	//����·����Ŀ¼
	void Close(TCHAR* _strpath,HTREEITEM hitm=0);	//����·���ر�Ŀ¼
	void ExpandAll(void);									//������Ŀ¼
	void CloseAll(void);										//�ر�����Ŀ¼

	//��Ϣ����
	//����Ϣ:������Ϣ������ԭ�ζ��� void ������( HWND hwnd,UINT message,UINT wParam,UINT lParam );
	//��Ӧҳ�Ͽؼ���Ϣ,����ע���WM_COMMAND��WM_NOTIFY
	void Message_PageCOMMAND_NOTIFY(UINT Dlgitemid,DWORD commandCode,PFUNC_MESSAGE pOnMessage);
	//ҳ����ʱ��ʼ���ؼ�״̬.������������ֻ����һ��,(��ȡWM_INITDIALOG)
	void Message_PageInit(UINT dlgid,PFUNC_MESSAGE pfunc);//wParam��Dialog��ID
	//ÿ���л�����ҳʱ���¿ؼ��ڵ�����,ÿ��ҳ��ʾǰ���ô�����,(��ȡWM_SHOWWINDOW)
	void Message_PageShowing(UINT dlgid,PFUNC_MESSAGE pfunc);
	//��һ��ҳ��������Ϣ
	void Message_Page(UINT DlgID,UINT message,PFUNC_MESSAGE pfunc);

	//GET
	//����Ŀ¼�е�ǰѡ�е���
	HWND GetCurrentPage();					//���ص�ǰҳ
	CString GetCurrentPath(bool isFullPath=1);			//FullPath=1Ϊ����Ŀ¼·��,FullPath=0Ϊѡ����
	HTREEITEM GetCurrentHTREEITEM();					//��ǰѡ���Ŀ¼���
	//�������TreeView�е�ѡ��·��
	CString GetTreeViewItemName(HTREEITEM hItem) const;					//����TreeView���������ظ�����
	CString GetTreeViewItemPath(HTREEITEM hItem) const;					//����TreeView���������ظ���Ŀ¼·��
	HTREEITEM GetTVItemCtrl(TCHAR* _strpath);									//ָ��·����HTREEITEM���,���û�б㴴��
	CString GetPageName(HWND hdlg);											//�Ի����Ӧ��TreeView������
	CString GetPagePath(HWND hdlg);											//�Ի����Ӧ��TreeView��������Ŀ¼·��
	unsigned int GetDlgID(HWND hWnd);										//���ضԻ���ID
	//���Ա�����
	HWND GetParent()const;							//�������Ա����ھ��
	HWND GetTreeViewCtrl()const;					//���TreeView���
	HWND GetPageCtrl(TCHAR* _str)const;			//�����Ѵ�����ҳ�Ի�����
	RECT GetPropertyRect(void)const;				//��������ҳ��RECT
	RECT GetPageRect(void)const;					//ҳ��CRET
	//�����������
	HWND GetBKHWND(){return m_hBase;};

	//����
	//����ҳ��Ѱ�ҿؼ�,��Message_PageInit��Ϣ�󶨵ĶԻ����ʼ���û��������޷�ʹ���ô˺���,����
	HWND FindDlgItem(int Dlgitemid)const;
	UINT GetPageCount(int OnlyCreate=0);	//OnlyCreate=1�����Ѵ�������ҳ,0��������ҳ������
	const zjlPAGE* GetPages(UINT index)const;		//��������ҳ,���緵��0
	HTREEITEM GetHTREEITEMs(UINT index)const;		//����TreeViewĿ¼����,���緵��0

	const zjlPAGE* GetPage(HWND hdlg)const;
	const zjlPAGE* GetPage(TCHAR* _str)const;
	const zjlPAGE* GetPage(HTREEITEM htm)const;
	//��������
	void SetTreeViewFont(TCHAR* _fontname,int fontHeight=20);

//////////////////���б���MESSAGEMAP,�����ں�������,���Լ��Ķ������
	vector<stMessage_Items> m_ItemMessage;		//WM_COMMAND��WM_NOTIFY�ؼ���Ϣ
	vector<stMessage_Pages> m_PagesMessage;	//���˿ؼ����PAGE������Ϣ
	vector<stMessage_Risgers> m_LaterRegister;	//��Щ��Ϣ��Ҫ���ھ������ʶ,���������Ժ���ܰ�
	vector<stMessage_Dialgs> m_DialogMessage;//�Լ��ĶԻ�������


	//ɾ����һ���ڵ��ϵͳ�Զ�ɾ�������е��ӽڵ�,�ɴ��ں�������
	void _TVN_DELETEITEM_ClearChildPage(HTREEITEM thischild);
	//TV_FormatPath
	bool FormatTVPath(vector<CString>& result,TCHAR* _str);
private:
	HWND m_hParent;			//������
	HWND m_hBase;				//������̬�ؼ�(����)
	HWND m_hTreeView;		//TreeView
	vector<zjlPAGE> m_Pages;	//����ҳ
	vector<HTREEITEM> m_TreeItems;	//���д�������Ŀ¼

	HWND m_hCurrentPage;		//��ǰ�����
	//Size
	RECT m_RECTBase;				//���Ա��С
	RECT m_RECTLV;					//�����״�ؼ���С
	RECT m_RECTPage;				//�Ҳ�ҳ��С
	//�ڲ�����
	HWND _CreatePage(UINT Dlgid);
	void _SetRECT(RECT* rect,float scaleTV=0.3);

	HTREEITEM _FindTreeChild( TCHAR* _str ,HTREEITEM Parent) const;
	HTREEITEM _InsertTVItem(bool isROOT,HTREEITEM ParentTree,TCHAR* _str);
	HTREEITEM _CreateThePath(vector<CString>& result);
	void _Expand(TCHAR* tchar,int misk);
	void _PathBackToInsertStr(CString& ItemName);//�����ֻ�ԭ�������ʱ��ѡ��(��/���˫//);
	//����öԻ������Ϣ
	void _ClearPageMessage(UINT dlgid);
	//ȡ�ô��ھ������_CreatePage��ע����Ϣ
	void _AttachHwndFormDlgid(HWND hdlg,UINT WM_Message,PFUNC_MESSAGE func);
};