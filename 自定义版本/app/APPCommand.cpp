#include "APPCommand.h"

namespace app {

	void OnCommand(Task* task)
	{
		switch (task->Head)
		{
		case 1:

			SERVERPRINT_INFO << "测试ok!" << endl;
			OnTest_1(task);
			break;
		case 10000:
			OnLogin_10000(task);
			break;
		case 10001:
			OnLoginState_10001(task);
			break;
    	case 20000:
			OnRegister_20000(task);
			break;
		case 20001:
			OnRegisterState_20001(task);
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

	void OnLoginState_10001(Task* task)
	{

		SOCKET playersock = 0;
		int PlayerID = -99;
		memcpy(&PlayerID, task->data, 4);
		memcpy(&playersock, task->data + 4, 4);
		if (PlayerID > 0)
		{
			SERVERPRINT_INFO << "登录成功" << "ID" << PlayerID << "Socket" << playersock << std::endl;
		}
		else
		{
			SERVERPRINT_INFO << "登录失败" << "错误代码" << PlayerID << "Socket" << playersock << std::endl;
			
		}
		char backSenndBuff[4];
		memset(backSenndBuff, 0, 4);
		memcpy(backSenndBuff, task->data, 4);


		auto c = __TcpServer->FindTcpClient(playersock);
		c->Send(10001, backSenndBuff, 4, true);
		

	}

	void OnRegister_20000(Task* task)
	{
		char LoginInfoBuff[44];
		memset(LoginInfoBuff, 0, 44);
		memcpy(LoginInfoBuff, task->data, 40);
		memcpy(LoginInfoBuff + 40, (char*)&task->Sock, 4);
		__DBLink->SendDB(2000, 44, LoginInfoBuff);


	}

	void OnRegisterState_20001(Task* task)
	{

		SOCKET playersock = 0;
		int PlayerID = -99;
		memcpy(&PlayerID, task->data, 4);
		memcpy(&playersock, task->data + 4, 4);
		if (PlayerID > 0)
		{
			SERVERPRINT_INFO << "注册成功" << "ID" << PlayerID << "Socket" << playersock << std::endl;
		}
		else
		{
			SERVERPRINT_INFO << "注册失败" << "错误代码" << PlayerID << "Socket" << playersock << std::endl;

		}
		char backSenndBuff[4];
		memset(backSenndBuff, 0, 4);
		memcpy(backSenndBuff, task->data, 4);


		auto c = __TcpServer->FindTcpClient(playersock);
		c->Send(20001, backSenndBuff, 4, true);


	}




	void OnTest_1(Task* task)
	{
		char buff[20];
		memset(buff, 0, 20);
		memcpy(buff, "Test Success", 12);

		
		auto p = __TcpServer->FindTcpClient(task->Sock);
		if (p != nullptr)
		{
			p->Send(1, buff, 20, true);
		}



	}




}


