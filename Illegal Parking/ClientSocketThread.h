#pragma once
#include <vector>

using namespace std;
// 同步配置信号
#define WM_CFGDATA WM_USER+141 
struct Factor
{
	char strCO[200], strO3[200];
};
extern bool bParkOpen;//违停抓拍 标识
class ClientSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(ClientSocketThread)

public:
	ClientSocketThread();           // 动态创建所使用的受保护的构造函数
	ClientSocketThread(CDialog* pDlg);
	virtual ~ClientSocketThread();

	void Start();
	void Stop();
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	BOOL m_blStop;
	CDialog* m_pDlg;
protected:
	DECLARE_MESSAGE_MAP()

private:
	vector<CString> m_vecSQL;		//sql语句
};


