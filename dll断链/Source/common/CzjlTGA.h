#pragma once
#include <windows.h>



class CzjlTGA
{
public:
	CzjlTGA(void)
	{
		m_pBmData = NULL;
	}
	~CzjlTGA(void)
	{
		if (m_pBmData)
			delete m_pBmData;
	}

	bool OnOpenDocument(wchar_t* lpszFileName);
	bool LoadTgaFile(wchar_t* lpszFileName);
	void AlignDOWD(BYTE *pDest, BYTE *pSrc, int nSrcWidth, int nSrcHeight, int nByte);
	SIZE GetPicSize();
	BYTE* GetBMP();
BYTE* m_pBmData;
unsigned int m_BmpSize;
};

