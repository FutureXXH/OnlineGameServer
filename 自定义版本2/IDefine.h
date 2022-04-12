#ifndef __IDEFINE_H
#define __IDEFINE_H
#include <string.h>
#include <thread>
#define _GLIBCXX_HAS_GTHREADS
#define _GLIBCXX_USE_C99_STDINT_TR1


using int64 = long long;
using int32 = int;
using int16 = short;
using int8 = char;
using uint64 = unsigned long long;
using uint32 = unsigned int;
using uint16 = unsigned short;
using uint8 = unsigned char;

#define str(x) to_string(x) 

//消息数据定义
struct Message
{
	int32 srcModuleID;
	int32 MessageID;
	uint32 dataSize;
	char data[1024] ;



	~Message()
	{

	}
	Message()
	{

	}
	bool reset()
	{
		MessageID = -1;
		dataSize = 0;
		srcModuleID = -1;
		memset(data,0,1024);
		return true;
	}

	Message(const Message& m)
	{
		MessageID = m.MessageID;
		dataSize = m.dataSize;
		srcModuleID = m.srcModuleID;
		memcpy(data, m.data, dataSize);
	}
	Message& operator=(const Message& m) 
	{
		MessageID = m.MessageID;
		dataSize = m.dataSize;
		srcModuleID = m.srcModuleID;
		memcpy(data, m.data, dataSize);
		return *this;
	}
};









#endif