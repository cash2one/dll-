#include "D3DManager.h"
#include "zjlcommon.h"





zD3D9Texture* CD3DManager::LoadResTex(CStringA szPath,CStringA szTag )
{
	zD3D9Texture* pResult=0;

	if (m_Res.pDevice==0)
	{
		DbgPrint("Error::_LoadResTex ErrorIDirect3DDevice9");
		return 0;
	}

	if(szTag=="")
	{
		DbgPrint("Error::_LoadResTex szTag==\"\"");
		return 0;
	}

	//查找是否已添加过
	for (unsigned int i=0;i<m_Res.m_vTextures.size();i++)
	{
		if (m_Res.m_vTextures[i].Tag == szTag)
		{
			return m_Res.m_vTextures[i].pTexture;
		}
	}

	if ( PathFileExistsA(szPath)==0 )
	{

		stD3D9Texture add;
		add.Tag=szTag;
		add.szPath=szPath;
		add.pTexture=0;
		m_Res.m_vTextures.push_back(add);
		DbgPrint("Error::_LoadResTex  FileNotFound [%s] Tag=[%s]",szPath,szTag);
		return 0;
	}

	zD3D9Texture* p=0;
	p=new zD3D9Texture;
	if (p&&	p->Create(m_Res.pDevice,szPath.GetBuffer())	)
	{
			stD3D9Texture add;
			add.Tag=szTag;
			add.szPath=szPath;
			add.pTexture=p;
			DbgPrint("LoadRes OK! File [%s] Tag=[%s]",szPath,szTag);
			//m_Res.m_CSRes.Lock();
			m_Res.m_vTextures.push_back(add);
			//m_Res.m_CSRes.Unlock();
			return p;
	}else{
		if (p)
		{
			delete p;
		}	
		DbgPrint("Error::_LoadResTex Create Error %s",szPath);
	}
	return 0;
}

zD3D9Texture* CD3DManager::_GetResTex( CStringA szTag )
{

	if (szTag=="")
	return 0;

	zD3D9Texture* pResult=0;
	//m_Res.m_CSRes.Lock();
	for (unsigned int i=0;i<m_Res.m_vTextures.size();i++)
	{
		if (m_Res.m_vTextures[i].Tag == szTag)
		{
			pResult= m_Res.m_vTextures[i].pTexture;
		}
	}
	//m_Res.m_CSRes.Unlock();
	return pResult;
}
/*
CFreeTypeFont* CD3DManager::_GetResFontFT( CStringA szTag )
{
	for (unsigned int i=0;i<m_Res.m_vFonts.size();i++)
	{
		if (m_Res.m_vFonts[i].Tag== szTag)
		{
			return m_Res.m_vFonts[i].pFontFT;
		}
	}

	return 0;
}


bool CD3DManager::_LoadResFont( IDirect3DDevice9 * pInitDevice,CStringA szPath,int FontSize,CStringA szTag )
{
	if (szTag=="" || PathFileExistsA(szPath)==0 || FontSize==0 )
	{
		DbgPrint("Error::_LoadResFont FileNotFound [%s] Tag=[%s] Size=[%d]",szPath,szTag,FontSize);
		return 0;
	}

	if (pInitDevice==0)
	{
		DbgPrint("Error::_LoadResFont ErrorIDirect3DDevice9");
		return 0;
	}

	CFreeTypeFont* p=new CFreeTypeFont(pInitDevice,szPath.GetBuffer(),FontSize);
	if (p)
	{
		stFreeTypeFont add;
		add.Tag=szTag;
		add.szPath=szPath;
		add.pFontFT=p;
		DbgPrint("LoadRes OK! File [%s] Tag=[%s] Size=[%d]",szPath,szTag,FontSize);
		m_Res.m_vFonts.push_back(add);
		return 1;
	}else{
		delete p;
	}
	DbgPrint("Error::_LoadResFont Create Error %s",szPath);
	return 0;
}*/


bool CD3DManager::LoadResFont( CStringA szPath,int FontSizeW,int FontSizeH,int Width,CStringA szTag )
{
	if (szTag=="" || szPath=="" )
	{
		DbgPrint("Error::_LoadResFont FileNotFound [%s] Tag=[%s] Size=[%d %d %d]",szPath,szTag,FontSizeW,FontSizeH,Width);
		return 0;
	}

	if (m_Res.pDevice==0)
	{
		DbgPrint("Error::_LoadResFont ErrorIDirect3DDevice9");
		return 0;
	}

	if (_GetResFontDX(szTag))
	{//已经加载过
		return 0;
	}

	D3DXFONT_DESC lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height = FontSizeH; //字体高度
	lf.Width = FontSizeW; // 字体宽度
	lf.Weight = Width*100; 
	lf.Italic = false;
	lf.Quality=6;
	lf.CharSet = DEFAULT_CHARSET;
	lstrcpy(lf.FaceName,szPath);

	ID3DXFont* p=NULL;
	if(D3D_OK!=D3DXCreateFontIndirect(m_Res.pDevice, &lf, &p) ) 
		return false;

	if (p)
	{
		stFreeTypeFont add;
		add.Tag=szTag;
		add.szPath=szPath;
		add.pFontDX=p;
		DbgPrint("LoadRes OK! File [%s]  Tag=[%s] Size=[%d %d %d]",szPath,szTag,FontSizeW,FontSizeH,Width);
		//m_Res.m_CSRes.Lock();
		m_Res.m_vFonts.push_back(add);
		//m_Res.m_CSRes.Unlock();
		return 1;
	}else{
		delete p;
	}
	DbgPrint("Error::_LoadResFont Create Error %s",szPath);
	return 0;
}

