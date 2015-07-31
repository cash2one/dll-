#pragma once


extern "C"  __declspec(dllexport)  bool AirLib_Match_Init();
bool SendMessagetoApp(void* pBuffer,unsigned int Len,int Commadnid);
CStringA GetServerNameByID(unsigned int ServerID);

#pragma pack(push)
#pragma pack(1)
//和助手主进程通信用_传输登陆ST和账号名
struct stSharePlayerMem_Account{
	wchar_t account[16];
	char ST[128];
	char IP[32];
	unsigned int Port;
};

#define	LOCAL_CMD_LOGININFO	1	//登陆后传给主程序玩家信息
#define	LOCAL_CMD_RESULT	2		//游戏结束传给主程序战绩
//大厅发给助手程序的消息
#define LOCAL_CMD_MATCH_CREATEROOM 3  //创建房间
#define LOCAL_CMD_MATCH_JOINROOM 4		//进入房间
#define LOCAL_CMD_MATCH_STARTGAME 5		//开始游戏
#define LOCAL_CMD_MATCH_SELECTSERVER 6  //刚输入完账号密码

#define LOCAL_CMD_MATCH_CHECK_REQ 10		//客户端向主程序查询创建或进入房间
#define LOCAL_CMD_MATCH_CHECK_RSP 11

#define LOCAL_CMD_MATCH_CHECK_STARTGAME_REQ 12		//客户端向主程序查询创建或进入房间
#define LOCAL_CMD_MATCH_CHECK_STARTGAME_RSP 13

#define LOCAL_CMD_MATCH_CHECK_QUITGAME_REQ 14		//在选人界面,手动退出游戏
				#define LEAVE_ROOM_QUIT_ROOM	2	//退房间
				#define LEAVE_ROOM_QUIT_BAN		3	//退BP界面

// LOCAL_CMD_MATCH_CREATEROOM / LOCAL_CMD_MATCH_JOINROOM / LOCAL_CMD_MATCH_STARTGAME
struct stLOCAL_CMD_MATCH_MSG
{
	BYTE 	bSucc;	// 是否成功
};


struct LocalCmdHeader
{
	int	nLen;
	int	nCmdId;
};

struct stLOCAL_CMD_LOGININFO{//LOCAL_CMD_LOGININFO结构体
	char szName[128];//玩家ID
	char szServer[128];//服务器名字
	unsigned int ServerID;//服务器ID
	unsigned int Head;//头像
	int Level;//账号等级
	long long QQ;//QQ号
	long long LOLID;//LOL全球唯一ID
};

struct LolClientConfig
{
	int		nLocalPort;	// local server socket listen port
	HWND	hwndSingleton;	// 单例用，主窗口句柄
	char	szCGAPath[MAX_PATH];	//助手程序根目录
	struct{
		bool	bMatch;		//是否比赛模式
		bool	bCreate;	// true：创建房间，false：加入房间
		//short	nReserved;
		bool	bOwnerCamp;	// 是否房主阵营
		BYTE	MapIndex;//地图 , 0=召唤师峡谷,1=水晶之恨,2=扭曲丛林,3=嚎哭深渊
		BYTE	ViewIndex;//观看模式 , 0=不能观战,1=所有人,2=仅限同大厅玩家,3=仅限好友
		BYTE	GameMode;//游戏模式, 0=自选模式,1=征召模式,2=随机模式		// 游戏房间信息
		bool	bManual;	// 是手动
		BYTE	nReserved;

		UINT	ServerID;	//服务器ID
		char	szRoomName[128];//房间名
		char	szPassword[128];//密码
	}Match;
};


typedef struct tagGLOBALCONFIG{
	BOOL bSkillCD; // 全技能CD				
	BOOL bHideHUD; // 透明界面(已废弃)
	BOOL bMonsterTimer; // 打野计时
	BOOL bInhibitorTimer; // 水晶计时
	BOOL bTowerCircle; // 防御塔范围
	BOOL bPureMode; // 纯净模式
	BOOL bUltraCD; // 大招计时
	BOOL bEnemyInfo; // 敌方信息
	BOOL bHealPackTimer; // 血包计时
	BOOL bHUDHP; // 数字显血
	BOOL bLastHitTip;//补刀提示
	BOOL bFriendSummonerSkillsCD;//友军召唤师技能CD
	BOOL bGankTip;//Gank提醒
}GLOBALCONFIG;

#pragma pack(pop)



