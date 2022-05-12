#ifndef __IDEFINE_H
#define __IDEFINE_H
#include <string.h>
#include <thread>

extern "C"
{
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
}

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


#define DataBufferSize 1024
//消息数据定义
class Message
{
private:
   int Data_head = 0;
   int Data_Tail = 0;

public:
    int32 aimModuleID;//目标模块ID 如果为-1 就转发给所有
	int32 srcModuleID;
	int32 MessageID;
	uint32 dataSize;
	char data[DataBufferSize] ;



	~Message()
	{

	}
	Message()
	{

	}
	bool reset()
	{
		aimModuleID= -1;
		MessageID = -1;
		dataSize = 0;
		srcModuleID = -1;
		Data_head = 0;
		Data_Tail = 0;
		memset(data,0,1024);
		return true;
	}

	Message(const Message& m)
	{
		aimModuleID = m.aimModuleID;
		MessageID = m.MessageID;
		dataSize = m.dataSize;
		srcModuleID = m.srcModuleID;
		memcpy(data, m.data, dataSize);
	}
	Message& operator=(const Message& m) 
	{
		aimModuleID = m.aimModuleID;
		MessageID = m.MessageID;
		dataSize = m.dataSize;
		srcModuleID = m.srcModuleID;
		memcpy(data, m.data, dataSize);
		return *this;
	}
    template<class T>
	bool writeData(const T& writedata);

	bool writeData(const char *buffer,int size);

	template<class T>
	bool readData( T& writedata);

	bool readData(char *buffer,int size);
};



template<class T>
inline bool Message::writeData(const T& writedata)
{
	if(Data_Tail+sizeof(writedata) >= DataBufferSize)return false;
	
    memcpy(data+Data_Tail,(char*)&writedata,sizeof(writedata));
    Data_Tail += sizeof(writedata);
	dataSize += sizeof(writedata);
	return true;
}

inline bool Message::writeData(const char *buffer,int size)
{
	if(Data_Tail+size >= DataBufferSize)return false;
	
    memcpy(data+Data_Tail,buffer,size);
    Data_Tail += size;
	dataSize += size;
	return true;
}


template<class T>
inline bool Message::readData( T& writedata)
{
	if(Data_head+sizeof(writedata) > dataSize)return false;
	
    memcpy(&writedata,data+Data_head,sizeof(writedata));
    Data_head += sizeof(writedata);
	return true;
}


inline bool Message::readData(char *buffer,int size)
{
	if(Data_head+size > dataSize)return false;
	
    memcpy(buffer,data+Data_head,size);
    Data_head += size;
	return true;
}




#endif