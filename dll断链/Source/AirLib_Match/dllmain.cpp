// dllmain.cpp : 定义 DLL 应用程序的入口点。
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
	int _HookData_GotoAddr;//hook地址
	int _HookData_BackAddr;//跳回来的地址
	int _HookData_CallAddr;//原来call的地址
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
		//保存原来的5字节
		memcpy(Hook.CmpMemLog,(void*)Hook._HookLog_GotoAddr,5);
		Hook.HookLog.HookAddrToMyFn_Start();
	}else{
		DbgPrint("MyCallLog hook失败");
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
		int g=*((int*)(Hook._HookData_GotoAddr+1));//原来call偏移
		Hook._HookData_CallAddr=g+Hook._HookData_BackAddr;//原来call地址
		//		zjlDebugBoxOutHex("_HookData_CallAddr",Hook._HookData_CallAddr);
		Hook.HookData.HookAddrToMyFn((LPVOID)(Hook._HookData_GotoAddr),MyCallData);
		DbgPrint("AirLib_Match Data 0x%08x\r\n",Hook._HookData_GotoAddr);
		//保存原来的5字节
		memcpy(Hook.CmpMemData,(void*)Hook._HookData_GotoAddr,5);
		CreateThread(0,0,ThreadReplace,0,0,0);
		Hook.HookData.HookAddrToMyFn_Start();
	}else{
		DbgPrint("MyCallData hook失败");
		//g_Log.AddString("*AirLib Error MyData");
//		zjlDebugBoxOutHex("_HookData_CallAddr",Hook._HookData_CallAddr);
	}	
	DbgPrint("提取LOL数据模块hook完毕,已开始工作");
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
DbgPrint("ParserLog 保存到 %s",szPath);
CzjlFile file(szPath);
file.WriteStringA(sz);*/



		int _begin=0,_end=0;
		if (sz.Left(4)=="ime=")
		{//获取QQ号与服务器
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
		{//获取账号名
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
			{//唯一ID
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
					//输出
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
			{//自己的rank积分,提取之

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
		long long QQ;			  //QQ号
		long long LOLGlobalID;//唯一ID
		char szMainPlayer[128];//玩家名

		unsigned int Level;	//账号等级
		char szRankLevel[128];		//排位等级

		unsigned int ServerID;//服务器ID
		char szServerName[128];//服务器名
		char szVersion[128];	//服务器版本
		time_t GameEndTime;//游戏结束的时间,减去游戏长度就是游戏开始时间

		//2014-11-12新加2个字段
		unsigned int HeadImgID;		//玩家头像ID*/

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
	DbgPrint("比赛数据信息:***********************************");
	DbgPrint("比赛模式 %d",g_ClientConfig->Match.bMatch);
	DbgPrint("创建房间 %d",g_ClientConfig->Match.bCreate);
	DbgPrint("地图 %d",g_ClientConfig->Match.MapIndex);
	DbgPrint("观看模式 %d",g_ClientConfig->Match.ViewIndex);
	DbgPrint("比赛模式 %d",g_ClientConfig->Match.GameMode);
	DbgPrint("是否手动 %d",g_ClientConfig->Match.bManual);
	DbgPrint("服务器 %d %s",g_ClientConfig->Match.ServerID,GetServerNameByID(g_ClientConfig->Match.ServerID));
	DbgPrint("房间名 %s",g_ClientConfig->Match.szRoomName);
	DbgPrint("密码 %s",g_ClientConfig->Match.szPassword);
	DbgPrint("队伍 %d",g_ClientConfig->Match.bOwnerCamp);
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
	//g_Log.AddString("*AirLib 开始登陆!");
	for (unsigned int i=0;true;i++)
	{
		if ( g_pAccount->account[0] 	 && g_pAccount->IP[0] && g_pAccount->Port)
		{
			CStringW sW=g_pAccount->account;
			CStringA sA=sW+"账号登陆中";
			//g_Log.AddString(sA);
			sA.Format("连接服务器IP : %s :%d",g_pAccount->IP,g_pAccount->Port);
			//g_Log.AddString(sA);
			if (g_Client.Open(g_pAccount->account,"","","",g_pAccount->ST,g_pAccount->IP,g_pAccount->Port))
			{
				break;//登陆成功
			}else{
				//g_Log.AddString("*AirLib 登陆失败!\r\n");
				memset(g_pAccount->account,0,16);
				memset(g_pAccount->ST	,0,128);
				Sleep(1000*2);
			}

		}else{
			printf("*AirLib 客户端未登陆账号\r\n");
			//g_Log.AddString("*AirLib 客户端未登陆账号!\r\n");
			Sleep(1000*10);
		}
	}
	//g_Log.AddString("*AirLib 登陆成功!");
}
*/



int RecvMessagetoApp(int Commandid)
{
//	DbgPrint("接受主程序通知....");
	char pRecvBuffer[1024]={0};
	int irecv=g_Socket.RecvData(pRecvBuffer,1024);
//	DbgPrint("接受数据包size=%d",irecv);
	if (irecv!=-1 && irecv!=0)
	{
		if (irecv>=sizeof(LocalCmdHeader))
		{
			byte bResu= ((stLOCAL_CMD_MATCH_MSG*)&pRecvBuffer[sizeof(LocalCmdHeader)])->bSucc;
			DbgPrint("接收到命令ID %d  Suc=%d ",((LocalCmdHeader*)pRecvBuffer)->nCmdId),bResu;
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
	szResult+="本局游戏信息\r\n";
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

	szResult+="上传者信息\r\n";
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

			szTemp.Format("小兵击杀 : %d ", GameData.Players[i].MINIONS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("兵营 : %d ", GameData.Players[i].BARRACKS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("拆塔 : %d ", GameData.Players[i].TURRETS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("插眼 : %d ", GameData.Players[i].WARD_PLACED);
			szResult += szTemp + "\r\n";

			szTemp.Format("排眼 : %d ", GameData.Players[i].WARD_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("最高多杀 : %d ", GameData.Players[i].LARGEST_MULTI_KILL);
			szResult += szTemp + "\r\n";

			szTemp.Format("最高连杀 : %d ", GameData.Players[i].LARGEST_KILLING_SPREE);
			szResult += szTemp + "\r\n";

			szTemp.Format("金钱 : %d ", GameData.Players[i].GOLD_EARNED);
			szResult+=szTemp+"\r\n";

			szTemp.Format("造成伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_DEALT);
			szResult+=szTemp+"\r\n";

			szTemp.Format("承受伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_TAKEN);
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

	//DbgPrint("开始给主程序发送数据包");

	int SendBufSize=Len+sizeof(LocalCmdHeader);

	char* pSendBuffer=new char[SendBufSize];
	if (pSendBuffer)
	{
		((LocalCmdHeader*)(pSendBuffer))->nLen=SendBufSize;
		((LocalCmdHeader*)(pSendBuffer))->nCmdId=Commadnid;

		memcpy(pSendBuffer+sizeof(LocalCmdHeader),pBuffer,Len);
		int SendByte=g_Socket.SendData(pSendBuffer,SendBufSize);		
		DbgPrint("开始给主程序发送数据包,长度: %d字节 命令ID: %d ,已发送 %d字节",SendBufSize,Commadnid,SendByte);

		delete[] pSendBuffer;
		if (SendByte==0 || SendByte==-1)
		{
			return 0;
		}
	}else{
		DbgPrint("分配内存失败 %d 字节",SendBufSize);
		return 0;
	}

	//DbgPrint("开始给主程序发送数据包 End");
}

bool bInfoUpdate=0;
DWORD WINAPI ThreadUpdateLog(LPVOID lp)
{
	//g_Log.AddString("*AirLib ThreadUpdateLog!");
	DbgPrint("给主程序传输战绩的子线程已启动");
			time_t	t;
			time(&t);
			//Test
			while(g_pQueue)
			{
				//战绩部分
				if (g_pQueue->Buffer.LenBuffer)
				{
					DbgPrint("捕获到游戏战绩,准备上传给主程序 Len=0x%08x \r\n",g_pQueue->Buffer.LenBuffer);
					CParserGameState Parser;
					enumBUFFTYPE result=Parser.Parser((unsigned char*)g_pQueue->Buffer.pDataBuffer,g_pQueue->Buffer.LenBuffer);
					if (TYPE_GAMESTATE==result)
					{
						CStringA szDebugPath="C:\\1.txt";
						DeleteFileA(szDebugPath);
						CzjlFile DebugFile(szDebugPath);

						CStringA szDebugFile=PrintGameInfo(*Parser.GetData());
						DbgPrint("战绩! %s",szDebugFile);
						DebugFile.WriteStringA(szDebugFile);
						Sleep(1000);
						ShellExecuteA(0,"open",szDebugPath,"","",SW_SHOWNORMAL);
					}
/*
					CStringA szOut;
					szOut.Format("捕获到游戏战绩,准备上传给主程序 Len=0x%08x \r\n",g_pQueue->Buffer.LenBuffer);
					//g_Log.AddString(szOut);
					Sleep(1000*5);//等待胜点数据填充
					//调试输出	
					SendMessagetoApp(g_pQueue->Buffer.pDataBuffer,g_pQueue->Buffer.LenBuffer,LOCAL_CMD_RESULT);
					//////////////////////////////////////////////////////////////////////////			
					Sleep(1000*5);
					//g_Log.AddString("*AirLib BufferClear OK! \r\n");*/

					g_pQueue->Buffer.LenBuffer=0;
					memset(g_pQueue->Buffer.pDataBuffer,0,1024*20);		
				}

				//头像部分
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

	 CStringA szHookDll=path+"\\dbghelp.dll";		//现在的dll
	 CStringA szTempDll=path+"\\cgatemp";
	 CStringA szResDll=path+"\\dbghelp.Bak";//原版DLL

	 if (PathFileExistsA(szResDll)==0)
	 {
		 DbgPrint("AirLib文件不存在: %s",szResDll);
		 return 0;
	 }
	 /*
	 游戏启动前要做的工作:
	 1.替换LOLPATH\Game\LogitechGkey.dll为改过的注入版DLL.(62k)
	 2.把原版DLL放置到LOLPATH\Game\LogitechGkey.Bak下(59k)
	 */
	 DeleteFileA(szTempDll);
	 if (MoveFileA(szHookDll,szTempDll) ==0) //注入版改名到临时3
	 {
		 DbgPrint("Error AirLib1;");
	 }

	 if (CopyFileA(szResDll,szHookDll,1)==0)//恢复原版
	 {
		 DbgPrint("Error AirLib2;");
	 }
	 DbgPrint("AirLib还原完毕");
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
	 CStringA szResDll=path+"\\dbghelp.Bak";//原版DLL


	 if (PathFileExistsA(szResDll)==0)
	 {
		 DbgPrint("GameLibGo文件不存在: %s",szResDll);
		 return 0;
	 }
	 / *
	 游戏启动前要做的工作:
	 1.替换LOLPATH\Game\LogitechGkey.dll为改过的注入版DLL.(62k)
	 2.把原版DLL放置到LOLPATH\Game\LogitechGkey.Bak下(59k)
	 * /
	 DeleteFileA(szTempDll);
	 if (MoveFileA(szHookDll,szTempDll) ==0) //注入版改名到临时3
	 {
		 DbgPrint("Error GameLibGo1;");
	 }


	 if (CopyFileA(szResDll,szHookDll,1)==0)//恢复原版
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
				DbgPrint("CGA_LOL_OPTION主窗口句柄= %08x",g_ClientConfig->hwndSingleton);
				if (IsWindow(g_ClientConfig->hwndSingleton)==false)
				{
					DbgPrint("error 无效的窗口句柄,程序启动失败");
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
		 HWND hLOL= FindWindowA(0, "PVP.net 客户端");
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
			DbgPrint("AirLib_Match DLL启动");
			
		//	RedirectIOToConsole(L"11box_Match");
		//	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TestTheard,NULL,0,NULL);//测试开

			/*
			if (GameLibReplace()==0 || CheckToolRun()==0)
			{
				DbgPrint("GameLibGo==0,Dll Exit");
				return 0;
			}
			*/
			InHook();

			//日志提取线程与内存
			g_pQueue=(stSharePlayerMem*)GetMemShare("CGA_LOL_SHAREDATA",sizeof(stSharePlayerMem));
			if (g_pQueue)
			{
				g_Parser.MemShareInit(g_pQueue);
				CreateThread(0,0,ThreadUpdateLog,0,0,0);//监听线程

			}else{
				DbgPrint("AirLib_Match g_pQueue==0");
				return 0;
			}


			DbgPrint("AirLib_Match DLL已开始运行");
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
	unsigned char protoHead = 0x03;		//抓包发现的
	if (fp) {
		fwrite(&protoHead, 1, 1, fp);
		fwrite(Buffer, 1, LenBuffer, fp);
		fclose(fp);
	}


	if (g_pQueue && LenBuffer>=0xA0 && LenBuffer<=1024*50 && IsBadReadPtr(Buffer,LenBuffer)==0)//跳过无用数据段
	{
		char* pB=new char[LenBuffer+1024];
		memset(pB,0,LenBuffer+1024);
		memcpy(pB,Buffer,LenBuffer);

		DbgPrint("Catch ParserData  len=%d",LenBuffer);
	time(&g_pQueue->GameState.GameEndTime);//保存当前时间
	enumBUFFTYPE Result=g_Parser.Parser((unsigned char*)pB,LenBuffer);

	if (Result==TYPE_GAMESTATE)
	{
		DbgPrint("*AirLib  游戏结束,捕获到战绩,已经复制到上传内存,Size=%d kb",g_pQueue->Buffer.LenBuffer/1024);
		//g_Log.AddString("*AirLib  游戏结束,捕获到战绩,已经复制到上传内存");
		//cout<<"游戏结束,捕获到战绩"<<endl;
	}else{//非游戏战绩
		
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
			{//胜点
				//g_Log.AddString("*AirLib  ParserData leaguePoints1");
				DbgPrint("*AirLib  ParserData leaguePoints1");
				Begin+=15;
				int End=sz.Find(",\"");
				CStringA szNum=sz.Mid(Begin,End-Begin);
				g_pQueue->GameState.RateNumber=atoi(szNum);
				//再次填充
				if (g_pQueue->Buffer.LenBuffer)
				{//已填充过,覆盖胜点标记位	
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
		push Hook._HookData_BackAddr;//返回游戏
		mov ecx,Hook._HookData_CallAddr;
		jmp ecx;//模拟call
	}
}


CStringA GetServerNameByID(unsigned int ServerID)
	{
		CStringA szServer;
		switch(ServerID)
		{
		case 257:
			szServer="艾欧尼亚";
			break;
		case 513:
			szServer="祖安";
			break;
		case 769:
			szServer="诺克萨斯";
			break;
		case 1025:
			szServer="班德尔城";
			break;
		case 1281:
			szServer="皮尔特沃夫";
			break;
		case 1537:
			szServer="战争学院";
			break;
		case 1793:
			szServer="巨神峰";
			break;
		case 2049:
			szServer="雷瑟守备";
			break;
		case 3073:
			szServer="钢铁烈阳";
			break;
		case 2305:
			szServer="裁决之地";
			break;
		case 2561:
			szServer="黑色玫瑰";
			break;
		case 2817:
			szServer="暗影岛";
			break;
		case 3585:
			szServer="均衡教派";
			break;
		case 3329:
			szServer="水晶之痕";
			break;
		case 3841:
			szServer="影流";
			break;
		case 4097:
			szServer="守望之海";
			break;
		case 4353:
			szServer="征服之海";
			break;
		case 4609:
			szServer="卡拉曼达";
			break;
		case 4865:
			szServer="皮城戒备";
			break;
		case 258://网通
			szServer="比尔吉沃特";
			break;
		case 514:
			szServer="德玛西亚";
			break;
		case 770:
			szServer="弗雷尔卓德";
			break;
		case 1026:
			szServer="无畏先锋";
			break;
		case 1282:
			szServer="恕瑞玛";
			break;
		case 1538:
			szServer="扭曲丛林";
			break;
		case 1794:
			szServer="巨龙之巢";
			break;
		case 65539://教育网
			szServer="教育网专区";
			break;
		default: szServer="未知服务器";
		}

		return szServer;
	}


void SetServerStr(unsigned int ServerID)
{
	CStringA szServer;
	switch(ServerID)
	{
	case 257:
		szServer="艾欧尼亚";
		break;
	case 513:
		szServer="祖安";
		break;
	case 769:
		szServer="诺克萨斯";
		break;
	case 1025:
		szServer="班德尔城";
		break;
	case 1281:
		szServer="皮尔特沃夫";
		break;
	case 1537:
		szServer="战争学院";
		break;
	case 1793:
		szServer="巨神峰";
		break;
	case 2049:
		szServer="雷瑟守备";
		break;
	case 3073:
		szServer="钢铁烈阳";
		break;
	case 2305:
		szServer="裁决之地";
		break;
	case 2561:
		szServer="黑色玫瑰";
		break;
	case 2817:
		szServer="暗影岛";
		break;
	case 3585:
		szServer="均衡教派";
		break;
	case 3329:
		szServer="水晶之痕";
		break;
	case 3841:
		szServer="影流";
		break;
	case 4097:
		szServer="守望之海";
		break;
	case 4353:
		szServer="征服之海";
		break;
	case 4609:
		szServer="卡拉曼达";
		break;
	case 4865:
		szServer="皮城戒备";
		break;
	case 258://网通
		szServer="比尔吉沃特";
		break;
	case 514:
		szServer="德玛西亚";
		break;
	case 770:
		szServer="弗雷尔卓德";
		break;
	case 1026:
		szServer="无畏先锋";
		break;
	case 1282:
		szServer="恕瑞玛";
		break;
	case 1538:
		szServer="扭曲丛林";
		break;
	case 1794:
		szServer="巨龙之巢";
		break;
	case 65539://教育网
		szServer="教育网专区";
		break;
	default: szServer="未知服务器";
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
		DbgPrint("战绩! %s", szDebugFile);
		DebugFile.WriteStringA(szDebugFile);
		Sleep(1000);
		ShellExecuteA(0, "open", szDebugPath, "", "", SW_SHOWNORMAL);
	}
}

extern "C"  __declspec(dllexport)  bool AirLib_Match_Init()
{
	return 1;
}

