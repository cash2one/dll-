#pragma once
#include <LOL\Include\LOLHelper.h>
#include <vector>
#include <zjlcommon.h>





void ParserActionMessage_DLD( unsigned int A1,unsigned int A2 );
void ParserActionMessage_DEF( unsigned int A1,unsigned int A2 );



struct MsgPraseData{//处理消息
	CStringA szVersion;

	int MSG_BUFF;
		int OffsetBuff_int;//Offset int
		int Buff_Flag11;
		int Buff_Flag12;
		int Buff_Flag21;
		int Buff_Flag22;
		int Buff_Flag31;//大龙
		int Buff_Flag32;

	int MSG_BLOOD;
		int OffsetBlood_int;
		int Blood_Flag1;
		int Blood_Flag2;
		int Blood_Flag3;
		int Blood_Flag4;

	int MSG_DEAD;
		int OffsetDead_int;//Dead

	int MSG_TOWER;
		int OffsetTower_Byte;
		int Tower_Flag11;
		int Tower_Flag12;
		int Tower_Flag13;
		int Tower_Flag21;
		int Tower_Flag22;
		int Tower_Flag23;
};


class ParserGameMsg
{
public:
	ParserGameMsg();
	~ParserGameMsg(){};
	void addOffsets(MsgPraseData add){m_vOffsets.push_back(add);};
	MsgPraseData GetOffsets()
	{
		if (m_szVersion=="")
		{
			m_szVersion=GetFileVersion("League of Legends.exe");
		}
		for (unsigned int i=0;i<m_vOffsets.size();i++)
		{
			if (m_vOffsets[i].szVersion==m_szVersion)
			{
				return m_vOffsets[i];
			}
		}
		MsgPraseData add;
		add.szVersion="";
		return add;
	};
private:
	CStringA m_szVersion;
	std::vector<MsgPraseData> m_vOffsets;
};


