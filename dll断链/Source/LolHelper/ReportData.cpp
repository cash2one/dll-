#include "stdafx.h"
#include "ReportData.h"
#include "HttpPost.h"
#include "llog.h"
#include "logfile/LoLHelperLocalLog.h"
#include "zjlcommon.h"

static Json::Value gLoginData;

static Json::Value MakeJsonGameData(const stLOLGame &GameData);
static Json::Value MakeJsonLoginData();
static Json::Value MakeJsonMsgLoginData();

static const char * gLoginDataParamName = "loginData";
static const char * gGameDataParamName = "gameData";
static const char * gMsgParamName = "msgData";
//static const char * gReportDataInterfaceUrl = "http://10.34.39.131:8080/lol-data-report.html";
//static const char * gReportLogInterfaceUrl = "http://10.34.39.131:8080/client-report-msg.html";  // 

static const char * gReportDataInterfaceUrl = "http://collection.96ni.net/lol-data-report.html";
static const char * gReportLogInterfaceUrl = "http://collection.96ni.net/client-report-msg.html";  

static BOOL GetReportDataHead(Json::Value & root);
static BOOL GetReportLogHead(Json::Value & root);

static BOOL ReadingFileGetDataHead(Json::Value & root);
static BOOL ReadingFileGetLogHead(Json::Value & root);

BOOL ReportData(const stLOLGame &GameData)
{
	//检查是否拿到了gLoginData
	Json::Value loginDataJson;
	BOOL bRet1 = FALSE, bRet2 = FALSE;

	bRet1 = GetReportDataHead(loginDataJson);
	bRet2 = ReadingFileGetDataHead(loginDataJson);

	if (bRet2 == FALSE && bRet1 == FALSE) {
		std::string str = "上报游戏数据的时候获取Head和UUID失败";
		ReportLog(ReportGetReportHeadFail, str);
		LoLLocalLogInfo("%s", str.c_str());
		return FALSE;
	}
	
	std::string param = "";
	Json::FastWriter writer;
	
	//构建loginData 
	param = std::string(gLoginDataParamName) + "=" + writer.write(loginDataJson);
	Log("==========loginData");
	Log(writer.write(loginDataJson).c_str());

	//构建gameData
	Json::Value gameDataJson = MakeJsonGameData(GameData);
	param = param + "&" + gGameDataParamName + "=" + writer.write(gameDataJson);
	Log((gameDataJson.toStyledString()).c_str());

	LoLLocalLogInfo("%s", param.c_str());

	//Post Http
	std::string rs = "";
	DEVWEB::RequestParam rp;
	rp.bBlock = false;
	rp.uiTimeout = 5000;
	rp.bPost = true;
	rp.pPostData = param.c_str();
	rp.uiPostLen = param.length();
	if (DEVWEB::WebRequest(gReportDataInterfaceUrl, rs, &rp))
	{
		rs = DEVWEB::utf2ansi((const char*)rs.c_str());
		bool bRet = DEVWEB::IsResult200(rs);
		Log(rs.c_str());

		if (bRet != true) {
			LoLLocalLogInfo("发送 POST HTTP GAMEDATA 失败,重新尝试");

			DEVWEB::RequestParam rpAgain;
			rpAgain.bPost = true;
			rpAgain.pPostData = param.c_str();
			rpAgain.uiPostLen = param.length();
			rs = "";
			if (DEVWEB::WebRequest(gReportDataInterfaceUrl, rs, &rpAgain))
			{
				rs = DEVWEB::utf2ansi((const char*)rs.c_str());
				bRet = DEVWEB::IsResult200(rs);
				if (bRet != true) {
					LoLLocalLogInfo("发送 POST HTTP GAMEDATA 失败");
				}
			}
			else {
				LoLLocalLogInfo("上报游戏数据成功");
			}
		}
		else {
			LoLLocalLogInfo("上报游戏数据成功");
		}
	}

	return TRUE;
}

