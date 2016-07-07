#pragma once
#include "afxmt.h"
#include "Lock.h"
#define LOG CLogFile::Log

class CLogFile : public CFile
{
public:
	CLogFile();
	~CLogFile();		
public:	
	static void Log(const TCHAR *lpszFmt, ...);
	static void GetAppName(CString &sPath,HMODULE hInstance=NULL);
	static BOOL FileExist(const char *sFileName);
	static BOOL MyCreateDirectory(const char *strPathName);
	static BOOL DeleteDirectory(const char *sDirName);
	static void RemoveHisFile(const char *pDelPath);
private:
	CString m_sAppPath; 
	CLock m_csLock;
};