#include "ParserGameState.h"
#include <stdio.h>
#include <string.h>
#include "LOLHeroDef.h"
#include "LOLDef.h"
extern LOLHERO stLOLHeroData[];
	using namespace std;


	int  Utf82Unicode(wchar_t* out,  int outsize , char* in,int insize) ;
	std::string w2c(const wchar_t * pw);



	int SkipToChar(unsigned char* pBuffer,unsigned int Len,unsigned int * Pos,unsigned char Flag)
	{
		int Now=*Pos;
		while(*Pos<Len)
		{
			unsigned char uC=pBuffer[*Pos];
			if (uC==Flag)
				break;
			(*Pos)++;
		}
		return *Pos-Now;
	}

	unsigned int MemSearchStr(unsigned char* pBuffer,unsigned int Len,string szTag)
	{
		int Result=-1;
		char* pTag=new char[szTag.length()+10];
		memset(pTag,0,szTag.length()+10);
		int szLen=szTag.length();

		
		strcpy(pTag,szTag.c_str());
		for (unsigned int i=0;i<Len-szTag.length()-1;i++)
		{
			if (memcmp(pTag,&pBuffer[i],szLen)==0)
			{
				Result=i;
				break;
			}
		}
		delete[] pTag;
		return Result;
	}

	string ReadStrBy06Pos(unsigned char* pBuffer,unsigned int* Pos)
	{
		unsigned int CurrentPos=*Pos+1;//Skix 0x06
		unsigned char LenBuffer=pBuffer[CurrentPos] ;
		string szResult;
		bool bErrorFlag=0;

		if (LenBuffer<0x80)
		{
			CurrentPos++;//Skip Len tag

			LenBuffer-=1;
			LenBuffer/=2;
			char chBuf[1024]={0};


			memcpy(chBuf,&pBuffer[CurrentPos],LenBuffer);
			 szResult=chBuf;
		}else{//Len=0x81
			szResult="*";
			LenBuffer=0;
			for (unsigned int i=0;i<20;i++)
			{
				unsigned int Byt=pBuffer[CurrentPos+i];
				if (Byt && Byt!=0x05)
				{
					char szTemp[128]={0};
					sprintf(szTemp,"%X",Byt);
					//szResult+=szTemp;
					LenBuffer++;
				}else{
					break;
				}
			}
		}
		
		CurrentPos+=LenBuffer;
		if (bErrorFlag)
		{
			*Pos+=2;//Pos+
		}else{
			*Pos=CurrentPos;//Pos+
		}
		
		return szResult;
	}

	string ReadAnsiStrBy06Pos(unsigned char* pBuffer,unsigned int* Pos)
	{
		string sz=ReadStrBy06Pos(pBuffer,Pos);

		return sz;
	}

#ifdef __linux
	#include <iconv.h>
	string UTF8toANSI(const std::string &from){
		char *inbuf=const_cast<char*>(from.c_str());
		size_t inlen = strlen(inbuf);
		size_t outlen = inlen *4;
		char *outbuf = (char *)malloc(inlen * 4 );
		bzero( outbuf, inlen * 4);
		char *in = inbuf;
		char *out = outbuf;
		iconv_t cd=iconv_open("GBK","UTF-8");
		iconv(cd,&in,&inlen,&out,&outlen);
		iconv_close(cd);
		string szResult=outbuf;
		delete[]outbuf;
		return szResult;
	}
#endif

	string ReadUtf8StrBy06Pos(unsigned char* pBuffer,unsigned int* Pos)
	{
		string sz=ReadStrBy06Pos(pBuffer,Pos);

		string szResult;
#ifdef __linux
			return UTF8toANSI(sz);
#else
		wchar_t wBuff[1024]={0};
		Utf82Unicode(wBuff,1024,(char*)sz.c_str(),sz.length());
		wstring strUnicode=wBuff;
		char szBuff[1024]={0};
		wcstombs(szBuff,strUnicode.c_str(),strUnicode.size()*2);
		szResult=szBuff;
#endif
		 //cout<<szResult;
		return szResult;
	}


	unsigned int ReadDoubleBy05Pos(unsigned char* pBuffer,unsigned int* Pos)
	{
		unsigned int CurrentPos=*Pos+1;//Skix 0x05
		char ByteSwapDouble[8]={0};

		int PA=0,PB=7;
		for (unsigned int i=0;i<8;i++)
		{
			ByteSwapDouble[PB]=pBuffer[CurrentPos+PA];

			PA++;
			PB--;
		}
		double dbResult=*((double*)ByteSwapDouble);
		unsigned int Result=(unsigned int)dbResult;
		*Pos+=9;
		return Result;
	}

	bool PassDouble(unsigned char* pBuffer,int Len,unsigned int* Pos,int PassCount)
	{
		unsigned int TempPos=*Pos;

		for (int i=0;i<PassCount;i++)
		{
			if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
				return 0;
			int TempDouble=ReadDoubleBy05Pos(pBuffer,&TempPos);//endGamexxxx
		}

		*Pos=TempPos;

		return 1;
	}

	long long ReadLongDoubleBy05Pos(unsigned char* pBuffer,unsigned int* Pos)
	{
		unsigned int CurrentPos=*Pos+1;//Skix 0x05
		char ByteSwapDouble[8]={0};

		int PA=0,PB=7;
		for (unsigned int i=0;i<8;i++)
		{
			ByteSwapDouble[PB]=pBuffer[CurrentPos+PA];

			PA++;
			PB--;
		}
		double dbResult=*((double*)ByteSwapDouble);
		long long Result=(long long)dbResult;
		*Pos+=9;
		return Result;
	}
	/*
	void* MyGetMemShare(string szName,unsigned int Size)
	{
		void* pResult=0;
		HANDLE hMap;
		hMap=CreateFileMappingA((HANDLE)-1,0,PAGE_READWRITE,0,Size,szName);


		int LE_CReateMap=GetLastError();

		if (hMap!=INVALID_HANDLE_VALUE)
		{
			pResult=MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0);
		}else{
			return 0;
		}

		if (pResult && LE_CReateMap!=ERROR_ALREADY_EXISTS)
		{//第一次创建
			//初始化数据
			RtlZeroMemory(pResult,Size);
		}


		return pResult;
	}
	*/


	CParserGameState::CParserGameState(void)
	{
		setlocale(LC_ALL, "chs");
		m_pShareMem=0;
	}


	CParserGameState::~CParserGameState(void)
	{
	}

