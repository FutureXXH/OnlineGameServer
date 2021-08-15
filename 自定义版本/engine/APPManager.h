#pragma once
#ifndef __APPMANAGER_H
#define __APPMANAGER_H

#include "IDefine.h"
#include "TCPSERVER.h"
#include "TaskManager.h"
#include "Player.h"
#include <thread>
#include <concurrent_queue.h>
#include <unordered_map>
#include <shared_mutex>


using namespace std;


//这里对业务层事件进行处理


class APPManager
{
public:

	int SafeCode;
	int threadnum = 0;
	unordered_map<SOCKET, Player*> Playerlist;
	//unordered_map<SOCKET, TCPClient*>* ClientMap;
	TCPSERVER* Ptcpserver;
	HANDLE semapthore;

public:
	bool InitManager( Concurrency::concurrent_queue<task*> * tasks, TCPSERVER* ptcpserver);
	void ProcessService(HANDLE semapthore);
	void Response(task & Task);
	

	void r10000(SOCKET& playersock);

	void r10002(SOCKET& playersock);
	
};



#endif // !__APPMANAGER_H


