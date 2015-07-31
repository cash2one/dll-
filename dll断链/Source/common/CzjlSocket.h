#pragma once
#include <atlstr.h>
#pragma comment(lib, "wsock32.lib")


class CzjlSocket
{
public:
	CzjlSocket(void);
	~CzjlSocket(void);
	bool ConnectServ(CStringA szIP,unsigned int Port);
	void DisConnect();
	int SendData(char* pBuffer,unsigned int Len,char* pRecvBuffer=0,unsigned int RecvBufLen=0,unsigned int* RecvLen=0);
	int RecvData(char* pBuffer,unsigned int Len);


private:
		bool Init();
	SOCKET m_socket;
	bool m_bConnect;
};

