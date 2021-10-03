#include "TCPClient.h"

TcpClient::~TcpClient()
{
	SERVERPRINT_INFO << "正在关闭socket" << sock << std::endl;
	closesocket(sock);

}

bool TcpClient::Reset()
{
	ip.clear();
	port = 0;
	sock = INVALID_SOCKET;
	memset(&caddr, 0, sizeof(caddr));
	state = 0;
	ID = -1;



	return false;
}

int TcpClient::Send(int head, char* buff, int buffsize,bool IsEncrypt)
{

	int sendsize = buffsize + 8;


	//发送数据

	LPPER_IO_OPERATION_DATA PerIoData = new PER_IO_OPERATION_DATA();
	ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
	PerIoData->DataBuf.buf = (char*)&(PerIoData->data);

	memset(PerIoData->data, 0, 8 + buffsize);
	memcpy(PerIoData->data, (char*)&head, 4);
	memcpy(PerIoData->data + 4, (char*)&buffsize, 4);
	memcpy(PerIoData->data + 8, buff, buffsize);
	//进行简单加密
	if (IsEncrypt)
	{
		for (int i = 0; i < 8 + buffsize; i++)
		{
			PerIoData->data[i] ^= SafeCode;
		}
	}



	PerIoData->DataBuf.len = sendsize;
	PerIoData->sock = this->sock;
	PerIoData->Type = 2;
	DWORD Flags = 0;
	DWORD sendedsize = 0;
	WSASend(PerIoData->sock, &PerIoData->DataBuf, 1, &sendedsize, Flags, &PerIoData->Overlapped, NULL);


	return sendedsize;


}
