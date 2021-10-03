#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H
#include "IDefine.h"
#include <string>
#include <atomic>

using namespace std;

struct TcpClientIndex
{
	int index;
	void Reset() { index = -1; };
};


class TcpClient
{
public:
	string ip;
	SOCKET sock;
	sockaddr_in caddr;
	int ID;
	int port;
	double HearTime;
	atomic<int> state;  //状态 0 未连接   1 已连接

	~TcpClient();
	bool Reset();
	int Send(int head,char * buff,int buffsize, bool IsEncrypt);
};


#endif // !__TCPCLIENT_H

