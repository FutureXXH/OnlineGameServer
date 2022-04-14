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

public:
    //模块线程ID
    int ID;
	//线程指针
    thread* myThread;
	
	//开始运行
	void ThreadRun();

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




public:
	ThreadManager(int SetModuleThreadNum);
	//开始运行线程
	void StartThread();


	ThreadManager& operator=(const ThreadManager&) = delete;
	ThreadManager(const ThreadManager&) = delete;
};


extern ThreadManager* __ThreadManager;






#endif // !__MYTHREAD_H
