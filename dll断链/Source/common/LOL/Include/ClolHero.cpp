#include "ClolHero.h"
#include <iostream>
#include <zjlcommon.h>
#include "ClolUnit.h"
using namespace std;

extern int bDebug;
extern bool bAppRun;
extern LOLHookOffset* g_pOffset;


 
ClolHero::ClolHero( int pUnit):ClolUnit(pUnit)
{
	//m_atlCS.Init();
	LoadSkill();//���ؼ���,����ʱ����
}

ClolHero::~ClolHero(void)
{
}


bool ClolHero::ParserSkill( unsigned int PointSkill,float CD )
{
	unsigned int SkillIndex=GetSkillIndexByPtr(PointSkill);//���������������
	//��ȡCD

	//�����۵�
	//CheckWardSkill(PointSkill);

	if (SkillIndex!=-1)
	{//��Ӣ�۵ļ���
	//	DbgPrint("ParserSkill : IndexInGameArray:%d %s CD:%.01f",m_vSkills[SkillIndex].Index,m_vSkills[SkillIndex].szSkillName,CD);
		//m_atlCS.Lock();
		VSkill(SkillIndex,CD);
		//m_atlCS.Unlock();
		return 1;
	}
	return 0;//
}




int ClolHero::GetSkillLevel( unsigned int Index )
{
	__try
	{
		if (Index<m_vSkills.size())
		{
			if (m_vSkills[Index].bDisable)
			{
				return 0;
			}else{
				int pSkill=m_vSkills[Index].pSkill;
				if (IsBadReadPtr((int*)(pSkill+g_pOffset->SKILL_LEVEL),sizeof(int))==0)
				{
					return *(int*)(pSkill+g_pOffset->SKILL_LEVEL);
				}
	
			}
		}
	}
	__except(1)
	{
		return 0;
	}



	return 0;
}

float ClolHero::GetCD( unsigned int Index )
{
	
		if (Index<m_vSkills.size())
		{
			if (m_vSkills[Index].bDisable)
			{
				return -1.0;
			}else{

				//m_atlCS.Lock();
				unsigned int CDms=m_vSkills[Index].CurrentCDms;
				unsigned int EndTime=m_vSkills[Index].TimeLastUsed+CDms;
				//m_atlCS.Unlock();
				unsigned int CurrentTime=GetTickCount();
				if (CurrentTime<EndTime)
				{//CDδ����
					float CDf=(float)((float)EndTime-(float)CurrentTime)/(float)1000.0;
					//	DbgPrint("Index %d   CD=%.2f",Index,CDf);
					return CDf;
				}else{
					return 0.0;
				}
			}
		}

	return 0.0;
}

bool ClolHero::LoadSkill( )
{
	int pUNIT=(int)m_pUnit;
	DbgPrint("LoadSkill 0x%08x",pUNIT+g_pOffset->SkillArray);
	for (unsigned int i=0;i<12;i++)
	{
		unsigned int pSkill=*(int*)(pUNIT+g_pOffset->SkillArray+i*4);//����ָ��

		if (IsBadReadPtr((void*)pSkill,g_pOffset->SKILL_INFO)==0)
		{
			CStringA szOut;
/*
			int Level=*(int*)(pSkill+SKILLLEVEL);
			float CD=*(float*)(pSkill+SKILLCD);
			float Damage2=*(float*)(pSkill+SKILLDAMAGE);*/

			int pSkillInfo=*(int*)(pSkill+g_pOffset->SKILL_INFO);

			if (IsBadReadPtr((void*)pSkillInfo,g_pOffset->STRINFO)==0)
			{
				char szSkillName[128]={0};
				int CharLen=GetGameString((LOLString *)(pSkillInfo+g_pOffset->STRINFO),szSkillName);
				CStringA strSkillName=szSkillName;
				if (strSkillName!="BaseSpell")
				{
					//���
					stSkill add;
					add.Index=i;
					add.pSkill=pSkill;
					add.szSkillName=strSkillName;
					DbgPrint("Skill [0x%08x] %02d  %s",pSkill,i,strSkillName);
					m_vSkills.push_back(add);
				}
			}
		}else{
			break;
		}
	}
	return m_vSkills.size();



}