ID3DXFont* CD3DManager::_GetResFontDX( CStringA szTag )
{
	ID3DXFont* pResult=0;
	//m_Res.m_CSRes.Lock();
	for (unsigned int i=0;i<m_Res.m_vFonts.size();i++)
	{
		if (m_Res.m_vFonts[i].Tag== szTag)
		{
			pResult= m_Res.m_vFonts[i].pFontDX;
		}
	}
	//m_Res.m_CSRes.Unlock();
	return pResult;
}

ID3DXSprite* CD3DManager::Init( IDirect3DDevice9 * pInitDevice )
{
	if (D3DXCreateSprite(pInitDevice,&m_Res.pSprite)!=S_OK || m_Res.pSprite==0)
	{
		DbgPrint("CreateSprite Error");
		FreeRes();
		m_Res.pDevice=0;
		return m_Res.pSprite;
	}
	m_Res.pDevice=pInitDevice;
	return 0;
}
void CD3DManager::FreeRes()
{
	m_Res.pDevice=0;

	for (unsigned int i=0;i<m_Res.m_vTextures.size();i++)
	{
		if (m_Res.m_vTextures[i].pTexture)
		{
			zD3D9Texture* pDelete=m_Res.m_vTextures[i].pTexture;
			m_Res.m_vTextures[i].pTexture=0;
			delete pDelete;
		}
	}
	m_Res.m_vTextures.clear();




	for (unsigned int i=0;i<m_Res.m_vFonts.size();i++)
	{

		if (m_Res.m_vFonts[i].pFontDX)
		{
			ID3DXFont* pDeleteFont=m_Res.m_vFonts[i].pFontDX;
			m_Res.m_vFonts[i].pFontDX=0;
			pDeleteFont->Release();			
		}
	}
	m_Res.m_vFonts.clear();


	if (m_Res.pSprite)
	{
		ID3DXSprite* pSprite=m_Res.pSprite;
		m_Res.pSprite=0;
		pSprite->Release();
	}
}

void CD3DManager::Draw_String( int posX,int posY,CStringA szChar,int color,CStringA szFontName,int DrawWidth/*=256*/ )
{
	ID3DXFont* pF=_GetResFontDX(szFontName);

	CStringA szNum;
	szNum.Format("%s",szChar);
	RECT  rect;
	rect.left=posX;
	rect.top=posY;
	rect.right=posX+DrawWidth;
	rect.bottom=posY+DrawWidth;
	if (pF)
	{
		pF->DrawTextA(0,szNum,szNum.GetLength(),&rect,DT_TOP | DT_LEFT|DT_WORDBREAK,color);
	}
}

void CD3DManager::Draw_Number( int posX,int posY,int Num,int color,CStringA szFontName )
{
	ID3DXFont* pF=_GetResFontDX(szFontName);


	CStringA szNum;
	szNum.Format("%d",Num);
	RECT  rect;
	rect.left=posX;
	rect.top=posY;
	rect.right=posX+128;
	rect.bottom=posY+128;
	if (pF)
	{
		pF->DrawTextA(0,szNum,szNum.GetLength(),&rect,DT_TOP | DT_LEFT|DT_WORDBREAK,color);
	}
}

CD3DManager::CD3DManager( void )
{
	m_Res.pDevice=0;
	m_Res.pSprite=0;
}

CD3DManager::~CD3DManager( void )
{

}

BOOL				__stdcall CD3DManager::Draw( CStringA szTag,int dx, int dy, BOOL bTrans )
{
	zD3D9Texture* pRes= _GetResTex(szTag);
	if (pRes && m_Res.pSprite)
	{
		return pRes->Draw(m_Res.pSprite,dx,dy,bTrans);
	}
	return 0;
}

BOOL				__stdcall CD3DManager::DrawScale( CStringA szTag, int dx, int dy, float xScale, float yScale, BOOL bTrans )
{
	zD3D9Texture* pRes= _GetResTex(szTag);
	if (pRes && m_Res.pSprite)
	{
		return pRes->DrawScale(m_Res.pSprite,dx,dy,xScale,yScale,bTrans);
	}
	return 0;
}

