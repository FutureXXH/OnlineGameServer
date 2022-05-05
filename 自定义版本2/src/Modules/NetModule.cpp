#include "../../include/NetModule.h"

//============================IOThread=========================================


void IOThread::ThreadRun(NetModule* np)
{
    Log(INFO,"IO线程开启 " + to_string(ID) );

   NetPtr = np;
   sockaddr_in clientAddr;
   socklen_t clen = sizeof(sockaddr);
   while (true)
   {
         int count = epoll_wait(NetPtr->epollfd,epollEvents,MaxEvent,-1);
         if(count <= 0)continue;
         for (int i = 0; i < count; i++)
         {
             
              if(epollEvents[i].data.fd == NetPtr->ServerSocket)
              {
                    // 有新连接
                    Socket acceptsocket = accept(NetPtr->ServerSocket,(sockaddr*)&clientAddr,&clen);
                    if(acceptsocket < 0)
                    {
                        //处理连接错误
                        continue;
                    }
                    
                    TCPClient* clientObj = NetPtr->GetNullClientObj();
                    if(clientObj == nullptr)
                    {
                        cout << "达到最大连接数量" << endl;
                        continue;
                    }


                    epoll_event tempEvent;
                    tempEvent.data.fd = acceptsocket;
                    tempEvent.events = EPOLLIN|EPOLLET;//边缘触发
                    epoll_ctl(NetPtr->epollfd,EPOLL_CTL_ADD,acceptsocket,&tempEvent);



                    string ip(inet_ntoa(clientAddr.sin_addr));

                    memcpy(clientObj->ip,ip.c_str(),ip.length());
                    clientObj->port = clientAddr.sin_port;
                    clientObj->clientSocket = acceptsocket;
                    clientObj->Epollfd = NetPtr->epollfd;
                    NetPtr->insert_ClientObj_Map(clientObj);

                    Log(INFO,"有新连接" + ip + "  sock:" + str(acceptsocket));


              }
              else if(epollEvents[i].events&EPOLLIN)
              {

                    TCPClient* clientObj = NetPtr->GetClientObj(epollEvents[i].data.fd);
                    if(clientObj == nullptr)continue;

                    int recvSize = recv(clientObj->clientSocket,clientObj->RecvBuff+clientObj->Recv_Tail,buffsize - clientObj->Recv_Tail,0);

                    
                    if(recvSize > 0)
                    {
                        clientObj->Recv_Tail += recvSize;
                        while (true)
                        {
                                //判断头
                                while(clientObj->RecvBuff[clientObj->Recv_Head] != '&')
                                {
                                    clientObj->Recv_Head += 1;
                                }
                                //判断头是否完整
                                if(clientObj->Recv_Tail - clientObj->Recv_Head < 9)break;
                                clientObj->Recv_Head += 1;

                                    Log(INFO,"收到数据大小" + str(recvSize)  + " "+ str(clientObj->Recv_Head) +"|" + str(clientObj->Recv_Tail) + "  "
                                    + str(NetPtr->OnlineTcpClients.size()) + "  " + str(NetPtr->ClientObjPool.size()));
                                    //获取一个消息对象
                                    auto msg = __ModuleManager->GetMessageObj();
                                    
                                    if(msg == nullptr)
                                    {
                                        //cout << "在处理数据时消息指针为空" << endl;
                                        continue;
                                    }

                                    //封装消息
                                    msg->srcModuleID = NetPtr->ID;
                                    memcpy(&msg->MessageID,clientObj->RecvBuff+clientObj->Recv_Head,4);
                                    memcpy(&msg->dataSize,clientObj->RecvBuff+clientObj->Recv_Head+4,4);
                                    if(clientObj->Recv_Head + 8 + msg->dataSize > clientObj->Recv_Tail)
                                    {
                                        clientObj->Recv_Head--;
                                        break;
                                    }
                                    memcpy(&msg->data,clientObj->RecvBuff+clientObj->Recv_Head+8,msg->dataSize);

                                    //将数据放入模块管理器的消息队列
                                    __ModuleManager->pushDataMessageQueue(msg);
                                    clientObj->Recv_Head += 8 + msg->dataSize;
                        }
                        if(clientObj->Recv_Head == clientObj->Recv_Tail)
                        {
                            clientObj->Recv_Head = 0;
                            clientObj->Recv_Tail = 0;
                        }
                        




                  }
                  else if(recvSize == 0)
                  {
                    Log(INFO, "连接断开" +    str(epollEvents[i].data.fd) + "  " +  str(NetPtr->OnlineTcpClients.size())+ "  " +  str(NetPtr->ClientObjPool.size()));
                    NetPtr->CloseSocketData(epollEvents[i].data.fd);


                  }
                  else{
                      Log(WARNING, "接收错误:" + str(recvSize));
        
                  }




                  
              }



         }
         
   }
   

}







