#include "APPCommand.h"

namespace app {

	void OnCommand(Task* task)
	{
		switch (task->Head)
		{
		case 1:
			SERVERPRINT_INFO << "测试ok!" << endl;
			break;
		case 10000:
			OnLogin_10000(task);
			break;
		case 10001:
			OnLoginSuccess_10001(task);
			break;



		default:
			break;
		}


	}

	void OnLogin_10000(Task* task)
	{
		char LoginInfoBuff[44];
		memset(LoginInfoBuff, 0, 44);
		memcpy(LoginInfoBuff, task->data, 40);
		memcpy(LoginInfoBuff+40, (char*)&task->Sock, 4);

		__DBLink->SendDB(1000, 44, LoginInfoBuff);

	}

	void OnLoginSuccess_10001(Task* task)
	{
		SOCKET playersock = 0;
		int PlayerID = -1;
		memcpy(&PlayerID, task->data, 4);
		memcpy(&playersock, task->data + 4, 4);
		SERVERPRINT_INFO << "登录成功" << "ID" << PlayerID  << "Socket" << playersock<< std::endl;

	}







}


