#include "stdafx.h"

#include "HttpPost.h"

#pragma comment(lib, "WS2_32")   
using namespace std;

namespace DEVWEB
{
	void GetRealIP(string& ip, string& retip)
	{
		retip = ip;
		unsigned long t = inet_addr((char*)(LPCSTR)ip.c_str());
		if (t == INADDR_NONE)
		{
			hostent* hostInfo = gethostbyname((char*)(LPCSTR)ip.c_str());
			if (hostInfo)
			{
				struct in_addr *addr;
				addr = (struct in_addr*)hostInfo->h_addr_list[0];
				if (addr != NULL)
				{
					retip = inet_ntoa(*addr);
				}
			}
			else{
				printf("GetRealIP can't parse domain %s\n", ip.c_str());
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//szURL 一个完整的URL仅HTTP，HTTPS不支持，可以包含GET参数，如index.php?username=test
	//response 为请求返回的结果，包含HTTP头信息
	//pRP为附加参数，完成更复杂的请求
	///////////////////////////////////////////////////////////////////////////
	bool WebRequest(const char* szURL, std::string& response, RequestParam* pRP)
	{
		if (!szURL)
			return false;
		WSADATA wsaData;
		int vMMVersion = MAKEWORD(1, 1);

		int res = WSAStartup(vMMVersion, &wsaData);
		SOCKET hSkt = socket(AF_INET, SOCK_STREAM, res);
		if (INVALID_SOCKET == hSkt){
			OutputDebugStringA("WebRequest socket create failed!\n");
			return false;
		}
		ACloseSkt askt(hSkt);
		//是否设置为异步
		if (pRP && pRP->bBlock == false)
		{
			ULONG nMode = 1;
			ioctlsocket(hSkt, FIONBIO, &nMode);
		}
		string strURL(szURL), host, ip, requrl;
		unsigned int nport = 80;
		if (_stricmp(string(strURL.substr(0, 7)).c_str(), "http://"))
		{
			OutputDebugStringA("WebRequest parse url error, need http://\n");
			return false;
		}
		else{  //parse url;
			size_t nMH = strURL.find(':', 8);
			size_t nPre = strURL.find('/', 8);
			if (nMH == -1 && nPre == -1)
			{
				host = strURL.substr(7);
				requrl = "/";
			}
			else if (nPre != -1){
				if (nMH != -1 && nPre > nMH){
					host = strURL.substr(7, nMH - 7);
					nport = atoi(string(strURL.substr(nMH + 1, nPre - 1 - nMH)).c_str());
				}
				else{
					host = strURL.substr(7, nPre - 7);
				}
				requrl = strURL.substr(nPre);
			}
			else if (nMH != -1){
				host = strURL.substr(7, nMH - 7);
				nport = atoi(string(strURL.substr(nMH + 1)).c_str());
				requrl = "/";
			}
		}
		GetRealIP(host, ip);
		sockaddr_in addr;
		addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		addr.sin_port = htons(nport);
		addr.sin_family = AF_INET;
		if (pRP && pRP->bBlock)
		{
			if (SOCKET_ERROR == connect(hSkt, (sockaddr*)&addr, sizeof(addr)))
			{
				OutputDebugStringA("WebRequest connect server failed!\n");
				return false;
			}
		}
		else{
			if (SOCKET_ERROR == connect(hSkt, (sockaddr*)&addr, sizeof(addr)))
			{
				if (WSAGetLastError() == 10035)
				{
					//connectioning
				}
				else{
					OutputDebugStringA("WebRequest connect server failed!\n");
					return false;
				}
			}
			DWORD dwTick = GetTickCount();
			do{
				fd_set    Set, WSet;
				FD_ZERO(&Set);
				FD_ZERO(&WSet);
				FD_SET(hSkt, &Set);
				FD_SET(hSkt, &WSet);
				timeval time;
				time.tv_sec = 0;
				time.tv_usec = 0;
				int nS = select(0, &Set, &WSet, NULL, &time);
				if (nS == SOCKET_ERROR)
				{
					OutputDebugStringA("WebRequest connect server failed!(SELECT)\n");
					return false;
				}
				else if (nS){
					break;//connect sucess.
				}
				else{
					if ((GetTickCount() - dwTick) > pRP->uiTimeout)
					{
						//timeout
						OutputDebugStringA("WebRequest connect server timeout!(SELECT)\n");
						return false;
					}
				}
			} while (true);
		}
		//fill header;
		string header;
		bool bSendBin = false;
		if (pRP)
		{
			header.append(pRP->bPost ? "POST " : "GET ");
			header.append(requrl);
			header.append(" HTTP/1.1\r\n");
			header.append("host:");
			header.append(host);
			header.append("\r\nUser-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
			header.append("Content-Type:application/x-www-form-urlencoded\r\n");
			header.append("Accept:text/html,application/xhtml+xml,*/*\r\nConnection:close\r\n");
			char szCSize[50];
			sprintf(szCSize, "Content-Length:%d\r\n\r\n", pRP->uiPostLen);
			header.append(szCSize);
			if (pRP->pPostData){
				if (strlen(pRP->pPostData) <= pRP->uiPostLen){
					bSendBin = true;
				}
				else{
					header.append(pRP->pPostData);
				}
			}
		}
		else{
			header.append("GET ");
			header.append(requrl);
			header.append(" HTTP/1.1\r\n");
			header.append("host:");
			header.append(host);
			header.append("\r\nUser-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
			header.append("Content-Type:application/x-www-form-urlencoded\r\n");
			header.append("Accept:text/html,application/xhtml+xml,*/*\r\nConnection:close\r\n\r\n");
		}
		size_t headerlen = header.size();
		size_t nSendLen = 0;
		const char* pdata = header.c_str();
		DWORD dwTick = GetTickCount();
		do
		{
			int n = send(hSkt, pdata + nSendLen, int(headerlen - nSendLen), 0);
			if (n == SOCKET_ERROR){
				if (10035 == WSAGetLastError())
				{
					//wait for send.
					if (pRP && GetTickCount() - dwTick >= pRP->uiTimeout)
					{
						OutputDebugStringA("WebRequest send failed!\n");
						return false;
					}
					Sleep(10);
				}
				else{
					OutputDebugStringA("WebRequest send failed!\n");
					return false;
				}
			}
			else if (n == 0)
			{
				OutputDebugStringA("WebRequest send failed!\n");
				return false;
				break;
			}
			else{
				dwTick = GetTickCount();
				nSendLen += n;
			}
		} while (nSendLen < headerlen);
		if (bSendBin && pRP && pRP->pPostData && pRP->uiPostLen)
		{
			nSendLen = 0;
			pdata = (const char*)pRP->pPostData;
			dwTick = GetTickCount();
			do
			{
				int n = send(hSkt, pdata + nSendLen, pRP->uiPostLen - nSendLen, 0);
				if (n == SOCKET_ERROR){
					if (10035 == WSAGetLastError())
					{
						//wait for send.
						if (pRP && GetTickCount() - dwTick >= pRP->uiTimeout)
						{
							OutputDebugStringA("WebRequest send timeout!\n");
							return false;
						}
						Sleep(10);
					}
					else{
						OutputDebugStringA("WebRequest send failed!\n");
						return false;
					}
				}
				else if (n == 0)
				{
					OutputDebugStringA("WebRequest send failed!\n");
					return false;
					break;
				}
				else{
					dwTick = GetTickCount();
					nSendLen += n;
				}
			} while (nSendLen < pRP->uiPostLen);
		}
		//recv response
		char    buf[2049];
		string& request = response;
		request.clear();
		dwTick = GetTickCount();
		do
		{
			int n = recv(hSkt, buf, 2048, 0);
			if (n == SOCKET_ERROR)
			{
				if (10035 != WSAGetLastError())
				{
					OutputDebugStringA("DevWebService recv failed!\n");
					return 0;
					break;
				}
				else{
					if (pRP && GetTickCount() - dwTick >= pRP->uiTimeout){
						OutputDebugStringA("WebRequest recv timeout!\n");
						return false;
					}
					Sleep(10);
				}
			}
			else if (n == 0){
				//host close recv finish
				OutputDebugStringA("WebRequest Recv FINISHED!\n");
				break;
			}
			else{
				buf[n] = '\0';
				request.append(buf);    dwTick = GetTickCount();
			}
		} while (true);
		return true;
	}

	std::string GetHtml(const std::string& response)
	{
		int conn_index = response.find("Connection:");
		string str = response.substr(conn_index);
		int line_index = str.find("\r\n");
		if (line_index != -1)
		{
			string contect = str.substr(line_index);
			//MessageBox(NULL,contect.c_str(),"ok",0);
			return contect;
		}
		return str;
	}

	bool IsResult200(const std::string& response)
	{
		const char* httpStr = "HTTP/1.1";
		int httpIndex = response.find(httpStr);
		if (httpIndex != std::string::npos) {
			string str = response.substr(httpIndex + strlen(httpStr), response.length() - 1);
			int dwRet = atoi(str.c_str());
			if (dwRet == 200)
				return true;
		}

		return false;
	}
	//UTF8编码
	std::string utf2ansi(LPCSTR pszSrc)
	{
		int nSize = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pszSrc, -1, 0, 0);
		if (nSize <= 0) return NULL;
		WCHAR *pwsz = new WCHAR[nSize + 1];
		if (NULL == pwsz) return NULL;
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pszSrc, nSize, pwsz, nSize);
		pwsz[nSize] = 0;
		char *psz = new char[nSize + 1];
		WideCharToMultiByte(CP_ACP, 0, pwsz, nSize, psz, nSize, NULL, NULL);
		string str = psz;
		delete[] pwsz;
		delete[] psz;
		return str;
	}
}



int WideToUTF8(const std::string& _src, string fileName)
{
	int nBufSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)_src.c_str(), -1, NULL, 0, 0, FALSE);
	char *szBuf = new char[nBufSize];
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)_src.c_str(), -1, szBuf, nBufSize, 0, FALSE);
	ofstream fout(fileName.c_str(), std::ios::out);//std::ios::binary 
	fout.write(szBuf, nBufSize);
	fout.close();
	//std::string strRet(szBuf);
	//delete []szBuf;
	//szBuf = NULL;
	/** 写出宽字符串时这样用，内容在text中
	if(fopen_s( &pFile, "log.txt","a")!=0)
	{
	std::string str = WideToUTF8( text );
	fputs(str.c_str(), pFile);
	fputc('\n',pFile);
	fclose(pFile);
	}*/
	return 0;
}


