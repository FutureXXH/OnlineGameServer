#include "TCPServer.h"


TcpServer* __TcpServer = nullptr;

bool TcpServer::StartTcpServer(int port)
{

    SERVERPRINT_INFO << "正在开启服务器" << std::endl;
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        SERVERPRINT_ERROR << "Winsock初始化失败" << std::endl;
        return false;
    }
    ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


    memset(&Server_adr, 0, sizeof(Server_adr));
    Server_adr.sin_family = AF_INET;
    Server_adr.sin_addr.S_un.S_addr = INADDR_ANY;
    Server_adr.sin_port = htons(port);

    if (bind(ServerSocket, (struct sockaddr*)&Server_adr, sizeof(Server_adr)) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "bind error" << std::endl;
        return false;
    }
    if (listen(ServerSocket, 20) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "listen error" << std::endl;
        return false;
    }
    SERVERPRINT_INFO << "服务器网络初始化完成" << std::endl;

    //设置服务器sock非阻塞模式
    unsigned long u = 1;
    if (ioctlsocket(ServerSocket, FIONBIO, &u) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "设置服务器非阻塞套接字失败!" << std::endl;
        return false;
    }


    SERVERPRINT_INFO << "正在预开辟TcpClient内存:" << maxConnect*sizeof(TcpClient) << "字节,TCP最大连接数" << maxConnect<< "个" << endl;


    TcpClientLinkers = new HashArray<TcpClient>(maxConnect);
    TcpClientIndexLinkers = new HashArray<TcpClientIndex>(maxSocket);




    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);







    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    
    threadnum = 2;

    for (int i = 0; i < threadnum; i++)
    {
        thread* threadp = new thread(&TcpServer::IOThread, this, CompletionPort);

    }

    SERVERPRINT_INFO << "创建" << threadnum << "个IO线程" << endl;



    CreateIoCompletionPort((HANDLE)ServerSocket, CompletionPort, (DWORD)ServerSocket, 0);//把服务器监听sock加入进监听里



    //传递accept监听事件

    LPPER_IO_OPERATION_DATA PerIoData;
    PerIoData = new PER_IO_OPERATION_DATA();
    ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
    PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
    PerIoData->DataBuf.len = DATA_BUFSIZE;
    PerIoData->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    PerIoData->Type = 1;
    DWORD dwRecv = 0;

    if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
    {
        int error = WSAGetLastError();
        if (error != ERROR_IO_PENDING)
        {
            SERVERPRINT_ERROR << "投递服务器sock失败!" << error << endl;

            return false;
        }
    }


    SERVERPRINT_INFO << "服务器成功开启" << endl;
    
    return true;

}

