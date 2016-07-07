#include "stdafx.h"
#include "LogFile.h"
#include <io.h>
#include <direct.h>
#include <sys/stat.h>

CLogFile g_LogFile;

CLogFile::CLogFile()
{
	GetAppName(m_sAppPath);
	m_sAppPath+="\\log";
	MyCreateDirectory(m_sAppPath);
}

CLogFile::~CLogFile()
{

}

void CLogFile::Log(const TCHAR *lpszFmt, ...)
{	
	//Format the message
	va_list arglist;
	va_start(arglist,lpszFmt);
	TCHAR szMessage[2048] = {0};
	TCHAR szBuf[2048] = {0};
	::wvsprintf(szMessage,lpszFmt,arglist);
	va_end(arglist);

	//Get Local Time
	time_t tm;
	struct tm *now;
	TCHAR szTimeBuf[20] = {0};
	time(&tm);
	now=localtime(&tm);
	_tcsftime(szTimeBuf,100,_T("%H:%M:%S"),now); //"%Y-%m-%d %H:%M:%S"
	_sntprintf(szBuf,sizeof(szBuf)/sizeof(TCHAR),_T("[%s] %s\r\n"),szTimeBuf, szMessage);

	TCHAR szLogFile[100] = {0};
	_tcsftime(szLogFile,100,_T("%Y-%m-%d.log"),now);

	TCHAR szTemp[MAX_PATH];
	memset(szTemp, 0, MAX_PATH);
	_sntprintf(szTemp,sizeof(szTemp)/sizeof(TCHAR),_T("%s/%s") ,g_LogFile.m_sAppPath, szLogFile);

	ScopeLock pLock(&g_LogFile.m_csLock);
	if( !g_LogFile.Open(szTemp, CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
		return;

	g_LogFile.SeekToEnd();
	g_LogFile.Write(szBuf, (UINT)_tcslen( szBuf ));
	g_LogFile.Close();
}

void CLogFile::GetAppName(CString &sPath,HMODULE hInstance)
{
	GetModuleFileName(hInstance, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos = sPath.ReverseFind('\\');
	if (nPos != -1)
		sPath = sPath.Left(nPos);
}

BOOL CLogFile::FileExist(const char *sFileName)
{
	return ((_access(sFileName, 0)) != -1);
}

BOOL CLogFile::MyCreateDirectory(const char *strPathName)
{
	if (!PathFileExists(strPathName))//文件夹不存在则创建by hb at 2016.03.18
		return SHCreateDirectoryEx(NULL, strPathName, NULL) == ERROR_SUCCESS;

	return TRUE;
}

BOOL CLogFile::DeleteDirectory(const char *sDirName)
{
	CFileFind finder;
	CString sFiles=sDirName;
	sFiles+="*.*";
	BOOL bWorking = finder.FindFile(sFiles);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		CString sFileName=sDirName+finder.GetFileName();
		if (finder.IsDirectory())
			DeleteDirectory(sFileName);
		else
			DeleteFile(sFileName);
	}
	finder.Close();

	SetFileAttributes(sDirName,FILE_ATTRIBUTE_NORMAL);
	if(!RemoveDirectory(sDirName))
		return FALSE;

	return TRUE;
}   

void CLogFile::RemoveHisFile(const char *pDelPath)
{
	time_t now;
	char searchpath[MAX_PATH+40] = { 0 }, fullfilename[256] = {0};

	strcpy(searchpath, pDelPath);
	strcat(searchpath, "\\*");	

	long hFile; 
	_finddata_t fileinfo; 	
	ZeroMemory(&fileinfo, sizeof(_finddata_t));
	hFile= _findfirst(searchpath, &fileinfo);
	if (hFile == -1)
		return;

	do 
	{
		if (fileinfo.attrib == 32)	//file
		{
			if ( strstr(fileinfo.name,".log") )
			{
				sprintf(fullfilename, "%s\\%s", pDelPath, fileinfo.name);
				now = time(0);
				if (labs(now - fileinfo.time_create) > 60*60*24*10)	//固定删除超过10天的日志 by hb at 2015.04.03
					remove(fullfilename);
			}
		}
	} 
	while ((_findnext(hFile, &fileinfo)!=-1));

	if( hFile != -1)
		_findclose(hFile);
}

