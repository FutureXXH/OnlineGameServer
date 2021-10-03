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
    
    __TcpServer = new TcpServer();
    if (__TcpServer->StartTcpServer(6666))
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
    WorkThreadManagerP->InitWorkThread(3);

    Sleep(1000);
    SERVERPRINT_INFO << "正在初始化DB管理器" << std::endl;
    __DBLink = new DBLink();
    __DBLink->init("10.17.78.88", 6667, __TcpServer->CompletionPort);
    __DBLink->ConnectDB();
    SERVERPRINT_INFO << "正在玩家管理器" << std::endl;
    __PlayerManager = new PlayerManager();



    SERVERPRINT_INFO << "服务器开启完成" << std::endl;

    while (true)
    {
        update();
    }


    return 0;
}
