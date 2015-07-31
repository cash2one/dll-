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

	//ע��ģ�����
	bool AddPlayer(ClolHero* pHero,bool bMainPlayer=0);//�������ָ��,����
	void CallBackUseSkillCD(unsigned int ESI,float CD);//����ָ��


	float GetHeroSkillCD(void* PlayerPoint,unsigned int IndexSkill);//��ѯ��,IndexSkill =  0-3 QWER
	float GetHeroSkillCDByIndex( int Team,int Index,unsigned int IndexSkill );//����CD


	CStringA GetHeroSkillName( void* PlayerPoint,unsigned int IndexSkill );//���ؼ�������
	int GetHeroSkillLevel(void* PlayerPoint,unsigned int IndexSkill);


private:
	vector<ClolHero*> m_vHeros;
	ClolHero* pMainHero;
};

