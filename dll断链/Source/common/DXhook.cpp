
#include "Dxhook.h"
#include "stdio.h"
#include "detours.h"
#pragma comment(lib,"d3dx9.lib")
#include <string.h>
#include <Strsafe.h>


//                        _oo0oo_
//                       o8888888o
//                       88" . "88
//                       (| -_- |)
//                       0\  =  /0
//                     ___/`---'\___
//                   .' \\|     |// '.
//                  / \\|||  :  |||// \
//                 / _||||| -:- |||||- \
//                |   | \\\  -  /// |   |
//                | \_|  ''\---/''  |_/ |
//                \  .-\__  '-'  ___/-. /
//              ___'. .'  /--.--\  `. .'___
//           ."" '<  `.___\_<|>_/___.' >' "".
//          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//          \  \ `_.   \_ __\ /__ _/   .-` /  /
//      =====`-.____`.___ \_____/___.-`___.-'=====
//                        `=---='
// 
// 
//      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//                ���汣��         ����BUG



 typedef HRESULT (_stdcall *PFNENDSCENE)(LPDIRECT3DDEVICE9 pDxdevice);
 typedef HRESULT (_stdcall *PFNBEGINSCENE)(LPDIRECT3DDEVICE9 pDxdevice);
 
 
 IDirect3DDevice9 *g_pDxdevice = NULL;

 LPD3DXFONT              g_Font       = NULL;

 HWND g_hForm;
 
 DWORD  BeginSceneAddr=NULL;
 DWORD  EndSceneAddr=NULL;
 