void TcpServer::IOThread(LPVOID lpParam)
{
    SERVERPRINT_INFO << "IO线程正在运行" << endl;
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    SOCKET csocket;
    LPPER_IO_OPERATION_DATA PerIoData;

    while(true)
    {
        if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&csocket, (LPOVERLAPPED*)&PerIoData, 10000))
        {
            if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
            {
                //SERVERPRINT_INFO << "等待超时" << endl;
                //IOCPSERVER::deleteclient(*(ClientMap->at(csocket)));
            }
            else
            {
                SERVERPRINT_WARNING << "完成端口失败 可能是因为相关连接已经断开" << endl;
            }
            continue;

        }

        if (PerIoData->Type == 1)
        {
            //处理新连接
            if (csocket == ServerSocket)
            {

                SOCKADDR_IN* LocalAddr = nullptr;
                SOCKADDR_IN* ClientAddr = nullptr;
                int len = sizeof(SOCKADDR_IN);
                GetAcceptExSockaddrs(PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &len, (LPSOCKADDR*)&ClientAddr, &len);
                SERVERPRINT_INFO << "客户端连接" << inet_ntoa(ClientAddr->sin_addr) << "|" << PerIoData->sock << "|" << ServerSocket << endl;
                //获取客户端socket
                SOCKET csock = PerIoData->sock;



                TcpClient* c = GetFreeTcpClient();
                if (c == nullptr)
                {
                    SERVERPRINT_ERROR << "客户端信息储存失败 可以连接已达到最大值" << endl;

                }

                TcpClientIndex* cindex = GetClientIndex(csock);
                if (cindex == nullptr)
                {
                    SERVERPRINT_ERROR << "socket无效或超过最大值" << endl;


                }

                if (cindex != nullptr && c != nullptr)
                {

                    cindex->index = c->ID;
                    ConnectedSockets.push_back(csock);
                    // 传递接收事件
                    CreateIoCompletionPort((HANDLE)csock, CompletionPort, (DWORD)csock, 0);
                    ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
                    PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
                    PerIoData->DataBuf.len = DATA_BUFSIZE;
                    PerIoData->Type = 1;
                    DWORD Flags = 0;
                    DWORD dwRecv = 0;
                    WSARecv(csock, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
                    //重新new一个 为下面继续传递连接事件做准备
                    PerIoData = new PER_IO_OPERATION_DATA();

                }



                //继续传递连接事件

                ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
                PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
                PerIoData->DataBuf.len = DATA_BUFSIZE;
                PerIoData->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
                PerIoData->Type = 1;
                DWORD Flags = 0;
                DWORD dwRecv = 0;

                if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
                {
                    int error = WSAGetLastError();
                    if (error != ERROR_IO_PENDING)
                    {
                        SERVERPRINT_ERROR << "投递服务器sock失败!" << endl;

                    }
                }

                CurConnectedNum++;
                continue;
            }

            //连接关闭
            if (BytesTransferred == 0)
            {
                //关闭  回收socket
                SERVERPRINT_INFO << "正在设置socket状态为0：" << csocket << endl;
                TcpClient* p = FindTcpClient(csocket);
                if (p == nullptr)continue;
                p->state = 0;

            }

            //处理数据
            if (BytesTransferred > 0)
            {

                //对头信息进行简单解密
                for (int i = 0; i < 8; i++)
                {
                    PerIoData->data[i] ^= SafeCode;
                }

                int head = -1;
                int datasize = -1;
                memcpy(&head, PerIoData->data, 4);
                memcpy(&datasize, PerIoData->data + 4, 4);

                //对数据进行简单解密
                for (int i = 8; i < 8 + datasize; i++)
                {
                    PerIoData->data[i] ^= SafeCode;
                }



                Task* task = __TaskManager->GetFreeTask();
                task->Head = head;
                task->datasize = datasize;
                task->Sock = csocket;
                if (datasize > 0)
                {
                    char* tempbuff = new char[datasize];
                    memset(tempbuff, 0, datasize);
                    memcpy(tempbuff, PerIoData->data + 8, datasize);
                    task->data = tempbuff;
                }
                else
                {
                    task->data = nullptr;
                }
                __TaskManager->PushTaskToWork(task);


                //继续传递接收事件
                ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
                PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
                PerIoData->DataBuf.len = DATA_BUFSIZE;
                PerIoData->Type = 1;
                DWORD Flags = 0;
                DWORD dwRecv = 0;
                WSARecv(csocket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
                continue;
            }
        }
        else if (PerIoData->Type == 2)
        {
          if (PerIoData == nullptr)continue;
          SERVERPRINT_INFO << "数据发送成功" << BytesTransferred << endl;
           delete PerIoData;
         }
        else if (PerIoData->Type == 3)
        {


        int head = -1;
        int datasize = -1;
        memcpy(&head, PerIoData->data, 4);
        memcpy(&datasize, PerIoData->data + 4, 4);



        Task* task = __TaskManager->GetFreeTask();
        task->Head = head;
        task->datasize = datasize;
        task->Sock = csocket;
        if (datasize > 0)
        {
            char* tempbuff = new char[datasize];
            memset(tempbuff, 0, datasize);
            memcpy(tempbuff, PerIoData->data + 8, datasize);
            task->data = tempbuff;
        }
        else
        {
            task->data = nullptr;
        }
        __TaskManager->PushTaskToWork(task);


        //继续传递接收事件
        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;
        PerIoData->Type = 3;
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        WSARecv(csocket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
        continue;

         }


    }
    

}

bool TcpServer::CloseSock(SOCKET sock)
{
    if (sock == SOCKET_ERROR || sock == INVALID_SOCKET)return false;
    lock_guard<mutex> guard(TCLinkerMutex);
    SERVERPRINT_INFO << "正在关闭清理socket" << sock << endl;

    auto cindex = TcpClientIndexLinkers->at(sock);
    if (cindex != nullptr)
    {
        auto c = TcpClientLinkers->at(cindex->index);
        if (c != nullptr)
        {
            c->Reset();
        }
        cindex->Reset();
    }
    for (int i = 0; i < ConnectedSockets.size(); i++)
    {
        if (ConnectedSockets[i] == sock)
        {
            ConnectedSockets.erase(ConnectedSockets.begin() + i);
            break;
        }
    }


    closesocket(sock);
    CurConnectedNum--;


    return true;
}

TcpClient* TcpServer::GetFreeTcpClient()
{

    lock_guard<mutex> guard(TCLinkerMutex);

    for (int i = 0; i < TcpClientLinkers->length; i++)
    {
        TcpClient* FreeClient = TcpClientLinkers->at(i);
        if (FreeClient->state == 0)
        {
            FreeClient->Reset();
            FreeClient->ID = i;
            FreeClient->state = 1;
            return FreeClient;
        }
    }


    return nullptr;
}

TcpClientIndex* TcpServer::GetClientIndex(int socket)
{
    if (socket < 0 || socket >= maxSocket) return nullptr;
    TcpClientIndex* c = TcpClientIndexLinkers->at(socket);


    return c;
}

TcpClient* TcpServer::FindTcpClient(SOCKET sock)
{

    auto p = TcpClientIndexLinkers->at(sock);
    if (p == nullptr)return nullptr;
    auto p2 = TcpClientLinkers->at(p->index);
    if (p2 == nullptr)return nullptr;

    return p2;
}
