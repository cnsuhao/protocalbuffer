// CPortDataThread.cpp : 实现文件
//

#include "stdafx.h"
#include "ClientSocketThread.h"

#include "LogFile.h"

#pragma comment(lib, "ws2_32.lib") 
#define MSGSIZE     2048 

IMPLEMENT_DYNCREATE(ClientSocketThread, CWinThread)

ClientSocketThread::ClientSocketThread() :m_pDlg(NULL), m_blStop(TRUE)
{
}

ClientSocketThread::ClientSocketThread(CDialog* pDlg) :m_pDlg(pDlg), m_blStop(TRUE)
{
}

ClientSocketThread::~ClientSocketThread()
{
}

BOOL ClientSocketThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int ClientSocketThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(ClientSocketThread, CWinThread)
END_MESSAGE_MAP()


// CPortDataThread 消息处理程序
// CWarnThread 消息处理程序
void ClientSocketThread::Start()
{
	if (m_blStop)
	{
		m_blStop = FALSE;
		m_bAutoDelete = FALSE;
		CreateThread();
	}
}

void ClientSocketThread::Stop()
{
	m_blStop = TRUE;
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
	
}

int ClientSocketThread::Run()
{
	WSADATA     wsaData;   
	SOCKET      sClient;   
	SOCKADDR_IN client;   
	int         iaddrSize = sizeof(SOCKADDR_IN);   
	// Initialize Windows socket library   
	WORD wVersionRequested;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) { 
		return -1;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) {
			WSACleanup( );
			return -2; 
	}
	bParkOpen;
	// Create listening socket   
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// Bind           
	client.sin_addr.S_un.S_addr = inet_addr("192.168.1.110");
	client.sin_family = AF_INET;
	client.sin_port = htons(8878);
// 	bind(sListen, (struct sockaddr *)&local, iaddrSize);   
// 	listen(sListen,SOMAXCONN);
	for (;;)
	{
		err = connect(sClient, (sockaddr*)&client, sizeof(sockaddr));
		if (err==0)
			break;
		else
			Sleep(5000);
	}
// 	send(sClient, "hello", strlen("hello") + 1, 0);
// 	char buf[MSGSIZE] = { 0 };
// 	recv(sClient, buf, MSGSIZE, 0);
// 	LOG("recv[%s]", buf);

	Factor ff;
	memset(&ff, 0, sizeof(Factor));
	strcpy(ff.strCO, "huangshang");
	strcpy(ff.strO3, "jiangjun");
	send(sClient, (char*)&ff, sizeof(Factor), 0);

	while (m_blStop == FALSE)
	{
		//memset(&ff, 0, sizeof(Factor));
		recv(sClient, (char*)&ff, sizeof(Factor), 0);
		LOG("recv[%s][%s]", ff.strCO, ff.strO3);
		m_pDlg->PostMessage(WM_CFGDATA, (WPARAM)&ff, sizeof(Factor));
	}
	//Sleep(1000);
	WSACleanup();
	return 0;
}
