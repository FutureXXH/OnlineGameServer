

#include "IOCPSERVER.h"


int main()
{
	IOCPSERVER* server = new IOCPSERVER();
	server->InitWinsock();
	server->InitWinSERVER(6666);
	server->StartIOCPSERVER();




}