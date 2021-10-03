#ifndef __IDEFINE_H
#define __IDEFINE_H
#include <winsock2.h>

#include <windows.h>
#include<iostream>
#include<atomic>

#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout<<"[INFO]:"
#define SERVERPRINT_WARNING NOWTIME();std::cout<<"[WARNING]:"
#define SERVERPRINT_ERROR NOWTIME();std::cout<<"[ERROR]:"
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

struct PlayerInfo
{
	int ID;
	std::atomic<int> Type = 1; // 0 已断开 1 已连接 2需要保存  3 保存中
	int TCPIndex;
	SOCKET sock;

	//===========自定义玩家数据================


	//===========================

};



#endif // !__IDEFINE_H
