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
		bool            bPost;      //�Ƿ�ΪPOST����
		const char*     pPostData;  //ΪPOST���ݣ���Ҫ����ά���ڴ�;
		unsigned int    uiPostLen;  //ΪPOST���ݳ��ȣ���ҪΪ�˼��ݿ����ϴ����������ݣ�һ�������ΪpPostData�ַ�������
		const char*     pReferer;   //���������ҳ�棬������Դҳ��
		const char*     pAttachHeader;//�����Զ���ͷ
		bool            bBlock;     //true=����ʽ���ӣ�false=�첽����,*******ע�⣺ʹ���첽�������ó�ʱʱ��
		unsigned int    uiTimeout;  //�첽��ʱ ��λms
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