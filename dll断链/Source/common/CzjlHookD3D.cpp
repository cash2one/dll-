#include "CzjlHookD3D.h"
#include <tlhelp32.h>
#include <shlwapi.h>
#include <stdio.h>
#include <atlstr.h>
#include <iostream>
#include <d3d9.h>
#include <winbase.h>
#include <windows.h>
#include <windef.h>
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")


#include <Include\d3dx9core.h>






extern int bDebug;
using namespace std;
unsigned int FindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen);
BOOL __stdcall D3DDrawText(LPDIRECT3DDEVICE9 pDxdevice,LPTSTR strText ,LPTSTR FontName ,int FontH,int FontW,int FWeight,int Rleft,int Rtop);
unsigned int FindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen);
HWND D3D_GetMainWindow();
struct{//hook地址
	int pPresent;
	CD3DHookApi_Jmp Hook_Present;
	pMyPresentCallBack pUserPresent;

	int pReset;
	CD3DHookApi_Jmp Hook_Reset;
	pMyResetCallBack pUserResetCreate;
	pMyResetCallBack pUserReset_Clear;

	int pEndScene;
	CD3DHookApi_Jmp Hook_EndScene;
	pMyEndSceneCallBack pUserEndScene;

}g_D3DAddr;


struct CreateDXPoint{
		IDirect3D9 *pD3d9 ;
	IDirect3DDevice9 *pDxdevice;
	CreateDXPoint(){
		pDxdevice=0;
		pD3d9=0;
	}
}g_CreateDXPoint;


//////////////////////////////////////////////////////////////////////////

/*

void DrawSprite_2DAA( 
	int dx,//窗口中的X座标
	int dy,//窗口中的Y座标
	int sx,//tex纹理中的X座标
	int sy,//tex纹理中的Y座标
	int sw,//tex纹理中的子区域宽度
	int sh,//tex纹理中的子区域高度
	IDirect3DDevice9* device,
	ID3DXSprite* sprite,
	IDirect3DTexture9* tex,//已生成的纹理
	bool trans )
{
	D3DXCOLOR c = D3DCOLOR_XRGB(255,255,255);
	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );

	RECT r;
	r.left = sx;
	r.top = sy;
	r.right = sx+sw;
	r.bottom = sy+sh;

	device->SetTransform( D3DTS_WORLD,&identity );
	device->SetTransform( D3DTS_VIEW,&identity );

	if( trans )
	{
		sprite->Begin( D3DXSPRITE_ALPHABLEND );
	}
	else
	{
		sprite->Begin( 0 );
	}
	//把纹理中的子区域画在指定的屏幕坐标上
	sprite->Draw( tex,&r,0,&D3DXVECTOR3(dx,dy,0),c );
	sprite->End();
}

ID3DXSprite* g_pSpriteAA=0;
IDirect3DTexture9* g_pTextureAA=0;

void FreeImageAA()
{
	if (g_pSpriteAA)
		g_pSpriteAA->Release();
	if (g_pTextureAA)
		g_pTextureAA->Release();

	g_pSpriteAA=0;
	g_pTextureAA=0;
}
bool LoadAimImagAA(IDirect3DDevice9* pThis,CStringA szPath)
{
	if (g_pSpriteAA &&g_pTextureAA)
	{
		return 1;
	}

	if (pThis)
	{
		D3DXCreateSprite(pThis,&g_pSpriteAA);
		if (S_OK==D3DXCreateSprite(pThis,&g_pSpriteAA) &&g_pSpriteAA)
		{
			//D3DXCreateTextureFromResourceA(pThis,GetModuleHandleA("D2Tool.dll"),MAKEINTRESOURCEA(IDB_INFOBK),&g_pTexture);
			D3DXCreateTextureFromFileA(pThis,szPath.GetBuffer(),&g_pTextureAA);
			if (!g_pTextureAA)
			{
				// 载入贴图失败处理代码。
				g_pSpriteAA->Release();
				g_pSpriteAA=0;
				return 0;
			}
			return 1;
		}
	}
	return 0;
	
}*/

//////////////////////////////////////////////////////////////////////////
static  void DbgPrintToFile(const char* str)
{
	FILE *fp = fopen("C:\\DebugPrint.txt", "a");
	if (fp) {
		fprintf(fp, "%s\n", str);
		fclose(fp);
	}
}


