#include "ParserEvent.h"



ParserGameMsg g_ParserMsg;

MsgPraseData add20150609()
{
	MsgPraseData add;
	add.szVersion="5.10.1.330";
	//打野
	add.MSG_BUFF=0x2B;
	add.OffsetBuff_int=0x0C;//Offset int
	add.Buff_Flag11=0xE3D0572E;
	add.Buff_Flag12=0xE3D08F74;
	add.Buff_Flag21=0xE3D04F3A;
	add.Buff_Flag22=0xE3D0B76A;
	add.Buff_Flag31=0xE3D06F46;//大龙
	add.Buff_Flag32=0xE3D0B741;//小龙
	//死亡
	add.MSG_DEAD=0x52;
	add.OffsetDead_int=0x06;//Dead
	//水晶
	add.MSG_TOWER=0xF2;
	add.OffsetTower_Byte=0x06;
	add.Tower_Flag11=0x3e;
	add.Tower_Flag12=0xf1;
	add.Tower_Flag13=0xe1;
	add.Tower_Flag21=0xd0;
	add.Tower_Flag22=0x1f;
	add.Tower_Flag23=0x0f;
	//大乱斗
	add.MSG_BLOOD=0x108;
	add.OffsetBlood_int=0x11;
	add.Blood_Flag1=0x57252525;
	add.Blood_Flag2=0xdb252525;
	add.Blood_Flag3=0xe5252525;
	add.Blood_Flag4=0x55252525;

	return add;
}



MsgPraseData add20150526()
{
	MsgPraseData add;
	add.szVersion="5.9.1.325";
	//打野
	add.MSG_BUFF=0x11b;
	add.OffsetBuff_int=0xF;//Offset int
	add.Buff_Flag11=0x16A0E12C;
	add.Buff_Flag12=0x16CB7C80;
	add.Buff_Flag21=0x163D8929;
	add.Buff_Flag22=0x160F36AE;
	add.Buff_Flag31=0x1670633B;//大龙
	add.Buff_Flag32=0x160F7AD2;//小龙
	//死亡
	add.MSG_DEAD=0x9A;
	add.OffsetDead_int=0x06;//Dead
	//水晶
	add.MSG_TOWER=0x10e;
	add.OffsetTower_Byte=0x06;
	add.Tower_Flag11=0x3e;
	add.Tower_Flag12=0xf1;
	add.Tower_Flag13=0xe1;
	add.Tower_Flag21=0xd0;
	add.Tower_Flag22=0x1f;
	add.Tower_Flag23=0x0f;
	//大乱斗
	add.MSG_BLOOD=add.MSG_BUFF;
	add.OffsetBlood_int=0x08;
	add.Blood_Flag1=0xD0940000;
	add.Blood_Flag2=0xD0F50000;
	add.Blood_Flag3=0xD0650000;
	add.Blood_Flag4=0xD0AE0000;

	return add;
}

MsgPraseData add20150512()
{
	MsgPraseData add;
	add.szVersion="5.8.1.463";
	//打野
	add.MSG_BUFF=0xB7;
	add.OffsetBuff_int=0xC;//Offset int
	add.Buff_Flag11=0x1303048E;
	add.Buff_Flag12=0x1373048E;
	add.Buff_Flag21=0x13B3048E;
	add.Buff_Flag22=0x1353048E;
	add.Buff_Flag31=0x13f3048E;//大龙
	add.Buff_Flag32=0x1343048E;//小龙
	//死亡
	add.MSG_DEAD=0xD8;
	add.OffsetDead_int=0x06;//Dead
	//水晶
	add.MSG_TOWER=0x13;
	add.OffsetTower_Byte=0x06;
	add.Tower_Flag11=0x3e;
	add.Tower_Flag12=0xf1;
	add.Tower_Flag13=0xe1;
	add.Tower_Flag21=0xd0;
	add.Tower_Flag22=0x1f;
	add.Tower_Flag23=0x0f;
	//大乱斗
	add.MSG_BLOOD=add.MSG_BUFF;
	add.OffsetBlood_int=0x0C;
	add.Blood_Flag1=0x1373048E;
	add.Blood_Flag2=0x1333048E;
	add.Blood_Flag3=0x1303048E;
	add.Blood_Flag4=0x1323048E;

	return add;
}

