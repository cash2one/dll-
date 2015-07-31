#include "LOLOffset.h"
#include <atlstr.h>


vector <LOLHookOffset> VersionOffset;

void add6_8()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.10.1.330");
	//Offset
	add.Offset_MainPlayer=0x10E65EC;
	add.Offset_PlayersArray1=0xBBFE1F;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x73A755;	//8B 44 24 14           - mov eax,[esp+14]****
	add.Offset_SkillUsed=0xA3E57E; //88 86 9A000000        - mov [esi+0000009A],al

	add.Offset_DrawHeroHp=0xA52985;
	add.Offset_DrawHp=0xA52A3D;

	add.Offset_Say=0x94DFF2;//mov ecx,["League of Legends.exe"+2DF1F54]
	add.Offset_TowerC=0x4413E6;//call "League of Legends.exe"+B5BAA0
	add.Offset_PutWard=0x5E26DE;// comiss xmm0,xmm1


	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x938;//武器附加伤害  0x938
	add.DAMAGE=0x9A0;//+0x68

	add.AttackRange=0x9CC;

	//技能用+0x24
	add.SkillUseArg_Ecx=0x1134;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x2228;//释放技能时填充使用的技能索引

	add.SkillArray=0x2730;
	add.LEVEL=0x33B4;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x794;//1是指向性,0是被动型技能
	add.SKILL_INFO_RANGE=0x538;
	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}

void add5_26()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.9.1.325");
	//Offset
	add.Offset_MainPlayer=0x10EAA68;
	add.Offset_PlayersArray1=0xBCA47F;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x9CE575;	//8B 44 24 14           - mov eax,[esp+14]****
	add.Offset_SkillUsed=0x214CCE; //88 86 9A000000        - mov [esi+0000009A],al

	add.Offset_DrawHeroHp=0x6A9E35;
	add.Offset_DrawHp=0x6A9EED;

	add.Offset_Say=0x249F02;//mov ecx,["League of Legends.exe"+2DF1F54]
	add.Offset_TowerC=0xA3A056;//call "League of Legends.exe"+B5BAA0
	add.Offset_PutWard=0x4333FE;// comiss xmm0,xmm1


	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

	//技能用+0x24
	add.SkillUseArg_Ecx=0x1124;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x2218;//释放技能时填充使用的技能索引

	add.SkillArray=0x2720;
	add.LEVEL=0x33A4;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x794;//1是指向性,0是被动型技能
	add.SKILL_INFO_RANGE=0x538;
	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}

void add5_12()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.8.1.463");
	//Offset
	add.Offset_MainPlayer=0x1173D74;
	add.Offset_PlayersArray1=0xBFC6BF;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x36EAA5;	//8B 44 24 14           - mov eax,[esp+14]****
	add.Offset_SkillUsed=0xA998FE; //88 86 9A000000        - mov [esi+0000009A],al

	add.Offset_DrawHeroHp=0xB964F5;
	add.Offset_DrawHp=0xB965AD;

	add.Offset_Say=0x656162;//mov ecx,["League of Legends.exe"+2DF1F54]
	add.Offset_TowerC=0x7DD5A6;//call "League of Legends.exe"+B5BAA0
	add.Offset_PutWard=0x5ECBDE;// comiss xmm0,xmm1


	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

	//技能用+0x24
	add.SkillUseArg_Ecx=0x1124;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x2218;//释放技能时填充使用的技能索引

	add.SkillArray=0x2720;
	add.LEVEL=0x33CC;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x794;//1是指向性,0是被动型技能

	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}

