// InternetGetFile.cpp: implementation of the CInternetGetFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Wininet.h>
#include "InternetGetFile.h"
#include <afx.h>

#pragma comment(lib, "Wininet.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInternetGetFile::CInternetGetFile()
{

}

CInternetGetFile::~CInternetGetFile()
{

}

int CInternetGetFile::GetFile(CString Host, CString Url, CString FileName, GETFILECALBACKPROC proc)
{
try{
	BYTE buf[8192];
	DWORD nBytesRead;
	int nRetCount = 0;

	CString strTempStr;
	CTime t = CTime::GetCurrentTime();
	strTempStr.Format(_T("%u"), t.GetTime());
	Url.MakeLower();
	if(Url.Find(_T("?"))>-1)
		Url += _T("&hsad73=") + strTempStr;
	else
		Url += _T("?hsad73=") + strTempStr;

	Url.Replace(_T("\\"), _T("/"));

	//HINTERNET hInet=InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	//if( !hInet )
	//	return -1;

//	HINTERNET hInet;
	//DWORD dwFlags = 1;
	//InternetGetConnectedState(&dwFlags, 0);
	//if(!(dwFlags & INTERNET_CONNECTION_PROXY))
	//	hInet = InternetOpen(_T("UUU9_LiveUpdate"), INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	//else
		hInet = InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if( !hInet )
		return -1;

	hConnect=::InternetConnect(hInet, Host, INTERNET_INVALID_PORT_NUMBER, _T(""), _T(""), INTERNET_SERVICE_HTTP, 0, 0);
	if(!hConnect)
		return -1;

	const TCHAR *szAcceptTypes[] = {_T("*/*"), NULL};
	hHttpFile=::HttpOpenRequest(hConnect, _T("GET"),
		Url,
		HTTP_VERSION, NULL, szAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_HYPERLINK, NULL);
	if(!hHttpFile)
		return -1;

	//if(!::HttpSendRequest(hHttpFile,_T("Accept-Language: zh-CN\r\nAccept-Encoding: gzip, deflate"),-1,0,0))
	//if(!::HttpSendRequest(hHttpFile,_T("Accept-Language: zh-CN"),-1,0,0))
	if(!::HttpSendRequest(hHttpFile,NULL,0,0,0))
		return -1;

	DWORD dwContentLen = 0;
	DWORD dwLengthBufQuery=sizeof(dwLengthBufQuery);
	if(!::HttpQueryInfo(hHttpFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwContentLen, &dwLengthBufQuery, 0))
	{
		dwContentLen = 0;
	}

	hFile.Open(FileName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite);

	BOOL bIsSuccess;
	while (TRUE)
	{
		bIsSuccess = InternetReadFile(hHttpFile, buf, 8192, &nBytesRead);
		if(bIsSuccess && nBytesRead == 0)
		{
			//下载成功
			break;
		}
		else if(!bIsSuccess)
		{
			//下载未完成，失败
			g_GlobalVar.bCFileExitFlag = FALSE;
			hFile.Close();
			DeleteFile(FileName);
			InternetCloseHandle(hHttpFile);
			InternetCloseHandle(hConnect);			
			InternetCloseHandle(hInet);

			return -1;
		}


		nRetCount += nBytesRead;

		hFile.Write(buf, nBytesRead);

		if(proc && dwContentLen>0)
			proc(dwContentLen, nRetCount);
	}

	g_GlobalVar.bCFileExitFlag = FALSE;
	hFile.Close();	
	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hConnect);			
	InternetCloseHandle(hInet);

	return nRetCount;
}catch(...){
	if(hFile!=NULL)	hFile.Close();
	if(hHttpFile!=NULL) InternetCloseHandle(hHttpFile);
	if(hConnect!=NULL) InternetCloseHandle(hConnect);			
	if(hInet!=NULL) InternetCloseHandle(hInet);
	return -1;}
}

