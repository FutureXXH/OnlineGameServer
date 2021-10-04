#include "WorkThread.h"

WorkThread::WorkThread(int id)
{
	ThreadID = id;
	semapthore = CreateSemaphore(NULL, 0, 5000, L"WorkThread");
	state = 0;
}

void WorkThread::Run()
{
	SERVERPRINT_INFO << "工作线程正在运行：" << this->ThreadID << endl;
	while (true)
	{

		WaitForSingleObject(semapthore, INFINITE);
		Task* task;
		task = __TaskManager->PopWorkTask();
		if (task == nullptr)
		{
			continue;
		}
		state = 1;
		//处理业务逻辑
		app::OnCommand(task);
		//================


		__TaskManager->PushTaskToPool(task);
		state = 0;
	}




}


//=================================================================
void WorkThreadManager::InitWorkThread(int ThreadNum)
{
	if (ThreadNum <= 0)
	{
		SERVERPRINT_WARNING << "当前工作业务线程设置异常，将设置为默认值3" << std::endl;
		WorkThreadNum = 3;
	}
	SERVERPRINT_INFO << "正在开启" << ThreadNum << "个工作业务线程" << endl;

	for (int i = 1; i <= ThreadNum; i++)
	{
		//初始化线程信息
		WorkThread* workthreadp = new WorkThread(1000 * i);
		//工作线程运行
		thread* p = new thread(&WorkThread::Run, workthreadp);
		//p->detach();
	}
}
