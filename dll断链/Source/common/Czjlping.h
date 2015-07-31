#pragma once


bool zjlpingByip(  char *ipaddr, int Count,int* ping);
int zjlping(char* pAddr)
{
	int ping=0;
	CStringA szOut;
	CStringA sIPAddress;
	struct hostent FAR *lpHostEnt=gethostbyname(pAddr);
	if(lpHostEnt == NULL)
	{
		sIPAddress = _T("");
	}
	LPSTR lpaddr = lpHostEnt->h_addr_list[0];
	if(lpaddr)
	{
		struct in_addr inAddr;
		memmove (&inAddr,lpaddr, 4);
		sIPAddress=inet_ntoa (inAddr);
		if(sIPAddress.IsEmpty())
			sIPAddress=_T("");
	} 
	if (zjlpingByip(sIPAddress.GetBuffer(),2,&ping)==0)
	{
		ping=-1;
	}

	return ping;
}




