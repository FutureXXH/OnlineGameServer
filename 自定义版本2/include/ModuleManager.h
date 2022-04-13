#ifndef __MODULEMANAGER_H
#define __MODULEMANAGER_H
#include "IDefine.h"
#include "MyTool.h"
#include <unordered_map>
#include <vector>
#include "ConsoleCmd.h"
#include <mutex>
#include <unordered_set>

#define REGMASSAGE  -3


using namespace std;

class ThreadManager;




//模块抽象基类
class ModuleBase
{
private:

     //消息队列
	ThreadSafe_Queue<Message*> MessageQueue;
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


	//放回一个消息对象
	bool PushMessageObj(Message* obj);

	//一直循环处理
	virtual void update() = 0;
	//初始化函数 开启服务器时调用
	virtual void Init() = 0;
	//解析消息
	virtual void parseMessage(Message* messagePtr) = 0;

};


//Lua模块类
class LuaModule:public ModuleBase
{
     string luaFileName;
	lua_State * luaPtr;
public:
    LuaModule(int setID,string FileName);
    //初始化Lua虚拟机
    void InitLua();
	//打开Lua文件
	bool OpenLuaFile();
	//重载Lua代码
	bool ReloadLua();
	//发送消息
    static int LuaSendMessage(lua_State *L);
    //注册消息
    static int LuaRegMessage(lua_State *L);
	//一直循环处理
	virtual void update();
	//初始化函数 开启服务器时调用
	virtual void Init() ;
	//解析消息
	virtual void parseMessage(Message* messagePtr);

};



//模块管理类
class ModuleManager
{
	
private:
     mutex regMessageMutex;

	//模块ID -》模组指针
	unordered_map<int32, ModuleBase*> ModuleTable;
	//消息ID -》注册消息的模块ID
	unordered_map<int, unordered_set<int32>> MessageTable;
	//数据队列 
	ThreadSafe_Queue<Message*> dataMessageQueue;
	//需要注册的模块集
	vector<ModuleBase* > ModuleReg;
    //记录模块注册的消息表
	unordered_map<int32,unordered_set<int32>> ModuleRegMessageTable;
     //消息对象池
	ThreadSafe_Queue<Message*> MessageObjPool;
	//消息对象池大小
	int MessageObjPoolSize = 1024;
     
	 friend void reloadLuaModule();
     friend void printModlueInfo();

	friend class ThreadManager;

	//初始化模块
	void InitModule();



	//分发数据
	void AssignData();
	//管理器开始执行函数
	void ManagerRun();

public:
	//注册消息
	bool RegisterMessage(int32 messageID, int32 moduleID);


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

    //生成C++模块
	template<class T>
	 ModuleBase* Generate_Module(int setid);
    //生成Lua模块
	 ModuleBase* Generate_LuaModule(int setid,string FileName);

};

template<class T>
inline ModuleBase* ModuleManager::Generate_Module(int setid)
{


    ModuleBase* p =  new T();

    p->ID = setid;

	RegisterModule(p);
    
    return p;
}

template<>
inline ModuleBase* ModuleManager::Generate_Module<LuaModule>(int setid)
 {

   // Log(ERROR, "没有指定lua模块文件名参数生成失败 ID:" + str(setid));
   return nullptr;
}










extern ModuleManager* __ModuleManager;

#endif // !__MODULEMANAGER_H

