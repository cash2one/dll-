// InternetGetFile.h: interface for the CInternetGetFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERNETGETFILE_H__CAB80AD3_633C_4426_8240_D132AC330B3A__INCLUDED_)
#define AFX_INTERNETGETFILE_H__CAB80AD3_633C_4426_8240_D132AC330B3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Wininet.h>


typedef VOID (CALLBACK * GETFILECALBACKPROC)(DWORD dwBytesCount, DWORD dwBytesRead);

class CInternetGetFile  
{
public:
	CInternetGetFile();
	virtual ~CInternetGetFile();

	CFile hFile;
	HINTERNET hInet;
	HINTERNET hConnect;
	HINTERNET hHttpFile;
	HINTERNET hInetUrl;

	int GetFile(CString Host, CString Url, CString FileName, GETFILECALBACKPROC proc = NULL);

	int GetText(CString Url, CString &Content);

	void CloseGet();

	CString MakeRequestHeaders(CString &strBoundary);

	CString MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID, CString &strUserID , CString &strSever);

	//CString MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID);

	CString MakePostFileData(CString &strBoundary);

	int SendTrack(CString defServerName, CString defObjectName, CString filePath, CString strUserID, CString strSever, CString &strUpdMessage, GETFILECALBACKPROC procSend = NULL);

};

#endif // !defined(AFX_INTERNETGETFILE_H__CAB80AD3_633C_4426_8240_D132AC330B3A__INCLUDED_)