void D3DDbgPrint(const char *fmt, ...)
{
	if (bDebug==0)//只有调试版输出
	{
		return;
	}
	char buf[1024*100]={0};
	va_list arg = (va_list)((char*)(&fmt) + 4); 
	vsprintf_s(buf, fmt, arg);
	CStringA sz,szbuf;
	szbuf=buf;


	int _begin=0,_end=0;
	for (unsigned int i=0;true;i++)
	{
		CStringA szOut;
		_end=szbuf.Find("\r\n",_begin);
		bool bEnd=0;
		if (_end==-1 )
		{
			if (i==0)//没有换行符
			{
				szOut.Format("=,= %s",szbuf);
			}else
			{//最后一行
				szOut.Format("=,= %s",szbuf.Mid(_begin,szbuf.GetLength()-_begin));
			}			   
			bEnd=1;
		}else{
			szOut.Format("=,= %s",szbuf.Mid(_begin,_end-_begin));
		}
		if (szOut!="")
		{
			OutputDebugStringA(szOut);
			DbgPrintToFile(szOut.GetBuffer(0));
			szOut.Release();
			printf("%s",szOut);
		}
		_begin=_end+2;
		if (bEnd)
		{
			break;
		}
	}   
}


struct{//Data
	D3DPRESENT_PARAMETERS PresentationParameters;
}g_D3DData;

CzjlHookD3D::CzjlHookD3D(void)
{
	memset(&g_D3DAddr,0,sizeof(g_D3DAddr));
	memset(&g_D3DData,0,sizeof(g_D3DData));
	g_D3DAddr.pUserEndScene=0;
	g_D3DAddr.pUserPresent=0;
	g_D3DAddr.pUserResetCreate=0;
	g_D3DAddr.pUserReset_Clear=0;
}


