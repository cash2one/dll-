// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "CzjlHook.h"
#include "dllmain.h"
#include "atlstr.h"
#include "zHook.h"
#include <shellapi.h>
#include "LOL/ParserGameState.h"
#include <Strsafe.h>
#include <time.h>
#include "CEnum.h"
#include <atltime.h>
#include "LOL\LOLDef.h"
#include "CzjlSocket.h"
#include "zjlcommon.h"

int bDebug=1;

void SetServerStr(unsigned int ServerID);
void __stdcall ParserData(unsigned int LenBuffer,char* Buffer);
 DWORD WINAPI TestTheard(PVOID param);
  DWORD WINAPI ThreadDisableInPut(LPVOID LPCSTR);
//CzjlTextLog //g_Log("C:\\AirLib_Match_Debug.txt");
bool bPrintMsg=0;

//CGALolClient_Match g_Client;
CParserGameState g_Parser;
stSharePlayerMem* g_pQueue=0;
stSharePlayerMem_Account* g_pAccount=0;
bool bLoadClientMsg=0;

GLOBALCONFIG* g_GameConfig;
LolClientConfig* g_ClientConfig;
CzjlSocket g_Socket;
bool g_DisableInput=0;
struct{
	CzjlHook HookLog;
	int _HookLog_GotoAddr;
	int _HookLog_BackAddr;
	char CmpMemLog[5];

	CzjlHook HookData;
	int _HookData_GotoAddr;//hook��ַ
	int _HookData_BackAddr;//�������ĵ�ַ
	int _HookData_CallAddr;//ԭ��call�ĵ�ַ
	char CmpMemData[5];

}Hook;
void   __stdcall MyCallLog();
void   __stdcall MyCallData();
unsigned int CountLog=1;
unsigned int CountData=1;

HOOKDATA pHookLoadLibraryA;

DWORD WINAPI __stdcall ThreadReplace(LPVOID lp)
{
	while(1)
	{
		if (memcmp((void*)Hook._HookData_GotoAddr,&Hook.CmpMemData[0],5)==0)
		{
			//g_Log.AddString("******************************************************Replace Data\r\n");
			cout<<"******************************************************Replace Data\r\n";
			Hook.HookData.HookAddrToMyFn_Start();
		}

		if (memcmp((void*)Hook._HookLog_GotoAddr,&Hook.CmpMemLog[0],5)==0)
		{
			//g_Log.AddString("******************************************************Replace Log\r\n");
			cout<<"******************************************************Replace Log\r\n";
			Hook.HookLog.HookAddrToMyFn_Start();
		}
		Sleep(100);
	}
}

bool Hooked=0;
void InHook()
{
	if (Hooked)
	{
		return;
	}
	Hooked=1;
		//OpenColsole();
	unsigned int hookAddr;

	// 3B C1 73 0A 80 3A 00 74 05 42 3B D1
	char TargetUI[]={0x3B,0xC1,0x73,0x0A,0x80,0x3A,0x00,0x74,0x05,0x42,0x3B,0xD1};
	hookAddr=CzjlHook::FindDllTarget("Adobe AIR.dll","",TargetUI,sizeof(TargetUI));
//	zjlDebugBoxOutHex("Adobe AIR.dll",hookAddr);
	if (hookAddr)
	{//Action
		Hook._HookLog_GotoAddr=hookAddr-5;
		Hook._HookLog_BackAddr=hookAddr;
		Hook.HookLog.HookAddrToMyFn((LPVOID)(Hook._HookLog_GotoAddr),MyCallLog);
		DbgPrint("AirLib_Match Log 0x%08x\r\n",Hook._HookLog_GotoAddr);
		//����ԭ����5�ֽ�
		memcpy(Hook.CmpMemLog,(void*)Hook._HookLog_GotoAddr,5);
		Hook.HookLog.HookAddrToMyFn_Start();
	}else{
		DbgPrint("MyCallLog hookʧ��");
		//g_Log.AddString("*AirLib Error MyLog");
//		zjlDebugBoxOutHex("hookAddrLog",hookAddr);
	}
	//C2 08 00 6A 01 8D 4D B4 E8 50 09 07 00
	//__asm int
#if 0
	int iii = 1;
	while (iii < 30) {
		Sleep(1000);
	}
	__asm nop
	__asm nop
	__asm nop
	__asm nop
	__asm nop
#endif 
	char TargetData[]={0xC2,0x08,0x00,0x6A,0x01,0x8D,0x4D,0xB4,0xE8,0x50,0x09,0x07,0x00};		//0x1eb332
	hookAddr=CzjlHook::FindDllTarget("Adobe AIR.dll","",TargetData,sizeof(TargetData));
//	zjlDebugBoxOutHex("Adobe AIR.dll",hookAddr);
	//		zjlDebugBoxOutHex("hookAddrA",hookAddr-5);
	if (hookAddr)
	{//Action
		Hook._HookData_GotoAddr=hookAddr-5;
		Hook._HookData_BackAddr=hookAddr;
		int g=*((int*)(Hook._HookData_GotoAddr+1));//ԭ��callƫ��
		Hook._HookData_CallAddr=g+Hook._HookData_BackAddr;//ԭ��call��ַ
		//		zjlDebugBoxOutHex("_HookData_CallAddr",Hook._HookData_CallAddr);
		Hook.HookData.HookAddrToMyFn((LPVOID)(Hook._HookData_GotoAddr),MyCallData);
		DbgPrint("AirLib_Match Data 0x%08x\r\n",Hook._HookData_GotoAddr);
		//����ԭ����5�ֽ�
		memcpy(Hook.CmpMemData,(void*)Hook._HookData_GotoAddr,5);
		CreateThread(0,0,ThreadReplace,0,0,0);
		Hook.HookData.HookAddrToMyFn_Start();
	}else{
		DbgPrint("MyCallData hookʧ��");
		//g_Log.AddString("*AirLib Error MyData");
//		zjlDebugBoxOutHex("_HookData_CallAddr",Hook._HookData_CallAddr);
	}	
	DbgPrint("��ȡLOL����ģ��hook���,�ѿ�ʼ����");
}


