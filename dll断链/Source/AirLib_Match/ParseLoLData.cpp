#include <Windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <Strsafe.h>
#include <time.h>
#include <atltime.h>

#include "ParseLoLData.h"

#include "CzjlHook.h"
#include "atlstr.h"
#include "zHook.h"
#include "LOL/ParserGameState.h"
#include "CEnum.h"
#include "LOL\LOLDef.h"
#include "CzjlSocket.h"
#include "zjlcommon.h"
#include "LLog.h"
#include "LSemaphore.h"
#include "LShareMem.h"
#include "logfile/LoLHelperLocalLog.h"

#pragma warning (disable:4995)

#define  LOL_MEM_SHARE_NAME "JS_LOL_SHAREDATA"

static const char* mutexName = "stSharePlayerMemMutex";
static HANDLE hMutex = NULL;
static stSharePlayerMem pQueue;
static CParserGameState g_Parser;

static BOOL MutextPlayerMemIn()		//互斥访问pQueue g_Parser
{
	if (hMutex == NULL) {
		hMutex = CreateMutexA(NULL, FALSE, mutexName);
		if (hMutex == 0)	return FALSE;
	}

	DWORD ret = WaitForSingleObject(
					hMutex,    // handle to mutex
					INFINITE);  // no time-out interval
	if (ret == WAIT_OBJECT_0) {
		return TRUE;
	}
	return FALSE;
}

static void MutextPlayerMemOut()
{
	if (hMutex == 0)	return;
	ReleaseMutex(hMutex);
}

void MutextPlayerMemDestory()
{
	if (hMutex)
		CloseHandle(hMutex);
}

static unsigned int MemSearchStr2(unsigned char* pBuffer, unsigned int Len, string szTag)
{
	int Result = -1;
	char* pTag = new char[szTag.length() + 10];
	memset(pTag, 0, szTag.length() + 10);
	int szLen = szTag.length();


	strcpy(pTag, szTag.c_str());
	for (unsigned int i = 0; i<Len - szTag.length() - 1; i++)
	{
		if (memcmp(pTag, &pBuffer[i], szLen) == 0)
		{
			Result = i;
			break;
		}
	}
	delete[] pTag;
	return Result;
}

static CStringA MyUTF8ToAnsi(const char* str)
{
	int textlen;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t * Unicode = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
	memset(Unicode, 0, (textlen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)Unicode, textlen);


	textlen = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, Unicode, -1, NULL, 0, NULL, NULL);
	char* result = (char *)malloc((textlen + 1)*sizeof(char));
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, Unicode, -1, result, textlen, NULL, NULL);


	CStringA sz = result;
	delete[] Unicode;
	delete[] result;
	return sz;
}

