
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
	__ModuleManager->RegisterModule(p);
    ModuleBase* p2 = new NetModule();
    __ModuleManager->RegisterModule(p2);
	ThreadManager* tm = new ThreadManager(2, __ModuleManager);
	tm->StartThread();

	//================================================================


	/*int i = 0;
	auto start = system_clock::now();
	while (i < 10000)
	{
		i++;
		Message* temp = new Message(*testm);
		__ModuleManager->pushDataMessageQueue(temp);
		
	}*/


	this_thread::sleep_for(seconds(10000));  

	//cout<< "ִ执行" << i << "次 消耗时间" << duration_cast<milliseconds>(system_clock::now() - start).count() << "毫秒" << endl;

	return 1;
}