unsigned int ClolHero::GetSkillIndexByPtr( unsigned int pSearchSkill )
{
	unsigned int Index=0;

	for (unsigned int i=0;i<12;i++)
	{
		unsigned int pSkill=*(int*)(m_pUnit+g_pOffset->SkillArray+i*4);//����ָ��
		if (pSkill==pSearchSkill)
		{
			return i;
		}
	}
	return -1;
}




CStringA ClolHero::GetSkillName( unsigned int Index )
{
	int pSkill=GetSkillPtrByIndex(Index);
	CStringA szResult;
	//DbgPrint("GetSkillName1 %08x %08x   %d",m_pUnit,pSkill,Index);
	if (pSkill)
	{
		char szSkillName[1024]={0};
		if (IsBadReadPtr((void*)pSkill,g_pOffset->SKILL_INFO)==0)
		{
			CStringA szOut;
			int pSkillInfo=*(int*)(pSkill+g_pOffset->SKILL_INFO);
			//DbgPrint("GetSkillName2 %08x   %d",pSkillInfo,Index);
			if (IsBadReadPtr((void*)pSkillInfo,g_pOffset->STRINFO)==0)
			{
				char szSkillName[128]={0};
				int CharLen=GetGameString((LOLString *)(pSkillInfo+g_pOffset->STRINFO),szSkillName);
				szResult=szSkillName;
			}
		}
	}

	return szResult;
}


unsigned int ClolHero::GetSkillPtrByIndex( unsigned int Index )
{
	unsigned int pSkill=0;
		if (Index<0x3F)
		{
		//	DbgPrint("GetSkillPtrByIndex : %08x ",m_pUnit+g_pOffset->SkillArray1+Index*4);
			if (IsBadReadPtr( m_pUnit+g_pOffset->SkillArray+Index*4,sizeof(int)  )  ==0)
			{		
			 pSkill=*((int*)(m_pUnit+g_pOffset->SkillArray+Index*4));//����ָ��
			}
		}

		return pSkill;


}



void ClolHero::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	if (IndexVector<m_vSkills.size())
	{

		m_vSkills[IndexVector].TimeLastUsed=CurrentTime;
		m_vSkills[IndexVector].CD=CD;
		//�Զ��崦��


		//Ĭ�ϴ���
		m_vSkills[IndexVector].CurrentCDms=unsigned int(CD*1000);
		m_vSkills[IndexVector].UsedCount++;
	}
}