MsgPraseData add20150506()
{
	MsgPraseData add;
	add.szVersion="5.8.1.448";
	//打野
	add.MSG_BUFF=0xB7;
	add.OffsetBuff_int=0xC;//Offset int
	add.Buff_Flag11=0x1303048E;
	add.Buff_Flag12=0x1373048E;
	add.Buff_Flag21=0x13B3048E;
	add.Buff_Flag22=0x1353048E;
	add.Buff_Flag31=0x13f3048E;//大龙
	add.Buff_Flag32=0x1343048E;//小龙
	//死亡
	add.MSG_DEAD=0xD8;
	add.OffsetDead_int=0x06;//Dead
	//水晶
	add.MSG_TOWER=0x13;
	add.OffsetTower_Byte=0x06;
	add.Tower_Flag11=0x3e;
	add.Tower_Flag12=0xf1;
	add.Tower_Flag13=0xe1;
	add.Tower_Flag21=0xd0;
	add.Tower_Flag22=0x1f;
	add.Tower_Flag23=0x0f;
	//大乱斗
	add.MSG_BLOOD=add.MSG_BUFF;
	add.OffsetBlood_int=0x0C;
	add.Blood_Flag1=0x1373048E;
	add.Blood_Flag2=0x1333048E;
	add.Blood_Flag3=0x1303048E;
	add.Blood_Flag4=0x1323048E;

	return add;
}


ParserGameMsg::ParserGameMsg()
{
	addOffsets(add20150506());
	addOffsets(add20150512());
	addOffsets(add20150526());
	addOffsets(add20150609());
}


