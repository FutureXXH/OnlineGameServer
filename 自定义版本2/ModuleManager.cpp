#include "ModuleManager.h"

#include <iostream>
ModuleManager* __ModuleManager = new ModuleManager();

ModuleManager::ModuleManager()
{
     for (int i = 0; i < MessageObjPoolSize; i++)
     {
     		MessageObjPool.push(new Message());
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
     
        ModuleReg[i]->Init();
       
        ModuleTable.emplace(ModuleReg[i]->ID, ModuleReg[i]);

    }




    //初始化消息表
    for (auto it = ModuleTable.begin(); it != ModuleTable.end(); it++)
    {
        if (it->second != nullptr)
        {

            for (int i = 0; i < it->second->MessageReg.size(); i++)
            {
               RegisterMessage(it->second->MessageReg[i], it->second->ID);
            }



        }
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
    MessageTable[messageID].emplace_back(moduleID);
    return true;
}

void ModuleManager::AssignData()
{
    Message* m = dataMessageQueue.pop();
    while (m != nullptr)
    {
        if (MessageTable.count(m->MessageID) != 0)
        {

            for (int i = 0; i < MessageTable[m->MessageID].size(); i++)
            {
                
                pushMessageToModule(m, MessageTable[m->MessageID][i]);
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
        cout << "获取的消息指针为nullptr" << endl;
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
    cout << "模块信息:"  << endl;
    int i = 1;
  for (auto it = __ModuleManager->ModuleTable.begin(); it != __ModuleManager->ModuleTable.end(); it++,i++)
  {
      cout  << i << "   模块ID:" << it->second->ID << "   模块注册的消息数量 : " << it->second->GetRegMessageSize() << endl;
  }

  

}




void ModuleManager::reg_ConsoleCMD()
{
   __ConsoleCMD->Register_Console_CMD("minfo",&printModlueInfo,"模块信息");
}