void CLOL_Garen::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	if (pSK->szSkillName=="GarenQ")
	{
		CD+=4.5+1.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="GarenQAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}
	if (pSK->szSkillName=="GarenE")
	{
		CD+=3.0;//3���ͷ�ʱ��
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Mordekaiser::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	//�Զ��崦��
	
	//[7556] *- ParserSkill : IndexInGameArray:0 MordekaiserMaceOfSpades CD:7.0 [0x20a035d8] 

	if (pSK->szSkillName=="MordekaiserMaceOfSpades")
	{
		CD+=10.0;//Buff����ʱ��
	}
	//[7556] *- ParserSkill : IndexInGameArray:47 MordekaiserNukeOfTheBeast CD:0.0 [0x27e68f30] 
	if (pSK->szSkillName=="MordekaiserNukeOfTheBeast")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}


	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Nasus::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	//�Զ��崦��

	//[948] *- ParserSkill : IndexInGameArray:0 NasusQ CD:8.0 [0x209cdca0] 
	if (pSK->szSkillName=="NasusQ")
	{
		CD+=10.0;//Buff����ʱ��
	}
	//[948] *- ParserSkill : IndexInGameArray:45 NasusQAttack CD:8.0 [0x27fcfcd8] 
	if (pSK->szSkillName=="NasusQAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}


	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Vayne::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	//�Զ��崦��

	//[7576] *- ParserSkill : IndexInGameArray:0 VayneTumble CD:6.0 [0x27f4db58] 
	if (pSK->szSkillName=="VayneTumble")
	{
		CD+=7.0;//Buff����ʱ��
	}
	//[7532] *- ParserSkill : IndexInGameArray:47 VayneTumbleAttack CD:0.0 [0x2ac02e38] 
	if (pSK->szSkillName=="VayneTumbleAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}


	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}


void CLOL_Volibear::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	//�Զ��崦��

	//[8124] *- ParserSkill : IndexInGameArray:0 VolibearQ CD:12.0 [0x27ef1610] 
	if (pSK->szSkillName=="VolibearQ")
	{
		CD+=4.0;//Buff����ʱ��
	}
	//[8124] *- ParserSkill : IndexInGameArray:45 VolibearQAttack CD:0.0 [0x27ef16f8] 
	if (pSK->szSkillName=="VolibearQAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}


	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_MonkeyKing::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��

	//[3708] *- ParserSkill : IndexInGameArray:0 MonkeyKingDoubleAttack CD:9.0 [0x217b9e48] 
	if (pSK->szSkillName=="MonkeyKingDoubleAttack")
	{
		CD+=6.0;//Buff����ʱ��
	}
	//[3708] *- ParserSkill : IndexInGameArray:47 MonkeyKingQAttack CD:0.0 [0x2a8ac180] 
	if (pSK->szSkillName=="MonkeyKingQAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}
	//[3708] *- ParserSkill : IndexInGameArray:3 MonkeyKingSpinToWin CD:120.0 [0x217b9b90] 
	if (pSK->szSkillName=="MonkeyKingSpinToWin")
	{
		CD+=4.0;
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}


void CLOL_TwistedFate::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	//�Զ��崦��

	//[4200] *- ParserSkill : IndexInGameArray:1 PickACard CD:6.0 [0x2a3018a0] 
	if (pSK->Index==SKILINDEX_W)
	{//����
				CStringA szCurrentSkillName=GetSkillName(pSK->pSkill);

				if (szCurrentSkillName=="PickACard")
				{
					cout<<"PickACard"<<endl;
					pSK->CD=CD;					//��¼W��CD
					CD=pSK->CD+6.0;//���û�κβ���,Buff����ʱ��
				}

				if (szCurrentSkillName.Right(4)=="lock")
				{//����
					cout<<"������"<<endl;
					CD=pSK->CD+6.0;//���û�κβ���,Buff����ʱ��
				}
	}else{//��������CD����
		pSK->CD=CD;
	}//W����
	

		/*
			[4200] *- bluecardpreattack [0x00000030] 
			[4200] *- redcardpreattack [0x00000031] 
			[4200] *- goldcardpreattack [0x00000032] 
		*/
			if (pSK->szSkillName.Right(6)=="attack")
			{
				cout<<"ʹ����"<<endl;
				m_vSkills[SKILINDEX_W].TimeLastUsed=CurrentTime;
				m_vSkills[SKILINDEX_W].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_W].CD*1000);//1����ˢ��CD
			}


	if (pSK->szSkillName=="Destiny")
	{
		CD+=8.0;
	}

	pSK->CurrentCDms=unsigned int(CD*1000);//�䶯��CD?
	pSK->UsedCount++;
}


void CLOL_Jax::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	pSK->UsedCount++;
	//[12012] *- JaxEmpowerTwo [0x00000001]   W
	if (pSK->szSkillName=="JaxEmpowerTwo")
	{
		CD+=3.0;//3���ͷ�ʱ��
	}
	//[12012] *- ParserSkill : IndexInGameArray:2 JaxCounterStrike CD:9.5 [0x281a7770] 
	if (pSK->szSkillName=="JaxCounterStrike")
	{
		CD+=2.0;//3���ͷ�ʱ��
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}


void CLOL_Ryze::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;
	//[12012] *- JaxEmpowerTwo [0x00000001]   W

	if (IndexVector<4)//0 1 2 3
	{//����CD
		for (unsigned int i=0;i<4;i++)
		{
			if (IndexVector!=i)//�����Լ�
			{
				stSkill* pOtherSK=&m_vSkills[i];
				if (pOtherSK->CurrentCDms>1000)
				{
					pOtherSK->CurrentCDms-=1000;
				}else{
					pOtherSK->CurrentCDms=0;
				}
			}
		}
	}


	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Ahri::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->CD=CD;
	//�Զ��崦��
	//[5344] *- ParserSkill : IndexInGameArray:1 AhriFoxFire CD:9.0 [0x2333e8b8]    W�ͷ�
	//[6100] *- ParserSkill : IndexInGameArray:48 AhriFoxFireMissileTwo CD:0.0 [0x2b2f3eb8]  W����
	if (pSK->szSkillName=="AhriFoxFire")
	{
		CD+=5.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="AhriFoxFireMissileTwo")
	{
		m_vSkills[SKILINDEX_W].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_W].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_W].CD*1000);//1����ˢ��CD
	}

	//[6100] *- ParserSkill : IndexInGameArray:3 AhriTumble CD:110.0 [0x286ac678] //����
	if (pSK->szSkillName=="AhriTumble")
	{
		if (CurrentTime-pSK->TimeLastUsed>1000*10)
		{//�µĴ���
			//cout<<"�µĿ�ʼ,NO1\r\n";
			pSK->UsedCount=0;
			CD+=10.0;//Buff����ʱ��
		}else{//����
			switch (pSK->UsedCount)
			{
			case 1:
				{
				//	cout<<"��ʼ��ʱ,NO2\r\n";
					pSK->UsedCount++;
					return;
				}
				break;
			case 2:
				{
			//		cout<<"��ʼ��ʱ,NO3\r\n";
				}
				break;
			}
		}
	}

	//Ĭ�ϴ���
	pSK->TimeLastUsed=CurrentTime;
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}