enumBUFFTYPE CParserGameState::Parser(unsigned char* pBuffer,unsigned int Len )
{
	//Clear
//	MessageBoxA(0,0,0,0);
	_ParserData._bIsAIGame=0;
	_ParserData.vTargetFlag.clear();
	_ParserData.PosMid_EndGame=0;
	_ParserData.TureSize=FuckC3(pBuffer,Len);//解压
	_ParserData.bPassMid=0;
	_ParserData.bEnd=0;
	_ParserData.T1PlayerCount=1;
	_ParserData.T2PlayerCount=0;
	_ParserData.bQueueTargetFlat=0;
	_ParserData.WinLoseFlag.byteTag=0;
	_ParserData.WinLoseFlag.Data=0;
	_ParserData.WinLoseFlag.szTag="";
	_ParserData._MatchServerGameMidFlag=0;
	memset(&m_Data,0,sizeof(m_Data));
	//Fuck 0xC3
	switch(GetBufferType(pBuffer,_ParserData.TureSize))
	{
	case TYPE_GAMESTATE:
		{
			//保存数据
			if (m_pShareMem)
			{//作为客户端注入DLL时
				cout<<"游戏结束,捕获到战绩,AddB"<<endl;

				//填充基本信息
				char chVersion[128]={0};
				sprintf(chVersion,"Version %d",LOLPARSERVERSION);
				strcpy(m_pShareMem->GameState.szVersion,chVersion);

				memcpy(&m_Data.GameState, &m_pShareMem->GameState, sizeof(stGameState));
				Parser_GameSate(pBuffer, _ParserData.TureSize);

				return TYPE_GAMESTATE;
			}
		}
	}

	return TYPE_EMPTY;
}

enumBUFFTYPE CParserGameState::GetBufferType(unsigned char* pBuffer,unsigned int Len )
{
	if (Len<LEN_CHECK)
		return TYPE_EMPTY;
	int TagOffset=MemSearchStr(pBuffer,LEN_CHECK,"EndOfGameStats");
	if (TagOffset!=-1)
	{//结束信息
		return TYPE_GAMESTATE;
	}

	return TYPE_EMPTY;
}