void CInternetGetFile::CloseGet()
{
	try
	{
		g_GlobalVar.bCFileExitFlag = FALSE;
		if(hFile!=NULL)	hFile.Close();
		if(hHttpFile!=NULL) InternetCloseHandle(hHttpFile);
		if(hConnect!=NULL) InternetCloseHandle(hConnect);			
		if(hInet!=NULL) InternetCloseHandle(hInet);
		//if(hInetUrl!=NULL) InternetCloseHandle(hInetUrl);
	}
	catch (...)
	{
	}
}

int CInternetGetFile::GetText(CString Url, CString &Content)
{
	BYTE buf[8194];
	DWORD nBytesRead;
	int nRetCount = 0;

	CString strTempStr;
	CTime t = CTime::GetCurrentTime();
	strTempStr.Format(_T("%u"), t.GetTime());
	Url.MakeLower();
	if(Url.Find(_T("?"))>-1)
		Url += _T("&hsad73=") + strTempStr;
	else
		Url += _T("?hsad73=") + strTempStr;

	hInet = InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 2.0.50727)"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if( !hInet )
		return -1;

	hInetUrl = InternetOpenUrl(hInet, Url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if( !hInetUrl )
		return -1;

	BOOL bIsSuccess;
	while (TRUE)
	{
		bIsSuccess = InternetReadFile(hInetUrl, buf, 8192, &nBytesRead);
		if(bIsSuccess && nBytesRead == 0)
		{
			//下载成功
			break;
		}
		else if(!bIsSuccess)
		{
			//下载未完成，失败

			InternetCloseHandle(hInetUrl);
			InternetCloseHandle(hInet);

			return -1;
		}

		nRetCount += nBytesRead;

		buf[nBytesRead] = 0;
		buf[nBytesRead+1] = 0;
		Content += CString((wchar_t*)buf);		
	}

	InternetCloseHandle(hInetUrl);
	InternetCloseHandle(hInet);
		
	return nRetCount;
}

CString CInternetGetFile::MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID, CString &strUserID , CString &strSever)	//, CString &strPwd
{
    /**/////////////////////////////////////////////////////////////////////////////////
    //Content-Type:
    //JPG image/pjpeg
    //PNG image/x-png
    //BMP image/bmp
    //TIF image/tiff
    //GIF image/gif
    CString strFormat;
    CString strData;
	
	char *szUtf8 = UnicodeToUTF8(strUserID);
	CString strUserID2 = szUtf8;

//  	for(int i=0; i<strlen(szUtf8); i++)
//  	{
//  		CString strTemp;
//  		strTemp.Format(_T("%2X"), (BYTE)szUtf8[i]);
//  		strUserID2 += _T("%")+strTemp;
// 	}
	free(szUtf8);
	strFormat += _T("--%s");
    strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"userid\"");//除了文件数据，还可以传送其它信息
    strFormat += _T("\r\n\r\n");
	strFormat += strUserID;
    strFormat += _T("\r\n");
	
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"lolserver\"");
	strFormat += _T("\r\n\r\n");
 	strFormat += strSever;
	strFormat += _T("\r\n");

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"logined\"");
	strFormat += _T("\r\n\r\n");
	strFormat += g_GlobalVar.bUserLogined?_T("1"):_T("0");
	strFormat += _T("\r\n");

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"host\"");
	strFormat += _T("\r\n\r\n");
	strFormat += g_GlobalVar.strHost;
	strFormat += _T("\r\n");
	
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"clientver\"");
	strFormat += _T("\r\n\r\n");
	strFormat += g_GlobalVar.strGameAppVer;
	strFormat += _T("\r\n");
	
    strFormat += _T("--%s");
    strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"upfile\"; filename=\"%s\"");
	//文件地址信息，此处的upfile需与PHP服务器端一致
    strFormat += _T("\r\n");
    //strFormat += _T("Content-Type: image/gif");
    strFormat += _T("Content-Type: application/octet-stream");
    strFormat += _T("\r\n\r\n");
    strData.Format(strFormat,  strBoundary, strBoundary, strBoundary, strBoundary, strBoundary, strBoundary, strFileName);//

