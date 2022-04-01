#ifndef __MODULE_H
#define __MODULE_H
#include "MyTool.h"
#include <vector>

using namespace std;

class ModuleManager;

//模块抽象基类
class ModuleBase
{
private:


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

	//放回一个消息对象
	bool PushMessageObj(Message* obj);

	//一直循环处理
	virtual void update() = 0;
	//初始化函数 开启服务器时调用
	virtual void Init() = 0;
	//解析消息
	virtual void parseMessage(Message* messagePtr) = 0;

};




#endif // !__MODULE_H