HMODULE WINAPI zLoadLibraryA(__in LPCSTR lpLibFileName)
{
	zHookStop(&pHookLoadLibraryA);


	HMODULE hModule = LoadLibraryA(lpLibFileName);
	CStringW sz=lpLibFileName;

	if (sz.Find(L"Adobe AIR.dll")!=-1)
	{
		InHook();
	}

	zHookStart(&pHookLoadLibraryA);

	return hModule;
}
HOOKDATA pHookLoadLibraryW;

HMODULE WINAPI zLoadLibraryW(__in LPWSTR lpLibFileName)
{
	zHookStop(&pHookLoadLibraryW);

	HMODULE hModule = LoadLibraryW(lpLibFileName);
	CStringW sz=lpLibFileName;

	if (sz.Find(L"Adobe AIR.dll")!=-1)
	{
		InHook();
	}

	zHookStart(&pHookLoadLibraryW);

	return hModule;
}


CStringA MyUTF8ToAnsi( const char* str )
{
	int textlen ;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ); 
	wchar_t * Unicode = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t)); 
	memset(Unicode,0,(textlen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)Unicode,textlen ); 


	textlen = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, Unicode, -1, NULL, 0, NULL, NULL );
	char* result =(char *)malloc((textlen+1)*sizeof(char));
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, Unicode, -1, result, textlen, NULL, NULL );


	CStringA sz=result;
	delete[] Unicode;
	delete[] result;
	return sz;
}