BOOL ReportLog(unsigned int type, const std::string &msg )
{
	//检查是否拿到了loginData
	Json::Value msgloginJson;
	if (ReadingFileGetLogHead(msgloginJson)) {

	}
	else if (GetReportLogHead(msgloginJson) == FALSE){		//没有拿到gLoginData
		LoLLocalLogErr("GetReportDataHead fail:%d", type);
		return FALSE;
	}

	Json::Value root;
	std::string param = "";
	Json::FastWriter writer;

	//构建loginData 
	
	param = std::string(gLoginDataParamName) + "=" + writer.write(msgloginJson);
	Log("==========loginData");
	Log(writer.write(msgloginJson).c_str());

	//构建gameData
	Json::Value msgDataJson;
	msgDataJson["type"] = type;
	msgDataJson["msg"] = msg;
	param = param + "&" + gMsgParamName + "=" + writer.write(msgDataJson);
	Log(msgDataJson.toStyledString().c_str());

	LoLLocalLogInfo("%s", param.c_str());
	//Post Http
	std::string rs = "";
	DEVWEB::RequestParam rp;
	rp.bBlock = false;
	rp.uiTimeout = 5000;
	rp.bPost = true;
	rp.pPostData = param.c_str();
	rp.uiPostLen = param.length();

	LoLLocalLogInfo("before WebRequest");
	if (DEVWEB::WebRequest(gReportLogInterfaceUrl, rs, &rp))
	{
		LoLLocalLogInfo("after WebRequest");
		rs = DEVWEB::utf2ansi((const char*)rs.c_str());
		Log(rs.c_str());
		bool bRet = DEVWEB::IsResult200(rs);

		if (bRet != true) {
			LoLLocalLogInfo("发送 POST HTTP LOG 失败");
			return FALSE;
		}
		else{
			LoLLocalLogInfo("发送 POST HTTP LOG 成功 %s\n", rs.c_str());
			return TRUE;
		}
	}
	LoLLocalLogInfo("after WebRequest");

	return FALSE;
}

static std::string GetServerNameByID(unsigned int ServerID)
{
	std::string szServer = "";
	switch (ServerID)
	{
	case 257:
		szServer = "艾欧尼亚";
		break;
	case 513:
		szServer = "祖安";
		break;
	case 769:
		szServer = "诺克萨斯";
		break;
	case 1025:
		szServer = "班德尔城";
		break;
	case 1281:
		szServer = "皮尔特沃夫";
		break;
	case 1537:
		szServer = "战争学院";
		break;
	case 1793:
		szServer = "巨神峰";
		break;
	case 2049:
		szServer = "雷瑟守备";
		break;
	case 3073:
		szServer = "钢铁烈阳";
		break;
	case 2305:
		szServer = "裁决之地";
		break;
	case 2561:
		szServer = "黑色玫瑰";
		break;
	case 2817:
		szServer = "暗影岛";
		break;
	case 3585:
		szServer = "均衡教派";
		break;
	case 3329:
		szServer = "水晶之痕";
		break;
	case 3841:
		szServer = "影流";
		break;
	case 4097:
		szServer = "守望之海";
		break;
	case 4353:
		szServer = "征服之海";
		break;
	case 4609:
		szServer = "卡拉曼达";
		break;
	case 4865:
		szServer = "皮城警备";
		break;
	case 258://网通
		szServer = "比尔吉沃特";
		break;
	case 514:
		szServer = "德玛西亚";
		break;
	case 770:
		szServer = "弗雷尔卓德";
		break;
	case 1026:
		szServer = "无畏先锋";
		break;
	case 1282:
		szServer = "恕瑞玛";
		break;
	case 1538:
		szServer = "扭曲丛林";
		break;
	case 1794:
		szServer = "巨龙之巢";
		break;
	case 65539://教育网
		szServer = "教育网专区";
		break;
	default: szServer = "未知服务器";
	}

	return szServer;
}

