#ifndef __MyTool_H
#define __MyTool_H
#include <queue>
#include <mutex>
#include <atomic>
#include "IDefine.h"

using namespace std;


//线程安全队列
template<class T>
class ThreadSafe_Queue
{
private:
    atomic_flag SLock;//实现自旋锁
   
	std::queue<T> q;
	std::mutex mlock;
public:
	//进队尾
	bool push(T obj);
	//队首出队
	T pop();
	//当前队列大小
	uint32 size();
	 uint32 CurSize = 0;
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
	//while(SLock.test_and_set())
	
	q.push(obj);
	CurSize++;
    //SLock.clear();
   return true;
}

template<class T>
 T ThreadSafe_Queue<T>::pop()
{
	std::lock_guard<std::mutex> lock(mlock);
  	//while(SLock.test_and_set())
      //Lock = true;
	

	if (CurSize == 0)return nullptr;
	T p = q.front();
	q.pop();
    CurSize--;
	 //SLock.clear();
	return p;
}



template<class T>
 uint32 ThreadSafe_Queue<T>::size()
{
     //while(Lock.test_and_set())
	//Lock.clear();
	return (uint32)q.size();
	
}




#endif // !MyTool_H


