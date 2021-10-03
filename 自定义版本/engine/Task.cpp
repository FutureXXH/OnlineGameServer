#include "Task.h"

TaskManager* __TaskManager;


void Task::Reset()
{
	Head = 0;
	datasize = 0;
	if (data != nullptr)
	{
		delete data;
		data = nullptr;
	}
	


}

bool TaskManager::init(int maxtask)
{

	semapthore = CreateSemaphore(NULL, 0, 5000, L"WorkThread");
	for (int i = 0; i < maxtask; i++)
	{
		Task* p = new Task();
		p->Reset();
		TaskPool.push(p);
	}



	return true;
}

bool TaskManager::PushTaskToWork(Task* pushTaskP)
{
	if (pushTaskP == nullptr)
	{
		SERVERPRINT_WARNING << "空任务指针" << endl;
		return false;
	}

	__WorkTasks.push(pushTaskP);
	CurWorkTaskNum++;

	//信号量资源+1
	ReleaseSemaphore(semapthore, 1, NULL);
	return true;
}

Task* TaskManager::PopWorkTask()
{

	if (__WorkTasks.empty())
	{
		return nullptr;
	}

	CurWorkTaskNum--;
	Task* p;
	__WorkTasks.try_pop(p);

	return p;
}

Task* TaskManager::GetFreeTask()
{
	if (TaskPool.empty())
	{
		return nullptr;
	}
	Task* p;
	TaskPool.try_pop(p);

	return p;
}

bool TaskManager::PushTaskToPool(Task* TaskP)
{
	if (TaskPool.unsafe_size() > MaxTask )
	{
		if (TaskP == nullptr)
		{
			return false;
	     }
		else
		{
			delete TaskP;
			return true;
		}
	}
	
	TaskP->Reset();
	TaskPool.try_pop(TaskP);




	return true;
}
