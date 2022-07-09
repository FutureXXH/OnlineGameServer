#include "../../include/UDPNetModule.h"

//=========================================================
uint8 Sendbuffer[BUFFSIZE];

int UDPClient::Send(int head,uint8* buff,int size)
{

    Sendbuffer[0] = '&';
    memcpy(Sendbuffer+1,(uint8*)&head,4);
    memcpy(Sendbuffer+5,(uint8*)&size,4);
    memcpy(Sendbuffer+9,buff,size);
    
    int sendDataByte = sendto(ServerSocket,Sendbuffer,size+9,0,(sockaddr*)&clientAddr,sizeof(sockaddr_in));


}



 long long UDPClient::GetID(const sockaddr_in &clientAddr)
{
 return   clientAddr.sin_addr.s_addr*100000+clientAddr.sin_port;
}

//=================================================================
UDPClient *UDPNetModule::GetUdpClient(long long ID)
{
    if (ClientList.count(ID))
    {
        return ClientList[ID];
    }

    return nullptr;
}



void UDPNetModule::RunRecvThread()
{
    Log(INFO, "接收数据线程开启");

    char buf[BUFFSIZE];
    memset(buf, 0, BUFFSIZE);
    int Head = 0;
    int Tail = 0;
    sockaddr_in clientAddr;
   int addrlen = sizeof(clientAddr);
    while (true)
    {

        
        int recvByte = recvfrom(ServerSocket, buf + Tail, BUFFSIZE - Tail, 0, (sockaddr *)&clientAddr, (socklen_t *)&addrlen);

        if (recvByte > 0)
        {
           cout <<"recv!" << recvByte <<endl;
          
           
              
            Tail += recvByte;
            while (Head < Tail)
            {
                
                //寻找头
                while (Head < Tail && buf[Head] != '&')
                {
                    Head++;
                }
                
                //判断是否完整
                if (Tail - Head < 9)
                    break;

                    
                Head++;
                auto msg = __ModuleManager->GetMessageObj();

                if (msg == nullptr)
                {
                    continue;
                }

                //封装消息
                msg->srcModuleID = ID;
                memcpy(&msg->MessageID, buf + Head, 4);
                memcpy(&msg->dataSize, buf + Head + 4, 4);
                

               // cout << msg->MessageID <<"|" << msg->dataSize   << endl;
               
                //判断数据是否接收完整 缺少直接当丢失处理
                if (Head + 8 + msg->dataSize > Tail)
                {

                    Head += 8 + msg->dataSize;
                    __ModuleManager->PushMessageObj(msg);
                    break;
                }

                if (msg->MessageID == 101)
                {
                    
                    //建立连接
                    msg->writeData(buf + Head + 8, msg->dataSize);
                    Connect(msg, clientAddr);
                    __ModuleManager->PushMessageObj(msg);
                }
                else if(msg->MessageID == 102)
                {
                      //处理心跳包
                       auto client = GetUdpClient(UDPClient::GetID(clientAddr));
                        msg->writeData(buf + Head + 8, msg->dataSize);
                    if (client == nullptr||client->State != CONNECTED)
                    {
                   
                    }
                    else{
                        client->HeartTime = time(NULL);
                        client->Send(102,(uint8*)&msg->data,12);
                    }
                      __ModuleManager->PushMessageObj(msg);

                }
                else
                {
                    //判断是否是个合法连接
                    auto client = GetUdpClient(UDPClient::GetID(clientAddr));
                    if (client == nullptr||client->State != CONNECTED)
                    {
                     __ModuleManager->PushMessageObj(msg);
                    }
                    else
                    {
                    //写入数据给消息体
                     msg->writeData(client->UDPClientID);
                     msg->writeData(buf + Head + 8, msg->dataSize);
                    //将数据放入模块管理器的消息队列
                    __ModuleManager->pushDataMessageQueue(msg);
                    }
                }
                 Head += 8 + msg->dataSize;
            }
        }
        else
        {
            Log(WARNING, "接收数据异常" + str(recvByte));
        }

        if (Head >= Tail)
        {
            Head = 0;
            Tail = 0;
        }
    }
}

//======================================================

  void UDPNetModule::update()
  {

  }

  void UDPNetModule::Exit()
  {

  }