void add5_4()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.8.1.448");
	//Offset
	add.Offset_MainPlayer=0x1173D74;
	add.Offset_PlayersArray1=0xBFC6BF;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x36EAA5;	//8B 44 24 14           - mov eax,[esp+14]****
	add.Offset_SkillUsed=0xA998FE; //88 86 9A000000        - mov [esi+0000009A],al

	add.Offset_DrawHeroHp=0xB964F5;
	add.Offset_DrawHp=0xB965AD;

	add.Offset_Say=0x656162;//mov ecx,["League of Legends.exe"+2DF1F54]
	add.Offset_TowerC=0x7DD5A6;//call "League of Legends.exe"+B5BAA0
	add.Offset_PutWard=0x5ECBDE;// comiss xmm0,xmm1

	add.Offset_UseSkill=0xBD3BE0;
	add.Offset_Attack=0x8F0600;

	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

	//技能用+0x24
	add.SkillUseArg_Ecx=0x2210;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x2218;//释放技能时填充使用的技能索引

	add.SkillArray=0x2720;
	add.LEVEL=0x33CC;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x794;//1是指向性,0是被动型技能

	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}

void add4_15()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.7.1.278");
	//Offset
	add.Offset_MainPlayer=0x10AA3A0;
	add.Offset_PlayersArray1=0xB6E70F;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x99DDB5;
	add.Offset_SkillUsed=0x765A0E; //88 86 9A000000        - mov [esi+0000009A],al

	add.Offset_DrawHeroHp=0x615C35;
	add.Offset_DrawHp=0x615CED;

	add.Offset_Say=0x77C7E2;//mov ecx,["League of Legends.exe"+2DF1F54]
	add.Offset_TowerC=0x73D8A6;//call "League of Legends.exe"+B5BAA0
	add.Offset_PutWard=0x5CF9DE;// comiss xmm0,xmm1

	add.Offset_UseSkill=0xBD3BE0;
	add.Offset_Attack=0x8F0600; 

	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

	//技能用+0x24
	add.SkillUseArg_Ecx=0x2210;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x2218;//释放技能时填充使用的技能索引

	add.SkillArray=0x2720;
	add.LEVEL=0x33CC;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x7C0;//1是指向性,0是被动型技能

	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}

/*

void add3_31()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.6.1.190");
	//Offset
	add.Offset_MainPlayer=0x11565AC;
	add.Offset_PlayersArray1=0xC29B1F;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x417E55;
	add.Offset_Skill=0x49236E;

	add.Offset_DrawHeroHp=0x70E470;
	add.Offset_DrawHp=0x70E528;
	
	add.Offset_Say=0xBD3F72;
	add.Offset_TowerC=0x638ED6;
	add.Offset_PutWard=0x3C0729;

	add.Offset_UseSkill=0x40C530;
	add.Offset_Attack=0x261400;

//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

//技能用
	add.SkillUseArg_Ecx=0x1104;	//释放技能时Ecx用的参数
	add.SkillUseArgIndex=0x21F4;//释放技能时填充使用的技能索引

	add.SkillArray=0x2700;
	add.LEVEL=0x33B4;

//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;	//技能信息

	//add.SKILL_INFO里的偏移
	add.SKILL_INFO_SKILLTYPE=0x7C4;//1是指向性,0是被动型技能

	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}*/

/*

void add_3_23()
{
	LOLHookOffset add;
	//3-18
	memset(&add,0,sizeof(LOLHookOffset));
	StrCpyA(add.szVersion,"5.5.1.283");
	//Offset
	add.Offset_MainPlayer=0x108CDC0;
	add.Offset_PlayersArray1=0xB9F921;
	add.Offset_PlayersArray2=add.Offset_PlayersArray1+0x08;

	add.Offset_Event=0x7C8127;
	add.Offset_Skill=0x27D690;

	add.Offset_DrawHp=0x678BD1;
	add.Offset_DrawHeroHp=0x678BFF;

	add.Offset_Say=0x92D684;
	add.Offset_TowerC=0xA49816;
	add.Offset_PutWard=0x4429C9;
	//stUnit 单位偏移
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x170;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1EC;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x928;//武器附加伤害
	add.DAMAGE=0x990;
	add.AttackRange=0x9CC;

	add.SkillUseArg=0x21E8;	//Ecx用的参数
	add.SkillUseIndex=0x21F0;//填充使用的技能
	add.SkillArray1=0x2700;
	add.LEVEL=0x33B4;

	//Skill技能偏移
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;
	//Str
	add.STRINFO=0x18;
	VersionOffset.push_back(add);
}
*/

