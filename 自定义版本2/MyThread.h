#ifndef __MYTHREAD_H
#define __MYTHREAD_H
#include "IDefine.h"
#include "ModuleManager.h"
#include <unordered_map>
#include <chrono>
#include <iostream>
#include "ConsoleCmd.h"


using namespace std;





//一个该类一个线程
class ModuleThread
{
	//线程管理的模块表 模块ID-》模块指针
	unordered_map<int32 , ModuleBase*> ModuleReg;
	
public:
    //模块线程ID
    int ID;
	//线程指针
    thread* myThread;
	
	//开始运行
	void ThreadRun();
	//注册线程运行模块
	bool RegisterModule( ModuleBase* m);
	ModuleThread(int setid) {myThread == nullptr;ID = setid;}
	ModuleThread& operator=(const ModuleThread&) = delete;
	ModuleThread(const ModuleThread&) = delete;
	
};




class ThreadManager
{
private:
	//模块线程数
	int ModuleThreadNum = 0;
	//模块线程容器
	vector<ModuleThread*> Pool;
	//模块管理器线程指针
	thread* ModuleManagerThreadPtr;
    //控制台线程指针
	thread* ConsoleThreadPtr;
	//模块管理器指针
	ModuleManager* ModuleManagerPtr;




public:
	ThreadManager(int SetModuleThreadNum,ModuleManager* setModuleManagerPtr);
	//开始运行线程
	void StartThread();


	ThreadManager& operator=(const ThreadManager&) = delete;
	ThreadManager(const ThreadManager&) = delete;
};









#endif // !__MYTHREAD_H
