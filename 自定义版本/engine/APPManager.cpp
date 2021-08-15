#include "APPManager.h"

bool APPManager::InitManager(int num,Concurrency::concurrent_queue<task*>* tasks)
{

    __tasks = tasks;
    
    //task::__tasks = new Concurrency::concurrent_queue<task*>();
    for (int i = 0; i < num; i++)
    {
        thread* threadp = new thread(&APPManager::ProcessService, this);

    }
    SERVERPRINT_INFO << "创建" << num << "个业务处理线程" << endl;



    return true;
}
//业务线程   从任务队列取出任务处理
void APPManager::ProcessService()
{
    
    while (true)
    {
        if (__tasks->unsafe_size() > 0)
        {
            
            task* temptask;
            if (!__tasks->try_pop(temptask))
            {
                continue;
            }
            if (temptask == nullptr)
            {
                continue;
            }

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
    case 10000:
        Task.clent->starttime = clock() / CLOCKS_PER_SEC;
        break;
    case 10001:
        SERVERPRINT_INFO << "测试数据" << endl;
        break;


    default:
        break;
    }

}




//=========================================================

