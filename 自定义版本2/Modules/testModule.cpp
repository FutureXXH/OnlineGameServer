#include "testModule.h"

void testModule::update()
{
	
}

void testModule::Init()
{

	//注册消息 id为1
    RegisterMessageID(1);
	cout << "初始化模块" << ID	<< endl;
}

void testModule::parseMessage(Message* messagePtr)
{
	if(messagePtr->MessageID == 1)
	{
		cout << "测试模块收到消息:" << messagePtr->data << endl;
	}
	

}