static CStringA PrintGameInfo(const stLOLGame &GameData)
{
	CStringA szResult, szTemp;
	szResult += "本局游戏信息\r\n";
	szTemp.Format("GameID : %I64u ", GameData.GameID);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameLengh : %d ", GameData.GameLengh);
	szResult += szTemp + "\r\n";

	szTemp.Format("PlayerCount : %d ", GameData.PlayerCount);
	szResult += szTemp + "\r\n";

	szTemp.Format("WinnerTeam : %d ", GameData.WinnerTeam);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameMode : %d ", GameData.GameMode);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameType : %d ", GameData.GameType);
	szResult += szTemp + "\r\n";

	szTemp.Format("GameTypeStr : %s ", GameData.GameTypeStr);
	szResult += szTemp + "\r\n";

	szResult += "上传者信息\r\n";
	szTemp.Format("QQ : %I64u ", GameData.GameState.QQ);
	szResult += szTemp + "\r\n";

	szTemp.Format("ServerID : %u ", GameData.GameState.ServerID);
	szResult += szTemp + "\r\n";

	szTemp.Format("LOLGlobalID : %u ", GameData.GameState.LOLGlobalID);
	szResult += szTemp + "\r\n";

	szTemp.Format("szMainPlayer : %s ", GameData.GameState.szMainPlayer);
	szResult += szTemp + "\r\n";

	szTemp.Format("Level : %d ", GameData.GameState.Level);
	szResult += szTemp + "\r\n";

	CTime time = GameData.GameState.GameEndTime;
	szTemp.Format("GameEndTime : %d-%d-%d %d:%d:%d \r\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	szResult += szTemp + "\r\n";

	szTemp.Format("szRankLevel : %s ", GameData.GameState.szRankLevel);
	szResult += szTemp + "\r\n";

	szTemp.Format("HeadImgID : %d ", GameData.GameState.HeadImgID);
	szResult += szTemp + "\r\n";

	szTemp.Format("RateNumber : %d ", GameData.GameState.RateNumber);
	szResult += szTemp + "\r\n";

	for (unsigned int i = 0; i<LOLMAXPLAYER; i++)
	{
		long long PGID = GameData.Players[i].globalPlayerID;

		if (PGID)
		{
			szResult += "______________________________\r\n";
			szTemp.Format("Index : %d ", i + 1);
			szResult += szTemp + "\r\n";

			szTemp.Format("TEAM : %d ", GameData.Players[i].TEAM);
			szResult += szTemp + "\r\n";

			szTemp.Format("Win? : %d ", GameData.Players[i].Win);
			szResult += szTemp + "\r\n";

			szTemp.Format("globalPlayerID : %I64u ", PGID);
			szResult += szTemp + "\r\n";

			szTemp.Format("szPlayerName : %s ", GameData.Players[i].szPlayerName);
			szResult += szTemp + "\r\n";

			szTemp.Format("HeroID : %d ", GameData.Players[i].HeroID);
			szResult += szTemp + "\r\n";

			szTemp.Format("szHeroName : %s ", GameData.Players[i].szHeroName);
			szResult += szTemp + "\r\n";

			szTemp.Format("LEVEL : %d ", GameData.Players[i].LEVEL);
			szResult += szTemp + "\r\n";

			szTemp.Format("Kill : %d ", GameData.Players[i].CHAMPIONS_KILLED);
			szResult += szTemp + "\r\n";

			szTemp.Format("Dead : %d ", GameData.Players[i].NUM_DEATHS);
			szResult += szTemp + "\r\n";

			szTemp.Format("ASSISTS : %d ", GameData.Players[i].ASSISTS);
			szResult += szTemp + "\r\n";

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
			szResult += szTemp + "\r\n";

			szTemp.Format("造成伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_DEALT);
			szResult += szTemp + "\r\n";

			szTemp.Format("承受伤害 : %d ", GameData.Players[i].TOTAL_DAMAGE_TAKEN);
			szResult += szTemp + "\r\n";

			szTemp.Format("ITEM0 : %d ", GameData.Players[i].ITEM0);
			szResult += szTemp + "\r\n";

			szTemp.Format("ITEM1 : %d ", GameData.Players[i].ITEM1);
			szResult += szTemp + "\r\n";
		}
	}

	return szResult;
}

void __stdcall ParserLog(char* pA1)
{
	Log(__FUNCTION__);

	if (MutextPlayerMemIn() == FALSE) {
		LoLLocalLogErr("ParserLog call MutextPlayerMemIn  Fail");
		return;
	}
	if (IsBadReadPtr(pA1, 1) == 0)
	{
		CStringA szUtf8 = pA1;
		CStringA szPath;
		CStringA sz = MyUTF8ToAnsi(szUtf8.GetBuffer());

#if 1//LIN_DBG
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
#endif
		LoLLocalLogInfo("=====================ParserLog running");

		int _begin = 0, _end = 0;
		if (sz.Left(4) == "ime=")
		{//获取QQ号与服务器
			_begin = sz.Find("uin=");
			_begin += 4;
			_end = sz.Find("&area=");
			long long QQ;
			unsigned int Server;
			if (_end != -1 && _begin != -1)
			{
				CStringA szNumber = sz.Mid(_begin, _end - _begin);
				QQ = _atoi64(szNumber);
				LoLLocalLogInfo("=====================拿到QQ号了 %s ", szNumber.GetBuffer(0));
			}

			if (_end != -1 && _begin != -1)
			{
				_begin = _end + 6;
				_end = sz.Find("&timestamp=");
				CStringA szServer = sz.Mid(_begin, _end - _begin);
				Server = atoi(szServer);

				LoLLocalLogInfo("=====================拿到服务器名字了 %s ", szServer.GetBuffer(0));
			}
			
			pQueue.GameState.QQ = QQ;
			pQueue.GameState.ServerID = Server;
		}


		_begin = sz.Find("<summoner_name>");
		if (_begin != -1)
		{//获取账号名
			_begin += 15;
			CStringA szName;
			_end = sz.Find("</summoner_name>", _begin);
			if (_end != -1)
			{
				szName = sz.Mid(_begin, _end - _begin);

				StrCpyA(pQueue.GameState.szMainPlayer, szName.GetBuffer());

				LoLLocalLogInfo("=====================拿到帐号名了 %s ", szName.GetBuffer(0));
			}
		}

		_begin = sz.Find("<jid>sum");
		if (_begin != -1)
		{
			_begin += 8;
			CStringA szGlobalID;
			_end = sz.Find("@pvp.net/xiff</jid>", _begin);
			if (_end != -1)
			{//唯一ID
				szGlobalID = sz.Mid(_begin, _end - _begin);
				pQueue.GameState.LOLGlobalID = _atoi64(szGlobalID);

				LoLLocalLogInfo("=====================拿到唯一ID了 %s ", szGlobalID.GetBuffer(0));
			}
		}

		_begin = sz.Find("</show><status>");
		if (_begin != -1)
		{
			CStringA szLevel, szRank;

			_begin = sz.Find("level&gt;");
			if (_begin != -1)
			{
				_begin += 9;
				_end = sz.Find("&lt;", _begin);

				if (_end != -1)
				{
					szLevel = sz.Mid(_begin, _end - _begin);
				}
			}
		}

		_begin = sz.Find("<presence to='sum");
		if (_begin != -1)
		{//RANK
			_begin += 17;
			CStringA szCmp1, szCmp2;

			_end = sz.Find("@pvp.net", _begin);
			if (_end != -1)
			{
				szCmp1 = sz.Mid(_begin, _end - _begin);
			}

			_begin = sz.Find("from='sum", _end);
			if (_begin != -1)
			{
				_begin += 9;
				_end = sz.Find("@pvp.net", _begin);
				if (_end != -1)
				{
					szCmp2 = sz.Mid(_begin, _end - _begin);
				}
			}
			if (szCmp1 == szCmp2)
			{//自己的rank积分,提取之

				_begin = sz.Find("</show><status>");
				if (_begin != -1)
				{
					CStringA szLevel, szTier, szDivision, szHeadImg;

					_begin = sz.Find("profileIcon&gt;");
					if (_begin != -1)
					{
						_begin += 15;
						_end = sz.Find("&lt;", _begin);

						if (_end != -1)
						{
							szHeadImg = sz.Mid(_begin, _end - _begin);
						}
					}


					_begin = sz.Find("level&gt;");
					if (_begin != -1)
					{
						_begin += 9;
						_end = sz.Find("&lt;", _begin);

						if (_end != -1)
						{
							szLevel = sz.Mid(_begin, _end - _begin);
						}
					}

					//  rankedLeagueTier&gt;DIAMOND&lt/;
					_begin = sz.Find("rankedLeagueTier&gt;");
					if (_begin != -1)
					{
						_begin += 20;
						_end = sz.Find("&lt;", _begin);

						if (_end != -1)
						{
							szTier = sz.Mid(_begin, _end - _begin);
						}
					}
					_begin = sz.Find("Division&gt;");
					if (_begin != -1)
					{
						_begin += 12;
						_end = sz.Find("&lt;/rankedLeagueDivision", _begin);

						if (_end != -1)
						{
							szDivision = sz.Mid(_begin, _end - _begin);
						}
					}

					if (szLevel != "")
					{
						pQueue.GameState.Level = atoi(szLevel);
						LoLLocalLogInfo("=====================拿到level了 %s ", szLevel.GetBuffer(0));
					}

					if (szHeadImg != "")
					{
						pQueue.GameState.HeadImgID = atoi(szHeadImg);

						LoLLocalLogInfo("=====================拿到szHeadImg了 %s ", szHeadImg.GetBuffer(0));
					}

					if (szDivision != "" && szTier != "")
					{
						CStringA szRank;
						szRank.Format("%s_%s", szTier, szDivision);
						StrCpyA(pQueue.GameState.szRankLevel, szRank);

						LoLLocalLogInfo("=====================拿到szRank了 %s ", szRank.GetBuffer(0));
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

	MutextPlayerMemOut();
	Log("parse log out");
}

void __stdcall ParserData(unsigned int LenBuffer, char* Buffer)
{
#if 1//LIN_DBG
	Log(__FUNCTION__);

	static int count = 0;
	time_t t = time(NULL);
	CreateDirectoryA("c:\\amf3", NULL);
	char time_buf[128] = { 0 };
	sprintf(time_buf, "%d_%ld", ++count, t);
	std::string fileName = std::string("c:\\amf3\\") + time_buf + ".rtmp";

	FILE *fp = fopen(fileName.c_str(), "wb");
	if (fp) {
		fwrite(Buffer, 1, LenBuffer, fp);
		fclose(fp);
	}
#endif 

	LoLLocalLogInfo("ParserData is running 1 %d %x", LenBuffer, Buffer);
	if (MutextPlayerMemIn() == FALSE) {
		LoLLocalLogErr("ParserData call MutextPlayerMemIn  Fail");
		return;
	}

	LoLLocalLogInfo("ParserData is running 2");
	if (LenBuffer >= 0xA0 && LenBuffer <= 1024 * 50 && IsBadReadPtr(Buffer, LenBuffer) == 0)//跳过无用数据段
	{
		char* pB = new char[LenBuffer + 1024];
		if (pB == 0)	{
			MutextPlayerMemOut();
			LoLLocalLogErr("ParserData call new char Fail");
			return;
		}
		memset(pB, 0, LenBuffer + 1024);
		memcpy(pB, Buffer, LenBuffer);

		DbgPrint("Catch ParserData  len=%d",LenBuffer);
		time(&pQueue.GameState.GameEndTime);//保存当前时间
		g_Parser.MemShareInit(&pQueue);
		enumBUFFTYPE Result=g_Parser.Parser((unsigned char*)pB,LenBuffer);

		LoLLocalLogInfo("parse data %d\n", Result);
		if (Result==TYPE_GAMESTATE)
		{
#if LIN_DBG
			//游戏结果
			CStringA szDebugPath = "C:\\1.txt";
			DeleteFileA(szDebugPath);
			CzjlFile DebugFile(szDebugPath);

			CStringA szDebugFile = PrintGameInfo(*g_Parser.GetData());
			DbgPrint("战绩! %s", szDebugFile);
			DebugFile.WriteStringA(szDebugFile);
			Sleep(1000);
			ShellExecuteA(0, "open", szDebugPath, "", "", SW_SHOWNORMAL);
#endif

			//把结果写入共享数据
			Log("LoLMemShareAccessIn before");
			LoLLocalLogInfo("ParserData LoLMemShareAccessIn before");
			HANDLE hMemShareAcc = LoLMemShareAccessIn();
			if (hMemShareAcc != 0) {

				HANDLE hMem = NULL;
				Log("LoLMemShareMap before");
				stLOLShareMem *game = (stLOLShareMem*)LoLMemShareMap(&hMem, sizeof(stLOLShareMem));	//初始化大小应该和这个一致
				if (game != 0)	{
					Log("LoLMemShareMap success");
					memcpy(&game->game, g_Parser.GetData(), sizeof(stLOLShareMem));
					game->ShareDataReady |= ShareDataReadyType_Ready;

					LoLLocalLogInfo("ParserData ShareDataReady ShareDataReadyType_Ready");

					LoLMemShareUnMap(game, hMem);
				}
				Log("LoLMemShareAccessOut before");
				LoLMemShareAccessOut(hMemShareAcc);
				Log("LoLMemShareMap after");
			}
		}else{//非游戏战绩

			int Pos=MemSearchStr2((unsigned char*)Buffer,LenBuffer,"\"leaguePoints\":");
			if (Pos!=-1)
			{
				DbgPrint("*AirLib  ParserData leaguePoints");
				char CPY[32] = { 0 };
				memcpy(CPY, &Buffer[Pos], 30);
				CStringA sz = CPY;
				DbgPrint("*-Get Point [%s]\r\n", CPY);
				int Begin = sz.Find("\"leaguePoints\":");

				if (Begin != -1)
				{//胜点
					//g_Log.AddString("*AirLib  ParserData leaguePoints1");
					DbgPrint("*AirLib  ParserData leaguePoints1");
					Begin += 15;
					int End = sz.Find(",\"");
					CStringA szNum = sz.Mid(Begin, End - Begin);
					pQueue.GameState.RateNumber = atoi(szNum);
					//再次填充
					if (pQueue.Buffer.LenBuffer)
					{//已填充过,覆盖胜点标记位	
						//g_Log.AddString("*AirLib  ParserData leaguePoints2");
						unsigned int PosSection = pQueue.Buffer.LenBuffer - sizeof(stGameState);
						DbgPrint("PosSection = 0x%x\r\n", PosSection);
						stGameState* pGameState = (stGameState*)&pQueue.Buffer.pDataBuffer[PosSection];
						DbgPrint("RateNumber = 0x%x\r\n", pGameState->RateNumber);
						pGameState->RateNumber = pQueue.GameState.RateNumber;
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


	MutextPlayerMemOut();
	Log("parse data out");
}

#if 0   //访问共享内存保护代码片段
HANDLE hMemShareAcc = LoLMemShareAccessIn();
if (hMemShareAcc == 0) return;

HANDLE hMem = NULL;
stSharePlayerMem *pQueue = (stSharePlayerMem*)LoLMemShareMap(&hMem, sizeof(stSharePlayerMem));	//初始化大小应该和这个一致
if (pQueue == 0)	{
	LoLMemShareAccessOut(hMemShareAcc);
	return;
}



LoLMemShareUnMap(pQueue, hMem);
LoLMemShareAccessOut(hMemShareAcc);
#endif
