/*******************************************************************************
 *
 * �ļ����ƣ�log_file.h
 * �ļ���ʶ��
 * ժ����Ҫ����־ģ��
 * �������ߣ�����
 * ������ڣ�2013��7��9��
 *
 * ˵    ������ģ��Ϊ������
 *
 *           �ڴ�ӡǰ��Ҫ����InitLog��ʼ���������Ϊ
 *              type��       ��־������ͣ�Ĭ�������������
 *              level��      ��־�������Ĭ��������м���
 *              reserve_day����־���������Ĭ��10��
 *              size��       ��־����С����λΪM��Ĭ��1024M
 *              directory��  ��־���Ŀ¼��Ĭ��Ϊ��ǰģ��*\\log
 *              name��       ��־������ƣ�Ĭ��Ϊ��ǰģ������
 *              �����ò���Ϊ0��������ó�Ĭ��ֵ
 *              ���ӣ�
 *                   InitLog(0, 0, 0, 0, NULL, NULL);
 *
 *           GenericLogΪ��־���
 *              ���ӣ�
 *                   ��ͨ��Ϣ��ӡ��GenericLog(LogInfo, "%s", "ok"); ��ӡ���Ϊ��ok
 *                   ������Ϣ��ӡ��GenericLog("trace", LogInfo, "%s", "ok");��ӡ���Ϊ��[trace] ok
 *           �ڳ������ʱ����UninitLogж��
 *
 * ��    ע�����ļ�Ϊ��дģʽ��ֻ��ӵ��ģ�����������־��
 *           ����м��ص�DLL��Ҫ�ڱ�ģ���ϴ�ӡ��־����Ҫ�ṩ��ӡ��־�ص���
 *           ����Ҫ������ӡ��־���򵥶�����name
 ******************************************************************************/
#pragma once

#ifndef __US_LOG_FILE_H_
#define __US_LOG_FILE_H_

#include <windows.h>
#include <memory>
#include <functional>


//////////////////////////////////////////////////////////////////////////

enum LogOutputLevel         // ��־�������
{
    LogStartup  = 1,		// ������Ϣ������ģ�������
    LogShutdown = 2,		// �ر���Ϣ������ģ��Ĺر�
    LogInfo     = 4,		// ��ͨ��Ϣ����̬��ӡ����������/ֹͣ��Ϣ...
    LogWarning  = 8,		// ������Ϣ���������о���״̬
    LogTrace    = 16,		// ������Ϣ��ҵ����̸���
    LogError    = 32,		// ������Ϣ����Ӱ��������еĴ���
    LogDebug    = 64,		// ������Ϣ��ֻ����Debug�汾
    LogFatal    = 128       // ������Ϣ�����³����޷��������еĴ���
};

enum LogOutputType          // ��־�������
{
    LogScreen = 1,			// ��Ļ���
    LogVSDebugWindow = 2,	// VC��DebugWindow���
    LogDiskFile = 4,		// �ļ����
};

// ��־ģ����ⳣ�ýӿ�
#define InitLog         us::LogFile::Instance().Init
#define UninitLog       us::LogFile::Instance().Uninit
#define GenericLog      us::LogFile::Instance().OutputLog
#define LOG				us::LogFile::Instance().OutputLog
#define ClrLogContent	us::LogFile::Instance().ClearLogContent

//////////////////////////////////////////////////////////////////////////
// ��־ʵ��

namespace us
{


class LogFile
{
    struct impl;
    std::unique_ptr<impl> impl_;

public:
    typedef std::function<std::string (const std::string &)> encrypt_func_t;
    LogFile(void);
    ~LogFile(void);

    static LogFile& Instance()
    {
        static LogFile obj;
        return obj;
    }

    /*
    * ��    �ܣ���־��ʼ��
    * ��    ����type��       ��־������ͣ�Ĭ�������������
    *           level��      ��־�������Ĭ��������м���
    *           reserve_day����־���������Ĭ��10��
    *           size��       ��־����С����λΪM��Ĭ��1024M
    *           directory��  ��־���Ŀ¼��Ĭ��Ϊ��ǰģ��*\\log
    *           name��       ��־������ƣ�Ĭ��Ϊ��ǰģ������
    * �� �� ֵ������ɹ��򷵻�true�����򷵻�false
    * ��    ע��������ò���Ϊ0��������ó�Ĭ��ֵ
    */
    bool Init(int type, int level, int reserve_day, int size, const char* directory, const char* name, bool is_encrypt=false);

    void SetEncryptHandle(encrypt_func_t);

    /*
    * ��    �ܣ���־ж�أ��ͷ��ڲ���Դ
    * ��    ����
    * �� �� ֵ��
    */
    void Uninit();

    // �����������
    void SetOuputType(int type);
    // �����������
    void SetOuputLevel(int level);
	//��յ�ǰ��־�ļ�����
	void ClearLogContent();
    // ������־��������
    void SetReserveDay(int reserve_day);

    void OutputLog(int level, const char* format, ...);
    void OutputLog(int level, const wchar_t* format, ...);

    void OutputLog(const char* trace, int level, const char* format, ...);
    void OutputLog(const wchar_t* trace, int level, const wchar_t* format, ...);

    void OutputArgLog(int level, const char* format, va_list argptr);
    void OutputArgLog(int level, const wchar_t* format, va_list argptr);

    void OutputArgLog(const char* trace, int level, const char* format, va_list argptr);
    void OutputArgLog(const wchar_t* trace, int level, const wchar_t* format, va_list argptr);

private:
    LogFile(const LogFile&);
    LogFile& operator=(const LogFile&);
};


//////////////////////////////////////////////////////////////////////////


// ģ����־
class ModuleLog
{
public:
    ModuleLog(const char* module_name);
    ~ModuleLog(void);

    void TraceLog(int level, const char* format, ...) const;
    void TraceLog(int level, const wchar_t* format, ...) const;

private:
    ModuleLog(void) {}
    ModuleLog(const ModuleLog&);
    ModuleLog& operator=(const ModuleLog&);

    char module_name_[260];
};


}// namespace us

#endif // __US_LOG_FILE_H_
