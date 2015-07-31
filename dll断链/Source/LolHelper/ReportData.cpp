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
	//����Ƿ��õ���gLoginData
	Json::Value loginDataJson;
	BOOL bRet1 = FALSE, bRet2 = FALSE;

	bRet1 = GetReportDataHead(loginDataJson);
	bRet2 = ReadingFileGetDataHead(loginDataJson);

	if (bRet2 == FALSE && bRet1 == FALSE) {
		std::string str = "�ϱ���Ϸ���ݵ�ʱ���ȡHead��UUIDʧ��";
		ReportLog(ReportGetReportHeadFail, str);
		LoLLocalLogInfo("%s", str.c_str());
		return FALSE;
	}
	
	std::string param = "";
	Json::FastWriter writer;
	
	//����loginData 
	param = std::string(gLoginDataParamName) + "=" + writer.write(loginDataJson);
	Log("==========loginData");
	Log(writer.write(loginDataJson).c_str());

	//����gameData
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
			LoLLocalLogInfo("���� POST HTTP GAMEDATA ʧ��,���³���");

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
					LoLLocalLogInfo("���� POST HTTP GAMEDATA ʧ��");
				}
			}
			else {
				LoLLocalLogInfo("�ϱ���Ϸ���ݳɹ�");
			}
		}
		else {
			LoLLocalLogInfo("�ϱ���Ϸ���ݳɹ�");
		}
	}

	return TRUE;
}

BOOL ReportLog(unsigned int type, const std::string &msg )
{
	//����Ƿ��õ���loginData
	Json::Value msgloginJson;
	if (ReadingFileGetLogHead(msgloginJson)) {

	}
	else if (GetReportLogHead(msgloginJson) == FALSE){		//û���õ�gLoginData
		LoLLocalLogErr("GetReportDataHead fail:%d", type);
		return FALSE;
	}

	Json::Value root;
	std::string param = "";
	Json::FastWriter writer;

	//����loginData 
	
	param = std::string(gLoginDataParamName) + "=" + writer.write(msgloginJson);
	Log("==========loginData");
	Log(writer.write(msgloginJson).c_str());

	//����gameData
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
			LoLLocalLogInfo("���� POST HTTP LOG ʧ��");
			return FALSE;
		}
		else{
			LoLLocalLogInfo("���� POST HTTP LOG �ɹ� %s\n", rs.c_str());
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
		szServer = "��ŷ����";
		break;
	case 513:
		szServer = "�氲";
		break;
	case 769:
		szServer = "ŵ����˹";
		break;
	case 1025:
		szServer = "��¶���";
		break;
	case 1281:
		szServer = "Ƥ�����ַ�";
		break;
	case 1537:
		szServer = "ս��ѧԺ";
		break;
	case 1793:
		szServer = "�����";
		break;
	case 2049:
		szServer = "��ɪ�ر�";
		break;
	case 3073:
		szServer = "��������";
		break;
	case 2305:
		szServer = "�þ�֮��";
		break;
	case 2561:
		szServer = "��ɫõ��";
		break;
	case 2817:
		szServer = "��Ӱ��";
		break;
	case 3585:
		szServer = "�������";
		break;
	case 3329:
		szServer = "ˮ��֮��";
		break;
	case 3841:
		szServer = "Ӱ��";
		break;
	case 4097:
		szServer = "����֮��";
		break;
	case 4353:
		szServer = "����֮��";
		break;
	case 4609:
		szServer = "��������";
		break;
	case 4865:
		szServer = "Ƥ�Ǿ���";
		break;
	case 258://��ͨ
		szServer = "�ȶ�������";
		break;
	case 514:
		szServer = "��������";
		break;
	case 770:
		szServer = "���׶�׿��";
		break;
	case 1026:
		szServer = "��η�ȷ�";
		break;
	case 1282:
		szServer = "ˡ����";
		break;
	case 1538:
		szServer = "Ť������";
		break;
	case 1794:
		szServer = "����֮��";
		break;
	case 65539://������
		szServer = "������ר��";
		break;
	default: szServer = "δ֪������";
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
	char  id_[260];//���Ψһ����(֤������+֤����)MD5֮��תBCD
	char  user_name_[128];//�û�����
	char  user_nation_[16];//�û�����
	char  user_sex_[4];//�û��Ա�
	char  user_brith_[16];//�û�����
	char  bar_id_[128];//����ID
	char  clsmn_product_ver_[32];//��Ʒ�汾��
	char  clsmn_file_ver_[32];//�ļ��汾��
	char  user_area_[16];// ���֤ǰ6λ
	char  card_id_[64]; //����
	int  user_type_;//������
	int  card_type_;//֤������
	int  user_state_;//�û�״̬ 0 δ�ϻ� 1�ϻ�
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
	UINT uRet = GetSystemDirectoryA(sysPathBuf, sizeof(sysPathBuf));	//���� C:\Windows\system32�����ܲ������Ŀ¼�£�

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