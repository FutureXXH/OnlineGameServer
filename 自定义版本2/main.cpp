
#include "ModuleManager.h"
#include "Modules/testModule.h"
#include "Modules/NetModule.h"
#include "ConsoleCmd.h"

#include "MyThread.h"
#include <thread>
#include <iostream>
#include <chrono>   
using namespace std;
using namespace chrono;


int main()
{
     ios::sync_with_stdio(false);
	 cout.tie(0);
	 //===============================================================




	ModuleBase* p = __ModuleManager->Generate_Module<testModule>(1000);
	
    ModuleBase* p2 = __ModuleManager->Generate_Module<NetModule>(2000);
    
	ThreadManager* tm = new ThreadManager(2, __ModuleManager);
	tm->StartThread();

	//================================================================


    while (true)
	{
	   string cmd;
	   cin >> cmd;
	   if(!__ConsoleCMD->parseConsoleCMD(cmd))
	   {
		   cout << "未知指令 输入help查看指令列表" << endl;
	   }
	}
	



	
	return 1;
}