#include "FreeTypeFont.h"   
#include <assert.h>   
#include <fstream>   



#pragma   warning(   push   )    
#pragma   warning(   disable   :   4244   )  /*disable long to float*/   
#pragma   warning(   disable   :   4267   )  /*disable size_t to int*/   

CFreeTypeFont::CFreeTypeFont(IDirect3DDevice9* pDevice,char* szPathTTF,unsigned int FontSize)  
	:m_pDevice(pDevice),  
	m_FT2Lib(NULL),  
	m_FT_Face(NULL),  
	_bOutLine(false),  
	_isInit(false),  
	_texUnitSize(0),  
	_texUnitLen(0),  
	_texUnitIndex(-1)  
{  
	init(szPathTTF, FontSize);  
	assert(_isInit);  

	_texUnitLen = TEXTURE_SIZE / _texUnitSize;  

	_pTexIndices = new LPCHAR_TEX[_texUnitLen * _texUnitLen];  
	memset(_pTexIndices, 0, sizeof(_pTexIndices) * _texUnitLen * _texUnitLen);  
	_pCharIndices = new LPCHAR_TEX[256 * 256];  
	memset(_pCharIndices, 0, sizeof(_pCharIndices) * 256 * 256);  
}  


CFreeTypeFont::~CFreeTypeFont(void)  
{  
	if (_pTex)  
	{  
		_pTex->Release();  
		_pTex = NULL;  
	}  
	if(m_FT2Lib)  
		FT_Done_FreeType(m_FT2Lib);  

	for (int i = 0; i < _texUnitLen * _texUnitLen; i++)  
	{  
		if (_pTexIndices[i])  
		{  
			delete _pTexIndices[i];  
		}  
	}  

	delete []_pTexIndices;  
	delete []_pCharIndices;  
}  

void CFreeTypeFont::init(const std::string& fontFile, const int& fontSize)  
{  
	_texUnitSize = fontSize;  

	if (FT_Init_FreeType( &m_FT2Lib) )   
	{  
		FT_Done_FreeType(m_FT2Lib);  
		m_FT2Lib = NULL;  
		return;  
	}  

	if (FT_New_Face(m_FT2Lib,fontFile.c_str(),0,&m_FT_Face ))  
	{  
		FT_Done_FreeType(m_FT2Lib);  
		m_FT2Lib = NULL;  
		return;  
	}  


	FT_Select_Charmap(m_FT_Face, FT_ENCODING_UNICODE);  

	FT_Set_Pixel_Sizes(m_FT_Face,fontSize, fontSize);  

	if (m_pDevice->CreateTexture(TEXTURE_SIZE, TEXTURE_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &_pTex, NULL) != D3D_OK)  
	{  
		FT_Done_FreeType(m_FT2Lib);  
		_pTex=0;
		m_FT2Lib = NULL;  
		return;  
	}  
	_pTexBuf = new BYTE[TEXTURE_SIZE*TEXTURE_SIZE*4];  
	memset( _pTexBuf , 0 , TEXTURE_SIZE*TEXTURE_SIZE*4 );  

	_isInit = true;  
}  

