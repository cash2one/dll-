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



//类内部使用
#define LEN_CHECK 0x200 //检查Buffer头的块大小
#define PASSSIZE 0x05
//二进制的数据类型归类结构体
struct DataInfoType{
	string szEN;
	string szCN;
	unsigned int Tag;
};
//类内部使用
struct stPlyayerData{
	string szTag;		//第一个玩家有的标志位
	unsigned char byteTag;	//后面玩家有的标志位
	unsigned int Data;	//数据
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

	//本地解析二进制文件用
	enumBUFFTYPE Parser(unsigned char* pBuffer,unsigned int Len);//解析数据部分
	 stLOLGame* GetData(){return &m_Data;};//解析以后返回指针


	 //注入时用
	 void MemShareInit(stSharePlayerMem * pMem){m_pShareMem=pMem;};
	 //内部用函数
	 unsigned int FuckC3(unsigned char* pBuffer,unsigned int Len);	//解压二进制文件,去除无效的C3位.会覆盖原有pBuffer缓冲区,返回解压后数据大小
	unsigned int GetTureSize(){return _ParserData.TureSize;};			//返回解压后数据段大小
	
private:
	enumBUFFTYPE GetBufferType(unsigned char* pBuffer,unsigned int Len);//数据类型
	enumBUFFTYPE Parser_GameSate(unsigned char* pBuffer, unsigned int Len);//分析战绩信息
	stPlayer GetFirstPlayer(unsigned char* pBuffer,unsigned int Len,unsigned int* CurrentPos );//第一个玩家数据
	stPlayer GetPlayers(unsigned  char* pBuffer,unsigned int Len,unsigned int* CurrentPos );//后续9个玩家数据
	 bool SavePlayerData( stPlayer& thePlayer,vector<stPlyayerData>& theDatas );				//保存玩家数据
	 bool SetPlayerDataByFlag(string szFlag,unsigned int Data, stPlayer& thePlayer);			//保存玩家的一个数据项
	void GetGameMode(unsigned  char* pBuffer,unsigned int Len);
	 
	 
	 stLOLGame m_Data;			//解析后的游戏数据
	 stSharePlayerMem* m_pShareMem;//注入时传输数据的内存

	struct{
		unsigned int TureSize;			//真实大小
		 int PosMid_EndGame;//EndGamexxxxxxx 标记位,判断队伍分界结束最后的字节位置
		unsigned int Pos_messagingDestinatio;	//游戏结尾标记
		bool bPassMid;				//队伍分界线
		bool bEnd;					//是否到结尾

		bool bQueueTargetFlat;				//flag是否处理完毕
		stPlyayerData WinLoseFlag;
		vector<stPlyayerData> vTargetFlag;	//数据段类型和顺序,包含第一个玩家数据
		int T1PlayerCount;
		int T2PlayerCount;
		bool bFuckC3;

		bool _bIsAIGame;
		int _MatchServerGameMidFlag;//比赛服录像
	}_ParserData;//内部使用结构体

};


#endif