CzjlHookD3D::~CzjlHookD3D(void)
{
}
HRESULT __stdcall HookInlinePresent( IDirect3DDevice9* pThis,THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{	
	g_D3DAddr.Hook_Present.SetHookOff();
	//Do smt
	if (g_D3DAddr.pUserPresent)
	{
		g_D3DAddr.pUserPresent(pThis,pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
	}
	HRESULT hResut= pThis->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
	g_D3DAddr.Hook_Present.SetHookOn();
	return hResut;
}

HRESULT __stdcall HookInlineReset(
	IDirect3DDevice9 * pThis ,
	D3DPRESENT_PARAMETERS* pPresentationParameters
	)
{

	g_D3DAddr.Hook_Reset.SetHookOff();
	if (pPresentationParameters)
	{
		memcpy(&g_D3DData.PresentationParameters,pPresentationParameters,sizeof(D3DPRESENT_PARAMETERS));
	}
	
	if (g_D3DAddr.pUserReset_Clear)
	{
		g_D3DAddr.pUserReset_Clear(pThis,pPresentationParameters);
	}
	HRESULT hResut= pThis->Reset(pPresentationParameters);

	if (g_D3DAddr.pUserResetCreate)
	{
		g_D3DAddr.pUserResetCreate(pThis,pPresentationParameters);
	}
	

	g_D3DAddr.Hook_Reset.SetHookOn();  
	return hResut;
}

HRESULT __stdcall HookInlineEndScene(
	IDirect3DDevice9 * pThis 
	)
{
	g_D3DAddr.Hook_EndScene.SetHookOff();
	if (g_D3DAddr.pEndScene)
	{
		g_D3DAddr.pUserEndScene(pThis);
	}
	HRESULT hResut= pThis->EndScene();
	g_D3DAddr.Hook_EndScene.SetHookOn();
	return hResut;
}



bool CzjlHookD3D::Init( int Type)//1=特征码提取.2=通过创建设备对象提取VF指针,3=已获得Vmt指针
{
	bool bRseult=0;

		switch (Type)
		{
		case 1:
			{
			bRseult= 	GetAddrByCode();
			}
			break;
		case 2:
			{

			bRseult= 	GetAddrByCreateD3d(D3D_GetMainWindow());
			}
		break;	
		case 3:
			{
				bRseult=GetAddrByAppCreateD3D_Client();
			}
			break;
		}

		if (bRseult)
		{
			if (	g_D3DAddr.Hook_Present.HookAddr((char*)g_D3DAddr.pPresent,(FARPROC)HookInlinePresent)==0	)
			{
				printf("HookInlinePresent Error\r\n!");
				return 0;
			}else{

			}
			
			if (	g_D3DAddr.Hook_Reset.HookAddr((char*)g_D3DAddr.pReset,(FARPROC)HookInlineReset)==0)
			{
					printf("HookInlineReset Error\r\n!");
				return 0;
			}



			if (		g_D3DAddr.Hook_EndScene.HookAddr((char*)g_D3DAddr.pEndScene,(FARPROC)HookInlineEndScene)==0)
			{
				printf("HookInlineEndScene Error\r\n!");
				return 0;
			}




			return 1;
		}
		return 0;
}	



CStringA GetSystemVersionStr()
{
	int a=0,b=0,i=0,j=0;  
	_asm  
	{  
		pushad  
			mov ebx,fs:[0x18] ; get self pointer from TEB  
			mov eax,fs:[0x30] ; get pointer to PEB / database  
			mov ebx,[eax+0A8h] ; get OSMinorVersion  
			mov eax,[eax+0A4h] ; get OSMajorVersion  
			mov j,ebx  
			mov i,eax  
			popad  
	}  

	D3DDbgPrint("GetSystemVersionStr Version: %d  - %d",i,j);

	if((i==5)&&(j==0))  
	{  
		D3DDbgPrint("系统版本为 Windows 2000");
		return "xp";
	}  
	else if((i==5)&&(j==1))  
	{   
		D3DDbgPrint("系统版本为 Windows XP");
		return "xp";
	}  
	else if((i==5)&&(j==2))  
	{  
		return "xp";
	}  
	else if((i==6)&&(j==0))  
	{  
		D3DDbgPrint("系统版本为 Windows Vista"); 
		return "win7";
	}  
	else if((i==6)&&(j==1))  
	{  
		D3DDbgPrint("系统版本为 Windows Win7");
		return "win7";
	}
	else if((i==6)&&(j==2))  
	{  
		D3DDbgPrint("系统版本为 Windows 8");
		return "win8";
	}

	return "";
}

bool CzjlHookD3D::GetAddrByCode()
{
	D3DDbgPrint("GetAddrByCode\r\n");
		//MyPresent
	//char TargetPresent[]={0x56,0x57,0x8B,0x7D,0x08,0x85,0xFF,0x74,0x44,0x8D,0x77,0x04,0x83,0x7E,0x18,0x00};
	//win7 x64  (-5) 56 57 8B 7D 08 85 FF 74 44 8D 77 04 83 7E 18 00  
	//char TargetPresent[]={0x51,0x53,0x8B,0x5D,0x08,0x85,0xDB,0x56,0x74};
	//xp (-5) 51 53 8B 5D 08 85 DB 56 74

	//Reset  
	//WIn7 x64 (-5) 81 EC 48 01 00 00 53 56 8B 75 08 85 F6
	//xp   (-5) 83 EC 1C 53 8B 5D 08 85 DB 56


	//Endscne
	//Win7  0x70C3279F 
	//XP	  0x4B6C71b0 F6 46 2C 02 89 5D FC 75 0E 8B 86 A4 1F 00 00 A8 01 C6 45 FC 01 75 24 39 5F 18 74 07 57
	if (g_D3DAddr.pPresent && g_D3DAddr.pReset)
	{
		return 1;
	}


	//找到D3D9
	CStringA szSySDir;
	GetSystemDirectoryA(szSySDir.GetBuffer(256),256);
	szSySDir.ReleaseBuffer();
	szSySDir+="\\d3d9.dll";
	szSySDir.Replace("\\\\","\\");

	unsigned int D3d9Offset=(unsigned int)GetModuleHandleA("d3d9.dll");
	printf("D3d9= 0x%08x\r\n",D3d9Offset);
	unsigned int StartSearch=D3d9Offset+0x1000;

	CStringA szD3D9;
	GetModuleFileNameA((HMODULE)D3d9Offset,szD3D9.GetBuffer(1024),1024);
	szD3D9.ReleaseBuffer();
	cout<<szD3D9.GetBuffer()<<D3d9Offset<<endl;

	unsigned int Offset_Present=0;
	unsigned int Offset_Reset=0;
	unsigned int Offset_EndScene=0;
	if (GetSystemVersionStr()=="win7")
	{
		printf("Win7\r\n");
		char TargetPresent[]={0x56,0x57,0x8B,0x7D,0x08,0x85,0xFF,0x74,0x44,0x8D,0x77,0x04,0x83,0x7E,0x18,0x00};
		//56 57 8b 7d 08 85 ff 74 44 8d 77 04 83 7e 18 00
		char TargetReset[]={0x81,0xEC,0x44,0x01,0x00,0x00,0x56,0x57,0x8B,0x7D};
		//81 EC 44 01 00 00 56 57 8B 7D
		char TargetEndScene[]={0x8D,0x7E,0x04,0x89,0x7D,0xE0,0x89,0x5D,0xE4,0x39,0x5F,0x18,0x0F,0x85};
		//8D 7E 04 89 7D E0 89 5D E4 39 5F 18 0F 85


		Offset_Present=FindDllTarget(szSySDir,"",TargetPresent,sizeof(TargetPresent),0x100000);
		Offset_Reset=FindDllTarget(szSySDir,"",TargetReset,sizeof(TargetReset),0x100000);
		Offset_EndScene=FindDllTarget(szSySDir,"",TargetEndScene,sizeof(TargetEndScene),0x100000);

		if (Offset_Reset)
		Offset_Reset-=5;

		if (Offset_Present)
		Offset_Present-=5;

		if (TargetEndScene)
			Offset_EndScene-=0x3A;

	}else if (GetSystemVersionStr()=="xp")
	{
		printf("XP\r\n");
		char TargetPresent[]={0x51,0x53,0x8B,0x5D,0x08,0x85,0xDB,0x56,0x74};
		char TargetReset[]={0x83,0xEC,0x1C,0x53,0x8B,0x5D,0x08,0x85,0xDB,0x56};
		char TargetEndScene[]={0xF6,0x46,0x2C,0x02,0x89,0x5D,0xFC,0x75,0x0E,0x8B,0x86,0xA4,0x1F,0x00,0x00,0xA8,0x01,0xC6,0x45,0xFC,0x01,0x75,0x24,0x39,0x5F,0x18,0x74,0x07,0x57};
	
		StartSearch+=0x3E000;
		Offset_Present=FindDllTarget(szSySDir,"",TargetPresent,sizeof(TargetPresent),0x100000);
		Offset_Reset=FindDllTarget(szSySDir,"",TargetReset,sizeof(TargetReset),0x100000);
		Offset_EndScene=FindDllTarget(szSySDir,"",TargetEndScene,sizeof(TargetEndScene),0x100000);

		if (Offset_Reset)
			Offset_Reset-=0x5;

		if (Offset_Present)
			Offset_Present-=5;

		if (Offset_EndScene)
			Offset_EndScene-=0x45;
	}else if (GetSystemVersionStr()=="win8")
		{
			printf("Win8\r\n");
			char TargetPresent[]={0x8D,0x46,0x04,0x83,0x64,0x24,0x0C,0x00,0x83,0x78,0x18,0x00,0x89,0x44,0x24,0x08};
			char TargetReset[]={0x33,0xC4,0x89,0x84,0x24,0x50,0x01,0x00,0x00,0x53,0x8B,0x5D,0x08,0x56,0x8B,0x75,0x0C};
			char TargetEndScene[]={0x89,0x5D,0xE0,0x89,0x7D,0xE4,0x39,0x7B,0x18};
	
			Offset_Present=FindDllTarget(szSySDir,"",TargetPresent,sizeof(TargetPresent),0x100000);
			Offset_Reset=FindDllTarget(szSySDir,"",TargetReset,sizeof(TargetReset),0x100000);
			Offset_EndScene=FindDllTarget(szSySDir,"",TargetEndScene,sizeof(TargetEndScene),0x100000);
	

			if (Offset_Present)
				Offset_Present-=0x13;

			if (Offset_Reset)
				Offset_Reset-=0x13;

	
			if (Offset_EndScene)
				Offset_EndScene-=0x18;
	
		}
	

	
	printf("Reset Offset= 0x%08x\r\n",Offset_Reset);
	printf("Present Offset= 0x%08x\r\n",Offset_Present);
	printf("EndScene Offset= 0x%08x\r\n",Offset_EndScene);
	g_D3DAddr.pPresent=Offset_Present;
	g_D3DAddr.pReset=Offset_Reset;
	g_D3DAddr.pEndScene=Offset_EndScene;
	//
	if (g_D3DAddr.pPresent && g_D3DAddr.pReset && g_D3DAddr.pEndScene)
	{
		return 1;
	}else{
		return 0;
	}
}
BOOL CALLBACK D3D_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{ 
	DWORD dwCurProcessId = *((DWORD*)lParam);
	DWORD dwProcessId = 0; 
	GetWindowThreadProcessId(hwnd, &dwProcessId); 
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{ 
		*((HWND *)lParam) = hwnd;
		return FALSE; 

	} 
	return TRUE;
} 

HWND D3D_GetMainWindow()
{ 
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	if(!EnumWindows(D3D_EnumWindowsProc, (LPARAM)&dwCurrentProcessId))
	{     
		return (HWND)dwCurrentProcessId; 
	} 
	return NULL;
}


unsigned int* D3D_GetD3dVFPrt(HWND hWnd)
{
	D3DDbgPrint("D3D_GetD3dVFPrt   hWnd  0x%08x",hWnd);
	//HookPresentInit();
	//zD3DHook();
	unsigned int* pVmt =0;
	g_CreateDXPoint.pD3d9 =Direct3DCreate9(D3D_SDK_VERSION);
	D3DDbgPrint("D3D_GetD3dVFPrt   pD3d9  0x%08x ",g_CreateDXPoint.pD3d9);
	if (g_CreateDXPoint.pD3d9)
	{
		//获取显示器信息的指针 D3DADAPTER_DEFAULT主显示器
		D3DDISPLAYMODE d3dmm;
		g_CreateDXPoint.pD3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dmm);
		//初始化D3DPRESENT_PARAMETERS
		D3DPRESENT_PARAMETERS d3ddp;
		ZeroMemory(&d3ddp,sizeof(D3DPRESENT_PARAMETERS));
		d3ddp.Windowed = true;
		d3ddp.SwapEffect = D3DSWAPEFFECT_DISCARD;    
		d3ddp.BackBufferFormat = d3dmm.Format;    
		//通过pd3d9指针操作设备对象 初始化工作
		
		HRESULT hResut= g_CreateDXPoint.pD3d9->CreateDevice(
			D3DADAPTER_DEFAULT,//使用默认显卡适配器
			D3DDEVTYPE_HAL,//请求使用硬件抽象层
			(HWND)hWnd,//窗口句柄war3
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,//软件处理顶点
			&d3ddp,//创建的参数
			&g_CreateDXPoint.pDxdevice);//创建IDirect3DDevice8指针
		D3DDbgPrint("D3D_GetD3dVFPrt   pDxdevice  0x%08x  hResut=0x%08x",g_CreateDXPoint.pDxdevice,hResut);
		if (g_CreateDXPoint.pDxdevice) 
		{
			pVmt=*((unsigned int**)g_CreateDXPoint.pDxdevice);
			D3DDbgPrint("pVmt= 0x%08x  \r\n",pVmt);
		}else 
		{
			pVmt=0;
		}
	}

	return pVmt;

}

bool CzjlHookD3D::GetAddrByCreateD3d(HWND hWnd)
{
	D3DDbgPrint("GetAddrByCreateD3d\r\n");
	if (g_D3DAddr.pPresent && g_D3DAddr.pReset)
	{
		return 1;
	}
	unsigned int * vFP=D3D_GetD3dVFPrt(hWnd);


	//0x724BA064
	if (vFP)
	{
		g_D3DAddr.pReset = (int)(vFP[16]);
		g_D3DAddr.pPresent = (int)(vFP[17]);
		g_D3DAddr.pEndScene = (int)(vFP[42]);

		if (g_CreateDXPoint.pDxdevice)
		{
			g_CreateDXPoint.pDxdevice->Release();
		}

		if (g_CreateDXPoint.pD3d9)
		{
			g_CreateDXPoint.pD3d9->Release();
		}
		D3DDbgPrint("pReset= 0x%08x  \r\n", g_D3DAddr.pReset);
		D3DDbgPrint("pPresent= 0x%08x  \r\n", g_D3DAddr.pPresent);
		D3DDbgPrint("pEndScene= 0x%08x  \r\n", g_D3DAddr.pEndScene);
		return 1;
	}
	return 0;
}

bool CzjlHookD3D::HookPresent( pMyPresentCallBack pMyPresent )
{
	//printf("HookPresent 0x%08x",pMyPresent);
	g_D3DAddr.pUserPresent=pMyPresent;
	g_D3DAddr.Hook_Present.SetHookOn();
	return pMyPresent;
}

bool CzjlHookD3D::HookReset( pMyResetCallBack pReset ,pMyResetCallBack pReset_Clar)
{
	g_D3DAddr.pUserResetCreate=pReset;
	g_D3DAddr.pUserReset_Clear=pReset_Clar;
	g_D3DAddr.Hook_Reset.SetHookOn();
	return pReset;
}

bool CzjlHookD3D::HookEndScene( pMyEndSceneCallBack pEndScene )
{
	g_D3DAddr.pUserEndScene=pEndScene;
	g_D3DAddr.Hook_EndScene.SetHookOn();
	return pEndScene;
}


bool CzjlHookD3D::GetSreenSize( unsigned int* x,unsigned int * y )
{
	if (g_D3DData.PresentationParameters.BackBufferHeight && g_D3DData.PresentationParameters.BackBufferWidth)
	{
		*x=g_D3DData.PresentationParameters.BackBufferWidth;
		*y=g_D3DData.PresentationParameters.BackBufferHeight;
		return 1;
	}else{
		return 0;
	}
}

/*
bool CzjlHookD3D::HookEndScene( pMyEndSceneCallBack pEndScene )
{
	g_D3DAddr.pUserEndScene=pEndScene;
	g_D3DAddr.Hook_EndScene.SetHookOn();
	return pEndScene;
}*/


bool CzjlHookD3D::HookPresent_End()
{
	g_D3DAddr.Hook_Present.SetHookOff();
	return 1;
}

bool CzjlHookD3D::HookReset_End()
{
	g_D3DAddr.Hook_Reset.SetHookOff();
	return 1;
}

/*
bool CzjlHookD3D::HookEndScene_End()
{
	g_D3DAddr.Hook_EndScene.SetHookOff();
	return 1;
}*/


LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)    
{    
	PAINTSTRUCT ps;
	HDC dc;
	switch(msg)    
	{    
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);    
		return 0;    
	}    

	return DefWindowProc(hWnd, msg, wParam, lParam);    
}    

