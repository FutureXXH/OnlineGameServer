#ifndef __APPCOMMAND_H
#define __APPCOMMAND_H
#include "..\engine\Task.h"
#include "..\engine\IDefine.h"
#include "..\engine\DBLink.h"
namespace app {



	extern void OnCommand(Task* task);



	extern void OnLogin_10000(Task* task);
	extern void OnLoginSuccess_10001(Task* task);



}


#endif // !__APPCOMMAND_H