void CFreeTypeFont::DrawSimpleText( const std::string& str, const int& size, const RECT& rect, const DWORD& color, bool bOutLine /*= false*/ )  
{  
	if (str.empty())  
	{  
		return;  
	}  

	float scale = (float)size /_texUnitSize;   
	int lineHeight = _texUnitSize + 4;  

	std::vector<FontVertex> fontVecs;  
	unsigned long charcode = 0;  
	const char* p = str.c_str();  
	long xPos = rect.left, yPos = rect.top;  
	while (*p)  
	{  
		if (*p == '\n')  
		{  
			p++;  
			xPos = rect.left;  
			yPos += lineHeight * scale;  
			continue;  
		}  
		if (*p & 0x80)  
		{  
			charcode = *(WORD*)p;  
		}  
		else   
		{  
			charcode = *(char*)p;  
		}  
		LPCHAR_TEX pCharTex = _pCharIndices[charcode];  
		if (pCharTex == NULL)  
		{  
			_texUnitIndex++;  
			if (_pTexIndices[_texUnitIndex] == NULL)  
			{  
				_pCharIndices[charcode] = new CHAR_TEX();  
				_pCharIndices[charcode]->c = charcode;  
				_pTexIndices[_texUnitIndex] = _pCharIndices[charcode];  
			}  
			else  
			{  
				//exchange texture   
				long tempIndex= _texUnitIndex;  
				while(_pTexIndices[_texUnitIndex]->used)  
				{  
					if (_texUnitIndex == _texUnitLen * _texUnitLen)  
					{  
						_texUnitIndex = -1;  
					}  
					_texUnitIndex++;  
					if (_texUnitIndex == tempIndex)  
					{  
						//all used   
						assert(false);  
					}  
				}  
				_pTexIndices[_pTexIndices[_texUnitIndex]->c] = NULL;  
				_pTexIndices[charcode] = _pTexIndices[_texUnitIndex];  
			}  
			_pCharIndices[charcode]->c = charcode;  
			_pCharIndices[charcode]->used = true;  
			_pCharIndices[charcode]->row = _texUnitIndex / _texUnitLen;  
			_pCharIndices[charcode]->col = _texUnitIndex % _texUnitLen;  
			copyCharToTexture(_pCharIndices[charcode]);  
		}  

		float x = xPos + _pCharIndices[charcode]->deltaX* scale - 0.5;  
		float y = yPos - _pCharIndices[charcode]->deltaY* scale + (lineHeight - _texUnitSize ) / 2  - 0.5 ;  
		float addX = _pCharIndices[charcode]->width * scale;  
		float addY = _pCharIndices[charcode]->height * scale;  
		if (*p & 0x80)  
		{  
			p += 2;  
		}  
		else   
		{  
			p += 1;  
		}  
		xPos += _pCharIndices[charcode]->width * scale;  

		//render   
		FontVertex vecs[4];  
		float bround = 0.001f;  
		float u = (float)(_pCharIndices[charcode]->col * _texUnitSize)  / TEXTURE_SIZE ;  
		float v = (float)(_pCharIndices[charcode]->row * _texUnitSize)  / TEXTURE_SIZE ;   
		float addU = (float)(_pCharIndices[charcode]->width) / TEXTURE_SIZE - bround ;  
		float addV = (float)(_pCharIndices[charcode]->height)/ TEXTURE_SIZE - bround ;  
		vecs[0] = FontVertex(x , y, 0.0f, 1.0f, color, u, v);  
		vecs[1] = FontVertex(x, y + addY , 0.0f, 1.0f, color, u, v + addV);  
		vecs[2] = FontVertex(x + addX, y, 0.0f, 1.0f, color, u + addU, v);  
		vecs[3] = FontVertex(x + addX, y + addY, 0.0f, 1.0f, color, u + addU, v + addV);  
		fontVecs.push_back(vecs[0]);  
		fontVecs.push_back(vecs[1]);  
		fontVecs.push_back(vecs[2]);  
		fontVecs.push_back(vecs[1]);  
		fontVecs.push_back(vecs[2]);  
		fontVecs.push_back(vecs[3]);  
	}  
	render(fontVecs);  
}  

void CFreeTypeFont::DrawSimpleTextByXY( const std::string& str, const int& size, int x ,int y, const DWORD& color, bool bOutLine /*= false*/ )
{
	RECT rect;
	rect.left=x;
	rect.top=y;
	rect.right=x+1024;
	rect.bottom=y+1024;

	DrawSimpleText(str,size,rect,color,bOutLine);
}