// 	char * szInfo = UnicodeToUTF8(strFormat);
// 	CFile f;
// 	if(f.Open(_T("E:\\data.txt"), CFile::modeCreate|CFile::modeWrite))
// 	{
// 		f.Write(szInfo, strlen(szInfo));
// 		f.Close();
// 	}
// 	free(szInfo);
    return strData;
}

CString CInternetGetFile::MakePostFileData(CString &strBoundary)//发送请求包
{
    CString strFormat;
    CString strData;
	
	strFormat = _T("\r\n");	
    strFormat += _T("--%s");
    strFormat += _T("\r\n");
    strFormat += _T("Content-Disposition: form-data; name=\"upload\"");
    strFormat += _T("\r\n\r\n");
    strFormat += _T("hello");
    strFormat += _T("\r\n");
    strFormat += _T("--%s--");
    strFormat += _T("\r\n");
	
    strData.Format(strFormat, strBoundary, strBoundary);
	
    return strData;
}

CString CInternetGetFile::MakeRequestHeaders(CString &strBoundary)//包头
{
    CString strFormat;
    CString strData;
    strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");//二进制文件传送Content-Type类型为: multipart/form-data
    strData.Format(strFormat, strBoundary);
    return strData;
}

int CInternetGetFile::SendTrack(CString defServerName, CString defObjectName, CString filePath, CString strUserID, CString strSever, CString &strUpdMessage, GETFILECALBACKPROC procSend)
{
	//CString defServerName ="war3h.uuu9.com";//服务器地址。
	//CString defObjectName ="/uploadusercfg.aspx";//保存的地址，服务器端处理文件相对路径
	//CString defServerName ="127.0.0.1";//服务器地址。
	//CString defObjectName ="/WSHUser/uploadusercfg.aspx";//保存的地址，服务器端处理文件相对路径
	// USES_CONVERSION;

//	AfxMessageBox(_T("上传开始Sever:") + strSever);
	CInternetSession Session;
	CHttpConnection *pHttpConnection = NULL;
	INTERNET_PORT   nPort = 80;
	CFile fTrack;
	CHttpFile* pHTTP = NULL;
	CString strHTTPBoundary;
	CString strPreFileData;
	CString strPostFileData;
	DWORD dwTotalRequestLength;
	DWORD dwChunkLength;
	DWORD dwReadLength;
	DWORD dwResponseLength;
	TCHAR szError[MAX_PATH];
	void* pBuffer;
	LPSTR szResponse;
	CString strResponse;
	int iStatus = 1;
	DWORD dwStatusCode;
	DWORD nCurrFileSize = 0, nFileSize = 0 ;
	
	if (fTrack.Open(filePath, CFile::modeRead | CFile::shareDenyWrite))
	{
		int iRecordID = 1;
		strHTTPBoundary = _T("---------------------------7b4a6d158c9");//定义边界值
		strPreFileData = MakePreFileData(strHTTPBoundary, filePath, iRecordID, strUserID, strSever);

		strPostFileData = MakePostFileData(strHTTPBoundary);
		char * szPreData = UnicodeToUTF8(strPreFileData); // 第二个字符串转换调用
		char * szPostData = UnicodeToUTF8(strPostFileData);
		nFileSize = fTrack.GetLength();
		dwTotalRequestLength = strlen(szPreData) + strlen(szPostData) + nFileSize;//计算整个包的总长度
		
		dwChunkLength = PER_SEND_ZISE;	//
		
		pBuffer = malloc(dwChunkLength);
		
		if (NULL != pBuffer)
		{
			try
			{
				Session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,5000); 
				pHttpConnection = Session.GetHttpConnection(defServerName,nPort);
				pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, defObjectName);
				pHTTP->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));//发送包头请求
				pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);
				//char * szPreData = UnicodeToAnsi(strPreFileData);
				pHTTP->Write(szPreData, strlen(szPreData));
				dwReadLength = -1;
		//		AfxMessageBox(_T("开始上传"));
				while (0 != dwReadLength)
				{
					dwReadLength = fTrack.Read(pBuffer, dwChunkLength);
					nCurrFileSize += dwReadLength;
					if(procSend)
						procSend(nFileSize, nCurrFileSize);
					if (0 != dwReadLength)
					{
						pHTTP->Write(pBuffer, dwReadLength);//写入服务器本地文件，用二进制进行传送
					}
				}
				
				pHTTP->Write(szPostData, strlen(szPostData));
				
				pHTTP->EndRequest(HSR_SYNC);
				
				dwResponseLength = pHTTP->GetLength();
				while (0 != dwResponseLength)
				{
					szResponse = (LPSTR)malloc(dwResponseLength + 1);
					szResponse[dwResponseLength] = '\0';
					pHTTP->Read(szResponse, dwResponseLength);
					strResponse += szResponse;
					free(szResponse);
					dwResponseLength = pHTTP->GetLength();
					pHTTP->QueryInfoStatusCode(dwStatusCode);
				}
		//		AfxMessageBox(_T("上传完成"));
			} 
			catch (CException* e)
			{
				e->GetErrorMessage(szError, MAX_PATH);
				e->Delete();
		//		AfxMessageBox(szError);
				iStatus = -10;
				goto rExit0;
			}
		rExit0:
			pHTTP->Close();
			if(pHTTP!=NULL)
			{
				delete pHTTP;
				pHTTP = NULL;
			}
			pHttpConnection->Close();
			delete pHttpConnection;
			Session.Close();
			fTrack.Close();
			
			if (NULL != pBuffer)
			{
		//		AfxMessageBox(_T("free(pBuffer)"));
				free(pBuffer);
				pBuffer = NULL;
			}
			DeleteFile(filePath);
		//	AfxMessageBox(_T("上传完成Response:") + strResponse);

			if(dwStatusCode==200 && iStatus==1)
			{
				//strResponse = _T("1;您获得了英雄联盟国服激活码：F43GHCB5HA7");
				int iPos = strResponse.Find(_T(";"));
				CString strUpdResult = _T("0");
				if(iPos >0)
				{
					strUpdResult = strResponse.Left(iPos+1);
					strResponse.Delete(0, iPos+1);
					wchar_t *wcResponse = (wchar_t *)strResponse.GetBuffer(strResponse.GetLength());
					char *cResponse= UnicodeToAnsi(wcResponse);
					wchar_t *wResponse = UTF8ToUnicode(cResponse);
					strResponse.ReleaseBuffer();
					strResponse = CString(wResponse);
					free(wcResponse);
					free(wResponse);
					strUpdMessage = strResponse;
				}
				else
					strUpdResult = strResponse;
				if(strUpdResult.GetLength()>0 && strUpdResult.GetLength()<3 && (strUpdResult == _T("1") || strUpdResult == _T("-1")))
					iStatus = 1;
				else if(strUpdResult.GetLength()<3)
					iStatus = _wtoi(strUpdResult);
				else
					iStatus = 0;
			}
			else if(iStatus!=-10 && dwStatusCode<=HTTP_STATUS_LAST && dwStatusCode>=HTTP_STATUS_FIRST)
				iStatus = dwStatusCode;
		}
		else
		{
			DeleteFile(filePath);
			iStatus = -9;
		}
		free(szPreData);
		free(szPostData);
	}
	else
	{
		DeleteFile(filePath);
		//AfxMessageBox(_T("上传配置失败!\r\n文件:usercfg.dat打开失败!"), MB_ICONERROR);
		iStatus = -8;
	}
	return iStatus;
}