void* D3DGetMemShare(CStringA szName,unsigned int Size)
{
	void* pResult=0;
	HANDLE hMap;
	hMap=CreateFileMappingA((HANDLE)-1,0,PAGE_READWRITE,0,Size,szName);


	int LE_CReateMap=GetLastError();

	if (hMap!=INVALID_HANDLE_VALUE)
	{
		pResult=MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0);
	}else{
		return 0;
	}

	if (pResult && LE_CReateMap!=ERROR_ALREADY_EXISTS)
	{//第一次创建
		//初始化数据
		D3DDbgPrint("内存映射%s 清零",szName);
		RtlZeroMemory(pResult,Size);
	}
	return pResult;
}

bool CzjlHookD3D::GetAddrByAppCreateD3D(HWND hWnd)
{
#define WINDOW_CLASS _T("UGPDX")    
#define WINDOW_NAME  _T("Blank D3D Window")    
/*
		
			WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,    
				GetModuleHandle(NULL), NULL, NULL, NULL, NULL,    
				WINDOW_CLASS, NULL };    
			RegisterClassEx(&wc);    
		
			// Create the application's window    
			HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_NAME, WS_OVERLAPPEDWINDOW,    
				0, 0, 1920, 1080, GetDesktopWindow(), NULL,    
				wc.hInstance, NULL);    */
		
	D3DDbgPrint("GetAddrByHookCreateDevice : Handle=%08x",hWnd);
	D3DOffset* pMem=(D3DOffset*)D3DGetMemShare("zjlD3DHelper_234073438",sizeof(D3DOffset));
	if (GetAddrByCreateD3d(hWnd))
	{
		if (pMem)
		{
			unsigned int D3d9Offset=(unsigned int)GetModuleHandleA("d3d9.dll");
/*
				if (pMem->hD3DWnd && IsWindow(pMem->hD3DWnd ))
				{
					SendMessage(pMem->hD3DWnd,WM_CLOSE,0,0);
					SendMessage(pMem->hD3DWnd,WM_QUIT,0,0);
					CloseWindow(pMem->hD3DWnd);
				}*/

				pMem->hD3DWnd=hWnd;
				pMem->Offset_Present=g_D3DAddr.pPresent-D3d9Offset;
				pMem->Offset_Reset=g_D3DAddr.pReset-D3d9Offset;
				pMem->Offset_EndScene=g_D3DAddr.pEndScene-D3d9Offset;
				D3DDbgPrint("Offset_Present=0x%08x",pMem->Offset_Present);
				D3DDbgPrint("Offset_Reset=0x%08x",pMem->Offset_Reset);
				D3DDbgPrint("Offset_EndScene=0x%08x",pMem->Offset_EndScene);

/*
				MSG msg;    
				ZeroMemory(&msg, sizeof(msg));    

				while (GetMessage(&msg, NULL, 0, 0))
				{
					if (!TranslateAccelerator(msg.hwnd, 0, &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
*/
				if (pMem->hD3DWnd &&pMem->Offset_Present && pMem->Offset_Reset	&&pMem->Offset_EndScene)
				{
					return 1;
				}
				
		}
	}
	return 0;

}


