#include "../include/ModuleManager.h"

#include <iostream>
ModuleManager* __ModuleManager = new ModuleManager();




 ModuleBase* ModuleManager::Generate_LuaModule(int setid,string FileName)
 {
     ModuleBase* p =  new LuaModule(setid,FileName);
     RegisterModule(p);

	 return p;

}




ModuleManager::ModuleManager()
{
    //预开辟消息数据内存
	Message *Messagepool = new Message[MessageObjPoolSize];
	for (int i = 0; i < MessageObjPoolSize; i++)
	{
		MessageObjPool.push(&Messagepool[i]);
	}

     reg_ConsoleCMD();
}



void ModuleManager::InitModule()
{



    //std::cout << ModuleReg.size() << std::endl;
    //初始化模块表 
    for (int i = 0; i < ModuleReg.size(); i++)
    {
       
        if (ModuleReg[i] == nullptr || ModuleTable.count(ModuleReg[i]->ID) > 0)continue;
       
        ModuleTable.emplace(ModuleReg[i]->ID, ModuleReg[i]);

    }




   //执行初始化函数
    for (auto it = ModuleTable.begin(); it != ModuleTable.end(); it++)
    {
        if(it->second == nullptr)continue;
         it->second->Init();
    }



}

bool ModuleManager::pushDataMessageQueue(Message* m)
{
   if(m == nullptr)return false;
    dataMessageQueue.push(m);
    return true;
}

bool ModuleManager::pushMessageToModule(Message* m, int32 moduleID)
{
    if (m == nullptr || ModuleTable.count(moduleID)  == 0|| ModuleTable[moduleID] == nullptr)return false;
    ModuleTable[moduleID]->pushMessage(m);
    return true;
}

bool ModuleManager::RegisterModule(ModuleBase* mp)
{
    if (mp == nullptr)return false;
    ModuleReg.emplace_back(mp);
    return true;
}

bool ModuleManager::RegisterMessage(int32 messageID, int32 moduleID)
{
    lock_guard<mutex> lock(regMessageMutex);
    if(MessageTable[messageID].count(moduleID))return true;
    ModuleRegMessageTable[moduleID].emplace(messageID);
    MessageTable[messageID].emplace(moduleID);
 
    return true;
}

void ModuleManager::AssignData()
{
    Message* m = dataMessageQueue.pop();
    while (m != nullptr)
    {




        if (MessageTable.count(m->MessageID) != 0)
        {

            for (auto it = MessageTable[m->MessageID].begin(); it != MessageTable[m->MessageID].end(); it++)
            {
                
                pushMessageToModule(m, *it);
            }

        }
        PushMessageObj(m);
        m = dataMessageQueue.pop();

    }


}

void ModuleManager::ManagerRun()
{

    InitModule();
    while (true)
    {
        AssignData();
    }
}


Message* ModuleManager::GetMessageObj()
{
    if(MessageObjPool.size() == 0)return nullptr;
	auto p = MessageObjPool.pop();
    if(p == nullptr){
        Log(WARNING,"获取的消息指针为nullptr");
        return nullptr;
    }
	p->reset();
	return p;
}


bool ModuleManager::PushMessageObj(Message* obj)
{
     MessageObjPool.push(obj);
     return true;
}


void printModlueInfo()
{
    Log(INFO,"模块信息:");
    int i = 1;
  for (auto it = __ModuleManager->ModuleTable.begin(); it != __ModuleManager->ModuleTable.end(); it++,i++)
  {
    Log(INFO, to_string(i) + "   模块ID:" + to_string(it->second->ID) + "   模块注册的消息数量 : " + to_string(__ModuleManager->ModuleRegMessageTable[it->second->ID].size()) );

  }

  

}


void reloadLuaModule()
{
    Log(INFO,"输入模块ID");
    int ID;
    cin >> ID;
   if(__ModuleManager->ModuleTable.count(ID) == 0)
   {
       Log(ERROR,"没有这个模块");
       return;
   }
   auto p =  dynamic_cast<LuaModule*>(__ModuleManager->ModuleTable[ID]);
   if(p == nullptr)
   {
       Log(ERROR,"重启模块异常");
      return;
   }
   p->ReloadLua();


}




void ModuleManager::reg_ConsoleCMD()
{
   __ConsoleCMD->Register_Console_CMD("minfo",&printModlueInfo,"模块信息");
      __ConsoleCMD->Register_Console_CMD("reloadL",&reloadLuaModule,"重载Lua模块");
}
