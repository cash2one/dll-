#pragma once
#include <atlstr.h>
#include <vector>
#include <Include\d3dx9core.h>
#include <Include\d3d9.h>
#include <Include\d3dx9.h>
#include <atlstr.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;


/*
void __stdcall zD3D9DeviceGetParam(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS * pParam)
{
	OutputDebugStringA("zD3D9DeviceGetParam 1");
	IDirect3DSwapChain9 * pSP;
	pDevice->GetSwapChain(0, &pSP);
	OutputDebugStringA("zD3D9DeviceGetParam 2");

	CStringA str;
	str.Format("zD3D9DeviceGetParam(0x%.8X, 0x%.8X)", pDevice, pParam);
	OutputDebugStringA(str);

	str.Format("pSP = 0x%.8X", pSP);
	OutputDebugStringA(str);

	pSP->GetPresentParameters(pParam);

	OutputDebugStringA("zD3D9DeviceGetParam 3");
	return;
}
*/





#define ZVOID

/************************************************************************/

class zD3D9Texture {
public:
	ZVOID				__stdcall zD3D9Texture		(void);
	ZVOID				__stdcall ~zD3D9Texture		(void);

	BOOL				__stdcall Create			(IDirect3DDevice9 * pDevice, char * szFilePath);
	BOOL				__stdcall Release			(void);
	BOOL				__stdcall GetCreateResult	(void);

	IDirect3DTexture9 *	__stdcall GetTexture		(void);

	BOOL				__stdcall Draw				(ID3DXSprite * pSprite, int dx, int dy, BOOL bTrans);
	BOOL				__stdcall DrawScale			(ID3DXSprite * pSprite, int dx, int dy, float xScale, float yScale, BOOL bTrans);
	BOOL				__stdcall DrawRect			(ID3DXSprite * pSprite, int xSrc, int ySrc, int wSrc, int hSrc, int xDest, int yDest, BOOL bTrans);
	BOOL				__stdcall DrawRectScale		(ID3DXSprite * pSprite, int xSrc, int ySrc, int wSrc, int hSrc, float xScale, float yScale, int xDest, int yDest, BOOL bTrans);

private:
	BOOL				m_bCreate;
	IDirect3DDevice9 *	m_pDevice;
	IDirect3DTexture9 *	m_pTexture;
};

/************************************************************************/

class zD3D9Text {
public:
	ZVOID	__stdcall zD3D9Text			(void);
	ZVOID	__stdcall ~zD3D9Text		(void);

	BOOL	__stdcall Create			(IDirect3DDevice9 * pInitDevice, LPTSTR szFontName, int FontH, int FontW, int FWeight);
	BOOL	__stdcall Release			(void);

	BOOL	__stdcall Draw				(int left, int top, D3DCOLOR c, LPTSTR szDraw);
	BOOL	__stdcall DrawWithFrame		(int left, int top, D3DCOLOR c, D3DCOLOR cFrame, LPTSTR szDraw);

private:
	IDirect3DDevice9 *	m_pDevice;
	ID3DXFont *			m_pFont;
	int					m_fw;
	int					m_fh;
};




struct stD3D9Texture{
	CStringA Tag;
	CStringA szPath;
	zD3D9Texture* pTexture;
	stD3D9Texture()
	{
		pTexture=0;
	}
};

struct stFreeTypeFont{
	CStringA Tag;
	CStringA szPath;
	ID3DXFont* pFontDX;
	stFreeTypeFont(){
		pFontDX=0;
	}
};




class CD3DManager
{
public:
	ID3DXSprite* Init(IDirect3DDevice9 * pInitDevice);
	void FreeRes();



	//绘图
	void Draw_Number(int posX,int posY,int Num,int color,CStringA szFontName);//画数字
	void Draw_String(int posX,int posY,CStringA szChar,int color,CStringA szFontName,int DrawWidth=256);//画字
	BOOL				__stdcall Draw				(CStringA szTag,int dx, int dy, BOOL bTrans);
	BOOL				__stdcall DrawScale			(CStringA szTag, int dx, int dy, float xScale, float yScale, BOOL bTrans);
	BOOL				__stdcall DrawRect			(CStringA szTag, int xSrc, int ySrc, int wSrc, int hSrc, int xDest, int yDest, BOOL bTrans);
	BOOL				__stdcall DrawRectScale		(CStringA szTag, int xSrc, int ySrc, int wSrc, int hSrc, float xScale, float yScale, int xDest, int yDest, BOOL bTrans);




	//资源
	zD3D9Texture* LoadResTex(CStringA szPath,CStringA szTag);
	bool LoadResFont(CStringA szPath,int FontSizeW,int FontSizeH,int Width,CStringA szTag);



	CD3DManager(void);
	~CD3DManager(void);
private:

	//资源
	struct{
		bool m_bResLoad;
		IDirect3DDevice9 * pDevice;
		ID3DXSprite* pSprite;
		vector<stD3D9Texture> m_vTextures;
		vector<stFreeTypeFont> m_vFonts;	
		//CComCriticalSection m_CSRes;
	}m_Res;

	zD3D9Texture* _GetResTex(CStringA szTag);
	ID3DXFont* _GetResFontDX(CStringA szTag);
};


