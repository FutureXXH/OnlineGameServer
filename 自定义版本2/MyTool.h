#ifndef __MyTool_H
#define __MyTool_H
#include <queue>
#include <mutex>
#include "IDefine.h"

using namespace std;


//线程安全队列
template<class T>
class ThreadSafe_Queue
{
private:
	std::queue<T> q;
	std::mutex mlock;
public:
	//进队尾
	bool push(T obj);
	//队首出队
	T pop();
	//当前队列大小
	uint32 size();
};


class Timer
{
private:
	
public:
	void update();

};









template<class T>
 bool ThreadSafe_Queue<T>::push(T obj)
{
	std::lock_guard<std::mutex> lock(mlock);
	q.push(obj);
   return true;
}

template<class T>
 T ThreadSafe_Queue<T>::pop()
{
	std::lock_guard<std::mutex> lock(mlock);

	if (q.size() == 0)return nullptr;
	T p = q.front();
	q.pop();
	return p;
}



template<class T>
 uint32 ThreadSafe_Queue<T>::size()
{
	std::lock_guard<std::mutex> lock(mlock);
	return (uint32)q.size();
}




#endif // !MyTool_H


