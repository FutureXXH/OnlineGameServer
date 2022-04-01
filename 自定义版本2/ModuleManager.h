#ifndef __MODULEMANAGER_H
#define __MODULEMANAGER_H
#include "MyTool.h"
#include <unordered_map>
#include <vector>
#include "Module.h"

class ThreadManager;

using namespace std;
class ModuleManager
{
	
private:


	//模块ID -》模组指针
	unordered_map<int32, ModuleBase*> ModuleTable;
	//消息ID -》注册消息的模块ID
	unordered_map<int, vector<int32>> MessageTable;
	//数据队列 
	ThreadSafe_Queue<Message*> dataMessageQueue;
	//需要注册的模块集
	vector<ModuleBase* > ModuleReg;

     //消息对象池
	ThreadSafe_Queue<Message*> MessageObjPool;
	int MessageObjPoolSize = 1024;



	friend class ThreadManager;

	//初始化模块
	void InitModule();


	//注册消息
	bool RegisterMessage(int32 messageID, int32 moduleID);
	//分发数据
	void AssignData();
	//管理器开始执行函数
	void ManagerRun();

public:
     ModuleManager();
    //获取一个消息对象
	Message* GetMessageObj();

	//放回一个消息对象
	bool PushMessageObj(Message* obj);

	//注册模块
	bool RegisterModule(ModuleBase* mp);
	//推送数据到数据队列 会拷贝消息数据 
	bool pushDataMessageQueue(Message* m);
	//推送消息给指定模块队列  会拷贝消息数据 
	bool pushMessageToModule(Message* m, int32 ModuleID);
};



extern ModuleManager* __ModuleManager;

#endif // !__MODULEMANAGER_H

