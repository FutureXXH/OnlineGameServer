
#include "ModuleManager.h"
#include "Modules/testModule.h"
#include "Modules/NetModule.h"


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




	ModuleBase* p = new testModule();
	
    ModuleBase* p2 = new NetModule();
    
	ThreadManager* tm = new ThreadManager(2, __ModuleManager);
	tm->StartThread();

	//================================================================


    while (true)
	{
	   string cmd;
	   cin >> cmd;
	   if(cmd == "q")
	   {
		   exit(0);
	   }
	}
	



	
	return 1;
}