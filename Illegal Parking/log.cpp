#include "stdafx.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <concrt.h>

using namespace Concurrency;


FILE *Clog::m_file = NULL;
SYSTEMTIME Clog::m_file_time;
critical_section Clog::m_lock;

//
//void Clog::WriteLog2File(const char* format, ...)
//{
//
//}
void Clog::WriteLog(const char* format, ...)
{
    m_lock.lock();
    if (IsNeedCreatLog())
    {
        if (m_file != NULL)
        {
            fclose(m_file);
        }
        char filename[MAX_PATH] = { 0 };
        MakeFileName(filename, MAX_PATH);
        fopen_s(&m_file, filename, "a+");
        if (m_file == NULL)
        {
            printf("open file error:%s\n", filename);
        }
        GetLocalTime(&m_file_time);
    }
    if (m_file == NULL)
    {
        printf("what happend.......\n");
        m_lock.unlock();
        return;
    }

    va_list va;
    char buf[1024] = { 0 };
    char logformat[256] = { 0 };
    sprintf_s(logformat, 256, "[%s:%d][%s]: ", __FUNCTION__, __LINE__,__TIME__);
    //strcat_s(logformat, 100, format);
    va_start(va, format);
    int n = vsnprintf_s(buf, sizeof(buf), format, va);
    va_end(va);           
    strcat_s(buf, 1024, "\r\n");
    fwrite(logformat, 1, strlen(logformat), m_file);
    fwrite(buf, 1, strlen(buf), m_file);
    fflush(m_file);
    m_lock.unlock();
    return;
}


Clog::Clog()
{
    if (m_file == NULL)
    {                 
        m_lock.lock();
        char filename[MAX_PATH] = { 0 };
        MakeFileName(filename, MAX_PATH);
        GetLocalTime(&m_file_time);
        fopen_s(&m_file,filename,"a+");
        if ( m_file == NULL )
        {
            printf("open file error:%s\n", filename);
        }
        m_lock.unlock();
    }
}

Clog::~Clog()
{

}

void Clog::MakeFileName(char *filename,int len)
{                                              
    ::GetCurrentDirectory(256, filename); 
    char tmp[40] = { 0 };
    strcat_s(filename, len, "\\");
    SYSTEMTIME localtime;
    GetLocalTime(&localtime);
    sprintf_s(tmp, 40, "%d-%d-%d.log", localtime.wYear, localtime.wMonth, localtime.wDay);
    strcat_s(filename,len, tmp);
}

bool Clog::IsNeedCreatLog()
{
    SYSTEMTIME localtime;
    GetLocalTime(&localtime);
    if (m_file_time.wDay != localtime.wDay )
   {
        return true;
   }
    return false;
}