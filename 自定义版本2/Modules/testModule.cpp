#include "testModule.h"

void testModule::update()
{
	
}

void testModule::Init()
{

	//注册消息 id为1
    RegisterMessageID(1);
	 Log(INFO,"初始化模块"+ str(ID));
}

void testModule::parseMessage(Message* messagePtr)
{
	if(messagePtr->MessageID == 1)
	{
		string s = messagePtr->data;
		 Log(INFO,"测试模块收到消息"+ s);
	}
	

}