/*
CString CInternetGetFile::MakeRequestHeaders(CString &strBoundary)
{
	CString strFormat;
	CString strData;

	strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");

	strData.Format(strFormat, strBoundary);

	return strData;
}

CString CInternetGetFile::MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID)
{
	/////////////////////////////////////////////////////////////////////////////////
	//Content-Type:
	//JPG image/pjpeg
	//PNG image/x-png
	//BMP image/bmp
	//TIF image/tiff
	//GIF image/gif
	CString strFormat;
	CString strData;

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"recordid\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("%i");
	strFormat += _T("\r\n");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"trackdata\"; filename=\"%s\"");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: image/pjpeg");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Transfer-Encoding: binary");
	strFormat += _T("\r\n\r\n");

	strData.Format(strFormat, strBoundary, iRecordID, strBoundary, strFileName);

	return strData;
}

CString CInternetGetFile::MakePostFileData(CString &strBoundary)
{
	CString strFormat;
	CString strData;

	strFormat = _T("\r\n");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("hello");
	strFormat += _T("\r\n");
	strFormat += _T("--%s--");
	strFormat += _T("\r\n");

	strData.Format(strFormat, strBoundary, strBoundary);

	return strData;
}

BOOL CInternetGetFile::SendTrack(CString _mFilePath)
{
	int startp = _mFilePath.ReverseFind('\\');
	int namelen = _mFilePath.GetLength()-startp-1;

	CString pcmname = _mFilePath.Mid(startp+1,namelen);

	CString defServerName ="127.0.0.1";
	CString defObjectName ="/upload.aspx";

	CInternetSession Session;
	CHttpConnection *pHttpConnection = NULL;
	INTERNET_PORT   nPort = 80;
	CFile fTrack;
	CHttpFile* pHTTP;
	CString strHTTPBoundary;
	CString strPreFileData;
	CString strPostFileData;
	DWORD dwTotalRequestLength;
	DWORD dwChunkLength;
	DWORD dwReadLength;
	DWORD dwResponseLength;
	TCHAR szError[MAX_PATH];
	void* pBuffer;
	LPSTR szResponse;
	CString strResponse;
	BOOL bSuccess = TRUE;

	CString strDebugMessage;

	if (FALSE == fTrack.Open(_mFilePath, CFile::modeRead | CFile::shareDenyWrite))
	{
		AfxMessageBox(_T("Unable to open the file."));
		return FALSE;
	}

	int iRecordID = 1;
	strHTTPBoundary = _T("wwwmyebecomshasureupload");
	strPreFileData = MakePreFileData(strHTTPBoundary, pcmname, iRecordID);
	strPostFileData = MakePostFileData(strHTTPBoundary);

	AfxMessageBox(strPreFileData);
	AfxMessageBox(strPostFileData);

	dwTotalRequestLength = strPreFileData.GetLength() + strPostFileData.GetLength() + fTrack.GetLength();

	dwChunkLength = 64 * 1024;

	pBuffer = malloc(dwChunkLength);

	if (NULL == pBuffer)
	{
		return FALSE;
	}

	try
	{
		pHttpConnection = Session.GetHttpConnection(defServerName,nPort);
		pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, defObjectName);
		pHTTP->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));
		pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

		char * szAnsi = UnicodeToAnsi(strPreFileData);
		pHTTP->Write(szAnsi, strPreFileData.GetLength());
		free(szAnsi);

		dwReadLength = -1;
		while (0 != dwReadLength)
		{
			strDebugMessage.Format(_T("%u / %u\n"), fTrack.GetPosition(), fTrack.GetLength());
			TRACE(strDebugMessage);
			dwReadLength = fTrack.Read(pBuffer, dwChunkLength);
			if (0 != dwReadLength)
			{
				pHTTP->Write(pBuffer, dwReadLength);
			}
		}

		szAnsi = UnicodeToAnsi(strPostFileData);
		pHTTP->Write(szAnsi, strPostFileData.GetLength());
		free(szAnsi);

		pHTTP->EndRequest(HSR_SYNC);

		dwResponseLength = pHTTP->GetLength();
		while (0 != dwResponseLength)
		{
			szResponse = (LPSTR)malloc(dwResponseLength + 1);
			szResponse[dwResponseLength] = '\0';
			pHTTP->Read(szResponse, dwResponseLength);
			strResponse += szResponse;
			free(szResponse);
			dwResponseLength = pHTTP->GetLength();
		}

		AfxMessageBox(strResponse);

	} 
	catch (CException* e)
	{
		e->GetErrorMessage(szError, MAX_PATH);
		e->Delete();
		AfxMessageBox(szError);
		bSuccess = FALSE;
	}

	pHTTP->Close();
	delete pHTTP;

	fTrack.Close();

	if (NULL != pBuffer)
	{
		free(pBuffer);
	}
	return bSuccess;
}
*/