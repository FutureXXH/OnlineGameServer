

#include "engine/TCPSERVER.h"
#include "share/ShareFunction.h"
#include "engine/TaskManager.h"
#include "engine/APPManager.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include "engine/HashArray.h"


using namespace std;

int main()
{
	char FileExePath[128];
	memset(FileExePath, 0, 128);

	//获取当前exe路径 
	GetModuleFileNameA(NULL, (LPSTR)FileExePath, 128);
	std::string strPath(FileExePath);
	size_t pos = strPath.find_last_of('\\'); //除去最后的exe文件名 获取当前文件目录
	strPath = strPath.substr(0, pos + 1);
	memcpy(FileExePath, strPath.c_str(), 128);
	TCPSERVER* server = new TCPSERVER();
	APPManager* appmanager = new APPManager();


	share::LoadServerXML("server_config.xml", FileExePath, server->port,server->SafeCode, server->threadnum,appmanager->threadnum);
	SERVERPRINT_INFO << endl 
		             << "==========================" << endl
		             << "配置文件读取结果："
		             << "端口:" << server->port << "安全码:" << server->SafeCode << endl
	                 << "==========================" << endl;

	__tasks = new Concurrency::concurrent_queue<task*>();//任务队列 

	appmanager->InitManager( __tasks);

	server->InitWinsock();
	server->InitWinSERVER(server->port);
	server->StartIOCPSERVER(__tasks);

	
	
}