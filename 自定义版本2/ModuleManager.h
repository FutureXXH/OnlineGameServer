#ifndef __MODULEMANAGER_H
#define __MODULEMANAGER_H
#include "MyTool.h"
#include <unordered_map>
#include <vector>
#include "ConsoleCmd.h"


using namespace std;

class ThreadManager;

//模块抽象基类
class ModuleBase
{
private:

     //消息队列
	ThreadSafe_Queue<Message*> MessageQueue;
	//需要注册的消息  
	vector<int32> MessageReg;
     //消息对象池
	ThreadSafe_Queue<Message*> MessageObjPool;
	int MessageObjPoolSize = 1024;

public:
	ModuleBase();


	friend class ModuleManager;
	
	
	//模块最大队列消息数量
	uint32 MaxQueueSize = 100000;
	//模块ID
	int32 ID = -1;
	//处理队列
	void parseQueue();
	//获取队列大小
	uint32 getQueueSize();
	//消息入队 （会进行消息拷贝 没有使用原消息数据）
	bool pushMessage(Message* m);
	//注册消息
	bool RegisterMessageID(int32 ID);

    //获取一个消息对象
	Message* GetMessageObj();
    //获取已注册消息的数量
	uint32 GetRegMessageSize();

	//放回一个消息对象
	bool PushMessageObj(Message* obj);

	//一直循环处理
	virtual void update() = 0;
	//初始化函数 开启服务器时调用
	virtual void Init() = 0;
	//解析消息
	virtual void parseMessage(Message* messagePtr) = 0;

};


//模块管理类
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
     
     friend void printModlueInfo();

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
	//===========================================
    //注册控制台指令
	void reg_ConsoleCMD();
	//===========================================
};



extern ModuleManager* __ModuleManager;

#endif // !__MODULEMANAGER_H

