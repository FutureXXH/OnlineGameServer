#include "TCPSERVER.h"



shared_mutex sharemutex;
HANDLE semapthore = CreateSemaphore(NULL, 0, 5000, L"ServiceThread");


TCPSERVER::TCPSERVER()
{

}

TCPSERVER::~TCPSERVER()
{
}

bool TCPSERVER::DeleteClientSocket(SOCKET ClientSocket)
{
    
    {
    unique_lock<shared_mutex> lock(sharemutex); //设置写锁


    if (ClientMap->find(ClientSocket) != ClientMap->end())
    {
        SERVERPRINT_INFO << "正在删除客户端socket" << inet_ntoa((*ClientMap)[ClientSocket]->caddr.sin_addr) << endl;
        //delete ClientMap->at(client.sock);

       ClientMap->erase(ClientSocket);
        
        shutdown(ClientSocket, 2);
      
        return true;
        
    }
    else
    {
        SERVERPRINT_INFO << "未找到相关socket 可能已经被删除" << endl;
    }

    }
    
    return false;
}

bool TCPSERVER::InitWinsock()
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        SERVERPRINT_ERROR << "Winsock初始化失败" << endl;
        return false;
    }

    return true;
 
}

bool TCPSERVER::InitWinSERVER(int port)
{
 

    ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


    memset(&Server_adr, 0, sizeof(Server_adr));
    Server_adr.sin_family = AF_INET;
    Server_adr.sin_addr.S_un.S_addr = INADDR_ANY;
    Server_adr.sin_port = htons(port);

    if (bind(ServerSocket, (struct sockaddr*)&Server_adr, sizeof(Server_adr)) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "bind error" << endl;
        return false;
    }
    if (listen(ServerSocket, 20) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "listen error" << endl;
        return false;
    }
    else
    {


        SERVERPRINT_INFO << "服务器网络初始化完成" << std::endl;
        return true;
    }


    SERVERPRINT_ERROR << "意外退出" << std::endl;
    return false;
}

