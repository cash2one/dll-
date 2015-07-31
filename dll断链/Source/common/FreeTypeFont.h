#pragma once   


#include <windows.h>   
/*
#include <freetype/freetype.h>   
#include <freetype/ftglyph.h>   
#include <freetype/fttypes.h>   */


/*

#include "\zjlufo\Include\freetype-2.4.11\include\freetype\freetype.h"
#include "\zjlufo\Include\freetype-2.4.11\include\freetype\ftglyph.h"
#include "\zjlufo\Include\freetype-2.4.11\include\freetype\fttypes.h"
*/
/*
#include "\zjlufo\Include\freetype-2.4.11\include\ft2build.h"
#include "\zjlufo\Include\freetype-2.4.11\include\freetype\freetype.h"*/
#include <freetype-2.4.11\include\ft2build.h>
#include <freetype-2.4.11\include\freetype\freetype.h>

#include <map>   
#include <vector>   
#include <Strsafe.h>
#include <d3d9.h>

#ifdef DEBUG
#pragma comment(lib,"freetype2411MT_D.lib")
#else
#pragma comment(lib,"freetype2411MT.lib")
#endif


const int TEXTURE_SIZE = 500;  

class CFreeTypeFont  
{  
public:  
	// 顶点数据结构   
	struct FontVertex  
	{  
		FontVertex()  
			:x(0), y(0), z(0), rhw(0), color(0), u(0), v(0)  
		{  
		}  
		FontVertex(float _x, float _y, float _z, float _rhw, DWORD _color, float _u, float _v)  
			: x(_x), y(_y), z(_z), rhw(_rhw), color(_color), u(_u), v(_v)  
		{  
		}  
		float x, y, z, rhw;  
		DWORD       color;    
		float u, v;  
	};  
#define VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)   

	typedef struct _CHAR_TEX  
	{  
		unsigned long c;  
		bool used;   
		long useCount;  
		long row;  //texture row   
		long col;   //texture col   
		long width;//char width   
		long height;//char height   
		long deltaX;  
		long deltaY;  
	}CHAR_TEX, *LPCHAR_TEX;  

public:  
	CFreeTypeFont(IDirect3DDevice9* pDevice,char* szPathTTF,unsigned int FontSize);  
	~CFreeTypeFont(void);  

	void DrawSimpleText(const std::string& str, const int& size, const RECT& rect, const DWORD& color, bool bOutLine = false);  
	void DrawSimpleTextByXY(const std::string& str, const int& size, int x ,int y, const DWORD& color, bool bOutLine = false);  
	void DrawSimpleText_MiaoBian(const std::string& str, const int& size, int x ,int y, const DWORD& color, const DWORD& color2,bool bOutLine = false);  
private:  
	void init(const std::string& fontFile, const int& fontSize);  
	bool copyCharToTexture(LPCHAR_TEX pCharTex);  
	void render(std::vector<FontVertex>& fontVecs);  
protected:  
	LPDIRECT3DDEVICE9 m_pDevice;  
	FT_Library          m_FT2Lib;  
	FT_Face             m_FT_Face;  
private:  
	LPDIRECT3DTEXTURE9 _pTex;  
	BYTE* _pTexBuf;  

	bool _bOutLine;  

	//纹理采用，先进先出的淘汰原则   
	long _texUnitSize;  //单元纹理的大小   
	long _texUnitLen;   //单元纹理的个数，横坐标或者纵坐标   
	long _texUnitIndex;//单元纹理使用的索引   

	LPCHAR_TEX* _pTexIndices;   //纹理对应字符索引   
	LPCHAR_TEX* _pCharIndices;  //字符对应纹理索引   

	bool _isInit;  
};  