bool CzjlHookD3D::CreateWindowFindVPtr(HWND hWnd  )
{
	return GetAddrByAppCreateD3D( hWnd);
}

bool CzjlHookD3D::GetAddrByAppCreateD3D_Client()
{
	D3DOffset* pMem=(D3DOffset*)D3DGetMemShare("zjlD3DHelper_234073438",sizeof(D3DOffset));
	unsigned int D3d9Offset=(unsigned int)GetModuleHandleA("d3d9.dll"); 
	
	if (pMem )
	{
		D3DDbgPrint("GetAddrByAppCreateD3D_Client: h%08x %08x %08x %08x d%08x ",pMem->hD3DWnd,pMem->Offset_Present,pMem->Offset_Reset  , pMem->Offset_EndScene , D3d9Offset);
		if ( IsWindow(pMem->hD3DWnd)  && pMem->Offset_Reset && pMem->Offset_Present && pMem->Offset_EndScene && D3d9Offset )
		{
			g_D3DAddr.pPresent=D3d9Offset+pMem->Offset_Present ;
			g_D3DAddr.pReset=D3d9Offset+pMem->Offset_Reset ;
			g_D3DAddr.pEndScene=D3d9Offset+pMem->Offset_EndScene ;
			D3DDbgPrint("GetAddrByAppCreateD3D_Client: h%08x %08x %08x %08x d%08x ",pMem->hD3DWnd,g_D3DAddr.pPresent,g_D3DAddr.pReset  , g_D3DAddr.pEndScene , D3d9Offset);
			return 1;
		}else{
			D3DDbgPrint("Param error");
		}


	}else{
		D3DDbgPrint("GetAddrByAppCreateD3D_Client ErroR pMem=%08x",pMem);
	}
	return 0;
}





