#include "TCPClient.h"


TCPClient::TCPClient(SOCKET Socket, sockaddr_in addr)
{
	
	sock = Socket;
	caddr = addr;
	ip = inet_ntoa(caddr.sin_addr);
	memset(cbuff, 0, bufsize);

	//starttime = clock() / CLOCKS_PER_SEC;
	//endtime = clock() / CLOCKS_PER_SEC;

}

TCPClient::~TCPClient()
{
	SERVERPRINT_INFO << "已删除玩家连接数据" << std::endl;
}

int TCPClient::Send(const int head, const char* data, const int DataSize, const int SafeCode)
{
	
	char* sendbuf = new char[8 + DataSize];
	memset(sendbuf, 0, 8 + DataSize);
	memcpy(sendbuf, (char*)&head, 4);
	memcpy(sendbuf+4, (char*)&DataSize, 4);
	memcpy(sendbuf + 8, data, DataSize);
	//进行简单加密
	for (int i = 0; i < 8+ DataSize; i++)
	{
		sendbuf[i] ^= SafeCode;
	}

	//发送数据
	int sendedsize = 0;
	int sendsize = DataSize+8;
	while (sendedsize < sendsize)
	{

		int len = send(sock, sendbuf+ sendedsize, sendsize - sendedsize, 0);
		if (len <= 0)
		{
			break;
		}
		sendedsize += len;
	}

	return sendedsize;
}