extern CLOLHelper g_Helper;
void ParserActionMessage_DLD( unsigned int A1,unsigned int A2 )
{//大乱斗处理过程
	MsgPraseData Offset=g_ParserMsg.GetOffsets();
	if (Offset.szVersion=="")
	return;
	int ActionFlag=*((short*)(A2+0x4));
	if ( ActionFlag==Offset.MSG_BLOOD)
	{
		DbgPrint("大乱斗-血包 \r\n %s ",PrintBytes("",(char*)(A2),0x30));
		unsigned int intFlag=*(( unsigned int*)(A2+Offset.OffsetBlood_int));
		DbgPrint("Offset=%x, intFlag=%08x",Offset.OffsetBlood_int,intFlag);

/*
		unsigned  int bCreate=*(( unsigned char*)(A2+0x12));
		if (bCreate==0x89)
		{
			DbgPrint("bCreate = %d",bCreate);
			return;
		}*/



		if (intFlag==Offset.Blood_Flag1)
		{
			g_Helper.Event(MiniMapEvent::BLOODBUFFER1,0,40); 
		}else 	if (intFlag==Offset.Blood_Flag2)
		{
			g_Helper.Event(MiniMapEvent::BLOODBUFFER2,0,40); 
		}else 	if (intFlag==Offset.Blood_Flag3)
		{
			g_Helper.Event(MiniMapEvent::BLOODBUFFER3,0,40); 
		}else 	if (intFlag==Offset.Blood_Flag4)
		{
			g_Helper.Event(MiniMapEvent::BLOODBUFFER4,0,40); 
		}


	}

	if ( ActionFlag==Offset.MSG_DEAD )
	{//死亡
		DbgPrint("大乱斗-死亡 \r\n %s ",PrintBytes("",(char*)(A2),0x30));
		int PlayerIndexFlag=*((int*)(A2+Offset.OffsetDead_int));
		DbgPrint("Offset=%x, intFlag=%08x",Offset.OffsetDead_int,PlayerIndexFlag);

		ClolUnit* pUnit=g_Helper.GetPlayerByFlag(PlayerIndexFlag);
		if (pUnit)
		{
			float	ftimeadd=((pUnit->GetLevel()-3)*2)+10;
			g_Helper.Event(MiniMapEvent::HERODEAD,PlayerIndexFlag,ftimeadd);
		}
		return;
	}
}
void ParserActionMessage_DEF( unsigned int A1,unsigned int A2 )
{//普通地图处理
	MsgPraseData Offset=g_ParserMsg.GetOffsets();
	if (Offset.szVersion=="")
	return;

	int ActionFlag=*((short*)(A2+4));		

	if ( ActionFlag==Offset.MSG_DEAD )
	{//死亡
		DbgPrint("死亡 \r\n %s ",PrintBytes("",(char*)(A2),0x30));
		int PlayerIndexFlag=*((int*)(A2+Offset.OffsetDead_int));
		DbgPrint("Offset=%x, intFlag=%08x",Offset.OffsetDead_int,PlayerIndexFlag);

		ClolUnit* pUnit=g_Helper.GetPlayerByFlag(PlayerIndexFlag);
		if (pUnit)
		{
			float	ftimeadd=(pUnit->GetLevel()*2.5)+8.5;
			g_Helper.Event(MiniMapEvent::HERODEAD,PlayerIndexFlag,ftimeadd);
		}

		return;
	}



	if ( ActionFlag==Offset.MSG_BUFF)
	{//打野
		DbgPrint("打野 \r\n %s ",PrintBytes("",(char*)(A2),0x30));

		unsigned int intFlag=*((unsigned int*)(A2+Offset.OffsetBuff_int));
		DbgPrint("Offset=%x, intFlag=%08x",Offset.OffsetBuff_int,intFlag);

		if (intFlag==Offset.Buff_Flag11)
		{
			g_Helper.Event(MiniMapEvent::CREEPS11,0,300);
		}else if (intFlag==Offset.Buff_Flag12)
		{
			g_Helper.Event(MiniMapEvent::CREEPS12,0,300);
		}else if (intFlag==Offset.Buff_Flag21)
		{
			g_Helper.Event(MiniMapEvent::CREEPS21,0,300);
		}else if (intFlag==Offset.Buff_Flag22)
		{
			g_Helper.Event(MiniMapEvent::CREEPS22,0,300);
		}else if (intFlag==Offset.Buff_Flag31)
		{
			g_Helper.Event(MiniMapEvent::BOSS1,0,420);
		}else if (intFlag==Offset.Buff_Flag32)
		{
			g_Helper.Event(MiniMapEvent::BOSS2,0,360);
		}
	}

	if (ActionFlag==Offset.MSG_TOWER )
	{//水晶
		//0x12C 0x94
		unsigned int byteFlag=*((unsigned char*)(A2+Offset.OffsetTower_Byte));
		DbgPrint("水晶 \r\n %s ",PrintBytes("",(char*)(A2),0x30));
		DbgPrint("Offset=%x, intFlag=%08x",Offset.OffsetTower_Byte,byteFlag);
		if (byteFlag==Offset.Tower_Flag11)
		{
			g_Helper.Event(MiniMapEvent::TOWER11,0,300);
		}else if (byteFlag==Offset.Tower_Flag12)
		{
			g_Helper.Event(MiniMapEvent::TOWER12,0,300);
		}else if (byteFlag==Offset.Tower_Flag13)
		{
			g_Helper.Event(MiniMapEvent::TOWER13,0,300);
		}else if (byteFlag==Offset.Tower_Flag21)
		{
			g_Helper.Event(MiniMapEvent::TOWER21,0,300);
		}else if (byteFlag==Offset.Tower_Flag22)
		{
			g_Helper.Event(MiniMapEvent::TOWER22,0,300);
		}else if (byteFlag==Offset.Tower_Flag23)
		{
			g_Helper.Event(MiniMapEvent::TOWER23,0,300);
		}
	}
	
}
