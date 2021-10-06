#ifndef __APPCOMMAND_H
#define __APPCOMMAND_H
#include "..\engine\Task.h"
#include "..\engine\IDefine.h"
#include "..\engine\DBLink.h"
#include "..\engine\TCPServer.h"
namespace app {



	extern void OnCommand(Task* task);


	extern void OnTest_1(Task* task);
	extern void OnLogin_10000(Task* task);
	extern void OnLoginState_10001(Task* task);
	extern void OnLoginFailed_10002(Task* task);



}


#endif // !__APPCOMMAND_H

