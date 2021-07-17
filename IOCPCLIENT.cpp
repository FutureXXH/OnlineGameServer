#include "IOCPSERVER.h"


//==========================================================================================================
//==========================================================================================================
//=======================================IOCPClient=========================================================
//==========================================================================================================
//==========================================================================================================


IOCPClient::IOCPClient(SOCKET tempsock)
{
    ip = "";
    sock = tempsock;
    threadp = nullptr;
    playerid = -1;
    starttime = clock() / CLOCKS_PER_SEC;
    endtime = clock() / CLOCKS_PER_SEC;
    Data = new PER_IO_OPERATION_DATA();
}

IOCPClient::~IOCPClient()
{
    SERVERPRINT_INFO << "正在删除客户端socket相关信息" << inet_ntoa(caddr.sin_addr) << endl;
    if (threadp != nullptr)
    {
        delete threadp;

    }
    if (Data != nullptr)
    {
        delete Data;
    }



}

void IOCPClient::response(int size, DATA data)
{
  
    SERVERPRINT_INFO << "正在处理数据" << endl;

    //char temp[6];
    //memcpy(&temp, tempbuf, 6);
    //temp[5] = '\0';
    //int headnum = atoi(temp);
    int headnum = data.head;
    SERVERPRINT_INFO << "头信息:" << headnum << endl;

    SERVERPRINT_INFO << "当前连接数量 " << ClientMap->size() << endl;

    switch (headnum)
    {
    case HEART:
        send10000(data);
        SERVERPRINT_INFO << "心跳包接收" << endl;
        break;
    case LOCATION:
        position(size, data);
        SERVERPRINT_INFO << "位置同步" << endl;
        break;
    case CLOSE_CONNECT:
        send20000(size, data);
        SERVERPRINT_INFO << "玩家退出" << endl;
        break;
    case SEND_MESSAGE:

        send20002(size, data);
        SERVERPRINT_INFO << "转发消息" << endl;
        break;
    default:
        //position(size, data);
        SERVERPRINT_INFO << "未知头信息" << endl;
        break;
    }


}

void IOCPClient::initmap(unordered_map<SOCKET, IOCPClient*>* tempClientMap)
{
    ClientMap = tempClientMap;


}

void IOCPClient::send10000(DATA data)
{
    this->starttime = clock() / CLOCKS_PER_SEC;
    this->playerid = data.ID;

}

void IOCPClient::send20000(int size, DATA data)
{
    for (auto iter = ClientMap->begin(); iter != ClientMap->end(); ++iter)
    {

        if (iter->first == this->sock)
        {
            SERVERPRINT_INFO << "跳过socket" << this->sock << endl;

        }
        else
        {
            int sendedsize = 0;
            int sendsize = size;
            while (sendedsize < sendsize)
            {

                int len = send(iter->first, (char*)(&data), sendsize - sendedsize, 0);
                if (len <= 0)
                {
                    break;
                }
                sendedsize += len;
            }
        }


    }


}

void IOCPClient::send20002(int size, DATA data)
{
    SERVERPRINT_INFO << "正在处理聊天 " << endl;

    for (auto iter = ClientMap->begin(); iter != ClientMap->end(); ++iter)
    {


        int sendedsize = 0;
        int sendsize = size;
        SERVERPRINT_INFO << "发送字节" << sendsize << endl;
        while (sendedsize < sendsize)
        {

            int len = send(iter->first, (char*)(&data), sendsize - sendedsize, 0);
            SERVERPRINT_INFO << "实际发送字节" << len << endl;
            if (len <= 0)
            {
                break;
            }
            sendedsize += len;
        }



    }

}

void IOCPClient::position(int size, DATA data)
{

    for (auto iter = ClientMap->begin(); iter != ClientMap->end(); ++iter)
    {
        //SERVERPRINT_INFO << "位置正在发送" << i << endl;
        if (iter->first == this->sock)
        {
            SERVERPRINT_INFO << "跳过socket" << iter->first << endl;

        }
        else
        {
            int sendedsize = 0;
            int sendsize = size;
            while (sendedsize < sendsize)
            {

                int len = send(iter->first, (char*)(&data), sendsize - sendedsize, 0);
                if (len <= 0)
                {
                    break;
                }
                sendedsize += len;
            }
        }


    }

}