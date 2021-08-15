#include "APPManager.h"


shared_mutex sharedmutex;

bool APPManager::InitManager(Concurrency::concurrent_queue<task*>* tasks , TCPSERVER* ptcpserver)
{

    __tasks = tasks;
    Ptcpserver = ptcpserver;
 
    semapthore = CreateSemaphore(NULL, 0, 5000, L"ServiceThread"); //创建信号量

    if (threadnum <= 0)
    {
        threadnum = 2;
    }
     
    //task::__tasks = new Concurrency::concurrent_queue<task*>();
    for (int i = 0; i < threadnum; i++)
    {
        thread* threadp = new thread(&APPManager::ProcessService, this, semapthore);

    }
    SERVERPRINT_INFO << "创建" << threadnum << "个业务处理线程" << endl;



    return true;
}
//业务线程   从任务队列取出任务处理
void APPManager::ProcessService(HANDLE semapthore)
{
    
    while (true)
    {
        WaitForSingleObject(semapthore, INFINITE); //等待信号量资源
        
        if (__tasks->unsafe_size() > 0)
        {
            
            task* temptask;
            if (!__tasks->try_pop(temptask))
            {
                SERVERPRINT_WARNING << "空任务 " << endl;
                continue;
            }
            if (temptask == nullptr)
            {
                SERVERPRINT_WARNING << "空任务指针" << endl;
                continue;
            }
            SERVERPRINT_INFO << "发现任务" << endl;
            Response(*temptask);

            delete temptask;
            
        }
    }

}
// 处理业务部分 
void APPManager::Response(task& Task)
{
    switch (Task.Head)
    {
    case HEART:
        r10000(Task.clent->sock);
        break;
    case TEST:
        r10001(Task.clent->sock);
        break;
    case NEWPLAYER:
        r10002(Task.clent->sock);
        SERVERPRINT_INFO << "创建新连接玩家数据" << endl;
        break;

    default:
        break;
    }

}

void APPManager::r10000(SOCKET& playersock)
{
    {
        unique_lock<shared_mutex> lock(sharedmutex); // 设置写锁
        Playerlist[playersock]->starttime  = clock() / CLOCKS_PER_SEC;
    }
}

void APPManager::r10001(SOCKET& playersock)
{
    {
        shared_lock<shared_mutex> lock(sharedmutex); // 设置读锁
        if (Playerlist[playersock]->islogin)
        {
            SERVERPRINT_INFO << "测试数据:已登录" << endl;
        }
        else
        {
            SERVERPRINT_INFO << "测试数据:未登录" << endl;
        }
    }
}


void APPManager::r10002(SOCKET& playersock) //新连接玩家，创建业务层数据
{
    Player* newplayer = new Player(playersock);
    {
    unique_lock<shared_mutex> lock(sharedmutex); // 设置写锁
    Playerlist.emplace(playersock, newplayer);

    }
}




//=========================================================

