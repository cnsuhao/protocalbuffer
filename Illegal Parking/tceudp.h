/*
     ʹ��˵����
     ���ͷ�������   TceUdp�� ����SendUdp����
     ���շ��� ���� TceUdp       ���� recvUdp ���ա���
     
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
    void SendUdp(const char* buf/*���͵�����*/, int len/*���ݳ���*/);
    int  recvUdp(char* buf/*���յ�������*/, int len/*buf�ռ�Ĵ�С*/); //���ؽ��յ��ĳ���
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