void TCPSERVER::StartIOCPSERVER(Concurrency::concurrent_queue<task*>* tasks)
{


    //设置服务器sock非阻塞模式
    unsigned long u = 1;
    if (ioctlsocket(ServerSocket, FIONBIO, &u) == SOCKET_ERROR)
    {
        SERVERPRINT_ERROR << "设置服务器非阻塞套接字失败!" << endl;
        return;
    }
    //
    __tasks = tasks;
   

    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);

    
    if (threadnum > 0)
    {
    }
    else if (SystemInfo.dwNumberOfProcessors >= 6)
    {
        threadnum = SystemInfo.dwNumberOfProcessors * 2 - 6;
        //如果处理器较多 分配多点给业务线程
    }
    else //
    {
        threadnum = SystemInfo.dwNumberOfProcessors;
    }

 
    for (int i = 0; i < threadnum; i++)
    {
        thread* threadp = new thread(&TCPSERVER::ProcessIO, this, CompletionPort);

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

    DWORD dwRecv = 0;

    if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
    {
        int error = WSAGetLastError();
        if (error != ERROR_IO_PENDING)
        {
            SERVERPRINT_ERROR << "投递服务器sock失败!" << error << endl;

            return;
        }
    }







    SERVERPRINT_INFO << "服务器成功开启" << endl;
    while (true)
    {
        string aa;
        aa += "当前连接数:";
        aa += std::to_string(ClientMap->size());
        aa += " 当前业务任务量";
        aa += std::to_string(__tasks->unsafe_size());
        system(("title " + aa).c_str());
    }


}
//iocp io线程
void TCPSERVER::ProcessIO(LPVOID lpParam)
{

    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    SOCKET csocket;
    LPPER_IO_OPERATION_DATA PerIoData;

    while (true)
    {
        //INFINITE
        if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&csocket, (LPOVERLAPPED*)&PerIoData, 10000))
        {
            
           
            //SERVERPRINT_INFO << "发现事件0" << GetLastError() << endl;
            if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
            {
                //SERVERPRINT_INFO << "等待超时" << endl;
                //IOCPSERVER::deleteclient(*(ClientMap->at(csocket)));

                //delete PerIoData;
            }
            else
            {

                SERVERPRINT_WARNING << "完成端口失败 可能是因为相关连接已经断开" << endl;
            }



            continue;

        }

        SERVERPRINT_INFO << "发现事件" << endl;
        //有客户端连接
        if (csocket == ServerSocket)
        {
           
            SOCKADDR_IN* LocalAddr = nullptr;
            SOCKADDR_IN* ClientAddr = nullptr;
            int len = sizeof(SOCKADDR_IN);
            GetAcceptExSockaddrs(PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &len, (LPSOCKADDR*)&ClientAddr, &len);
            SERVERPRINT_INFO << "客户端连接" << inet_ntoa(ClientAddr->sin_addr) << endl;
            {
                unique_lock<shared_mutex> lock(sharemutex); //设置写锁
                ClientMap->emplace(PerIoData->sock, new TCPClient(PerIoData->sock, *ClientAddr));
                
            }
            CreateIoCompletionPort((HANDLE)PerIoData->sock, CompletionPort, (DWORD)PerIoData->sock, 0);
            //传递给业务层创建新玩家数据信息
            task* createnewplayertask = new task();
            createnewplayertask->clent = ClientMap->at(PerIoData->sock);
            createnewplayertask->Head = 10002;
            __tasks->push(createnewplayertask);
            ReleaseSemaphore(semapthore, 1, NULL);



            // 传递接收事件
            
            SOCKET temp = PerIoData->sock;
            
            ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
            PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
            PerIoData->DataBuf.len = DATA_BUFSIZE;
            DWORD Flags = 0;
            DWORD dwRecv = 0;
            WSARecv(temp, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);



            //继续传递连接事件
            PerIoData = new PER_IO_OPERATION_DATA();
            ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
            PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
            PerIoData->DataBuf.len = DATA_BUFSIZE;
            PerIoData->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            

            if (AcceptEx(ServerSocket, PerIoData->sock, PerIoData->DataBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &PerIoData->Overlapped) == false)
            {
                int error = WSAGetLastError();
                if (error != ERROR_IO_PENDING)
                {
                    SERVERPRINT_ERROR << "投递服务器sock失败!" << endl;
                    
                }
            }

            continue;
        }

        // 客户端退出
        if (BytesTransferred == 0)
        {
            SERVERPRINT_INFO << "客户端退出" << csocket << endl;
            DeleteClientSocket(csocket);
            delete PerIoData;

            continue;
        }


        if (BytesTransferred > 0)
        {

            SERVERPRINT_INFO << "接收到消息" << BytesTransferred    << endl;
          
                int head = -1;
                int size = -1;
                //对头部信息进行解密
                for (int i = 0; i < 8; i++)
                {
                    PerIoData->data[i] ^= SafeCode;
                }

                memcpy(&head, PerIoData->data, 4);
                memcpy(&size, PerIoData->data + 4, 4);

                if (head < 0 || head > 100000 || size < 0 || size > 1016)
                {
                    //非法数据 不进行处理
                    continue;
                }
                //对数据信息进行解密
                for (int i = 8; i < size; i++)
                {
                    PerIoData->data[i] ^= SafeCode;
                }
                //将读取数据推入业务处理队列中  不处理业务逻辑
                task* temptask = new task();
                // 读锁
                {
                    shared_lock<shared_mutex> lock(sharemutex);
                    temptask->clent = ClientMap->at(csocket);

                }
                
                memcpy(temptask->Data, PerIoData->data + 8, size);
                temptask->DataSize = size;
                temptask->Head = head;

                __tasks->push(temptask);
                ReleaseSemaphore(semapthore, 1, NULL);
           

            //ClientMap->at(csocket)->response(BytesTransferred, PerIoData->data);
            
        }



        // 继续向 socket 投递WSARecv操作
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;
        WSARecv(csocket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
    }

    return;

}