void CLOL_Blitzcrank::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[3196] *- ParserSkill : IndexInGameArray:2 PowerFist CD:9.0 [0x2758bc88]  E
	if (pSK->szSkillName=="PowerFist")
	{
		CD+=10.;//Buff����ʱ��
	}
	//[3196] *- ParserSkill : IndexInGameArray:46 PowerFistAttack CD:0.0 [0x2a4248b8] E�ͷ�
	if (pSK->szSkillName=="PowerFistAttack")
	{
		m_vSkills[SKILINDEX_E].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_E].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_E].CD*1000);//1����ˢ��CD
	}
	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}



void CLOL_Darius::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[9332] *- ParserSkill : IndexInGameArray:1 DariusNoxianTacticsONH CD:8.0 [0x275ebab8]  W
	//[9332] *- ParserSkill : IndexInGameArray:45 DariusNoxianTacticsONHAttack CD:0.0 [0x275ee298] 

	if (pSK->szSkillName=="DariusNoxianTacticsONH")
	{
		CD+=4.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="DariusNoxianTacticsONHAttack")
	{
		m_vSkills[SKILINDEX_W].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_W].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_W].CD*1000);//1����ˢ��CD
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Fizz::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[4124] *- ParserSkill : IndexInGameArray:2 FizzJump CD:14.0 [0x21fe4460] 
	if (pSK->szSkillName=="FizzJump")
	{
		CD+=2.0;//3���ͷ�ʱ��
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}





