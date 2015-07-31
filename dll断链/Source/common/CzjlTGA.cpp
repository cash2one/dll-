#include "StdAfx.h"
#include "CzjlTGA.h"




bool CzjlTGA::OnOpenDocument( wchar_t* lpszFileName )
{
	if (!LoadTgaFile(lpszFileName))
	{
		return FALSE;
	}
	return 1;
}

bool CzjlTGA::LoadTgaFile( wchar_t* lpszFileName )
{
	HANDLE hFile=CreateFileW(lpszFileName,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if (hFile==INVALID_HANDLE_VALUE)
	return 0;

	DWORD ReadCount;

	//CFile file(lpszFileName, CFile::modeRead);
	BOOL bResult = FALSE;
	////////////header
	BYTE tgaif[0x12];
	ReadFile(hFile,tgaif,sizeof(tgaif),&ReadCount,0);

	//file.Read(tgaif, sizeof(tgaif));
	BYTE bHasPal = tgaif[1];//���޵�ɫ��
	BYTE type = tgaif[2];//ͼƬ����
	WORD palEntries = *(WORD*)(tgaif + 0x05);//��ɫ����ɫ����
	WORD tgWidth = *(WORD*)(tgaif + 0x0c);//TGA��
	WORD tgHeight = *(WORD*)(tgaif + 0x0e);//TGA��
	BYTE bitCount = *(BYTE*)(tgaif + 0x10);//TGA bit out
	int bmWidth = tgWidth % 4 == 0 ? tgWidth : tgWidth + 4 - (tgWidth % 4);//ת����4�ֽڶ����BMP��
	DWORD bmBitsSize  = bmWidth * tgHeight * (bitCount / 8);//BMPͼ�����ߴ�
	DWORD bmInfoSize  = sizeof(BITMAPINFOHEADER) + palEntries * sizeof(RGBQUAD);//BMP��Ϣ�ߴ�
	DWORD bmSize = sizeof(BITMAPFILEHEADER) + bmInfoSize + bmBitsSize;//BMP�ļ��ߴ�
	BITMAPFILEHEADER bmfh = {0x4d42, bmSize, 0, 0, sizeof(BITMAPFILEHEADER) + bmInfoSize};//BMP�ļ�ͷ
	BITMAPINFOHEADER bmih = {0x28, tgWidth, - tgHeight, 1, bitCount, 0, 0, 0, 0, palEntries, 0};//BMP��Ϣͷ
	///////////////////////////
	
	
	int tgSize = GetFileSize(hFile,0);
	BYTE* pTgBuf = new BYTE[tgSize];//pTGA
	SetFilePointer(hFile,0,0,FILE_BEGIN);
	ReadFile(hFile,pTgBuf,tgSize,&ReadCount,0);
	CloseHandle(hFile);

	if (ReadCount!=tgSize)
	return 0;

	m_BmpSize=bmSize;
	m_pBmData = new BYTE[bmSize];	//pBMP
	memset(m_pBmData, 0, bmSize);
	memcpy(m_pBmData, &bmfh, sizeof(bmfh));//bitmap file header
	memcpy(m_pBmData + sizeof(bmfh), &bmih, sizeof(bmih));//bitmap info header
	BYTE* pTg = pTgBuf + 0x12 + *pTgBuf;
	BYTE* pBm = m_pBmData + sizeof(bmfh) + sizeof(bmih);
	if ((type == 1 || type == 9) && bHasPal == 1)
	{
		//////////palette
		for(int i = 0; i < palEntries; i ++)
		{
			BYTE* p  = pTg + i * 3;
			RGBQUAD rgb = {*p, *(p + 1), *(p + 2), 0};
			*(RGBQUAD*)(pBm + i * 4) = rgb;
		}
		/////////bits
		pTg += palEntries * 3;//TGA���ݿ�ʼλ��
		pBm += palEntries * sizeof(RGBQUAD);//BMP���ݿ�ʼλ��
		if (type == 1)
		{
			AlignDOWD(pBm, pTg, tgWidth, tgHeight, bitCount / 8);//����DWORD
		}
		else if (type == 9)
		{
			//decompress bits
			BYTE* pTempBits = new BYTE[bmBitsSize];
			BYTE* pTemp = pTempBits;
			while (pTg < pTgBuf + tgSize)
			{
				BYTE flags = *pTg;//��־
				int size = (flags & 0x7F) + 1;//����
				pTg ++;
				if (flags > 0x7F)
				{
					memset(pTemp, *pTg, size);//�ظ�size��
					pTg ++;
				}
				else
				{
					memcpy(pTemp, pTg, size);//����size��ԭ��
					pTg += size;
				}
				pTemp += size;//Ŀ��ָ��
			}
			AlignDOWD(pBm, pTempBits, tgWidth, tgHeight, bitCount / 8);//����DWORD
			delete pTempBits;
		}
		bResult = TRUE;
	}
	else if ((type == 2) && bHasPal == 0)
	{
		AlignDOWD(pBm, pTg, tgWidth, tgHeight, bitCount / 8);//����DWORD
		bResult = TRUE;
	}
	delete pTgBuf;

	return bResult;
}

void CzjlTGA::AlignDOWD( BYTE *pDest, BYTE *pSrc, int nSrcWidth, int nSrcHeight, int nByte )
{
	if (nSrcWidth % 4 == 0)
	{
		memcpy(pDest, pSrc, nSrcWidth * nSrcHeight * nByte);
	}
	else
	{
		int nDestWidth = nSrcWidth + 4 - (nSrcWidth % 4);
		for (int rows = 0; rows < nSrcHeight; rows ++)
		{
			memcpy(pDest + rows * nDestWidth * nByte, pSrc + rows * nSrcWidth * nByte, nSrcWidth * nByte);
		}
	}
}

SIZE CzjlTGA::GetPicSize()
{
	BITMAPINFOHEADER* p = (BITMAPINFOHEADER*)(m_pBmData + sizeof(BITMAPFILEHEADER));
	int cy = p->biHeight < 0 ? -p->biHeight : p->biHeight;
	SIZE sz = {p->biWidth, cy};
	return sz;
}

BYTE* CzjlTGA::GetBMP()
{
	return m_pBmData;
//	BYTE* lpbInit = m_pBmData + ((BITMAPFILEHEADER*)m_pBmData)->bfOffBits;
}