void __stdcall ParserLog( char* pA1)
{
	if (IsBadReadPtr(pA1,1)==0)
	{
		CStringA szUtf8=pA1;
		CStringA szPath;
		CStringA sz=MyUTF8ToAnsi(szUtf8.GetBuffer());


		static int count = 0;
		time_t t = time(NULL);
		CreateDirectoryA("c:\\amf3", NULL);
		CreateDirectoryA("c:\\amf3\\xmpp", NULL);
		char time_buf[128] = { 0 };
		sprintf(time_buf, "%d_xmpp_%ld", ++count, t);
		std::string fileName = std::string("c:\\amf3\\xmpp\\") + time_buf + ".rtmp";

		FILE *fp = fopen(fileName.c_str(), "wb");
		if (fp) {
			fwrite(sz.GetBuffer(sz.GetLength()), 1, sz.GetLength(), fp);
			fclose(fp);
		}

/*
CreateDirectoryA("C:\\Test1\\",0);
szPath.Format("C:\\Test1\\%05d__Len_x%x__.txt",CountLog++,sz.GetLength());
DbgPrint("ParserLog ���浽 %s",szPath);
CzjlFile file(szPath);
file.WriteStringA(sz);*/



		int _begin=0,_end=0;
		if (sz.Left(4)=="ime=")
		{//��ȡQQ���������
			_begin=sz.Find("uin=");
			_begin+=4;
			_end=sz.Find("&area=");
			long long QQ;
			unsigned int Server;
			if (_end!=-1 && _begin!=-1)
			{
				CStringA szNumber=sz.Mid(_begin,_end-_begin);
				QQ=_atoi64(szNumber);
			}

			if (_end!=-1 && _begin!=-1)
			{
				_begin=_end+6;
				_end=sz.Find("&timestamp=");
				CStringA szServer=sz.Mid(_begin,_end-_begin);
				Server=atoi(szServer);
			}
			if (g_pQueue)
			{
				g_pQueue->GameState.QQ=QQ;
				g_pQueue->GameState.ServerID=Server;
						SetServerStr(Server);

			}
		}


		_begin=sz.Find("<summoner_name>");
		if(_begin!=-1) 
		{//��ȡ�˺���
			_begin+=15;
			CStringA szName;
			_end=sz.Find("</summoner_name>",_begin);
			if (_end!=-1)
			{
				szName=sz.Mid(_begin,_end-_begin);

				if (g_pQueue)
				{
					StrCpyA(g_pQueue->GameState.szMainPlayer,szName.GetBuffer());

				}
			}
		}

		_begin=sz.Find("<jid>sum");
		if(_begin!=-1) 
		{
			_begin+=8;
			CStringA szGlobalID;
			_end=sz.Find("@pvp.net/xiff</jid>",_begin);
			if (_end!=-1)
			{//ΨһID
				szGlobalID=sz.Mid(_begin,_end-_begin);
				if (g_pQueue)
				{
					g_pQueue->GameState.LOLGlobalID=_atoi64(szGlobalID);
				}
			}
		}

		_begin=sz.Find("</show><status>");
		if(_begin!=-1) 
		{
			CStringA szLevel,szRank;

			_begin=sz.Find("level&gt;");
			if (_begin!=-1)
			{
				_begin+=9;
				_end=sz.Find("&lt;",_begin);

				if (_end!=-1)
				{
					szLevel=sz.Mid(_begin,_end-_begin);
					//���
/*
					CStringA szOut;
					szOut.Format("2QQ:%I64u\r\nPlayerName:%s\r\nServerID:%d\r\nServerName:%s\r\nID:%I64u\r\nLevel:%d\r\nRank:%s\r\n\r\n",g_pQueue->GameState.QQ,g_pQueue->GameState.szMainPlayer,g_pQueue->GameState.ServerID,g_pQueue->GameState.szServerName,g_pQueue->GameState.LOLGlobalID,g_pQueue->GameState.Level,g_pQueue->GameState.szRankLevel );
					zjlDebugOut(szOut);
					//g_Log.AddString(szOut);*/

				}
			}			
		}	

		_begin=sz.Find("<presence to='sum");
		if(_begin!=-1) 
		{//RANK
			_begin+=17;
			CStringA szCmp1,szCmp2;

			_end=sz.Find("@pvp.net",_begin);
			if (_end!=-1)
			{
				szCmp1=sz.Mid(_begin,_end-_begin);
			}

			_begin=sz.Find("from='sum",_end);
			if(_begin!=-1) 
			{
				_begin+=9;
				_end=sz.Find("@pvp.net",_begin);
				if (_end!=-1)
				{
					szCmp2=sz.Mid(_begin,_end-_begin);
				}
			}
			if (szCmp1==szCmp2)
			{//�Լ���rank����,��ȡ֮

				_begin=sz.Find("</show><status>");
				if(_begin!=-1) 
				{
					CStringA szLevel,szTier,szDivision,szHeadImg;

					_begin=sz.Find("profileIcon&gt;");
					if (_begin!=-1)
					{
						_begin+=15;
						_end=sz.Find("&lt;",_begin);

						if (_end!=-1)
						{
							szHeadImg=sz.Mid(_begin,_end-_begin);
						}
					}


					_begin=sz.Find("level&gt;");
					if (_begin!=-1)
					{
						_begin+=9;
						_end=sz.Find("&lt;",_begin);

						if (_end!=-1)
						{
							szLevel=sz.Mid(_begin,_end-_begin);
						}
					}

					//  rankedLeagueTier&gt;DIAMOND&lt/;
					_begin=sz.Find("rankedLeagueTier&gt;");
					if (_begin!=-1)
					{
						_begin+=20;
						_end=sz.Find("&lt;",_begin);

						if (_end!=-1)
						{
							szTier=sz.Mid(_begin,_end-_begin);
						}
					}
					_begin=sz.Find("Division&gt;");
					if (_begin!=-1)
					{
						_begin+=12;
						_end=sz.Find("&lt;/rankedLeagueDivision",_begin);

						if (_end!=-1)
						{
							szDivision=sz.Mid(_begin,_end-_begin);
						}
					}

					if (g_pQueue)
					{
						if (szLevel!="")
						{
							g_pQueue->GameState.Level=atoi(szLevel);
/*
							CStringA szOut;
								szOut.Format("*LEVEL_QQ:%I64u\r\nPlayerName:%s\r\nServerID:%d\r\nServerName:%s\r\nID:%I64u\r\nLevel:%d\r\nRank:%s\r\nImageHead:%d\r\n",g_pQueue->GameState.QQ,g_pQueue->GameState.szMainPlayer,g_pQueue->GameState.ServerID,g_pQueue->GameState.szServerName,g_pQueue->GameState.LOLGlobalID,g_pQueue->GameState.Level,g_pQueue->GameState.szRankLevel,g_pQueue->GameState.HeadImgID );
							//g_Log.AddString(szOut);*/

						}
						
						if (szHeadImg!="")
						{
							g_pQueue->GameState.HeadImgID=atoi(szHeadImg);
								
/*
							printf("g_pQueue->GameState.HeadImgID = %d\r\n",g_pQueue->GameState.HeadImgID);
							CStringA szOut;
							szOut.Format("*LEVEL_QQ:%I64u\r\nPlayerName:%s\r\nServerID:%d\r\nServerName:%s\r\nID:%I64u\r\nLevel:%d\r\nRank:%s\r\nImageHead:%d\r\n",g_pQueue->GameState.QQ,g_pQueue->GameState.szMainPlayer,g_pQueue->GameState.ServerID,g_pQueue->GameState.szServerName,g_pQueue->GameState.LOLGlobalID,g_pQueue->GameState.Level,g_pQueue->GameState.szRankLevel,g_pQueue->GameState.HeadImgID );
							//g_Log.AddString(szOut);*/

						}

						if (szDivision!="" && szTier!=""  )
						{
							CStringA szRank;
							szRank.Format("%s_%s",szTier,szDivision);
							StrCpyA(g_pQueue->GameState.szRankLevel,szRank);
						}								
					}

				}
				
			}
		}
/*
		long long QQ;			  //QQ��
		long long LOLGlobalID;//ΨһID
		char szMainPlayer[128];//�����

		unsigned int Level;	//�˺ŵȼ�
		char szRankLevel[128];		//��λ�ȼ�

		unsigned int ServerID;//������ID
		char szServerName[128];//��������
		char szVersion[128];	//�������汾
		time_t GameEndTime;//��Ϸ������ʱ��,��ȥ��Ϸ���Ⱦ�����Ϸ��ʼʱ��

		//2014-11-12�¼�2���ֶ�
		unsigned int HeadImgID;		//���ͷ��ID*/

	}	
}
__declspec(naked) void   __stdcall MyCallLog()
{
	__asm{
		pushad;
		pushfd;
		push eax;
		call ParserLog;
		popfd;
		popad;
		lea ecx,[eax+edi];
		mov edx,eax;
		jmp Hook._HookLog_BackAddr;
	}
}

void PrintMatchInfo()
{
	DbgPrint("����������Ϣ:***********************************");
	DbgPrint("����ģʽ %d",g_ClientConfig->Match.bMatch);
	DbgPrint("�������� %d",g_ClientConfig->Match.bCreate);
	DbgPrint("��ͼ %d",g_ClientConfig->Match.MapIndex);
	DbgPrint("�ۿ�ģʽ %d",g_ClientConfig->Match.ViewIndex);
	DbgPrint("����ģʽ %d",g_ClientConfig->Match.GameMode);
	DbgPrint("�Ƿ��ֶ� %d",g_ClientConfig->Match.bManual);
	DbgPrint("������ %d %s",g_ClientConfig->Match.ServerID,GetServerNameByID(g_ClientConfig->Match.ServerID));
	DbgPrint("������ %s",g_ClientConfig->Match.szRoomName);
	DbgPrint("���� %s",g_ClientConfig->Match.szPassword);
	DbgPrint("���� %d",g_ClientConfig->Match.bOwnerCamp);
	DbgPrint("************************************************");
}



