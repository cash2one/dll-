#pragma once
#include <atlstr.h>
#include <vector>
#include <winbase.h>
#include <atlcore.h>
#include "LOLOffset.h"
#include "ClolUnit.h"
using namespace  std;

class ClolHero;
ClolHero* CreateHero(int pHero);




struct stSkill{
	bool bDisable;		//有问题的技能,需要屏蔽的

	unsigned int Index;//在SkillArray里序号
	unsigned int pSkill;//技能指针
	CStringA szSkillName;//技能名
	float CD;					//最后一次使用的CD

	unsigned int UsedCount;	//技能使用计数
	unsigned int TimeLastUsed;//最后一次使用时间
	unsigned int CurrentCDms;//当前CD,毫秒

	stSkill()
	{
		Index=0;
		pSkill=0;
		CD=0.0;
		bDisable=0;

		UsedCount=0;
		TimeLastUsed=0;
		CurrentCDms=0;

	}
};


class ClolHero : public  ClolUnit//一个英雄
{
public:
	ClolHero(int pUnit);
	~ClolHero(void);

	 bool ParserSkill(unsigned int PointSkill,float CD);//解析技能消息,回调使用,返回1处理成功
	float GetCD(unsigned int Index);//提取一个英雄的技能CD剩余时间,查询使用

	int GetSkillLevel(unsigned int Index);//提取技能等级
	CStringA GetSkillName(unsigned int Index);//提取技能名
	unsigned int GetSkillPtrByIndex(unsigned int Index);
	unsigned int GetSkillIndexByPtr(unsigned int pSkill);


protected:
	bool LoadSkill( );
	virtual void VSkill(unsigned int IndexVector,float CD);//不同英雄重载之
	vector<stSkill> m_vSkills;//基础技能


};

//////////////////////////////////////////////////////////////////////////


class CLOL_Garen :public ClolHero
{//盖伦
public:	CLOL_Garen(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Mordekaiser :public ClolHero
{//金属大师
public:	CLOL_Mordekaiser(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Nasus :public ClolHero
{//沙漠死神
public:	CLOL_Nasus(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Vayne :public ClolHero
{//暗夜猎手
	public: CLOL_Vayne(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Volibear :public ClolHero
{//雷霆咆哮
public:	CLOL_Volibear(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_MonkeyKing :public ClolHero
{//猴王
public:	CLOL_MonkeyKing(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_TwistedFate :public ClolHero
{//卡牌
public:	CLOL_TwistedFate(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Jax :public ClolHero
{//武器大师
public:	CLOL_Jax(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Ryze :public ClolHero
{//瑞兹
public:	CLOL_Ryze(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Ahri :public ClolHero
{//狐狸
	public: CLOL_Ahri(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Blitzcrank :public ClolHero
{//蒸汽机器人
public:	CLOL_Blitzcrank(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Darius :public ClolHero
{//诺克萨斯之手 
public:	CLOL_Darius(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Fizz :public ClolHero
{//小鱼
public:	CLOL_Fizz(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Shyvana :public ClolHero
{//龙血武姬
public:	CLOL_Shyvana(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Talon :public ClolHero
{//刀锋之影
public:	CLOL_Talon(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_XinZhao :public ClolHero
{
public:	CLOL_XinZhao(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Trundle :public ClolHero
{
public:	CLOL_Trundle(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Zed :public ClolHero
{
public:	CLOL_Zed(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Lucian :public ClolHero
{
public:	CLOL_Lucian(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
//////////////////////////////////////////////////////////////////////////    屏蔽某些技能的
