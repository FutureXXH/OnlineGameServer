
#ifndef __TASKMANAGER_H
#define __TASKMANAGER_H
#include "TCPClient.h"
#include <concurrent_queue.h>

class task
{
public:
	int Head;
	int DataSize;
	char Data[1024];
	TCPClient* clent;

	
	
};

static Concurrency::concurrent_queue<task*>* __tasks;


#endif // !__TASKMANAGER_H

