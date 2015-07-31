#pragma once
#include "windows.h"
#include <winbase.h>
#include <atlstr.h>
#include <vector>
#include <zjlcommon.h>
using namespace std;


#define SKILINDEX_Q 0
#define SKILINDEX_W 1
#define SKILINDEX_E 2
#define SKILINDEX_R 3

/*

enum OffsetUNIT {
	TEAM=0x14,
	PLAYERNAME=0x20,
	HERONAME=0x38,
	X=0x5C,
	Z=0x60,
	Y=0x64,
	INDEX=0xF8,
	HP=0x13C,
	MAXHP=HP+0x10,//0x14C,
	MP=0x1B8,
	MAXMP=MP+0x10,
	WARDSEC=0x1B8,
/ *
	//2-19
	DAMAGE_WEAPON=0x764,//武器附加伤害
	DAMAGE=0x7CC,
	SkillArray1=0x1704,
	LEVEL=0x237C* /
	//3-3
	DAMAGE_WEAPON=0x774,//武器附加伤害
	DAMAGE=0x7DC,

	SkillUseArg=0x1F18,	//Ecx用的参数
	SkillUseIndex=0x1F24,//填充使用的技能
	SkillArray1=0x2400,

	LEVEL=0x3084



};



/ *
3-18
enum OffsetUNIT {
	TEAM=0x14,
	PLAYERNAME=0x20,
	HERONAME=0x38,
	X=0x5C,
	Z=0x60,
	Y=0x64,
	INDEX=0xF8,
	HP=0x170,//134
	MAXHP=HP+0x10,
	MP=0x1EC,
	MAXMP=MP+0x10,

	WARDSEC=0x1EC,
/ *
	//2-19
	DAMAGE_WEAPON=0x764,//武器附加伤害
	DAMAGE=0x7CC,
	SkillArray1=0x1704,
	LEVEL=0x237C* /
	//3-3
	DAMAGE_WEAPON=0x774,//武器附加伤害
	DAMAGE=0x7DC,

	SkillUseArg=0x1F18,	//Ecx用的参数
	SkillUseIndex=0x1F24,//填充使用的技能
	SkillArray1=0x2700,

	LEVEL=0x3084
};* /



enum OffsetSKILL {
	SKILLLEVEL=0x10,
	SKILLCD=0x2C,//float
	SKILLDAMAGE=0x44,//附加伤害
	SKILLCLASS=0xD0,	//释放技能用类
	SKILLINFO=0xD4
};

enum OffsetSTRINFO{
	STRINFO=0x18
};*/



typedef struct LOLString{  //字符串结构
	char Str[16];
	int len;
	int flag;
};




#define CALL_USESKILL 0x



 struct LOLHookOffset{
	char szVersion[256];
	char szMD5[256];

	unsigned int Offset_MainPlayer;
	unsigned int Offset_PlayersArray1;
	unsigned int Offset_PlayersArray2;
	unsigned int Offset_Event;
	unsigned int Offset_SkillUsed;
	unsigned int Offset_DrawHp;
	unsigned int Offset_DrawHeroHp;
	unsigned int Offset_Say;
	unsigned int Offset_TowerC;
	unsigned int Offset_PutWard;

	unsigned int Offset_UseSkill;
	unsigned int Offset_Attack;
//Unit
	unsigned int TEAM;
	unsigned int UNITTYPE;
	unsigned int PLAYERNAME;
	unsigned int HERONAME;
	unsigned int X;
	unsigned int Z;
	unsigned int Y;
	unsigned int INDEX;
	unsigned int HP;
	unsigned int MAXHP;//0x14C;
	unsigned int MP;
	unsigned int MAXMP;
	unsigned int DAMAGE_WEAPON;//武器附加伤害
	unsigned int DAMAGE;
	unsigned int AttackRange;

		/*
	[9140] =,=  e0 8f 27 01 |  ff ff ff ff |  05 00 00 00 |  04 00 00 00    SkillUseArg  Use 4
	[9140] =,=  0c 36 25 2f |  cc 31 25 2f |  28 33 25 2f |  c8 39 25 2f    SkillUseArg 

	Arg
	-1=Arg+0x04
	选择的技能=Arg+0x08 *只有
	使用的技能=Arg+0x0C
	*/
	unsigned int SkillUseArg_Ecx;
	unsigned int SkillUseArgIndex;
	unsigned int SkillArray;
	unsigned int LEVEL;

//Skill
	unsigned int SKILL_LEVEL;
	unsigned int SKILL_CD;//float
	unsigned int SKILL_DAMAGE;//附加伤害
	unsigned int SKILL_CLASS;	//释放技能用类
	unsigned int SKILL_INFO;	//技能信息

	unsigned int SKILL_INFO_SKILLTYPE;//技能类型,1是指向性,0不是
	unsigned int SKILL_INFO_RANGE;
	unsigned int STRINFO;
};




int GetGameString(LOLString *Str,char *OutStr);
void MyOutputDebugStringA(char* pChar,int Hex);
LOLHookOffset* GetVersionOffset();
void OffsetInit();