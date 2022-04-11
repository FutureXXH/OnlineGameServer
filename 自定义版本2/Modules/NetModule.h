#ifndef __NETMODULE_H
#define __NETMODULE_H
#include "../IDefine.h"
#include "../ModuleManager.h"
#include "../MyTool.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
//========================
#define _atoi64(val)     strtoll(val, NULL, 10)  
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
using Socket = int;

//===============================
//
//   这是网络模块 包含了TCP网络，Epoll以及IO线程
//
//==============================

using namespace std;
class ModuleBase;
class NetModule;
#define buffsize 1024 //buff大小
#define MaxEvent 1024 // 最大事件容量
#define MaxClient 1024 //最大连接的客户端

class TCPClient
{
public:
 char ip[20];
 int port;
 sockaddr_in clientAddr;
 Socket clientSocket;
 int Epollfd=-1;
 char RecvBuff[buffsize];
 int Recv_Head;
 int Recv_Tail;
bool Reset()
{
  memset(RecvBuff,0,1024);
  Recv_Head = 0;
  Recv_Tail = 0;
  
  return true;
}


 void Send(int head,char* buff,int size);


};


class IOThread
{
NetModule* NetPtr;
epoll_event epollEvents[MaxEvent];


public:
  int ID = -1;
  thread * ThreadPtr;
  void ThreadRun(NetModule* np);


};


class NetModule:public ModuleBase
{
private:
   mutex NetMutex;
   vector<IOThread*> IOThreadPool;
   unordered_map<Socket ,TCPClient*> OnlineTcpClients;
   ThreadSafe_Queue<TCPClient*> ClientObjPool;
   
   friend class IOThread;
public:
    
     Socket ServerSocket;
    int epollfd;
	virtual void update() override;
	virtual void Init() override;
	virtual void parseMessage(Message* messagePtr) override;


   static bool SetNonblockingSocket(Socket socket);

   bool TcpServerInit(int port);
   bool EpollInit();
   bool EpollThreadInit(int ThreadNum);

   bool insert_ClientObj_Map(TCPClient* obj);

   bool delelte_ClientObj_Map(Socket sock);
   TCPClient*GetClientObj(Socket sock);

   TCPClient* GetNullClientObj();
   bool PushClientObj(TCPClient* obj);

   bool CloseSocketData(Socket sock);



};

#endif