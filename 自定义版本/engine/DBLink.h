#ifndef __DBLINK_H
#define __DBLINK_H
#include "IDefine.h"
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
using namespace std;

//DB服务器连接器
class DBLink
{
public:
	char DBip[20];
	int port;
	SOCKET DBsock;
	sockaddr_in DBServer_adr;
	HANDLE CompletionPort = NULL;

	void init(const char* ip, int port, HANDLE CompletionPortHandle);
	bool ConnectDB();
	bool SendDB(int head, int size, char* buff);
	



};

extern DBLink* __DBLink;
#endif // !__DBLINK_H
