#include "zD3D9.h"
/************************************************************************/

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
