#ifndef _LOLDEF
#define _LOLDEF


enum LOLGAMEMODE{//游戏模式
	CLASSIC=1,		//经典模式
	ARAM=2,			//大乱斗
	ODIN=3,			//统治战场(夺旗模式)
	ASCENSION=4, //飞升争夺战
	KINGPORO=5 //波罗大乱斗
};
enum LOLGAMETYPE{//匹配模式
	NORMAL=1,					//普通匹配
	NONE=2,							//非匹配模式(自定义房间)
	RANKED_SOLO_5x5=3,	//5v5排位
	NORMAL_3x3=4,				//3v3匹配
	ARAM_UNRANKED_5x5=5,//大乱斗5v5
	ODIN_UNRANKED=6,		//统治战场匹配
	BOT=7,								//匹配AI
	HEXAKILL=8,							//6v6模式
	KING_PORO=9,						//波罗乱斗
	URF=10,							//无限火力
	MID6V6=11				//6v6模式
};


enum enumBUFFTYPE{//解析后返回的结果
	TYPE_EMPTY=1024,	//无效的数据段
	TYPE_GAMESTATE,	//游戏结束的战绩段
	TYPE_GAMESTATE_ERROR//游戏战绩解析错误
};
#define LOLMAXPLAYER 12

struct stGameState{//上传数据的主机信息,作为文件尾上传到数据库
	//全局数据,通过大厅获得的
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
	unsigned int HeadImgID;		//玩家头像ID
	unsigned int RateNumber;	//胜点
	//548Byte+

	struct{//游戏结束玩家名排序用,由游戏内注入进程填写
		char szHero[32];
		char szPlayer[32];
		unsigned int Team;
	}PlayerData[LOLMAXPLAYER];
};



//一个玩家
struct stPlayer{
	bool bParserRight;	//解析是否正常
	long long globalPlayerID;	//全球唯一游戏ID,同一个QQ号在不同服务器建立账号以后也有全球唯一账号
	int Win;		//1=win  0=loss
	char szPlayerName[128];	//玩家游戏角色名
	unsigned int HeroID;		//英雄ID
	char szHeroName[128];	//英雄名
	int TEAM;					//阵营 , 0x64是蓝队, 0xC8红队

	//游戏内对应的数据
	int	LEVEL;					//等级

	int	CHAMPIONS_KILLED;			//击杀K
	int	ASSISTS;					//助攻A
	int	NUM_DEATHS;				//死亡D

	int	LARGEST_KILLING_SPREE;			//最高连杀
	int	LARGEST_MULTI_KILL;			//最高多杀

	int	TOTAL_DAMAGE_DEALT_TO_CHAMPIONS;		//对英雄的总伤害
	int	PHYSICAL_DAMAGE_DEALT_TO_CHAMPIONS;	//对英雄的物理伤害
	int	MAGIC_DAMAGE_DEALT_TO_CHAMPIONS;		//对英雄的法术伤害
	int	TRUE_DAMAGE_DEALT_TO_CHAMPIONS;		//对英雄的真实伤害

	int	TOTAL_DAMAGE_DEALT;			//输出总伤害
	int	PHYSICAL_DAMAGE_DEALT_PLAYER;		//输出物理伤害
	int	MAGIC_DAMAGE_DEALT_PLAYER;		//输出法术伤害
	int	TRUE_DAMAGE_DEALT_PLAYER;		//输出真实伤害
	int	LARGEST_CRITICAL_STRIKE;			//最大暴击伤害

	int	TOTAL_HEAL;				//治疗总量
	int	TOTAL_DAMAGE_TAKEN;			//承受总伤害
	int	PHYSICAL_DAMAGE_TAKEN;			//承受物理伤害
	int	MAGIC_DAMAGE_TAKEN;			//承受法术伤害
	int	TRUE_DAMAGE_TAKEN;			//承受真实伤害