//---------------------------------------------------------------------------
CD3DHookApi_Jmp::CD3DHookApi_Jmp(void)
{ 
	InitializeCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
CD3DHookApi_Jmp::~CD3DHookApi_Jmp()
{ 
	CloseHandle(hProc); 
	DeleteCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
void CD3DHookApi_Jmp::SetHookOn(void)
{ 
	if (m_lpHookFunc==0)
	return;
	DWORD dwOldFlag;
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag))
	{  
		if(WriteProcessMemory(hProc,m_lpHookFunc,m_NewFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))    
				return;  
		} 
	} 
	MessageBoxA(NULL,"SetHookOn","fail",MB_OK); 
	return;
}
//---------------------------------------------------------------------------
void CD3DHookApi_Jmp::SetHookOff(void)
{ 
	if (m_lpHookFunc==0)
		return;
	DWORD dwOldFlag; 
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
	{  
		if(WriteProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))    
				return;  
		}
	}
	MessageBoxW(NULL,L"SetHookOff",L"fail",MB_OK);
	return;
}
//---------------------------------------------------------------------------

BOOL CD3DHookApi_Jmp::Initialize(wchar_t* ModuleName,char* ApiName, FARPROC lpNewFunc)
{
	m_lpHookFunc=0;
	m_lpHookFunc = GetProcAddress(GetModuleHandleW(ModuleName),ApiName);
	hProc = GetCurrentProcess();
	DWORD dwOldFlag;
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
	{  
		if(ReadProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))   
			{    
				m_NewFunc[0]=0xe9;    
				DWORD*pNewFuncAddress;    
				pNewFuncAddress=(DWORD*)&m_NewFunc[1];    
				*pNewFuncAddress=(DWORD)lpNewFunc-(DWORD)m_lpHookFunc-5;
				return TRUE;   
			}  
		}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
