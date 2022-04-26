#include "../include/ModuleManager.h"

#include <iostream>
ModuleManager* __ModuleManager = nullptr;




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
    ModuleTable.emplace(mp->ID,mp);
    ThreadModuleQueue.push(mp);
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

        //给模块管理器的消息 ID 0
        //模块删除是单线程的，所以不用担心模块删除造成悬空指针问题
        if(m->MessageID == 0)
        {
              parseSelfMessage(m);
        }
        else if (MessageTable.count(m->MessageID) != 0)
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


void ModuleManager::parseSelfMessage(Message* m)
{
    switch (m->srcModuleID)
    {
    case LoadLuaModule:
    {
           int ModuleID = -1;
           memcpy(&ModuleID,m->data,4);
           string filename;
           filename.assign(m->data+4,m->dataSize-4);
           auto p =Generate_LuaModule(ModuleID,filename);
    }
    break;
    case ReLoadLuaModule:
       {
           int relaodID = -1;
           memcpy(&relaodID,m->data,4);
           reloadLuaModule(relaodID);
       }
    break;
    case DeleteModule:
   {
    int relaodID = -1;
    memcpy(&relaodID,m->data,4);
    deleteModule(relaodID);
         
   }
    break;
    
    default:
        break;
    }
}







void ModuleManager::ManagerRun()
{

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

bool ModuleManager::reloadLuaModule(int32 moduleID)
{


   if(ModuleTable.count(moduleID) == 0)
   {
       Log(ERROR,"没有这个模块");
       return false;
   }
   auto p =  dynamic_cast<LuaModule*>(ModuleTable[moduleID]);
   if(p == nullptr)
   {
       Log(ERROR,"重启模块异常");
      return false;
   }
   p->ReloadLua();
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


void ReloadLuaModule()
{
    Log(INFO,"输入模块ID");
    int ID;
    cin >> ID;

    auto m = __ModuleManager->GetMessageObj();
    if(m == nullptr)return;

    m->MessageID= 0;
    m->dataSize = 4;
    memcpy(m->data,(char*)&ID,4);
    m->srcModuleID = ReLoadLuaModule;

    __ModuleManager->pushDataMessageQueue(m);

}


void delete_Module()
{
    Log(INFO,"输入模块ID");
    int ID;
    cin >> ID;

    auto m = __ModuleManager->GetMessageObj();
    if(m == nullptr)return;

    m->MessageID= 0;
    m->dataSize = 4;
    memcpy(m->data,(char*)&ID,4);
    m->srcModuleID = DeleteModule;

    __ModuleManager->pushDataMessageQueue(m);

}


void load_Module()
{
    Log(INFO,"输入新创建的模块ID");
    int ID;
    cin >> ID;
    Log(INFO,"输入新创建的lua模块文件名");
    string filename;
    cin >> filename;


    auto m = __ModuleManager->GetMessageObj();
    if(m == nullptr)return;

    m->MessageID= 0;
    m->dataSize = filename.length()+4;

    memcpy(m->data,(char*)&ID,4);
    memcpy(m->data+4,filename.c_str(),filename.length());
    m->srcModuleID = LoadLuaModule;

    __ModuleManager->pushDataMessageQueue(m);

}



void ModuleManager::reg_ConsoleCMD()
{
   __ConsoleCMD->Register_Console_CMD("minfo",&printModlueInfo,"模块信息");
   __ConsoleCMD->Register_Console_CMD("reloadl",&ReloadLuaModule,"重载Lua模块");
   __ConsoleCMD->Register_Console_CMD("deletem",&delete_Module,"删除模块");
    __ConsoleCMD->Register_Console_CMD("loadm",&load_Module,"加载模块");
}


bool ModuleManager::deleteModule(int id)
{
   if(ModuleTable.count(id) == 0)
   {
       Log(ERROR,"没有找到该模块");
       return false;
   }

    auto p = ModuleTable[id];

    //删除模块注册的消息
    for (auto it = ModuleRegMessageTable[id].begin(); it !=  ModuleRegMessageTable[id].end(); it++)
    {
        if(MessageTable[*it].count(id) > 0)
        {
            MessageTable[*it].erase(id);
        }
    }
    ModuleRegMessageTable.erase(id);
    //删除模块的记录
    ModuleTable.erase(id);
    //更新模块状态
    p->ModuleState = MODULE_CLOSING;



}