//数据段
enumBUFFTYPE CParserGameState::Parser_GameSate(unsigned char* pBuffer,unsigned int Len)
{
	//准备
	unsigned int Pos;
	Pos=MemSearchStr(pBuffer,Len,"spell1Id");
	if (Pos==-1)
		return TYPE_GAMESTATE_ERROR;


	if (-1!=MemSearchStr(pBuffer,Len,"CO_OP_VS_AI"))
		_ParserData._bIsAIGame=1;
	


	
	_ParserData._MatchServerGameMidFlag=MemSearchStr(pBuffer,Len,"#_com.riotgames.platform.statistics");


	//游戏时间
	_ParserData.PosMid_EndGame=MemSearchStr(pBuffer,Len,"endGame");
	if (_ParserData.PosMid_EndGame==-1)
		return TYPE_GAMESTATE_ERROR;
	unsigned int GameLenghPos=_ParserData.PosMid_EndGame-0x16;
	m_Data.GameLengh=ReadDoubleBy05Pos(pBuffer,&GameLenghPos);
	//GameID
	GameLenghPos=_ParserData.PosMid_EndGame-2;
	string szGameID=ReadStrBy06Pos(pBuffer,&GameLenghPos);
	string szGameIDNumber=szGameID.substr(7);
	m_Data.GameID=atoi(szGameIDNumber.c_str());



	//结尾标记  )messagingDestination
	_ParserData.Pos_messagingDestinatio=MemSearchStr(pBuffer,Len,")messagingDestination");
	if (_ParserData.Pos_messagingDestinatio==-1)
		return TYPE_GAMESTATE_ERROR;

	//GameMode
	GetGameMode(pBuffer,Len);


	Pos+=0x08;//Pos=第一个英雄
	stPlayer stFirstPlayer=GetFirstPlayer(pBuffer,Len,&Pos);
	m_Data.Players[0]=stFirstPlayer;

	//读取剩下玩家
	for (unsigned int i=1;i<LOLMAXPLAYER;)
	{
		if (_ParserData.bPassMid==1)
		{
			if (i<LOLMAXPLAYER/2)
			{
				i=LOLMAXPLAYER/2;
			}
			_ParserData.T2PlayerCount++;
		}else{
			_ParserData.T1PlayerCount++;
		}
		if (i==7)
		int stop=0;
		m_Data.Players[i]=GetPlayers(pBuffer,Len,&Pos);
		if (_ParserData.bEnd)
		break;
		i++;
	}
	//计算玩家数
	for (unsigned int i=0;i<LOLMAXPLAYER;i++)
	{
		if (m_Data.Players[i].bParserRight)
		{
			m_Data.PlayerCount++;
		}
	}



	//设置输赢标记位
	if (stFirstPlayer.WIN==1)
	{
		for (unsigned int i=0;i<LOLMAXPLAYER/2;i++)
			m_Data.Players[i].Win=1;		

		m_Data.WinnerTeam=stFirstPlayer.TEAM;

	}else if (stFirstPlayer.LOSE==1)
	{
		for (unsigned int i=LOLMAXPLAYER/2;i<LOLMAXPLAYER;i++)
			m_Data.Players[i].Win=1;

		if (stFirstPlayer.TEAM==100)
		{
			m_Data.WinnerTeam=200;
		}else{
			m_Data.WinnerTeam=100;
		}
		
	}

	//设置队伍
	for (unsigned int i=1;i<LOLMAXPLAYER;i++)
	{
		if (i<LOLMAXPLAYER/2)
		{
			m_Data.Players[i].TEAM=m_Data.Players[0].TEAM;
		}else{
			if (m_Data.Players[0].TEAM==0x64)
			{
				m_Data.Players[i].TEAM=0xC8;
			}else{
				m_Data.Players[i].TEAM=0x64;
			}
		}
	}


	
	
		//排序
		stLOLGame TempData;
		memset(&TempData,0,sizeof(TempData));

	unsigned int CopyCount=0;
	for (unsigned int j=0;j<LOLMAXPLAYER;j++)
	{//遍历排序数组
		string szHero= m_Data.GameState.PlayerData[j].szHero;
		string szPlayer=m_Data.GameState.PlayerData[j].szPlayer;
		unsigned int  Team=m_Data.GameState.PlayerData[j].Team;
		if (szHero=="" || szPlayer=="" || Team==0)
		{//空位置,跳过
			continue;
		}

		for (unsigned int i=0;i<LOLMAXPLAYER;i++)
		{

			if ( (szHero==m_Data.Players[i].szHeroName || szPlayer==m_Data.Players[i].szPlayerName) && Team==m_Data.Players[i].TEAM)
			{//相符
				//填充玩家名
				stPlayer CopyPlayer;
				memset(&CopyPlayer,0,sizeof(CopyPlayer));
				CopyPlayer=m_Data.Players[i];
				strcpy(CopyPlayer.szHeroName,szHero.c_str());
				strcpy(CopyPlayer.szPlayerName,szPlayer.c_str());
				TempData.Players[j]=CopyPlayer;
				CopyCount++;
				break;
			}

		}
	}

	if (CopyCount==m_Data.PlayerCount)
	{
		for (unsigned int i=0;i<LOLMAXPLAYER;i++)
		{
			m_Data.Players[i]=TempData.Players[i];
		}
	}

		//检查KDA警戒线
	for (unsigned int i=0;i<LOLMAXPLAYER;i++)
	{
		if (m_Data.Players[i].CHAMPIONS_KILLED>=100)
		{
			m_Data.Players[i].CHAMPIONS_KILLED=100;
		}
		if (m_Data.Players[i].NUM_DEATHS>=100)
		{
			m_Data.Players[i].NUM_DEATHS=100;
		}
		if (m_Data.Players[i].ASSISTS>=100)
		{
			m_Data.Players[i].ASSISTS=100;
		}
	}


	//计算kda;
	unsigned int TEAM_BLUE_K=0;
	unsigned int TEAM_BLUE_D=0;
	unsigned int TEAM_BLUE_A=0;

	unsigned int TEAM_RED_K=0;
	unsigned int TEAM_RED_D=0;
	unsigned int TEAM_RED_A=0;

	for (unsigned int i=0;i<LOLMAXPLAYER;i++)
	{
		if (m_Data.Players[i].TEAM==100)
		{//蓝队
			TEAM_BLUE_K+=m_Data.Players[i].CHAMPIONS_KILLED;
			TEAM_BLUE_D+=m_Data.Players[i].NUM_DEATHS;
			TEAM_BLUE_A+=m_Data.Players[i].ASSISTS;
		}else if(m_Data.Players[i].TEAM==200){//红队
			TEAM_RED_K+=m_Data.Players[i].CHAMPIONS_KILLED;
			TEAM_RED_D+=m_Data.Players[i].NUM_DEATHS;
			TEAM_RED_A+=m_Data.Players[i].ASSISTS;
		}
	}
	m_Data.TEAM_BLUE_K=TEAM_BLUE_K;
	m_Data.TEAM_BLUE_D=TEAM_BLUE_D;
	m_Data.TEAM_BLUE_A=TEAM_BLUE_A;
	m_Data.TEAM_RED_K=TEAM_RED_K;
	m_Data.TEAM_RED_D=TEAM_RED_D;
	m_Data.TEAM_RED_A=TEAM_RED_A;

	//英雄名换算英雄数字ID
	for (unsigned int i=0;i<LOLMAXPLAYER;i++)
	{
		unsigned int Index=0;
		while(1)
		{
			if (stLOLHeroData[Index].ID==0)
				break;
			string strHeroName=m_Data.Players[i].szHeroName;
			if (strHeroName  == stLOLHeroData[Index].szEN	)
			{
				m_Data.Players[i].HeroID=stLOLHeroData[Index].ID;
				break;
			}
			Index++;
		}
	}


#if 0
	if (!m_Data.GameID ||!m_Data.GameState.ServerID ||!m_Data.GameState.QQ )
	//if (!m_Data.GameID ||!m_Data.GameLengh ||!m_Data.GameMode || !m_Data.GameType)
	{
		return TYPE_GAMESTATE_ERROR;
	}
#endif

	return TYPE_GAMESTATE;
}

