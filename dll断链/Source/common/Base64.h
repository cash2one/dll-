/* ----------------------------------------------------------
�ļ����ƣ�BASE64_API.h

���ߣ��ؽ���

MSN��splashcn@msn.com

��ǰ�汾��V1.1

��ʷ�汾��
	V1.1	2010��05��11��
			����BASE64�����Bug��

	V1.0	2010��05��07��
			�����ʽ�汾��

����������
	BASE64����ͽ���

�ӿں�����
	Base64_Encode
	Base64_Decode

˵����
	1.	�ο�openssl-1.0.0��
	2.	�Ľ��ӿڣ���ʹ����ӦTCHAR�ַ�����
	3.	����EVP_DecodeBlock��������ʱδȥ������ֽڵ�ȱ�ݡ�
 ------------------------------------------------------------ */
#pragma once

#include <windows.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
���ܣ�������������ת����BASE64�����ַ���
����˵����
	inputBuffer��Ҫ����Ķ���������
	inputCount�����ݳ���
	outputBuffer���洢ת�����BASE64�����ַ���
����ֵ��
	 -1����������
	>=0����Ч���볤�ȣ��ַ�����,�������ַ�����������
��ע��
	��Ч��openssl��EVP_EncodeBlock����
*/
INT BASE64_Encode( const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer );

/*
���ܣ���BASE64�����ַ���ת��Ϊ����������
����˵����
	inputBuffer��BASE64�����ַ���
	inputCount�����볤�ȣ��ַ�����,Ӧ��Ϊ4�ı�����
	outputBuffer���洢ת����Ķ���������
����ֵ��
	 -1����������
	 -2�����ݴ���
	>=0��ת������ֽ���
��ע��
	��Ч��openssl��EVP_DecodeBlock����
*/
INT BASE64_Decode( const TCHAR* inputBuffer, INT inputCount, BYTE* outputBuffer );

#ifdef  __cplusplus
}
#endif
