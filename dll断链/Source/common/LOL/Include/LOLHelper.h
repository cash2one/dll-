#pragma once
#include "ClolUnit.h"
#include "time.h"
#include <vector>
#include "LOLSkillManager.h"
#include <list>
#include <D3DManager.h>
using namespace std;


enum MiniMapEvent{
	CREEPS11=1,
	CREEPS12,
	CREEPS21,
	CREEPS22,
	BOSS1,//大龙
	BOSS2,//小龙

	TOWER11,
	TOWER12,
	TOWER13,
	TOWER21,
	TOWER22,
	TOWER23,

	HERODEAD,

	//大乱斗计时
	BLOODBUFFER1,
	BLOODBUFFER2,
	BLOODBUFFER3,
	BLOODBUFFER4
};

struct stWardInfo{
/*
	[107636] =,= 插眼!  X=2121, Y=1096, Name=SightWard ,CD=180  *180s   绿眼
	[107636] =,= 插眼!  X=2878, Y=983, Name=SightWard ,CD=60   *60s,到9级时120s //饰品眼

	[107636] =,= 插眼!  X=1650, Y=721, Name=VisionWard ,CD=0  *0s		//红眼
	[14064] =,=   X=4518, Y=4348, Name=VisionWard ,CD=180		//眼石
*/
	unsigned int pUnit;
	int Team;
	CStringA szName;
	POINT pt;
	unsigned int CD;
	time_t TimeEnd;
	stWardInfo()
	{
		pUnit=0;
		Team=0;
		CD=0;
		TimeEnd=0;
	}
};


struct stPlayerState{
	bool bMainPlayer;
	ClolHero* pPlayer;

	time_t TimeBack;//复活时间


	stPlayerState()
	{
		bMainPlayer=0;
		pPlayer=0;
		TimeBack=0;
	}
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



class CLOLHelper
{
public:
	CLOLHelper(void);
	~CLOLHelper(void);
	//初始化
	void SetResPath(CStringA szGamePath,CStringA szImg);	//路径
	void SetPos(POINT ScreenSize,float ScaleGUI,float ScaleMINIMAP,bool bIsRightMap);//屏幕大小
	void AddPlayer(int pUnit,bool bIsMainPlayer=0);//添加玩家
	void BeginMyThread();																//启动所有工作线程

	//事件
	void Event(MiniMapEvent event,int IndexFlag,float time);	//事件
	void Skill(unsigned int pSkill,float CD);											//技能
	void AddWard(unsigned int pUnitWard);
	bool Menu_Msg(UINT nMsg, WPARAM wParam, LPARAM lParam,GLOBALCONFIG* pOption); //窗口鼠标事件,关闭返回0


	//绘图
	void Draw_UI(bool bEnemyInfo,bool bMonsterTimer,bool bInhibitorTimer,bool bUltraCD,bool bHealPackTimer,bool bFriendSummonerSkillsCD,bool bDrawGankTip);//画界面
	void Draw_Hero(void* pUnit,int posX,int posY,bool bSkillCD,bool bHUDHP);//画人
	void Draw_HpHitTip(unsigned int pUnit,unsigned int x,unsigned int y);//补刀提示
	void Draw_Menu(GLOBALCONFIG* pOption);
	void Draw_MiniMap();
	//资源
	void D3DLoadRes(IDirect3DDevice9 * pInitDevice);//加载资源
	void D3DFreeRes();//释放资源

	//查询玩家
	ClolHero* GetMainPlayer();				//主玩家
	ClolHero* GetPlayerByFlag(unsigned int IndexFlag);
	ClolHero* GetPlayerByIndex(unsigned int Index);
	int GetPlayerCount(){return m_vPlayers.size();};


	void SayChar(char* pSay,unsigned int OffsetSayChar);
	void Draw_String(int posX,int posY,CStringA szChar,int color,CStringA szFontName,int DrawWidth=256);//画字

	 struct{
		unsigned int T1Count;
		unsigned int T2Count;
	}m_UpdateData;

	 //计时
	 struct{
		 time_t CREEPS11;
		 time_t CREEPS12;
		 time_t CREEPS21;
		 time_t CREEPS22;

		 time_t BOSS1;//大龙
		 time_t BOSS2;//小龙

		 time_t TOWER11;
		 time_t TOWER12;
		 time_t TOWER13;
		 time_t TOWER21;
		 time_t TOWER22;
		 time_t TOWER23;

		 //大乱斗计时
		time_t BLOODBUFFER1;
		time_t BLOODBUFFER2;
		time_t BLOODBUFFER3;
		time_t BLOODBUFFER4;
	 }m_Times;

	 static DWORD WINAPI ThreadGankTip(LPVOID lp);
	 static DWORD WINAPI ThreadCheckWard(LPVOID lp);
	 list<stWardInfo> m_liWards;//眼位统计
	 vector<stPlayerState> m_vPlayers;	//第一个是主玩家
private:
	//玩家指针
	stPlayerState* _GetPlayerByFlag(int IndexFlag);
	stPlayerState* _GetPlayerByPtr(int pUnit);
	

	CLOLSkillManager m_SkillManager;
	CD3DManager m_D3DManager;

	struct{
	RECT rectMiniMap;//小地图
	RECT rectFrientHead;//左边英雄头像
	RECT rectEnemyDeadHead;//敌方头像
	RECT rectScreen;//屏幕大小

	RECT rectMenu;//菜单
	POINT ptMouse;//鼠标位置
	}m_DrawParam;

	//Draw技能条
	void Draw_Hero_SkillBar(void* pUnit,int posX,int posY);//XY为技能框背景图片
	void DrawRightEnemyHead();//更新右侧敌方英雄头像
	void DrawLeftFriendHead();//左侧英雄头像
	//Menu
	void Draw_MenuTip(int x, int y,CStringA szTitle,CStringA szInfo,bool bOpen);
	

	struct{
		bool m_bResLoad;
		CStringA szPathImgs;//召唤师技能,HUD
		CStringA szPathGame;//英雄头像
		//CComCriticalSection m_CSRes;
	}m_Res;
};

 int HowLong2Point(POINT pt1,POINT pt2);
int MathHPFontColor(int CurrentHP,int MaxHp);
unsigned int MyFindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen);