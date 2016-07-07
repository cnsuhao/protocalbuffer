#include "StdAfx.h"
#include "Lock.h"

CLock::CLock(void)
{
	InitializeCriticalSection(&m_csLock);
}

CLock::~CLock(void)
{
	DeleteCriticalSection(&m_csLock);
}

void CLock::Lock()
{
	EnterCriticalSection(&m_csLock);
}

void CLock::UnLock()
{
	LeaveCriticalSection(&m_csLock);
}