bool CFreeTypeFont::copyCharToTexture(LPCHAR_TEX pCharTex)  
{  
	char*  chr = (char*)&(pCharTex->c);  
	wchar_t wchar;  

	MultiByteToWideChar( CP_ACP, 0, chr,strlen(chr)+1, &wchar,1 );  
	if(FT_Load_Char(m_FT_Face, wchar,FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT) )  
		return false;  

	FT_GlyphSlot slot = m_FT_Face->glyph;  
	FT_Bitmap bitmap = slot->bitmap;  

	int width  =  bitmap.width;  
	int height =  bitmap.rows;  
	int row = pCharTex->row;  
	int col = pCharTex->col;  
	pCharTex->width = BYTE(m_FT_Face->glyph->advance.x / 64);  
	pCharTex->height = BYTE(m_FT_Face->size->metrics.y_ppem);  
	pCharTex->deltaX = (slot->metrics.horiBearingX >> 6);  
	pCharTex->deltaY = (slot->bitmap_top-_texUnitSize);  

	int dstStride = TEXTURE_SIZE * 4;  
	int dstbegin = row * _texUnitSize * TEXTURE_SIZE * 4 + col * _texUnitSize * 4;  
	unsigned char* src = bitmap.buffer;  

	switch (m_FT_Face->glyph->bitmap.pixel_mode)  
	{  
	case FT_PIXEL_MODE_GRAY:  
		{  
			for(int j=0; j  < height ; j++)  
			{  
				for(int i=0; i < width; i++)  
				{  
					unsigned char _vl =  src[i + bitmap.width*j];  

					_pTexBuf[dstbegin + (4*i + j * dstStride)  ] = 0xFF;  
					_pTexBuf[dstbegin + (4*i + j * dstStride)+1] = 0xFF;  
					_pTexBuf[dstbegin + (4*i + j * dstStride)+2] = 0xFF;  
					_pTexBuf[dstbegin + (4*i + j * dstStride)+3] = _vl;  

					_pTexBuf[4 *(i + j * TEXTURE_SIZE) + 0] = 0xff;  

				}  
			}  
		}  
		break;  
	case FT_PIXEL_MODE_MONO:  
		{     
			for(int i=0; i  < height ; i++)  
			{  
				for(int j=0; j < width; j++)  
				{  
					unsigned char _vl = 0;  
					if(src[i*bitmap.pitch + j/8] & (0x80 >> (j & 7)))  
						_vl = 0xFF;  
					else  
						_vl = 0x00;  

					_pTexBuf[dstbegin + (4*j + i * dstStride)  ] = 0xFF;  
					_pTexBuf[dstbegin + (4*j + i * dstStride)+1] = 0xFF;  
					_pTexBuf[dstbegin + (4*j + i * dstStride)+2] = 0xFF;  
					_pTexBuf[dstbegin + (4*j + i * dstStride)+3] = _vl;  

				}  
			}  
		}  
		break;  
	}  


	D3DLOCKED_RECT desc;  
	if (_pTex->LockRect(0, &desc, NULL, 0)!=D3D_OK)  
		return false;  
	for (int y = 0; y < TEXTURE_SIZE; ++y)  
	{  
		for (int x = 0; x < TEXTURE_SIZE; ++x)  
		{  
			byte* source_pixel = _pTexBuf + (TEXTURE_SIZE * 4 * y) + (x * 4);  
			byte* destination_pixel = ((byte*) desc.pBits) + desc.Pitch * y + x * 4;  

			destination_pixel[0] = source_pixel[2];  
			destination_pixel[1] = source_pixel[1];  
			destination_pixel[2] = source_pixel[0];  
			destination_pixel[3] = source_pixel[3];  

		}  
	}  
	_pTex->UnlockRect(0);  

	return true;  
}  

void CFreeTypeFont::render(std::vector<FontVertex>& fontVecs)  
{  
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);  

	m_pDevice->SetTextureStageState( 0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1 );  
	//m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);   

	m_pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);  
	m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);  
	m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);  

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);  

	m_pDevice->SetFVF(VERTEX_FVF);     

	m_pDevice->SetTexture(0,_pTex);  
	int count = fontVecs.size() / 3;  
	if (count > 0)  
	{  
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count, &fontVecs[0], sizeof(FontVertex));  
	}  
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);  
}  

void CFreeTypeFont::DrawSimpleText_MiaoBian( const std::string& str, const int& size, int x ,int y, const DWORD& color, const DWORD& color2,bool bOutLine /*= false*/ )
{
	DrawSimpleTextByXY(str,size,x-1,y-1,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x-0,y-1,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x+1,y-1,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x-1,y-0,color2,bOutLine);

	DrawSimpleTextByXY(str,size,x+1,y-0,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x-1,y+1,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x-0,y+1,color2,bOutLine);
	DrawSimpleTextByXY(str,size,x+1,y+1,color2,bOutLine);

	DrawSimpleTextByXY(str,size,x,y,color,bOutLine);

}
