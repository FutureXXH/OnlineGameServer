#ifndef __TCPSERVER_H
#define __TCPSERVER_H
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
#include "HashArray.h"
#include "IDefine.h"
#include "TCPClient.h"
#include "Task.h"
#include "DBLink.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <windows.h>
#include <unordered_map>
#include <shared_mutex>
#include <mswsock.h>
#include <vector>

using namespace std;

class TcpServer
{
public:
	int threadnum;
	int maxConnect = 1000;
	int maxSocket = 100000;
	atomic<int> CurConnectedNum = 0;
	SOCKET ServerSocket;
	sockaddr_in Server_adr;
	HANDLE CompletionPort;
	mutex TCLinkerMutex;


	HashArray<TcpClientIndex>* TcpClientIndexLinkers;
	HashArray<TcpClient>* TcpClientLinkers;
	vector<SOCKET> ConnectedSockets;

	bool StartTcpServer(int port);
	void IOThread(LPVOID lpParam);


	//关闭socket
	bool CloseSock(SOCKET sock);
	//获取空的TcpClient对象
	TcpClient* GetFreeTcpClient();
	//获取空的索引
	TcpClientIndex* GetClientIndex(int socket);
	//查找TcpClient对象
	TcpClient* FindTcpClient(SOCKET sock);

};

extern TcpServer* __TcpServer;


#endif // !__TCPSERVER_H

