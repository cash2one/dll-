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
	bool bDisable;		//������ļ���,��Ҫ���ε�

	unsigned int Index;//��SkillArray�����
	unsigned int pSkill;//����ָ��
	CStringA szSkillName;//������
	float CD;					//���һ��ʹ�õ�CD

	unsigned int UsedCount;	//����ʹ�ü���
	unsigned int TimeLastUsed;//���һ��ʹ��ʱ��
	unsigned int CurrentCDms;//��ǰCD,����

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


class ClolHero : public  ClolUnit//һ��Ӣ��
{
public:
	ClolHero(int pUnit);
	~ClolHero(void);

	 bool ParserSkill(unsigned int PointSkill,float CD);//����������Ϣ,�ص�ʹ��,����1����ɹ�
	float GetCD(unsigned int Index);//��ȡһ��Ӣ�۵ļ���CDʣ��ʱ��,��ѯʹ��

	int GetSkillLevel(unsigned int Index);//��ȡ���ܵȼ�
	CStringA GetSkillName(unsigned int Index);//��ȡ������
	unsigned int GetSkillPtrByIndex(unsigned int Index);
	unsigned int GetSkillIndexByPtr(unsigned int pSkill);


protected:
	bool LoadSkill( );
	virtual void VSkill(unsigned int IndexVector,float CD);//��ͬӢ������֮
	vector<stSkill> m_vSkills;//��������


};

//////////////////////////////////////////////////////////////////////////


class CLOL_Garen :public ClolHero
{//����
public:	CLOL_Garen(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Mordekaiser :public ClolHero
{//������ʦ
public:	CLOL_Mordekaiser(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Nasus :public ClolHero
{//ɳĮ����
public:	CLOL_Nasus(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Vayne :public ClolHero
{//��ҹ����
	public: CLOL_Vayne(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Volibear :public ClolHero
{//��������
public:	CLOL_Volibear(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_MonkeyKing :public ClolHero
{//����
public:	CLOL_MonkeyKing(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_TwistedFate :public ClolHero
{//����
public:	CLOL_TwistedFate(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Jax :public ClolHero
{//������ʦ
public:	CLOL_Jax(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Ryze :public ClolHero
{//����
public:	CLOL_Ryze(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Ahri :public ClolHero
{//����
	public: CLOL_Ahri(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Blitzcrank :public ClolHero
{//����������
public:	CLOL_Blitzcrank(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Darius :public ClolHero
{//ŵ����˹֮�� 
public:	CLOL_Darius(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Fizz :public ClolHero
{//С��
public:	CLOL_Fizz(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Shyvana :public ClolHero
{//��Ѫ�伧
public:	CLOL_Shyvana(int pUnit):ClolHero(pUnit){};
	virtual void VSkill(unsigned int IndexVector,float CD);
};
class CLOL_Talon :public ClolHero
{//����֮Ӱ
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
//////////////////////////////////////////////////////////////////////////    ����ĳЩ���ܵ�
