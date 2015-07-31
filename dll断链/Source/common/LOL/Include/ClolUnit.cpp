
#include "ClolUnit.h"
#include "LOLOffset.h"
#include <windows.h>
#include <memory.h>
#include <atlstr.h>
#include <iostream>
#include <zjlcommon.h>
using namespace std;

extern int bDebug;
extern bool bAppRun;
extern LOLHookOffset* g_pOffset;


ClolUnit::ClolUnit( int* pUnit )
{
	m_pUnit=(char*)pUnit;
}

ClolUnit::ClolUnit( int pUnit )
{
	m_pUnit=(char*)pUnit;
}


ClolUnit::~ClolUnit(void)
{
}

CStringA ClolUnit::PrintState()
{
	CStringA szOut;
	szOut.Format("#0x%08x -[Type:%x][Team:%d][Index:0x%08x][Name:%s][Hero:%s][Pos:%d,%d] [HP:%d/%d][MP:%d/%d][Level:%d][AttackDamage:%d][AttackRange=%d]",GetPtr(),GetType(),GetTeam(),GetIndex(),GetName(),GetHeroName(),GetX(),GetY(),GetHP(),GetMaxHP(),GetMP(),GetMaxMP(),GetLevel(),GetAttackDamage(),GetAttackRange());
	DbgPrint("PrintState %s",szOut);

	return szOut;
}

unsigned int ClolUnit::GetTeam()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->TEAM,sizeof(int))==0 )
		{
			iResult=*(unsigned int *)(m_pUnit+g_pOffset->TEAM);
		}
	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetTeam  0x%08x",m_pUnit);
		return iResult;
	}

	return iResult;
}

CStringA ClolUnit::GetName()
{
	CStringA  szResult;

		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->PLAYERNAME,0x18)==0 )
		{
			char szHeroName[1024]={0};
			GetGameString((LOLString *)(m_pUnit+g_pOffset->PLAYERNAME),szHeroName);
			szResult=szHeroName;
		}
		szResult=UTF8ToAnsi(szResult);

	return szResult;
}

CStringA ClolUnit::GetHeroName()
{
	CStringA  szResult;
	char szHeroName[1000]={0};


		if (IsBadReadPtr((int*)(m_pUnit+g_pOffset->HERONAME),sizeof(int))==0		)
		{
			int pSkillInfo=*(int*)(m_pUnit+g_pOffset->HERONAME);
			int CharLen=GetGameString((LOLString *)(pSkillInfo+g_pOffset->STRINFO),szHeroName);
		}
		if (szHeroName!="")
		{
			CStringA hName=szHeroName;
			int bpos=hName.Find("_");
			if (bpos!=-1)
			{
				szResult=hName.Left(bpos-1);
			}else{
				szResult=szHeroName;
			}
		}





	return szResult;
}

unsigned int ClolUnit::GetX()
{
	 int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->X,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->X);
			iResult=fResult;
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetX  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult>20000 ||  iResult<0)
	{
		iResult=0;
	}


	return iResult;
}

unsigned int ClolUnit::GetY()
{
	 int iResult=0;
	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->Y,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->Y);
			iResult=fResult;
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetY  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult>20000 ||  iResult<0)
	{
		iResult=0;
	}

	return iResult;
}

unsigned int ClolUnit::GetHP()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->HP,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->HP);
			fResult+=0.99;
			iResult=fResult;
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetHP  0x%08x",m_pUnit);
		return iResult;
	}


	return iResult;
}

unsigned int ClolUnit::GetMaxHP()
{
	unsigned int iResult=1;
	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->MAXHP,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->MAXHP);
			fResult+=0.99;
			iResult=fResult;
		}
	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetMaxHP  0x%08x",m_pUnit);
		return 1;
	}


	if (iResult==0)
	{
		iResult=1;
	}
	return iResult;
} 


unsigned int ClolUnit::GetIndex()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->INDEX,sizeof(int))==0 )
		{
			int fResult=*(int*)(m_pUnit+g_pOffset->INDEX);
			iResult=fResult;
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetIndex  0x%08x",m_pUnit);
		return iResult;
	}


	return iResult;
}

unsigned int ClolUnit::GetPtr()
{
	return (unsigned int)m_pUnit;
}



unsigned int ClolUnit::GetLevel()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->LEVEL,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->LEVEL);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetLevel  0x%08x",m_pUnit);
		return 0;
	}

	return iResult;
}


unsigned int ClolUnit::GetMP()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->MP,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->MP);
			fResult+=0.9;
			iResult=fResult;
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetMP  0x%08x",m_pUnit);
		return iResult;
	}


	return iResult;
}

unsigned int ClolUnit::GetMaxMP()
{
	unsigned int iResult=1;
	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->MAXMP,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->MAXMP);
			fResult+=0.9;
			iResult=fResult;
		}
	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetMaxMP  0x%08x",m_pUnit);
		return 1;
	}
	if (iResult==0)
	{
		iResult= 1;
	}
	return iResult;
	
}

unsigned int ClolUnit::GetAttackDamage()
{
	unsigned int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->DAMAGE,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->DAMAGE);
			iResult+=(fResult+0.99);
		}


		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->DAMAGE_WEAPON,sizeof(float))==0 )
		{
			float fResult=*(float*)(m_pUnit+g_pOffset->DAMAGE_WEAPON);
			iResult+=(fResult+0.99);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetAttackDamage  0x%08x",m_pUnit);
		return iResult;
	}




	return iResult;
}

bool ClolUnit::IsGoodPtr()
{
	if (IsBadReadPtr(m_pUnit,g_pOffset->LEVEL))
	{
		return 0;
	}

	return 1;
}



float ClolUnit::GetfX()
{
	float fResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->X,sizeof(float))==0 )
		{
			 fResult=*(float*)(m_pUnit+g_pOffset->X);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfX  0x%08x",m_pUnit);
		return 0;
	}

	if (fResult>20000 ||  fResult<0)
	{
		fResult=0;
	}


	return fResult;
}

float ClolUnit::GetfY()
{
	float fResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->Y,sizeof(float))==0 )
		{
			fResult=*(float*)(m_pUnit+g_pOffset->Y);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	if (fResult>20000 ||  fResult<0)
	{
		fResult=0;
	}


	return fResult;
}

float ClolUnit::GetfZ()
{
	float fResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->Z,sizeof(float))==0 )
		{
			fResult=*(float*)(m_pUnit+g_pOffset->Z);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfZ  0x%08x",m_pUnit);
		return 0;
	}

	if (fResult>20000 ||  fResult<0)
	{
		fResult=0;
	}


	return fResult;
}


stLOLPos ClolUnit::GetPos()
{
	stLOLPos PosResult={0};

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->X,sizeof(float))==0 )
		{
			PosResult=*(stLOLPos*)(m_pUnit+g_pOffset->X);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetPos  0x%08x",m_pUnit);
		return PosResult;
	}



	return PosResult;
}

bool ClolUnit::IsHero()
{
	int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->UNITTYPE,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->UNITTYPE);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult==0x1401)
	{
		return 1;
	}else{
		return 0;
	}
}

bool ClolUnit::IsUnit()
{
	int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->UNITTYPE,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->UNITTYPE);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult==0xC01)
	{

		return 1;
	}else{
		return 0;
	}
}

bool ClolUnit::IsCreep()
{
	int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->UNITTYPE,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->UNITTYPE);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult==0xC01)
	{
		return 1;
	}else{
		return 0;
	}
}

unsigned int ClolUnit::MathHowLong( int _TargetUnit )
{
	ClolUnit TargetUnit(_TargetUnit);
	unsigned int X1=GetX()	;
	unsigned int Y1=GetY()	;

	unsigned int X2=TargetUnit.GetX();
	unsigned int Y2=TargetUnit.GetY();

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

	//DbgPrint("Player1 x=%d Y=%d   Player2 x=%d,y=%d  ,¾àÀë%d",X1,Y1,X2,Y2,ResultF);
	return (unsigned int)ResultF;
}

unsigned int ClolUnit::GetAttackRange()
{
	float fResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->AttackRange,sizeof(float))==0 )
		{
			fResult=*(float*)(m_pUnit+g_pOffset->AttackRange);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::AttackRange  0x%08x",m_pUnit);
		return 0;
	}

	if (fResult>2000)
	{
		return 0;
	}


	return fResult;
}

unsigned int ClolUnit::GetType()
{
	int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->UNITTYPE,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->UNITTYPE);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	return iResult;
}

bool ClolUnit::IsBulid()
{
	int iResult=0;

	__try
	{
		if (m_pUnit && IsBadReadPtr(m_pUnit+g_pOffset->UNITTYPE,sizeof(int))==0 )
		{
			iResult=*(int*)(m_pUnit+g_pOffset->UNITTYPE);
		}

	}
	__except(1)
	{
		DbgPrint("__except Error : ClolUnit::GetfY  0x%08x",m_pUnit);
		return 0;
	}

	if (iResult==0x2401)
	{

		return 1;
	}else{
		return 0;
	}
}

