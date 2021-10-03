#ifndef __WORKTHREAD_H
#define __WORKTHREAD_H
#include "IDefine.h"
#include "Task.h"
#include "..\app\APPCommand.h"

class WorkThread
{
public:
	int ThreadID;
	int state; // 0 空闲 1繁忙

	HANDLE semapthore;

	WorkThread(int id);
	void Run();
};


class WorkThreadManager
{
public:
	int WorkThreadNum;
	//初始化并开启工作线程
	void InitWorkThread(int ThreadNum);
};




#endif // !__WORKTHREAD_H

