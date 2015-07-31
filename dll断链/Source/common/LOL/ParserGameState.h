#ifndef _PARSERGAMESTATE
#define _PARSERGAMESTATE

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
using namespace std;
#include "LOLHeroDef.h"
#include "LOLDef.h"
struct stSharePlayerMem;


#define LOLPARSERVERSION 20150609



//���ڲ�ʹ��
#define LEN_CHECK 0x200 //���Bufferͷ�Ŀ��С
#define PASSSIZE 0x05
//�����Ƶ��������͹���ṹ��
struct DataInfoType{
	string szEN;
	string szCN;
	unsigned int Tag;
};
//���ڲ�ʹ��
struct stPlyayerData{
	string szTag;		//��һ������еı�־λ
	unsigned char byteTag;	//��������еı�־λ
	unsigned int Data;	//����
	stPlyayerData()
	{
		byteTag=0;
		Data=0;
	}
};

class CParserGameState
{
public:
	CParserGameState(void);
	~CParserGameState(void);

	//���ؽ����������ļ���
	enumBUFFTYPE Parser(unsigned char* pBuffer,unsigned int Len);//�������ݲ���
	 stLOLGame* GetData(){return &m_Data;};//�����Ժ󷵻�ָ��


	 //ע��ʱ��
	 void MemShareInit(stSharePlayerMem * pMem){m_pShareMem=pMem;};
	 //�ڲ��ú���
	 unsigned int FuckC3(unsigned char* pBuffer,unsigned int Len);	//��ѹ�������ļ�,ȥ����Ч��C3λ.�Ḳ��ԭ��pBuffer������,���ؽ�ѹ�����ݴ�С
	unsigned int GetTureSize(){return _ParserData.TureSize;};			//���ؽ�ѹ�����ݶδ�С
	
private:
	enumBUFFTYPE GetBufferType(unsigned char* pBuffer,unsigned int Len);//��������
	enumBUFFTYPE Parser_GameSate(unsigned char* pBuffer, unsigned int Len);//����ս����Ϣ
	stPlayer GetFirstPlayer(unsigned char* pBuffer,unsigned int Len,unsigned int* CurrentPos );//��һ���������
	stPlayer GetPlayers(unsigned  char* pBuffer,unsigned int Len,unsigned int* CurrentPos );//����9���������
	 bool SavePlayerData( stPlayer& thePlayer,vector<stPlyayerData>& theDatas );				//�����������
	 bool SetPlayerDataByFlag(string szFlag,unsigned int Data, stPlayer& thePlayer);			//������ҵ�һ��������
	void GetGameMode(unsigned  char* pBuffer,unsigned int Len);
	 
	 
	 stLOLGame m_Data;			//���������Ϸ����
	 stSharePlayerMem* m_pShareMem;//ע��ʱ�������ݵ��ڴ�

	struct{
		unsigned int TureSize;			//��ʵ��С
		 int PosMid_EndGame;//EndGamexxxxxxx ���λ,�ж϶���ֽ���������ֽ�λ��
		unsigned int Pos_messagingDestinatio;	//��Ϸ��β���
		bool bPassMid;				//����ֽ���
		bool bEnd;					//�Ƿ񵽽�β

		bool bQueueTargetFlat;				//flag�Ƿ������
		stPlyayerData WinLoseFlag;
		vector<stPlyayerData> vTargetFlag;	//���ݶ����ͺ�˳��,������һ���������
		int T1PlayerCount;
		int T2PlayerCount;
		bool bFuckC3;

		bool _bIsAIGame;
		int _MatchServerGameMidFlag;//������¼��
	}_ParserData;//�ڲ�ʹ�ýṹ��

};


#endif
