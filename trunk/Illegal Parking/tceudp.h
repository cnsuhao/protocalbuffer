/*
     使用说明：
     发送方：构造   TceUdp， 调用SendUdp即可
     接收方： 构造 TceUdp       调用 recvUdp 接收。。
     
*/

#include <winsock2.h>
#include <iostream>
#include <stdio.h>
using namespace std;
//oid initClient();

class  TceUdp
{
public:
     TceUdp();
    ~ TceUdp();
public:    
    void SendUdp(const char* buf/*发送的数据*/, int len/*数据长度*/);
    int  recvUdp(char* buf/*接收到的数据*/, int len/*buf空间的大小*/); //返回接收到的长度
private:
    SOCKET _ClientSoc;
    SOCKET _ServerSoc;
    bool _IsOk;
    bool _ClientOk;
    bool _ServerOk;
private:
    void InitNet();
    void InitClient();
    void InitServer();
    void CleanUp();

};



