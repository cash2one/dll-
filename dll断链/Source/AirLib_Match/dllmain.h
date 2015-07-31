#pragma once


extern "C"  __declspec(dllexport)  bool AirLib_Match_Init();
bool SendMessagetoApp(void* pBuffer,unsigned int Len,int Commadnid);
CStringA GetServerNameByID(unsigned int ServerID);

#pragma pack(push)
#pragma pack(1)
//������������ͨ����_�����½ST���˺���
struct stSharePlayerMem_Account{
	wchar_t account[16];
	char ST[128];
	char IP[32];
	unsigned int Port;
};

#define	LOCAL_CMD_LOGININFO	1	//��½�󴫸������������Ϣ
#define	LOCAL_CMD_RESULT	2		//��Ϸ��������������ս��
//�����������ֳ������Ϣ
#define LOCAL_CMD_MATCH_CREATEROOM 3  //��������
#define LOCAL_CMD_MATCH_JOINROOM 4		//���뷿��
#define LOCAL_CMD_MATCH_STARTGAME 5		//��ʼ��Ϸ
#define LOCAL_CMD_MATCH_SELECTSERVER 6  //���������˺�����

#define LOCAL_CMD_MATCH_CHECK_REQ 10		//�ͻ������������ѯ��������뷿��
#define LOCAL_CMD_MATCH_CHECK_RSP 11

#define LOCAL_CMD_MATCH_CHECK_STARTGAME_REQ 12		//�ͻ������������ѯ��������뷿��
#define LOCAL_CMD_MATCH_CHECK_STARTGAME_RSP 13

#define LOCAL_CMD_MATCH_CHECK_QUITGAME_REQ 14		//��ѡ�˽���,�ֶ��˳���Ϸ
				#define LEAVE_ROOM_QUIT_ROOM	2	//�˷���
				#define LEAVE_ROOM_QUIT_BAN		3	//��BP����

// LOCAL_CMD_MATCH_CREATEROOM / LOCAL_CMD_MATCH_JOINROOM / LOCAL_CMD_MATCH_STARTGAME
struct stLOCAL_CMD_MATCH_MSG
{
	BYTE 	bSucc;	// �Ƿ�ɹ�
};


struct LocalCmdHeader
{
	int	nLen;
	int	nCmdId;
};

struct stLOCAL_CMD_LOGININFO{//LOCAL_CMD_LOGININFO�ṹ��
	char szName[128];//���ID
	char szServer[128];//����������
	unsigned int ServerID;//������ID
	unsigned int Head;//ͷ��
	int Level;//�˺ŵȼ�
	long long QQ;//QQ��
	long long LOLID;//LOLȫ��ΨһID
};

struct LolClientConfig
{
	int		nLocalPort;	// local server socket listen port
	HWND	hwndSingleton;	// �����ã������ھ��
	char	szCGAPath[MAX_PATH];	//���ֳ����Ŀ¼
	struct{
		bool	bMatch;		//�Ƿ����ģʽ
		bool	bCreate;	// true���������䣬false�����뷿��
		//short	nReserved;
		bool	bOwnerCamp;	// �Ƿ�����Ӫ
		BYTE	MapIndex;//��ͼ , 0=�ٻ�ʦϿ��,1=ˮ��֮��,2=Ť������,3=������Ԩ
		BYTE	ViewIndex;//�ۿ�ģʽ , 0=���ܹ�ս,1=������,2=����ͬ�������,3=���޺���
		BYTE	GameMode;//��Ϸģʽ, 0=��ѡģʽ,1=����ģʽ,2=���ģʽ		// ��Ϸ������Ϣ
		bool	bManual;	// ���ֶ�
		BYTE	nReserved;

		UINT	ServerID;	//������ID
		char	szRoomName[128];//������
		char	szPassword[128];//����
	}Match;
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

#pragma pack(pop)