RECT g_FontPosition;


 BOOL rrr=FALSE;
 PFNENDSCENE pfn_endscene=NULL;
 PFNBEGINSCENE pfn_beginscene=NULL;
 
 BYTE old[5];
 BYTE myadd[5]={0xe9,0x00,0x00,0x00,0x00};
 
 BOOL GetAddrEndScene();
 HRESULT _stdcall My_BeginScene(LPDIRECT3DDEVICE9 pDxdevice);
 void D3DInit()
 {
 	GetAddrEndScene();
 
 	pfn_endscene=(PFNENDSCENE)EndSceneAddr;
 	pfn_beginscene=(PFNBEGINSCENE)(BeginSceneAddr);
 
 	DWORD oldpro;
 	VirtualProtect((LPVOID)(pfn_beginscene),0x5,PAGE_EXECUTE_READWRITE,&oldpro);
 	memcpy(old,(LPCVOID)pfn_beginscene,5);
 	DWORD myjmp=(DWORD)My_BeginScene-0x5-(DWORD)pfn_beginscene;
 	memcpy(myadd+1,&myjmp,4);
 	memcpy(pfn_beginscene,myadd,5);
 	VirtualProtect((LPVOID)(pfn_beginscene),0x5,oldpro,&oldpro);

 }
 
 BOOL GetAddrEndScene()
 {
 
 
 	PDWORD pVmt=0;
 	IDirect3D9 *pD3d9 = NULL;
 	IDirect3DDevice9 *pDxdevice = NULL;
 	IDirect3D9 * (WINAPI *pfnDirect3DCreate9)(int)  = (IDirect3D9 *(WINAPI *)(int))GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9");	//IDirect3D9 * WINAPI Direct3DCreate8(UINT SDKVersion)
 	pD3d9 = pfnDirect3DCreate9(D3D_SDK_VERSION);
 
 	if (pD3d9) 
 	{
 		//��ȡ��ʾ����Ϣ��ָ�� D3DADAPTER_DEFAULT����ʾ��
 		D3DDISPLAYMODE d3dmm;
 		pD3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dmm);
 		//��ʼ��D3DPRESENT_PARAMETERS
 		D3DPRESENT_PARAMETERS d3ddp;
 		ZeroMemory(&d3ddp,sizeof(d3ddp));
 		//���Ӧ�ó������д����棻�����Ӧ�ó������е�ȫ��Ļ��
 		d3ddp.Windowed = true;
 		//d3dswapeffect_discard���ڵ�������ͨ����������κλ�������ǿ��ִ�С�
 		d3ddp.SwapEffect = D3DSWAPEFFECT_DISCARD;
 		//��֪����ɶ��˼
 		d3ddp.BackBufferFormat = d3dmm.Format;
 		//��֪�������
 		pVmt = *((PDWORD*)pD3d9);		
 
 		//ͨ��pd3d9ָ������豸���� ��ʼ������
 		pD3d9->CreateDevice(
 			D3DADAPTER_DEFAULT,//ʹ��Ĭ���Կ�������
 			D3DDEVTYPE_HAL,//����ʹ��Ӳ�������
 			(HWND)g_hForm,//���ھ��war3
 			D3DCREATE_SOFTWARE_VERTEXPROCESSING,//���������
 			&d3ddp,//�����Ĳ���
 			&pDxdevice);//����IDirect3DDevice8ָ��
 
 		if (pDxdevice) 
 		{
 			pVmt=*((PDWORD*)pDxdevice);
 			EndSceneAddr= pVmt[42];
 			BeginSceneAddr=pVmt[41];
 
 			// 			pDxdevice->Release();
 			//  			pDxdevice = NULL;
 		}
 		// 		
 		// 		pD3d9->Release();
 		// 		pD3d9 = NULL;
 	}
 
 
 	if (EndSceneAddr&&BeginSceneAddr)
 	{
 		return TRUE;
 	}
 	return FALSE;
 }
 
 

 

 HRESULT _stdcall My_EndScene(LPDIRECT3DDEVICE9 pDxdevice)
 {
 
	g_pDxdevice = pDxdevice;

	if(g_Font)
	{
		//RECT FontPosition = {5,5,199,99};
		//printf("%x %x %x %x\r\n",g_FontPosition.left,g_FontPosition.top,g_FontPosition.right,g_FontPosition.bottom);
		//g_Font->DrawText(NULL,"��񿪹�", -1, &FontPosition, DT_CENTER, D3DCOLOR_XRGB(255,111,111));

		//g_Font->Release();
	}
	else
	{
		D3DXFONT_DESCA fontDesc;
		ZeroMemory(&fontDesc, sizeof(D3DXFONT_DESC));

		fontDesc.Height         = 13;    // ��
		fontDesc.Width          = 8;    // ��
		fontDesc.Weight         = 800;   // ����(0(ϸ) - 1000(��))
		fontDesc.Italic         = false;  
		fontDesc.CharSet        = DEFAULT_CHARSET;
		fontDesc.OutputPrecision   = 0;             
		fontDesc.MipLevels       = D3DX_DEFAULT;         
		fontDesc.Quality        = 0;          
		fontDesc.PitchAndFamily = 0;          
		StrCpyA(fontDesc.FaceName, "Times New Roman"); // font style

		if(D3D_OK != (D3DXCreateFontIndirectA(pDxdevice, &fontDesc, &g_Font)))
		{

		}
	}
	return pfn_endscene(pDxdevice);
 		
 }
 HRESULT _stdcall My_BeginScene(LPDIRECT3DDEVICE9 pDxdevice)
 {
 	if(!rrr)
 	{
 		DWORD oldpro;
 
 		VirtualProtect((LPVOID)(pfn_beginscene),0x5,PAGE_EXECUTE_READWRITE,&oldpro);
 		memcpy(pfn_beginscene,old,5);
 		//WriteProcessMemory(Handle_War3,pfn_beginscene,old,5,NULL);
 		VirtualProtect((LPVOID)(pfn_beginscene),0x5,oldpro,&oldpro);
 		PDWORD pVmt = *((PDWORD*)pDxdevice);
 		VirtualProtect(&((PDWORD*)pVmt)[42], sizeof(PVOID), PAGE_READWRITE, &oldpro);
 		char tmp[255];sprintf(tmp,"��ȡ�� EndSence %x",&((PDWORD*)pVmt)[42]);OutputDebugStringA(tmp);
 		//(pVmt)[42]=(DWORD)My_EndScene;
 		pfn_endscene = (PFNENDSCENE)DetourFunction((PBYTE)(pVmt)[42], (PBYTE)My_EndScene);
 		VirtualProtect(&((PDWORD*)pVmt)[42], sizeof(PVOID), oldpro, &oldpro);
 
 		rrr=TRUE;
 	}
 	return pfn_beginscene(pDxdevice);
 
 }