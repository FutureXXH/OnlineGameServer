


#ifndef __DEFINE_H
#define __DEFINE_H

#include <iostream>
#include <winsock2.h>

#include <windows.h>
#include<iostream>
#include <thread>
#include <mutex>
#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout<<"[INFO]:"
#define DATA_BUFSIZE sizeof(DATA)

#define ECHO 10001
#define HEART 10000
#define CLOSE_CONNECT 20000
#define LOCATION 20001
#define SEND_MESSAGE 20002

struct DATA
{
	int head;
	char buf[1024];
	int bufsize;
	int state;
	int ID;


};

typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	DATA data;
}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;






#endif