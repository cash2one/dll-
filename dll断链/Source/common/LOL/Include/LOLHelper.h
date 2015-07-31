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
	BOSS1,//����
	BOSS2,//С��

	TOWER11,
	TOWER12,
	TOWER13,
	TOWER21,
	TOWER22,
	TOWER23,

	HERODEAD,

	//���Ҷ���ʱ
	BLOODBUFFER1,
	BLOODBUFFER2,
	BLOODBUFFER3,
	BLOODBUFFER4
};

struct stWardInfo{
/*
	[107636] =,= ����!  X=2121, Y=1096, Name=SightWard ,CD=180  *180s   ����
	[107636] =,= ����!  X=2878, Y=983, Name=SightWard ,CD=60   *60s,��9��ʱ120s //��Ʒ��

	[107636] =,= ����!  X=1650, Y=721, Name=VisionWard ,CD=0  *0s		//����
	[14064] =,=   X=4518, Y=4348, Name=VisionWard ,CD=180		//��ʯ
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

	time_t TimeBack;//����ʱ��


	stPlayerState()
	{
		bMainPlayer=0;
		pPlayer=0;
		TimeBack=0;
	}
};



typedef struct tagGLOBALCONFIG{
	BOOL bSkillCD; // ȫ����CD				
	BOOL bHideHUD; // ͸������(�ѷ���)
	BOOL bMonsterTimer; // ��Ұ��ʱ
	BOOL bInhibitorTimer; // ˮ����ʱ
	BOOL bTowerCircle; // ��������Χ
	BOOL bPureMode; // ����ģʽ
	BOOL bUltraCD; // ���м�ʱ
	BOOL bEnemyInfo; // �з���Ϣ
	BOOL bHealPackTimer; // Ѫ����ʱ
	BOOL bHUDHP; // ������Ѫ
	BOOL bLastHitTip;//������ʾ
	BOOL bFriendSummonerSkillsCD;//�Ѿ��ٻ�ʦ����CD
	BOOL bGankTip;//Gank����
}GLOBALCONFIG;



class CLOLHelper
{
public:
	CLOLHelper(void);
	~CLOLHelper(void);
	//��ʼ��
	void SetResPath(CStringA szGamePath,CStringA szImg);	//·��
	void SetPos(POINT ScreenSize,float ScaleGUI,float ScaleMINIMAP,bool bIsRightMap);//��Ļ��С
	void AddPlayer(int pUnit,bool bIsMainPlayer=0);//������
	void BeginMyThread();																//�������й����߳�

	//�¼�
	void Event(MiniMapEvent event,int IndexFlag,float time);	//�¼�
	void Skill(unsigned int pSkill,float CD);											//����
	void AddWard(unsigned int pUnitWard);
	bool Menu_Msg(UINT nMsg, WPARAM wParam, LPARAM lParam,GLOBALCONFIG* pOption); //��������¼�,�رշ���0


	//��ͼ
	void Draw_UI(bool bEnemyInfo,bool bMonsterTimer,bool bInhibitorTimer,bool bUltraCD,bool bHealPackTimer,bool bFriendSummonerSkillsCD,bool bDrawGankTip);//������
	void Draw_Hero(void* pUnit,int posX,int posY,bool bSkillCD,bool bHUDHP);//����
	void Draw_HpHitTip(unsigned int pUnit,unsigned int x,unsigned int y);//������ʾ
	void Draw_Menu(GLOBALCONFIG* pOption);
	void Draw_MiniMap();
	//��Դ
	void D3DLoadRes(IDirect3DDevice9 * pInitDevice);//������Դ
	void D3DFreeRes();//�ͷ���Դ

	//��ѯ���
	ClolHero* GetMainPlayer();				//�����
	ClolHero* GetPlayerByFlag(unsigned int IndexFlag);
	ClolHero* GetPlayerByIndex(unsigned int Index);
	int GetPlayerCount(){return m_vPlayers.size();};


	void SayChar(char* pSay,unsigned int OffsetSayChar);
	void Draw_String(int posX,int posY,CStringA szChar,int color,CStringA szFontName,int DrawWidth=256);//����

	 struct{
		unsigned int T1Count;
		unsigned int T2Count;
	}m_UpdateData;

	 //��ʱ
	 struct{
		 time_t CREEPS11;
		 time_t CREEPS12;
		 time_t CREEPS21;
		 time_t CREEPS22;

		 time_t BOSS1;//����
		 time_t BOSS2;//С��

		 time_t TOWER11;
		 time_t TOWER12;
		 time_t TOWER13;
		 time_t TOWER21;
		 time_t TOWER22;
		 time_t TOWER23;

		 //���Ҷ���ʱ
		time_t BLOODBUFFER1;
		time_t BLOODBUFFER2;
		time_t BLOODBUFFER3;
		time_t BLOODBUFFER4;
	 }m_Times;

	 static DWORD WINAPI ThreadGankTip(LPVOID lp);
	 static DWORD WINAPI ThreadCheckWard(LPVOID lp);
	 list<stWardInfo> m_liWards;//��λͳ��
	 vector<stPlayerState> m_vPlayers;	//��һ���������
private:
	//���ָ��
	stPlayerState* _GetPlayerByFlag(int IndexFlag);
	stPlayerState* _GetPlayerByPtr(int pUnit);
	

	CLOLSkillManager m_SkillManager;
	CD3DManager m_D3DManager;

	struct{
	RECT rectMiniMap;//С��ͼ
	RECT rectFrientHead;//���Ӣ��ͷ��
	RECT rectEnemyDeadHead;//�з�ͷ��
	RECT rectScreen;//��Ļ��С

	RECT rectMenu;//�˵�
	POINT ptMouse;//���λ��
	}m_DrawParam;

	//Draw������
	void Draw_Hero_SkillBar(void* pUnit,int posX,int posY);//XYΪ���ܿ򱳾�ͼƬ
	void DrawRightEnemyHead();//�����Ҳ�з�Ӣ��ͷ��
	void DrawLeftFriendHead();//���Ӣ��ͷ��
	//Menu
	void Draw_MenuTip(int x, int y,CStringA szTitle,CStringA szInfo,bool bOpen);
	

	struct{
		bool m_bResLoad;
		CStringA szPathImgs;//�ٻ�ʦ����,HUD
		CStringA szPathGame;//Ӣ��ͷ��
		//CComCriticalSection m_CSRes;
	}m_Res;
};

 int HowLong2Point(POINT pt1,POINT pt2);
int MathHPFontColor(int CurrentHP,int MaxHp);
unsigned int MyFindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen);