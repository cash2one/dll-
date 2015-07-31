#pragma once

#include <atlstr.h>

struct stLOLPos{
	float x;
	float z;
	float y;
	int Flag;
};

class ClolUnit
{
public:
	ClolUnit(int* pUnit);
	ClolUnit(int pUnit);
	~ClolUnit(void);

	bool IsGoodPtr();

	bool IsHero();//1401
	bool IsUnit();//Ð¡±ø0xC01
	bool IsBulid();//2401
	bool IsCreep();//C01 Ò°¹Ö
	unsigned int GetType();

	CStringA PrintState();
	unsigned int GetPtr();

	unsigned int GetIndex();
	unsigned int GetTeam();//100 200 ÖÐÁ¢300
	CStringA GetName();
	CStringA GetHeroName();
	unsigned int GetX();
	unsigned int GetY();
	float GetfX();
	float GetfZ();
	float GetfY();
	stLOLPos GetPos();
	unsigned int GetHP();
	unsigned int GetMaxHP();
	unsigned int GetMP();
	unsigned int GetMaxMP();
	unsigned int GetLevel();
	unsigned int GetAttackRange();
	//
	unsigned int GetAttackDamage();
	unsigned int MathHowLong(int TargetUnit);



protected:
	char* m_pUnit;
};

 