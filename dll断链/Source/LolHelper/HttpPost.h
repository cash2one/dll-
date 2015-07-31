#ifndef HTTP_POST_H
#define HTTP_POST_H

#include <iostream>

#include <string>
#include <fstream>
#include <winsock2.h> 
#include <stdio.h>

namespace DEVWEB
{
	struct ACloseSkt
	{
		ACloseSkt(SOCKET h)
		{
			m_h = h;
		}
		~ACloseSkt()
		{
			if (m_h)
				closesocket(m_h);
			m_h = 0;
		}
		SOCKET m_h;
	};
	struct RequestParam
	{
		bool            bPost;      //是否为POST请求
		const char*     pPostData;  //为POST数据，需要自已维护内存;
		unsigned int    uiPostLen;  //为POST数据长度，主要为了兼容可以上传二进制数据，一般请求就为pPostData字符串长度
		const char*     pReferer;   //请求的引用页面，就是来源页面
		const char*     pAttachHeader;//附加自定义头
		bool            bBlock;     //true=阻塞式连接，false=异步连接,*******注意：使用异步必须设置超时时间
		unsigned int    uiTimeout;  //异步超时 单位ms
		RequestParam(){
			memset(this, 0, sizeof(RequestParam));
			bBlock = true; bPost = false;
		}
	};
	bool WebRequest(const char* szURL, std::string& response, RequestParam* pRP = 0);
	std::string GetHtml(const std::string& response);
	std::string utf2ansi(LPCSTR pszSrc);
	bool IsResult200(const std::string& response);
}

#endif