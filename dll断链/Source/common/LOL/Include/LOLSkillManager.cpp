#include "LOLSkillManager.h"
#include <zjlcommon.h>
#include "ClolUnit.h"

extern int bDebug;
extern bool bAppRun;
extern LOLHookOffset* g_pOffset;

unsigned int g_HEROARRAY=0;
unsigned int g_PlayerCount=0;





void CLOLSkillManager::CallBackUseSkillCD( unsigned int ESI ,float CD)
{	
	for (unsigned int i=0;i<m_vHeros.size();i++)//遍历所有本地英雄
	{
		if (m_vHeros[i]->ParserSkill(ESI,CD))
		{//处理完毕

			return;
		}
	}
	
}

bool CLOLSkillManager::AddPlayer(ClolHero* pHero,bool bMainPlayer )
{
	

		
/*
		if (IsBadReadPtr((void*)pHero,HERONAME)==0	&&    IsBadReadPtr((void*)*(int*)(pHero+HERONAME),STRINFO)==0)
		{
			unsigned int pHeroNameInfo=*(int*)(pHero+HERONAME);//英雄名结构指针

			char szHeroName[128]={0};
			int CharLen=GetGameString((LOLString *)(pHeroNameInfo+STRINFO),szHeroName);//英雄名
			CStringA strszHeroName=szHeroName;
*/
			


				if (bMainPlayer)
				{
					pMainHero=pHero;
				}else{
					m_vHeros.push_back(pHero);	
				}			

				
/*
		}else{
			DbgPrint("CLOLSkillManager::AddPlayer Error 0x%08x",pHero);
			return 0;
		}*/

	
	return 1;
}


float CLOLSkillManager::GetHeroSkillCD( void* PlayerPoint,unsigned int IndexSkill )
{
	if (PlayerPoint)
	{
		for (unsigned int i=0;i<m_vHeros.size();i++)
		{
			if (m_vHeros[i]->GetPtr()==(int)PlayerPoint)
			{
				return m_vHeros[i]->GetCD(IndexSkill);
			}
		}
	}
	return 0;
}

float CLOLSkillManager::GetHeroSkillCDByIndex( int Team,int Index,unsigned int IndexSkill )
{
	int TeamCount=0;
		for (unsigned int i=0;i<m_vHeros.size();i++)
		{
			ClolUnit Unit((int*)m_vHeros[i]);
			if (Unit.GetTeam()==Team)
			{
				if (TeamCount==Index)
				{
				//	DbgPrint("GetHeroSkillCDByIndex team[%d] Index[%d] Skill[%d] CD[%f]",Team,Index,IndexSkill,m_vHeros[i]->GetCD(IndexSkill));
					return m_vHeros[i]->GetCD(IndexSkill);
				}
				TeamCount++;
			}		
		}
	return 0;


}


CLOLSkillManager::CLOLSkillManager( void )
{
}

CLOLSkillManager::~CLOLSkillManager( void )
{

}

int CLOLSkillManager::GetHeroSkillLevel( void* PlayerPoint,unsigned int IndexSkill )
{
	for (unsigned int i=0;i<m_vHeros.size();i++)
	{
		if (m_vHeros[i]->GetPtr()==(unsigned int)PlayerPoint)
		{
			return m_vHeros[i]->GetSkillLevel(IndexSkill);
		}
	}
	return 0;
}


CStringA CLOLSkillManager::GetHeroSkillName( void* PlayerPoint,unsigned int IndexSkill )
{
	for (unsigned int i=0;i<m_vHeros.size();i++)
	{
		if (m_vHeros[i]->GetPtr()==(int)PlayerPoint)
		{
			//DbgPrint("玩家%08x, 技能%d,  =%s",PlayerPoint,IndexSkill,m_vHeros[i]->GetSkillName(IndexSkill));
				return m_vHeros[i]->GetSkillName(IndexSkill);
		}		
	}
	return "";
}






