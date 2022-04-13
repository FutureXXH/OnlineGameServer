#include "../include/MyThread.h"

void ModuleThread::ThreadRun()
{
	Log(INFO,"模块处理线程开启: "+str(ID));

	while (true)
	{
		for (auto it = ModuleReg.begin(); it != ModuleReg.end(); it++)
		{
			if (it->second == nullptr)
			{
				//模块指针为null
				continue;
			}

			it->second->parseQueue();
			it->second->update();
           
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}


}

bool ModuleThread::RegisterModule( ModuleBase* m)
{
	if (ModuleReg.count(m->ID) > 0)return false;
	ModuleReg.emplace(m->ID, m);

	return true;
}

ThreadManager::ThreadManager(int SetModuleThreadNum, ModuleManager* setModuleManagerPtr)
{

	ModuleThreadNum = SetModuleThreadNum;
	ModuleManagerPtr = setModuleManagerPtr;

}

void ThreadManager::StartThread()
{

	 Log = bind(&ConsoleLog::push_ConsoleLog,__ConsoleLog,placeholders::_1,placeholders::_2);
	ConsoleThreadPtr = new thread(&ConsoleLog::ConsoleThreadRun,__ConsoleLog);
	this_thread::sleep_for(chrono::seconds(1));
    Log(INFO,"服务器控制台初始化完毕");


	if (ModuleManagerPtr == nullptr)
	{
		throw"模块管理器指针为null";
	}
	//开启模块管理器线程
	ModuleManagerThreadPtr = new thread(&ModuleManager::ManagerRun, ModuleManagerPtr);
	ModuleManagerThreadPtr->detach();

	//==================================================================================
	int ModuleNum = ModuleManagerPtr->ModuleReg.size();
	int AveThreadModuleNum = ModuleNum / ModuleThreadNum;
	int Cur = 0;
	//平均分配模块给线程
	for (int i = 0; i < ModuleThreadNum; i++)
	{
		ModuleThread* ModuleThreadp = new ModuleThread(i);
		if (i == ModuleThreadNum - 1)
		{
			while (Cur < ModuleNum)
			{
				
				ModuleThreadp->RegisterModule(ModuleManagerPtr->ModuleReg[Cur]);
				Cur++;
			}
		}
		else
		{
			for (int j = 0; j < AveThreadModuleNum; j++)
			{
				
				ModuleThreadp->RegisterModule(ModuleManagerPtr->ModuleReg[Cur]);
				Cur++;
			}
		}
		Pool.emplace_back(ModuleThreadp);
		ModuleThreadp->myThread = new thread(&ModuleThread::ThreadRun, ModuleThreadp);
		ModuleThreadp->myThread->detach();
	}


}
