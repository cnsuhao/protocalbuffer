#ifndef __LOG_H
#define __LOG_H

#include "stdafx.h"
#include <stdio.h>
#include <atlstr.h>

#include <concrt.h>

using namespace Concurrency;

                                 
class  Clog
{
public:
     Clog();
    ~ Clog();

public:
    void WriteLog(const char* format, ...);
private:
   static FILE *m_file;
   static SYSTEMTIME m_file_time;
private:
    void MakeFileName(char *filename,int len);
    bool IsNeedCreatLog();
    static critical_section m_lock;
    //void WriteLog2File(const char* format, ...);

};













#endif