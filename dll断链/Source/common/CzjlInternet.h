#pragma once
#include <atlstr.h>
#include <wininet.h>
class CzjlInternet
{
public:
    CzjlInternet(void);
    ~CzjlInternet(void);
	
	bool Start();
   static unsigned int LoadFile(CStringW strUrl,CStringW strSavePath);
   static unsigned int LoadFile(CStringW strUrl,CStringW strSavePath,unsigned int BufferSize);
   static unsigned int LoadHtml( CStringW strUrl,CStringW& sBuffer );
   static unsigned int LoadHtml( CStringA strUrl,CStringA& sBuffer );
   static unsigned int LoadHtmlLen( CStringA strUrl,CStringA& sBuffer,unsigned int ReadByte);
   static unsigned int QueryFileSize(CStringW strUrl);

   static unsigned int PostHttps(CStringW strUrl);

private:
};