//解析第一个玩家,第一个玩家英雄名指针.
stPlayer CParserGameState::GetFirstPlayer(unsigned char* pBuffer,unsigned int Len,unsigned int* CurrentPos )
{
	unsigned int TempPos=*CurrentPos;
	stPlayer FirstPlayer;
	memset(&FirstPlayer,0,sizeof(stPlayer));
	//英雄名
	string szHeroName=ReadAnsiStrBy06Pos(pBuffer,&TempPos);
	strcpy(FirstPlayer.szHeroName,szHeroName.c_str());

	//阵营
	TempPos+=0x17;
	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return FirstPlayer;
	FirstPlayer.TEAM=ReadDoubleBy05Pos(pBuffer,&TempPos);
	//第一个玩家的数据段
	TempPos=MemSearchStr(pBuffer,Len,"futureDat");
	if (TempPos==-1)
	return FirstPlayer;
	//跳过
	TempPos+=0X0C;
	while(TempPos<Len)
	{
		stPlyayerData add;
		//读Double
		add.Data=ReadDoubleBy05Pos(pBuffer,&TempPos);
		//读Flag
		if (pBuffer[TempPos]!=0x06)
			TempPos++;
		add.szTag=ReadStrBy06Pos(pBuffer,&TempPos);

		if (add.szTag=="WIN" || add.szTag =="LOSE")
			_ParserData.WinLoseFlag=add;

		_ParserData.vTargetFlag.push_back(add);



		//Skip xx 01 0A 11 04 00x5
		//End xx  01  05 -----
		TempPos++;//Skip 01

		if (pBuffer[TempPos]==0x05 || pBuffer[TempPos]==0x04)
		{//loop
			break;
		}else{//end
			TempPos+=4;
		}
	}


	//Find Next 0x05
	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return FirstPlayer;
	int Temp1=ReadDoubleBy05Pos(pBuffer,&TempPos);//0x1E

	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return FirstPlayer;
	FirstPlayer.globalPlayerID=ReadLongDoubleBy05Pos(pBuffer,&TempPos);//0

	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return FirstPlayer;
	int Temp3=ReadDoubleBy05Pos(pBuffer,&TempPos);//4

	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return FirstPlayer;
	int Temp5=ReadDoubleBy05Pos(pBuffer,&TempPos);//344

	//玩家id
	if (SkipToChar(pBuffer,Len,&TempPos,0x06)>=PASSSIZE)
		return FirstPlayer;
	string szPlayerName=ReadUtf8StrBy06Pos(pBuffer,&TempPos);
	strcpy(FirstPlayer.szPlayerName,szPlayerName.c_str());
	int Temp6=ReadDoubleBy05Pos(pBuffer,&TempPos);//0x333
	int Temp7=ReadDoubleBy05Pos(pBuffer,&TempPos);//0xB
	int Temp8=ReadDoubleBy05Pos(pBuffer,&TempPos);//0xB

	FirstPlayer.bParserRight=1;
	*CurrentPos=TempPos;


	if (_ParserData.PosMid_EndGame-TempPos<0x40)
	{//接近EndGame字段
		TempPos++;
		if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
			return FirstPlayer;
		int Temp6=ReadDoubleBy05Pos(pBuffer,&TempPos);
		string szTemp=ReadStrBy06Pos(pBuffer,&TempPos);//NONE
		m_Data.GameLengh=ReadDoubleBy05Pos(pBuffer,&TempPos);//游戏时间
		if (_ParserData._bIsAIGame)
		{//AI
			TempPos=_ParserData.PosMid_EndGame+0x7F;
		}else{
			TempPos=_ParserData.PosMid_EndGame+0x2A;
		}
		_ParserData.bPassMid=1;
	}

	



	//录入玩家数据
	if (SavePlayerData(FirstPlayer,_ParserData.vTargetFlag))
	{
		FirstPlayer.bParserRight=1;

	}else{
		FirstPlayer.bParserRight=0;
	}


	*CurrentPos=TempPos;
	return FirstPlayer;
}

//解析第一个玩家,第一个玩家英雄名指针.
stPlayer CParserGameState::GetPlayers(unsigned char* pBuffer,unsigned int Len,unsigned int* CurrentPos )
{
	unsigned int TempPos=*CurrentPos;
	stPlayer thePlayer;
	memset(&thePlayer,0,sizeof(stPlayer));

	if (SkipToChar(pBuffer,Len,&TempPos,0x06)>6)
	{
		if (_ParserData._MatchServerGameMidFlag!=-1)
		{//体验服比赛
			TempPos=_ParserData._MatchServerGameMidFlag+0x56;

			if (SkipToChar(pBuffer,Len,&TempPos,0x06)>6)
			{
					return thePlayer;
			}
		}else{
					return thePlayer;
		}
	}

	//英雄名
	string szHeroName=ReadAnsiStrBy06Pos(pBuffer,&TempPos);
	strcpy(thePlayer.szHeroName,szHeroName.c_str());
	if (szHeroName.compare(0,1,"*")==0)
	{
		if (SkipToChar(pBuffer,Len,&TempPos,0x05)>0x10)
		return thePlayer;
		strcpy(thePlayer.szHeroName,"");
	}

	int Temp0=ReadDoubleBy05Pos(pBuffer,&TempPos);//0x2c29d90c 0x3e0ea46e 0x3e16730c 0x3e16730c
	TempPos+=5;
	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)
		return thePlayer;
	int Temp01=ReadDoubleBy05Pos(pBuffer,&TempPos);//
	thePlayer.TEAM=ReadDoubleBy05Pos(pBuffer,&TempPos);//64  C8=40 69 00 00
	TempPos+=8;

	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>0x10)//跳到玩家数据段第一字节
		return thePlayer;
	vector<stPlyayerData> vDatas;
	while(TempPos<Len)
	{
		if (pBuffer[TempPos]!=0x05)
			break;
		stPlyayerData add;
		//读Double
		add.Data=ReadDoubleBy05Pos(pBuffer,&TempPos);
		//读Flag
		//Skip 06 81 xx 01 0A 11 04 00
		//End 06  81 xx  01  05 -----
		//胜利玩家位
		//       06 07  W I N 01 0A 11 04 00
		if (pBuffer[TempPos+1]==0x81 || pBuffer[TempPos+1]==0x82)
		{
			TempPos+=2;//Skip 06 81
			//读06标志位
			add.byteTag=pBuffer[TempPos]; 

			TempPos+=2;//Skip xx 01
		}else if ( pBuffer[TempPos+1]==0x07 ||  pBuffer[TempPos+1]==0x09 )
		{
			string szWin=ReadStrBy06Pos(pBuffer,&TempPos);
			add=_ParserData.WinLoseFlag;

			TempPos+=1;//Skip  01
		}else{//Error
			return thePlayer;
		}

		vDatas.push_back(add);
		//检查结尾
		if (pBuffer[TempPos]==0x0A)
		{//没到结尾
			TempPos+=4;
		}else{//结尾 0x05
			TempPos+=9;
			break;
		}
	}

	//排序
	for (int j=vDatas.size()-1 ; j>0 ; j--)
	{
		for(int i=0 ; i<j ; i++)
		{
			if (vDatas[i].byteTag>vDatas[i+1].byteTag)
			{
				stPlyayerData Temp=vDatas[i];
				vDatas[i]=vDatas[i+1];
				vDatas[i+1]=Temp;
			}
		}
	}