static void MakeJsonOnePlayerData(Json::Value &outValue, const stPlayer* player)
{
	outValue["summonerName"] = player->szPlayerName;
	outValue["heroLevel"] = player->LEVEL;
	outValue["headCount"] = player->CHAMPIONS_KILLED;
	outValue["assists"] = player->ASSISTS;
	outValue["deaths"] = player->NUM_DEATHS;
	outValue["gold"] = player->GOLD_EARNED;
	outValue["minionsKill"] = player->MINIONS_KILLED;
	outValue["turretsKill"] = player->TURRETS_KILLED;
	outValue["hero"] = player->szHeroName;
	outValue["maxMoreKill"] = player->LARGEST_MULTI_KILL;
	outValue["maxLinkKill"] = player->LARGEST_KILLING_SPREE;
	outValue["totalDemageToHero"] = player->TOTAL_DAMAGE_DEALT_TO_CHAMPIONS;
	outValue["physicalDamageToHero"] = player->PHYSICAL_DAMAGE_DEALT_TO_CHAMPIONS;
	outValue["magicDamageToHero"] = player->MAGIC_DAMAGE_DEALT_TO_CHAMPIONS;
	outValue["trueDamageToHero"] = player->TRUE_DAMAGE_DEALT_TO_CHAMPIONS;
	outValue["totalDamageDealt"] = player->TOTAL_DAMAGE_DEALT;
	outValue["physicalDamageDealt"] = player->PHYSICAL_DAMAGE_DEALT_PLAYER;
	outValue["magicDamageDealt"] = player->MAGIC_DAMAGE_DEALT_PLAYER;
	outValue["trueDamageDealt"] = player->TRUE_DAMAGE_DEALT_PLAYER;
	outValue["maxCrit"] = player->LARGEST_CRITICAL_STRIKE;
	outValue["totalHeal"] = player->TOTAL_HEAL;
	outValue["totalDamageTaken"] = player->TOTAL_DAMAGE_TAKEN;
	outValue["physicalDamageTaken"] = player->PHYSICAL_DAMAGE_TAKEN;
	outValue["magicDamageTaken"] = player->MAGIC_DAMAGE_TAKEN;
	outValue["trueDamageTaken"] = player->TRUE_DAMAGE_TAKEN;
	outValue["barracksKill"] = player->BARRACKS_KILLED;
	outValue["neutralMinionsKill"] = player->NEUTRAL_MINIONS_KILLED;
	outValue["neutralMinionsKillEnemy"] = player->NEUTRAL_MINIONS_KILLED_ENEMY_JUNGLE;
	outValue["neutralMinionsKillYour"] = player->NEUTRAL_MINIONS_KILLED_YOUR_JUNGLE;
	outValue["totalDeadTime"] = player->TOTAL_TIME_SPENT_DEAD;
	outValue["totalControlTime"] = player->TOTAL_TIME_CROWD_CONTROL_DEALT;

	outValue["item0"] = player->ITEM0;
	outValue["item1"] = player->ITEM1;
	outValue["item2"] = player->ITEM2;
	outValue["item3"] = player->ITEM3;
	outValue["item4"] = player->ITEM4;
	outValue["item5"] = player->ITEM5;
	outValue["item6"] = player->ITEM6;
	outValue["sightWardsBuy"] = player->SIGHT_WARDS_BOUGHT_IN_GAME;
	outValue["sightWardsBuy"] = player->VISION_WARDS_BOUGHT_IN_GAME;
	outValue["wardKilled"] = player->WARD_KILLED;
	outValue["wardPlaced"] = player->WARD_PLACED;
}

static Json::Value MakeJsonGameData(const stLOLGame &GameData)
{
	Json::Value root;
	root["qq"] = GameData.GameState.QQ;
	root["playerName"] = GameData.GameState.szMainPlayer;
	root["serverName"] = GetServerNameByID(GameData.GameState.ServerID).c_str();
	root["serverId"] = GameData.GameState.ServerID;
	root["beginTime"] = GameData.GameState.GameEndTime - GameData.GameLengh;
	root["timeLength"] = GameData.GameLengh;
	root["gameType"] = GameData.GameType;
	root["gameTypeStr"] = GameData.GameTypeStr;
	
	int myTeam = 0;
	int success = 0;
	const stPlayer *pMe = 0;
	for (pMe = &GameData.Players[0]; pMe != &GameData.Players[LOLMAXPLAYER]; pMe++) {
		if (pMe->globalPlayerID == GameData.GameState.LOLGlobalID) {
			myTeam = pMe->TEAM;
			success = pMe->Win;
			break;
		}
	}


	Json::Value ourData;
	Json::Value enemyData;
	for (pMe = &GameData.Players[0]; pMe != &GameData.Players[LOLMAXPLAYER]; pMe++) {
		Json::Value OnePlayerData;
		if (pMe->TEAM && pMe->szPlayerName[0] != 0) {
			MakeJsonOnePlayerData(OnePlayerData, pMe);
			if (pMe->TEAM == myTeam) {
				ourData.append(OnePlayerData);
			}
			else {
				enemyData.append(OnePlayerData);
			}
		}
	}
	
	root["ourData"] = ourData;
	root["enemyData"] = enemyData;

	root["tier"] = GameData.GameState.szRankLevel;
	root["level"] = GameData.GameState.Level;
	root["success"] = success;
	
	return root;
}

