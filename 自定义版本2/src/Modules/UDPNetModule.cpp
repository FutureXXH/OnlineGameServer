#include "../../include/UDPNetModule.h"

//=========================================================



UDPClient *UDPNetModule::GetUdpClient(int ID)
{
    if (ClientList.count(ID))
    {
        return ClientList[ID];
    }

    return nullptr;
}


 long long UDPClient::GetID(const sockaddr_in &clientAddr)
{
 return   clientAddr.sin_addr.s_addr*100000+clientAddr.sin_port;
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

                //判断数据是否接收完整
                if (Head + 8 + msg->dataSize > Tail)
                {

                    Head--;
                    __ModuleManager->PushMessageObj(msg);
                    break;
                }

                if (msg->MessageID = 101)
                {
                    //建立连接
                    msg->writeData(buf + Head + 8, msg->dataSize);
                    Connect(msg, clientAddr);
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
                    //写入数据给消息体
                     msg->writeData(client->UDPClientID);
                     msg->writeData(buf + Head + 8, msg->dataSize);
                    //将数据放入模块管理器的消息队列
                    __ModuleManager->pushDataMessageQueue(msg);
                   
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
    for (int i = 0; i < RecvThreadNum; i++)
    {
        thread *newRecvThreadp = new thread(&UDPNetModule::RunRecvThread, this);
        RecvThreads.emplace_back(newRecvThreadp);
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
        auto newclientp = ClientObjPool.pop();
        if (newclientp == nullptr)
        {
            Log(ERROR, "创建udp客户端失败");
        }
        newclientp->Reset();
        newclientp->clientAddr = clientAddr;
        newclientp->UDPClientID = UDPClient::GetID(clientAddr);

        //第二次握手
    }
    break;

    default:
        break;
    }
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