#ifndef __UDPNETMODULE_H
#define __UDPNETMODULE_H
#include "IDefine.h"
#include "ModuleManager.h"
#include "ConsoleCmd.h"



#include <iostream>
#include <unordered_map>
#include <mutex>
#include <cstring>
#include <thread>
#include <string>
//===========linux库=============
#define _atoi64(val)     strtoll(val, NULL, 10)  
#include <cstdio>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
using Socket = int;

//===============================
//
//   这是网络模块 包含了UDP网络
//
//==============================

using namespace std;

#define BUFFSIZE 512 //buff大小
#define MaxEvent 1024 // 最大事件容量
#define MaxClient 1024 //最大连接的客户端

#define DISCONNECT  0
#define CONNECTING  1
#define CONNECTED  2
#define DISCONNECTING 3

class UDPNetModule;

class UDPClient
{
  public:
    char State = DISCONNECT;
    long long UDPClientID ;
    sockaddr_in clientAddr;
    int HeartTime;

    Socket ServerSocket;

    inline void Reset()
    {
      State = DISCONNECT;
      HeartTime= time(NULL);
      UDPClientID = -1;
      memset(&clientAddr,0,sizeof(sockaddr_in));
    }

    int Send(int head,uint8* buff,int size);

    static long long GetID(const sockaddr_in &clientAddr) ;
   
};



class UDPNetModule : public ModuleBase
{
private:
 int RecvThreadNum = 2;
  thread* HeartThreadP;
  vector<thread*> RecvThreads;
  unordered_map<long long,UDPClient*> ClientList;
  
   ThreadSafe_Queue<UDPClient*> ClientObjPool;
  //开启网络接收线程
  void RunRecvThread();
  //开启心跳检测线程
  void RunHeartCheckThread();

  
  //接收数据 
  void RecvData();


  //连接  模拟tcp三次握手
  void Connect(Message* meg, sockaddr_in clientAddr);
  //关闭连接
  void CloseConnect(long long ClientID);


public:
     Socket ServerSocket;


	//一直循环处理
	virtual void update() override;
	//初始化函数 开启服务器时调用
	virtual void Init() override;
	//解析消息
	virtual void parseMessage(Message* messagePtr) override;
	//模块退出执行
	virtual void Exit() override;



  UDPClient* GetUdpClient(long long ID);



};


#endif