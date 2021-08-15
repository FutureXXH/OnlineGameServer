


#ifndef __DEFINE_H
#define __DEFINE_H

#include <winsock2.h>

#include <windows.h>
#include<iostream>

#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout<<"[INFO]:"
#define SERVERPRINT_WARNING NOWTIME();std::cout<<"[WARNING]:"
#define SERVERPRINT_ERROR NOWTIME();std::cout<<"[ERROR]:"
#define DATA_BUFSIZE 1024

#define TEST 10001
#define HEART 10000
#define NEWPLAYER 10002
#define CLOSE_CONNECT 20000
#define LOCATION 20001
#define SEND_MESSAGE 20002




typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	char data[1024];
	SOCKET sock;
}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;






#endif