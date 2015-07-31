#pragma once
#include <atlstr.h>
class Czjlini
{
public:
	Czjlini(CStringA pIniPath);
	~Czjlini(void);

	CStringA GetStr(CStringA AppName,CStringA KeyName);
	bool SetStr(CStringA AppName,CStringA KeyName,CStringA strValue);

	unsigned int GetInt(CStringA AppName,CStringA KeyName);
	bool SetInt(CStringA AppName,CStringA KeyName,unsigned int  IntValue);
private:
	CStringA m_szPathINI;
};