void UDPNetModule::Init()
{
    Log(INFO, "UDP网络模块初始化");

    for (int i = 1; i <= MaxClient; i++)
    {
        auto p = new UDPClient();
        p->UDPClientID = i;

        ClientObjPool.push(p);
    }

    //发送数据消息注册
    RegisterMessageID(100);



    ServerSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(ServerSocket < 0)
    {
        Log(ERROR,"服务器初始化socket失败");
        return;
    }
    
    sockaddr_in addserver;
    addserver.sin_family = AF_INET;
	addserver.sin_addr.s_addr = htonl(INADDR_ANY);
	addserver.sin_port = htons(5678);
	int error = bind(ServerSocket, (sockaddr*)&(addserver), sizeof(addserver));
	if (error < 0)
	{
		Log(ERROR,"服务器bind失败");
		return;
	}

    Log(INFO,"UDP初始化成功");

     HeartThreadP= new thread(&UDPNetModule::RunHeartCheckThread, this);

    for (int i = 0; i < RecvThreadNum; i++)
    {
        thread *newRecvThreadp = new thread(&UDPNetModule::RunRecvThread, this);
        RecvThreads.emplace_back(newRecvThreadp);
    }
    
}


  void UDPNetModule::RunHeartCheckThread()
  {
    Log(INFO,"正在初始化UDP心跳包检测线程");
    while (true)
    {
            cout << ClientObjPool.size() << "|" << ClientList.size() <<endl;
               for (auto it = ClientList.begin(); it != ClientList.end(); it++)
                {
                    
                    auto cp = it->second;
                    if(cp == nullptr)continue;
                    switch (cp->State)
                    {
                    case CONNECTING:
                        {
                            if(time(NULL) - cp->HeartTime < 3)break;
                            cp->State = DISCONNECTING;
                            cp->HeartTime = time(NULL);
                        }
                        break;
                    case CONNECTED:
                        {
                            if(time(NULL) - cp->HeartTime < 10)break;
                            cp->State = DISCONNECTING;
                            cp->HeartTime = time(NULL);
                        }
                        break;    
                    case DISCONNECTING:
                        {
                         
                            if(time(NULL) - cp->HeartTime < 3)break;
                     
                            CloseConnect(cp->UDPClientID);
                            
                           
                        }
                        break;    
                    default:
                        break;
                    }
                }
                this_thread::sleep_for(chrono::seconds(1));
    }
    
   
    
  }



void UDPNetModule::Connect(Message *msg, sockaddr_in clientAddr)
{
    int CMD = -1;
    msg->readData(CMD);
    switch (CMD)
    {

    case 1:
    {
        UDPClient * newclientp = nullptr;
        int ID = -1;
        msg->readData(ID);
        if(ClientList.count(UDPClient::GetID(clientAddr)) > 0)
        {
            cout << "Exist Connect Client" << endl;
            
             newclientp = ClientList[UDPClient::GetID(clientAddr)];
             if(newclientp->State == CONNECTED)break;
             
        }
        else
        {
            cout << "Create Connect Client" << endl;
            newclientp = ClientObjPool.pop();
            if (newclientp == nullptr)
            {
                Log(ERROR, "创建udp客户端失败");
            }
            newclientp->Reset();
            newclientp->clientAddr = clientAddr;
            newclientp->UDPClientID = UDPClient::GetID(clientAddr);
            newclientp->ServerSocket = ServerSocket;
            newclientp->State = CONNECTING;
            ClientList.emplace(newclientp->UDPClientID,newclientp);
        }
         if(newclientp == nullptr)break;

        
        //第二次握手
        uint8 sendbuff[12];
        memcpy(sendbuff,(uint8*)&newclientp->UDPClientID,8);
         memcpy(sendbuff+8,(uint8*)&ID,4);
        newclientp->Send(101,sendbuff,12);
    }
    break;
    case 2:
    {
  
        //第三次握手建立连接
        long long ClientID = -1;
        msg->readData(ClientID);
        ClientID--;
        auto clientp = GetUdpClient(ClientID);
        if(clientp == nullptr)return ;
        cout << "Connected Client " << clientp->UDPClientID << endl;
        clientp->State = CONNECTED;
    }
    break;

    default:
        break;
    }
}




void  UDPNetModule::CloseConnect(long long ClientID)
{
 
    auto cp = GetUdpClient(ClientID);
    if(cp == nullptr)return ;
      cout << "close client  " << ClientID << endl; 
    ClientList.erase(ClientID);
    cp->Reset();
    ClientObjPool.push(cp);
}

void UDPNetModule::parseMessage(Message *messagePtr)
{
    if (messagePtr == nullptr)
        return;
    switch (messagePtr->MessageID)
    {
    case 101:
    {
        
    }
    break;

    default:
        break;
    }
}