void CLOL_Shyvana::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[7176] *- ParserSkill : IndexInGameArray:0 ShyvanaDoubleAttack CD:10.0 [0x228774c0] 
	//[7176] *- ParserSkill : IndexInGameArray:45 ShyvanaDoubleAttackHit CD:0.0 [0x2af56d50] 

	if (pSK->szSkillName=="ShyvanaDoubleAttack")
	{
		CD+=6.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="ShyvanaDoubleAttackHit")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}
	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Talon::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[7224] *- ParserSkill : IndexInGameArray:0 TalonNoxianDiplomacy CD:8.0 [0x224d64f8] 
	//[7224] *- ParserSkill : IndexInGameArray:45 TalonNoxianDiplomacyAttack CD:0.0 [0x2ac9c418] 

	if (pSK->szSkillName=="TalonNoxianDiplomacy")
	{
		CD+=5.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="TalonNoxianDiplomacyAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}


void CLOL_XinZhao::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	////[5952] *- ParserSkill : IndexInGameArray:0 XenZhaoComboTarget CD:8.0 [0x266ac640]  Q
	if (pSK->szSkillName=="XenZhaoComboTarget")
	{
		CD+=5.0;//Buff����ʱ��
	}

/*
	[5952] *- ParserSkill : IndexInGameArray:45 XenZhaoThrust CD:0.0 [0x26d4f240] 
	[5952] *- ParserSkill : IndexInGameArray:46 XenZhaoThrust2 CD:0.0 [0x26d4f328] 
	[5952] *- ParserSkill : IndexInGameArray:47 XenZhaoThrust3 CD:0.0 [0x26d4f410] */
	if (pSK->szSkillName.Left(13)=="XenZhaoThrust")
	{
		//cout<<"��CD!\r\n";
		for (unsigned int i=1;i<4;i++)//WER CD
		{	
				stSkill* pOtherSK=&m_vSkills[i];
				if (pOtherSK->CurrentCDms>1000)
				{
					pOtherSK->CurrentCDms-=1000;
				}else{
					pOtherSK->CurrentCDms=0;
				}
		}

		if (pSK->szSkillName=="XenZhaoThrust3")
		{
			m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
			m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
		}
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Trundle::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[5944] *- ParserSkill : IndexInGameArray:0 TrundleTrollSmash CD:4.0 [0x20c558f8]   Q 
	//[5944] *- ParserSkill : IndexInGameArray:45 TrundleQ CD:0.0 [0x29b48268] Q Attack

	if (pSK->szSkillName=="TrundleTrollSmash")
	{
		CD+=7.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="TrundleQ")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Zed::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	if (pSK->Index==SKILINDEX_W)
	{//����
		CStringA szCurrentSkillName=GetSkillName(pSK->pSkill);
		//[4448] *- ParserSkill : IndexInGameArray:1 ZedShadowDash CD:18.0 [0x20a51c70] 
		if (szCurrentSkillName=="ZedShadowDash")//��һ���ͷ�
		{
			CD+=4.0;//Buff����ʱ��
		}
		if (szCurrentSkillName=="zedw2")//��һ���ͷ�
		{
			m_vSkills[SKILINDEX_W].TimeLastUsed=CurrentTime;
			m_vSkills[SKILINDEX_W].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_W].CD*1000);//1����ˢ��CD
		}
	}
	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

void CLOL_Lucian::VSkill( unsigned int IndexVector,float CD )
{
	
	unsigned int CurrentTime=GetTickCount();
	stSkill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	//[5104] *- ParserSkill : IndexInGameArray:51 LucianPassiveShot CD:0.0 [0x26c42658] ������CD
	if (pSK->szSkillName=="LucianPassiveShot")
	{
		stSkill* pOtherSK=&m_vSkills[SKILINDEX_E];
		if (pOtherSK->CurrentCDms>1500)
		{
			pOtherSK->CurrentCDms-=1500;
		}else{
			pOtherSK->CurrentCDms=0;
		}
	}
	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}

/*
void CLOL_Garen::VSkill( unsigned int IndexVector,float CD )
{
	unsigned int CurrentTime=GetTickCount();
	Skill* pSK=&m_vSkills[IndexVector];
	pSK->TimeLastUsed=CurrentTime;
	pSK->CD=CD;

	//�Զ��崦��
	if (pSK->szSkillName=="GarenQ")
	{
		CD+=4.5+1.0;//Buff����ʱ��
	}
	if (pSK->szSkillName=="GarenQAttack")
	{
		m_vSkills[SKILINDEX_Q].TimeLastUsed=CurrentTime;
		m_vSkills[SKILINDEX_Q].CurrentCDms=unsigned int(m_vSkills[SKILINDEX_Q].CD*1000);//1����ˢ��CD
	}
	if (pSK->szSkillName=="GarenE")
	{
		CD+=3.0;//3���ͷ�ʱ��
	}

	//Ĭ�ϴ���
	pSK->CurrentCDms=unsigned int(CD*1000);
	pSK->UsedCount++;
}
	
*/


