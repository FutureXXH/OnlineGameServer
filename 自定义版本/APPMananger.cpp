#include "APPMananger.h"



void update()
{
    string aa;
    aa += "当前网络连接数:";
    aa += std::to_string(__TcpServer->CurConnectedNum);
    aa += " 当前业务任务量";
    aa += std::to_string(__TaskManager->CurWorkTaskNum);
    aa += " 当前玩家数";
    aa += std::to_string(__PlayerManager->CurOnlinePlayer);
    system(("title " + aa).c_str());


   

}



void app::HeartCheck()
{
    for (int i = 0; i < __TcpServer->ConnectedSockets.size(); i++)
    {
        auto p = __TcpServer->FindTcpClient(__TcpServer->ConnectedSockets[i]);
        if (p == nullptr)continue;

        switch (p->state)
        {
        case 0:
            __TcpServer->CloseSock(__TcpServer->ConnectedSockets[i]);
            break;
        case 1:
        {
            int temptime = (int)time(NULL);
            if (temptime - p->HearTime > 5)
            {
                p->state = 0;
            }
        }
            break;
        default:
            break;
        }
    }


}

int app::run()
{
    int port = -1;
    int IOThreadNum = 0;
    int WorkThreadNum = 0;
    int maxConnect = 0;
    int MaxOnlinePlayer = 0;
    share::LoadServerXML("server_config.xml", nullptr, port, SafeCode, IOThreadNum, WorkThreadNum, maxConnect,MaxOnlinePlayer);

    SERVERPRINT_INFO << "正在启动服务器" << std::endl;

    //初始化服务器网络管理器
    __TcpServer = new TcpServer();
    __TcpServer->threadnum = IOThreadNum;
    __TcpServer->maxConnect = maxConnect;
    if (__TcpServer->StartTcpServer(port))
    {  
        SERVERPRINT_INFO << "服务器网络初始化完成" << std::endl;
    }

    //初始化任务管理器
    SERVERPRINT_INFO << "正在初始化任务管理器" << std::endl;
    __TaskManager = new TaskManager();
    __TaskManager->init(1000);

    //初始化工作线程管理器
    SERVERPRINT_INFO << "正在初始化工作线程管理器" << std::endl;
    WorkThreadManager* WorkThreadManagerP = new WorkThreadManager();
    WorkThreadManagerP->InitWorkThread(WorkThreadNum);

    Sleep(1000);
    //初始化DB连接器
    SERVERPRINT_INFO << "正在初始化DB连接器" << std::endl;
    __DBLink = new DBLink();
    __DBLink->init("10.17.78.88", 6667, __TcpServer->CompletionPort);
    __DBLink->ConnectDB();

    //初始化玩家管理器
    SERVERPRINT_INFO << "正在初始化玩家管理器" << std::endl;
    __PlayerManager = new PlayerManager();
    __PlayerManager->init(MaxOnlinePlayer);


    SERVERPRINT_INFO << "服务器开启完成" << std::endl;

    while (true)
    {
        update();
    }


    return 0;
}