/*
	for (int i=0;i<vDatas.size();i++)
	printf("0x%02x  , %d  \r\n 0x%02x  , %d  %s \r\n\r\n ",vDatas[i].byteTag,vDatas[i].Data,_ParserData.vTargetFlag[i].byteTag,_ParserData.vTargetFlag[i].Data,_ParserData.vTargetFlag[i].szTag.c_str());*/

	//第二个玩家确定标记位
	if (_ParserData.bQueueTargetFlat==0&& vDatas.size()>2)
	{
		unsigned int AddCheckBytes=vDatas[1].byteTag;//第0个是Win/Lose

		//第二个玩家,处理标记位.
		for (unsigned int i=2;i<vDatas.size();i++)
		{
			AddCheckBytes+=2;
			if (AddCheckBytes!=vDatas[i].byteTag)
			{//WinLose
				vDatas[0].byteTag=AddCheckBytes;
				_ParserData.bQueueTargetFlat=1;
				break;
			}
		}

		if (		_ParserData.bQueueTargetFlat	)
		{
			//再排序
			for (int j=vDatas.size()-1 ; j>0 ; j--)
			{
				for(int i=0 ; i<j ; i++)
				{
					if (vDatas[i].byteTag>vDatas[i+1].byteTag)
					{
						stPlyayerData Temp=vDatas[i];
						vDatas[i]=vDatas[i+1];
						vDatas[i+1]=Temp;
					}
				}
			}
		}
	}

	//第二个玩家时填充标记位
	if (_ParserData.vTargetFlag.size() && _ParserData.vTargetFlag[0].byteTag==0)
	{
		//填充
		if (vDatas.size()>=  _ParserData.vTargetFlag.size())
		{
			for (unsigned int i=0;i<_ParserData.vTargetFlag.size();i++)
			{
					_ParserData.vTargetFlag[i].byteTag=vDatas[i].byteTag;
			}
		}
	}

	if (SkipToChar(pBuffer,Len,&TempPos,0x05)>PASSSIZE)//skip02
		return thePlayer;
	thePlayer.globalPlayerID=ReadLongDoubleBy05Pos(pBuffer,&TempPos);//
	if(_ParserData._bIsAIGame && thePlayer.globalPlayerID==0)
	{
		thePlayer.globalPlayerID=1;
	}


	PassDouble(pBuffer,Len,&TempPos,2);
	

	//玩家id
	if (SkipToChar(pBuffer,Len,&TempPos,0x06)>PASSSIZE)
		return thePlayer;
	string szPlayerName=ReadUtf8StrBy06Pos(pBuffer,&TempPos);
	if (szPlayerName.compare(0,1,"*")==0 && _ParserData._bIsAIGame)
	{
				 szPlayerName="AI_BOT";
	}
	strcpy(thePlayer.szPlayerName,szPlayerName.c_str());

PassDouble(pBuffer,Len,&TempPos,3);
	//检查结尾
	if (_ParserData.Pos_messagingDestinatio-TempPos>0x130)
	{//没到结尾
		//检查队伍分界线
		if (_ParserData.bPassMid==0 && ((unsigned int)_ParserData.PosMid_EndGame<=TempPos || _ParserData.PosMid_EndGame-TempPos<0x40))
		{//接近EndGame字段
			_ParserData.bPassMid=1;
		string szTemp=ReadStrBy06Pos(pBuffer,&TempPos);//NONE
		 PassDouble(pBuffer,Len,&TempPos,2);
		if (SkipToChar(pBuffer,Len,&TempPos,0x06)>PASSSIZE)
			return thePlayer;
		 szTemp=ReadStrBy06Pos(pBuffer,&TempPos);//endGamexxxx
		 PassDouble(pBuffer,Len,&TempPos,5);
			 if (SkipToChar(pBuffer,Len,&TempPos,0x06)>PASSSIZE)
				 return thePlayer;
			 szTemp=ReadStrBy06Pos(pBuffer,&TempPos);//!sddasidoasdpomlzxc
			  PassDouble(pBuffer,Len,&TempPos,2);
			  if (SkipToChar(pBuffer,Len,&TempPos,0x06)>PASSSIZE)
				  return thePlayer;
			  szTemp=ReadStrBy06Pos(pBuffer,&TempPos);//MATCHED_GAME
			  PassDouble(pBuffer,Len,&TempPos,2);
			m_Data.GameLengh=ReadDoubleBy05Pos(pBuffer,&TempPos);//游戏时间


		if (_ParserData._bIsAIGame)
		{//AI
		//	TempPos=_ParserData.PosMid_EndGame+0x7F;
			TempPos+=0x64;
		}else{
		//TempPos=_ParserData.PosMid_EndGame+0x2A;
			TempPos+=0x10;
		}

		}
	}else{//到结尾
		_ParserData.bEnd=1;
	}


	if (SavePlayerData(thePlayer,vDatas))
	{
		thePlayer.bParserRight=1;
		
	}else{
		thePlayer.bParserRight=0;
	}
	*CurrentPos=TempPos;
	return thePlayer;
}


unsigned int CParserGameState::FuckC3(unsigned char* pBuffer,unsigned int Len)
{
	if (Len<LEN_CHECK)
	{
		return Len;
	}

	unsigned int Pos1=0;
	SkipToChar(pBuffer,0x100,&Pos1,0xC3);
	if (Pos1==0x100)
		return Len;//
	unsigned int Pos2=Pos1+1;
	SkipToChar(pBuffer,Pos1+0x100,&Pos2,0xC3);

	unsigned int SectionSize=Pos2-Pos1-1;

	if (SectionSize>0x100)
	{
		return 0;
	}


	unsigned int AddPos=0;
	unsigned int CopyPos=0;

	unsigned char* pTemp=new unsigned char[Len];
	memset(pTemp,0,Len);

	 memcpy(&pTemp[AddPos],&pBuffer[CopyPos],Pos1);
	 AddPos=Pos1;//目标
	 CopyPos=Pos1;//源

	 while(CopyPos<Len)
	 {

		if (pBuffer[CopyPos]==0xC3)
		{//复制
			if ( (Len-CopyPos)>=SectionSize )
			 {
				 CopyPos++;//Skip 0xC3
				 memcpy(&pTemp[AddPos],&pBuffer[CopyPos],SectionSize);
				 AddPos+=SectionSize;
				 CopyPos+=SectionSize;
			 }else{//复制剩余字节
				  CopyPos++;//Skip 0xC3
				  memcpy(&pTemp[AddPos],&pBuffer[CopyPos],Len-CopyPos);
				  AddPos+=Len-CopyPos;
				 break;
			 }
		 }else{
			 delete[] pTemp;
			 return 0;
		}
	 }


	 memcpy(pBuffer,pTemp,Len);
	delete[] pTemp;

	 return AddPos;
}
/*
unsigned int ServerID;//服务器ID
long long GameID;//该场比赛在该服务器内唯一ID
unsigned int GameLengh;	//游戏长度,单位秒

LOLGAMEMODE GameMode; //游戏模式

LOLGAMETYPE GameType;	//匹配模式*/