static Json::Value MakeJsonLoginData()
{
	Json::Value root;

	root["userId"] = 0x1234; 
	root["snbid"] = "snbid";


	return root;
}

static Json::Value MakeJsonMsgLoginData()
{
	Json::Value root;

	root["snbid"] = "snbid";
	root["clientVersion"] = "clientVersion";
	root["clientVersion"] = LolHelperVersion;

	return root;
}

#pragma pack (1)
struct com_info_t
{
	com_info_t() :user_type_(0), size_(0), version_(1), user_state_(0)
	{
		size_ = sizeof(com_info_t);
		memset(id_, 0, 260);
		memset(user_name_, 0, 128);
		memset(user_nation_, 0, 16);
		memset(user_sex_, 0, 4);
		memset(user_brith_, 0, 8);
		memset(bar_id_, 0, 128);
		memset(clsmn_product_ver_, 0, 32);
		memset(clsmn_file_ver_, 0, 32);
		memset(user_area_, 0, 16);
		memset(card_id_, 0, 64);
	}
	DWORD  size_;
	DWORD  version_;
	char  id_[260];//身份唯一编码(证件类型+证件号)MD5之后转BCD
	char  user_name_[128];//用户姓名
	char  user_nation_[16];//用户民族
	char  user_sex_[4];//用户性别
	char  user_brith_[16];//用户生日
	char  bar_id_[128];//网吧ID
	char  clsmn_product_ver_[32];//产品版本号
	char  clsmn_file_ver_[32];//文件版本号
	char  user_area_[16];// 身份证前6位
	char  card_id_[64]; //卡号
	int  user_type_;//卡类型
	int  card_type_;//证件类型
	int  user_state_;//用户状态 0 未上机 1上机
};
#pragma pack ()

static BOOL GetReportDataHead(Json::Value & root)
{
	HANDLE mem_map_handle_c_ = OpenFileMappingW(FILE_MAP_READ, FALSE, L"SWWanXCom");
	if (mem_map_handle_c_ == NULL)
	{
		return FALSE;
	}
	com_info_t * mem_view_c_ = (com_info_t*)MapViewOfFile(mem_map_handle_c_, FILE_MAP_READ, 0, 0, 200 * 1024);
	if (mem_view_c_ == NULL)
	{
		CloseHandle(mem_map_handle_c_);
		return FALSE;
	}
	else
	{
		/*
		root["useId"] = atoi(UUidStr.c_str());
		root["snbid"] = nbiStr.c_str();
		*/
		root["account"] = mem_view_c_->card_id_;
		root["type"] = mem_view_c_->user_type_;
		root["snbid"] = mem_view_c_->bar_id_;

		UnmapViewOfFile(mem_view_c_);
	}
	CloseHandle(mem_map_handle_c_);

	return TRUE;
}

static BOOL GetReportLogHead(Json::Value & root)
{
	HANDLE mem_map_handle_c_ = OpenFileMappingW(FILE_MAP_READ, FALSE, L"SWWanXCom");
	if (mem_map_handle_c_ == NULL)
	{
		return FALSE;
	}
	com_info_t * mem_view_c_ = (com_info_t*)MapViewOfFile(mem_map_handle_c_, FILE_MAP_READ, 0, 0, 200 * 1024);
	if (mem_view_c_ == NULL)
	{
		CloseHandle(mem_map_handle_c_);
		return FALSE;
	}
	else
	{
		root["snbid"] = mem_view_c_->bar_id_;
		root["clientVersion"] = mem_view_c_->clsmn_file_ver_;
		root["lolHelperVersion"] = LolHelperVersion;
		UnmapViewOfFile(mem_view_c_);
	}
	CloseHandle(mem_map_handle_c_);

	return TRUE;
}

