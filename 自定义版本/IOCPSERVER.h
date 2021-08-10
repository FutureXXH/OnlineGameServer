
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


#ifndef _IOCPSERVER_H
#define _IOCPSERVER_H

#include "IDefine.h"
#include <unordered_map>


using namespace std;



class IOCPClient;

static  unordered_map<SOCKET, IOCPClient*>* ClientMap;

class IOCPClient
{
public:
	string ip;
	SOCKET sock;
	sockaddr_in caddr;
	
	int playerid;
	double starttime, endtime;
	LPPER_IO_OPERATION_DATA Data;


	IOCPClient(SOCKET tempsock);
	~IOCPClient();
	void response(int size, DATA data );


	static void initmap(unordered_map<SOCKET, IOCPClient*>* tempClientMap);
	void send10000(DATA &data);
	void send20000(int &size, DATA& data);
	void send20002(int &size, DATA &data);
	void send20001(int& size, DATA& data);

};




class IOCPSERVER
{
public:
	SOCKET serversocket;
	SOCKADDR_IN server_adr;
	HANDLE CompletionPort;
	//IOCPClient iocpclient;
	
	IOCPSERVER();
	

    static void deleteclient(IOCPClient &client);
	bool InitWinsock();
	bool InitWinSERVER(int port);
	void StartIOCPSERVER();

};



#endif