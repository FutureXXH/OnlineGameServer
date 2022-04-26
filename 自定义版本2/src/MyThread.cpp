#include "../include/MyThread.h"

ThreadManager* __ThreadManager = nullptr;


void ModuleThread::ThreadRun()
{
	Log(INFO,"模块处理线程开启: "+str(ID));


    //线程调度
	while (true)
	{
		//从模块队列中取出模块进行处理
		auto ModulePtr = __ModuleManager->ThreadModuleQueue.pop();
		if(ModulePtr == nullptr)continue;


       switch (ModulePtr->ModuleState)
	   {
	   case MODULE_ERROR:
	   {
		   
	   }
	   break;
	   case MODULE_INIT:
	   {
		   	ModulePtr->Init();
			ModulePtr->ModuleState = MODULE_RUNING;
			__ModuleManager->ThreadModuleQueue.push(ModulePtr);
	   }
		   break;
	   case MODULE_RUNING:
	   {
		   ModulePtr->parseQueue();
		    ModulePtr->update();
          __ModuleManager->ThreadModuleQueue.push(ModulePtr);
	   }
		   break;
	   case MODULE_CLOSING:
	   {
		   //模块即将删除
			Log(WARNING,"模块ID" + str(ModulePtr->ID)+ "正在删除");
			ModulePtr->Exit();
				
			delete ModulePtr;
	   }
		   break;   
	   default:
		   break;
	   }


		this_thread::sleep_for(chrono::milliseconds(10));
	}


}



ThreadManager::ThreadManager(int SetModuleThreadNum)
{

	ModuleThreadNum = SetModuleThreadNum;


}

void ThreadManager::StartThread()
{


	if (__ModuleManager == nullptr)
	{
		throw"模块管理器指针为null";
	}
	//开启模块管理器线程
	ModuleManagerThreadPtr = new thread(&ModuleManager::ManagerRun, __ModuleManager);
	ModuleManagerThreadPtr->detach();

	//==================================================================================
	for (int i = 0; i < ModuleThreadNum; i++)
	{
		ModuleThread* ModuleThreadp = new ModuleThread(i);
		Pool.emplace_back(ModuleThreadp);
		ModuleThreadp->myThread = new thread(&ModuleThread::ThreadRun, ModuleThreadp);
		ModuleThreadp->myThread->detach();
	}


}
