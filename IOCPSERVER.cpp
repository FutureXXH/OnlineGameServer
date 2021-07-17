#include "IOCPSERVER.h"

std::mutex mtx;
void heart()
{

    for (auto iter = ClientMap->begin(); iter != ClientMap->end(); ++iter)
    {
        iter->second->endtime = clock() / CLOCKS_PER_SEC;
        if (iter->second->endtime - iter->second->starttime > 10)
        {
            SERVERPRINT_INFO << "连接超时" << inet_ntoa(iter->second->caddr.sin_addr) << "|" << iter->second->endtime - iter->second->starttime << endl;
            if (iter->second->playerid != -1)
            {

                DATA data;
                data.head = 20000;
                data.ID = iter->second->playerid;
                data.state = 1;
                SERVERPRINT_INFO << "向其他玩家发送离开信息" << endl;
                iter->second->send20000(sizeof(data), data);
            }

            IOCPSERVER::deleteclient(*(iter->second));
            break;
        }
    }
    Sleep(1000);
    heart();
}





//==========================================================================================================
//==========================================================================================================
//=======================================IOCPSERVER=========================================================
//==========================================================================================================
//==========================================================================================================

IOCPSERVER::IOCPSERVER()
{


}

void IOCPSERVER::deleteclient(IOCPClient& client)
{
    if (ClientMap->count(client.sock) > 0)
    {
        SERVERPRINT_INFO << "正在删除客户端socket" << inet_ntoa(client.caddr.sin_addr) << endl;
        ClientMap->erase(client.sock);
        closesocket(client.sock);
        delete& client;
    }
    else
    {
        SERVERPRINT_INFO << "重复删除" << endl;
    }
 

}

bool IOCPSERVER::InitWinsock()
{

    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        SERVERPRINT_INFO << "Winsock初始化失败" << endl;
        return false;
    }

    return true;
}

bool IOCPSERVER::InitWinSERVER(int port)
{
    serversocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


    memset(&server_adr, 0, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_adr.sin_port = htons(port);

    if (bind(serversocket, (struct sockaddr*)&server_adr, sizeof(server_adr)) == SOCKET_ERROR)
    {
        std::cout << "bind error";
        return false;
    }
    if (listen(serversocket, 10) == SOCKET_ERROR)
    {
        std::cout << "listen error";
        return false;
    }
    else
    {


        SERVERPRINT_INFO << "服务器已开启" << std::endl;
        return true;
    }


    SERVERPRINT_INFO << "意外退出" << std::endl;
    return false;
}


void ProcessIO(LPVOID lpParam)
{
    
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    SOCKET csocket;
    LPPER_IO_OPERATION_DATA PerIoData;
    
    while (true)
    {

        if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&csocket, (LPOVERLAPPED*)&PerIoData, INFINITE))
        {
            if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
            {
                SERVERPRINT_INFO << "等待超时" << endl;
                IOCPSERVER::deleteclient(*ClientMap->at(csocket));

                delete PerIoData;
        
                continue;
            }
            else
            {
                SERVERPRINT_INFO << "完成端口失败 可能连接已经断开" << endl;
            }
            return ;
        }

        // 说明客户端已经退出
        if (BytesTransferred == 0)
        {
            cout << "closing socket" << csocket << endl;
            IOCPSERVER::deleteclient(*ClientMap->at(csocket));
            delete PerIoData;
           
            continue;
        }
        if (BytesTransferred == sizeof(DATA))
        {
            cout << csocket << "发送过来的消息头：" << PerIoData->data.head << endl;
            ClientMap->at(csocket)->response(BytesTransferred, PerIoData->data);
        }
       
        

        // 继续向 socket 投递WSARecv操作
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;
        WSARecv(csocket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
    }

    return ;

}

void IOCPSERVER::StartIOCPSERVER()
{
    ClientMap = new unordered_map<SOCKET, IOCPClient*>();
    IOCPClient::initmap(ClientMap);

    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    
    for (int i = 0; i < SystemInfo.dwNumberOfProcessors * 2-2; i++)
    {
        thread* threadp = new thread(ProcessIO, CompletionPort);

    }
    SERVERPRINT_INFO << "创建" << SystemInfo.dwNumberOfProcessors * 2 -2 << "个IO线程" << endl;


    thread* heardthreadp = new thread(heart);
    SERVERPRINT_INFO << "创建心跳包检测线程" << endl;

    SOCKET sClient;
    
    LPPER_IO_OPERATION_DATA PerIoData;

    sockaddr_in tempcaddr;
    int len = sizeof(tempcaddr);
    while (true)
    {
        SERVERPRINT_INFO << "等待客户端接入" << endl;
   
        
        sClient = accept(this->serversocket, (sockaddr*)&tempcaddr, &len);
        SERVERPRINT_INFO << "Socket " << sClient << "连接进来" << endl;


       
        ClientMap->emplace(sClient, new IOCPClient(sClient));
        ClientMap->at(sClient)->sock = sClient;
        ClientMap->at(sClient)->caddr = tempcaddr;
    


        SERVERPRINT_INFO << "当前连接数量 " << ClientMap->size() << endl;
        // 将接入的客户端和完成端口联系起来
        CreateIoCompletionPort((HANDLE)sClient, CompletionPort, (DWORD)sClient, 0);

        // 建立一个Overlapped，并使用这个Overlapped结构对socket投递操作
        PerIoData = new PER_IO_OPERATION_DATA();

        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;

        // 投递一个WSARecv操作
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        WSARecv(sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
    }

    DWORD dwByteTrans;

    PostQueuedCompletionStatus(CompletionPort, dwByteTrans, 0, 0);  

    closesocket(this->serversocket);
}
