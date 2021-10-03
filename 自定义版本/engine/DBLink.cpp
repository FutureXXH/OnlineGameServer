#include "DBLink.h"


DBLink* __DBLink = nullptr;

void DBLink::init(const char* setip, int setport, HANDLE CompletionPortHandle)
{
	if (strlen(setip) > 20)
	{
		SERVERPRINT_ERROR << "DB服务器IP初始化异常" << endl;
		return;
	}

	memset(DBip, 0, 20);
	memcpy(DBip, setip, strlen(setip));
	port = setport;
	CompletionPort = CompletionPortHandle;

	SERVERPRINT_INFO << "DB服务器设置初始化成功" << endl;
}

bool DBLink::ConnectDB()
{

    DBsock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


    memset(&DBServer_adr, 0, sizeof(DBServer_adr));
    DBServer_adr.sin_family = AF_INET;
    DBServer_adr.sin_addr.S_un.S_addr = inet_addr(DBip);;
    DBServer_adr.sin_port = htons(port);

	SERVERPRINT_INFO << "正在连接DB服务器" << endl;
	if (connect(DBsock, (SOCKADDR*)&DBServer_adr, sizeof(DBServer_adr)) == SOCKET_ERROR)
	{
		SERVERPRINT_ERROR << "连接DB服务器失败" << endl;
		return false;
	}



	if (CompletionPort == NULL)
     {
			SERVERPRINT_ERROR << "DBLink 完成端口对象为null" << endl;
	 }
	//把DB服务器sock加入进监听里
	CreateIoCompletionPort((HANDLE)DBsock, CompletionPort, (DWORD)DBsock, 0);
	LPPER_IO_OPERATION_DATA PerIoData = new PER_IO_OPERATION_DATA();
	ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
	PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
	PerIoData->DataBuf.len = DATA_BUFSIZE;
	PerIoData->Type = 3;
	DWORD Flags = 0;
	DWORD dwRecv = 0;
	WSARecv(DBsock, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);


	SERVERPRINT_INFO << "已连接DB服务器" << endl;
    return true;
}

bool DBLink::SendDB(int head, int size, char* buff)
{

	int sendsize = size + 8;


	//发送数据

	LPPER_IO_OPERATION_DATA PerIoData = new PER_IO_OPERATION_DATA();
	ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
	PerIoData->DataBuf.buf = (char*)&(PerIoData->data);

	memset(PerIoData->data, 0, sendsize);

	memcpy(PerIoData->data, (char*)&head, 4);
	memcpy(PerIoData->data + 4, (char*)&size, 4);
	memcpy(PerIoData->data + 8, buff, size);


	PerIoData->DataBuf.len = sendsize;
	PerIoData->sock = this->DBsock;
	PerIoData->Type = 2;
	DWORD Flags = 0;
	DWORD sendedsize = 0;
	WSASend(PerIoData->sock, &PerIoData->DataBuf, 1, &sendedsize, Flags, &PerIoData->Overlapped, NULL);






	return true;
}
