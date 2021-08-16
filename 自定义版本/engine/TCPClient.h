
#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#include <time.h>
#include "IDefine.h"
#include <string>
using namespace std;

//这里对网络层连接数据进行定义


class TCPClient
{
public:
	string ip;
	SOCKET sock;
	sockaddr_in caddr;
	const int bufsize = 1024;
	char cbuff[1024];
	


	TCPClient(SOCKET Socket, sockaddr_in addr);
	~TCPClient();
	int send(const int head, const char* data, const int DataSize, const int SafeCode);

};


#endif // !__TCPCLIENT_H

