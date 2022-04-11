#include "ModuleManager.h"



ModuleBase::ModuleBase()
{
	for (int i = 0; i < MessageObjPoolSize; i++)
	{
		MessageObjPool.push(new Message());
	}

   __ModuleManager->RegisterModule(this);

	
}

void ModuleBase::parseQueue()
{
	Message* p = nullptr;
	p = MessageQueue.pop();
	while (p != nullptr)
	{
		parseMessage(p);
        
		//放回对象池中
	    PushMessageObj(p);
		p = MessageQueue.pop();
	}

}

uint32 ModuleBase::getQueueSize()
{
	return MessageQueue.size();
}

bool ModuleBase::pushMessage(Message* m)
{
	if (MessageQueue.size() >= MaxQueueSize)return false;
    //从对象池取出
    Message* cpm  = GetMessageObj();
	*cpm = *m;
	MessageQueue.push(cpm);
	return true;
}

bool ModuleBase::RegisterMessageID(int32 ID)
{
	MessageReg.emplace_back(ID);

	return true;
}

Message* ModuleBase::GetMessageObj()
{
    if(MessageObjPool.size() == 0)return nullptr;
	auto p = MessageObjPool.pop();
	p->reset();
	return p;
}


bool ModuleBase::PushMessageObj(Message* obj)
{
     MessageObjPool.push(obj);
	return true;
}

uint32 ModuleBase::GetRegMessageSize()
{
  return MessageReg.size();

}