//============================NetModule=========================================
bool NetModule::SetNonblockingSocket(Socket socket)
{
	int f = fcntl(socket, F_GETFL);
	if (f < 0)return false;
	f |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, f) < 0)return false;
	return true;
}

 bool NetModule::TcpServerInit(int port)
 {
      ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    SetNonblockingSocket(ServerSocket);

    sockaddr_in ServerAddr;
	memset(&ServerAddr, 0, sizeof(sockaddr_in));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
	  
    int err = bind(ServerSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (err < 0)return false;
     err = listen(ServerSocket, SOMAXCONN);
	if (err < 0)return false;
     return true;
 }

bool NetModule::EpollInit()
{
    
    epollfd = epoll_create(1024);
    epoll_event ev;
    ev.data.fd = ServerSocket;
    ev.events = EPOLLIN|EPOLLET;//边缘触发
    int err = epoll_ctl(epollfd,EPOLL_CTL_ADD,ServerSocket,&ev);
    return true;

}

bool NetModule::EpollThreadInit(int ThreadNum)
{
   if(ThreadNum <= 0)return false;
   for (int i = 1; i <= ThreadNum; i++)
   {
       IOThread *IOThreadptr = new IOThread();
       IOThreadptr->ID = i;
       IOThreadptr->ThreadPtr = new thread(&IOThread::ThreadRun,IOThreadptr,this);
       IOThreadptr->ThreadPtr->detach();
   }
   return true;
}


bool NetModule::insert_ClientObj_Map(TCPClient* obj)
{
     OnlineTcpClients.emplace(obj->clientSocket,obj);
     return true;
}


bool NetModule::delelte_ClientObj_Map(Socket sock)
{
    if(OnlineTcpClients.count(sock) == 0)return false;
      OnlineTcpClients.erase(sock);
     return true;
}
TCPClient* NetModule::GetClientObj(Socket sock)
{
    if(OnlineTcpClients.count(sock) == 0)return nullptr;
     return OnlineTcpClients[sock];
}


TCPClient* NetModule::GetNullClientObj()
{
    if(ClientObjPool.size() == 0)return nullptr;
    auto p = ClientObjPool.pop();
    if(p == nullptr)return nullptr;
    p->Reset();
    return p;
}



bool NetModule::PushClientObj(TCPClient* obj)
{
    ClientObjPool.push(obj);
    return true;
}


bool NetModule::CloseSocketData(Socket sock)
{
        unique_lock<mutex> lock(NetMutex);
        auto clientObj = GetClientObj(sock);
        if(clientObj == nullptr)return false;
        close(sock);
        epoll_ctl(epollfd,EPOLL_CTL_DEL,sock,NULL);
        delelte_ClientObj_Map(clientObj->clientSocket);
        PushClientObj(clientObj);
       return true;
}













 void NetModule::Init()
 {
    Log(INFO,"初始化模块"+ str(ID));
     
     for (int i = 0; i < MaxClient; i++)
     {
         ClientObjPool.push(new TCPClient());
     }
     


     TcpServerInit(5678);
     EpollInit();
     EpollThreadInit(5);
    Log(INFO,"网络模块初始化完成"+ str(ID));

 }

 void NetModule::parseMessage(Message* messagePtr)
{
	
	

}

void NetModule::update()
{
	
}

void NetModule::Exit() 
{

}
