
#include "Czjlini.h"




Czjlini::Czjlini( CStringA pIniPath )
{
	m_szPathINI=pIniPath;
}


Czjlini::~Czjlini(void)
{
}

CStringA Czjlini::GetStr( CStringA AppName,CStringA KeyName )
{
	CStringA sz;
	GetPrivateProfileStringA(AppName,KeyName,"",sz.GetBuffer(1024),MAX_PATH,m_szPathINI);
	sz.ReleaseBuffer();
	return sz;
}

bool Czjlini::SetStr( CStringA AppName,CStringA KeyName,CStringA strValue )
{
	return WritePrivateProfileStringA(AppName,KeyName,strValue,m_szPathINI);
}

unsigned int Czjlini::GetInt( CStringA AppName,CStringA KeyName )
{
	CStringA sz;
	GetPrivateProfileStringA(AppName,KeyName,"0",sz.GetBuffer(1024),MAX_PATH,m_szPathINI);
	sz.ReleaseBuffer();
	if (!sz.IsEmpty())
	{
		unsigned int Result=atoi(sz);
		return Result;
	}else{
		return 0;
	}
}

bool Czjlini::SetInt( CStringA AppName,CStringA KeyName,unsigned int IntValue )
{
	CStringA sz;
	sz.Format("%d",IntValue);
	return WritePrivateProfileStringA(AppName,KeyName,sz,m_szPathINI);
}