bool CParserGameState::SavePlayerData( stPlayer& thePlayer,vector<stPlyayerData>& theDatas )
{
	for (unsigned int i=0;i<theDatas.size();i++)
	{
		stPlyayerData theOne=theDatas[i];
		if (theOne.szTag=="")
		{//后面的玩家
			//用标记位去找数据字符串
			string szDataType;
			for (unsigned int j=0;j<_ParserData.vTargetFlag.size();j++)
			{
				if (_ParserData.vTargetFlag[j].byteTag==theOne.byteTag)
				{
					szDataType=_ParserData.vTargetFlag[j].szTag;
					break;
				}
			}
			if (szDataType!="")
			{//找到
				if (SetPlayerDataByFlag(szDataType,theOne.Data,thePlayer)==0	)
				{
					 continue;
				}
			}


		}else{//第一个玩家

			if (SetPlayerDataByFlag(theOne.szTag,theOne.Data,thePlayer)==0	)
			{
					continue;
			}

		}

	}
	return 1;
}

bool CParserGameState::SetPlayerDataByFlag( string szFlag,unsigned int Data, stPlayer& thePlayer )
{
	if (szFlag=="LEVEL")
	{
		thePlayer.LEVEL=Data;
		return 1;
	}
	//等级

	if (szFlag=="CHAMPIONS_KILLED")
	{
		thePlayer.CHAMPIONS_KILLED=Data;
		return 1;
	}
	//击杀
	if (szFlag=="ASSISTS")
	{
		thePlayer.ASSISTS=Data;
		return 1;
	}
	//助攻
	if (szFlag=="NUM_DEATHS")
	{
		thePlayer.NUM_DEATHS=Data;
		return 1;
	}
	//死亡
	if (szFlag=="LARGEST_KILLING_SPREE")
	{
		thePlayer.LARGEST_KILLING_SPREE=Data;
		return 1;
	}
	//最高连杀
	if (szFlag=="LARGEST_MULTI_KILL")
	{
		thePlayer.LARGEST_MULTI_KILL=Data;
		return 1;
	}
	//最高多杀

	if (szFlag=="TOTAL_DAMAGE_DEALT_TO_CHAMPIONS")
	{
		thePlayer.TOTAL_DAMAGE_DEALT_TO_CHAMPIONS=Data;
		return 1;
	}
	//对英雄的总伤害
	if (szFlag=="PHYSICAL_DAMAGE_DEALT_TO_CHAMPIONS")
	{
		thePlayer.PHYSICAL_DAMAGE_DEALT_TO_CHAMPIONS=Data;
		return 1;
	}
	//对英雄的物理伤害
	if (szFlag=="MAGIC_DAMAGE_DEALT_TO_CHAMPIONS")
	{
		thePlayer.MAGIC_DAMAGE_DEALT_TO_CHAMPIONS=Data;
		return 1;
	}
	//对英雄的法术伤害
	if (szFlag=="TRUE_DAMAGE_DEALT_TO_CHAMPIONS")
	{
		thePlayer.TRUE_DAMAGE_DEALT_TO_CHAMPIONS=Data;
		return 1;
	}
	//对英雄的真实伤害

	if (szFlag=="TOTAL_DAMAGE_DEALT")
	{
		thePlayer.TOTAL_DAMAGE_DEALT=Data;
		return 1;
	}
	//输出总伤害
	if (szFlag=="PHYSICAL_DAMAGE_DEALT_PLAYER")
	{
		thePlayer.PHYSICAL_DAMAGE_DEALT_PLAYER=Data;
		return 1;
	}
	//输出物理伤害
	if (szFlag=="MAGIC_DAMAGE_DEALT_PLAYER")
	{
		thePlayer.MAGIC_DAMAGE_DEALT_PLAYER=Data;
		return 1;
	}
	//输出法术伤害
	if (szFlag=="TRUE_DAMAGE_DEALT_PLAYER")
	{
		thePlayer.TRUE_DAMAGE_DEALT_PLAYER=Data;
		return 1;
	}
	//输出真实伤害
	if (szFlag=="LARGEST_CRITICAL_STRIKE")
	{
		thePlayer.LARGEST_CRITICAL_STRIKE=Data;
		return 1;
	}
	//最大暴击伤害

	if (szFlag=="TOTAL_HEAL")
	{
		thePlayer.TOTAL_HEAL=Data;
		return 1;
	}
	//治疗总量
	if (szFlag=="TOTAL_DAMAGE_TAKEN")
	{
		thePlayer.TOTAL_DAMAGE_TAKEN=Data;
		return 1;
	}
	//承受总伤害
	if (szFlag=="PHYSICAL_DAMAGE_TAKEN")
	{
		thePlayer.PHYSICAL_DAMAGE_TAKEN=Data;
		return 1;
	}
	//承受物理伤害
	if (szFlag=="MAGIC_DAMAGE_TAKEN")
	{
		thePlayer.MAGIC_DAMAGE_TAKEN=Data;
		return 1;
	}
	//承受法术伤害
	if (szFlag=="TRUE_DAMAGE_TAKEN")
	{
		thePlayer.TRUE_DAMAGE_TAKEN=Data;
		return 1;
	}
	//承受真实伤害

	if (szFlag=="GOLD_EARNED")
	{
		thePlayer.GOLD_EARNED=Data;
		return 1;
	}
	//金钱
	if (szFlag=="TURRETS_KILLED")
	{
		thePlayer.TURRETS_KILLED=Data;
		return 1;
	}
	//拆塔数
	if (szFlag=="MINIONS_KILLED")
	{
		thePlayer.MINIONS_KILLED=Data;
		return 1;
	}
	//小兵击杀
	if (szFlag=="BARRACKS_KILLED")
	{
		thePlayer.BARRACKS_KILLED=Data;
		return 1;
	}
	//拆兵营数
	if (szFlag=="NEUTRAL_MINIONS_KILLED")
	{
		thePlayer.NEUTRAL_MINIONS_KILLED=Data;
		return 1;
	}
	//野怪击杀
	if (szFlag=="NEUTRAL_MINIONS_KILLED_ENEMY_JUNGLE")
	{
		thePlayer.NEUTRAL_MINIONS_KILLED_ENEMY_JUNGLE=Data;
		return 1;
	}
	//敌方野怪击杀
	if (szFlag=="NEUTRAL_MINIONS_KILLED_YOUR_JUNGLE")
	{
		thePlayer.NEUTRAL_MINIONS_KILLED_YOUR_JUNGLE=Data;
		return 1;
	}
	//己方野怪击杀

	if (szFlag=="TOTAL_TIME_SPENT_DEAD")
	{
		thePlayer.TOTAL_TIME_SPENT_DEAD=Data;
		return 1;
	}
	//总计死亡时间
	if (szFlag=="TOTAL_TIME_CROWD_CONTROL_DEALT")
	{
		thePlayer.TOTAL_TIME_CROWD_CONTROL_DEALT=Data;
		return 1;
	}
	//输出团控时间

	if (szFlag=="ITEM0")
	{
		thePlayer.ITEM0=Data;
		return 1;
	}
	//物品1
	if (szFlag=="ITEM1")
	{
		thePlayer.ITEM1=Data;
		return 1;
	}
	//物品2
	if (szFlag=="ITEM2")
	{
		thePlayer.ITEM2=Data;
		return 1;
	}
	//物品3
	if (szFlag=="ITEM3")
	{
		thePlayer.ITEM3=Data;
		return 1;
	}
	//物品4
	if (szFlag=="ITEM4")
	{
		thePlayer.ITEM4=Data;
		return 1;
	}
	//物品5
	if (szFlag=="ITEM5")
	{
		thePlayer.ITEM5=Data;
		return 1;
	}
	//物品6
	if (szFlag=="ITEM6")
	{
		thePlayer.ITEM6=Data;
		return 1;
	}
	//物品7

	if (szFlag=="SIGHT_WARDS_BOUGHT_IN_GAME")
	{
		thePlayer.SIGHT_WARDS_BOUGHT_IN_GAME=Data;
		return 1;
	}
	//普通眼购买数
	if (szFlag=="VISION_WARDS_BOUGHT_IN_GAME")
	{
		thePlayer.VISION_WARDS_BOUGHT_IN_GAME=Data;
		return 1;
	}
	//真眼购买数
	if (szFlag=="WARD_KILLED")
	{
		thePlayer.WARD_KILLED=Data;
		return 1;
	}
	//拆眼数
	if (szFlag=="WARD_PLACED")
	{
		thePlayer.WARD_PLACED=Data;
		return 1;
	}
	//放置眼数
	if (szFlag=="WIN")
	{
		thePlayer.WIN=Data;
		return 1;
	}
	//Win

	if (szFlag=="LOSE")
	{
		thePlayer.LOSE=Data;
		return 1;
	}
	//Lose

	if (szFlag=="COMBAT_PLAYER_SCORE")
	{
		thePlayer.COMBAT_PLAYER_SCORE=Data;
		return 1;
	}
	
	if (szFlag=="NODE_CAPTURE_ASSIST")
	{
		thePlayer.NODE_CAPTURE_ASSIST=Data;
		return 1;
	}

	if (szFlag=="OBJECTIVE_PLAYER_SCORE")
	{
		thePlayer.OBJECTIVE_PLAYER_SCORE=Data;
		return 1;
	}
	
	if (szFlag=="VICTORY_POINT_TOTAL")
	{
		thePlayer.VICTORY_POINT_TOTAL=Data;
		return 1;
	}

	if (szFlag=="TOTAL_PLAYER_SCORE")
	{
		thePlayer.TOTAL_PLAYER_SCORE=Data;
		return 1;
	}

	if (szFlag=="TOTAL_SCORE_RANK")
	{
		thePlayer.TOTAL_SCORE_RANK=Data;
		return 1;
	}
	

	if (szFlag=="NODE_CAPTURE")
	{
		thePlayer.NODE_CAPTURE=Data;
		return 1;
	}

	if (szFlag=="TEAM_OBJECTIVE")
	{
		thePlayer.TEAM_OBJECTIVE=Data;
		return 1;
	}

	if (szFlag=="NODE_NEUTRALIZE")
	{
		thePlayer.NODE_NEUTRALIZE=Data;
		return 1;
	}

	if (szFlag=="NODE_NEUTRALIZE_ASSIST")
	{
		thePlayer.NODE_NEUTRALIZE_ASSIST=Data;
		return 1;
	}
	
	
	return 0;
}

