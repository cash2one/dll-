#include "LOLHelper.h"
#include <zjlcommon.h>


  extern int bDebug;
  extern bool bAppRun;
  extern LOLHookOffset* g_pOffset;
  bool bExtMode=0;

#define MENU_BK_W 191
#define MENU_BK_H 86



void CLOLHelper::Event( MiniMapEvent event,int IndexFlag,float fCDtime)
{
	time_t timeCurrent=0;
	time(&timeCurrent);
	timeCurrent+=(int) fCDtime;

	DbgPrint("ClolHelper::Event %d  %08x %.02f ",event,IndexFlag,fCDtime);

	stPlayerState* pPlayer=_GetPlayerByFlag(IndexFlag);

	switch(event)
	{
	case MiniMapEvent::CREEPS11:
		DbgPrint("ClolHelper::Event   CREEPS11 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.CREEPS11=timeCurrent;
		break;
	case MiniMapEvent::CREEPS12:
		DbgPrint("ClolHelper::Event   CREEPS12 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.CREEPS12=timeCurrent;
		break;
	case MiniMapEvent::CREEPS21:
		DbgPrint("ClolHelper::Event   CREEPS21 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.CREEPS21=timeCurrent;
		break;
	case MiniMapEvent::CREEPS22:
		DbgPrint("ClolHelper::Event   CREEPS22 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.CREEPS22=timeCurrent;
		break;
	case MiniMapEvent::BOSS1:
		DbgPrint("ClolHelper::Event   BOSS1 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.BOSS1=timeCurrent;
		break;
	case MiniMapEvent::BOSS2:
		DbgPrint("ClolHelper::Event   BOSS2 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.BOSS2=timeCurrent;
		break;


	case MiniMapEvent::TOWER11:
		DbgPrint("ClolHelper::Event   TOWER11 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER11=timeCurrent;
		break;
	case MiniMapEvent::TOWER12:
		DbgPrint("ClolHelper::Event   TOWER12 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER12=timeCurrent;
		break;
	case MiniMapEvent::TOWER13:
		DbgPrint("ClolHelper::Event   TOWER13 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER13=timeCurrent;
		break;
	case MiniMapEvent::TOWER21:
		DbgPrint("ClolHelper::Event   TOWER21 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER21=timeCurrent;
		break;
	case MiniMapEvent::TOWER22:
		DbgPrint("ClolHelper::Event   TOWER22 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER22=timeCurrent;
		break;
	case MiniMapEvent::TOWER23:
		DbgPrint("ClolHelper::Event   TOWER23 player[%08x] time[%.02f]",IndexFlag,fCDtime);
		m_Times.TOWER23=timeCurrent;
		break;
	case MiniMapEvent::HERODEAD:
		DbgPrint("ClolHelper::Event   HERODEAD player[%08x] time[%.02f]",IndexFlag,fCDtime);
		if (pPlayer)
		{
			pPlayer->TimeBack=timeCurrent;
		}
		break;

	case MiniMapEvent::BLOODBUFFER1:
		DbgPrint("ClolHelper::Event   BLOODBUFFER1 player[%08x] time[%.02f]",IndexFlag,fCDtime);
			m_Times.BLOODBUFFER1=timeCurrent;
		break;
	case MiniMapEvent::BLOODBUFFER2:
		DbgPrint("ClolHelper::Event   BLOODBUFFER2 player[%08x] time[%.02f]",IndexFlag,fCDtime);
			m_Times.BLOODBUFFER2=timeCurrent;
		break;
	case MiniMapEvent::BLOODBUFFER3:
		DbgPrint("ClolHelper::Event   BLOODBUFFER3 player[%08x] time[%.02f]",IndexFlag,fCDtime);
			m_Times.BLOODBUFFER3=timeCurrent;
		break;
	case MiniMapEvent::BLOODBUFFER4:
		DbgPrint("ClolHelper::Event   BLOODBUFFER4 player[%08x] time[%.02f]",IndexFlag,fCDtime);
			m_Times.BLOODBUFFER4=timeCurrent;
		break;

	}



}

void CLOLHelper::AddPlayer( int _pUnit ,bool bIsMainPlayer)
{
	if (_GetPlayerByPtr((int)_pUnit))
	return;//已经添加过

	ClolUnit unitAdd(_pUnit);

	if (unitAdd.IsGoodPtr() && unitAdd.IsHero())
	{
	VMProtectBegin
		stPlayerState add;
		add.pPlayer=CreateHero(_pUnit);
		add.bMainPlayer=bIsMainPlayer;
		m_vPlayers.push_back(add);
		//Skill
		m_SkillManager.AddPlayer(add.pPlayer,bIsMainPlayer);
	VMProtectEnd
	}else{
		DbgPrint("CLOLHelper::AddPlayer Error [%x %d]",_pUnit,bIsMainPlayer);
	}
	
}

stPlayerState* CLOLHelper::_GetPlayerByFlag( int IndexFlag )
{
	if (IndexFlag==0)
	{
		return 0;
	}

	for (unsigned int i=0;i<m_vPlayers.size();i++)
	{
		if (m_vPlayers[i].pPlayer->GetIndex()==IndexFlag)
		{
			return &m_vPlayers[i];
		}
	}
		return 0;
}

stPlayerState* CLOLHelper::_GetPlayerByPtr( int pUnit )
{
	for (unsigned int i=0;i<m_vPlayers.size();i++)
	{
		if (m_vPlayers[i].pPlayer->GetPtr() ==pUnit)
		{
			return &m_vPlayers[i];
		}
	}
	return 0;
}



void CLOLHelper::SetPos( POINT ScreenSize,float ScaleGUI,float ScaleMINIMAP,bool bIsRightMap )
{
	DbgPrint("分辨率设置: %dx%d  ScaleGUI:%.02lf  ScaleMINIMAP:%.02lf bRightMap:%d",ScreenSize.x,ScreenSize.y,ScaleGUI,ScaleMINIMAP,bIsRightMap);

	if (ScaleGUI>1)
	{
		ScaleGUI=1;
	}
	m_DrawParam.rectScreen.left=0;
	m_DrawParam.rectScreen.top=0;
	m_DrawParam.rectScreen.right=ScreenSize.x;
	m_DrawParam.rectScreen.bottom=ScreenSize.y;

	//菜单位置
	m_DrawParam.rectMenu.left=0;
	m_DrawParam.rectMenu.top=ScreenSize.y/4;

	RECT rectMiniMap,rectFriendHead,rectEnemyDeadHead;
	//小地图为高的1/4
	int Base=ScreenSize.y;

	float fScaleMap=0.125+ScaleMINIMAP*100*0.00125;//(0.25-0.125)/100; 一个百分比是0.00125,基数0.125
	int MiniMapWidth=fScaleMap*Base;
	int EnemyHeadHeigh=57;
	int EnemyHeadWidth=EnemyHeadHeigh*0.74;

	DbgPrint("MiniMapWidth=%d  EnemyHeadHeigh=%d  EnemyHeadWidth=%d",MiniMapWidth,EnemyHeadHeigh,EnemyHeadWidth);
	//敌方头像
	rectEnemyDeadHead.top=Base*0.25;
	rectEnemyDeadHead.left=ScreenSize.x-42;
	rectEnemyDeadHead.right=rectEnemyDeadHead.left+EnemyHeadWidth;
	rectEnemyDeadHead.bottom=rectEnemyDeadHead.top+EnemyHeadHeigh;


	//队友英雄头像
	//头像Y
	rectFriendHead.top=Base*0.087;
	//头像X
	rectFriendHead.left=Base*0.027;
	//头像高
	rectFriendHead.bottom=0.04*Base;

	//缩放
	rectFriendHead.top+=ScaleGUI*Base*0.097;
	rectFriendHead.left+=ScaleGUI*Base*0.03;
	rectFriendHead.bottom+=rectFriendHead.top+ScaleGUI*Base*0.0435;
	//头像宽
	rectFriendHead.right=rectFriendHead.left+500;

	//小地图
	if (bIsRightMap)
	{
		rectMiniMap.left=ScreenSize.x-MiniMapWidth;
		rectMiniMap.top=ScreenSize.y-MiniMapWidth;
		rectMiniMap.right=ScreenSize.x;
		rectMiniMap.bottom=ScreenSize.y;
	}else{
		rectMiniMap.left=0;
		rectMiniMap.top=ScreenSize.y-MiniMapWidth;
		rectMiniMap.right=MiniMapWidth;
		rectMiniMap.bottom=ScreenSize.y;
	}

	m_DrawParam.rectMiniMap=rectMiniMap;
	m_DrawParam.rectFrientHead=rectFriendHead;
	m_DrawParam.rectEnemyDeadHead=rectEnemyDeadHead;
}

void CLOLHelper::D3DLoadRes( IDirect3DDevice9 * pInitDevice )
{
	DbgPrint("D3DLoadRes Begin");

	m_D3DManager.Init(pInitDevice);
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\HealthBarLeft.png","hpl");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\HealthBarRight.png","hpr");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\SkillBar.png","SKillBar");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\TeamFrame.png","TeamFrameBar");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\GankTip.png","GankTip");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\HpBarTip.png","HpBarTip");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\GameControl.png","Menu");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\MiniMap.png","MiniMap");
	m_D3DManager.LoadResTex(m_Res.szPathImgs+"HUD\\ItemIcons.png","ItemIcons");
//	_LoadResFont(pInitDevice,m_Res.szPathFonts+"FZLTCH.TTF",12,"方正兰亭12");
	m_D3DManager.LoadResFont("Arial",5,10,10,"Arial10");
	m_D3DManager.LoadResFont("Arial",6,12,1,"Arial12");	//技能条倒计时
	m_D3DManager.LoadResFont("Arial",8,16,10,"NumHP");
	m_D3DManager.LoadResFont("Arial",6,12,10,"Arial16");
	m_D3DManager.LoadResFont("Arial",9,18,10,"Arial18");
	m_D3DManager.LoadResFont("Arial",12,24,10,"NumDead");

	m_D3DManager.LoadResFont("黑体",8,16,10,"MenuTitle");
	m_D3DManager.LoadResFont("宋体",6,12,1,"MenuInfo");
	DbgPrint("D3DLoadRes End");
}

void CLOLHelper::D3DFreeRes( )
{
	DbgPrint("D3DFreeRes Begin");
	//m_Res.m_CSRes.Lock();
	m_D3DManager.FreeRes();
	//m_Res.m_CSRes.Unlock();
	DbgPrint("D3DFreeRes End");
}



BOOL __stdcall zD3DDrawText1(LPDIRECT3DDEVICE9 pDxdevice,
	LPTSTR strText ,
	LPTSTR FontName ,
	int FontH,
	int FontW,
	int FWeight,
	int Rleft,
	int Rtop,
	int Color)
{
	int nbuf = lstrlen(strText);

	RECT myrect;
	myrect.left=Rleft; //文本块的x坐标
	myrect.top=Rtop;  //文本块的y坐标
	myrect.right=Rleft+2048;
	myrect.bottom=Rtop+2048;

	D3DXFONT_DESC lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height = FontH; //字体高度
	lf.Width = FontW; // 字体宽度
	lf.Weight = FWeight; 
	lf.Italic = false;
	lf.CharSet = DEFAULT_CHARSET;
	lstrcpy(lf.FaceName,FontName);

	ID3DXFont* font=NULL;
	if(D3D_OK!=D3DXCreateFontIndirect(pDxdevice, &lf, &font)) 
		return false;

	font->DrawText(NULL,strText,nbuf,&myrect,DT_TOP | DT_LEFT|DT_WORDBREAK,Color);


	font->Release();//释放对象
	return TRUE;
}

 int HowLong2Point(POINT pt1,POINT pt2)
{
	unsigned int X1=pt1.x;
	unsigned int Y1=pt1.y;

	unsigned int X2=pt2.x;
	unsigned int Y2=pt2.y;

	if (X1==0 || Y1==0 || X1>20000 || Y1>20000)
	{
		return -1;
	}
	if (X2==0 || Y2==0 || X2>20000 || Y2>20000)
	{
		return -1;
	}
	unsigned int XX=0,YY=0;
	if (X1>X2)
	{
		XX=X1-X2;
	}else{
		XX=X2-X1;
	}

	if (Y1>Y2)
	{
		YY=Y1-Y2;
	}else{
		YY=Y2-Y1;
	}

	long long SQ=YY*YY+XX*XX;
	double db=SQ;
	int ResultF=sqrt(db);
/*
	CStringA sz;
	sz.Format("X1=%d X2=%d Y1=%d Y2=%d XX=%d YY=%d SQ=%d ResultF=%d\r\n",X1,X2,Y1,Y2,XX,YY,SQ,ResultF);
	MessageBoxA(0,sz,0,0);*/

	//DbgPrint("Player1 x=%d Y=%d   Player2 x=%d,y=%d  ,距离%d",X1,Y1,X2,Y2,ResultF);
	return (unsigned int)ResultF;
}

void CLOLHelper::Draw_Hero(void* pUnit,int posX,int posY,bool bSkillCD,bool bHUDHP)
{
	if (posX<0 || posX>m_DrawParam.rectScreen.right || posY<0 || posY>m_DrawParam.rectScreen.bottom)
	{
		return;
	}




	stPlayerState* pP=_GetPlayerByPtr((int)pUnit);
	if (pP==0 )
	return;

	if (pP->bMainPlayer)
	{//主玩家
		posX-=14;
		posY+=20;
		//数字显血背景框
		if (bHUDHP )
		{
			m_D3DManager.Draw("hpl",posX+11,posY-6,1);
		//数字显血数字
			m_D3DManager.Draw_Number(posX+19,posY-5,pP->pPlayer->GetHP(),MathHPFontColor(pP->pPlayer->GetHP(),pP->pPlayer->GetMaxHP()),"NumHP");
		}

		if (0)//屏蔽自己
		{
			if (bSkillCD)
			{
				m_D3DManager.DrawRect("SKillBar",1,1,58,18,posX+63,posY-5,1);
				Draw_Hero_SkillBar(pUnit,posX+63,posY-5);
			}
		}

	}else{//其他玩家
		posX+=118;
		posY+=20;
		//数字显血背景框
		if (bHUDHP)
		{
			m_D3DManager.Draw("hpr",posX-139,posY-6,1);
		//数字显血数字
		m_D3DManager.Draw_Number(posX-53,posY-5,pP->pPlayer->GetHP(),MathHPFontColor(pP->pPlayer->GetHP(),pP->pPlayer->GetMaxHP()),"NumHP");
		}

		ClolUnit* pMainPlayer=GetPlayerByIndex(0);


		if (pMainPlayer && pMainPlayer->GetTeam()== pP->pPlayer->GetTeam())
		{//只显示队友的
			//技能条
			if (  bSkillCD  )
			{
				m_D3DManager.DrawRect("SKillBar",1,1,58,18,posX-121,posY-5,1);
				Draw_Hero_SkillBar(pUnit,posX-121,posY-5);
			}

		}

		if (bDebug || bExtMode)//敌方技能条
		{
			//技能条
			if (bSkillCD )
			{
				m_D3DManager.DrawRect("SKillBar",1,1,58,18,posX-121,posY-5,1);
				Draw_Hero_SkillBar(pUnit,posX-121,posY-5);
			}
		}

	}
}

void CLOLHelper::Draw_UI( bool bEnemyInfo,bool bMonsterTimer,bool bInhibitorTimer,bool bUltraCD,bool bHealPackTimer,bool bFriendSummonerSkillsCD,bool bDrawGankTip)
{

	if (bUltraCD)
	{
			//大招CD
		if (m_vPlayers.size()>1)
		{
			for (unsigned int i=1;i<5;i++)
			{
				int UltCD=m_SkillManager.GetHeroSkillCDByIndex(m_vPlayers[0].pPlayer->GetTeam(),i,3);
			//	UltCD=99;
				if (UltCD>0)
				{
					int Heigh=m_DrawParam.rectFrientHead.bottom-m_DrawParam.rectFrientHead.top;
					int Y=m_DrawParam.rectFrientHead.top+Heigh*(i-1);
					m_D3DManager.Draw_Number(m_DrawParam.rectFrientHead.left+5,Y,UltCD,D3DCOLOR_XRGB(30,255,30),"Arial18");
				}
			}
		}
	}


	//小地图计时,水晶与野怪
	time_t CurrentTime;
	time(&CurrentTime);

	int MapX=m_DrawParam.rectMiniMap.left;
	int MapY=m_DrawParam.rectMiniMap.top;
	int MapWidth=m_DrawParam.rectMiniMap.right-m_DrawParam.rectMiniMap.left;
	int MapHeigh=m_DrawParam.rectMiniMap.bottom-m_DrawParam.rectMiniMap.top;


	if (bMonsterTimer)
	{
		if (m_Times.CREEPS11>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.21,MapY+MapHeigh*0.42,m_Times.CREEPS11-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}
		if (m_Times.CREEPS12>CurrentTime)
		{
			//DbgPrint("Draw_UI MapX=%d,MapY=%d, MapWidth=%d, MapHeigh=%d",MapX,MapY,MapWidth,MapHeigh);
			m_D3DManager.Draw_Number(MapX+MapWidth*0.485,MapY+MapHeigh*0.677,m_Times.CREEPS12-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}
		if (m_Times.CREEPS21>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.426,MapY+MapHeigh*0.248,m_Times.CREEPS21-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}
		if (m_Times.CREEPS22>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.685,MapY+MapHeigh*0.5,m_Times.CREEPS22-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}

		if (m_Times.BOSS1>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.277,MapY+MapHeigh*0.277,m_Times.BOSS1-CurrentTime,D3DCOLOR_XRGB(255,255,80),"Arial18");
		}
		if (m_Times.BOSS2>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.63,MapY+MapHeigh*0.666,m_Times.BOSS2-CurrentTime,D3DCOLOR_XRGB(255,255,80),"Arial18");
		}
	}

	if (bInhibitorTimer)
	{
		if (m_Times.TOWER11>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.03,MapY+MapHeigh*0.74,m_Times.TOWER11-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}
		if (m_Times.TOWER12>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.177,MapY+MapHeigh*0.755,m_Times.TOWER12-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}
		if (m_Times.TOWER13>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.2,MapY+MapHeigh*0.877,m_Times.TOWER13-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}
		if (m_Times.TOWER21>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.715,MapY+MapHeigh*0.07,m_Times.TOWER21-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}
		if (m_Times.TOWER22>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.74,MapY+MapHeigh*0.160,m_Times.TOWER22-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}
		if (m_Times.TOWER23>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.820,MapY+MapHeigh*0.215,m_Times.TOWER23-CurrentTime,D3DCOLOR_XRGB(255,255,255),"Arial18");
		}

	}

	//大乱斗血包计时
	if (bHealPackTimer)
	{
		if (m_Times.BLOODBUFFER1>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.344,MapY+MapHeigh*0.685,m_Times.BLOODBUFFER1-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}

		if (m_Times.BLOODBUFFER2>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.4444,MapY+MapHeigh*0.592,m_Times.BLOODBUFFER2-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}

		if (m_Times.BLOODBUFFER3>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.5666,MapY+MapHeigh*0.459,m_Times.BLOODBUFFER3-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}

		if (m_Times.BLOODBUFFER4>CurrentTime)
		{
			m_D3DManager.Draw_Number(MapX+MapWidth*0.6666,MapY+MapHeigh*0.37,m_Times.BLOODBUFFER4-CurrentTime,D3DCOLOR_XRGB(30,255,30),"Arial18");
		}
	}

	//gank提醒
	if (bDrawGankTip&& m_UpdateData.T1Count+1<m_UpdateData.T2Count)
		//	if (1)
	{
		//m_D3DManager.Draw_String(posX+10,posY-57,"附近敌方人多!",D3DCOLOR_XRGB(255,30,30),"Arial18");
			m_D3DManager.DrawRect("GankTip",0,0,512,41,m_DrawParam.rectScreen.right/2-512/2,m_DrawParam.rectScreen.bottom/4,1);
			//m_D3DManager.DrawScale(m_Res.pSprite,m_DrawParam.rectScreen.right/2-100,m_DrawParam.rectScreen.bottom/4,0.5,0.5,1);
	}
/*
	m_D3DManager.Draw_Number(MapX+MapWidth*0.21,MapY+MapHeigh*0.42,11,D3DCOLOR_XRGB(30,255,30),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.485,MapY+MapHeigh*0.677,12,D3DCOLOR_XRGB(30,255,30),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.426,MapY+MapHeigh*0.248,21,D3DCOLOR_XRGB(30,255,30),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.685,MapY+MapHeigh*0.5,22,D3DCOLOR_XRGB(30,255,30),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.277,MapY+MapHeigh*0.277,999,D3DCOLOR_XRGB(255,255,80),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.63,MapY+MapHeigh*0.666,888,D3DCOLOR_XRGB(255,255,80),"Arial18");

	m_D3DManager.Draw_Number(MapX+MapWidth*0.03,MapY+MapHeigh*0.74,511,D3DCOLOR_XRGB(255,255,255),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.177,MapY+MapHeigh*0.755,512,D3DCOLOR_XRGB(255,255,255),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.2,MapY+MapHeigh*0.877,513,D3DCOLOR_XRGB(255,255,255),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.715,MapY+MapHeigh*0.07,521,D3DCOLOR_XRGB(255,255,255),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.74,MapY+MapHeigh*0.160,522,D3DCOLOR_XRGB(255,255,255),"Arial18");
	m_D3DManager.Draw_Number(MapX+MapWidth*0.820,MapY+MapHeigh*0.215,523,D3DCOLOR_XRGB(255,255,255),"Arial18");


*/


	//敌方右侧头像
	if (bEnemyInfo)
	{
		DrawRightEnemyHead();
	}

	//友军技能
	if (bFriendSummonerSkillsCD)
	{
		DrawLeftFriendHead();
	}
	
	
}

void CLOLHelper::Skill( unsigned int pSkill,float CD)
{
	if(CD>0 && CD<500)
	{
		m_SkillManager.CallBackUseSkillCD(pSkill,CD);
	}	
}

void CLOLHelper::SetResPath( CStringA szGamePath,CStringA szCGAPath)
{
	szCGAPath+="\\";
	szCGAPath.Replace("\\\\","\\");

	m_Res.szPathImgs=szCGAPath;
	m_Res.szPathImgs+="Res\\";

	m_Res.szPathGame=szGamePath;
	m_Res.szPathGame+="\\";
	m_Res.szPathGame.Replace("\\\\","\\");


	DbgPrint("设置目录  游戏[%s]  Res[%s] ",m_Res.szPathGame,m_Res.szPathImgs);

	//隐藏模式
	if (PathFileExistsA(szCGAPath+"Debug.txt") || PathFileExistsA(szCGAPath+"\\Debug.txt"))
	{
		OutputDebugStringA("=,= 调试模式开启");
		bExtMode=1;
	}else{
		OutputDebugStringA("=,= 调试模式关闭");
		bExtMode=0;
	}
}

void CLOLHelper::Draw_Hero_SkillBar(void* pUnit,int posX,int posY )
{
	//QWER图片
	int CD=0;
	int Level=0;


		//Q
		CD=m_SkillManager.GetHeroSkillCD(pUnit,0);
		Level=m_SkillManager.GetHeroSkillLevel(pUnit,0);
		if (Level>0)
		{
			if (CD==0 )
			{
				m_D3DManager.DrawRect("SKillBar",3,52,6,10,posX+4,posY+2,1);

			}else{//画CD
				if (CD>99)
					CD=99;
				if (CD>=10)
				{
					m_D3DManager.Draw_Number(posX+2,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
				}else if (CD){
					m_D3DManager.Draw_Number(posX+6,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
				}
				//	m_D3DManager.DrawRect(m_Res.pSprite,2,40,6,10,posX+4,posY+2,1);
			}
			for (unsigned int i=0;i<Level;i++)
			{
				m_D3DManager.DrawRect("SKillBar",1,27,1,2,posX+3+i*2,posY+15,1);
			}
		}

		//W
		CD=m_SkillManager.GetHeroSkillCD(pUnit,1);
		Level=m_SkillManager.GetHeroSkillLevel(pUnit,1);
		if (Level>0)
		{
			if (CD==0  )
			{
				m_D3DManager.DrawRect("SKillBar",15,52,10,9,posX+17,posY+3,1);
			}else{
				if (CD>99)
					CD=99;
				if (CD>=10)
				{
					m_D3DManager.Draw_Number(posX+15,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
				}else if (CD){
					m_D3DManager.Draw_Number(posX+19,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
				}

			}
			for (unsigned int i=0;i<Level;i++)
			{
				m_D3DManager.DrawRect("SKillBar",1,27,1,2,posX+17+i*2,posY+15,1);
			}
		}
		//E
		CD=m_SkillManager.GetHeroSkillCD(pUnit,2);
		Level=m_SkillManager.GetHeroSkillLevel(pUnit,2);
		if (Level>0)
		{
				if (CD==0  )
				{
					m_D3DManager.DrawRect("SKillBar",29,52,9,10,posX+31,posY+2,1);		
				}else{
					if (CD>99)
						CD=99;
					if (CD>=10)
					{
						m_D3DManager.Draw_Number(posX+29,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
					}else if (CD){
						m_D3DManager.Draw_Number(posX+33,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
					}
					//m_D3DManager.DrawRect(m_Res.pSprite,29,40,9,10,posX+31,posY+2,1);
				}
				for (unsigned int i=0;i<Level;i++)
				{
					m_D3DManager.DrawRect("SKillBar",1,27,1,2,posX+31+i*2,posY+15,1);
				}
		}
		//R
		CD=m_SkillManager.GetHeroSkillCD(pUnit,3);
		Level=m_SkillManager.GetHeroSkillLevel(pUnit,3);
		if (Level>0)
		{
				if (CD==0  )
				{
					m_D3DManager.DrawRect("SKillBar",44,52,7,10,posX+46,posY+2,1);			
				}else{
					if (CD>99)
						CD=99;
					if (CD>=10)
					{
						m_D3DManager.Draw_Number(posX+44,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
					}else if (CD){
						m_D3DManager.Draw_Number(posX+48,posY+2,CD,D3DCOLOR_XRGB(255,100,10),"Arial12");
					}
					//m_D3DManager.DrawRect(m_Res.pSprite,44,40,7,10,posX+46,posY+2,1);
				}
				for (unsigned int i=0;i<Level;i++)
				{
					m_D3DManager.DrawRect("SKillBar",3,27,2,2,posX+46+i*3,posY+15,1);
				}
		}

	

}

ClolHero* CLOLHelper::GetPlayerByFlag( unsigned int IndexFlag )
{
	if (IndexFlag==0)
	return 0;

	for (unsigned int i=0;i<m_vPlayers.size();i++)
	{
		if (m_vPlayers[i].pPlayer->GetIndex()==IndexFlag)
		{
			return m_vPlayers[i].pPlayer;
		}
	}
}

void CLOLHelper::DrawRightEnemyHead()
{
	time_t CurrentTime;
	time(&CurrentTime);
	//边框
		//画图
		unsigned int IndexDraw=0;
		for (unsigned int i=0;i<m_vPlayers.size();i++)
		{
			if (m_vPlayers[0].pPlayer->GetTeam() ==m_vPlayers[i].pPlayer->GetTeam() )
			{
				continue;
			}
			int EnemyHeadWidth=m_DrawParam.rectEnemyDeadHead.right-m_DrawParam.rectEnemyDeadHead.left;
			int EnemyHeadHeigh=m_DrawParam.rectEnemyDeadHead.bottom-m_DrawParam.rectEnemyDeadHead.top;
			int X=m_DrawParam.rectEnemyDeadHead.left;//背景框坐标
			int Y=m_DrawParam.rectEnemyDeadHead.top+IndexDraw*EnemyHeadHeigh ;
			float fScale=0.29;//头像缩放
			int HPwidth=(float)(((float)m_vPlayers[i].pPlayer->GetHP()/(float)m_vPlayers[i].pPlayer->GetMaxHP()))*38;//血条
			int MANAwidth=(float)(((float)m_vPlayers[i].pPlayer->GetMP()/(float)m_vPlayers[i].pPlayer->GetMaxMP()))*38;//MP

			//英雄头像
			CStringA szHeadImagePath;
			szHeadImagePath.Format("%sAir\\assets\\images\\champions\\%s_Square_0.png",m_Res.szPathGame,m_vPlayers[i].pPlayer->GetHeroName());
			m_D3DManager.LoadResTex(szHeadImagePath,m_vPlayers[i].pPlayer->GetHeroName());

				m_D3DManager.DrawScale(m_vPlayers[i].pPlayer->GetHeroName(),X+5,Y+5,fScale,fScale,1);

				if (bDebug || bExtMode)
				{
					//敌方召唤师技能CD
					CStringA szPathS1,szPathS2;

					CStringA szSkillName1=m_SkillManager.GetHeroSkillName((void*)m_vPlayers[i].pPlayer->GetPtr(),4);
					int SKCD1=m_SkillManager.GetHeroSkillCD((void*)m_vPlayers[i].pPlayer->GetPtr(),4);
					szPathS1.Format("%sSummonerSkills\\%s.png",m_Res.szPathImgs,szSkillName1);

					CStringA szSkillName2=m_SkillManager.GetHeroSkillName((void*)m_vPlayers[i].pPlayer->GetPtr(),5);
					int SKCD2=m_SkillManager.GetHeroSkillCD((void*)m_vPlayers[i].pPlayer->GetPtr(),5);
					szPathS2.Format("%sSummonerSkills\\%s.png",m_Res.szPathImgs,szSkillName2);




					m_D3DManager.LoadResTex(szPathS1,szSkillName1);
					m_D3DManager.LoadResTex(szPathS2,szSkillName2);


					int SkillHeigh=EnemyHeadHeigh/2;
					float SkillScale=float(SkillHeigh)/64.0;


						POINT PosDraw;
						PosDraw.x=m_DrawParam.rectEnemyDeadHead.left-SkillHeigh;
						PosDraw.y=m_DrawParam.rectEnemyDeadHead.top+SkillHeigh*2*(IndexDraw);
						//DbgPrint("Index:%d S1 %d %d  %s ",IndexDraw,PosS1Draw.x,PosS1Draw.y,szSkillName1);
						m_D3DManager.DrawScale(szSkillName1,PosDraw.x,PosDraw.y,SkillScale,SkillScale,1);
						if (SKCD1)
						{

								m_D3DManager.DrawRect("TeamFrameBar",46,2,SkillHeigh,SkillHeigh,PosDraw.x,PosDraw.y,1);

							if (SKCD1>=100)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-22,PosDraw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}else	if (SKCD1>=10)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-15,PosDraw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}else if (SKCD1)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-8,PosDraw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}
						}
					


						PosDraw.x=m_DrawParam.rectEnemyDeadHead.left-SkillHeigh;
						PosDraw.y=m_DrawParam.rectEnemyDeadHead.top+SkillHeigh*2*(IndexDraw)+SkillHeigh;
						//DbgPrint("Index:%d S1 %d %d  %s ",IndexDraw,PosS1Draw.x,PosS1Draw.y,szSkillName1);
						m_D3DManager.DrawScale(szSkillName2,PosDraw.x,PosDraw.y,SkillScale,SkillScale,1);
						if (SKCD2)
						{
							m_D3DManager.DrawRect("TeamFrameBar",46,2,SkillHeigh,SkillHeigh,PosDraw.x,PosDraw.y,1);
							SKCD1=9;
							if (SKCD2>=100)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-22,PosDraw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}else	if (SKCD2>=10)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-15,PosDraw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}else if (SKCD2)
							{
								m_D3DManager.Draw_Number(PosDraw.x+SkillHeigh-8,PosDraw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
							}
						}

				}
			
			//死亡计时
			if (m_vPlayers[i].TimeBack-CurrentTime>=10 )
			{
				m_D3DManager.DrawRect("TeamFrameBar",46,2,36,36,X+5,Y+5,1);//灰色
				m_D3DManager.Draw_Number(X+6,Y+8,m_vPlayers[i].TimeBack-CurrentTime,D3DCOLOR_XRGB(255,255,255),"NumDead");
			}else 	if (m_vPlayers[i].TimeBack-CurrentTime>0)
			{
				m_D3DManager.DrawRect("TeamFrameBar",46,2,36,36,X+5,Y+5,1);//灰色
				m_D3DManager.Draw_Number(X+15,Y+8,m_vPlayers[i].TimeBack-CurrentTime,D3DCOLOR_XRGB(255,255,255),"NumDead");
			}

			//边框
			m_D3DManager.DrawRect("TeamFrameBar",1,1,42,57,X,Y,1);
			//血条
			m_D3DManager.DrawRect("TeamFrameBar",3,60,HPwidth,7,X+2,Y+42,1);
			//mana
			m_D3DManager.DrawRect("TeamFrameBar",3,68,MANAwidth,5,X+2,Y+50,1);
			//等级
			m_D3DManager.Draw_Number(X+3,Y+31,m_vPlayers[i].pPlayer->GetLevel(),D3DCOLOR_XRGB(255,255,120),"Arial10");
			IndexDraw++;
		}


	

}

char* g_pch=0;
void CLOLHelper::SayChar( char* pSay,unsigned int OffsetSayChar )
{
	DbgPrint("喊话.%s",pSay);

	if (pSay)
	{
/*
		char TargetDataSay[]={0x83,0xC4,0x08,0xC1,0xE8,0x07,0x24,0x01,0x75};
		int hookAddr=MyFindDllTarget("League of Legends.exe","",TargetDataSay,sizeof(TargetDataSay),0x1000000);*/
		unsigned int HandleLOL=(unsigned int)GetModuleHandleA("League of Legends.exe");
	//	DbgPrint("hookAddr %08x",hookAddr);
		if (HandleLOL && OffsetSayChar)
		{
			OffsetSayChar+=2;
			unsigned int CallAddr=HandleLOL+OffsetSayChar;

			if (IsBadReadPtr((void*)CallAddr,sizeof(int))==0)
			{
				CallAddr=*(int*) CallAddr;			//3A687B8
			}
			if (IsBadReadPtr((void*)CallAddr,sizeof(int))==0)
			{
				CallAddr=*(int*) CallAddr;			//Ecx=[3A687B8]
			}
				
			int eaxadd=0;
				//League of Legends.exe+57FB35 - FF 90 80000000        - call dword ptr [eax+00000080]  获取0x80
			if (IsBadReadPtr((void*)(HandleLOL+OffsetSayChar+0x0B),sizeof(int))==0)
			{
				 eaxadd=*(int*)(HandleLOL+OffsetSayChar+0x0B);
			}
			if (bDebug>=2)
				DbgPrint("CallAddr=0x%08x, eaxadd=%08x",CallAddr,eaxadd);
		
			if (g_pch==0)
			 g_pch=new char[1024];

			if (g_pch && eaxadd &&CallAddr)
			{
				memset(g_pch,0,1024);
				CStringA szAnsi;
				szAnsi.Format("%s [cga助手]",pSay);
				CStringA szUtf8=AnsiToUTF8(szAnsi);


				StrCpyA(g_pch,szUtf8.GetBuffer());
				//strcpy(pch,"TXCNM\r\nCNM!");

				DbgPrint("C%s",pSay);

				_asm
				{
					push 0;
					mov edi,g_pch;
					push edi;
					mov ecx,CallAddr;
					mov   eax, dword ptr [ecx];
					add eax,eaxadd;
					call [eax];
				}
			}else{
				OutputDebugStringA("*GameLib Error TargetAction5");
			}
		}else{
			DbgPrint("喊话失败.%s",pSay);
		}
	}






		return;
}





CLOLHelper::CLOLHelper( void )
{
	DbgPrint("CLOLHelper::CLOLHelper");
	m_Res.m_bResLoad=0;
	m_UpdateData.T1Count=0;
	m_UpdateData.T2Count=0;
	//m_Res.m_CSRes.Init();
	memset(&m_Times,0,sizeof(m_Times));
	memset(&m_UpdateData,0,sizeof(m_UpdateData));
	memset(&m_DrawParam,0,sizeof(m_DrawParam));




}



void RunGankTip(LPVOID lp)
{
	struct stPlayerPOINT{
		ClolUnit* pPlayer;
		POINT LastPOINT;
		time_t LastTime;
	};
	vector<stPlayerPOINT> vEnemyPoints;

	CLOLHelper* p=(CLOLHelper*)lp;
	while(bAppRun)
	{
		time_t CurrentTime=0;
		time(&CurrentTime);


		//gank提醒
		int T1=0,T2=0;
		ClolUnit* pMainPlayer=p->GetPlayerByIndex(0);
		//DbgPrint("ClolUnit* %08x",  pMainPlayer);
		if (pMainPlayer && pMainPlayer->GetHP())
		{
			unsigned int IndexPlayer=1;
			while(1)
			{
				ClolUnit* pOtherPlayer=p->GetPlayerByIndex(IndexPlayer++);
				//DbgPrint("pOtherPlayer* %08x",  pOtherPlayer);
				if (pOtherPlayer && pOtherPlayer->GetHP())
				{
					POINT ptMain;
					ptMain.x=pMainPlayer->GetX();
					ptMain.y=pMainPlayer->GetY();

					POINT ptO;
					ptO.x=pOtherPlayer->GetX();
					ptO.y=pOtherPlayer->GetY();


					if (HowLong2Point(ptMain,ptO)<2500)
					{
						if (pOtherPlayer->GetTeam()==pMainPlayer->GetTeam())
						{//队友
							T1++;
						}else{//敌人
							bool bFind=0;
							for (unsigned int i=0;i<vEnemyPoints.size();i++)
							{
								if (vEnemyPoints[i].pPlayer && vEnemyPoints[i].pPlayer==pOtherPlayer)
								{
									//检查参数
									if (vEnemyPoints[i].LastPOINT.x!=ptO.x  || vEnemyPoints[i].LastPOINT.y!=ptO.y	)
									{//坐标不一样
										//赋值.
										vEnemyPoints[i].LastPOINT=ptO;
										vEnemyPoints[i].LastTime=CurrentTime;
										T2++;
									}else{//坐标和上次相同
										if (CurrentTime-vEnemyPoints[i].LastTime>=4)
										{
											//			DbgPrint("玩家%s离开视野已4秒",pOtherPlayer->GetName());
										}else{
											//			DbgPrint("玩家%s和上次坐标未变动且刚离开视野",pOtherPlayer->GetName());
											T2++;
										}

									}
									bFind=1;
									break;
								}
							}

							if (bFind==0)
							{//第一次添加
								stPlayerPOINT add;
								add.LastPOINT=ptO;
								add.LastTime=CurrentTime;
								add.pPlayer=pOtherPlayer;
								DbgPrint("玩家%s已添加",pOtherPlayer->GetName());
								vEnemyPoints.push_back(add);
							}
						}
					}
				}else{
					break;
				}
			}
		}
		//DbgPrint("2500范围内有%d个队友,%d个敌人",T1,T2);
		p->m_UpdateData.T1Count=T1;
		p->m_UpdateData.T2Count=T2;

		Sleep(1000);
	}

}


DWORD WINAPI CLOLHelper::ThreadGankTip(LPVOID lp)
{
	//DbgPrint("Thread1Second Work");

	__try
	{
		RunGankTip(lp);
	}
	__except(1)
	{
		DbgPrint("__except Error : Thread1Second");
	}

	DbgPrint("Thread1Second Work End");
	return 0;
}




void RunCheckWardAndUnits(LPVOID lp)
{
	CLOLHelper* p=(CLOLHelper*)lp;
	while(bAppRun)
	{
		time_t CurrentTime=0;
		time(&CurrentTime);
		//Ward
		for (list<stWardInfo>::iterator pos=p->m_liWards.begin();pos!=p->m_liWards.end();++pos)
		{
			bool bDelete=0;
			ClolUnit unit(pos->pUnit);
			if ( pos->CD &&   CurrentTime>pos->TimeEnd)
			{//眼时间结束
				DbgPrint("眼时间到了 %s",unit.PrintState());
				bDelete=1;
			}	
			if (unit.GetHP()==0 && unit.GetHP()>=5 )
			{
				DbgPrint("眼被打掉 %s",unit.PrintState());
				bDelete=1;
			}
			if (bDelete)
			{
				pos=p->m_liWards.erase(pos);
				if (pos==p->m_liWards.end())
				{
					break;
				}
			}
		}
		Sleep(500);
	}
}

DWORD WINAPI CLOLHelper::ThreadCheckWard(LPVOID lp)
{
	//DbgPrint("ThreadCheckWard Work");

	__try
	{
		RunCheckWardAndUnits(lp);
	}
	__except(1)
	{
		DbgPrint("__except Error : ThreadCheckWard");
	}

	DbgPrint("ThreadCheckWard Work End");
	return 0;
}





void CLOLHelper::BeginMyThread()
{

	CreateThread(0,0,ThreadGankTip,this,0,0);
	CreateThread(0,0,ThreadCheckWard,this,0,0);

}

ClolHero* CLOLHelper::GetPlayerByIndex( unsigned int Index )
{
	if (Index<m_vPlayers.size())
	{
		return m_vPlayers[Index].pPlayer;
	}else{
		return 0;
	}
}

void CLOLHelper::DrawLeftFriendHead()
{
	//DbgPrint("DrawLeftFriendHead Begin");
	unsigned int IndexDraw=0;
	for (unsigned int i=1;i<m_vPlayers.size();i++)
	{
		if (m_vPlayers[0].pPlayer->GetTeam() !=m_vPlayers[i].pPlayer->GetTeam() )
		{
			continue;//跳过对手
		}
	//	DbgPrint("DrawLeftFriendHead %d",IndexDraw);
		CStringA szPathS1,szPathS2;
		

		CStringA szSkillName1=m_SkillManager.GetHeroSkillName((void*)m_vPlayers[i].pPlayer->GetPtr(),4);
		int SKCD1=m_SkillManager.GetHeroSkillCD((void*)m_vPlayers[i].pPlayer->GetPtr(),4);
		szPathS1.Format("%sSummonerSkills\\%s.png",m_Res.szPathImgs,szSkillName1);
		CStringA szSkillName2=m_SkillManager.GetHeroSkillName((void*)m_vPlayers[i].pPlayer->GetPtr(),5);
		int SKCD2=m_SkillManager.GetHeroSkillCD((void*)m_vPlayers[i].pPlayer->GetPtr(),5);
		szPathS2.Format("%sSummonerSkills\\%s.png",m_Res.szPathImgs,szSkillName2);

/*
		DbgPrint("DrawLeftFriendHead S1%s",szPathS1);
		DbgPrint("DrawLeftFriendHead S2%s",szPathS2);*/


		m_D3DManager.LoadResTex(szPathS1,szSkillName1);
		m_D3DManager.LoadResTex(szPathS2,szSkillName2);

		//画头像
		int GameHeadHeigh=m_DrawParam.rectFrientHead.bottom-m_DrawParam.rectFrientHead.top;//头像高
		int SkillHeigh=GameHeadHeigh*0.37;//技能图片高和宽

		POINT PosS1Draw;
		PosS1Draw.x=m_DrawParam.rectFrientHead.left;
		PosS1Draw.y=m_DrawParam.rectFrientHead.top+GameHeadHeigh*(IndexDraw)+GameHeadHeigh*0.25;
		POINT PosS2Draw;
		PosS2Draw.x=m_DrawParam.rectFrientHead.left;
		PosS2Draw.y=PosS1Draw.y+SkillHeigh;

		float SkillScale=float(SkillHeigh)/64.0;

/*
		SKCD1=999;
		SKCD2=99;*/


			//DbgPrint("Index:%d S1 %d %d  %s ",IndexDraw,PosS1Draw.x,PosS1Draw.y,szSkillName1);
			m_D3DManager.DrawScale(szSkillName1,PosS1Draw.x,PosS1Draw.y,SkillScale,SkillScale,1);
			if (SKCD1)
			{
					m_D3DManager.DrawRect("TeamFrameBar",46,2,SkillHeigh,SkillHeigh,PosS1Draw.x,PosS1Draw.y,1);

				if (SKCD1>=100)
				{
					m_D3DManager.Draw_Number(PosS1Draw.x+SkillHeigh-22,PosS1Draw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}else	if (SKCD1>=10)
				{
					m_D3DManager.Draw_Number(PosS1Draw.x+SkillHeigh-15,PosS1Draw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}else if (SKCD1)
				{
					m_D3DManager.Draw_Number(PosS1Draw.x+SkillHeigh-8,PosS1Draw.y+SkillHeigh-11,SKCD1,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}
			}

		

			//DbgPrint("Index:%d S2 %d  %d  %s ",IndexDraw,PosS2Draw.x,PosS2Draw.y,szSkillName2);
			m_D3DManager.DrawScale(szSkillName2,PosS2Draw.x,PosS2Draw.y,SkillScale,SkillScale,1);
			if (SKCD2)
			{
					m_D3DManager.DrawRect("TeamFrameBar",46,2,SkillHeigh,SkillHeigh,PosS2Draw.x,PosS2Draw.y,1);


				if (SKCD2>=100)
				{
					m_D3DManager.Draw_Number(PosS2Draw.x+SkillHeigh-22,PosS2Draw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}else	if (SKCD2>=10)
				{
					m_D3DManager.Draw_Number(PosS2Draw.x+SkillHeigh-15,PosS2Draw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}else if (SKCD2)
				{
					m_D3DManager.Draw_Number(PosS2Draw.x+SkillHeigh-8,PosS2Draw.y+SkillHeigh-11,SKCD2,D3DCOLOR_XRGB(240,200,80),"Arial16");
				}

			}
			
		

		IndexDraw++;
	}

	//DbgPrint("DrawLeftFriendHead End");
}

void CLOLHelper::Draw_HpHitTip( unsigned int pUnit,unsigned int x,unsigned int y )
{

	if (x<=0 || x>=m_DrawParam.rectScreen.right || y<=0 || y>=m_DrawParam.rectScreen.bottom)
	{
		return;
	}



	ClolUnit Unit(pUnit);
	CStringA szName=Unit.GetName();
	//DbgPrint("Draw_HpHitTip1 %s , x=%d,y=%d",Unit.PrintState(),x,y);
	if (szName!="" && szName.Find("Minion_")!=-1)
	{//小兵
		ClolUnit* pMainPlayer=	GetPlayerByIndex(0);
		//DbgPrint("Draw_HpHitTip2 %s , x=%d,y=%d",Unit.PrintState(),x,y);
		if (pMainPlayer &&  Unit.GetTeam()!=pMainPlayer->GetTeam()	)
		{
			int AttackDamage=pMainPlayer->GetAttackDamage();

			int CurrentHp=Unit.GetHP();
			int MaxHp=Unit.GetMaxHP();
			if (MaxHp && CurrentHp && Unit.GetHP()<=AttackDamage)
			{
			//DbgPrint("Draw_HpHitTip3 %s , x=%d,y=%d",Unit.PrintState(),x,y);
				float ff=(float)CurrentHp/(float)MaxHp;
				int WidthDraw=62*ff;
				WidthDraw++;


				//	DbgPrint(" Draw_HpHitTip2  x=%d y=%d hp=%d WidthDraw=%d",x,y,CurrentHp,WidthDraw);
					m_D3DManager.DrawRect("HpBarTip",0,0,WidthDraw,4,x,y,1);
				
			}
		}
	}
	
}

void CLOLHelper::Draw_Menu( GLOBALCONFIG* pOption )
{

	//背景
		m_D3DManager.DrawRect("Menu",0,0,402,429,m_DrawParam.rectMenu.left,m_DrawParam.rectMenu.top,1);
	


	for (unsigned int i=0;i<4;i++)
	{
		switch(i)
		{
		case 0:
			{
				Draw_MenuTip(m_DrawParam.rectMenu.left+8,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"水晶计时","记录水晶重生时间",pOption->bInhibitorTimer);
				Draw_MenuTip(m_DrawParam.rectMenu.left+203,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"打野计时","记录野怪复活时间",pOption->bMonsterTimer);
				break;
			}
		case 1:
			{
				Draw_MenuTip(m_DrawParam.rectMenu.left+8,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"大招计时","队友大招技能CD",pOption->bUltraCD);
				Draw_MenuTip(m_DrawParam.rectMenu.left+203,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"QWER计时","队友QWER技能CD提示",pOption->bSkillCD);
				break;
			}
		case 2:
			{
				Draw_MenuTip(m_DrawParam.rectMenu.left+8,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"数字显血","血条下方数字标识血量",pOption->bHUDHP);
				Draw_MenuTip(m_DrawParam.rectMenu.left+203,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"补刀提示","可以补刀的时候小兵血条变色",pOption->bLastHitTip);
				break;
			}
		case 3:
			{
				Draw_MenuTip(m_DrawParam.rectMenu.left+8,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"召唤师技能","队友的召唤师技能cd计时",pOption->bFriendSummonerSkillsCD);
				Draw_MenuTip(m_DrawParam.rectMenu.left+203,m_DrawParam.rectMenu.top+40+(MENU_BK_H+5)*i,"Gank提醒","视野周围敌方人数多时提示Gank",pOption->bGankTip);
				break;
			}
		}

	}
	

}

CLOLHelper::~CLOLHelper( void )
{
	DbgPrint("~ClolHelper");
//	D3DFreeRes();
}

void CLOLHelper::Draw_MenuTip( int x, int y,CStringA szTitle,CStringA szInfo,bool bOpen )
{

		m_D3DManager.DrawRect("Menu",402,0,191,86,x,y,1);//背景

		if (bOpen)
		{
			m_D3DManager.DrawRect("Menu",402,86,57,16,x+120,y+20,1);//按钮
		}else{
			m_D3DManager.DrawRect("Menu",402,120,57,16,x+120,y+20,1);//按钮
		}
	


	m_D3DManager.Draw_String(x+5,y+15,szTitle,D3DCOLOR_XRGB(200,200,200),"MenuTitle",MENU_BK_W-5*2);
	m_D3DManager.Draw_String(x+5,y+55,szInfo,D3DCOLOR_XRGB(178,178,178),"MenuInfo",MENU_BK_W-5*2);
}

bool CLOLHelper::Menu_Msg( UINT nMsg, WPARAM wParam, LPARAM lParam,GLOBALCONFIG* pOption )
{

	POINT pt;
	pt.x= LOWORD(lParam); 
	pt.y= HIWORD(lParam);

	switch(nMsg)
	{
	case WM_LBUTTONDOWN:
		{
			POINT ptMenu;
			ptMenu.x=pt.x-m_DrawParam.rectMenu.left;
			ptMenu.y=pt.y-m_DrawParam.rectMenu.top;
/*
			Draw_MenuTip(11,40+(MENU_BK_H+5)*i,"水晶计时","记录水晶重生时间",pOption->bInhibitorTimer);
			Draw_MenuTip(203,40+(MENU_BK_H+5)*i,"打野计时","记录野怪复活时间",pOption->bMonsterTimer);*/

			if (ptMenu.x>0 && ptMenu.x<400 && ptMenu.y>0 && ptMenu.y<430 && pOption )
			{

				int MenuIndex_x=0;
				if (ptMenu.x>8 && ptMenu.x<(8+MENU_BK_W))
				{
					MenuIndex_x=1;
				}else if (ptMenu.x>203 && ptMenu.x<(203+MENU_BK_W))
				{
					MenuIndex_x=2;
				}

				int MenuIndex_y=(ptMenu.y-40)/(MENU_BK_H+5);
				MenuIndex_y++;

				DbgPrint("MenuPoint x=%d y=%d  Index %d %d ",ptMenu.x,ptMenu.y,MenuIndex_x,MenuIndex_y);

				switch(MenuIndex_x)
				{
				case 1:
					{
						switch(MenuIndex_y)
						{
						case 1:
							pOption->bInhibitorTimer?pOption->bInhibitorTimer=0:pOption->bInhibitorTimer=1;
							break;
						case 2:
							pOption->bUltraCD?pOption->bUltraCD=0:pOption->bUltraCD=1;
							break;
						case 3:
							pOption->bHUDHP?pOption->bHUDHP=0:pOption->bHUDHP=1;
							break;
						case 4:
							pOption->bFriendSummonerSkillsCD?pOption->bFriendSummonerSkillsCD=0:pOption->bFriendSummonerSkillsCD=1;
							break;
						}
					}
					break;
				case 2:
					{
						switch(MenuIndex_y)
						{
						case 1:
							pOption->bMonsterTimer?pOption->bMonsterTimer=0:pOption->bMonsterTimer=1;
							break;
						case 2:
							pOption->bSkillCD?pOption->bSkillCD=0:pOption->bSkillCD=1;
							break;
						case 3:
							pOption->bLastHitTip?pOption->bLastHitTip=0:pOption->bLastHitTip=1;
							break;
						case 4:
							pOption->bGankTip?pOption->bGankTip=0:pOption->bGankTip=1;
							break;
						}
					}
				}
				return 1;
			}else{
				return 0;
			}

			break;
		}
	case WM_MOUSEMOVE:
		{		
			m_DrawParam.ptMouse=pt;
			break;
		}
	}


	return 1;
}

void CLOLHelper::AddWard( unsigned int pUnitWard )
{
	if (pUnitWard==0)
	{
		return;
	}

	for (list<stWardInfo>::iterator pos=m_liWards.begin();pos!=m_liWards.end();++pos)
	{
		if (pos->pUnit==pUnitWard)
		{
			return;
		}
	}

	ClolUnit unit(pUnitWard);
	if (unit.IsGoodPtr())
	{
		stWardInfo addWard;
		addWard.CD=unit.GetMP();
		addWard.pt.x=unit.GetX();
		addWard.pt.y=unit.GetY();
		addWard.pUnit=pUnitWard;
		addWard.szName=unit.GetName();
		addWard.Team=unit.GetTeam();
		time_t CurrentTime=0;
		time(&CurrentTime);
		addWard.TimeEnd=CurrentTime+addWard.CD;
		
			if ( (addWard.szName.Find("SightWard")!=-1 ||   addWard.szName.Find("VisionWard")!=-1 ))
		{
			DbgPrint("AddWard %s",unit.PrintState());
			m_liWards.push_back(addWard);
		}	
	}
}




void CLOLHelper::Draw_MiniMap()
{
		m_D3DManager.DrawRect("MiniMap",0,0,512,512,m_DrawParam.rectScreen.right-512,m_DrawParam.rectScreen.bottom-512,1);

		for (list<stWardInfo>::iterator pos=m_liWards.begin();pos!=m_liWards.end();++pos)
		{
			int DrawWard_x=(m_DrawParam.rectScreen.right-512)+ 512 * (double)((double)pos->pt.x/(double)15000);
			int DrawWard_y=m_DrawParam.rectScreen.bottom- 512 * (double)((double)pos->pt.y/(double)15000);

			
			time_t timeCurrent=0;
			time(&timeCurrent);
			int LastSecond=pos->TimeEnd-timeCurrent;

			ClolUnit * pMain=GetMainPlayer();
			int RGB=0;
			if (pMain && pMain->GetTeam()== pos->Team)
			{
				RGB=D3DCOLOR_XRGB(50,255,50);
			}else{
				RGB=D3DCOLOR_XRGB(255,50,50);
			}
	/*
	[107636] =,= 插眼!  X=2878, Y=983, Name=SightWard ,CD=60   *60s,到9级时120s //饰品眼
	[107636] =,= 插眼!  X=2121, Y=1096, Name=SightWard ,CD=180  *180s   绿眼

	[107636] =,= 插眼!  X=1650, Y=721, Name=VisionWard ,CD=0  *0s		//红眼
	[14064] =,=   X=4518, Y=4348, Name=VisionWard ,CD=180		//眼石
*/
			if (LastSecond >=0)
			{			
				if (pos->szName=="SightWard" &&  (pos->CD==60 || pos->CD==120 ) )
				{//饰品眼
					m_D3DManager.DrawRectScale("ItemIcons",0,0,64,64,0.5,0.5,DrawWard_x,DrawWard_y,1);
					m_D3DManager.Draw_Number(DrawWard_x+5,DrawWard_y+5,LastSecond,RGB,"Arial18");	
				}

				if (pos->szName=="SightWard" &&  pos->CD==180)
				{//绿眼
					m_D3DManager.DrawRectScale("ItemIcons",64,0,64,64,0.5,0.5,DrawWard_x,DrawWard_y,1);
					m_D3DManager.Draw_Number(DrawWard_x+5,DrawWard_y+5,LastSecond,RGB,"Arial18");			
				}


				if (pos->szName=="VisionWard" &&  pos->CD==180)
				{//眼石
					m_D3DManager.DrawRectScale("ItemIcons",192,0,64,64,0.5,0.5,DrawWard_x,DrawWard_y,1);
					m_D3DManager.Draw_Number(DrawWard_x+5,DrawWard_y+5,LastSecond,RGB,"Arial18");			
				}

			}else{
				if (pos->szName=="VisionWard" &&  pos->CD==0 )
				{//红眼
					m_D3DManager.DrawRectScale("ItemIcons",128,0,64,64,0.5,0.5,DrawWard_x,DrawWard_y,1);
					//m_D3DManager.Draw_Number(DrawWard_x+8,DrawWard_y+8,LastSecond,RGB,"Arial18");	
				}
			}



		}
	

}

void CLOLHelper::Draw_String( int posX,int posY,CStringA szChar,int color,CStringA szFontName,int DrawWidth/*=256*/ )
{
	m_D3DManager.Draw_String(posX,posY,szChar,color,szFontName,DrawWidth);
}

ClolHero* CLOLHelper::GetMainPlayer()
{

	for (unsigned int i=0;i<m_vPlayers.size();i++)
	{
		if (m_vPlayers[i].bMainPlayer)
		{
			return m_vPlayers[i].pPlayer;
		}
	}
	return 0;
}





 
//////////////////////////////////////////////////////////////////////////
int MathHPFontColor(int CurrentHP,int MaxHp)
{
	int nHP = CurrentHP;
	int nMaxHP = MaxHp;
	int n;
	byte bR, bG;
	n = nHP * 469 / nMaxHP;
	if (n > 255)
	{
		bR = 40 + (215 - (n - 255));
		bG = 255;
	}
	else
	{
		bR = 255;
		bG = n;
	}

	int iResult=D3DCOLOR_XRGB(bR, bG, 40);
	return iResult;
}




unsigned int MyFindDllTarget( CStringA szDll,CStringA szTarget,char* pTag,unsigned int LenTag,unsigned int SearchLen)
{
	unsigned int  hDllOffset=0;
	unsigned int Result=0;
	szDll.MakeLower();
	szTarget.MakeLower();

	DbgPrint("CzjlHook::FindDllTarget Begin :  %s %s\r\n",szDll,szTarget);
	if (szDll.Find(":")!=-1 && PathFileExistsA(szDll))
	{//完整路径

		DbgPrint("CzjlHook::FindDllTarget Begin :  %s 匹配完整路径 \r\n",szDll);
		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		bool bRet=Module32First(hModuleSnap,&me32);


		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll==szDll)
			{
				hDllOffset=(unsigned int)me32.hModule;

				DbgPrint("CzjlHook::FindDllTarget Begin :  %s 完整路径匹配成功 \r\n",szDll);
				break;
			}
		}
		if (hDllOffset==0)
		{
			DbgPrint("CzjlHook::FindDllTarget Begin :  %s 匹配完整路径失败 \r\n",szDll);
			return 0;
		}
	}else if (szTarget.GetLength()>0)
	{//相对路径

		DbgPrint("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径 \r\n",szDll);

		MODULEENTRY32 me32={0};
		me32.dwSize=sizeof(MODULEENTRY32);
		HANDLE hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		int i=0;
		bool bRet=Module32First(hModuleSnap,&me32);
		while(bRet)
		{ 
			i+=1; 
			bRet=Module32Next(hModuleSnap,&me32);
			CStringW strTheDll=me32.szExePath;
			CStringA szTheDll=strTheDll;
			szTheDll.MakeLower();
			if (szTheDll.Find(szTarget)!=-1 && szTheDll.Find(szDll)!=-1)
			{
				hDllOffset=(unsigned int)me32.hModule;
				DbgPrint("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径成功 \r\n",szDll);
				break;
			}
		}

		if (hDllOffset==0)
		{
			DbgPrint("CzjlHook::FindDllTarget Begin :  %s 匹配相对路径失败 \r\n",szDll);
			return 0;
		}
	}

	if (hDllOffset==0)
	{
		hDllOffset=(unsigned int)GetModuleHandleA(szDll);

		DbgPrint("CzjlHook::FindDllTarget Begin :  %s 通过GetModuleHandleA查找起始位置 \r\n",szDll);
	}

	if (hDllOffset)
	{
		//获取DLL代码段
		unsigned int CodeStart=0,CodeEnd=0,CodeSize=0;
		unsigned int SearchS=hDllOffset;
		//计算代码段长度
		bool bCodeing=0;
		while(1)
		{
			MEMORY_BASIC_INFORMATION  st={0};
			if (!VirtualQuery((void*)SearchS,&st,sizeof(st)))
			{ //PAGE_EXECUTE_READWRITE 
				break;
			}else{			
				if (st.Protect&PAGE_EXECUTE || st.Protect&PAGE_EXECUTE_READ || st.Protect&PAGE_EXECUTE_READWRITE ||st.Protect&PAGE_EXECUTE_WRITECOPY)
				{//代码段
					if (bCodeing==0)
					{//开始记录
						bCodeing=1;
						CodeStart=(int)st.BaseAddress;
					}
					CodeSize+=st.RegionSize;
					break;
				}else{//不是代码段了
					if (bCodeing==1)
					{//结束代码段
						break;
					}
				}
				SearchS+=st.RegionSize;
			}
		}

		if (CodeSize==0)
		{
			DbgPrint("%s 没有找到代码段 0x%x \r\n",szDll,CodeStart);

		}

		//查找标志位
		unsigned int searchPos=CodeStart;
		if (SearchLen)
		{//用指定长度
			CodeEnd=CodeStart+SearchLen-LenTag;
		}else{//用默认长度
			CodeEnd=CodeStart+CodeSize-LenTag;
		}
		DbgPrint("搜索起始地址 0x%x   搜索长度 0x%x \r\n",searchPos,CodeSize);
		while(searchPos<CodeEnd)
		{
			if (    memcmp(pTag, (char*)searchPos,LenTag)==0)
				break;
			searchPos++;
		}
		//找到
		if (searchPos<CodeEnd-LenTag)
		{
			Result=searchPos;
		}
	}
	DbgPrint("CzjlHook::FindDllTarget End=0x%08x \r\n",Result);
	return Result;
}
