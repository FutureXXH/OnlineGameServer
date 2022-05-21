#include "../include/MyThread.h"

ThreadManager* __ThreadManager = nullptr;


void ModuleThread::ThreadRun()
{
	Log(INFO,"模块处理线程开启: "+str(ID));

    ModuleBase* ModulePtr  = nullptr;
    //线程调度
	while (true)
	{
		
		//从模块队列中取出模块进行处理
		if(IsSpecialTheard)
		{
            ModulePtr = SpecialTheardModule;
			if(ModulePtr == nullptr)
			{
				Log(ERROR,"模块处理线程" + str(ID) + "获取独占模块失败！");
			}
		}
		else
		{
			
		    ModulePtr = __ModuleManager->ThreadModuleQueue.pop();
		   if(ModulePtr == nullptr)continue;
		  
		}
		 


    
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
			if(!IsSpecialTheard)
			__ModuleManager->ThreadModuleQueue.push(ModulePtr);
	   }
		   break;
	   case MODULE_RUNING:
	   {
		   
		   ModulePtr->parseQueue();
		   //传入时间戳 毫秒
		    ModulePtr->update();
			if(!IsSpecialTheard)
          __ModuleManager->ThreadModuleQueue.push(ModulePtr);
	   }
		   break;
	   case MODULE_CLOSING:
	   {
		   //模块即将删除
			Log(WARNING,"模块ID" + str(ModulePtr->ID)+ "正在删除");
			ModulePtr->Exit();
				
			delete ModulePtr;

			if(IsSpecialTheard)
			{
			Log(WARNING ,"线程"+ str(ID) + "退出");
			return;
			}

	   }
		   break;   
	   default:
		   break;
	   }


		
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


	

    //对需要独占线程的模块开启线程
	for (int i = 0; i < __ModuleManager->SpecialTheardReg.size(); i++)
	{
		ModuleThread* ModuleThreadp = new ModuleThread(i);
		ModuleThreadp->IsSpecialTheard = true;
		ModuleThreadp->SpecialTheardModule = __ModuleManager->SpecialTheardReg[i];
		Pool.emplace_back(ModuleThreadp);
		ModuleThreadp->myThread = new thread(&ModuleThread::ThreadRun, ModuleThreadp);
		ModuleThreadp->myThread->detach();
	}
	

	//共享模块线程开启
	for (int i = __ModuleManager->SpecialTheardReg.size(); i < __ModuleManager->SpecialTheardReg.size()+ModuleThreadNum; i++)
	{
		ModuleThread* ModuleThreadp = new ModuleThread(i);
		Pool.emplace_back(ModuleThreadp);
		ModuleThreadp->myThread = new thread(&ModuleThread::ThreadRun, ModuleThreadp);
		ModuleThreadp->myThread->detach();
	}


}