int writeToTxt(string fileName, string data)
{
	ofstream fout(fileName.c_str(), std::ios::out);//std::ios::binary 
	//0xef, 0xbb, 0xbf, 
	//fout.write((char*)0x00ef,4);
	//fout.write((char*)0x00bb,4);
	//fout.write((char*)0x00bf,4);	
	//unsigned char uniTxt[] = {0xFF, 0xFE};          // Unicode file header  
	//unsigned char endianTxt[] = {0xFE, 0xFF};       // Unicode big endian file header  
	//unsigned char utf8Txt[] = {0xEF, 0xBB, 0xBF};   // UTF_8 file header  
	fout << 0xEF << 0xBB << 0xBF << endl;
	fout.write(data.c_str(), data.length());
	fout.close();
	return 0;
}
int writeToTxt002(string fileName, string data)
{
	FILE *file;
	file = fopen(fileName.c_str(), "rw");
	if (file == NULL)
	{
		printf("打开失败!\n");
		return -1;
	}
	fwrite(data.c_str(), data.length(), 1, file);
	fclose(file);
	return 0;
}


//request("10.34.39.131:8080", "lol-data-report.html", "loginData=abcd&gameData=cdef");
int test_post(int argc, char* argv[])
{
	string rs;
	char post[80];
	sprintf(post, "loginData=xj&gameData=123456");
	DEVWEB::RequestParam rp;
	rp.bPost = true;
	rp.pPostData = post;
	rp.uiPostLen = strlen(post);
	if (DEVWEB::WebRequest("http://10.34.39.131:8080/lol-data-report.html", rs, &rp)) {
		rs = DEVWEB::utf2ansi((const char*)rs.c_str());
		MessageBoxA(NULL, rs.c_str(), "ok", 0);
		string html = DEVWEB::GetHtml(rs);
		MessageBoxA(NULL, html.c_str(), "ok", 0);
		writeToTxt("test.html", html);
	}
	else {
		MessageBoxA(NULL, "错误", "ok", 0);
	}
	getchar();
	return 0;
}