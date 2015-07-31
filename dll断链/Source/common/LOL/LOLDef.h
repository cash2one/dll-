#ifndef _LOLDEF
#define _LOLDEF


enum LOLGAMEMODE{//��Ϸģʽ
	CLASSIC=1,		//����ģʽ
	ARAM=2,			//���Ҷ�
	ODIN=3,			//ͳ��ս��(����ģʽ)
	ASCENSION=4, //��������ս
	KINGPORO=5 //���޴��Ҷ�
};
enum LOLGAMETYPE{//ƥ��ģʽ
	NORMAL=1,					//��ͨƥ��
	NONE=2,							//��ƥ��ģʽ(�Զ��巿��)
	RANKED_SOLO_5x5=3,	//5v5��λ
	NORMAL_3x3=4,				//3v3ƥ��
	ARAM_UNRANKED_5x5=5,//���Ҷ�5v5
	ODIN_UNRANKED=6,		//ͳ��ս��ƥ��
	BOT=7,								//ƥ��AI
	HEXAKILL=8,							//6v6ģʽ
	KING_PORO=9,						//�����Ҷ�
	URF=10,							//���޻���
	MID6V6=11				//6v6ģʽ
};


enum enumBUFFTYPE{//�����󷵻صĽ��
	TYPE_EMPTY=1024,	//��Ч�����ݶ�
	TYPE_GAMESTATE,	//��Ϸ������ս����
	TYPE_GAMESTATE_ERROR//��Ϸս����������
};
#define LOLMAXPLAYER 12

struct stGameState{//�ϴ����ݵ�������Ϣ,��Ϊ�ļ�β�ϴ������ݿ�
	//ȫ������,ͨ��������õ�
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
	unsigned int HeadImgID;		//���ͷ��ID
	unsigned int RateNumber;	//ʤ��
	//548Byte+

	struct{//��Ϸ���������������,����Ϸ��ע�������д
		char szHero[32];
		char szPlayer[32];
		unsigned int Team;
	}PlayerData[LOLMAXPLAYER];
};



//һ�����
struct stPlayer{
	bool bParserRight;	//�����Ƿ�����
	long long globalPlayerID;	//ȫ��Ψһ��ϷID,ͬһ��QQ���ڲ�ͬ�����������˺��Ժ�Ҳ��ȫ��Ψһ�˺�
	int Win;		//1=win  0=loss
	char szPlayerName[128];	//�����Ϸ��ɫ��
	unsigned int HeroID;		//Ӣ��ID
	char szHeroName[128];	//Ӣ����
	int TEAM;					//��Ӫ , 0x64������, 0xC8���

	//��Ϸ�ڶ�Ӧ������
	int	LEVEL;					//�ȼ�

	int	CHAMPIONS_KILLED;			//��ɱK
	int	ASSISTS;					//����A
	int	NUM_DEATHS;				//����D

	int	LARGEST_KILLING_SPREE;			//�����ɱ
	int	LARGEST_MULTI_KILL;			//��߶�ɱ

	int	TOTAL_DAMAGE_DEALT_TO_CHAMPIONS;		//��Ӣ�۵����˺�
	int	PHYSICAL_DAMAGE_DEALT_TO_CHAMPIONS;	//��Ӣ�۵������˺�
	int	MAGIC_DAMAGE_DEALT_TO_CHAMPIONS;		//��Ӣ�۵ķ����˺�
	int	TRUE_DAMAGE_DEALT_TO_CHAMPIONS;		//��Ӣ�۵���ʵ�˺�

	int	TOTAL_DAMAGE_DEALT;			//������˺�
	int	PHYSICAL_DAMAGE_DEALT_PLAYER;		//��������˺�
	int	MAGIC_DAMAGE_DEALT_PLAYER;		//��������˺�
	int	TRUE_DAMAGE_DEALT_PLAYER;		//�����ʵ�˺�
	int	LARGEST_CRITICAL_STRIKE;			//��󱩻��˺�

	int	TOTAL_HEAL;				//��������
	int	TOTAL_DAMAGE_TAKEN;			//�������˺�
	int	PHYSICAL_DAMAGE_TAKEN;			//���������˺�
	int	MAGIC_DAMAGE_TAKEN;			//���ܷ����˺�
	int	TRUE_DAMAGE_TAKEN;			//������ʵ�˺�

	int	GOLD_EARNED;				//��Ǯ
	int	TURRETS_KILLED;				//������
	int	MINIONS_KILLED;				//С����ɱ
	int	BARRACKS_KILLED;				//���Ӫ��
	int	NEUTRAL_MINIONS_KILLED;			//Ұ�ֻ�ɱ
	int	NEUTRAL_MINIONS_KILLED_ENEMY_JUNGLE;	//�з�Ұ�ֻ�ɱ
	int	NEUTRAL_MINIONS_KILLED_YOUR_JUNGLE;	//����Ұ�ֻ�ɱ

	int	TOTAL_TIME_SPENT_DEAD;			//�ܼ�����ʱ��
	int	TOTAL_TIME_CROWD_CONTROL_DEALT;		//����ſ�ʱ��

	int	ITEM0;					//��Ʒ1
	int	ITEM1;					//��Ʒ2
	int	ITEM2;					//��Ʒ3
	int	ITEM3;					//��Ʒ4
	int	ITEM4;					//��Ʒ5
	int	ITEM5;					//��Ʒ6
	int	ITEM6;					//��Ʒ7

	int	SIGHT_WARDS_BOUGHT_IN_GAME;	//��ͨ�۹�����
	int	VISION_WARDS_BOUGHT_IN_GAME;		//���۹�����
	int	WARD_KILLED;				//������
	int	WARD_PLACED;				//������

//*****************************��������ݶ�¼�����ݿ�ʱ��������~
	int	LOSE;					//ʧ�ܷ�	
	int	WIN;					//ʧ�ܷ�	

	//ͳ��ս��ģʽ���е�����
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

//һ����Ϸ
enum ShareDataReadyType{
	ShareDataReadyType_NOT_Ready = 0,
	ShareDataReadyType_Ready = 1,       //������
	ShareDataErrInfoReadyType_Ready = 2, 
};



struct stLOLGame{
	stGameState GameState;//�ϴ�����Ϣ

	long long GameID;//�ó������ڸ÷�������ΨһID
	unsigned int GameLengh;	//��Ϸ����,��λ��
	LOLGAMEMODE GameMode; //��Ϸģʽ
	LOLGAMETYPE GameType;	//ƥ��ģʽ
	char GameTypeStr[40];

	unsigned int PlayerCount;//�������
	unsigned int WinnerTeam;//ʤ����  100��  200��

	unsigned int TEAM_BLUE_K;//����ɱ����
	unsigned int TEAM_BLUE_D;//����������
	unsigned int TEAM_BLUE_A;//����������

	unsigned int TEAM_RED_K;//���ɱ����
	unsigned int TEAM_RED_D;//���������
	unsigned int TEAM_RED_A;//���������


	stPlayer Players[LOLMAXPLAYER];//10����ҵ�����
};

struct stLOLShareMem {
	int stLoLShareMemVersion;
	unsigned int  ShareDataReady;	//1:ready  0:not ready
	unsigned int  ShareDataErrNum;	//������

	stLOLGame game;

	stLOLShareMem() {
		stLoLShareMemVersion = 1;
		ShareDataErrNum = 0;
		ShareDataReady = ShareDataReadyType_NOT_Ready;
	}
};


//������������ͨ����
struct stSharePlayerMem{

	stGameState GameState;//��ȡClient ��ȡQQ,����������Ϣ

	struct{
		unsigned int PlayerHead;//���ͷ��
	}LocalGameData;

	struct{//�������ͨ������,�����ϴ���־
		//������ԭʼ�ļ�.��С���̶�
		char pDataBuffer[1024*20];//һ��10�˱���������10K����
		unsigned int LenBuffer;//
	}Buffer;

	struct{//�ͻ��˲�ѯ���������ݻص���
		unsigned int LenBuffer;//
		char pBuffer[1024*1024];//һ��10�˱���������10K����
	}Back;
};


#endif