void CD3DHookApi_Jmp::Lock(void) //多线程下使用
{
	EnterCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
void CD3DHookApi_Jmp::Unlock(void)
{ 
	LeaveCriticalSection(&m_cs);
}

BOOL CD3DHookApi_Jmp::HookAddr( char* lpAddr, FARPROC lpNewFunc )
{
	m_lpHookFunc=0;
	m_lpHookFunc =(FARPROC) lpAddr;
	hProc = GetCurrentProcess();
	DWORD dwOldFlag;
	if(VirtualProtectEx(hProc,m_lpHookFunc,5,PAGE_READWRITE,&dwOldFlag)) 
	{  
		if(ReadProcessMemory(hProc,m_lpHookFunc,m_OldFunc,5,0))  
		{   
			if(VirtualProtectEx(hProc,m_lpHookFunc,5,dwOldFlag,&dwOldFlag))   
			{    
				m_NewFunc[0]=0xe9;    
				DWORD*pNewFuncAddress;    
				pNewFuncAddress=(DWORD*)&m_NewFunc[1];    
				*pNewFuncAddress=(DWORD)lpNewFunc-(DWORD)m_lpHookFunc-5;
				return TRUE;   
			}  
		}
	}
	return FALSE;
}

bool CD3DHookApi_Jmp::ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size )
{
	DWORD dwOldFlag;
	hProc = GetCurrentProcess();
	if(VirtualProtectEx(hProc,lpAddr,Size,PAGE_EXECUTE_READWRITE,&dwOldFlag)) 
	{  
		memcpy(lpAddr,pBuffer,Size);
		DWORD dwNew;
		VirtualProtectEx(hProc,lpAddr,Size,dwOldFlag,&dwNew);
		return TRUE;
	}else{
		return 0;
	}
}

//---------------------------------------------------------------------------