/*

void add_2_25()
{
	LOLHookOffset add;

	memset(&add,0,sizeof(LOLHookOffset));
	//2-25
	StrCpyA(add.szVersion,"5.4.1.242");
//Offset
	add.Offset_MainPlayer=0x1130b1c;
	add.Offset_PlayersArray1=0x95C5FF;
	add.Offset_PlayersArray2=0x95C607;

	add.Offset_Event=0x729B45;
	add.Offset_Skill=0x26D87E;
	add.Offset_DrawHeroHp=0x434CDF;
	add.Offset_Say=0x78444C;
	add.Offset_TowerC=0x669F26;
	add.Offset_DrawHp=0x434CB1;
	add.Offset_PutWard=0x4F5E49;

	add.Offset_UseSkill=0x92D170;
	add.Offset_Attack=0x452190;

//stUnit
	add.TEAM=0x14;
	add.UNITTYPE=0x18;
	add.PLAYERNAME=0x20;
	add.HERONAME=0x38;
	add.X=0x5C;
	add.Z=0x60;
	add.Y=0x64;
	add.INDEX=0xF8;
	add.HP=0x13C;
	add.MAXHP=add.HP+0x10;//0x14C;
	add.MP=0x1B8;
	add.MAXMP=add.MP+0x10;
	add.DAMAGE_WEAPON=0x774;//武器附加伤害
	add.DAMAGE=0x7DC;
	add.AttackRange=0x818;

	add.SkillUseArg=0x1F18;	//Ecx用的参数
	add.SkillUseIndex=0x1F24;//填充使用的技能
	add.SkillArray1=0x2400;
	add.LEVEL=0x3084;
//stSkill
	add.SKILL_LEVEL=0x10;
	add.SKILL_CD=0x2C;//float
	add.SKILL_DAMAGE=0x44;//附加伤害
	add.SKILL_CLASS=0xD0;	//释放技能用类
	add.SKILL_INFO=0xD4;
//stStr
	add.STRINFO=0x18;

	VersionOffset.push_back(add);
}
*/

void OffsetInit()
{
//	add_2_25();// 2-28 5.4.1.242
//	add_3_23();//3-23 5.5.1.283
//	add3_31();//3-31
	add4_15();
	add5_4();
    add5_12();
	add5_26();
	add6_8();
}


int GetGameString(LOLString *Str,char *OutStr)
{

	__try
	{
		if (IsBadReadPtr(Str,0x18)==0)
		{
			if (Str->flag != 0xF)
			{
				int pChar=(int)&Str->Str;
				if (IsBadReadPtr((int*)pChar,Str->len)==0)
				{
					memcpy(OutStr,(const void*)*(int*)pChar,Str->len);	
				}

			}
			else
			{
				memcpy(OutStr,Str->Str,Str->len);	
			}
			return strlen(OutStr);
		}

	}
	__except(1)
	{
		return -1;
	}

	return -1;
}

void MyOutputDebugStringA(char* pChar,int Hex)
{
	CStringA sz;
	sz.Format("*- %s [0x%08x]\r\n",pChar,Hex);
	OutputDebugStringA(sz);
}

LOLHookOffset* GetVersionOffset()
{
	OffsetInit();
	CStringA szVersion=GetFileVersion("League of Legends.exe");

	unsigned int Index=0;
	for (unsigned int i=0;i<VersionOffset.size();i++)
	{
		if (VersionOffset[i].szVersion==szVersion)
		{
			return &VersionOffset[i];
		}
	}
	return 0;
}


