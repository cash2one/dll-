#include "CzjlInternet.h"
#include <wininet.h>
#include <shlobj.h>
#pragma comment(lib,"Wininet.lib")

static HINTERNET g_hNet=0;
#define SIZEDOWNBUFFER		1024*10											//下载缓冲区大小



CzjlInternet::CzjlInternet(void)
{
    g_hNet = InternetOpenA("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)", INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
}


CzjlInternet::~CzjlInternet(void)
{
	InternetCloseHandle(g_hNet);
}

unsigned int CzjlInternet::LoadFile( CString strUrl,CString strSavePath )
{
	DeleteFileW(strSavePath);
    unsigned int ReadSize=0;//已读
    char* pBuffer=0;
    if (!g_hNet)
    {
        g_hNet = InternetOpenA("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)", INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
        if(g_hNet == NULL)
            return 0;
    }


    HINTERNET hNetFile = InternetOpenUrl(g_hNet, strUrl, NULL, 0, 0, 0); 
    if(hNetFile == NULL)
    {
        return 0;
    }

    //获得下载大小
    DWORD dwContentLen;
    DWORD dwBufLen = sizeof(dwContentLen);
    if (HttpQueryInfo(hNetFile, 
        HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
        (LPVOID)&dwContentLen,
        &dwBufLen,
        0))
    {
        pBuffer=0;
        pBuffer=new char[dwContentLen];

        unsigned long nSize = -1;
        while(nSize &&
            InternetReadFile(hNetFile,(LPVOID)(&(pBuffer[ReadSize])), SIZEDOWNBUFFER, &nSize))
        {
            ReadSize+=nSize;
        }
    }
    InternetCloseHandle(hNetFile);
    if (ReadSize!=dwContentLen)
    {
        delete[] pBuffer;
        return 0;
    }

    //本地储存
    CString strPath=strSavePath.Mid(0,strSavePath.ReverseFind('\\'));
    SHCreateDirectoryExW(0,strPath,0);

    HANDLE hFile = CreateFileW(strSavePath,                // name of the write
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,          // overwrite existing
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template


    if (INVALID_HANDLE_VALUE == hFile)
    {
        if (pBuffer)
        {
            delete[] pBuffer;
            return 0;
        }
    }

    DWORD readsize;
    WriteFile(hFile,pBuffer,dwContentLen,&readsize,0);

    CloseHandle(hFile);

    if (pBuffer)
        delete[] pBuffer;
    if (readsize == dwContentLen)
    {
        return readsize;
    }else
    {
        return 0;
    }

}

unsigned int CzjlInternet::LoadFile( CString strUrl,CString strSavePath,unsigned int BufferSize )
{

    if (!g_hNet)
    {
        g_hNet = InternetOpenA("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)", INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
        if(g_hNet == NULL)
            return 0;
    }

    HINTERNET hNetFile = InternetOpenUrl(g_hNet, strUrl, NULL, 0, 0, 0); 
    if(hNetFile == NULL)
    {
        return 0;
    }


    //read
    unsigned int ReadSize=0;//已读
    char* _File=new char[BufferSize];
    if (!_File)
    {
            InternetCloseHandle(hNetFile);
        return 0;
    }

    unsigned long nSize = -1;
    while(nSize &&
        InternetReadFile(hNetFile,(LPVOID)(&(_File[ReadSize])), 1024, &nSize))
    {
        ReadSize+=nSize;
    }
    InternetCloseHandle(hNetFile);

                DWORD Filereadsize;
    if (ReadSize)
    {//有内容
        //本地储存

        HANDLE hFile = CreateFileW(strSavePath,                // name of the write
            GENERIC_WRITE,          // open for writing
            0,                      // do not share
            NULL,                   // default security
            CREATE_ALWAYS,          // overwrite existing
            FILE_ATTRIBUTE_NORMAL,  // normal file
            NULL);                  // no attr. template


        if (INVALID_HANDLE_VALUE != hFile)
        {
            WriteFile(hFile,_File,ReadSize,&Filereadsize,0);

            CloseHandle(hFile);
        }

    }

    delete[] _File;
    return Filereadsize;
}



unsigned int CzjlInternet::LoadHtml( CStringW strUrl,CStringW& sBuffer )
{
    unsigned int ReadResult=0;



    unsigned int ReadSize=0;//已读
    char* pBuffer=0;
    if (!g_hNet)
    {
		//Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)
		//Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)
        g_hNet = InternetOpenA("Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)", 0,NULL, NULL, 0);
        if(g_hNet == NULL)
            return 0;
    }

    HINTERNET hNetFile = InternetOpenUrlW(g_hNet, strUrl.GetBuffer(), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_NO_COOKIES, 0); 
	//DWORD d=GetLastError();

    if(hNetFile == NULL)
    {
        return 0;
    }
    //read
	char _Buffer[SIZEDOWNBUFFER+1]={0};
    while(true)
    {
        unsigned long nSize = 0;
        BOOL bRet = InternetReadFile(hNetFile, _Buffer, SIZEDOWNBUFFER, &nSize);
        if(!bRet || nSize == 0)
            break;
        ReadResult+=nSize;
        _Buffer[nSize] = '\0';
        sBuffer += _Buffer;
    }
    InternetCloseHandle(hNetFile);
	sBuffer.ReleaseBuffer();
    return ReadResult;
}

unsigned int CzjlInternet::LoadHtml( CStringA szUrl,CStringA& sBuffer )
{
	CStringW strUrl=szUrl;
    CStringW strTemp;
    unsigned int result=LoadHtml(strUrl,strTemp);
    sBuffer=strTemp;
    return result;
}

unsigned int CzjlInternet::QueryFileSize( CString strUrl )
{
    unsigned int ReadSize=0;//已读
    char* pBuffer=0;
    if (!g_hNet)
    {
        g_hNet = InternetOpenA("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)", INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
        if(g_hNet == NULL)
            return 0;
    }


    HINTERNET hNetFile = InternetOpenUrl(g_hNet, strUrl, NULL, 0, 0, 0); 
    if(hNetFile == NULL)
    {
        return 0;
    }
    //获得下载大小
    DWORD dwContentLen;
    DWORD dwBufLen = sizeof(dwContentLen);
    if (HttpQueryInfo(hNetFile, 
        HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
        (LPVOID)&dwContentLen,
        &dwBufLen,
        0))
    {
        return dwContentLen;
    }else{
        return 0;
    }
}

bool CzjlInternet::Start()
{
	if (g_hNet==0)
	{
		g_hNet = InternetOpen(0, 0,NULL, NULL, 0);
		if(g_hNet == NULL)
			return 0;
	}else{
	return 1;
	}
}

unsigned int CzjlInternet::PostHttps( CStringW strUrl )
{
	return 1;
}

unsigned int CzjlInternet::LoadHtmlLen( CStringA strUrl,CStringA& sBuffer,unsigned int ReadByte )
{
	unsigned int ReadResult=0;



	unsigned int ReadSize=0;//已读
	char* pBuffer=0;
	if (!g_hNet)
	{
		g_hNet = InternetOpenA("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)", INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
		if(g_hNet == NULL)
			return 0;
	}

	HINTERNET hNetFile = InternetOpenUrlA(g_hNet, strUrl.GetBuffer(), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0); 
	DWORD d=GetLastError();

	if(hNetFile == NULL)
	{
		return 0;
	}
	//read
	char* _Buffer=new char[ReadByte+1];

		unsigned long nSize = 0;
		BOOL bRet = InternetReadFile(hNetFile, _Buffer, ReadByte, &nSize);
		ReadResult=nSize;

		_Buffer[ReadByte] = '\0';
		sBuffer += _Buffer;

	InternetCloseHandle(hNetFile);
	return ReadResult;
}
