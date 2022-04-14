
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
    __ModuleManager = new ModuleManager();
	__ThreadManager = new ThreadManager(ThreadNum);
}










int main()
{
     ios::sync_with_stdio(false);
	 cout.tie(0);
     InitServer(2);
	  



	 //===============================================================
     //C++模块 网络模块
	 ModuleBase* NetM = __ModuleManager->Generate_Module<NetModule>(1000);
	//Lua模块
    ModuleBase* luaM1 = __ModuleManager->Generate_LuaModule(2000,"lua1.lua");
    ModuleBase* luaM2 = __ModuleManager->Generate_LuaModule(3000,"lua2.lua");


    
	ThreadManager* tm = new ThreadManager(2);
	tm->StartThread();

	//================================================================


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