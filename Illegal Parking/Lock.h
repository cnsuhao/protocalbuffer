#pragma once

class CLock
{
public:
	CLock(void);
	~CLock(void);
public:
	void Lock();
	void UnLock();
private:
	CRITICAL_SECTION m_csLock;
};

class ScopeLock
{
public:
	ScopeLock(CLock *pLock)
	{
		m_pLock=pLock;
		if (m_pLock)
		{
			m_pLock->Lock();
		}
	}
	~ScopeLock()
	{
		if (m_pLock)
		{
			m_pLock->UnLock();
		}
	}
	void UnLock()
	{
		if (m_pLock)
		{
			m_pLock->UnLock();
			m_pLock=NULL;
		}
	}
private:
	CLock *m_pLock;
};