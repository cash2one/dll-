//////////////////////////////////////////////////////////////////////
//
// Written by Michael Dunn (mdunn at inreach dot com).  Copyright and all
// that stuff.  Use however you like but give me credit where it's due.  I'll
// know if you don't. bwa ha ha ha ha!
// 
// Release history:
//   December 24, 1999: Version 1.0.  First release.
//
//////////////////////////////////////////////////////////////////////

// BindStatusCallback.cpp: implementation of the CBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindStatusCallback.h"
#include <shlwapi.h>                    // for StrFormatByteSize()
#include <atlstr.h>
#include <commctrl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCallback::CCallback() : bStop(0)
{
	m_pCallBackFunc=0;
}

CCallback::~CCallback()
{
}

HRESULT CCallback::OnProgress( ULONG ulProgress,   ULONG ulProgressMax,
                                ULONG ulStatusCode, LPCWSTR wszStatusText )
{
// Local variables are declared static so they don't have to be reallocated on
// the stack every time.  This is safe in this app since I know I'll only have
// one thread downloading.
static CString sIEStatusMsg;
static TCHAR   szCustomStatusMsg [256];
static TCHAR   szAmtDownloaded [256], szTotalSize [256];

    UNREFERENCED_PARAMETER(ulStatusCode);

    // Did the user hit the Stop button?
    if (  bStop )
        return E_ABORT;

      if ( 0 != ulProgressMax )
	 {
	   if (m_pCallBackFunc	)
	   {
		   m_pCallBackFunc(strFileName,ulProgressMax,ulProgress,m_nFlag);
	   }
	//  Sleep(100);
	 }
    return S_OK;
}