	int	GOLD_EARNED;				//金钱
	int	TURRETS_KILLED;				//拆塔数
	int	MINIONS_KILLED;				//小兵击杀
	int	BARRACKS_KILLED;				//拆兵营数
	int	NEUTRAL_MINIONS_KILLED;			//野怪击杀
	int	NEUTRAL_MINIONS_KILLED_ENEMY_JUNGLE;	//敌方野怪击杀
	int	NEUTRAL_MINIONS_KILLED_YOUR_JUNGLE;	//己方野怪击杀

	int	TOTAL_TIME_SPENT_DEAD;			//总计死亡时间
	int	TOTAL_TIME_CROWD_CONTROL_DEALT;		//输出团控时间

	int	ITEM0;					//物品1
	int	ITEM1;					//物品2
	int	ITEM2;					//物品3
	int	ITEM3;					//物品4
	int	ITEM4;					//物品5
	int	ITEM5;					//物品6
	int	ITEM6;					//物品7

	int	SIGHT_WARDS_BOUGHT_IN_GAME;	//普通眼购买数
	int	VISION_WARDS_BOUGHT_IN_GAME;		//真眼购买数
	int	WARD_KILLED;				//拆眼数
	int	WARD_PLACED;				//插眼数

//*****************************下面的数据段录入数据库时可以跳过~
	int	LOSE;					//失败方	
	int	WIN;					//失败方	

	//统治战场模式才有的数据
	int	 COMBAT_PLAYER_SCORE;
	int	NODE_CAPTURE_ASSIST;
	int	OBJECTIVE_PLAYER_SCORE;
	int     VICTORY_POINT_TOTAL;
	int  TOTAL_PLAYER_SCORE;
	int TOTAL_SCORE_RANK;
	int NODE_CAPTURE;
	int TEAM_OBJECTIVE;
	int NODE_NEUTRALIZE;
	int NODE_NEUTRALIZE_ASSIST;
};

//一局游戏
enum ShareDataReadyType{
	ShareDataReadyType_NOT_Ready = 0,
	ShareDataReadyType_Ready = 1,       //或起来
	ShareDataErrInfoReadyType_Ready = 2, 
};



struct stLOLGame{
	stGameState GameState;//上传者信息

	long long GameID;//该场比赛在该服务器内唯一ID
	unsigned int GameLengh;	//游戏长度,单位秒
	LOLGAMEMODE GameMode; //游戏模式
	LOLGAMETYPE GameType;	//匹配模式
	char GameTypeStr[40];

	unsigned int PlayerCount;//玩家数量
	unsigned int WinnerTeam;//胜利方  100蓝  200红

	unsigned int TEAM_BLUE_K;//蓝队杀人数
	unsigned int TEAM_BLUE_D;//蓝队死亡数
	unsigned int TEAM_BLUE_A;//蓝队助攻数

	unsigned int TEAM_RED_K;//红队杀人数
	unsigned int TEAM_RED_D;//红队死亡数
	unsigned int TEAM_RED_A;//红队助攻数


	stPlayer Players[LOLMAXPLAYER];//10个玩家的数据
};

struct stLOLShareMem {
	int stLoLShareMemVersion;
	unsigned int  ShareDataReady;	//1:ready  0:not ready
	unsigned int  ShareDataErrNum;	//错误码

	stLOLGame game;

	stLOLShareMem() {
		stLoLShareMemVersion = 1;
		ShareDataErrNum = 0;
		ShareDataReady = ShareDataReadyType_NOT_Ready;
	}
};


//和助手主进程通信用
struct stSharePlayerMem{

	stGameState GameState;//读取Client 获取QQ,服务器等信息

	struct{
		unsigned int PlayerHead;//玩家头像
	}LocalGameData;

	struct{//与服务器通信数据,用于上传日志
		//二进制原始文件.大小不固定
		char pDataBuffer[1024*20];//一般10人比赛数据在10K左右
		unsigned int LenBuffer;//
	}Buffer;

	struct{//客户端查询服务器数据回调用
		unsigned int LenBuffer;//
		char pBuffer[1024*1024];//一般10人比赛数据在10K左右
	}Back;
};


#endif