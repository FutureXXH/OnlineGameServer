#ifndef __IDEFINE_H
#define __IDEFINE_H


#include <winsock2.h>

#include <windows.h>
#include "..\share\ConsoleColor.h"
#include<iostream>
#include<atomic>

#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout << green <<"[INFO]" << white << ":"
#define SERVERPRINT_WARNING NOWTIME();std::cout<< yellow <<"[WARNING]" << white << ":"
#define SERVERPRINT_ERROR NOWTIME();std::cout  << red<<"[ERROR]" << white << ":"
#define DATA_BUFSIZE 1024


extern int SafeCode;


typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	char data[1024];
	SOCKET sock;
	int Type;  // 1 接收 2 发送   3 内网数据接收 

}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;





#endif // !__IDEFINE_H
