#ifndef __TASK_H
#define __TASK_H
#include <concurrent_queue.h>
#include "IDefine.h"
#include <mutex>
#include <atomic>

using namespace std;
struct Task
{
	int Head;
	int datasize;
	char* data;
	unsigned int Sock;

	void Reset();
};





class TaskManager
{
private:
	//任务队列
	Concurrency::concurrent_queue<Task*> __WorkTasks;
	//空任务池
	Concurrency::concurrent_queue<Task*> TaskPool;
public:
	atomic<int> CurWorkTaskNum = 0;

	//信号量 唤醒线程
	HANDLE semapthore;

	mutex TaskMutex;
	// 最大任务数量
	int MaxTask = 1000;
	//初始化任务管理器
	bool init(int maxtask);
	//推入一个任务进入工作队列
	bool PushTaskToWork(Task* pushTaskP);

	//推出一个工作任务对象
	Task* PopWorkTask();


	//获取一个任务对象
	Task* GetFreeTask();
	//回收任务对象
	bool PushTaskToPool(Task* TaskP);




};

extern TaskManager* __TaskManager;
#endif // !__TASK_H
