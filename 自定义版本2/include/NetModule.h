#ifndef __NETMODULE_H
#define __NETMODULE_H
#include "IDefine.h"
#include "ModuleManager.h"
#include "ConsoleCmd.h"



#include <iostream>
#include <unordered_map>
#include <mutex>
#include <cstring>
//===========linux库=============
#define _atoi64(val)     strtoll(val, NULL, 10)  
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
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
	virtual void Exit() override;
   //设置非阻塞socket
   static bool SetNonblockingSocket(Socket socket);
   //TCP初始化
   bool TcpServerInit(int port);
   //Epoll初始化
   bool EpollInit();
   //IO线程初始化
   bool EpollThreadInit(int ThreadNum);
   //插入一个客户端对象
   bool insert_ClientObj_Map(TCPClient* obj);
   //删除一个客户端对象
   bool delelte_ClientObj_Map(Socket sock);
   //查找获取一个客户端对象
   TCPClient*GetClientObj(Socket sock);
   //获取一个空客户端对象
   TCPClient* GetNullClientObj();
   //放入一个对象进对象池
   bool PushClientObj(TCPClient* obj);
   //关闭一个客户端对象以及socket
   bool CloseSocketData(Socket sock);



};

#endif