void CParserGameState::GetGameMode( unsigned  char* pBuffer,unsigned int Len )
{
	unsigned int TempPos=_ParserData.Pos_messagingDestinatio;

	if (TempPos==0)
	return;

	if (_ParserData._bIsAIGame)
	{
		TempPos-=0x60;
	}else{
		TempPos-=0x4F;
	}
		
		if (SkipToChar(pBuffer,Len,&TempPos,0x06)>0x20)
		return;

		//	m_Data.GameMode=CLASSIC;
		//	m_Data.GameType=BOT;//默认

			string szGameMode=ReadStrBy06Pos(pBuffer,&TempPos);

			if (szGameMode=="CLASSIC")
			{
				m_Data.GameMode=CLASSIC;
			}else 			if (szGameMode=="ARAM")
			{
				m_Data.GameMode=ARAM;
			}else 			if (szGameMode=="ODIN")
			{
				m_Data.GameMode=ODIN;
			}else 			if (szGameMode=="ASCENSION")
			{
				m_Data.GameMode=ASCENSION;
			}else if (   szGameMode== "KINGPORO")
			{
				m_Data.GameMode=KINGPORO;
			}


			if (SkipToChar(pBuffer,Len,&TempPos,0x06)>0x10)
				return ;
			string szGameType=ReadStrBy06Pos(pBuffer,&TempPos);

/*
			NORMAL=1,					//普通匹配
				NONE=2,							//非匹配模式(自定义房间)
				ARAM_UNRANKED_5x5=3,//大乱斗5v5
				ODIN_UNRANKED=4,		//统治战场匹配*/

			if (szGameType=="NORMAL")
			{
				m_Data.GameType=NORMAL;

			}else 			if (szGameType=="NONE")
			{
				m_Data.GameType=NONE;
			}else 			if (szGameType=="RANKED_SOLO_5x5")
			{
				m_Data.GameType=RANKED_SOLO_5x5;

			}else 			if (szGameType=="ODIN_UNRANKED")
			{
				m_Data.GameType=ODIN_UNRANKED;

			}else 			if (szGameType=="ARAM_UNRANKED_5x5")
			{
				m_Data.GameType=ARAM_UNRANKED_5x5;

			}else 			if (szGameType=="NORMAL_3x3")
			{
				m_Data.GameType=NORMAL_3x3;
			}else if (szGameType=="BOT")
			{
				m_Data.GameType=BOT;
			}else if (szGameType=="HEXAKILL")
			{
				m_Data.GameType=HEXAKILL;
				
			}else if (szGameType=="ReRoll")
			{
				m_Data.GameType=KING_PORO;			
			}else if (szGameType=="URF")
			{
				m_Data.GameType=URF;		
			}else if (szGameType=="6v6")
			{
				m_Data.GameType=MID6V6;		
			}
			memcpy(m_Data.GameTypeStr, szGameType.c_str(), szGameType.length());
}