BOOL				__stdcall CD3DManager::DrawRect( CStringA szTag, int xSrc, int ySrc, int wSrc, int hSrc, int xDest, int yDest, BOOL bTrans )
{
	zD3D9Texture* pRes= _GetResTex(szTag);
	if (pRes && m_Res.pSprite)
	{
		return pRes->DrawRect(m_Res.pSprite,xSrc,ySrc,wSrc,hSrc,xDest,yDest,bTrans);
	}
	return 0;
}

BOOL				__stdcall CD3DManager::DrawRectScale( CStringA szTag, int xSrc, int ySrc, int wSrc, int hSrc, float xScale, float yScale, int xDest, int yDest, BOOL bTrans )
{
	zD3D9Texture* pRes= _GetResTex(szTag);
	if (pRes && m_Res.pSprite)
	{
		return pRes->DrawRectScale(m_Res.pSprite,xSrc,ySrc,wSrc,hSrc,xScale,yScale,xDest,yDest,bTrans);
	}
	return 0;
}



ZVOID __stdcall zD3D9Texture::zD3D9Texture(void)
{
	this->m_bCreate = FALSE;
	this->m_pTexture = NULL;
}

ZVOID __stdcall zD3D9Texture::~zD3D9Texture(void)
{
	this->Release();
}

