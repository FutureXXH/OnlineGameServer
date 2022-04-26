
#include "../include/IDefine.h"
#include "../include/ModuleManager.h"
#include "../include/ConsoleCmd.h"
#include "../include/MyThread.h"
#include "../include/NetModule.h"




#include <thread>
#include <iostream>
#include <chrono>   
using namespace std;
using namespace chrono;

void InitServer(int ThreadNum)
{
    __ConsoleCMD = new ConsoleCMD();
    __ConsoleLog = new ConsoleLog();
    __ModuleManager = new ModuleManager();
	__ThreadManager = new ThreadManager(ThreadNum);

	Log = bind(&ConsoleLog::push_ConsoleLog,__ConsoleLog,placeholders::_1,placeholders::_2);
	__ThreadManager->ConsoleThreadPtr = new thread(&ConsoleLog::ConsoleThreadRun,__ConsoleLog);
	this_thread::sleep_for(chrono::seconds(1));
  
}










int main()
{
     ios::sync_with_stdio(false);
	 cout.tie(0);
     InitServer(2);
	  


	 //===============================================================

	//Lua 模块
    ModuleBase* luaM1 = __ModuleManager->Generate_LuaModule(1,"LuaStart.lua");

     //C++模块 网络模块
	 ModuleBase* NetM = __ModuleManager->Generate_CModule<NetModule>(1000);

 


    


	//================================================================

	__ThreadManager->StartThread();

    while (true)
	{
	   string cmd;
	   cin >> cmd;
	   if(!__ConsoleCMD->parseConsoleCMD(cmd))
	   {
		   Log(INFO,"未知指令 输入help查看指令列表");
	   }
	}
	



	
	return 1;
}