void TestServer()
{
	StrCpyW(g_pAccount->account,L"2000021318");
	CzjlFile file("C:\\st_bin");
	memcpy(g_pAccount->ST,file.GetFileBuffer(),128);
}

/*

void ConnectServer()
{
	//g_Log.AddString("*AirLib ��ʼ��½!");
	for (unsigned int i=0;true;i++)
	{
		if ( g_pAccount->account[0] 	 && g_pAccount->IP[0] && g_pAccount->Port)
		{
			CStringW sW=g_pAccount->account;
			CStringA sA=sW+"�˺ŵ�½��";
			//g_Log.AddString(sA);
			sA.Format("���ӷ�����IP : %s :%d",g_pAccount->IP,g_pAccount->Port);
			//g_Log.AddString(sA);
			if (g_Client.Open(g_pAccount->account,"","","",g_pAccount->ST,g_pAccount->IP,g_pAccount->Port))
			{
				break;//��½�ɹ�
			}else{
				//g_Log.AddString("*AirLib ��½ʧ��!\r\n");
				memset(g_pAccount->account,0,16);
				memset(g_pAccount->ST	,0,128);
				Sleep(1000*2);
			}

		}else{
			printf("*AirLib �ͻ���δ��½�˺�\r\n");
			//g_Log.AddString("*AirLib �ͻ���δ��½�˺�!\r\n");
			Sleep(1000*10);
		}
	}
	//g_Log.AddString("*AirLib ��½�ɹ�!");
}
*/



int RecvMessagetoApp(int Commandid)
{
//	DbgPrint("����������֪ͨ....");
	char pRecvBuffer[1024]={0};
	int irecv=g_Socket.RecvData(pRecvBuffer,1024);
//	DbgPrint("�������ݰ�size=%d",irecv);
	if (irecv!=-1 && irecv!=0)
	{
		if (irecv>=sizeof(LocalCmdHeader))
		{
			byte bResu= ((stLOCAL_CMD_MATCH_MSG*)&pRecvBuffer[sizeof(LocalCmdHeader)])->bSucc;
			DbgPrint("���յ�����ID %d  Suc=%d ",((LocalCmdHeader*)pRecvBuffer)->nCmdId),bResu;
			if (((LocalCmdHeader*)pRecvBuffer)->nCmdId==Commandid)
			{
				return bResu;
			}		
		}
	}
		
	

	return 0;
}