std::string w2c(const wchar_t * pw)
{
	std::string val = "";
	if(!pw)
	{
		return val;
	}
	size_t size= wcslen(pw)*sizeof(wchar_t)+1;
	char *pc = NULL;
	if(!(pc = (char*)malloc(size)))
	{
		return val;
	}
	memset(pc,0,size);
	size_t destlen = wcstombs(pc,pw,size);
	/*转换不为空时，返回值为-1。如果为空，返回值0*/
	if (destlen ==(size_t)(0))
	{
		free(pc);
		return val;
	}
	val = pc;
	free(pc);
	return val;
}


/** 
  * 该函数用于将utf8数组转为Unicode格式! 
  * 目前该函数返回值为：转换后unicode数据占用的wchar_t的个数（切记不是总char的个数） ！ 
  *     @param out   转换结果存放区域指针 
  *     @param outsize   转换结果存放区域大小 
  *     @param in   源串存放区域指针 
  *     @param insize   源串存放区域大小 
  *     @return QINT 转换结果在目的串中的长度，转换失败则返回-1 
  */   
int  Utf82Unicode(wchar_t* out,  int outsize , char* in,int insize)  
{  
    //-------------------------------------------------------------------------------------------   
    //参数有效性判断   
    if(out == NULL || in == NULL || insize<0)  
    {  
        return -1;  
    }  
  
    int totalNum = 0;  
    char *p = in;  
    for(int i=0;i<insize;i++)  
    {  
        if (*p >= 0x00 && *p <= 0x7f)//说明最高位为'0'，这意味着utf8编码只有1个字节！   
        {  
            p++;  
            totalNum += 1;  
        }  
        else if ((*p & (0xe0))== 0xc0)//只保留最高三位，看最高三位是不是110，如果是则意味着utf8编码有2个字节！   
        {  
            p++;  
            p++;  
            totalNum += 1;  
        }  
        else if ((*p & (0xf0))== 0xe0)//只保留最高四位，看最高三位是不是1110，如果是则意味着utf8编码有3个字节！   
        {  
            p++;  
            p++;  
            p++;  
            totalNum += 1;  
        }  
    }  
    if( outsize < totalNum )//参数有效性判断！   
    {  
        return -1;  
    }  
    //------------------------------------------------   
        int resultsize = 0;  
  
        p = in;  
        char* tmp = (char *)out;  
        while(*p)  
        {  
            if (*p >= 0x00 && *p <= 0x7f)//说明最高位为'0'，这意味着utf8编码只有1个字节！   
            {  
                *tmp = *p;  
                tmp++;  
                //*tmp = '/0';   
                tmp++;  
                resultsize += 1;  
            }  
            else if ((*p & 0xe0)== 0xc0)//只保留最高三位，看最高三位是不是110，如果是则意味着utf8编码有2个字节！   
            {  
                wchar_t t = 0;  
                char t1 = 0;  
                char t2 = 0;  
  
                t1 = *p & (0x1f);//高位的后5位！（去除了头部的110这个标志位）   
                p++;  
                t2 = *p & (0x3f);//低位的后6位！（去除了头部的10这个标志位）   
  
                *tmp = t2 | ((t1 & (0x03)) << 6);  
                tmp++;  
                *tmp = t1 >> 2;//留下其保留的三位   
                tmp++;  
                resultsize += 1;  
            }  
            else if ((*p & (0xf0))== 0xe0)//只保留最高四位，看最高三位是不是1110，如果是则意味着utf8编码有3个字节！   
            {  
                wchar_t t = 0;  
                wchar_t t1 = 0;  
                wchar_t t2 = 0;  
                wchar_t t3 = 0;  
                t1 = *p & (0x1f);  
                p++;  
                t2 = *p & (0x3f);  
                p++;  
                t3 = *p & (0x3f);  
  
                *tmp = ((t2 & (0x03)) << 6) | t3;  
                tmp++;  
                *tmp = (t1 << 4) | (t2 >> 2);  
                tmp++;  
                resultsize += 1;  
            }  
            p++;  
        }  
        /*不考虑结束符，如果考虑则打开此段！  
        *tmp = '/0';  
        tmp++;  
        *tmp = '/0';  
        resultsize += 2;  
        */  
        return resultsize;  
}  
