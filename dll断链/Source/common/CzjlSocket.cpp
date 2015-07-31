#include "CzjlSocket.h"
#include "zjlcommon.h"

#pragma warning (disable:4996 4800)

CzjlSocket::CzjlSocket(void)
{
	WORD version;
	WSADATA wsaData = { 0 };
	int rVal = 0;

	version = MAKEWORD(1, 1);

	WSAStartup(version, (LPWSADATA)&wsaData);
	m_socket = 0;
	m_bConnect = 0;
}


CzjlSocket::~CzjlSocket(void)
{
	DbgPrint("CzjlSocket::~CzjlSocket");
	if (m_socket)
	{
		closesocket(m_socket);
	}

	WSACleanup();
}

bool CzjlSocket::ConnectServ(CStringA szIP, unsigned int Port)
{
	DbgPrint("ConnectServ %s:%d", szIP, Port);
	if (Init() == 0)
	{
		DbgPrint("ConnectServ Error Socket=%s", m_socket);
		return 0;
	}
	if (m_bConnect == 0)
	{
		SOCKADDR_IN serverInfo;
		serverInfo.sin_family = PF_INET;
		serverInfo.sin_addr.s_addr = inet_addr(szIP);
		serverInfo.sin_port = htons(Port);

		int rVal = connect(m_socket, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
		if (rVal == SOCKET_ERROR)
		{
			m_bConnect = 0;
		}
		else{
			m_bConnect = 1;
		}
	}
	DbgPrint("连接主程序 ConnectServ Recv %d", m_bConnect);
	return m_bConnect;
}



int CzjlSocket::SendData(char* pBuffer, unsigned int Len, char* pRecvBuffer, unsigned int RecvBufLen, unsigned int* RecvLen)
{
	unsigned int SendByte = 0;
	if (m_bConnect && pBuffer && Len)
	{
		SendByte = send(m_socket, (char*)pBuffer, Len, 0);

		/*
		if (pRecvBuffer && RecvLen)
		{
		Rev=recv(m_socket,(char*)&pRecvBuffer,RecvBufLen,0);
		if (RecvLen)
		{
		*RecvLen=Rev;
		}
		}*/

	}

	//DbgPrint("发送数据包 Send%d字节, 真实发送%d字节",Len,SendByte);

	return SendByte;
}

bool CzjlSocket::Init()
{
	if (m_socket == 0)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		DbgPrint("Socket = 0x%08x", m_socket);
		return m_socket;
	}
	else{
		return 1;
	}
}

void CzjlSocket::DisConnect()
{
	DbgPrint("CzjlSocket::DisConnect()");
	if (m_socket)
	{
		closesocket(m_socket);
	}
}

int CzjlSocket::RecvData(char* pRecvBuffer, unsigned int RecvLen)
{
	if (pRecvBuffer && RecvLen && m_socket)
	{
		int result = recv(m_socket, (char*)pRecvBuffer, RecvLen, 0);
		//DbgPrint("接受数据包 Recv%d字节, 真实接受%d字节",RecvLen,result);
		//DbgPrint("%s",PrintBytes("Recv Bytes",pRecvBuffer,result));

		return result;
	}
	else
	{
		DbgPrint("接受数据包失败");
	}
	return -1;
}
