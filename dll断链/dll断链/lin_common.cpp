#include "lin_common.h"

using namespace  lin_common;
//
//BOOL IsEqualUnicodeString(const _UNICODE_STRING &uStr1, const _UNICODE_STRING &uStr2)
//{
//	if (uStr1.Length == uStr2.Length) {
//		if (memcmp(uStr1.Buffer, uStr2.Buffer, uStr1.Length) == 0)
//			return TRUE;
//	}
//
//	return FALSE;
//}

BOOL IsEqualUnicodeString(const _UNICODE_STRING &uStr1, const std::wstring &wStr)
{
	if (uStr1.Length == wStr.length()) {
		if (memcmp(uStr1.Buffer, wStr.c_str(), uStr1.Length) == 0)
			return TRUE;
	}

	return FALSE;
}