BOOL IsReportWxClientReady()
{
	static int flag = 1;
	Json::Value root1, root2/*, root3, root4*/;

	/*
	if (ReadingFileGetLogHead(root3)) {
		if (flag) {LoLLocalLogInfo("ReadingFileGetLogHead success");}
	}else {
		if (flag) {LoLLocalLogInfo("ReadingFileGetLogHead fail");}
	}

	if (GetReportLogHead(root4)) {
		if (flag) {LoLLocalLogInfo("GetReportLogHead success");}
	}else {
		if (flag) {LoLLocalLogInfo("GetReportLogHead fail");}
	}
	*/

	BOOL bRet1 = FALSE, bRet2 = FALSE;

	bRet1 = GetReportDataHead(root1);
	if (bRet1) {
		if (flag) {LoLLocalLogInfo("GetReportDataHead success");}
	}else {
		if (flag) {LoLLocalLogInfo("GetReportDataHead fail");}
	}

	bRet2 = ReadingFileGetDataHead(root1);
	if (bRet2) {
		if (flag) {LoLLocalLogInfo("ReadingFileGetDataHead success");}
	}else {
		if (flag) {LoLLocalLogInfo("ReadingFileGetDataHead fail");}
	}
	flag = 0;

	return bRet2 || bRet1;
}

static BOOL GetFileKey(const std::string & file, const std::string &key, std::string & value)
{
	BOOL bRet = FALSE;
	char sysPathBuf[MAX_PATH] = {0};
	UINT uRet = GetSystemDirectoryA(sysPathBuf, sizeof(sysPathBuf));	//返回 C:\Windows\system32，可能不在这个目录下，

	value = "";

	if (uRet) {
		std::string uuidFile = std::string(sysPathBuf) + file;
		if (IsExsitDirOrFile(uuidFile.c_str()) == true) {
			FILE *fp = fopen(uuidFile.c_str(), "rb");
			if (fp) {
				fseek(fp, 0, SEEK_END);
				long fileLen = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				char* pFileBuf = new char[fileLen + 1];
				if (pFileBuf) {
					fread(pFileBuf, 1, fileLen, fp);
					value = pFileBuf;
					delete[] pFileBuf;
				}
				fclose(fp);
			}
		}
	}

	if (value.empty() != true) {
		std::string::size_type index = value.find(key);
		if (index != std::string::npos) {
			value = value.substr(index + key.length(), value.length() - 1);

			std::string::size_type index1, index2;
			index1 = value.find("\r\n");
			index2 = value.find("\n");
			if (index1 != std::string::npos) {
				value = value.substr(0, index1);
			}else if (index2 != std::string::npos) {
				value = value.substr(0, index2);
			}

			bRet = TRUE;
		}
	}

	return bRet;
}

static BOOL ReadingFileGetDataHead(Json::Value & root)
{
	std::string  UUidStr = "",  nbiStr = "";
	BOOL bRet1 = FALSE, bRet2 = FALSE, bRet3 = FALSE, bRet4 = FALSE;
	
	bRet1 = GetFileKey("\\uuid_config.ini", "uuid=", UUidStr);
	bRet2 = GetFileKey("\\NBMediaInfo_Adv.ini", "NBI=", nbiStr);

	if (bRet1 && bRet2) {
		root["useId"] = atoi(UUidStr.c_str());
		if (atoi(UUidStr.c_str()) != 0) {
			bRet3 = TRUE;
		}
		root["snbid"] = nbiStr.c_str();
		if (nbiStr.length() != 0) {
			bRet4 = TRUE;
		}
	}

	return bRet1 && bRet2 && bRet3 && bRet4;
}

static BOOL ReadingPEGetClientVersion(string &strVersion)
{
	std::string clientPath = std::string(GetAppLocalPathA().GetBuffer(0)) + "clsmn.exe"; 
	strVersion = GetPEVersion(clientPath);

	if (strVersion.empty() != true)
		return TRUE;

	return FALSE;
}

static BOOL ReadingFileGetLogHead(Json::Value & root)
{
	std::string  strVersion = "",  nbiStr = "";
	BOOL bRet1 = FALSE, bRet2 = FALSE;

	bRet1 = GetFileKey("\\NBMediaInfo_Adv.ini", "NBI=", nbiStr);
	bRet2 = ReadingPEGetClientVersion(strVersion);

	if (bRet1 && bRet2) {
		root["snbid"] = nbiStr.c_str();
		root["clientVersion"] = strVersion.c_str();
		root["lolHelperVersion"] = LolHelperVersion;
	}

	return bRet1 && bRet2;
}