BOOL __stdcall zD3D9Texture::Create(IDirect3DDevice9 * pDevice, char * szFilePath)
{
	if (pDevice == NULL || pDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (m_bCreate == TRUE && m_pTexture != NULL && m_pTexture != INVALID_HANDLE_VALUE)
	{
		this->Release();
	}

	HRESULT hCreate = D3DXCreateTextureFromFileA(pDevice, szFilePath, &this->m_pTexture);

	if (FAILED(hCreate))
	{
		this->m_pTexture = NULL;
		return FALSE;
	}

	this->m_pDevice = pDevice;

	this->m_bCreate = TRUE;
	return TRUE;
}

BOOL __stdcall zD3D9Texture::Release(void)
{
	if (this->m_bCreate == FALSE)
	{
		if (this->m_pTexture != NULL && this->m_pTexture != INVALID_HANDLE_VALUE)
		{
			this->m_pTexture = NULL;
		}
		return TRUE;
	}
	else
	{
		if (this->m_pTexture != NULL && this->m_pTexture != INVALID_HANDLE_VALUE)
		{
			this->m_pTexture->Release();
		}
		this->m_bCreate = FALSE;
		this->m_pTexture = NULL;
		this->m_pDevice = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL __stdcall zD3D9Texture::GetCreateResult(void)
{
	return this->m_bCreate;
}

IDirect3DTexture9 * __stdcall zD3D9Texture::GetTexture(void)
{
	if (this->m_bCreate)
	{
		return this->m_pTexture;
	}
	return NULL;
}

BOOL __stdcall zD3D9Texture::Draw(ID3DXSprite * pSprite, int dx, int dy, BOOL bTrans)
{
	if (pSprite == NULL || pSprite == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (m_bCreate == FALSE)
	{
		return FALSE;
	}
	else if (m_pDevice == NULL || m_pTexture == NULL)
	{
		return FALSE;
	}

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1);
	pSprite->SetTransform(&scale);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &identity);

	if(bTrans)
	{
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
	else
	{
		pSprite->Begin(0);
	}

	pSprite->Draw(this->m_pTexture, NULL, NULL, &D3DXVECTOR3((float)dx, (float)dy, 0), 0xFFFFFFFF);
	pSprite->End();
	return TRUE;
}

BOOL __stdcall zD3D9Texture::DrawScale(ID3DXSprite * pSprite, int dx, int dy, float xScale, float yScale, BOOL bTrans)
{
	if (pSprite == NULL || pSprite == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (m_bCreate == FALSE)
	{
		return FALSE;
	}
	else if (m_pDevice == NULL || m_pTexture == NULL)
	{
		return FALSE;
	}

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xScale, yScale, 1);
	pSprite->SetTransform(&scale);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &identity);

	if(bTrans)
	{
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
	else
	{
		pSprite->Begin(0);
	}

	pSprite->Draw(this->m_pTexture, NULL, NULL, &D3DXVECTOR3((float)dx / xScale,(float)dy / yScale,0), 0xFFFFFFFF);
	pSprite->End();
	return TRUE;
}

BOOL __stdcall zD3D9Texture::DrawRect(ID3DXSprite * pSprite, int xSrc, int ySrc, int wSrc, int hSrc, int xDest, int yDest, BOOL bTrans)
{
	if (pSprite == NULL || pSprite == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (m_bCreate == FALSE)
	{
		return FALSE;
	}
	else if (m_pDevice == NULL || m_pTexture == NULL)
	{
		return FALSE;
	}

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1);
	pSprite->SetTransform(&scale);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &identity);

	if(bTrans)
	{
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
	else
	{
		pSprite->Begin(0);
	}

	RECT rcSrc;
	rcSrc.left = xSrc;
	rcSrc.top = ySrc;
	rcSrc.right = xSrc + wSrc;
	rcSrc.bottom = ySrc + hSrc;

	pSprite->Draw(this->m_pTexture, &rcSrc, NULL, &D3DXVECTOR3((float)xDest,(float)yDest, 0), 0xFFFFFFFF);
	pSprite->End();
	return TRUE;
}

BOOL __stdcall zD3D9Texture::DrawRectScale(ID3DXSprite * pSprite, int xSrc, int ySrc, int wSrc, int hSrc, float xScale, float yScale, int xDest, int yDest, BOOL bTrans)
{
	if (pSprite == NULL || pSprite == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if (m_bCreate == FALSE)
	{
		return FALSE;
	}
	else if (m_pDevice == NULL || m_pTexture == NULL)
	{
		return FALSE;
	}

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, xScale, yScale, 1);
	pSprite->SetTransform(&scale);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &identity);

	if(bTrans)
	{
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
	else
	{
		pSprite->Begin(0);
	}

	RECT rcSrc;
	rcSrc.left = xSrc;
	rcSrc.top = ySrc;
	rcSrc.right = xSrc + wSrc;
	rcSrc.bottom = ySrc + hSrc;

	pSprite->Draw(this->m_pTexture, &rcSrc, NULL, &D3DXVECTOR3((float)xDest / xScale,(float)yDest / yScale,0), 0xFFFFFFFF);
	pSprite->End();
	return TRUE;
}


/************************************************************************/

ZVOID __stdcall zD3D9Text::zD3D9Text(void)
{
	this->m_pDevice = NULL;
	this->m_pFont = NULL;
	this->m_fw = 0;
	this->m_fh = 0;
}

ZVOID __stdcall zD3D9Text::~zD3D9Text(void)
{
	this->Release();
}


BOOL __stdcall zD3D9Text::Create(IDirect3DDevice9 * pInitDevice, LPTSTR szFontName, int FontH, int FontW, int FWeight)
{
	if (pInitDevice == NULL || pInitDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	m_pDevice = pInitDevice;
	m_fw = FontW;
	m_fh = FontH;

	D3DXFONT_DESC lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height = FontH; //字体高度
	lf.Width = FontW; // 字体宽度
	lf.Weight = FWeight; 
	lf.Italic = false;
	lf.CharSet = DEFAULT_CHARSET;
	lstrcpy(lf.FaceName,szFontName);

	m_pFont = NULL;
	if( D3D_OK != D3DXCreateFontIndirect(pInitDevice, &lf, &m_pFont)) 
	{
		return FALSE;
	}

	return TRUE;
}

BOOL __stdcall zD3D9Text::Release(void)
{
	if (m_pFont == 0)
	{
		return FALSE;
	}
	m_pFont->Release();
	m_pFont = 0;

	return TRUE;
}

BOOL __stdcall zD3D9Text::Draw(int left, int top, D3DCOLOR c, LPTSTR szDraw)
{
	if (m_pFont == 0)
	{
		return FALSE;
	}
	int nbuf = lstrlen(szDraw);

	RECT rcDraw;
	rcDraw.left = left; //文本块的x坐标
	rcDraw.top = top;  //文本块的y坐标
	rcDraw.right = left + (m_fw + 2) * nbuf;
	rcDraw.bottom = top + m_fh + 16;

	m_pFont->DrawText(NULL, szDraw, nbuf, &rcDraw, DT_TOP | DT_LEFT, c);

	return TRUE;
}

BOOL __stdcall zD3D9Text::DrawWithFrame(int left, int top, D3DCOLOR c, D3DCOLOR cFrame, LPTSTR szDraw)
{
	if (m_pFont == 0)
	{
		return FALSE;
	}
	this->Draw(left - 1, top - 1, cFrame, szDraw);
	this->Draw(left - 0, top - 1, cFrame, szDraw);
	this->Draw(left + 1, top - 1, cFrame, szDraw);
	this->Draw(left - 1, top - 0, cFrame, szDraw);
	this->Draw(left - 0, top - 0, cFrame, szDraw);
	this->Draw(left + 1, top - 0, cFrame, szDraw);
	this->Draw(left - 1, top + 1, cFrame, szDraw);
	this->Draw(left - 0, top + 1, cFrame, szDraw);
	this->Draw(left + 1, top + 1, cFrame, szDraw);

	this->Draw(left, top, c, szDraw);

	return TRUE;
}
