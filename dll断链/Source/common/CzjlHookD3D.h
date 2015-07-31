#pragma once
#include "windows.h"
#include <wtypes.h>
#include <windef.h>
#include <winbase.h>
#include <d3d9.h>


typedef HRESULT   (__stdcall* pMyPresentCallBack)( IDirect3DDevice9* pThis,THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
typedef HRESULT (__stdcall* pMyResetCallBack)(IDirect3DDevice9 * pThis ,D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT  (__stdcall*  pMyEndSceneCallBack)(IDirect3DDevice9 * pThis );
typedef HRESULT  (__stdcall*  pMyBeginSceneCallBack)(IDirect3DDevice9 * pThis );

 struct D3DOffset{
	HWND hD3DWnd;
	unsigned int Offset_Present;
	unsigned int Offset_Reset;
	unsigned int Offset_EndScene;
};


class CzjlHookD3D
{
public:
	CzjlHookD3D(void);
	~CzjlHookD3D(void);

	bool CreateWindowFindVPtr(HWND hWnd);

	bool Init(int Type);//1=特征码提取.2=通过创建设备对象提取VF指针.3=通过外面的D3DHelper.exe内存传进来地址



	bool HookPresent(pMyPresentCallBack pMyPresent);
	bool HookPresent_End();

	bool HookReset(pMyResetCallBack pReset,pMyResetCallBack pReset_Clar);
	bool HookReset_End();


	bool HookEndScene(pMyEndSceneCallBack pEndScene);
	bool HookEndScene_End();


	bool GetSreenSize(unsigned int* x,unsigned int * y);//屏幕坐标
private:
	bool GetAddrByCode();
	bool GetAddrByCreateD3d(HWND hWnd);

	bool GetAddrByAppCreateD3D(HWND hWnd);//D3DHelper.exe
	bool GetAddrByAppCreateD3D_Client();//dll读取上面的

};

class CD3DHookApi_Jmp  
{
public: 
	HANDLE hProc; 
	void Unlock(void);
	void Lock(void); 
	BOOL Initialize(wchar_t*  ModuleName, char*  ApiName, FARPROC lpNewFunc);
	BOOL HookAddr(char* lpAddr, FARPROC lpNewFunc);
	void SetHookOn(void); 
	void SetHookOff(void); 
	CD3DHookApi_Jmp(void); 
	virtual ~CD3DHookApi_Jmp();

	bool ReplaceMem( char* lpAddr,char* pBuffer,unsigned int Size );

	BYTE m_OldFunc[8]; 
protected: 
	BYTE m_NewFunc[8]; 
	FARPROC m_lpHookFunc; 
	CRITICAL_SECTION m_cs;
};