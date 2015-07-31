#pragma once
#include "atlstr.h"
#include <winbase.h>
#include <VMPDefine.h>
#include <tlhelp32.h>
#include <iostream>
#include "LOLOffset.h"
#include "ClolHero.h"
#include <vector>

using namespace std;



class CLOLSkillManager
{
public:
	CLOLSkillManager(void);
	~CLOLSkillManager(void);

	//注入模块调用
	bool AddPlayer(ClolHero* pHero,bool bMainPlayer=0);//玩家数组指针,数量
	void CallBackUseSkillCD(unsigned int ESI,float CD);//技能指针


	float GetHeroSkillCD(void* PlayerPoint,unsigned int IndexSkill);//查询用,IndexSkill =  0-3 QWER
	float GetHeroSkillCDByIndex( int Team,int Index,unsigned int IndexSkill );//返回CD


	CStringA GetHeroSkillName( void* PlayerPoint,unsigned int IndexSkill );//返回技能名字
	int GetHeroSkillLevel(void* PlayerPoint,unsigned int IndexSkill);


private:
	vector<ClolHero*> m_vHeros;
	ClolHero* pMainHero;
};