unsigned int FindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen)
{
	unsigned int  hDllOffset=0;
	unsigned int Result=0;
	szDll.MakeLower();
	szTarget.MakeLower();

	printf("CzjlHook::FindDllTarget Begin :  %s %s\r\n",szDll,szTarget);
	if (szDll.Find(":")!=-1 && PathFileExistsA(szDll))
	{//完整路径

		printf("CzjlHook::FindDllTarget Begin :  %s 匹配完整路径 \r\n",szDll);
		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		BOOL bRet=Module32First(hModuleSnap,&me32);


		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll==szDll)
			{
				hDllOffset=(unsigned int)me32.hModule;

				printf("CzjlHook::FindDllTarget Begin :  %s 完整路径匹配成功 \r\n",szDll);
				break;
			}
		}
		if (hDllOffset==0)
		{
			printf("CzjlHook::FindDllTarget Begin :  %s 匹配完整路径失败 \r\n",szDll);
			return 0;
		}
	}else if (szTarget.GetLength()>0)
	{//相对路径

		printf("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径 \r\n",szDll);

		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		bool bRet=Module32First(hModuleSnap,&me32);
		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll.Find(szTarget)!=-1 && szTheDll.Find(szDll)!=-1)
			{
				hDllOffset=(unsigned int)me32.hModule;
				printf("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径成功 \r\n",szDll);
				break;
			}
		}

		if (hDllOffset==0)
		{
			printf("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径失败 \r\n",szDll);
			return 0;
		}
	}

	if (hDllOffset==0)
	{
		hDllOffset=(unsigned int)GetModuleHandleA(szDll);

		printf("CzjlHook::FindDllTarget Begin :  %s 通过GetModuleHandleA查找起始位置 \r\n",szDll);
	}


	if (hDllOffset)
	{
		//获取DLL代码段
		unsigned int CodeStart=0,CodeEnd=0,CodeSize=0;
		unsigned int SearchS=hDllOffset;
		//计算代码段长度
		bool bCodeing=0;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{			
				if (st.Protect&PAGE_EXECUTE || st.Protect&PAGE_EXECUTE_READ || st.Protect&PAGE_EXECUTE_READWRITE ||st.Protect&PAGE_EXECUTE_WRITECOPY)
				{//代码段
					if (bCodeing==0)
					{//开始记录
						bCodeing=1;
						CodeStart=(int)st.BaseAddress;
					}
					CodeSize+=st.RegionSize;
					break;
				}else{//不是代码段了
					if (bCodeing==1)
					{//结束代码段
						break;
					}
				}
				SearchS+=st.RegionSize;
			}
		}

		if (CodeSize==0)
		{
			printf("%s 没有找到代码段 0x%x \r\n",szDll,CodeStart);

		}

		//查找标志位
		unsigned int searchPos=CodeStart;
		if (SearchLen)
		{//用指定长度
			CodeEnd=CodeStart+SearchLen-LenTag;
		}else{//用默认长度
			CodeEnd=CodeStart+CodeSize-LenTag;
		}
		printf("搜索起始地址 0x%x   搜索长度 0x%x \r\n",searchPos,CodeSize);
		while(searchPos<CodeEnd)
		{
			if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
				break;
			searchPos++;
		}
		//找到
		if (searchPos<CodeEnd-LenTag)
		{
			Result=searchPos;
		}
	}
	printf("CzjlHook::FindDllTarget End=0x%08x \r\n",Result);
	return Result;
}



// ===========================================================
// D3D绘制文字
// ===========================================================
BOOL __stdcall D3DDrawText(LPDIRECT3DDEVICE9 pDxdevice,
	LPTSTR strText ,
	LPTSTR FontName ,
	int FontH,
	int FontW,
	int FWeight,
	int Rleft,
	int Rtop)
{
	int nbuf = lstrlen(strText);

	RECT myrect;
	myrect.left=Rleft; //文本块的x坐标
	myrect.top=Rtop;  //文本块的y坐标
	myrect.right=Rleft+1000;
	myrect.bottom=Rtop+1000;

	D3DXFONT_DESC lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height = FontH; //字体高度
	lf.Width = FontW; // 字体宽度
	lf.Weight = FWeight; 
	lf.Italic = false;
	lf.CharSet = DEFAULT_CHARSET;
	lstrcpy(lf.FaceName,FontName);

	ID3DXFont* font=NULL;
	if(D3D_OK!=D3DXCreateFontIndirect(pDxdevice, &lf, &font)) 
		return false;

	font->DrawText(NULL,strText,nbuf,&myrect,DT_TOP | DT_LEFT,D3DCOLOR_XRGB(255,255,0));

	font->Release();//释放对象
	return TRUE;
}


