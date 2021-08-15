#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#ifndef __TCPSERVER_H
#define __TCPSERVER_H

#include "IDefine.h"
#include"TCPClient.h"
#include "TaskManager.h"
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <windows.h>
#include <unordered_map>
#include <shared_mutex>
#include <mswsock.h>


//static unordered_map<SOCKET, TCPClient*>* ClientMap = new unordered_map<SOCKET, TCPClient*>();

using namespace std;
class TCPSERVER
{
public:
	 int port;
     int SafeCode;
	 int threadnum = 0;
	SOCKET ServerSocket;
	sockaddr_in Server_adr;
	HANDLE CompletionPort;

	unordered_map<SOCKET, TCPClient*>* ClientMap = new unordered_map<SOCKET, TCPClient*>();
public:
	TCPSERVER();
	~TCPSERVER();


	bool DeleteClientSocket(SOCKET ClientSocket);
	bool InitWinsock();
	bool InitWinSERVER(int port);
	void StartIOCPSERVER(Concurrency::concurrent_queue<task*>* tasks);
	void ProcessIO(LPVOID lpParam);

};



#endif // !__TCPSERVER_H



