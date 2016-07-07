#include "stdafx.h"
#include "tceudp.h"
#pragma comment(lib,"ws2_32.lib")


TceUdp::TceUdp() :_IsOk(true), _ClientSoc(-1), _ServerSoc(-1), _ClientOk(false), _ServerOk(false)
{
    InitNet();
}

TceUdp::~TceUdp()
{
    CleanUp();
}
void TceUdp::InitClient()
{
    _ClientSoc = socket(AF_INET, SOCK_DGRAM, 0);
    _ClientOk = true;
}
void TceUdp::InitServer()
{
    _ServerSoc = socket(AF_INET, SOCK_DGRAM, 0);

    SOCKADDR_IN sockSrc;
    sockSrc.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    sockSrc.sin_port = htons(10662);
    sockSrc.sin_family = AF_INET;

    int ret = bind(_ServerSoc, (SOCKADDR *)&sockSrc, sizeof(SOCKADDR));
    if (ret < 0)
    {
        printf("bind error...%d", GetLastError());
        _IsOk = false;
    }
    _ServerOk = true;
}
void TceUdp::CleanUp()
{
    if ( _ClientSoc != -1 )
    {
        closesocket(_ClientSoc);
    }
    if (_ServerSoc != -1 )
    {
        closesocket(_ServerSoc);
    }                                  
    WSACleanup();
}
void TceUdp::InitNet()
{

    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(1, 1), &wsaData);

    if (error != 0)
    {
        cout << "初始化DLL失败" << endl;
        return;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        cout << "版本出错" << endl;
        return;
    }
    
            
}



void TceUdp::SendUdp(const char* buf, int len)
{
    if ( !_ClientOk )
    {
        InitClient();
    }
    SOCKADDR_IN sockSend;
    sockSend.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockSend.sin_port = htons(10662);
    sockSend.sin_family = AF_INET;                     
    ::sendto(_ClientSoc, buf, len, 0, (sockaddr *)&sockSend, sizeof(sockaddr));
}
int  TceUdp::recvUdp(char* buf, int len)
{                                  
    if ( !_ServerOk )
    {
        InitServer();
    }
    if ( !_IsOk )
    {
        return -1;
    }
    int ret  =  recvfrom(_ServerSoc, buf, len, 0, NULL, NULL);
    if ( ret <=0 )
    {
        printf("recv error..%d", GetLastError());
        return -1;
    }
    return ret;

}