CStringA PrintGameInfo(stLOLGame GameData)
{
	CStringA szResult,szTemp;
	szResult+="������Ϸ��Ϣ\r\n";
	szTemp.Format("GameID : %I64u ", GameData.GameID);
	szResult+=szTemp+"\r\n";

	szTemp.Format("GameLengh : %d ", GameData.GameLengh);
	szResult+=szTemp+"\r\n";

	szTemp.Format("PlayerCount : %d ", GameData.PlayerCount);
	szResult+=szTemp+"\r\n";

	szTemp.Format("WinnerTeam : %d ", GameData.WinnerTeam);
	szResult+=szTemp+"\r\n";

	szTemp.Format("GameMode : %d ", GameData.GameMode);
	szResult+=szTemp+"\r\n";

	szTemp.Format("GameType : %d ", GameData.GameType);
	szResult+=szTemp+"\r\n";

	szResult+="�ϴ�����Ϣ\r\n";
	szTemp.Format("QQ : %I64u ", GameData.GameState.QQ);
	szResult+=szTemp+"\r\n";

	szTemp.Format("ServerID : %u ", GameData.GameState.ServerID);
	szResult+=szTemp+"\r\n";

	szTemp.Format("LOLGlobalID : %u ", GameData.GameState.LOLGlobalID);
	szResult+=szTemp+"\r\n";

	szTemp.Format("szMainPlayer : %s ", GameData.GameState.szMainPlayer);
	szResult+=szTemp+"\r\n";

	szTemp.Format("Level : %d ", GameData.GameState.Level);
	szResult+=szTemp+"\r\n";

	CTime time=GameData.GameState.GameEndTime;
	szTemp.Format("GameEndTime : %d-%d-%d %d:%d:%d \r\n",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
	szResult+=szTemp+"\r\n";

	szTemp.Format("szRankLevel : %s ", GameData.GameState.szRankLevel);
	szResult+=szTemp+"\r\n";

	szTemp.Format("HeadImgID : %d ", GameData.GameState.HeadImgID);
	szResult+=szTemp+"\r\n";

	szTemp.Format("RateNumber : %d ", GameData.GameState.RateNumber);
	szResult+=szTemp+"\r\n";

	for (unsigned int i=0;i<LOLMAXPLAYER;i++)
	{
		long long PGID=GameData.Players[i].globalPlayerID;

		if (PGID)
		{
			szResult+="______________________________\r\n";
			szTemp.Format("Index : %d ", i+1);
			szResult+=szTemp+"\r\n";

			szTemp.Format("TEAM : %d ", GameData.Players[i].TEAM);
			szResult+=szTemp+"\r\n";

			szTemp.Format("Win? : %d ", GameData.Players[i].Win);
			szResult+=szTemp+"\r\n";

			szTemp.Format("globalPlayerID : %I64u ", PGID);
			szResult+=szTemp+"\r\n";

			szTemp.Format("szPlayerName : %s ", GameData.Players[i].szPlayerName);
			szResult+=szTemp+"\r\n";

			szTemp.Format("HeroID : %d ", GameData.Players[i].HeroID);
			szResult+=szTemp+"\r\n";

			szTemp.Format("szHeroName : %s ", GameData.Players[i].szHeroName);
			szResult+=szTemp+"\r\n";

			szTemp.Format("LEVEL : %d ", GameData.Players[i].LEVEL);
			szResult+=szTemp+"\r\n";

			szTemp.Format("Kill : %d ", GameData.Players[i].CHAMPIONS_KILLED);
			szResult+=szTemp+"\r\n";

			szTemp.Format("Dead : %d ", GameData.Players[i].NUM_DEATHS);
			szResult+=szTemp+"\r\n";

			szTemp.Format("ASSISTS : %d ", GameData.Players[i].ASSISTS);
			szResult+=szTemp+"\r\n";

			szTemp.Format("С����ɱ : %d ", GameData.Players[i].MINIONS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("��Ӫ : %d ", GameData.Players[i].BARRACKS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("���� : %d ", GameData.Players[i].TURRETS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("���� : %d ", GameData.Players[i].WARD_PLACED);
			szResult += szTemp + "\r\n";

			szTemp.Format("���� : %d ", GameData.Players[i].WARD_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("��߶�ɱ : %d ", GameData.Players[i].LARGEST_MULTI_KILL);
			szResult += szTemp + "\r\n";

			szTemp.Format("�����ɱ : %d ", GameData.Players[i].LARGEST_KILLING_SPREE);
			szResult += szTemp + "\r\n";

			szTemp.Format("��Ǯ : %d ", GameData.Players[i].GOLD_EARNED);
			szResult+=szTemp+"\r\n";

			szTemp.Format("����˺� : %d ", GameData.Players[i].TOTAL_DAMAGE_DEALT);
			szResult+=szTemp+"\r\n";

			szTemp.Format("�����˺� : %d ", GameData.Players[i].TOTAL_DAMAGE_TAKEN);
			szResult+=szTemp+"\r\n";

			szTemp.Format("ITEM0 : %d ", GameData.Players[i].ITEM0);
			szResult+=szTemp+"\r\n";

			szTemp.Format("ITEM1 : %d ", GameData.Players[i].ITEM1);
			szResult+=szTemp+"\r\n";
		}
	}

	return szResult;
}
bool SendMessagetoApp(void* pBuffer,unsigned int Len,int Commadnid)
{

	//DbgPrint("��ʼ�������������ݰ�");

	int SendBufSize=Len+sizeof(LocalCmdHeader);

	char* pSendBuffer=new char[SendBufSize];
	if (pSendBuffer)
	{
		((LocalCmdHeader*)(pSendBuffer))->nLen=SendBufSize;
		((LocalCmdHeader*)(pSendBuffer))->nCmdId=Commadnid;

		memcpy(pSendBuffer+sizeof(LocalCmdHeader),pBuffer,Len);
		int SendByte=g_Socket.SendData(pSendBuffer,SendBufSize);		
		DbgPrint("��ʼ�������������ݰ�,����: %d�ֽ� ����ID: %d ,�ѷ��� %d�ֽ�",SendBufSize,Commadnid,SendByte);

		delete[] pSendBuffer;
		if (SendByte==0 || SendByte==-1)
		{
			return 0;
		}
	}else{
		DbgPrint("�����ڴ�ʧ�� %d �ֽ�",SendBufSize);
		return 0;
	}

	//DbgPrint("��ʼ�������������ݰ� End");
}

bool bInfoUpdate=0;
DWORD WINAPI ThreadUpdateLog(LPVOID lp)
{
	//g_Log.AddString("*AirLib ThreadUpdateLog!");
	DbgPrint("����������ս�������߳�������");
			time_t	t;
			time(&t);
			//Test
			while(g_pQueue)
			{
				//ս������
				if (g_pQueue->Buffer.LenBuffer)
				{
					DbgPrint("������Ϸս��,׼���ϴ��������� Len=0x%08x \r\n",g_pQueue->Buffer.LenBuffer);
					CParserGameState Parser;
					enumBUFFTYPE result=Parser.Parser((unsigned char*)g_pQueue->Buffer.pDataBuffer,g_pQueue->Buffer.LenBuffer);
					if (TYPE_GAMESTATE==result)
					{
						CStringA szDebugPath="C:\\1.txt";
						DeleteFileA(szDebugPath);
						CzjlFile DebugFile(szDebugPath);

						CStringA szDebugFile=PrintGameInfo(*Parser.GetData());
						DbgPrint("ս��! %s",szDebugFile);
						DebugFile.WriteStringA(szDebugFile);
						Sleep(1000);
						ShellExecuteA(0,"open",szDebugPath,"","",SW_SHOWNORMAL);
					}
/*
					CStringA szOut;
					szOut.Format("������Ϸս��,׼���ϴ��������� Len=0x%08x \r\n",g_pQueue->Buffer.LenBuffer);
					//g_Log.AddString(szOut);
					Sleep(1000*5);//�ȴ�ʤ���������
					//�������	
					SendMessagetoApp(g_pQueue->Buffer.pDataBuffer,g_pQueue->Buffer.LenBuffer,LOCAL_CMD_RESULT);
					//////////////////////////////////////////////////////////////////////////			
					Sleep(1000*5);
					//g_Log.AddString("*AirLib BufferClear OK! \r\n");*/

					g_pQueue->Buffer.LenBuffer=0;
					memset(g_pQueue->Buffer.pDataBuffer,0,1024*20);		
				}

				//ͷ�񲿷�
				if (bInfoUpdate==0 && bLoadClientMsg ==0 && g_pQueue->GameState.QQ && g_pQueue->GameState.LOLGlobalID && g_pQueue->GameState.szMainPlayer[0] && g_pQueue->GameState.Level &&g_pQueue->GameState.ServerID)
				{
					CStringA szOut;
					szOut.Format("*LEVEL_QQ:%I64u\r\nPlayerName:%s\r\nServerID:%d\r\nServerName:%s\r\nID:%I64u\r\nLevel:%d\r\nRank:%s\r\nImageHead:%d\r\n",g_pQueue->GameState.QQ,g_pQueue->GameState.szMainPlayer,g_pQueue->GameState.ServerID,g_pQueue->GameState.szServerName,g_pQueue->GameState.LOLGlobalID,g_pQueue->GameState.Level,g_pQueue->GameState.szRankLevel,g_pQueue->GameState.HeadImgID );
					DbgPrint("%s",szOut);

					stLOCAL_CMD_LOGININFO stAdd;
					StrCpyA(stAdd.szName,g_pQueue->GameState.szMainPlayer);
					StrCpyA(stAdd.szServer,g_pQueue->GameState.szServerName);
					stAdd.ServerID=g_pQueue->GameState.ServerID;
					stAdd.Head =g_pQueue->GameState.HeadImgID;
					stAdd.QQ=g_pQueue->GameState.QQ;
					stAdd.LOLID=g_pQueue->GameState.LOLGlobalID;
					stAdd.Level=g_pQueue->GameState.Level;
					SendMessagetoApp(&stAdd,sizeof(stLOCAL_CMD_LOGININFO),LOCAL_CMD_LOGININFO);
					bInfoUpdate=1;
				}


				Sleep(1000);
			}
			//g_Log.AddString("*AirLib threadU Exit");
	
	
	return 1;
}




 bool GameLibReplace()             
 {
	 HMODULE  hmod=GetModuleHandleA("AirLib.dll");
	 CStringA path; 
	 GetModuleFileNameA(hmod,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
	 path.ReleaseBuffer(); 
	 int pos = path.ReverseFind('\\'); 
	 path = path.Left(pos);

	 CStringA szHookDll=path+"\\dbghelp.dll";		//���ڵ�dll
	 CStringA szTempDll=path+"\\cgatemp";
	 CStringA szResDll=path+"\\dbghelp.Bak";//ԭ��DLL

	 if (PathFileExistsA(szResDll)==0)
	 {
		 DbgPrint("AirLib�ļ�������: %s",szResDll);
		 return 0;
	 }
	 /*
	 ��Ϸ����ǰҪ���Ĺ���:
	 1.�滻LOLPATH\Game\LogitechGkey.dllΪ�Ĺ���ע���DLL.(62k)
	 2.��ԭ��DLL���õ�LOLPATH\Game\LogitechGkey.Bak��(59k)
	 */
	 DeleteFileA(szTempDll);
	 if (MoveFileA(szHookDll,szTempDll) ==0) //ע����������ʱ3
	 {
		 DbgPrint("Error AirLib1;");
	 }

	 if (CopyFileA(szResDll,szHookDll,1)==0)//�ָ�ԭ��
	 {
		 DbgPrint("Error AirLib2;");
	 }
	 DbgPrint("AirLib��ԭ���");
	 return 1;
 }
/*

 bool GameLibGo()             
 {
	 HMODULE  hmod=GetModuleHandleA("GameLib.dll");
	 CStringA path; 
	 GetModuleFileNameA(hmod,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
	 path.ReleaseBuffer(); 
	 int pos = path.ReverseFind('\\'); 
	 path = path.Left(pos);


	 CStringA szHookDll=path+"\\dbghelp.dll";
	 CStringA szTempDll=path+"\\cgatemp";
	 CStringA szResDll=path+"\\dbghelp.Bak";//ԭ��DLL


	 if (PathFileExistsA(szResDll)==0)
	 {
		 DbgPrint("GameLibGo�ļ�������: %s",szResDll);
		 return 0;
	 }
	 / *
	 ��Ϸ����ǰҪ���Ĺ���:
	 1.�滻LOLPATH\Game\LogitechGkey.dllΪ�Ĺ���ע���DLL.(62k)
	 2.��ԭ��DLL���õ�LOLPATH\Game\LogitechGkey.Bak��(59k)
	 * /
	 DeleteFileA(szTempDll);
	 if (MoveFileA(szHookDll,szTempDll) ==0) //ע����������ʱ3
	 {
		 DbgPrint("Error GameLibGo1;");
	 }


	 if (CopyFileA(szResDll,szHookDll,1)==0)//�ָ�ԭ��
	 {
		 DbgPrint("Error GameLibGo2;");
	 }


	 return 1;
 }
*/

 bool CheckToolRun()
 {
	 g_GameConfig=(GLOBALCONFIG*)GetMemShare("CGA_LOL_OPTION",sizeof(GLOBALCONFIG));
	 if (g_GameConfig)
	 {
			g_ClientConfig=(LolClientConfig*)((LPBYTE)g_GameConfig + 1024);
			if (g_ClientConfig)
			{
				//
				DbgPrint("CGA_LOL_OPTION�����ھ��= %08x",g_ClientConfig->hwndSingleton);
				if (IsWindow(g_ClientConfig->hwndSingleton)==false)
				{
					DbgPrint("error ��Ч�Ĵ��ھ��,��������ʧ��");
					return 0;
				}
			}else{
				DbgPrint("error g_ClientConfig==0;");
				return 0;
			}
			return 1;
	 }
	 else {
		 return 0;
	 }
 }



 WNDPROC oldProc;
 LRESULT CALLBACK mouseProc (HWND hWnd,UINT nMsg, WPARAM wParam, LPARAM lParam)
 {
	 switch (nMsg)
	 {
	 case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		{
/*
			POINT pt;
			pt.x= LOWORD(lParam); 
			pt.y= HIWORD(lParam);
			if (pt.x>1220 && pt.y<30)
			{
				break;
			}*/

			if ( wParam & 0x80000000)
			{
				break;
			}
		}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		{
			if ( lParam & 0x4000000)
			{
				break;
			}			
		}
	 case WM_LBUTTONDBLCLK:
	 case WM_RBUTTONDBLCLK:
	 case WM_RBUTTONDOWN:
	 case WM_RBUTTONUP:
	 case WM_MOUSEMOVE:
	 case  WM_MBUTTONDOWN:                  
	 case  WM_MBUTTONUP:                    
	 case  WM_MBUTTONDBLCLK:
		 if (g_DisableInput)
		 {
		//	 DbgPrint("Disable Mouse Click");
			 return 0;
		 }
		 break;
	 }
	 //	DbgPrint("WM_DESTROY %x  %x  %x ",nMsg,wParam,lParam);
	 //DbgPrint("mouseProc %d",nMsg);
	 return	CallWindowProc(oldProc,hWnd,nMsg,wParam,lParam);  
 }

 DWORD WINAPI ThreadDisableInPut(LPVOID LPCSTR)
 {
	 DbgPrint("ThreadMenuInPut Run");
	 while(1)
	 {
		 HWND hLOL= FindWindowA(0, "PVP.net �ͻ���");
		// DbgPrint("hLOL= %d ",hLOL);
		 if (hLOL)
		 {
			 oldProc=(WNDPROC)SetWindowLong(hLOL,GWL_WNDPROC,(LONG)mouseProc);
			 break;
		 }

		 Sleep(500);
	 }
	 return 0;
 }




BOOL APIENTRY DllMain_old( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			SetDbgPrintTip("AirLib");
			DbgPrint("AirLib_Match DLL����");
			
		//	RedirectIOToConsole(L"11box_Match");
		//	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TestTheard,NULL,0,NULL);//���Կ�

			/*
			if (GameLibReplace()==0 || CheckToolRun()==0)
			{
				DbgPrint("GameLibGo==0,Dll Exit");
				return 0;
			}
			*/
			InHook();

			//��־��ȡ�߳����ڴ�
			g_pQueue=(stSharePlayerMem*)GetMemShare("CGA_LOL_SHAREDATA",sizeof(stSharePlayerMem));
			if (g_pQueue)
			{
				g_Parser.MemShareInit(g_pQueue);
				CreateThread(0,0,ThreadUpdateLog,0,0,0);//�����߳�

			}else{
				DbgPrint("AirLib_Match g_pQueue==0");
				return 0;
			}


			DbgPrint("AirLib_Match DLL�ѿ�ʼ����");
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


unsigned int MemSearchStr2(unsigned char* pBuffer,unsigned int Len,string szTag)
{
	int Result=-1;
	char* pTag=new char[szTag.length()+10];
	memset(pTag,0,szTag.length()+10);
	int szLen=szTag.length();


	strcpy(pTag,szTag.c_str());
	for (unsigned int i=0;i<Len-szTag.length()-1;i++)
	{
		if (memcmp(pTag,&pBuffer[i],szLen)==0)
		{
			Result=i;
			break;
		}
	}
	delete[] pTag;
	return Result;
}



void __stdcall ParserData(unsigned int LenBuffer,char* Buffer)
{
	//get file path
	static int count = 0;
	time_t t = time(NULL);
	CreateDirectoryA("c:\\amf3", NULL);
	char time_buf[128] = { 0 };
	sprintf(time_buf, "%d_recv_%ld", ++count, t);
	std::string fileName = std::string("c:\\amf3\\") + time_buf + ".rtmp";

	FILE *fp = fopen(fileName.c_str(), "wb");
	unsigned char protoHead = 0x03;		//ץ�����ֵ�
	if (fp) {
		fwrite(&protoHead, 1, 1, fp);
		fwrite(Buffer, 1, LenBuffer, fp);
		fclose(fp);
	}


	if (g_pQueue && LenBuffer>=0xA0 && LenBuffer<=1024*50 && IsBadReadPtr(Buffer,LenBuffer)==0)//�����������ݶ�
	{
		char* pB=new char[LenBuffer+1024];
		memset(pB,0,LenBuffer+1024);
		memcpy(pB,Buffer,LenBuffer);

		DbgPrint("Catch ParserData  len=%d",LenBuffer);
	time(&g_pQueue->GameState.GameEndTime);//���浱ǰʱ��
	enumBUFFTYPE Result=g_Parser.Parser((unsigned char*)pB,LenBuffer);

	if (Result==TYPE_GAMESTATE)
	{
		DbgPrint("*AirLib  ��Ϸ����,����ս��,�Ѿ����Ƶ��ϴ��ڴ�,Size=%d kb",g_pQueue->Buffer.LenBuffer/1024);
		//g_Log.AddString("*AirLib  ��Ϸ����,����ս��,�Ѿ����Ƶ��ϴ��ڴ�");
		//cout<<"��Ϸ����,����ս��"<<endl;
	}else{//����Ϸս��
		
		int Pos=MemSearchStr2((unsigned char*)Buffer,LenBuffer,"\"leaguePoints\":");
		if (Pos!=-1)
		{
			DbgPrint("*AirLib  ParserData leaguePoints");
			char CPY[32]={0};
			memcpy(CPY,&Buffer[Pos],30);
			CStringA sz=CPY;
			DbgPrint("*-Get Point [%s]\r\n",CPY);
			int Begin=sz.Find("\"leaguePoints\":");

			if (Begin!=-1)
			{//ʤ��
				//g_Log.AddString("*AirLib  ParserData leaguePoints1");
				DbgPrint("*AirLib  ParserData leaguePoints1");
				Begin+=15;
				int End=sz.Find(",\"");
				CStringA szNum=sz.Mid(Begin,End-Begin);
				g_pQueue->GameState.RateNumber=atoi(szNum);
				//�ٴ����
				if (g_pQueue->Buffer.LenBuffer)
				{//������,����ʤ����λ	
					//g_Log.AddString("*AirLib  ParserData leaguePoints2");
					unsigned int PosSection=g_pQueue->Buffer.LenBuffer-sizeof(stGameState);
					DbgPrint("PosSection = 0x%x\r\n",PosSection);
					stGameState* pGameState=(stGameState*)&g_pQueue->Buffer.pDataBuffer[PosSection];
					DbgPrint("RateNumber = 0x%x\r\n",pGameState->RateNumber);
					pGameState->RateNumber=g_pQueue->GameState.RateNumber;
					//g_Log.AddString("*AirLib  ParserData leaguePoints3");
				}
			}
		}
	}




	if (pB)
	{
		delete[] pB;
	}
}

}

__declspec(naked) void   __stdcall MyCallData()
{
	__asm{
		pushad;
		pushfd;
		Test eax,eax;
		je Go;
		Test esi,esi;
		je Go;
		push [esi+0x60];
		push [esi+0x64];
		call ParserData;
	Go:
		popfd;
		popad;
		push Hook._HookData_BackAddr;//������Ϸ
		mov ecx,Hook._HookData_CallAddr;
		jmp ecx;//ģ��call
	}
}


CStringA GetServerNameByID(unsigned int ServerID)
	{
		CStringA szServer;
		switch(ServerID)
		{
		case 257:
			szServer="��ŷ����";
			break;
		case 513:
			szServer="�氲";
			break;
		case 769:
			szServer="ŵ����˹";
			break;
		case 1025:
			szServer="��¶���";
			break;
		case 1281:
			szServer="Ƥ�����ַ�";
			break;
		case 1537:
			szServer="ս��ѧԺ";
			break;
		case 1793:
			szServer="�����";
			break;
		case 2049:
			szServer="��ɪ�ر�";
			break;
		case 3073:
			szServer="��������";
			break;
		case 2305:
			szServer="�þ�֮��";
			break;
		case 2561:
			szServer="��ɫõ��";
			break;
		case 2817:
			szServer="��Ӱ��";
			break;
		case 3585:
			szServer="�������";
			break;
		case 3329:
			szServer="ˮ��֮��";
			break;
		case 3841:
			szServer="Ӱ��";
			break;
		case 4097:
			szServer="����֮��";
			break;
		case 4353:
			szServer="����֮��";
			break;
		case 4609:
			szServer="��������";
			break;
		case 4865:
			szServer="Ƥ�ǽ䱸";
			break;
		case 258://��ͨ
			szServer="�ȶ�������";
			break;
		case 514:
			szServer="��������";
			break;
		case 770:
			szServer="���׶�׿��";
			break;
		case 1026:
			szServer="��η�ȷ�";
			break;
		case 1282:
			szServer="ˡ����";
			break;
		case 1538:
			szServer="Ť������";
			break;
		case 1794:
			szServer="����֮��";
			break;
		case 65539://������
			szServer="������ר��";
			break;
		default: szServer="δ֪������";
		}

		return szServer;
	}


void SetServerStr(unsigned int ServerID)
{
	CStringA szServer;
	switch(ServerID)
	{
	case 257:
		szServer="��ŷ����";
		break;
	case 513:
		szServer="�氲";
		break;
	case 769:
		szServer="ŵ����˹";
		break;
	case 1025:
		szServer="��¶���";
		break;
	case 1281:
		szServer="Ƥ�����ַ�";
		break;
	case 1537:
		szServer="ս��ѧԺ";
		break;
	case 1793:
		szServer="�����";
		break;
	case 2049:
		szServer="��ɪ�ر�";
		break;
	case 3073:
		szServer="��������";
		break;
	case 2305:
		szServer="�þ�֮��";
		break;
	case 2561:
		szServer="��ɫõ��";
		break;
	case 2817:
		szServer="��Ӱ��";
		break;
	case 3585:
		szServer="�������";
		break;
	case 3329:
		szServer="ˮ��֮��";
		break;
	case 3841:
		szServer="Ӱ��";
		break;
	case 4097:
		szServer="����֮��";
		break;
	case 4353:
		szServer="����֮��";
		break;
	case 4609:
		szServer="��������";
		break;
	case 4865:
		szServer="Ƥ�ǽ䱸";
		break;
	case 258://��ͨ
		szServer="�ȶ�������";
		break;
	case 514:
		szServer="��������";
		break;
	case 770:
		szServer="���׶�׿��";
		break;
	case 1026:
		szServer="��η�ȷ�";
		break;
	case 1282:
		szServer="ˡ����";
		break;
	case 1538:
		szServer="Ť������";
		break;
	case 1794:
		szServer="����֮��";
		break;
	case 65539://������
		szServer="������ר��";
		break;
	default: szServer="δ֪������";
	}
	if (g_pQueue)
	{
		StrCpyA(&g_pQueue->GameState.szServerName[0],szServer.GetBuffer());
	}
	return;
}

extern "C"  __declspec(dllexport)  void AirLib_Match_Test(unsigned char* buf, int len)
{
	CParserGameState Parser;
	enumBUFFTYPE result = Parser.Parser(buf, len);
	if (TYPE_GAMESTATE == result)
	{
		CStringA szDebugPath = "C:\\1.txt";
		DeleteFileA(szDebugPath);
		CzjlFile DebugFile(szDebugPath);

		CStringA szDebugFile = PrintGameInfo(*Parser.GetData());
		DbgPrint("ս��! %s", szDebugFile);
		DebugFile.WriteStringA(szDebugFile);
		Sleep(1000);
		ShellExecuteA(0, "open", szDebugPath, "", "", SW_SHOWNORMAL);
	}
}

extern "C"  __declspec(dllexport)  bool AirLib_Match_Init()
{
	return 1;
}

