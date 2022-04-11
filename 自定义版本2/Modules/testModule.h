#ifndef __TESTMODULE_H
#define __TESTMODULE_H



#include "../ModuleManager.h"
#include "../IDefine.h"
#include <iostream>

using namespace std;
class testModule :public ModuleBase
{

public:
	int a = 0;
	virtual void update() override;
	virtual void Init() override;
	virtual void parseMessage(Message* messagePtr) override;
};


#endif // !__TESTMODULE_H