ClolHero* CreateHero(int pHero)
{
	ClolUnit unit(pHero);
	CStringA szHeroName=unit.GetHeroName();
	//DbgPrint("CreateHero ->%s ",szHeroName);

	ClolHero* p=0;
/*
	if (szHeroName=="Garen")//����
	{
		p=new CLOL_Garen(pHero);
	}else if (szHeroName=="Mordekaiser")
	{
		p=new CLOL_Mordekaiser(pHero);
	}else if (szHeroName=="Nasus")
	{
		p=new CLOL_Nasus(pHero);
	}else if (szHeroName=="Vayne")
	{
		p=new CLOL_Vayne(pHero);
	}else if (szHeroName=="Volibear")
	{
		p=new CLOL_Volibear(pHero);
	}else if (szHeroName=="MonkeyKing")
	{
		p=new CLOL_MonkeyKing(pHero);
	}else if (szHeroName=="TwistedFate")
	{
		p=new CLOL_TwistedFate(pHero);
	}else if (szHeroName=="Jax")
	{
		p=new CLOL_Jax(pHero);
	}else if (szHeroName=="Ryze")
	{
		p=new CLOL_Ryze(pHero);
	}else if (szHeroName=="Ahri")
	{
		p=new CLOL_Ahri(pHero);
	}else if (szHeroName=="Blitzcrank")
	{
		p=new CLOL_Blitzcrank(pHero);
	}else if (szHeroName=="Darius")
	{
		p=new CLOL_Darius(pHero);
	}else if (szHeroName=="Fizz")
	{
		p=new CLOL_Fizz(pHero);
	}else if (szHeroName=="Shyvana")
	{
		p=new CLOL_Shyvana(pHero);
	}else if (szHeroName=="Talon")
	{
		p=new CLOL_Talon(pHero);
	}else if (szHeroName=="XinZhao")
	{
		p=new CLOL_XinZhao(pHero);
	}else if (szHeroName=="Trundle")
	{
		p=new CLOL_Trundle(pHero);
	}else if (szHeroName=="Zed")
	{
		p=new CLOL_Zed(pHero);
	}else if (szHeroName=="Lucian")
	{
		p=new CLOL_Lucian(pHero);
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="MasterYi")
	{
		p=new CLOL_Lucian(pHero);
		p->DisableSkill(SKILINDEX_Q);
	}
*/


	/*
	//����ĳЩ���ܵ�
	if (szHeroName=="Zyra")
	{//����֮��
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_W);//�ر�2����
	}else if (szHeroName=="Alistar")
	{//ţͷ����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Amumu")
	{//��֮ľ����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Akali")
	{//������
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="Cassiopeia")
	{//ħ��֮ӵ
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Corki")
	{//Ӣ��Ͷ����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="Diana")
	{//���Ů��
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_W);
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="Draven")
	{//��ҫ���̹�
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_W);
	}else if (szHeroName=="Graves")
	{//�����ͽ
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Irelia")
	{//������־
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="KhaZix")
	{//������־
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Olaf")
	{//������
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Rammus")
	{//����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Renekton")
	{//��Į����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Rumble")
	{//��е����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="Riven")
	{//����
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="Shaco")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Skarner")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Teemo")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_R);
	}else if (szHeroName=="Thresh")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Tryndamere")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_E);
	}else if (szHeroName=="VelKoz")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_W);
	}else if (szHeroName=="Yorick")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Gnar")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}else if (szHeroName=="Azir")
	{
		p=new CLOLBase();
		p->DisableSkill(SKILINDEX_Q);
	}



	 

	//�ݲ������
	if (szHeroName=="Elise" ||szHeroName=="Jayce" ||szHeroName=="Nidalee"   ||szHeroName=="Rengar"  ||szHeroName=="Sivir"   ||szHeroName=="Udyr"||szHeroName=="Varus"||  szHeroName=="Vi"||  szHeroName=="Xerath"\
		||szHeroName=="Yorick" ||szHeroName=="Zed"||szHeroName=="Zec"||szHeroName=="Yasuo")// || szHeroName=="Skarner" ||szHeroName=="Skarner" ||szHeroName=="Skarner" ||szHeroName=="Skarner" ||szHeroName=="Skarner" ||szHeroName=="Skarner" || )
	{//֩��Ů��,��˹,��ҰŮ����,��֮׷����
		return 0;
	}
*/



	if (p==0)
	{//����Ӣ��
		p=new ClolHero(pHero);
	}
	return p;
}