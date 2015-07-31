#pragma once
#include <windows.h>
#include <Dbghelp.h>
#include <Psapi.h>
#include <string>
#include <iostream>
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Psapi.lib")
//δ�ĵ���
typedef struct _IMAGE_DELAY_IMPORT_DESCRIPTOR
{
	DWORD grAttrs;
	DWORD DLLName;
	DWORD Hmod;
	DWORD IAT;
	DWORD INT;
	DWORD BoundIAT;
	DWORD UnloadIAT;
	DWORD TimeDateStamp;
}IMAGE_DELAY_IMPORT_DESCRIPTOR, *PIMAGE_DELAY_IMPORT_DESCRIPTOR;
//�޸�IAT���Եľ���ʵ��
struct CIatPolicyImpl
{
	static void Replace(LPSTR dllName, LPVOID oldFuncBase, LPVOID newFuncBase)
	{
		HMODULE hMods[1024] = {0};   
		DWORD cbNeeded;   
		HANDLE hProcess = ::GetCurrentProcess();   

		if( ::EnumProcessModules( hProcess, hMods, sizeof(hMods), &cbNeeded))   
		{   
			for ( UINT i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )   
				ReplaceIATEntryInImageImportTable( hMods[i], dllName, oldFuncBase, newFuncBase);   
		}   
	}
	static BOOL ReplaceIATEntryInImageImportTable(HANDLE hBaseAddress, LPCSTR lpszDllName, LPVOID pfnCurrent, LPVOID pfnNew)   
	{   
		// ��ȡ IMAGE_IMPORT_DESCRIPTOR   
		DWORD dwSize = 0;   
		PIMAGE_SECTION_HEADER pFoundHeader = NULL;   
		PIMAGE_IMPORT_DESCRIPTOR pImgImportDescriptor=(PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToDataEx(hBaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &dwSize, &pFoundHeader);   
		if( pImgImportDescriptor == NULL )
			return FALSE;
		while (pImgImportDescriptor->Name)   
		{   
			if ( _strcmpi((CHAR*)((PBYTE)hBaseAddress+pImgImportDescriptor->Name), lpszDllName) == 0 )   
			{   
				break; // �ҵ�   
			}   
			++pImgImportDescriptor;   
		}   
		// ������Ҫ�ر�ע��!!!!   
		// �����IMAGE_DIRECTORY_ENTRY_IMPORT�Ҳ������Ⳣ����IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT����   
		// mshtml.dll����ʹ�����ӳټ��صġ������������ҹ����ϡ�   
		if( !pImgImportDescriptor->Name )   
			return ReplaceIATEntryInDelayImageImportTable( hBaseAddress, lpszDllName, pfnCurrent, pfnNew);   
		// ��ȡ IAT   
		PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)(((LPBYTE)hBaseAddress) + pImgImportDescriptor->FirstThunk);   
		// ѭ��IAT����   
		while(pThunk->u1.Function)     
		{     
			PDWORD lpAddr = (PDWORD)&(pThunk->u1.Function);     
			if(*lpAddr == (DWORD)pfnCurrent)     
			{   
				// �ҵ����޸ĵ�ַΪת�Ӻ���   
				::WriteProcessMemory(::GetCurrentProcess(), lpAddr, &pfnNew, sizeof(DWORD), NULL);   
				return TRUE;   
			}      
			pThunk++;     
		}   
		return FALSE;   
	}
	static BOOL ReplaceIATEntryInDelayImageImportTable(HANDLE hBaseAddress, LPCSTR lpszDllName, LPVOID pfnCurrent, LPVOID pfnNew)   
	{   
		DWORD dwSize = 0;   
		PIMAGE_SECTION_HEADER pFoundHeader = NULL;   
		PIMAGE_DELAY_IMPORT_DESCRIPTOR pImgDelayDescr=(PIMAGE_DELAY_IMPORT_DESCRIPTOR)ImageDirectoryEntryToDataEx(hBaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, &dwSize, &pFoundHeader);   
		if( pImgDelayDescr == NULL )
		{ return FALSE; }   
		while (pImgDelayDescr->DLLName)   
		{   
			if ( _strcmpi((CHAR*)((PBYTE)hBaseAddress+pImgDelayDescr->DLLName), lpszDllName) == 0 )   
			{   
				break;   
			}   
			++pImgDelayDescr;   
		}   
		// �Ҳ�����ģ��   
		if( !pImgDelayDescr->DLLName )   
			return FALSE;   
		// ��ȡ IAT   
		PIMAGE_THUNK_DATA pThunk = NULL;   
		//��֪���Ƿ���ȷ
		if( (pImgDelayDescr->grAttrs & 0x1) == 0 )   
			return FALSE;   
		pThunk = (PIMAGE_THUNK_DATA)(((LPBYTE)hBaseAddress) + pImgDelayDescr->IAT);   
		// ѭ��IAT����   
		while(pThunk->u1.Function)     
		{     
			PDWORD lpAddr = (PDWORD)&(pThunk->u1.Function);     
			if(*lpAddr == (DWORD)pfnCurrent)     
			{   
				// �滻   
				::WriteProcessMemory(::GetCurrentProcess(), lpAddr, &pfnNew, sizeof(DWORD), NULL);   
				return TRUE;   
			}      
			pThunk++;     
		}   
		return FALSE;   
	}  
};
//�޸�IAT�Ĳ���
template <typename FT>
class CIatPolicy
{
protected:
	void Replace(LPSTR dllName, FT oldFuncBase, FT newFuncBase)
	{
		return CIatPolicyImpl::Replace(dllName, oldFuncBase, newFuncBase);
	}
};
//���ڲ������Hook API��
template <typename FuncType,
	template<typename > class Policy=CIatPolicy >
class CHookApi : public Policy<FuncType>
{
public:
	CHookApi() : m_IsHook(false), m_old(NULL), m_new(NULL) {}
	~CHookApi() 
	{
		Recover();
	}
	void Hook(LPSTR dllName, FuncType oldFuncBase, FuncType newFuncBase)
	{
		if ( m_IsHook )
			return;
		m_IsHook=true;
		m_dll=dllName;
		m_old=oldFuncBase;
		m_new=newFuncBase;
		Policy<FuncType>::Replace(dllName, oldFuncBase, newFuncBase);
	}
	void Recover()
	{
		if ( !m_IsHook )
			return;
		m_IsHook=false;
		Policy<FuncType>::Replace((LPSTR)m_dll.c_str(), m_new, m_old); 
	}
	const FuncType Addr() const { return m_old; }
private:
	bool m_IsHook;
	std::string m_dll;
	FuncType m_old;
	FuncType m_new;
};
/*
//MessageBox�ĺ���ָ������
typedef int (WINAPI* MyMessageBox)(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType);
//�����͵�ַ�õ�
MyMessageBox p=NULL;
//�Լ���MessageBox����
int WINAPI MyMessageBoxA(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType)
{
	std::cout<<lpText<<std::endl;
	if ( !p )
		return 0;
	return p(hWnd, lpText, lpCaption, uType);
}*/