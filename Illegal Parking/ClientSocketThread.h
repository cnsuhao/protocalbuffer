#pragma once
#include <vector>

using namespace std;
// ͬ�������ź�
#define WM_CFGDATA WM_USER+141 
struct Factor
{
	char strCO[200], strO3[200];
};
extern bool bParkOpen;//Υͣץ�� ��ʶ
class ClientSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(ClientSocketThread)

public:
	ClientSocketThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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
	vector<CString> m_vecSQL;		//sql���
};


